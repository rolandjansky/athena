/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Tool responsible for wrapping ATHENA identifier to Standalone Muon Fixed Identifier
 * --------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 20 May 2005
 * Last Update  : 20 May 2005
 ***************************************************************************/

#ifndef MUONCALIB_IDTOFIXEDID_H
#define MUONCALIB_IDTOFIXEDID_H
// std
#include <string>

// other packages
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

// this package
#include "MuonCalibITools/IIdToFixedIdTool.h"

namespace MuonCalib {

/**
@class IdToFixedIdTool
Tool responsible for wrapping the athena dependent Identifier class to a standalone Calib-EDM
MuonFixedId identifier.

The athena-dependent Indentifier relies on idHelpers in order to decode the content. A MuonFixedId has methods itself to decode the content of the identifier.
   
@author Zdenko.Van.Kesteren@cern.ch
*/

class IdToFixedIdTool : public AthAlgTool, virtual public MuonCalib::IIdToFixedIdTool {
  public: 
    IdToFixedIdTool(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);  //!< AlgTool constructor 
    virtual ~IdToFixedIdTool()=default;
    
    StatusCode initialize(); //!< IdToFixedIdTool initialization: retrieve all the IdHelpers needed to do strap the ATHENA Identifier

    void print(const Identifier& id) const;               //!< print method to output an ATHENA Identifier, warning the user when the conversion and reconversion yields different results. 
    void print(const MuonFixedId& fid) const;             //!< dump MuonFixedId 

    Identifier fixedIdToId(const MuonFixedId& fid) const; //!< Convert MuonFixedId to ATHENA Identifier 
    MuonFixedId idToFixedId(const Identifier& id) const;  //!< Convert ATHENA Identifier to MuonFixedId 
    Identifier regionKeyToId(std::string region) const;   //!< Returns an ATHENA Identifier for a given Region key. 

 private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    int                                 m_print_level; //!< Sets printlevel of output
};
}

#endif //MUONCALIB_IDTOFIXEDID_H
