/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONCNV_MUONSEGMENTCNVALG_H
#define XAODMUONCNV_MUONSEGMENTCNVALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"

#include <string>

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
     // the following segments do NOT contain MuGirl segments
     SG::ReadHandleKey<Trk::SegmentCollection> m_muonSegmentLocation{this,"SegmentContainerName","TrackMuonSegments"};
     SG::WriteHandleKey<xAOD::MuonSegmentContainer> m_xaodContainerName{this,"xAODContainerName","MuonSegments"};

     ToolHandle<xAODMaker::IMuonSegmentConverterTool> m_muonSegmentConverterTool{this,"MuonSegmentConverterTool","Muon::MuonSegmentConverterTool/MuonSegmentConverterTool"};
   }; // class MuonSegmentCnvAlg

} // namespace xAODMaker

#endif // XAODCREATORALGS_MUONCREATOR_H
