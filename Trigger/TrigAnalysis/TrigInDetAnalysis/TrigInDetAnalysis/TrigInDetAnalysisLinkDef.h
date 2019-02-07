/* emacs: this is -*- c++ -*- */
/**
 **  @file    TrigInDetAnalysisLinkDef.h         
 **   
 **
 **  @author M.Sutton
 **
 **  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#if defined(__CLING__) || defined( __MAKECINT__)
#pragma link C++ namespace TIDA;
#pragma link C++ class TIDA::Vertex+;
#pragma link C++ class TIDA::Roi+;
#pragma link C++ class TIDA::Chain+;
#pragma link C++ class TIDA::Vertex+;
#pragma link C++ class TIDA::Event+;
#pragma link C++ class TIDARoiDescriptor+;
#pragma link C++ class TIDARoiParameters+;
#pragma link C++ class TFileString+;
#pragma link C++ class TrackTrigObject+;
#pragma link C++ class TIDA::Track+;
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedef;
#endif
