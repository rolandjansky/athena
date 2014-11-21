// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IJEMHitsCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_IJEMHITSCNVTOOL_H
#define XAODTRIGL1CALOCNV_IJEMHITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/JEMHitsContainer.h"

// Forward declaration(s):
namespace LVL1{
  class JEMHits;
}

namespace xAODMaker {

   /// The interface provided by IJEMHitsCnvTool
   static const InterfaceID
   IID_IJEMHitsCnvTool( "xAODMaker::IJEMHitsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating JEMHitsContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/JEMHits objects from an existing ESD container into
    *         an xAOD::JEMHitsContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class IJEMHitsCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::JEMHitsContainer
      virtual StatusCode convert( const DataVector<LVL1::JEMHits>* esd,
                                  xAOD::JEMHitsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IJEMHitsCnvTool;
      }

   }; // class IJEMHitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_IJEMHITSCNVTOOL_H
