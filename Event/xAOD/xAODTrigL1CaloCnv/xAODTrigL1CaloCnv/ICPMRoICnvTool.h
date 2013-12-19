// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICPMRoICnvTool.h 576328 2013-12-19 16:09:32Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICPMROICNVTOOL_H
#define XAODTRIGL1CALOCNV_ICPMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMRoIContainer.h"

// Forward declaration(s):
namespace LVL1{
  class CPMRoI;
}

namespace xAODMaker {

   /// The interface provided by ICPMRoICnvTool
   static const InterfaceID
   IID_ICPMRoICnvTool( "xAODMaker::ICPMRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CPMRoIContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CPMRoI objects from an existing ESD container into
    *         an xAOD::CPMRoIContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576328 $
    * $Date: 2013-12-19 17:09:32 +0100 (Thu, 19 Dec 2013) $
    */
   class ICPMRoICnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CPMRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::CPMRoI>* esd,
                                  xAOD::CPMRoIContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICPMRoICnvTool;
      }

   }; // class ICPMRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICPMROICNVTOOL_H
