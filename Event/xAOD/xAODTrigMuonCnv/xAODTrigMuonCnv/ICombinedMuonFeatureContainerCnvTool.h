/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// V1.0
#ifndef xAODTrigMuonCNV_ICombinedMuonFeatureContainerCnvTool_H
#define xAODTrigMuonCNV_ICombinedMuonFeatureContainerCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTrigMuon/L2CombinedMuonContainer.h"


// Forward declaration(s):
class CombinedMuonFeatureContainer;

namespace xAODMaker {

  /// The interface 
  static const InterfaceID
    IID_ICombinedMuonFeatureContainerCnvTool( "xAODMaker::ICombinedMuonFeatureContainerCnvTool", 1, 0 );

  /**
   *  @short Interface for the tool creating CombinedMuonFeatureContainer from an AOD
   *
   *         This interface is implemented by the tool that converts the 
   *         CombinedMuonFeatureContainer from and existing AOD into an xAOD::L2CombinedMuonContainer
   */
  class ICombinedMuonFeatureContainerCnvTool : public virtual IAlgTool {

    public:
      /// Function that fills an existing xAOD::L2CombinedMuonContainer
        virtual StatusCode convert( const CombinedMuonFeatureContainer* aod,
                                   xAOD::L2CombinedMuonContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
        return IID_ICombinedMuonFeatureContainerCnvTool;
      }

  }; // class ICombinedMuonFeatureContainerCnvTool

} // namespace xAODMaker

#endif // xAODTrigMuonCNV_ICombinedMuonFeatureContainerCnvTool_H
