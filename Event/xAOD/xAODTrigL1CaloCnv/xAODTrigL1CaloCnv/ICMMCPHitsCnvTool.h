// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICMMCPHitsCnvTool.h 575995 2013-12-17 16:56:45Z morrisj $
#ifndef XAODTRIGL1CALOCNV_ICMMCPHITSCNVTOOL_H
#define XAODTRIGL1CALOCNV_ICMMCPHITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/CMMCPHitsContainer.h"

// Forward declaration(s):
namespace LVL1{
  class CMMCPHits;
}

namespace xAODMaker {

   /// The interface provided by ICMMCPHitsCnvTool
   static const InterfaceID
   IID_ICMMCPHitsCnvTool( "xAODMaker::ICMMCPHitsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating CMMCPHitsContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/CMMCPHits objects from an existing ESD container into
    *         an xAOD::CMMCPHitsContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 575995 $
    * $Date: 2013-12-17 17:56:45 +0100 (Tue, 17 Dec 2013) $
    */
   class ICMMCPHitsCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::CMMCPHitsContainer
      virtual StatusCode convert( const DataVector<LVL1::CMMCPHits>* esd,
                                  xAOD::CMMCPHitsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICMMCPHitsCnvTool;
      }

   }; // class ICMMCPHitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_ICMMCPHITSCNVTOOL_H
