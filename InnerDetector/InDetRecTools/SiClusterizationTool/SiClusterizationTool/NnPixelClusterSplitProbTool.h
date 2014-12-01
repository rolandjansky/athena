/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//NnPixelClusterSplitProbTool.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Estimate cluster split probability by using multivariate techniquew
///////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////
// @author Giacinto Piacquadio (CERN PH-ADE-ID)
//
///////////////////////////////////////////////////////////////////

#ifndef SiClusterizationTool_NnPixelClusterSplitProbTool_H
#define SiClusterizationTool_NnPixelClusterSplitProbTool_H


#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
//Beam Spot Condition
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkParameters/TrackParameters.h"

class IBeamCondSvc;

namespace InDet {

  class NnClusterizationFactory;

  class NnPixelClusterSplitProbTool : public AthAlgTool, virtual public IPixelClusterSplitProbTool
  {
  public:
    
    NnPixelClusterSplitProbTool(const std::string& t, const std::string& n, const IInterface*  p);

    virtual ~NnPixelClusterSplitProbTool() {};
    
    StatusCode initialize();

    virtual InDet::PixelClusterSplitProb splitProbability(const InDet::PixelCluster& origCluster ) const;

    virtual InDet::PixelClusterSplitProb splitProbability(const InDet::PixelCluster& origCluster, const Trk::TrackParameters& trackParameters ) const;


  private:
    
    InDet::PixelClusterSplitProb compileSplitProbability(std::vector<double>& vectorOfProbs ) const;
    
    ToolHandle<NnClusterizationFactory> m_NnClusterizationFactory;
    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;

    std::vector<double> m_priorMultiplicityContent;

    bool m_useBeamSpotInfo;

  };
  
}


#endif
    
