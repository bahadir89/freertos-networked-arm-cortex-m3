/****************************************************************************************
|  Description: Standard I/O integer optimized library source file
|               The following functions are implemented:
|                 <stdio.h>
|                   - int printf (const char *format, ...)
|                   - int scanf(const char *fmt, ...)
|    File Name: stdio_mini.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work
| that includes OpenBLT without being obliged to provide the source code for any
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
|
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "../uart.h"


/****************************************************************************************
* Structure definitions
****************************************************************************************/
struct printf_conversion /* A printf() conversion. */
{
  /* Flags. */
  enum
  {
    MINUS = 1 << 0,         /* '-' */
    PLUS = 1 << 1,          /* '+' */
    SPACE = 1 << 2,         /* ' ' */
    POUND = 1 << 3,         /* '#' */
    ZERO = 1 << 4,          /* '0' */
    GROUP = 1 << 5          /* '\'' */
  }
  flags;

  /* Minimum field width. */
  int width;

  /* Numeric precision. -1 indicates no precision was specified. */
  int precision;

  /* Type of argument to format. */
  enum
  {
    CHAR = 1,               /* hh */
    SHORT = 2,              /* h */
    INT = 3,                /* (none) */
    LONG = 5,               /* l */
  }
  type;
};


struct integer_base
{
  int base;                   /* Base. */
  const char *digits;         /* Collection of digits. */
  int x;                      /* `x' character to use, for base 16 only. */
  int group;                  /* Number of digits to group with ' flag. */
};


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void output_dup(char ch, size_t cnt);
static void format_integer(unsigned long value, char is_signed, char negative,
                           const struct integer_base *b,
                           const struct printf_conversion *c);
static const char *parse_conversion(const char *format, struct printf_conversion *c,
                                    va_list *args);
static int libvprintf(const char *format, va_list args);


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
static const struct integer_base base_d = {10, "0123456789", 0, 3};
static const struct integer_base base_o = {8, "01234567", 0, 3};
static const struct integer_base base_x = {16, "0123456789abcdef", 'x', 4};
static const struct integer_base base_X = {16, "0123456789ABCDEF", 'X', 4};


/****************************************************************************************
* Local data declarations
****************************************************************************************/
static volatile unsigned int txcharcnt;


/****************************************************************************************
** NAME:           puts
** PARAMETER:      character to write
** RETURN VALUE:   the character written.
** DESCRIPTION:    writes the string s and a newline to the terminal and returns a non-
**                 negative value.
**
****************************************************************************************/
int puts(const char *s)
{
  while(*s != '\0')
  {
    UartTxChar(*s);
    s++;
  }
  UartTxChar('\n');
  UartTxChar('\r');
  
  return 0;
} /*** end of puts ***/


/****************************************************************************************
** NAME:           printf
** PARAMETER:      format string and specifiers
** RETURN VALUE:   number of printed characters.
** DESCRIPTION:    This is a minimal implementation of the STDIO ANSI-C library function
**                 with support for the d,i,o,u,x,X format string specifiers. If this
**                 library is linked before the standard ANSI-C library iwth STDIO, the
**                 standard library functions are automatically overridden.
**
****************************************************************************************/
int printf (const char *format, ...)
{
  va_list args;
  int retval;

  va_start (args, format);
  retval = libvprintf (format, args);
  va_end (args);

  return retval;
}


/****************************************************************************************
** NAME:           scanf
** PARAMETER:      format string and specifiers
** RETURN VALUE:   number of conversions.
** DESCRIPTION:    This is a minimal implementation of the STDIO ANSI-C library function
**                 with support for the d,u,o,x format string specifiers. If this
**                 library is linked before the standard ANSI-C library iwth STDIO, the
**                 standard library functions are automatically overridden.
**
****************************************************************************************/
int scanf(const char *fmt, ...)
{
  char *s0, c;
  char buf[64];
  char *s = &buf[0];
  va_list ap;
  long L, *Lp;
  int i, *ip, rc = 0;

  do
  {
    c = UartRxChar(1); /* read byte */
    UartTxChar(c);
    *s++ = c; /* store in buf */
  }
  while((c != '\r') && (c != '\n')); /* read bytes until enter is pressed */
  *s = '\0'; /* add string termination */
  s = &buf[0]; /* set pointer to start of buf for further processing */

  va_start(ap, fmt);

  for( ; ; )
  {
    for(;;)
    {
      switch(i = *(unsigned char *)fmt++)
      {
        case 0:
          goto done;
        case '%':
          break;
        default:
          if (i <= ' ')
          {
            while(*s <= ' ')
              if (!*s++)
                goto done;
          }
          else if (*s++ != i)
            goto done;
          continue;
      }
      break;
    }

    switch(*fmt++)
    {
      case 'l':
        if (*fmt == 'd')
        {
          fmt++;
          Lp = va_arg(ap, long*);
          L = strtol(s0 = s, &s, 10);
          if (s > s0)
          {
            rc++;
            *Lp = L;
            continue;
          }
        }
        else if (*fmt == 'u')
        {
          fmt++;
          Lp = (long*)va_arg(ap, unsigned long*);
          L = strtol(s0 = s, &s, 10);
          if (s > s0)
          {
            rc++;
            *Lp = L;
            continue;
          }
        }
        else if (*fmt == 'x')
        {
          fmt++;
          Lp = (long*)va_arg(ap, unsigned long*);
          L = strtol(s0 = s, &s, 16);
          if (s > s0)
          {
            rc++;
            *Lp = L;
            continue;
          }
        }
        else if (*fmt == 'o')
        {
          fmt++;
          Lp = (long*)va_arg(ap, unsigned long*);
          L = strtol(s0 = s, &s, 8);
          if (s > s0)
          {
            rc++;
            *Lp = L;
            continue;
          }
        }
        else
          goto error;
        goto done;
      case 'd':
        ip = va_arg(ap, int*);
        L = strtol(s0 = s, &s, 10);
        if (s > s0)
        {
          rc++;
          *ip = (int)L;
          continue;
        }
        goto done;
      case 'u':
        ip = (int*)va_arg(ap, unsigned int*);
        L = strtoul(s0 = s, &s, 10);
        if (s > s0)
        {
          rc++;
          *ip = (int)L;
          continue;
        }
        goto done;
      case 'x':
        ip = va_arg(ap, int*);
        L = strtol(s0 = s, &s, 16);
        if (s > s0)
        {
          rc++;
          *ip = (int)L;
          continue;
        }
        goto done;
      case 'o':
        ip = va_arg(ap, int*);
        L = strtol(s0 = s, &s, 8);
        if (s > s0)
        {
          rc++;
          *ip = (int)L;
          continue;
        }
        goto done;
      default:
        goto error; /* wrong format */
    }
  }

  done:
    va_end(ap);
    return rc;

  error:
    va_end(ap);
    return 0;

} /*** end of scanf ***/


/****************************************************************************************
** NAME:           output_dup
** PARAMETER:      character to output and how many times.
** RETURN VALUE:   none
** DESCRIPTION:    writes ch to output cnt times.
**
****************************************************************************************/
static void output_dup(char ch, size_t cnt)
{
  while (cnt-- > 0)
  {
    UartTxChar(ch);
    txcharcnt++;
  }
} /*** end of output_dup ***/


/****************************************************************************************
** NAME:           format_integer
** PARAMETER:      all necessary conversion information.
** RETURN VALUE:   none
** DESCRIPTION:    Formats an integer to a string and transmits each character through
**                 the terminal communication interface.
**
****************************************************************************************/
static void format_integer(unsigned long value, char is_signed, char negative,
                           const struct integer_base *b,
                           const struct printf_conversion *c)
{
  char buf[64], *cp;            /* Buffer and current position. */
  int x;                        /* `x' character to use or 0 if none. */
  int sign;                     /* Sign character or 0 if none. */
  int precision;                /* Rendered precision. */
  int pad_cnt;                  /* # of pad characters to fill field width. */
  int digit_cnt;                /* # of digits output so far. */

  /* Determine sign character, if any.
     An unsigned conversion will never have a sign character,
     even if one of the flags requests one. */
  sign = 0;
  if (is_signed)
    {
      if (c->flags & PLUS)
        sign = negative ? '-' : '+';
      else if (c->flags & SPACE)
        sign = negative ? '-' : ' ';
      else if (negative)
        sign = '-';
    }

  /* Determine whether to include `0x' or `0X'.
     It will only be included with a hexadecimal conversion of a
     nonzero value with the # flag. */
  x = (c->flags & POUND) && value ? b->x : 0;

  /* Accumulate digits into buffer.
     This algorithm produces digits in reverse order, so later we
     will output the buffer's content in reverse. */
  cp = buf;
  digit_cnt = 0;
  while (value > 0)
    {
      if ((c->flags & GROUP) && digit_cnt > 0 && digit_cnt % b->group == 0)
        *cp++ = ',';
      *cp++ = b->digits[value % b->base];
      value /= b->base;
      digit_cnt++;
    }

  /* Append enough zeros to match precision.
     If requested precision is 0, then a value of zero is
     rendered as a null string, otherwise as "0".
     If the # flag is used with base 8, the result must always
     begin with a zero. */
  precision = c->precision < 0 ? 1 : c->precision;
  while (cp - buf < precision && cp < buf + sizeof buf - 1)
    *cp++ = '0';
  if ((c->flags & POUND) && b->base == 8 && (cp == buf || cp[-1] != '0'))
    *cp++ = '0';

  /* Calculate number of pad characters to fill field width. */
  pad_cnt = c->width - (cp - buf) - (x ? 2 : 0) - (sign != 0);
  if (pad_cnt < 0)
    pad_cnt = 0;

  /* Do output. */
  if ((c->flags & (MINUS | ZERO)) == 0)
    output_dup (' ', pad_cnt);
  if (sign)
  {
    UartTxChar(sign);
    txcharcnt++;
  }
  if (x)
  {
    UartTxChar ('0');
    txcharcnt++;
    UartTxChar(x);
    txcharcnt++;
  }
  if (c->flags & ZERO)
    output_dup ('0', pad_cnt);
  while (cp > buf)
  {
    UartTxChar (*--cp);
    txcharcnt++;
  }
  if (c->flags & MINUS)
    output_dup (' ', pad_cnt);
} /*** end of format_integer ***/


/****************************************************************************************
** NAME:           parse_conversion
** PARAMETER:      all necessary parsing information and the format string.
** RETURN VALUE:   pointer to the unchanged format string.
** DESCRIPTION:    Parses the actual printf format string for all arguments.
**
****************************************************************************************/
static const char *parse_conversion(const char *format, struct printf_conversion *c,
                                    va_list *args)
{
  /* Parse flag characters. */
  c->flags = 0;
  for (;;)
    {
      switch (*format++)
        {
        case '-':
          c->flags |= MINUS;
          break;
        case '+':
          c->flags |= PLUS;
          break;
        case ' ':
          c->flags |= SPACE;
          break;
        case '#':
          c->flags |= POUND;
          break;
        case '0':
          c->flags |= ZERO;
          break;
        case '\'':
          c->flags |= GROUP;
          break;
        default:
          format--;
          goto not_a_flag;
        }
    }
 not_a_flag:
  if (c->flags & MINUS)
    c->flags &= ~ZERO;
  if (c->flags & PLUS)
    c->flags &= ~SPACE;

  /* Parse field width. */
  c->width = 0;
  if (*format == '*')
    {
      format++;
      c->width = va_arg (*args, int);
    }
  else
    {
      for (; isdigit ((int)*format); format++)
        c->width = c->width * 10 + *format - '0';
    }
  if (c->width < 0)
    {
      c->width = -c->width;
      c->flags |= MINUS;
    }

  /* Parse precision. */
  c->precision = -1;
  if (*format == '.')
    {
      format++;
      if (*format == '*')
        {
          format++;
          c->precision = va_arg (*args, int);
        }
      else
        {
          c->precision = 0;
          for (; isdigit ((int)*format); format++)
            c->precision = c->precision * 10 + *format - '0';
        }
      if (c->precision < 0)
        c->precision = -1;
    }
  if (c->precision >= 0)
    c->flags &= ~ZERO;

  /* Parse type. */
  c->type = INT;
  switch (*format++)
    {
    case 'h':
      if (*format == 'h')
        {
          format++;
          c->type = CHAR;
        }
      else
        c->type = SHORT;
      break;

    case 'l':
        c->type = LONG;
      break;

    default:
      format--;
      break;
    }

  return format;
} /*** end of parse_conversion ***/


/****************************************************************************************
** NAME:           libvprintf
** PARAMETER:      format string and argument list.
** RETURN VALUE:   number of printed characters.
** DESCRIPTION:    low level virtual library printf function.
**
****************************************************************************************/
static int libvprintf(const char *format, va_list args)
{
   for (; *format != '\0'; format++)
    {
      struct printf_conversion c;

      /* Literally copy non-conversions to output. */
      if (*format != '%')
        {
          UartTxChar (*format);
          txcharcnt++;
          continue;
        }
      format++;

      /* %% => %. */
      if (*format == '%')
        {
          UartTxChar (*format);
          txcharcnt++;
          continue;
        }

      /* Parse conversion specifiers. */
      format = parse_conversion (format, &c, &args);

      /* Do conversion. */
      switch (*format)
        {
        case 'd':
        case 'i':
          {
            /* Signed integer conversions. */
            signed long value;

            switch (c.type)
              {
              case CHAR:
                value = (signed char) va_arg (args, int);
                break;
              case SHORT:
                value = (short) va_arg (args, int);
                break;
              case INT:
                value = va_arg (args, int);
                break;
              case LONG:
                value = va_arg (args, long);
                break;
              default:
                value = 0;
                break;
              }

            format_integer (value < 0 ? -value : value,
                            1, value < 0, &base_d, &c);
          }
          break;

        case 'o':
        case 'u':
        case 'x':
        case 'X':
          {
            /* Unsigned integer conversions. */
            unsigned long value;
            const struct integer_base *b;

            switch (c.type)
              {
              case CHAR:
                value = (unsigned char) va_arg (args, unsigned);
                break;
              case SHORT:
                value = (unsigned short) va_arg (args, unsigned);
                break;
              case INT:
                value = va_arg (args, unsigned);
                break;
              case LONG:
                value = va_arg (args, unsigned long);
                break;
              default:
                value = 0;
                break;
              }

            switch (*format)
              {
              case 'o': b = &base_o; break;
              case 'u': b = &base_d; break;
              case 'x': b = &base_x; break;
              case 'X': b = &base_X; break;
              default: b = &base_d; break;
              }

            format_integer (value, 0, 0, b, &c);
          }
          break;

        default:
          break;
        }
    }
    return txcharcnt;
} /*** end of libvprintf ***/


/************************************ end of stdio.c ***********************************/


