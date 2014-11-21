// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICMMRoICnvTool.h 576007 2013-12-17 18:07:37Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICMMROICNVTOOL_H
#define XAODTRIGL1CALOCNV_ICMMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMRoI.h"

// Forward declaration(s):
namespace LVL1{
  class CMMRoI;
}

namespace xAODMaker {

   /// The interface provided by ICMMRoICnvTool
   static const InterfaceID
   IID_ICMMRoICnvTool( "xAODMaker::ICMMRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CMMRoI from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CMMRoI objects from an existing ESD container into
    *         an xAOD::CMMRoIContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576007 $
    * $Date: 2013-12-17 19:07:37 +0100 (Tue, 17 Dec 2013) $
    */
   class ICMMRoICnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CMMRoIContainer
      virtual StatusCode convert( const LVL1::CMMRoI* esd,
                                  xAOD::CMMRoI* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICMMRoICnvTool;
      }

   }; // class ICMMRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICMMROICNVTOOL_H
