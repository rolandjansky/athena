/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETVERTEXSPLITTERHIST_H
#define INDET_INDETVERTEXSPLITTERHIST_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : InDetVertexSplitterHist.h
/// Package : offline/PhysicsAnalysis/AnalysisCommon/AnalysisExamples
/// Author  : Peter V. Loscutoff
/// Created : February 2009
///
/// DESCRIPTION:
///
/// This class is used to plot intersting quantities from a vertex that has been split in two
/// 
/// Peter V. Loscutoff, Februarry 2009
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h" 

#include <string>
#include <vector>
#include <map>
#include "TTree.h"

namespace InDet {

  class InDetVertexSplitterHist : public AthAlgorithm {
    
  public:
    
    InDetVertexSplitterHist(const std::string& name, ISvcLocator* pSvcLocator);
    ~InDetVertexSplitterHist();
    
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
    
  private:
    
    StatusCode makeSplitHist();
    
    /// get a handle on the Hist/TTree registration service
    ServiceHandle<ITHistSvc> m_thistSvc;
 
    std::string m_splitVxName;

    TTree * m_ntuple;
        
    struct vertexBranch{
      vertexBranch():size(0),x(0.),y(0.),z(0.),c00(0.),c01(0.),c11(0.),c22(0.){}
      int size;
      float x,y,z,c00,c01,c11,c22;
    };
    struct metaData{
      metaData():run(0),lumi(0),event(0){}
      int run,lumi,event;
    };
    
    vertexBranch m_oddBranch;
    vertexBranch m_evenBranch;
    metaData m_metaData;
    
  };

}
#endif // INDET_INDETVERTEXSPLITTERHIST_H

