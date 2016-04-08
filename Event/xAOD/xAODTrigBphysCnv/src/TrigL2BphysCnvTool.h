// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBphysCNV_TrigL2BphysCNVTOOL_H
#define xAODTrigBphysCNV_TrigL2BphysCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigBphysCnv/ITrigL2BphysCnvTool.h"

// forward declarations
//class TrigL2Bphys;


namespace xAODMaker {
    
    /**
     *  @short Implementation for the ITrigL2BphysCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in ITrigL2BphysCnvTool.
     *
     * @author
     *
     */
    class TrigL2BphysCnvTool : public AthAlgTool,
    public virtual ITrigL2BphysCnvTool {
        
    public:
        /// Regular AlgTool constructor
        TrigL2BphysCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );
        
        /// Function initialising the tool
        virtual StatusCode initialize();
        
        /// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert( const TrigL2Bphys * aod,
                                   xAOD::TrigBphys* xaod );
        
    }; // class TrigElectronCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigBphysCNV_TrigL2BphysCNVTOOL_H

