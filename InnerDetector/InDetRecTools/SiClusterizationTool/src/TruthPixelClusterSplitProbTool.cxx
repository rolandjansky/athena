/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TruthPixelClusterSplitProbTool.cxx
/// Package : SiClusterizationTool 
/// Author  : Roland Jansky & Felix Cormier
/// Created : April 2016
///
/// DESCRIPTION: Compute cluster splitting probabilities 
/// (for splitting a cluster into  2 .... N subclusters)
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/TruthPixelClusterSplitProbTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "SiClusterizationTool/TruthClusterizationFactory.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "VxVertex/RecVertex.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"



namespace InDet
{
  

  TruthPixelClusterSplitProbTool::TruthPixelClusterSplitProbTool(const std::string& t, const std::string& n, const IInterface*  p)
          :AthAlgTool(t,n,p),
           m_truthClusterizationFactory("InDet::NnClusterizationFactory/TruthClusterizationFactory", this),
           m_iBeamCondSvc("BeamCondSvc",n),
           m_useBeamSpotInfo(true)
  {

    m_priorMultiplicityContent.push_back(2793337);
    m_priorMultiplicityContent.push_back(82056);
    m_priorMultiplicityContent.push_back(19944);


    declareInterface<IPixelClusterSplitProbTool>(this);

    declareProperty("NnClusterizationFactory",m_truthClusterizationFactory);
    declareProperty("BeamCondSv",m_iBeamCondSvc);
    declareProperty("PriorMultiplicityContent",m_priorMultiplicityContent);
    declareProperty("useBeamSpotInfo",m_useBeamSpotInfo);


  }
  

  StatusCode TruthPixelClusterSplitProbTool::initialize()
  {
    
 
    if (m_truthClusterizationFactory.retrieve().isFailure())
    {
      ATH_MSG_ERROR(" Unable to retrieve "<< m_truthClusterizationFactory );
      return StatusCode::FAILURE;
    }

    if (m_iBeamCondSvc.retrieve().isFailure())
    {
      ATH_MSG_ERROR( "Could not find BeamCondSvc." );
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(" Cluster split prob tool initialized successfully "<< m_truthClusterizationFactory );
    return StatusCode::SUCCESS;
  }

  
  InDet::PixelClusterSplitProb TruthPixelClusterSplitProbTool::splitProbability(const InDet::PixelCluster& origCluster ) const
  {
    std::vector<double> vectorOfProbs=m_truthClusterizationFactory->estimateNumberOfParticles(origCluster);

    ATH_MSG_VERBOSE(" Got splitProbability, size of vector: " << vectorOfProbs.size() );

    if (vectorOfProbs.size()==0)
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

InDet::PixelClusterSplitProb TruthPixelClusterSplitProbTool::splitProbability(const InDet::PixelCluster& origCluster, const Trk::TrackParameters& /*trackParameters*/ ) const
  {
    std::vector<double> vectorOfProbs=m_truthClusterizationFactory->estimateNumberOfParticles(origCluster);

    ATH_MSG_VERBOSE(" Got splitProbability, size of vector: " << vectorOfProbs.size() );

    if (vectorOfProbs.size()==0)
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



  InDet::PixelClusterSplitProb TruthPixelClusterSplitProbTool::compileSplitProbability(std::vector<double>& vectorOfProbs ) const
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

    if (m_priorMultiplicityContent.size()<vectorOfProbs.size())
    {
      ATH_MSG_ERROR("Prior compatibilities count " <<  m_priorMultiplicityContent.size() << " is too small: please correct through job properties.");
      return InDet::PixelClusterSplitProb(std::vector<double>());
    }

    double psum=0;
    int count=0;
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter,++count)
    {
      psum+=(*iter)/m_priorMultiplicityContent[count];
    }

    count=0;
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter,++count)
    {
      (*iter)/=m_priorMultiplicityContent[count];
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



