// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOCNV_JETELEMENTCNVTOOL_H
#define XAODTRIGL1CALOCNV_JETELEMENTCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/IJetElementCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJetElementCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJetElementCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    */
   class JetElementCnvTool : public AthAlgTool,
                             public virtual IJetElementCnvTool {

   public:
      /// Regular AlgTool constructor
      JetElementCnvTool( const std::string& type, const std::string& name,
                         const IInterface* parent );

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::JetElement>* esd,
                                  xAOD::JetElementContainer* xaod ) override;

   }; // class JetElementCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_JETELEMENTCNVTOOL_H
