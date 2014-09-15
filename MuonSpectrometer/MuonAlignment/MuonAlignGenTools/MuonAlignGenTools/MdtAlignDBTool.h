/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MDTALIGNDBTOOL_H
#define MUONALIGNGENTOOLS_MDTALIGNDBTOOL_H

#include "MuonAlignGenTools/MuonAlignDBTool.h"

/**
   @file MdtAlignDBTool.h
   @class MdtAlignDBTool

   @brief Tool for reading and writing alignment constants.  Can read constants from oracle DB or an ASCII file.  Writes constants to an output ASCII file.

   Modified to inherit from MuonAlignDBTool on 7/7/10.

   @author Robert Harrington <roberth@bu.edu>, Steffen Kaiser <Steffen.Kaiser@cern.ch>
   @date 10/14/2008
*/

namespace Muon {

  class MdtAlignDBTool : public MuonAlignDBTool {
    
  public:
    MdtAlignDBTool(const std::string& type, const std::string& name,
			    const IInterface* parent);
    virtual ~MdtAlignDBTool();
        
  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MDTALIGNDBTOOL_H
