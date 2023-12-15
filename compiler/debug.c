#include "utils.c"

char *type_to_string(Type type)
{
    struct
    {
        Type type;
        char *name;
    } Types[] = {
        {eof_, "EOF"},
        {add_, "+"},
        {sub_, "-"},
        {mul_, "*"},
        {div_, "/"},
        {lparent_, "("},
        {rparent_, ")"},
        {assign_, "="},
        {void_var_, "void_var"},
        {num_val_, "num_val"},
        {char_val_, "char_val"},
        {num_var_, "num_var"},
        {char_var_, "char_var"},
        {func_call_, "function call"},
        {func_dec_, "function declaration"}};

    for (int i = 0; sizeof(Types) / sizeof(*Types); i++)
        if (Types[i].type == type)
            return Types[i].name;
    return NULL;
}

// built in functions
// printing
long power(long exponent, int p)
{
    if (exponent < 0)
        return 0; // negative exponents are not handled in this simple example

    long result = 1;
    for (long i = 0; i < exponent; i++)
        result *= p;
    return result;
}

void ft_putchar(int fd, int c)
{
    write(fd, &c, 1);
}

void ft_putstr(int fd, char *str)
{
    if (!str)
        return (ft_putstr(fd, "(null)"));
    write(fd, str, strlen(str));
}

void ft_putnbr(int fd, long int num)
{
    if (num < 0)
    {
        ft_putchar(fd, '-');
        num = -num;
    }
    if (num >= 0 && num < 10)
        ft_putchar(fd, '0' + num);
    if (num >= 10)
    {
        ft_putnbr(fd, num / 10);
        ft_putchar(fd, num % 10 + '0');
    }
}
// TODO: see if you can remove this shit !!!
int numberOfDigits(long n)
{
    int count = 0;
    while (n != 0)
    {
        count++;
        n /= 10;
    }
    return count;
}

void ft_putFixedPoint(int fd, long number, int exponent)
{
    long p = power(exponent, 10);
    long int_part = exponent ? number / p : number;
    long frac_part = exponent ? number % p : 0;
    ft_putnbr(fd, int_part);
    if (exponent)
    {
        int limit = exponent - numberOfDigits(frac_part);
        for (int i = 0; i < limit; i++)
            ft_putchar(fd, '0');
        ft_putnbr(fd, frac_part);
    }
}

void ft_putnbrBase(int fd, size_t num, char *to)
{
    size_t len = strlen(to);
    if (num < len)
        ft_putchar(fd, to[num]);
    if (num >= len)
    {
        ft_putnbrBase(fd, num / len, to);
        ft_putchar(fd, to[num % len]);
    }
}

void ft_putfloat(int fd, double num, int decimal_places)
{
    // TODO: protect it from overflow
    if (num < 0.0)
    {
        ft_putchar(fd, '-');
        num = -num;
    }
    long int_part = (long)num;
    double float_part = num - (double)int_part;
    while (decimal_places > 0)
    {
        float_part *= 10;
        decimal_places--;
    }
    ft_putnbr(fd, int_part);
    if (decimal_places)
    {
        ft_putchar(fd, '.');
        ft_putnbr(fd, (long)round(float_part));
    }
}
void error(char *msg)
{
    // free memory before exiting
    ft_putstr(2, "\033[0;31mError: ");
    ft_putstr(2, msg);
    ft_putstr(2, "\n");
    exit(1);
}

void debug(char *conv, ...)
{
    size_t len = strlen(conv);
    size_t i = 0;

    va_list args;
    va_start(args, conv);
    int fd = 1;
    while (i < len)
    {
        if (conv[i] == '%')
        {
            i++;
            switch (conv[i])
            {
            case 'c':
                ft_putchar(fd, va_arg(args, int));
                break;
            case 's':
                ft_putstr(fd, va_arg(args, char *));
                break;
            case 'p':
                ft_putstr(fd, "0x");
                ft_putnbrBase(fd, (size_t)(va_arg(args, void *)), "0123456789abcdef");
                break;
            case 'x':
                ft_putnbrBase(fd, (size_t)va_arg(args, void *), "0123456789abcdef");
                break;
            case 'X':
                ft_putnbrBase(fd, (size_t)va_arg(args, void *), "0123456789ABCDEF");
                break;
            case 'd':
                ft_putnbr(fd, (int)va_arg(args, int));
                break;
            case 'f':
                ft_putfloat(fd, (double)va_arg(args, double), 10);
                break;
            case '%':
                ft_putchar(fd, '%');
                break;
            case 'k':
            {
                Token *token = (Token *)va_arg(args, Token *);
                if (token)
                {
                    ft_putstr(fd, "type: ");
                    ft_putstr(fd, type_to_string(token->type));
                    switch (token->type)
                    {
                    case char_var_:
                    case void_var_:
                    case num_var_:
                        ft_putstr(fd, ", name: ");
                        ft_putstr(fd, token->name);
                    default:
                        break;
                    }
                    switch (token->type)
                    {
                    case char_val_:
                        ft_putstr(fd, ", value: ");
                        ft_putstr(fd, token->character);
                        ft_putstr(fd, " , number: LC");
                        ft_putnbr(fd, token->LC);
                        break;
                    case num_val_:
                        ft_putstr(fd, ", value: ");
                        ft_putnbr(fd, token->number);
                        break;
                    default:
                        // ft_putstr(fd, "Unkown");
                        break;
                    }
                }
                else
                    ft_putstr(fd, "(null)");
                break;
            }
            default:
                error("in debug function");
                break;
            }
            // i++;
        }
        else
            ft_putchar(fd, conv[i]);
        i++;
    }
}