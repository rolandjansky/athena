/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// V1.0
#ifndef xAODTrigMuonCNV_IIsoMuonFeatureCnvTool_H
#define xAODTrigMuonCNV_IIsoMuonFeatureCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMuon/L2IsoMuon.h"


// Forward declaration(s):
class IsoMuonFeature;

namespace xAODMaker {

  /// The interface 
  static const InterfaceID
    IID_IIsoMuonFeatureCnvTool( "xAODMaker::IIsoMuonFeatureCnvTool", 1, 0 );

  /**
   *  @short Interface for the tool creating IsoMuonFeature from an AOD
   *
   *         This interface is implemented by the tool that converts the 
   *         IsoMuonFeature from and existing AOD into an xAOD::L2IsoMuon
   */
  class IIsoMuonFeatureCnvTool : public virtual IAlgTool {

    public:
      /// Function that fills an existing xAOD::L2IsoMuon
        virtual StatusCode convert( const IsoMuonFeature* aod,
                                   xAOD::L2IsoMuon* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
        return IID_IIsoMuonFeatureCnvTool;
      }

  }; // class IIsoMuonFeatureCnvTool

} // namespace xAODMaker

#endif // xAODTrigMuonCNV_IIsoMuonFeatureCnvTool_H
