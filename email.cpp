#include <iostream>
#include <string>
#include <stdbool.h>
#include <boost/regex.hpp>

/* This is not RFC 822 complaint XXX TODO*/
char exper[] = "([^.@]+)(\\.[^.@]+)*@([^.@]+\\.)+([^.@]+)";

bool 
validate_email (const char *s)
{
    boost::regex e1(exper);

    if (boost::regex_match(s, e1) == 0) { 
        return false;
    }
    return true;
}
