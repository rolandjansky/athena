// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoICnvAlg.h 576007 2013-12-17 18:07:37Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CMMROICNVALG_H
#define XAODTRIGL1CALOCNV_CMMROICNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICMMRoICnvTool.h"

namespace xAODMaker {
   /**
    *  @short Algorithm for creating xAOD::CMMRoI from an ESD
    *
    *         This algorithm can be used to create an xAOD CMMRoI container
    *         out of the objects found in an ESD file.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576007 $
    * $Date: 2013-12-17 19:07:37 +0100 (Tue, 17 Dec 2013) $
    */
   class CMMRoICnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      CMMRoICnvAlg( const std::string& name, ISvcLocator* svcLoc );

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
      ToolHandle< ICMMRoICnvTool > m_cnvTool;

   }; // class EmTauRoICnvAlg

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CMMROICNVALG_H
