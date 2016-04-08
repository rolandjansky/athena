/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventTPCnv/vectorize.h"
#include <cctype>


void strToUI( const std::string &m, std::vector<unsigned int> &v){
    unsigned short l = (unsigned short)m.size();
    v.reserve(v.size() + (unsigned int)((2. + l) / 4. + 0.9));
    unsigned int t   = 0;
    unsigned int rem = 16;
    t |= l << 16u; 
	
    for (unsigned int w = 0; w < l; w++)
        if (rem){
            t |= m[w] % 255 << (rem - 8);
            rem -= 8;
        }else{	
            v.push_back(t); 
            t = 0;
            rem = 24;
            t |= m[w] % 255 << rem;
        }
    v.push_back(t);
}

void UITostr(std::string &n, std::vector<unsigned int>::const_iterator &i,
             bool bugcompat){
    int l = (*i) >> 16;

    n.clear();
    n.reserve(l);
    int rem = 16;
	
    for (int w=0;w<l;w++)
        if(rem){
            rem-=8;
            short o = (*i) >> rem & 0xff;
            n.push_back(o);
        }else{
            ++i;
            rem = 24;
            short o = (*i) >> rem & 0xff; 
            n.push_back(o);
        }
    ++i;	

    // EVIL HACK:
    // Previous versions of this packing code had a bug, in which for
    // strings of length 4n+2, the last word didn't get written.
    // That bug has been fixed, but that causes problems when we
    // read old data.  We now read the proper number of words,
    // which means that for one of these corrupted strings,
    // we'll gobble the next word of the next item.
    // If the next item is a string as well, then the count
    // will be wrong, likely very large.  This will likely cause
    // us to fall off the end of the input vector, leading to a crash.
    // The code here tries to work around this.  If we read a string
    // with length 4n+2 and the last four characters are not all
    // printable (or in \n\t\r) we assume that we've hit this bug
    // and drop the last word.
    // In current releases, we set a bit in the version word
    // to mark that the packing bug has been fixed, so we only
    // do this test if that flag was clear.
    // We can't just test that flag without testing the string
    // as well, because release 15.4.0 had the packing bug
    // fixed but didn't have the bug fixed flag.
    if (bugcompat && l > 2 && (l%4) == 2)
        {
            bool ok = true;
            for (int j = 0; ok && j < 4; j++) {
                char c = n[l-j-1];
                if (!std::isprint(c) && c != '\n' && c != '\t' && c != '\r')
                    ok = false;
            }
            if (!ok) {
                n.resize(l-4);
                --i;
            }
        }
}

void bitmapToUI( const std::vector<bool> &m, std::vector<unsigned int> &v){
    unsigned short l = m.size();
    int rem=16;
    unsigned int t=0;
    t|=l<<16;
    for(int w=0;w<l;w++){
        rem -=1;
        t|=m[w]<<rem;

        if (rem==0) {
            v.push_back(t);
            t=0;
            rem=32;
        }
    }
    if(rem!=32) v.push_back(t);
}

void UITobitmap(std::vector<bool> &n,
                std::vector<unsigned int>::const_iterator &i,
                bool bugcompat)
{
    n.clear();
    int l=(*i)>>16;
    n.reserve(l);
    int rem = 16;
    for (int w=0;w<l;w++){
        rem -=1;
        n.push_back((*i)>>rem & 0x1);

        if(rem==0){
            ++i;
            rem=32;
        }
    }
    if (rem!=32) ++i;	

    if (bugcompat && l > 16 && (l%32) == 16) {
        --i;
        n.resize (l-32);
    }
}


