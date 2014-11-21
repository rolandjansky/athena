// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITriggerTowerCnvTool.h 576327 2013-12-19 16:08:56Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ITRIGGERTOWERCNVTOOL_H
#define XAODTRIGL1CALOCNV_ITRIGGERTOWERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

// Forward declaration(s):
namespace LVL1{
  class TriggerTower;
}

namespace xAODMaker {

   /// The interface provided by ITriggerTowerCnvTool
   static const InterfaceID
   IID_ITriggerTowerCnvTool( "xAODMaker::ITriggerTowerCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TriggerTowerContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/TriggerTower objects from an existing ESD container into
    *         an xAOD::TriggerTowerContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576327 $
    * $Date: 2013-12-19 17:08:56 +0100 (Thu, 19 Dec 2013) $
    */
   class ITriggerTowerCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::TriggerTowerContainer
      virtual StatusCode convert( const DataVector<LVL1::TriggerTower>* esd,
                                  xAOD::TriggerTowerContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITriggerTowerCnvTool;
      }

   }; // class ITriggerTowerCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ITRIGGERTOWERCNVTOOL_H
