// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSegmentCnvAlg.h 297747 2013-10-28 15:14:24Z krasznaa $
#ifndef XAODMUONCNV_MUONSEGMENTCNVALG_H
#define XAODMUONCNV_MUONSEGMENTCNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
// xAOD include
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"

namespace Analysis {
  class Muon;
}


namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::MuonSegments from MuonSegments
    *
    *         This algorithm can be used to translate the MuonSegments coming
    *         from an AOD, and create xAOD::MuonSegment objects out of them
    *         for an output xAOD.
    *
    * @author Niels van Eldik
    *
    */
   class MuonSegmentCnvAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      MuonSegmentCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      std::string   m_muonSegmentLocation; //!< Location/Key for Muon::MuonSegment
      std::string   m_xaodContainerName; //!< Location/Key for xAOD::MuonSegment

      ToolHandle<xAODMaker::IMuonSegmentConverterTool>  m_muonSegmentConverterTool;
   }; // class MuonSegmentCnvAlg

} // namespace xAODMaker

#endif // XAODCREATORALGS_MUONCREATOR_H
