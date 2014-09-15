/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_CSCALIGNDBTOOL_H
#define MUONALIGNGENTOOLS_CSCALIGNDBTOOL_H

#include "MuonAlignGenTools/MuonAlignDBTool.h"

/**
   @file CscAlignDBTool.h
   @class CscAlignDBTool

   @brief Tool for reading and writing alignment constants.  Can read constants from oracle DB or an ASCII file.  Writes constants to an output ASCII file.
   @ Specific modifications for Csc from RH's MuonAlignDBTool
   @author Prolay Mal <prolay@physics.arizona.edu>
   @date 10/16/2009
*/

class IRDBAccessSvc;
class TFile;

namespace MuonCalib {
  class IIdToFixedIdTool;
}

namespace MuonGM {
  class MuonDetectorManager;
  class CscReadoutElement;
}

namespace Trk {
  class IAlignModuleTool;
  class TrkDetElementBase;
}

class IMultiLevelAlignmentDbTool;

namespace Muon {

  class CscAlignDBTool : virtual public MuonAlignDBTool {
    
  public:
    CscAlignDBTool(const std::string& type, const std::string& name,
		   const IInterface* parent);
    virtual ~CscAlignDBTool();
    
  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_CSCALIGNDBTOOL_H
