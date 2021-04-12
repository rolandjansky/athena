// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_CombinedMuonFeatureCNVTOOL_H
#define xAODTrigMuonCNV_CombinedMuonFeatureCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMuonCnv/ICombinedMuonFeatureCnvTool.h"

// forward declarations
//class CombinedMuonFeature;


namespace xAODMaker {
    
    /**
     *  @short Implementation for the ICombinedMuonFeatureCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in ICombinedMuonFeatureCnvTool.
     *
     * @author stefano giagu <stefano.giagu@cern.ch>
     *
     */
    class CombinedMuonFeatureCnvTool : public AthAlgTool,
    public virtual ICombinedMuonFeatureCnvTool {
        
    public:
        /// Regular AlgTool constructor
        CombinedMuonFeatureCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );
        
        /// Function that fills an existing xAOD::L2CombinedMuon
        virtual StatusCode convert( const CombinedMuonFeature * aod,
                                   xAOD::L2CombinedMuon* xaod ) const override;
        
    }; // class TrigElectronCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_CombinedMuonFeatureCNVTOOL_H

