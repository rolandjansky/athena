/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/ArgQual.h"

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

using namespace pool;

void ArgQual::print(std::ostream& theStream) const
{
  if (required) theStream << "(REQUIRED) ";
  else theStream << "(OPTIONAL) ";
  if (multiarg) theStream << "Takes arguments in groups of " << argsize << std::endl;
  else theStream << "Takes " << argsize << " arguments" << std::endl;
  if (desc.str().size()>0) theStream << "Format: " << desc.str() << std::endl;
  else theStream << "NO DESCRIPTION FOR THIS QUALIFIER" << std::endl;
}

std::ostream& operator<<(std::ostream& theStream, ArgQual& rhs) {
  if (rhs.multiarg) theStream << "Takes arguments in groups of " << rhs.argsize << std::endl;
  else theStream << "Takes " << rhs.argsize << " arguments" << std::endl;
  if (rhs.desc.str().size()>0) theStream << rhs.desc.str() << std::endl;
  else theStream << "NO DESCRIPTION FOR THIS QUALIFIER" << std::endl;
  return theStream;
}

