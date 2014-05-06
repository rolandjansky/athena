/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 352368 2011-03-17 12:29:37Z krasznaa $
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ class vector<float>+;
#pragma link C++ class vector<short>+;

#pragma link C++ class D3PDReader::EventInfoD3PDObject+;
#pragma link C++ class D3PDReader::MuonROID3PDObject+;
#pragma link C++ class D3PDReader::MuonROID3PDObjectElement+;

#endif // __CINT__
