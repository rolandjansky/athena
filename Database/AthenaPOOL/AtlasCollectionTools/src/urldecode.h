#ifndef ACOLLTOOLS_URLDECODE
#define ACOLLTOOLS_URLDECODE

/*  urldecode - decode application/x-www-form-urlencoded data
    
    Copyright (C) 2009 ePoint Systems Ltd.
    Author: Rooslan S. Khayrov
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <strings.h>
#include <sstream>
#include <string>

namespace urldecode
{

static int hexdigit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    return -1;
}

enum states { ST_SYM, ST_PERCENT, ST_PERCENT_AND_SYM };

typedef struct decoder_state_s
{
    int state;
    char sym;
} decoder_state;

int is_unreserved(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || index("-_.~!*'();:@&=+$,/?%#[]", c);
}

/**
 Decodes URL-encoded data. Because encoded data is always bigger conversion is 
 done in-place.
 @return Number of decoded bytes written to data. Negative integer if data is 
 not valid URL-encoded sequence.
 */
ssize_t urldecode(decoder_state *state, char *data, size_t size)
{
    size_t inpos = 0, outpos = 0;
    int d1, d2;
    while (inpos < size)
    {
        char in = data[inpos++];
        switch (in)
        {
        case '%':
            switch (state->state)
            {
            case ST_SYM:
                state->state = ST_PERCENT;
                break;
            default:
                return -1;
            }
            break;
        case '+':
            switch (state->state)
            {
            case ST_SYM:
                data[outpos++] = ' ';
                break;
            default:
                return -1;
            }
            break;
        default:
            switch (state->state)
            {
            case ST_PERCENT_AND_SYM:
                d1 = hexdigit(state->sym), d2 = hexdigit(in);
                if (d1 >= 0 && d2 >= 0)
                {
                    data[outpos++] = (d1 << 4) | d2;
                }
                else
                {
                    return -1;
                }
                state->state = ST_SYM;
                break;
            case ST_PERCENT:
                state->sym = in;
                state->state = ST_PERCENT_AND_SYM;
                break;
            case ST_SYM:
                if (is_unreserved(in))
                {
                    data[outpos++] = in;
                }
                else
                {
                    return -1;
                }
                break;
            }
        }
    }
    return outpos;
}


std::pair<std::string,int> urldecode(decoder_state *state, const std::string& data, size_t size)
{
    size_t inpos = 0; 
    int outpos = 0;
    int d1, d2;
    std::stringstream output;
    while (inpos < size)
    {
        char in = data[inpos++];
        switch (in)
        {
        case '%':
            switch (state->state)
            {
            case ST_SYM:
                state->state = ST_PERCENT;
                break;
            default:
                outpos=-1;
            }
            break;
        case '+':
            switch (state->state)
            {
            case ST_SYM:
                //data[outpos++] = ' ';
                output << ' ';
                break;
            default:
                outpos=-1;
            }
            break;
        default:
            switch (state->state)
            {
            case ST_PERCENT_AND_SYM:
                d1 = hexdigit(state->sym), d2 = hexdigit(in);
                if (d1 >= 0 && d2 >= 0)
                {
                    //data[outpos++] = (d1 << 4) | d2;
                    char temp = (d1 << 4) | d2;
                    output << temp;
                }
                else
                {
                    outpos=-1;
                }
                state->state = ST_SYM;
                break;
            case ST_PERCENT:
                state->sym = in;
                state->state = ST_PERCENT_AND_SYM;
                break;
            case ST_SYM:
                if (is_unreserved(in))
                {
                    //data[outpos++] = in;
                    output << in;
                }
                else
                {
                    outpos=-1;
                }
                break;
            }
        }
    }
    return std::make_pair(output.str(),outpos);
}

} // end namespace urldecode

#endif
