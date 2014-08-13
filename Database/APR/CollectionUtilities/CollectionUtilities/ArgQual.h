/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_ARGQUAL
#define UTILITIES_COLLECTION_ARGQUAL

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

/**********************************************************

ArgQual is a class version of a struct

Purpose: Contain all information on the state of 
         input for a command line argument qualifier

         argsize: cardinality of the qualifier arguments
         multiarg: whether there can be multiple units 
                   of the cardinality
         required: whether the argument is required
         desc: a string stream with a readable description 
               of the qualifier and it's defaults

Example of Usage with CmdLineArgs2:

      Args2Container argsVec(thisProgram);

      QualList markers;
      markers.insert( make_pair("-somequal", ArgQual()) );
      markers["-somequal"].desc << "this is a qualifier with default=this";

      CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      cmdLineArgs.evalArgs(argc,argv);
      argsVec.push_back(&cmdLineArgs);  // Add it to the list

      ...

      % someProgram.exe -somequal thisval

**********************************************************/
namespace pool
{
   class ArgQual
   {
  public:
     ArgQual() : argsize(1),multiarg(false),required(false)      {}

     ArgQual(int i, bool b=false, bool r=false) : argsize(i),
						  multiarg(b),
						  required(r) 
     {}

     ArgQual(std::string d, int i, bool b=false, bool r=false)
           : argsize(i),
             multiarg(b),
             required(r) 
     {
        desc << d;
     }

     ArgQual(const ArgQual& a)          { *this = a; }
     
     ArgQual& operator =(const ArgQual& r) {
        if( this != &r ) {
           argsize = r.argsize;
           multiarg = r.multiarg;
           required = r.required;
           desc.clear();  desc << r.desc.str();
        }
        return *this;
     }

     int argsize;
     bool multiarg;
     bool required;
     std::stringstream desc;

     void print(std::ostream&) const;

  friend std::ostream& operator<<(std::ostream& theStream, ArgQual& rhs);
   };

   typedef std::map<std::string,ArgQual> QualList;
}

#endif  // UTILITIES_COLLECTION_ARGQUAL



