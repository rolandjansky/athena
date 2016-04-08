// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_IsoMuonFeatureContainerCNVTOOL_H
#define xAODTrigMuonCNV_IsoMuonFeatureContainerCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMuonCnv/IIsoMuonFeatureContainerCnvTool.h"
#include "xAODTrigMuonCnv/IIsoMuonFeatureCnvTool.h"


namespace xAODMaker {
    
    /**
     *  @short Implementation for the IIsoMuonFeatureContainerCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in IIsoMuonFeatureContainerCnvTool.
     *
     * @author
     *
     */
    class IsoMuonFeatureContainerCnvTool : public AthAlgTool,
    public virtual IIsoMuonFeatureContainerCnvTool {
        
    public:
        /// Regular AlgTool constructor
        IsoMuonFeatureContainerCnvTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
        
        /// Function initialising the tool
        virtual StatusCode initialize();
        
        /// Function that fills an existing xAOD::TrigMuonContainer
        virtual StatusCode convert(const IsoMuonFeatureContainer * aod,
                                   xAOD::L2IsoMuonContainer* xaod );
        
    private:
        ToolHandle< IIsoMuonFeatureCnvTool > m_cnvTool; // does the actual conversions

        
    }; // class IsoMuonFeatureContainerCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_IsoMuonFeatureContainerCNVTOOL_H

