 /*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_VertexReco/AFP_VertexRecoBasic.h"



AFP_VertexRecoBasic::AFP_VertexRecoBasic (const std::string &type, const std::string &name, const IInterface *parent)
 : base_class(type, name, parent)
{
  ATH_MSG_DEBUG("in AFP_VertexRecoBasic constructor");
}


StatusCode AFP_VertexRecoBasic::configInfo () const 
{
  ATH_MSG_INFO("----- AFP_VertexRecoBasic -----");

  ATH_MSG_INFO("\tCuts:\n");
  ATH_MSG_INFO("\t\ttrackDistance [mm]: " << m_trackDistance );
  
  CHECK( m_tofVtxParamDBTool.retrieve() );
  
  return StatusCode::SUCCESS;
}


StatusCode AFP_VertexRecoBasic::initialize () 
{

  CHECK( m_tofTrackContainerKey.initialize() );
  CHECK( m_protonContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode AFP_VertexRecoBasic::doVertexReco(std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer, const EventContext& ctx) const 
{
  
  SG::ReadHandle<xAOD::AFPToFTrackContainer> tofTrackContainer( m_tofTrackContainerKey, ctx );
  if(!tofTrackContainer.isValid())
  {
    // this is allowed, there might be no AFP data in the input
    return StatusCode::SUCCESS;
  }
  

  SG::ReadHandle<xAOD::AFPProtonContainer> protonContainer( m_protonContainerKey, ctx );
  if(!protonContainer.isValid())
  {
    // this is allowed, there might be no AFP data in the input
    return StatusCode::SUCCESS;
  }


  // Select ToF tracks on side A
  std::vector<const xAOD::AFPToFTrack*> tofTrackSideAContainer;
  std::copy_if(tofTrackContainer->begin(), tofTrackContainer->end(), std::back_inserter(tofTrackSideAContainer),
      [](auto track) { return track->stationID() == 0; });
  if(tofTrackSideAContainer.empty()) return StatusCode::SUCCESS;
  
  // Select ToF tracks on side C      
  std::vector<const xAOD::AFPToFTrack*> tofTrackSideCContainer;
  std::copy_if(tofTrackContainer->begin(), tofTrackContainer->end(), std::back_inserter(tofTrackSideCContainer),
      [](auto track) { return track->stationID() == 3; });
  if(tofTrackSideCContainer.empty()) return StatusCode::SUCCESS;

  // Select protons on side A      
  std::vector<const xAOD::AFPProton*> protonSideAContainer;
  std::copy_if(protonContainer->begin(), protonContainer->end(), std::back_inserter(protonSideAContainer),
      [](auto proton) { return proton->side() == 0; });
  if(protonSideAContainer.empty()) return StatusCode::SUCCESS;
  
  // Select protons on side C
  std::vector<const xAOD::AFPProton*> protonSideCContainer;
  std::copy_if(protonContainer->begin(), protonContainer->end(), std::back_inserter(protonSideCContainer),
      [](auto proton) { return proton->side() == 1; });
  if(protonSideCContainer.empty()) return StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG("tofTrackSideAContainer size: " << tofTrackSideAContainer.size());
  ATH_MSG_DEBUG("tofTrackSideCContainer size: " << tofTrackSideCContainer.size());
  ATH_MSG_DEBUG("protonSideAContainer size: " << protonSideAContainer.size());
  ATH_MSG_DEBUG("protonSideCContainer size: " << protonSideCContainer.size());
  
  
  nlohmann::json dataTVP=m_tofVtxParamDBTool->parametersData(ctx);
  const AFP::ToFVtxParamData TVP_A=m_tofVtxParamDBTool->parameters(dataTVP, 0);
  const AFP::ToFVtxParamData TVP_C=m_tofVtxParamDBTool->parameters(dataTVP, 3);
  
  // Loop over four containers
  for (const xAOD::AFPToFTrack* tofTrackSideA : tofTrackSideAContainer) {

    for (const xAOD::AFPProton* protonSideA : protonSideAContainer) {
      // Apply cuts

      double protonXPositionFar = protonSideA->track(0)->xLocal();
      if(protonSideA->nTracks()==2 && protonSideA->track(1)->stationID()==0) 
                                   protonXPositionFar = protonSideA->track(1)->xLocal(); 
      
      double dx = std::min(std::abs(protonXPositionFar-TVP_A.trainEdge(tofTrackSideA->trainID())),std::abs(protonXPositionFar-TVP_A.trainEdge(tofTrackSideA->trainID()+1))); 
      double distA = dx;
      if( protonXPositionFar > TVP_A.trainEdge(tofTrackSideA->trainID()) && protonXPositionFar < TVP_A.trainEdge(tofTrackSideA->trainID()+1) ) distA = -dx;

      if (distA > m_trackDistance) {
        ATH_MSG_DEBUG(
            "Tracks too far away from each other (xProton, xLeftPositionSideA; xRightPositionSideA; distance) [mm]: "
            << protonXPositionFar << ", " << TVP_A.trainEdge(tofTrackSideA->trainID()) << "; "
            << TVP_A.trainEdge(tofTrackSideA->trainID()+1)  << ", " << distA  << "; " << distA);

        continue;
      }



     for (const xAOD::AFPToFTrack* tofTrackSideC : tofTrackSideCContainer) {

       for (const xAOD::AFPProton* protonSideC : protonSideCContainer) {
         // Apply cuts

         double protonXPositionFar = protonSideC->track(0)->xLocal();
         if(protonSideC->nTracks()==2 && protonSideC->track(1)->stationID()==3) 
                                   protonXPositionFar = protonSideC->track(1)->xLocal();
         double dx = std::min(std::abs(protonXPositionFar-TVP_C.trainEdge(tofTrackSideC->trainID())),std::abs(protonXPositionFar-TVP_C.trainEdge(tofTrackSideC->trainID()+1)));
         double distC = dx;
         if( protonXPositionFar > TVP_C.trainEdge(tofTrackSideC->trainID()) && protonXPositionFar < TVP_C.trainEdge(tofTrackSideC->trainID()+1) ) distC = -dx;

         if (distC > m_trackDistance) {
            ATH_MSG_DEBUG(
              "Tracks too far away from each other (xProton, xLeftPositionSideC; xRightPositionSideC; distance) [mm]: "
              << protonXPositionFar << ", " << TVP_C.trainEdge(tofTrackSideC->trainID()) << "; "
              << TVP_C.trainEdge(tofTrackSideC->trainID()+1)  << ", " << distC  << "; " << distC);

           continue;
         }

           // Reconstruct vertex and add it to the container
           xAOD::AFPVertex * vertex = reco(distA, distC, tofTrackSideA, tofTrackSideC, protonSideA, protonSideC, TVP_A, TVP_C, outputContainer);

           if (!vertex)
              continue;


           // Create link to tracks
           linkToFTracksToVertex(tofTrackSideA, tofTrackContainer, vertex);
           linkToFTracksToVertex(tofTrackSideC, tofTrackContainer, vertex);
           linkProtonsToVertex(protonSideA, protonContainer,vertex);
           linkProtonsToVertex(protonSideC, protonContainer,vertex);
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}


xAOD::AFPVertex * AFP_VertexRecoBasic::createVertex(const double position, const double distA, const double distC, std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer) const 
{

  auto * vertex = outputContainer->push_back(std::make_unique<xAOD::AFPVertex>());
  
  // Set vertex properties

  vertex->setPosition(position);
  vertex->setDistA(distA);
  vertex->setDistC(distC);
  vertex->setAlgID(0);
  
  ATH_MSG_DEBUG("Reconstructed  AFP vertex (position): " << vertex->position() <<", distA "<<vertex->distA()<<", distC "<<vertex->distC());

  return vertex;
}


void AFP_VertexRecoBasic::linkToFTracksToVertex
  (const xAOD::AFPToFTrack* tofTrack, SG::ReadHandle<xAOD::AFPToFTrackContainer>& tofTrackContainer, xAOD::AFPVertex * vertex) const {

  ElementLink<xAOD::AFPToFTrackContainer> tofTrackLink;

  tofTrackLink.toContainedElement(*tofTrackContainer, tofTrack);
  vertex->addToFTrack(tofTrackLink);
}

void AFP_VertexRecoBasic::linkProtonsToVertex
  (const xAOD::AFPProton* proton, SG::ReadHandle<xAOD::AFPProtonContainer>& protonContainer, xAOD::AFPVertex * vertex) const {

  ElementLink<xAOD::AFPProtonContainer> protonLink;

  protonLink.toContainedElement(*protonContainer, proton);
  vertex->addProton(protonLink);
}




xAOD::AFPVertex * AFP_VertexRecoBasic::reco(const double distA, const double distC,
  const xAOD::AFPToFTrack* tofTrackSideA, const xAOD::AFPToFTrack* tofTrackSideC, 
  const xAOD::AFPProton* protonSideA, const xAOD::AFPProton* protonSideC,
  const AFP::ToFVtxParamData& TVP_A, const AFP::ToFVtxParamData& TVP_C,
  std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer) const 
{
  int trainID = tofTrackSideA->trainID();
  double protonYPositionFar = protonSideA->track(0)->yLocal();
  if(protonSideA->nTracks()==2 && protonSideA->track(1)->stationID()==0)
    protonYPositionFar = protonSideA->track(1)->yLocal();
  double timeA = tofTrackSideA->trainTime() - (TVP_A.timeOffset(trainID)+TVP_A.timeSlope(trainID)*protonYPositionFar);

  trainID = tofTrackSideC->trainID();
  protonYPositionFar = protonSideC->track(0)->yLocal();
  if(protonSideC->nTracks()==2 && protonSideC->track(1)->stationID()==3)
    protonYPositionFar = protonSideC->track(1)->yLocal();

  double timeC = tofTrackSideC->trainTime() - (TVP_C.timeOffset(trainID)+TVP_C.timeSlope(trainID)*protonYPositionFar);

  double position = (timeC-timeA)/2.*0.299792458 - (TVP_C.timeGlobalOffset()-TVP_A.timeGlobalOffset()); 

  return createVertex(position, distA, distC, outputContainer);
}



