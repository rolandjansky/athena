// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IJEMEtSumsCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_IJEMETSUMSCNVTOOL_H
#define XAODTRIGL1CALOCNV_IJEMETSUMSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"

// Forward declaration(s):
namespace LVL1{
  class JEMEtSums;
}

namespace xAODMaker {

   /// The interface provided by IJEMEtSumsCnvTool
   static const InterfaceID
   IID_IJEMEtSumsCnvTool( "xAODMaker::IJEMEtSumsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating JEMEtSumsContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/JEMEtSums objects from an existing ESD container into
    *         an xAOD::JEMEtSumsContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class IJEMEtSumsCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::JEMEtSumsContainer
      virtual StatusCode convert( const DataVector<LVL1::JEMEtSums>* esd,
                                  xAOD::JEMEtSumsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IJEMEtSumsCnvTool;
      }

   }; // class IJEMEtSumsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_IJEMETSUMSCNVTOOL_H
