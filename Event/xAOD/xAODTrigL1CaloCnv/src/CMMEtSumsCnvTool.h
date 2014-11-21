// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsCnvTool.h 575995 2013-12-17 16:56:45Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CMMETSUMSCNVTOOL_H
#define XAODTRIGL1CALOCNV_CMMETSUMSCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICMMEtSumsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ICMMEtSumsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ICMMEtSumsCnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 575995 $
    * $Date: 2013-12-17 17:56:45 +0100 (Tue, 17 Dec 2013) $
    */
   class CMMEtSumsCnvTool : public AthAlgTool,
                            public virtual ICMMEtSumsCnvTool {

   public:
      /// Regular AlgTool constructor
      CMMEtSumsCnvTool( const std::string& type, const std::string& name,
                        const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::CMMEtSums>* esd,
                                  xAOD::CMMEtSumsContainer* xaod );

   }; // class CMMEtSumsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CMMETSUMSCNVTOOL_H
