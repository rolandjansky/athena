// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSumsCnvAlg.h 575995 2013-12-17 16:56:45Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CMMETSUMSCNVALG_H
#define XAODTRIGL1CALOCNV_CMMETSUMSCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICMMEtSumsCnvTool.h"

namespace xAODMaker {
   /**
    *  @short Algorithm for creating xAOD::CMMEtSums from an ESD
    *
    *         This algorithm can be used to create an xAOD CMMEtSums container
    *         out of the objects found in an ESD file.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 575995 $
    * $Date: 2013-12-17 17:56:45 +0100 (Tue, 17 Dec 2013) $
    */
   class CMMEtSumsCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      CMMEtSumsCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input container
      std::string m_esdKey;
      /// StoreGate key for the output container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< ICMMEtSumsCnvTool > m_cnvTool;

   }; // class EmTauRoICnvAlg

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CMMETSUMSCNVALG_H
