// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderCnvAlg.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_RODHEADERCNVALG_H
#define XAODTRIGL1CALOCNV_RODHEADERCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/IRODHeaderCnvTool.h"

namespace xAODMaker {
   /**
    *  @short Algorithm for creating xAOD::RODHeader from an ESD
    *
    *         This algorithm can be used to create an xAOD RODHeader container
    *         out of the objects found in an ESD file.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class RODHeaderCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      RODHeaderCnvAlg( const std::string& name, ISvcLocator* svcLoc );

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
      ToolHandle< IRODHeaderCnvTool > m_cnvTool;

   }; // class EmTauRoICnvAlg

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_RODHEADERCNVALG_H
