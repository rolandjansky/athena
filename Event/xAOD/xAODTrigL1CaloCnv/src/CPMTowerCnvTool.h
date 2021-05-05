// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOCNV_CPMTOWERCNVTOOL_H
#define XAODTRIGL1CALOCNV_CPMTOWERCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICPMTowerCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ICPMTowerCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ICPMTowerCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    */
   class CPMTowerCnvTool : public AthAlgTool,
                           public virtual ICPMTowerCnvTool {

   public:
      /// Regular AlgTool constructor
      CPMTowerCnvTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::CPMTower>* esd,
                                  xAOD::CPMTowerContainer* xaod ) override;

   }; // class CPMTowerCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CPMTOWERCNVTOOL_H
