// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOCNV_JEMETSUMSCNVTOOL_H
#define XAODTRIGL1CALOCNV_JEMETSUMSCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/IJEMEtSumsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJEMEtSumsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJEMEtSumsCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    */
   class JEMEtSumsCnvTool : public AthAlgTool,
                            public virtual IJEMEtSumsCnvTool {

   public:
      /// Regular AlgTool constructor
      JEMEtSumsCnvTool( const std::string& type, const std::string& name,
                        const IInterface* parent );

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::JEMEtSums>* esd,
                                  xAOD::JEMEtSumsContainer* xaod ) override;

   }; // class JEMEtSumsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_JEMETSUMSCNVTOOL_H
