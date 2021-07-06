// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOCNV_TRIGGERTOWERCNVTOOL_H
#define XAODTRIGL1CALOCNV_TRIGGERTOWERCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ITriggerTowerCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITriggerTowerCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITriggerTowerCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    */
   class TriggerTowerCnvTool : public AthAlgTool,
                               public virtual ITriggerTowerCnvTool {

   public:
      /// Regular AlgTool constructor
      TriggerTowerCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::TriggerTower>* esd,
                                  xAOD::TriggerTowerContainer* xaod ) override;
   }; // class TriggerTowerCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_TRIGGERTOWERCNVTOOL_H
