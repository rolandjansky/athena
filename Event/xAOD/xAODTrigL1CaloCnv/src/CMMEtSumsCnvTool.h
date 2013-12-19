// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsCnvTool.h 576328 2013-12-19 16:09:32Z morrisj $
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
    * $Revision: 576328 $
    * $Date: 2013-12-19 17:09:32 +0100 (Thu, 19 Dec 2013) $
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
