// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBjetCnvAlg.h 785655 2016-11-22 06:15:01Z ssnyder $
#ifndef XAODBTAGGINGCNV_TRIGBJETCNVALG_H
#define XAODBTAGGINGCNV_TRIGBJETCNVALG_H


// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODBTaggingCnv/ITrigBjetCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Algorithm for creating xAOD::BjetContainer from an AOD
    *
    *         This algorithm can be used to create an xAOD muon RoI container
    *         out of the objects found in an AOD file.
    *
    */
   class TrigBjetCnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      TrigBjetCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input container
      std::string m_aodKey;
      /// StoreGate key for the output container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< ITrigBjetCnvTool > m_cnvTool;

   }; // class TrigBjetCnvAlg

} // namespace xAODMaker

#endif // XAODBTAGGINGCNV_TRIGBJETCNVALG_H
