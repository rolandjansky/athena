/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILETRIPREADERLINKDEF_H
#define TILETRIPREADERLINKDEF_H

#include "TileTripReader/TTileTripReader.h"

#ifdef __CINT__


#include <vector>

#pragma link off all typedefs;
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class vector<char>+; 
#pragma link C++ class Root::TTileTripReader+;

#endif // #ifdef __CINT__

#endif /*TILETRIPREADERLINKDEF_H*/
