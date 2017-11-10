/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_PRIVXFINDER_VXPRIMARYALLTE_H
#define TRIGFTK_PRIVXFINDER_VXPRIMARYALLTE_H

#include "GaudiKernel/ServiceHandle.h"

//!< Trigger specific stuff
#include "TrigInterfaces/AllTEAlgo.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"
#include <vector>

/** FTK Primary Vertex Finder.  Gets Vertices from FTK_DataProviderSvc */



  class IVertexFinder;

  class TrigFTK_VxPrimaryAllTE : public HLT::AllTEAlgo
  {
  public:
    TrigFTK_VxPrimaryAllTE(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigFTK_VxPrimaryAllTE();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&, unsigned int output);    
    HLT::ErrorCode hltFinalize();
    
  private:
    int m_nTracks;
    int m_nVertices;
    std::vector<int> m_VertexType;
    std::vector<int> m_nTracksPriVtx;
    std::vector<int> m_nTracksPileUp;

    std::vector<float>   m_zOfPriVtx;
    std::vector<float>   m_zOfPileUp;
    std::vector<float>   m_zOfNoVtx;
    
    ServiceHandle<IFTK_DataProviderSvc> m_DataProviderSvc;
    bool m_useRawTracks;
    bool m_useRefittedTracks;
    ftk::FTK_TrackType m_trackType;
    std::string m_vertexContainerName;
    std::string m_vxContainerName;
    bool m_useFastVertexTool;
  };

#endif
