/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : NnPixelClusterSplitProbTool.cxx
/// Package : SiClusterizationTool 
/// Author  : Giacinto Piacquadio (PH-ADE-ID)
/// Created : January 2011
///
/// DESCRIPTION: Compute cluster splitting probabilities 
/// (for splitting a cluster into  2 .... N subclusters)
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/NnPixelClusterSplitProbTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "VxVertex/RecVertex.h"



namespace InDet
{
  

  NnPixelClusterSplitProbTool::NnPixelClusterSplitProbTool(const std::string& t, const std::string& n, const IInterface*  p)
          :base_class(t,n,p)
  {
  }


  StatusCode NnPixelClusterSplitProbTool::initialize()
  {
    
 
    if (m_NnClusterizationFactory.retrieve().isFailure())
    {
      ATH_MSG_ERROR(" Unable to retrieve "<< m_NnClusterizationFactory );
      return StatusCode::FAILURE;
    }

    ATH_CHECK(m_beamSpotKey.initialize());

    ATH_MSG_INFO(" Cluster split prob tool initialized successfully "<< m_NnClusterizationFactory );
    return StatusCode::SUCCESS;
  }

  
  InDet::PixelClusterSplitProb NnPixelClusterSplitProbTool::splitProbability(const InDet::PixelCluster& origCluster ) const
  {

    Amg::Vector3D beamSpotPosition(0,0,0);
    if(m_useBeamSpotInfo){
       SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
       beamSpotPosition = beamSpotHandle->beamPos();
    }

    std::vector<double> vectorOfProbs=m_NnClusterizationFactory->estimateNumberOfParticles(origCluster,beamSpotPosition);

    ATH_MSG_VERBOSE(" Got splitProbability, size of vector: " << vectorOfProbs.size() );

    if (vectorOfProbs.empty())
    {
      std::vector<double> vectorOfSplitProbs;
      vectorOfSplitProbs.push_back(-100);
      PixelClusterSplitProb  clusterSplitProb(vectorOfSplitProbs);
      ATH_MSG_VERBOSE(" Returning single split prob equal to -100 " );
      return clusterSplitProb;
    }
     

    ATH_MSG_VERBOSE( 
        " P(1): " << vectorOfProbs[0] << 
        " P(2): " << vectorOfProbs[1] << 
        " P(>=3): " << vectorOfProbs[2] );


    return compileSplitProbability(vectorOfProbs); 
  }

  InDet::PixelClusterSplitProb NnPixelClusterSplitProbTool::splitProbability(const InDet::PixelCluster& origCluster, const Trk::TrackParameters& trackParameters ) const
  {
    
    Amg::Vector3D beamSpotPosition(0,0,0);
    if(m_useBeamSpotInfo){
       SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
       beamSpotPosition = beamSpotHandle->beamPos();
    }

    std::vector<double> vectorOfProbs=m_NnClusterizationFactory->estimateNumberOfParticles(origCluster, trackParameters.associatedSurface(), trackParameters);

    ATH_MSG_VERBOSE(" Got splitProbability, size of vector: " << vectorOfProbs.size() );

    if (vectorOfProbs.empty())
    {
      std::vector<double> vectorOfSplitProbs;
      vectorOfSplitProbs.push_back(-100);
      PixelClusterSplitProb  clusterSplitProb(vectorOfSplitProbs);
      ATH_MSG_VERBOSE(" Returning single split prob equal to -100 " );
      return clusterSplitProb;
    }
     

    ATH_MSG_VERBOSE( 
        " P(1): " << vectorOfProbs[0] << 
        " P(2): " << vectorOfProbs[1] << 
        " P(>=3): " << vectorOfProbs[2] );


    return compileSplitProbability(vectorOfProbs); 
  }



  InDet::PixelClusterSplitProb NnPixelClusterSplitProbTool::compileSplitProbability(std::vector<double>& vectorOfProbs ) const
  {


    double sum=0;

    std::vector<double>::iterator begin=vectorOfProbs.begin();
    std::vector<double>::iterator end=vectorOfProbs.end();

    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      sum+=*iter;
    }

    
    ATH_MSG_VERBOSE(" Sum of cluster probabilities is: "<<sum);

    std::vector<double> vectorOfSplitProbs;

    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      (*iter)/=sum;
    }

    if (m_priorMultiplicityContent.value().size()<vectorOfProbs.size())
    {
      ATH_MSG_ERROR("Prior compatibilities count " <<  m_priorMultiplicityContent.value().size() << " is too small: please correct through job properties.");
      return InDet::PixelClusterSplitProb(std::vector<double>());
    }

    double psum=0;
    int count=0;
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter,++count)
    {
      psum+=(*iter)/m_priorMultiplicityContent.value()[count];
    }

    count=0;
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter,++count)
    {
      (*iter)/=m_priorMultiplicityContent.value()[count];
      (*iter)/=psum;
    }

    double sumTest=0;

    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      ATH_MSG_VERBOSE("After update prob is: " << *iter);
      sumTest+=*iter;
    }

    ATH_MSG_VERBOSE(" Sum of cluster probabilities is: "<<sumTest);
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      if (iter!=begin)
      {
        vectorOfSplitProbs.push_back(*iter);
      }
    }

    ATH_MSG_VERBOSE(" normalized P(1->2): " << vectorOfSplitProbs[0] << " P(2->3): " << vectorOfSplitProbs[1] );

    PixelClusterSplitProb  clusterSplitProb(vectorOfSplitProbs);

    ATH_MSG_VERBOSE("SplitProb: " << clusterSplitProb.splitProbability(2) << " -->3 " << clusterSplitProb.splitProbability(3) );
    
    return clusterSplitProb;
  }



  
  
}//end namespace



