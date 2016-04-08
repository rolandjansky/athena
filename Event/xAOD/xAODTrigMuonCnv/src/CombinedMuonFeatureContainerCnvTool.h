// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_CombinedMuonFeatureContainerCNVTOOL_H
#define xAODTrigMuonCNV_CombinedMuonFeatureContainerCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMuonCnv/ICombinedMuonFeatureContainerCnvTool.h"
#include "xAODTrigMuonCnv/ICombinedMuonFeatureCnvTool.h"


namespace xAODMaker {
    
    /**
     *  @short Implementation for the ICombinedMuonFeatureContainerCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in ICombinedMuonFeatureContainerCnvTool.
     *
     * @author
     *
     */
    class CombinedMuonFeatureContainerCnvTool : public AthAlgTool,
    public virtual ICombinedMuonFeatureContainerCnvTool {
        
    public:
        /// Regular AlgTool constructor
        CombinedMuonFeatureContainerCnvTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
        
        /// Function initialising the tool
        virtual StatusCode initialize();
        
        /// Function that fills an existing xAOD::TrigMuonContainer
        virtual StatusCode convert(const CombinedMuonFeatureContainer * aod,
                                   xAOD::L2CombinedMuonContainer* xaod );
        
    private:
        ToolHandle< ICombinedMuonFeatureCnvTool > m_cnvTool; // does the actual conversions

        
    }; // class CombinedMuonFeatureContainerCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_CombinedMuonFeatureContainerCNVTOOL_H

