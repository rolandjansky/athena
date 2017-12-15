/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
/**
// filename: TrigFTK_VxPrimary.h
//
// author: John Baines
//
// Description:  Get Primary Vertices from FTK_DataProviderSvc
//
// -------------------------------
// ATLAS Collaboration
*/
//***************************************************************************

#ifndef TRIGFTK_PRIVXFINDER_VXPRIMARY_H
#define TRIGFTK_PRIVXFINDER_VXPRIMARY_H

#include "GaudiKernel/ServiceHandle.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

#include <vector>




class TrigFTK_VxPrimary : public HLT::FexAlgo
{
 public:
    TrigFTK_VxPrimary(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigFTK_VxPrimary();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);    
    HLT::ErrorCode hltFinalize();
    
  private:

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
