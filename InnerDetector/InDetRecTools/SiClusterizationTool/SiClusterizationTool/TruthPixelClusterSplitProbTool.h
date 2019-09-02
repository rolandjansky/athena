/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//TruthPixelClusterSplitProbTool.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Estimate cluster split probability by using multivariate techniquew
///////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////
// @author Roland Jansky & Felix Cormier
//
///////////////////////////////////////////////////////////////////

#ifndef SiClusterizationTool_TruthPixelClusterSplitProbTool_H
#define SiClusterizationTool_TruthPixelClusterSplitProbTool_H


#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "TrkParameters/TrackParameters.h"


namespace InDet {

  class TruthClusterizationFactory;

  class TruthPixelClusterSplitProbTool : public extends<AthAlgTool, IPixelClusterSplitProbTool>
  {
  public:
    
    TruthPixelClusterSplitProbTool(const std::string& t, const std::string& n, const IInterface*  p);

    virtual ~TruthPixelClusterSplitProbTool() = default;
    
    StatusCode initialize();

    virtual InDet::PixelClusterSplitProb splitProbability(const InDet::PixelCluster& origCluster ) const;

    virtual InDet::PixelClusterSplitProb splitProbability(const InDet::PixelCluster& origCluster, const Trk::TrackParameters& trackParameters ) const;


  private:
    
    InDet::PixelClusterSplitProb compileSplitProbability(std::vector<double>& vectorOfProbs ) const;
    
    ToolHandle<TruthClusterizationFactory> m_truthClusterizationFactory{this, "NnClusterizationFactory", "InDet::NnClusterizationFactory/TruthClusterizationFactory"};

    DoubleArrayProperty m_priorMultiplicityContent{this, "PriorMultiplicityContent", {2793337, 82056, 19944}};

  };
  
}

#endif
