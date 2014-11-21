// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IJEMRoICnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_IJEMROICNVTOOL_H
#define XAODTRIGL1CALOCNV_IJEMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMRoIContainer.h"

// Forward declaration(s):
namespace LVL1{
  class JEMRoI;
}

namespace xAODMaker {

   /// The interface provided by IJEMRoICnvTool
   static const InterfaceID
   IID_IJEMRoICnvTool( "xAODMaker::IJEMRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating JEMRoIContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/JEMRoI objects from an existing ESD container into
    *         an xAOD::JEMRoIContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class IJEMRoICnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::JEMRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::JEMRoI>* esd,
                                  xAOD::JEMRoIContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IJEMRoICnvTool;
      }

   }; // class IJEMRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_IJEMROICNVTOOL_H
