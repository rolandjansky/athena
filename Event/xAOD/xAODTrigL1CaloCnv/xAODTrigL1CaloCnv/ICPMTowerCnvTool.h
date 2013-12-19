// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICPMTowerCnvTool.h 576328 2013-12-19 16:09:32Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICPMTOWERCNVTOOL_H
#define XAODTRIGL1CALOCNV_ICPMTOWERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMTowerContainer.h"

// Forward declaration(s):
namespace LVL1{
  class CPMTower;
}

namespace xAODMaker {

   /// The interface provided by ICPMTowerCnvTool
   static const InterfaceID
   IID_ICPMTowerCnvTool( "xAODMaker::ICPMTowerCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CPMTowerContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CPMTower objects from an existing ESD container into
    *         an xAOD::CPMTowerContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576328 $
    * $Date: 2013-12-19 17:09:32 +0100 (Thu, 19 Dec 2013) $
    */
   class ICPMTowerCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CPMTowerContainer
      virtual StatusCode convert( const DataVector<LVL1::CPMTower>* esd,
                                  xAOD::CPMTowerContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICPMTowerCnvTool;
      }

   }; // class ICPMTowerCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICPMTOWERCNVTOOL_H
