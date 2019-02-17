/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class CompetingRIOsOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingRIOsOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkCompetingRIOsOnTrackTool/CompetingRIOsOnTrackTool.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

Trk::CompetingRIOsOnTrackTool::CompetingRIOsOnTrackTool(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_idHelper{},
        m_mbHelper{},
        m_compPixelToolHandle("InDet::CompetingPixelClustersOnTrackTool/CompetingPixelClustersOnTrackTool"),
        m_compPixelTool{},
        m_compSCT_Tool("InDet::CompetingSCT_ClustersOnTrackTool/CompetingSCT_ClustersOnTrackTool"),
        m_compTRT_Tool("InDet::CompetingTRT_DriftCirclesOnTrackTool/CompetingTRT_DriftCirclesOnTrackTool"),
        m_compMuonDriftCircleTool(""),
        m_compMuonClusterTool("")  {

    declareInterface<ICompetingRIOsOnTrackTool>(this);
    declareProperty("ToolForCompPixelClusters",    m_compPixelToolHandle,          "CompetingPixelClustersOnTrackTool");
    declareProperty("ToolForCompSCT_Clusters",     m_compSCT_Tool,           "CompetingSCT_ClustersOnTrackTool");
    declareProperty("ToolForCompTRT_DriftCircles", m_compTRT_Tool,           "CompetingTRT_DriftCirclesOnTrackTool");
    declareProperty("ToolForCompMuonDriftCircles", m_compMuonDriftCircleTool,"CompetingMuonDriftCirclesOnTrackTool");
    declareProperty("ToolForCompMuonClusters",     m_compMuonClusterTool,    "CompetingMuonClustersOnTrackTool");
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

Trk::CompetingRIOsOnTrackTool::~CompetingRIOsOnTrackTool() {}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode Trk::CompetingRIOsOnTrackTool::initialize() {

    StatusCode sc = AthAlgTool::initialize();
    if (sc.isFailure()) return sc;

    // --- Get the tool to create Competing SiClusters on Track
    if ( ! m_compPixelToolHandle.empty() ) {
      IAlgTool* p;
      sc=toolSvc()->retrieveTool(m_compPixelToolHandle.type(),m_compPixelToolHandle.name(),p);
      if(sc.isFailure())   {
        ATH_MSG_FATAL( "Could not retrieve the Pixel tool: "<< m_compPixelToolHandle );
        return sc;
      }
      m_compPixelTool = dynamic_cast<Trk::ICompetingRIOsOnTrackTool*>(p);
      if (!m_compPixelTool) {
        ATH_MSG_FATAL("Tool is not a ICompetingRIOsOnTrackTool!");
        return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("No Tool for making CompetingPixelClustersOnTrack given.");
      m_compPixelTool = 0;
    }
   
    if ( ! m_compSCT_Tool.empty() ) {
      ATH_CHECK(m_compSCT_Tool.retrieve()); 
    }

    /* Get the tool to create competing TRT_DriftCircles on Track
     */
    if ( ! m_compTRT_Tool.empty() ) {
      ATH_CHECK(m_compTRT_Tool.retrieve()); 
    }

    /* Get the tool to create competing MuonDriftCircles (MDT) on Track
     */
    if ( ! m_compMuonDriftCircleTool.empty() ) {
      ATH_CHECK (m_compMuonDriftCircleTool.retrieve()); 
    } 

    /* Get the tool to create Competing CSC / RPC / TGC Clusters on Track
     */
    if ( ! m_compMuonClusterTool.empty() ) {
      ATH_CHECK (m_compMuonClusterTool.retrieve());
    } 

    // Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
    ATH_CHECK (detStore()->retrieve(m_idHelper, "AtlasID"));
    m_mbHelper = new MeasurementTypeID(m_idHelper);

    ATH_MSG_DEBUG( " initialize() successful in " << name() );
    return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode Trk::CompetingRIOsOnTrackTool::finalize() {
    StatusCode sc = AlgTool::finalize();
    delete m_mbHelper;
    return sc;
}

///////////////////////////////////////////////////////////////////
// General Trk::CompetingRIOsOnTrack production
///////////////////////////////////////////////////////////////////
const Trk::CompetingRIOsOnTrack* Trk::CompetingRIOsOnTrackTool::createCompetingROT(
    const std::list< const Trk::PrepRawData* >& RIO_List,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta ) const {


    // identify by first RIO in list (if list is not genuine it is not my problem)
    Identifier id = (*(RIO_List.begin()))->identify();
    ATH_MSG_VERBOSE("first RIO ID prints as " <<m_idHelper->print_to_string(id));

    if (m_idHelper->is_pixel(id) && m_compPixelTool)
      return m_compPixelTool->createCompetingROT(RIO_List, trkPar, beta);

    if (m_idHelper->is_sct(id)   && !m_compSCT_Tool.empty())
      return m_compSCT_Tool->createCompetingROT(RIO_List, trkPar, beta);

    if (m_idHelper->is_trt(id)   && !m_compTRT_Tool.empty())
      return m_compTRT_Tool->createCompetingROT(RIO_List, trkPar, beta);

    if (m_idHelper->is_mdt(id)   && !m_compMuonDriftCircleTool.empty())
      return m_compMuonDriftCircleTool->createCompetingROT(RIO_List, trkPar, beta);

    if ( (m_idHelper->is_csc(id) || m_idHelper->is_rpc(id) || m_idHelper->is_tgc(id))
         && !m_compMuonClusterTool.empty())
      return m_compMuonClusterTool->createCompetingROT(RIO_List, trkPar, beta);


    // --- if we arrive here, the necessary sub-tool is not given or the identifier invalid
    // --- this is not necessarily a problem or a wrong configuration, e.g. the DAF can
    // --- operate in a mode, where annealing is just performed for some of the sub-detectors
    // simply return a NULL pointer:
    return 0;

}


///////////////////////////////////////////////////////////////////
// Trk::CompetingRIOsOnTrack update
///////////////////////////////////////////////////////////////////
void Trk::CompetingRIOsOnTrackTool::updateCompetingROT(
    Trk::CompetingRIOsOnTrack& compROT,
    const Trk::TrackParameters& trkPar,
    const Trk::IWeightCalculator::AnnealingFactor beta
    //const bool recreateROTs=false
) const {


    // identify by first ROT of the competing ROTs
    Identifier id = compROT.rioOnTrack(0).identify();
    ATH_MSG_VERBOSE("first ROT ID prints as " << m_idHelper->print_to_string(id));

    if (m_idHelper->is_pixel(id) && m_compPixelTool)
      return m_compPixelTool->updateCompetingROT(compROT, trkPar, beta);

    if (m_idHelper->is_sct(id)   && !m_compSCT_Tool.empty())
      return m_compSCT_Tool->updateCompetingROT(compROT, trkPar, beta);

    if (m_idHelper->is_trt(id)   && !m_compTRT_Tool.empty())
      return m_compTRT_Tool->updateCompetingROT(compROT, trkPar, beta);

    if (m_idHelper->is_mdt(id)   && !m_compMuonDriftCircleTool.empty())
      return m_compMuonDriftCircleTool->updateCompetingROT(compROT, trkPar, beta);

    if ( (m_idHelper->is_csc(id) || m_idHelper->is_rpc(id) || m_idHelper->is_tgc(id))
         && !m_compMuonClusterTool.empty())
      return m_compMuonClusterTool->updateCompetingROT(compROT, trkPar, beta);

    // --- if we arrive here, the necessary sub-tool is not given or the identifier invalid
    // --- this should not happen: once we created a CompetingROT, we should be able to update
    // --- its assignment probabilities
    ATH_MSG_WARNING( "could not identify CompetingROT as one of those with a detector-specific tool given!");
    ATH_MSG_WARNING( "PRD with identifier " << m_idHelper->print_to_string(id));
    return;
}

StatusCode Trk::CompetingRIOsOnTrackTool::updateCompetingROTprobs(
        Trk::CompetingRIOsOnTrack& compROT,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >& assignmentProbs
    ) const {
    // identify by first ROT of the competing ROTs
    Identifier id = compROT.rioOnTrack(0).identify();
    ATH_MSG_VERBOSE("first ROT ID prints as " << m_idHelper->print_to_string(id));

    if (m_idHelper->is_pixel(id) && m_compPixelTool)
      return m_compPixelTool->updateCompetingROTprobs(compROT, assignmentProbs);

    if (m_idHelper->is_sct(id)   && !m_compSCT_Tool.empty())
      return m_compSCT_Tool->updateCompetingROTprobs(compROT, assignmentProbs);

    if (m_idHelper->is_trt(id)   && !m_compTRT_Tool.empty())
      return m_compTRT_Tool->updateCompetingROTprobs(compROT, assignmentProbs);

    if (m_idHelper->is_mdt(id)   && !m_compMuonDriftCircleTool.empty())
      return m_compMuonDriftCircleTool->updateCompetingROTprobs(compROT, assignmentProbs);

    if ( (m_idHelper->is_csc(id) || m_idHelper->is_rpc(id) || m_idHelper->is_tgc(id))
         && !m_compMuonClusterTool.empty())
      return m_compMuonClusterTool->updateCompetingROTprobs(compROT, assignmentProbs);

    // --- if we arrive here, the necessary sub-tool is not given or the identifier invalid
    // --- this should not happen: once we created a CompetingROT, we should be able to update
    // --- its assignment probabilities
    ATH_MSG_WARNING( "could not identify CompetingROT as one of those with a detector-specific tool given!");
    ATH_MSG_WARNING( "PRD with identifier " << m_idHelper->print_to_string(id));
    return StatusCode::FAILURE;        
}

///////////////////////////////////////////////////////////////////
// Simplified Trk::CompetingRIOsOnTrack production (1 channel per CompROT)
///////////////////////////////////////////////////////////////////
const Trk::CompetingRIOsOnTrack* 
Trk::CompetingRIOsOnTrackTool::createSimpleCompetingROT
  (const Trk::PrepRawData& prd,
   const Trk::TrackParameters& trkPar,
   const Trk::IWeightCalculator::AnnealingFactor beta,
   const Trk::TrackState::MeasurementType mtype) const
{    
  Identifier id = prd.identify();

  if (mtype == Trk::TrackState::Pixel
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_pixel(id)) ) {
    if (!m_compPixelTool) return 0;
    std::list< const Trk::PrepRawData* > plist;
    plist.push_back(&prd);
    return m_compPixelTool->createCompetingROT(plist,trkPar,beta);
  }
  if (mtype == Trk::TrackState::SCT
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_sct(id)) ) {
    if (m_compSCT_Tool.empty()) return 0;
    std::list< const Trk::PrepRawData* > plist;
    plist.push_back(&prd);
    return m_compSCT_Tool->createCompetingROT(plist,trkPar,beta);
  }
  if (mtype == Trk::TrackState::TRT
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_trt(id)) )
    return (m_compTRT_Tool.empty() ? 0 :
            m_compTRT_Tool->createSimpleCompetingROT(prd,trkPar,beta) );

  if (mtype == Trk::TrackState::MDT
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_mdt(id)) )
    return (m_compMuonDriftCircleTool.empty() ? 0 :
        m_compMuonDriftCircleTool->createSimpleCompetingROT(prd,trkPar,beta));

  return (m_compMuonClusterTool.empty() ? 0 :
          m_compMuonClusterTool->createSimpleCompetingROT(prd,trkPar,beta));
} 

void Trk::CompetingRIOsOnTrackTool::reequipCompetingROT
(Trk::CompetingRIOsOnTrack*& modifiableCompROT,
 const Trk::RIO_OnTrack*     newROT,
 const Trk::TrackParameters& trkPar,
 const Trk::IWeightCalculator::AnnealingFactor beta,
 const Trk::TrackState::MeasurementType        mtype) const
{
  if (!newROT) {
    ATH_MSG_WARNING( "inconsistency with use of reequipCompetingROT() !" );
    return;
  }
  Identifier id = newROT->identify();

  if (mtype == Trk::TrackState::Pixel
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_pixel(id)) ) {
    if (!m_compPixelTool) return;
    m_compPixelTool->reequipCompetingROT(modifiableCompROT, newROT, trkPar, beta);
    return;
  }

  if (mtype == Trk::TrackState::SCT
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_sct(id)) ) {
    if (m_compSCT_Tool.empty()) return;
    m_compSCT_Tool->reequipCompetingROT(modifiableCompROT, newROT, trkPar, beta);
    return;
  }

  if (mtype == Trk::TrackState::TRT
      || (mtype==Trk::TrackState::unidentified && m_idHelper->is_trt(id)) ) {
    if (m_compTRT_Tool.empty()) return;
    m_compTRT_Tool->reequipCompetingROT(modifiableCompROT, newROT, trkPar, beta);
    return;
  }

}
