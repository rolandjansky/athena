/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// V1.0
#ifndef xAODTrigMuonCNV_IIsoMuonFeatureContainerCnvTool_H
#define xAODTrigMuonCNV_IIsoMuonFeatureContainerCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTrigMuon/L2IsoMuonContainer.h"


// Forward declaration(s):
class IsoMuonFeatureContainer;

namespace xAODMaker {

  /// The interface 
  static const InterfaceID
    IID_IIsoMuonFeatureContainerCnvTool( "xAODMaker::IIsoMuonFeatureContainerCnvTool", 1, 0 );

  /**
   *  @short Interface for the tool creating IsoMuonFeatureContainer from an AOD
   *
   *         This interface is implemented by the tool that converts the 
   *         IsoMuonFeatureContainer from and existing AOD into an xAOD::L2IsoMuonContainer
   */
  class IIsoMuonFeatureContainerCnvTool : public virtual IAlgTool {

    public:
      /// Function that fills an existing xAOD::L2IsoMuonContainer
        virtual StatusCode convert( const IsoMuonFeatureContainer* aod,
                                   xAOD::L2IsoMuonContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
        return IID_IIsoMuonFeatureContainerCnvTool;
      }

  }; // class IIsoMuonFeatureContainerCnvTool

} // namespace xAODMaker

#endif // xAODTrigMuonCNV_IIsoMuonFeatureContainerCnvTool_H
