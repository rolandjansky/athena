// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMHitsCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_JEMHITSCNVTOOL_H
#define XAODTRIGL1CALOCNV_JEMHITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/IJEMHitsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJEMHitsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJEMHitsCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class JEMHitsCnvTool : public AthAlgTool,
                          public virtual IJEMHitsCnvTool {

   public:
      /// Regular AlgTool constructor
      JEMHitsCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::JEMHits>* esd,
                                  xAOD::JEMHitsContainer* xaod );

   }; // class JEMHitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_JEMHITSCNVTOOL_H
