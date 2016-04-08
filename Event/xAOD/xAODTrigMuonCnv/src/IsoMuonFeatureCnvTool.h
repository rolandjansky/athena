// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_IsoMuonFeatureCNVTOOL_H
#define xAODTrigMuonCNV_IsoMuonFeatureCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMuonCnv/IIsoMuonFeatureCnvTool.h"

// forward declarations
//class IsoMuonFeature;


namespace xAODMaker {
    
    /**
     *  @short Implementation for the IIsoMuonFeatureCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in IIsoMuonFeatureCnvTool.
     *
     * @author stefano giagu <stefano.giagu@cern.ch>
     *
     */
    class IsoMuonFeatureCnvTool : public AthAlgTool,
    public virtual IIsoMuonFeatureCnvTool {
        
    public:
        /// Regular AlgTool constructor
        IsoMuonFeatureCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );
        
        /// Function initialising the tool
        virtual StatusCode initialize();
        
        /// Function that fills an existing xAOD::L2IsoMuon
        virtual StatusCode convert( const IsoMuonFeature * aod,
                                   xAOD::L2IsoMuon* xaod );
        
    }; // class TrigElectronCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_IsoMuonFeatureCNVTOOL_H

