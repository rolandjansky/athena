/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OUTPUTDEF_H
#define OUTPUTDEF_H


// for compatibility between gcc2.95 and gcc3.2
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    #include <sstream>
    typedef std::ostringstream __osstream;
    #define COUNT str().size()
#else
    // put your gcc 2.95 specific code here
    #include <strstream>
    typedef ostrstream __osstream;
    #define COUNT pcount()
#endif


#endif
