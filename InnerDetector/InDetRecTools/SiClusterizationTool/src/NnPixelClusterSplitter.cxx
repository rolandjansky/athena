/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : NnClusterizationFactory.cxx
/// Package : SiClusterizationTool 
/// Author  : Andi Salzburger (CERN PH-ADP)
///           Giacinto Piacquadio (PH-ADE-ID)
/// Created : January 2011
///
/// DESCRIPTION: Split cluster in sub-clusters,
///              given a certain PixelClusterSplitProb
///              If no PixelClusterSplitProb is given
///              split cluster in 1,2 and 3 sub-clusters 
///              and store all results.
///////////////////////////////////////////////////////////////////////////////////////////////////////


#include "SiClusterizationTool/NnPixelClusterSplitter.h"
#include "VxVertex/RecVertex.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"




InDet::NnPixelClusterSplitter::NnPixelClusterSplitter(const std::string &type,
        const std::string &name,
        const IInterface *parent) :
        base_class(type,name,parent)
{
}

StatusCode InDet::NnPixelClusterSplitter::initialize() {
    
  if (m_NnClusterizationFactory.retrieve().isFailure())
  {
    ATH_MSG_ERROR(" Unable to retrieve "<< m_NnClusterizationFactory );
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK(m_beamSpotKey.initialize());
  
  ATH_MSG_INFO(" Cluster splitter initialized successfully "<< m_NnClusterizationFactory );
  return StatusCode::SUCCESS;
}

StatusCode InDet::NnPixelClusterSplitter::finalize() {
    return StatusCode::SUCCESS;
}

/* default method which simply splits cluster into 2 */
std::vector<InDet::PixelClusterParts> InDet::NnPixelClusterSplitter::splitCluster(const InDet::PixelCluster& origCluster ) const
{

  //add treatment for b-layer only HERE


  const std::vector<Identifier>& rdos  = origCluster.rdoList();  
  const std::vector<int>&  totList     = origCluster.totList();

  //fill lvl1group all with the same value... (not best way but ...)
  std::vector<int> lvl1group(rdos.size(),origCluster.LVL1A());
  
  

  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>       allErrorMatrix;

  
  std::vector<Amg::MatrixX> errorMatrix;

  Amg::Vector3D beamSpotPosition(0,0,0);
  if(m_useBeamSpotInfo){
     SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
     beamSpotPosition = beamSpotHandle->beamPos();
  }

  std::vector<Amg::Vector2D> localPosition=m_NnClusterizationFactory->estimatePositions(origCluster,
                                                                                             beamSpotPosition,
                                                                                             errorMatrix,
                                                                                             2);



  if (errorMatrix.size()!=2 || localPosition.size()!=2)
  {
    ATH_MSG_WARNING("Error matrix or local position vector size is not 2, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
  }

  std::vector<InDet::PixelClusterParts> allMultiPClusters;
  
  
  allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
  allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[1],errorMatrix[1]);
 
  
  return allMultiPClusters;

}

std::vector<InDet::PixelClusterParts> InDet::NnPixelClusterSplitter::splitCluster(const InDet::PixelCluster& origCluster, 
                                                                                  const InDet::PixelClusterSplitProb& splitProb) const
{


  if (m_splitOnlyOnBLayer)
  {
    const InDetDD::SiDetectorElement* element=origCluster.detectorElement();
    if (element==nullptr) {
      ATH_MSG_WARNING("Could not get detector element");
      return {};
    }
    const AtlasDetectorID* aid = element->getIdHelper();
    if (aid==nullptr)
    {
      ATH_MSG_WARNING("Could not get ATLASDetectorID");
      return {};
    }
    
    if (aid->helper() != AtlasDetectorID::HelperType::Pixel)
    {
      ATH_MSG_WARNING("Not a PixelID helper");
      return {};
    } 
    const PixelID* pixelIDp=static_cast<const PixelID*>(aid);
    //check if original pixel is on b-layer and if yes continue, otherwise interrupt...
    Identifier pixelId = origCluster.identify();
    if (!pixelIDp->is_blayer(pixelId))
    {
      //return empty object...
      ATH_MSG_VERBOSE(" Cluster not on b-layer. Return empty object-->back to default clustering." );
      
      return {};
    }
  }

  //add treatment for b-layer only HERE

  const std::vector<Identifier>& rdos  = origCluster.rdoList();  
  const std::vector<int>&  totList     = origCluster.totList();

  //fill lvl1group all with the same value... (not best way but ...)
  std::vector<int> lvl1group(rdos.size(), origCluster.LVL1A());
 
  

  if (splitProb.getHighestSplitMultiplicityStored()<3) return {};

  double splitProb2=splitProb.splitProbability(2);
  double splitProb3rel=splitProb.splitProbability(3);

  double splitProb3=splitProb3rel/(splitProb3rel+splitProb2);

  ATH_MSG_VERBOSE( " SplitProb -->2 " << splitProb2 << " SplitProb -->3 " << splitProb3 );

  int nParticles=1;
  
  if (splitProb2>m_thresholdSplittingIntoTwoClusters)
  {
    if (splitProb3>m_thresholdSplittingIntoThreeClusters)
    {
      nParticles=3;
    }
    else
    {
      nParticles=2;
    }
  }


  ATH_MSG_VERBOSE( " Decided for n. particles: " << nParticles << "." );

  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>       allErrorMatrix;

  Amg::Vector3D beamSpotPosition(0,0,0);
  if(m_useBeamSpotInfo){
     SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
     beamSpotPosition = beamSpotHandle->beamPos();
  }
  
  std::vector<InDet::PixelClusterParts> allMultiPClusters;
  
  if (nParticles==1)
  {
    std::vector<Amg::MatrixX> errorMatrix;
    std::vector<Amg::Vector2D> localPosition=m_NnClusterizationFactory->estimatePositions(origCluster,
                                                                                               beamSpotPosition,
                                                                                               errorMatrix,
                                                                                               1);

    if (errorMatrix.size()!=1 || localPosition.size()!=1)
    {
      ATH_MSG_ERROR("Error matrix or local position vector size is not 1, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
    }
    
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
  }
  else if (nParticles==2)
  {

    std::vector<Amg::MatrixX> errorMatrix;
    std::vector<Amg::Vector2D> localPosition=m_NnClusterizationFactory->estimatePositions(origCluster,
                                                                                               beamSpotPosition,
                                                                                               errorMatrix,
                                                                                               2);
    
    if (errorMatrix.size()!=2 || localPosition.size()!=2)
    {
      ATH_MSG_ERROR("Error matrix or local position vector size is not 2, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
    }
    
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[1],errorMatrix[1]);
  }
  else if (nParticles==3)
  {

    std::vector<Amg::MatrixX> errorMatrix;
    std::vector<Amg::Vector2D> localPosition=m_NnClusterizationFactory->estimatePositions(origCluster,
                                                                                               beamSpotPosition,
                                                                                               errorMatrix,
                                                                                               3);
    
    if (errorMatrix.size()!=3 || localPosition.size()!=3)
    {
      ATH_MSG_ERROR("Error matrix or local position vector size is not 2, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
    }
    
    
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[1],errorMatrix[1]);
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[2],errorMatrix[2]);
  }
  
  return allMultiPClusters;

}
