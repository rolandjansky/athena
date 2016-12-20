/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedInDetExtensionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCOMBINEDINDETEXTENSIONTOOL_H
#define IRECMUONCOMBINEDINDETEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

namespace MuonCombined {

  static const InterfaceID IID_IMuonCombinedInDetExtensionTool("MuonCombined::IMuonCombinedInDetExtensionTool", 1, 0);

  /** @class IMuonCombinedInDetExtensionTool
      @brief interface for tools buildingmuons from ID candidates
 
      @author Niels van Eldik
   */

  class IMuonCombinedInDetExtensionTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCombinedInDetExtensionTool interface: build combined muons from ID and MS candidates */    
    virtual void extend( const InDetCandidateCollection& inDetCandidates ) = 0;

  };

  inline const InterfaceID& IMuonCombinedInDetExtensionTool::interfaceID()
    { 
      return IID_IMuonCombinedInDetExtensionTool; 
    }

} // end of namespace

#endif 
