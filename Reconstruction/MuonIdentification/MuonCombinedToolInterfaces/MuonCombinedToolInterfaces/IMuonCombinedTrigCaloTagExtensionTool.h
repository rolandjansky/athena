/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRECMUONCOMBINEDTRIGCALOTAGEXTENSIONTOOL_H
#define IRECMUONCOMBINEDTRIGCALOTAGEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace MuonCombined {

  static const InterfaceID IID_IMuonCombinedTrigCaloTagExtensionTool("MuonCombined::IMuonCombinedTrigCaloTagExtensionTool", 1, 0);

  /** @class IMuonCombinedTrigCaloTagExtensionTool
      @brief interface for tools buildingmuons from ID candidates
 
      @author Niels van Eldik
   */

  class IMuonCombinedTrigCaloTagExtensionTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCombinedTrigCaloTagExtensionTool interface: build combined muons from ID and MS candidates */    
    virtual void extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, 
                         const CaloCellContainer* caloCellCont = nullptr,
                         const xAOD::CaloClusterContainer* caloClusterCont = nullptr ) const = 0;
  };

  inline const InterfaceID& IMuonCombinedTrigCaloTagExtensionTool::interfaceID()
    { 
      return IID_IMuonCombinedTrigCaloTagExtensionTool; 
    }

} // end of namespace

#endif 
