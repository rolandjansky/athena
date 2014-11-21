// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerCnvTool.h 630277 2014-11-21 19:48:39Z vscharf $
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
    * $Revision: 630277 $
    * $Date: 2014-11-21 20:48:39 +0100 (Fri, 21 Nov 2014) $
    */
   class TriggerTowerCnvTool : public AthAlgTool,
                               public virtual ITriggerTowerCnvTool {

   public:
      /// Regular AlgTool constructor
      TriggerTowerCnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::TriggerTower>* esd,
                                  xAOD::TriggerTowerContainer* xaod );
   }; // class TriggerTowerCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_TRIGGERTOWERCNVTOOL_H
