/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCosmicMuonFillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on November 2, 2011, 1:26 PM
 */

#include "TileCosmicMuonFillerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"

using namespace std;

namespace D3PD{

TileCosmicMuonFillerTool::TileCosmicMuonFillerTool(const string& type, 
        const string& name, const IInterface* parent):
        BlockFillerTool<TileCosmicMuon>(type,name,parent)
{
  TileCosmicMuonFillerTool::book().ignore(); // Avoid coverity warnings
}

TileCosmicMuonFillerTool::~TileCosmicMuonFillerTool() {
}

StatusCode TileCosmicMuonFillerTool::book(){
//    CHECK(addVariable("nTracks",          m_nTracks));
//    CHECK(addVariable("trackOfMaxE",      m_trackOfMaxE));
//    CHECK(addVariable("nSegments",        m_nSegments));
    
    CHECK(addVariable("x",                m_positionX));
    CHECK(addVariable("y",                m_positionY));
    CHECK(addVariable("z",                m_positionZ));
    CHECK(addVariable("phi",              m_directionPhi));
    CHECK(addVariable("theta",            m_directionTheta));
    CHECK(addVariable("time",             m_time));
    CHECK(addVariable("fitQuality",       m_fitQuality));
    CHECK(addVariable("fitNcells",        m_fitNCells));
    CHECK(addVariable("trackNcells",      m_trackNCells));
    CHECK(addVariable("path",             m_fullPath));
    CHECK(addVariable("energy",           m_fullEnergy));

    CHECK(addVariable("pathTopA",         m_pathTopA));
    CHECK(addVariable("pathTopBC",        m_pathTopBC));
    CHECK(addVariable("pathTopD",         m_pathTopD));
    CHECK(addVariable("pathBottomA",      m_pathBottomA));
    CHECK(addVariable("pathBottomBC",     m_pathBottomBC));
    CHECK(addVariable("pathBottomD",      m_pathBottomD));
    CHECK(addVariable("energyTopA",       m_energyTopA));
    CHECK(addVariable("energyTopBC",      m_energyTopBC));
    CHECK(addVariable("energyTopD",       m_energyTopD));
    CHECK(addVariable("energyBottomA",    m_energyBottomA));
    CHECK(addVariable("energyBottomBC",   m_energyBottomBC));
    CHECK(addVariable("energyBottomD",    m_energyBottomD));

    CHECK(addVariable("segmentPath",      m_segmentPath));
    CHECK(addVariable("segmentPartition", m_segmentPartition));
    CHECK(addVariable("segmentModule",    m_segmentModule));
    CHECK(addVariable("segmentSampling",  m_segmentSampling));
//    CHECK(addVariable("segmentTrack",     m_segmentTrack));
    
    return StatusCode::SUCCESS;
}

StatusCode TileCosmicMuonFillerTool::fill(const TileCosmicMuon& p){
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << " in TileCosmicMuonFillerTool::book()" << 
            endmsg;
    
    *m_positionX=p.GetPositionX();
    *m_positionY=p.GetPositionY();
    *m_positionZ=p.GetPositionZ();
    *m_directionPhi=p.GetDirectionPhi();        
    *m_directionTheta=p.GetDirectionTheta();
    *m_time=p.GetTime();
    *m_fitQuality=p.GetFitQuality();
    *m_fitNCells=p.GetFitNCells();
    
    if(p.GetNSamples()!=3){
        log << MSG::DEBUG << "Warning!: TileCosmicMuon with "<<
                p.GetNSamples()<<" samples."<<endmsg;
        *m_fullPath=0;
        *m_fullEnergy=0;
        *m_pathTopA=0;
        *m_pathTopBC=0;
        *m_pathTopD=0;
        *m_pathBottomA=0;
        *m_pathBottomBC=0;
        *m_pathBottomD=0;
        *m_energyTopA=0;
        *m_energyTopBC=0;
        *m_energyTopD=0;
        *m_energyBottomA=0;
        *m_energyBottomBC=0;
        *m_energyBottomD=0;
    }
    else{
        *m_fullPath=const_cast<TileCosmicMuon*>(&p)->GetFullPath();
        *m_fullEnergy=const_cast<TileCosmicMuon*>(&p)->GetFullEnergy();
        *m_pathTopA=p.GetPathTop(0);
        *m_pathTopBC=p.GetPathTop(1);
        *m_pathTopD=p.GetPathTop(2);
        *m_pathBottomA=p.GetPathBottom(0);
        *m_pathBottomBC=p.GetPathBottom(1);
        *m_pathBottomD=p.GetPathBottom(2);
        *m_energyTopA=p.GetEnergyTop(0);
        *m_energyTopBC=p.GetEnergyTop(1);
        *m_energyTopD=p.GetEnergyTop(2);
        *m_energyBottomA=p.GetEnergyBottom(0);
        *m_energyBottomBC=p.GetEnergyBottom(1);
        *m_energyBottomD=p.GetEnergyBottom(2);
    }
    
    for(int i=0;i<p.GetNSegments();++i){
        m_segmentPath->push_back(p.GetSegmentPath(i));
        m_segmentPartition->push_back(p.GetSegmentPartition(i));
        m_segmentModule->push_back(p.GetSegmentModule(i));
        m_segmentSampling->push_back(p.GetSegmentSampling(i));
    }
    
    return StatusCode::SUCCESS;
}

}
