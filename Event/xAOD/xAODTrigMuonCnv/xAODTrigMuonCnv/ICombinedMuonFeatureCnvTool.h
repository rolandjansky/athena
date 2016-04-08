/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// V1.0
#ifndef xAODTrigMuonCNV_ICombinedMuonFeatureCnvTool_H
#define xAODTrigMuonCNV_ICombinedMuonFeatureCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMuon/L2CombinedMuon.h"


// Forward declaration(s):
class CombinedMuonFeature;

namespace xAODMaker {

  /// The interface 
  static const InterfaceID
    IID_ICombinedMuonFeatureCnvTool( "xAODMaker::ICombinedMuonFeatureCnvTool", 1, 0 );

  /**
   *  @short Interface for the tool creating CombinedMuonFeature from an AOD
   *
   *         This interface is implemented by the tool that converts the 
   *         CombinedMuonFeature from and existing AOD into an xAOD::L2CombinedMuon
   */
  class ICombinedMuonFeatureCnvTool : public virtual IAlgTool {

    public:
      /// Function that fills an existing xAOD::L2CombinedMuon
        virtual StatusCode convert( const CombinedMuonFeature* aod,
                                   xAOD::L2CombinedMuon* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
        return IID_ICombinedMuonFeatureCnvTool;
      }

  }; // class ICombinedMuonFeatureCnvTool

} // namespace xAODMaker

#endif // xAODTrigMuonCNV_ICombinedMuonFeatureCnvTool_H
