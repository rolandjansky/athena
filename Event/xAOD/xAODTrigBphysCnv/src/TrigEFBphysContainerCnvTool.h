// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBphysCNV_TrigEFBphysContainerCNVTOOL_H
#define xAODTrigBphysCNV_TrigEFBphysContainerCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigBphysCnv/ITrigEFBphysContainerCnvTool.h"
#include "xAODTrigBphysCnv/ITrigEFBphysCnvTool.h"


namespace xAODMaker {
    
    /**
     *  @short Implementation for the ITrigEFBphysContainerCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in ITrigEFBphysContainerCnvTool.
     *
     * @author
     *
     */
    class TrigEFBphysContainerCnvTool : public AthAlgTool,
    public virtual ITrigEFBphysContainerCnvTool {
        
    public:
        /// Regular AlgTool constructor
        TrigEFBphysContainerCnvTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
        
        /// Function initialising the tool
        virtual StatusCode initialize();
        
        /// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert(const TrigEFBphysContainer * aod,
                                   xAOD::TrigBphysContainer* xaod );
        
    private:
        ToolHandle< ITrigEFBphysCnvTool > m_cnvTool; // does the actual conversions

        
    }; // class TrigEFBphysContainerCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigBphysCNV_TrigEFBphysContainerCNVTOOL_H

