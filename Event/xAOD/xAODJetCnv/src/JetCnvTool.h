// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODJETCNV_JETCNVTOOL_H
#define XAODJETCNV_JETCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODJetCnv/IJetCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJetCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJetCnvTool.
    *
    * @author P-A Delsart
    */
  class JetCnvTool : public AthAlgTool,
                     public virtual IJetCnvTool {

  public:
    /// Regular AlgTool constructor
    JetCnvTool( const std::string& type, const std::string& name,
                const IInterface* parent );
    
    /// Function that fills an existing xAOD::MuonRoIContainer
    virtual StatusCode convert( const JetCollection* aodCont,
                                xAOD::JetContainer* xaodCont,
                                DataLink<xAOD::IParticleContainer> xaodConstitCont = DataLink<xAOD::IParticleContainer>(),
                                bool constitSearch = false) const override;
    
  }; // class JetCnvTool
  
} // namespace xAODMaker

#endif // XAODTRIGGERCNV_MUONROICNVTOOL_H
