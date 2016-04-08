/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////////
/// Author: Peter V. Loscutoff
/// February 2009
/////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ITHistSvc.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "GeneratorObjects/McEventCollection.h"

#include "xAODEventInfo/EventInfo.h"

/// head file
#include "InDetVertexSplitterHist.h"

#include <algorithm>
#include <math.h>
#include <sstream>
#include "TFitter.h"

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

InDet::InDetVertexSplitterHist::InDetVertexSplitterHist(const std::string& name,
  ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator), 
  m_thistSvc("THistSvc",name),
  m_ntuple(0)
{
  declareProperty("SpitVxContainer",m_splitVxName = "SplitVxCandidates");
  declareProperty("HistService",m_thistSvc);

}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

InDet::InDetVertexSplitterHist::~InDetVertexSplitterHist() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize

StatusCode InDet::InDetVertexSplitterHist::initialize() {
  ATH_MSG_DEBUG("Initializing InDetVertexSplitterHist");

  StatusCode sc = StatusCode::SUCCESS;

  m_ntuple = new TTree("splitVertex","Split Vertices");

  m_ntuple->Branch("odd",&m_oddBranch,"tracks_odd/I:x_odd/F:y_odd:z_odd:c00_odd:c01_odd:c11_odd:c22_odd");
  m_ntuple->Branch("even",&m_evenBranch,"tracks_even/I:x_even/F:y_even:z_even:c00_even:c01_even:c11_even:c22_even");
  m_ntuple->Branch("meta",&m_metaData,"run/I:lumi:event");
  sc = m_thistSvc->regTree("/AANT/ntuple",m_ntuple);
  
  return StatusCode::SUCCESS;
} 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode InDet::InDetVertexSplitterHist::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode InDet::InDetVertexSplitterHist::execute() {

  ATH_MSG_DEBUG("in execute()");

  StatusCode sc = StatusCode::SUCCESS;

  sc = makeSplitHist();
  if (sc.isFailure()) {
     return sc;
  }

  return sc;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode InDet::InDetVertexSplitterHist::makeSplitHist() {
  ATH_MSG_DEBUG("in makeSplitHist()");

  StatusCode sc = StatusCode::SUCCESS;

  const VxContainer* vxCont = 0;
  sc = evtStore()->retrieve(vxCont, m_splitVxName);
  if(sc.isSuccess()){
    ATH_MSG_DEBUG("Retrieved vertex container " << m_splitVxName);
  }
  else{
    ATH_MSG_WARNING("Failed to retrieve vertex container " << m_splitVxName);
    return StatusCode::FAILURE;
  }

  int vcsize = ((int)vxCont->size() - 1)/2;
  int vxi = 0;
  while ( vxi < vcsize){
    const Trk::VxCandidate *vxContO = vxCont->at(2*vxi);
    const Trk::VxCandidate *vxContE = vxCont->at(2*vxi+1);
    if( vxContO->vxTrackAtVertex()->empty() or vxContE->vxTrackAtVertex()->empty())
      {ATH_MSG_DEBUG("Found a vertex with no tracks, skipping it");}
    else{
      float xo = vxContO->recVertex().position().x();
      float xeo = vxContO->recVertex().covariancePosition()(0,0);
      float yo = vxContO->recVertex().position().y();
      float yeo = vxContO->recVertex().covariancePosition()(1,1);
      float zo = vxContO->recVertex().position().z();
      float zeo =vxContO->recVertex().covariancePosition()(2,2);
      float cxyo = vxContO->recVertex().covariancePosition()(0,1);
      int so = vxContO->vxTrackAtVertex()->size();
      
      float xe = vxContE->recVertex().position().x();
      float xee = vxContE->recVertex().covariancePosition()(0,0);
      float ye = vxContE->recVertex().position().y();
      float yee = vxContE->recVertex().covariancePosition()(1,1);
      float ze = vxContE->recVertex().position().z();
      float zee =vxContE->recVertex().covariancePosition()(2,2);
      float cxye = vxContE->recVertex().covariancePosition()(0,1);
      int se = vxContE->vxTrackAtVertex()->size();
      
      m_oddBranch.size=so;
      m_oddBranch.x=xo;
      m_oddBranch.y=yo;
      m_oddBranch.z=zo;
      m_oddBranch.c00=xeo;
      m_oddBranch.c01=cxyo;
      m_oddBranch.c11=yeo;
      m_oddBranch.c22=zeo;
      
      m_evenBranch.size=se;
      m_evenBranch.x=xe;
      m_evenBranch.y=ye;
      m_evenBranch.z=ze;
      m_evenBranch.c00=xee;
      m_evenBranch.c01=cxye;
      m_evenBranch.c11=yee;
      m_evenBranch.c22=zee;
      
      const DataHandle<xAOD::EventInfo> eventInfo;
      sc = evtStore()->retrieve( eventInfo );
      if(sc.isSuccess()){
        m_metaData.lumi = eventInfo->lumiBlock();
        m_metaData.run = eventInfo->runNumber();
        m_metaData.event = eventInfo->eventNumber();
      }
      else{
        return sc;
      }
      m_ntuple->Fill(); 
    }
    vxi++;
  }  
  return StatusCode::SUCCESS;
}

