// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBphysCNV_TrigL2BphysContainerCNVTOOL_H
#define xAODTrigBphysCNV_TrigL2BphysContainerCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigBphysCnv/ITrigL2BphysContainerCnvTool.h"
#include "xAODTrigBphysCnv/ITrigL2BphysCnvTool.h"


namespace xAODMaker {
    
    /**
     *  @short Implementation for the ITrigL2BphysContainerCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in ITrigL2BphysContainerCnvTool.
     *
     * @author
     *
     */
    class TrigL2BphysContainerCnvTool : public AthAlgTool,
    public virtual ITrigL2BphysContainerCnvTool {
        
    public:
        /// Regular AlgTool constructor
        TrigL2BphysContainerCnvTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
        
        /// Function initialising the tool
        virtual StatusCode initialize();
        
        /// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert(const TrigL2BphysContainer * aod,
                                   xAOD::TrigBphysContainer* xaod );
        
    private:
        ToolHandle< ITrigL2BphysCnvTool > m_cnvTool; // does the actual conversions

        
    }; // class TrigL2BphysContainerCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigBphysCNV_TrigL2BphysContainerCNVTOOL_H

