// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMJetHitsCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CMMJETHITSCNVTOOL_H
#define XAODTRIGL1CALOCNV_CMMJETHITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICMMJetHitsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ICMMJetHitsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ICMMJetHitsCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class CMMJetHitsCnvTool : public AthAlgTool,
                             public virtual ICMMJetHitsCnvTool {

   public:
      /// Regular AlgTool constructor
      CMMJetHitsCnvTool( const std::string& type, const std::string& name,
                         const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::CMMJetHits>* esd,
                                  xAOD::CMMJetHitsContainer* xaod );

   }; // class CMMJetHitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CMMJETHITSCNVTOOL_H
