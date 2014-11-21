// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICMMJetHitsCnvTool.h 575995 2013-12-17 16:56:45Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICMMJETHITSCNVTOOL_H
#define XAODTRIGL1CALOCNV_ICMMJETHITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMJetHitsContainer.h"

// Forward declaration(s):
namespace LVL1{
  class CMMJetHits;
}

namespace xAODMaker {

   /// The interface provided by ICMMJetHitsCnvTool
   static const InterfaceID
   IID_ICMMJetHitsCnvTool( "xAODMaker::ICMMJetHitsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CMMJetHitsContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CMMJetHits objects from an existing ESD container into
    *         an xAOD::CMMJetHitsContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 575995 $
    * $Date: 2013-12-17 17:56:45 +0100 (Tue, 17 Dec 2013) $
    */
   class ICMMJetHitsCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CMMJetHitsContainer
      virtual StatusCode convert( const DataVector<LVL1::CMMJetHits>* esd,
                                  xAOD::CMMJetHitsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICMMJetHitsCnvTool;
      }

   }; // class ICMMJetHitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICMMJETHITSCNVTOOL_H
