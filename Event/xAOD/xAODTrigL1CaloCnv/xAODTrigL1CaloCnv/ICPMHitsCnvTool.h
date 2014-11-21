// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICPMHitsCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICPMHitsCNVTOOL_H
#define XAODTRIGL1CALOCNV_ICPMHitsCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CPMHitsContainer.h"

// Forward declaration(s):
namespace LVL1{
  class CPMHits;
}

namespace xAODMaker {

   /// The interface provided by ICPMHitsCnvTool
   static const InterfaceID
   IID_ICPMHitsCnvTool( "xAODMaker::ICPMHitsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CPMHitsContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CPMHits objects from an existing ESD container into
    *         an xAOD::CPMHitsContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class ICPMHitsCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CPMHitsContainer
      virtual StatusCode convert( const DataVector<LVL1::CPMHits>* esd,
                                  xAOD::CPMHitsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICPMHitsCnvTool;
      }

   }; // class ICPMHitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICPMHitsCNVTOOL_H
