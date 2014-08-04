/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_IMUONPRINTINGTOOL_H
#define REC_IMUONPRINTINGTOOL_H

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"

namespace Rec {

  /** Interface ID for Muon*/  
  static const InterfaceID IID_IMuonPrintingTool("Rec::IMuonPrintingTool", 1, 0);


  /** @class IMuonPrintingTool
      @brief interface for tool that creates a dump output of different detail to strings or files
      @author Niels van Eldik, Jean-Francois Laporte, Wolfgang Liebig
  */

  class IMuonPrintingTool : virtual public IAlgTool {
  public:
  
    static const InterfaceID& interfaceID( ) { return IID_IMuonPrintingTool; };

    /** print muon object to a string */
    virtual std::string print( const xAOD::Muon& muon ) const = 0;

    /** print a muon container to a string */
    virtual std::string print(const xAOD::MuonContainer& muons) const = 0;
  
 };

}

#endif // REC_IMUONPRINTINGTOOL_H
