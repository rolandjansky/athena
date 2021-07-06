// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBphysCNV_TrigEFBphysCNVTOOL_H
#define xAODTrigBphysCNV_TrigEFBphysCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigBphysCnv/ITrigEFBphysCnvTool.h"

// forward declarations
//class TrigEFBphys;


namespace xAODMaker {
    
    /**
     *  @short Implementation for the ITrigEFBphysCnvTool interface
     *
     *         This is a simple tool that implements the converter function
     *         defined in ITrigEFBphysCnvTool.
     */
    class TrigEFBphysCnvTool : public AthAlgTool,
    public virtual ITrigEFBphysCnvTool {
        
    public:
        /// Regular AlgTool constructor
        TrigEFBphysCnvTool( const std::string& type, const std::string& name,
                            const IInterface* parent );
        
        /// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert( const TrigEFBphys * aod,
                                   xAOD::TrigBphys* xaod ) const override;
        
    }; // class TrigElectronCnvTool
    
} // namespace xAODMaker

#endif // xAODTrigBphysCNV_TrigEFBphysCNVTOOL_H

