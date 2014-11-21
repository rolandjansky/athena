// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICMMEtSumsCnvTool.h 575995 2013-12-17 16:56:45Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICMMETSUMSCNVTOOL_H
#define XAODTRIGL1CALOCNV_ICMMETSUMSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMEtSumsContainer.h"

// Forward declaration(s):
namespace LVL1{
  class CMMEtSums;
}

namespace xAODMaker {

   /// The interface provided by ICMMEtSumsCnvTool
   static const InterfaceID
   IID_ICMMEtSumsCnvTool( "xAODMaker::ICMMEtSumsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CMMEtSumsContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CMMEtSums objects from an existing ESD container into
    *         an xAOD::CMMEtSumsContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 575995 $
    * $Date: 2013-12-17 17:56:45 +0100 (Tue, 17 Dec 2013) $
    */
   class ICMMEtSumsCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CMMEtSumsContainer
      virtual StatusCode convert( const DataVector<LVL1::CMMEtSums>* esd,
                                  xAOD::CMMEtSumsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICMMEtSumsCnvTool;
      }

   }; // class ICMMEtSumsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICMMETSUMSCNVTOOL_H
