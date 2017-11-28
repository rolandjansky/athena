/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   AthTileTripReader.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on August 21, 2012, 11:32 AM
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"
#include "EventKernel/INavigable4Momentum.h"
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/MsgStream.h"

#include "TSystem.h"

#include "TileTripReader/AthTileTripReader.h"

using namespace std;
using namespace Root;
using xAOD::EventInfo;

AthTileTripReader::AthTileTripReader(const std::string& type, 
        const std::string& name, 
        const IInterface* parent): AthAlgTool(type,name,parent) {
    
    m_tripReader=new Root::TTileTripReader;
    declareInterface<IAthSelectorTool>(this);
    declareInterface<AthTileTripReader>(this);
    
    declareProperty("TileTripFile",
            m_tripFile="TileTripReader/CompleteTripList_2011-2012.root",
            "Path to the trip list file");
    
    declareProperty("dR",m_dR=0.4);
        
}


AthTileTripReader::~AthTileTripReader() {
    if(m_tripReader)delete m_tripReader;
}


StatusCode AthTileTripReader::initialize(){
    MsgStream mslog( msgSvc(), name() );
    const char* fname;
    if(m_tripFile.find("/")==0 || m_tripFile.find("$")==0 || 
            m_tripFile.find(".")==0 || m_tripFile.find(":")!=string::npos){
        fname = gSystem->ExpandPathName( m_tripFile.c_str() );
    }
    else{
        m_tripFile = PathResolverFindXMLFile( m_tripFile );
        if ( m_tripFile.empty() ) 
            ATH_MSG_WARNING("Could NOT resolve file name "<<m_tripFile);
        fname=m_tripFile.data();
    }
    
    if(!m_tripReader->setTripFile(fname)){
        ATH_MSG_ERROR("ERROR! No trip file loaded!");
        return StatusCode::FAILURE;
    }
    MSG::Level cLevel=mslog.currentLevel();
    if(cLevel<=MSG::DEBUG)
        m_tripReader->setVerbosity(1);
    else m_tripReader->setVerbosity(0);
    m_tripReader->initialize();
    return StatusCode::SUCCESS;
}

StatusCode AthTileTripReader::finalize(){
    if(!m_tripReader->finalize()){
        ATH_MSG_ERROR ( "ERROR! Something went wrong at finalize!" );
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

const TAccept& AthTileTripReader::accept(const INavigable4Momentum* part){
    const EventInfo* evt=0;
    StatusCode sc=evtStore()->retrieve(evt,"ByteStreamEventInfo");
    if(sc.isFailure()){
        ATH_MSG_ERROR ( "ERROR! Could not retrieve event info from store gate!");
        return m_acceptDummy;
    }
    if(!part) return m_tripReader->accept(evt->runNumber(),
					  evt->lumiBlock(),
					  evt->eventNumber());
    
    int tileFlags=evt->eventFlags(EventInfo::Tile);
    int tileError=evt->errorState(EventInfo::Tile);
    return m_tripReader->accept(evt->runNumber(),
				evt->lumiBlock(),
				evt->eventNumber(),
				part->eta(),
				part->phi(),
				tileError,
				tileFlags);
}

const TResult& AthTileTripReader::calculate(const INavigable4Momentum* part){
    const EventInfo* evt=0;
    StatusCode sc=evtStore()->retrieve(evt,"ByteStreamEventInfo");
    if(sc.isFailure()){
        ATH_MSG_ERROR ( "ERROR! Could not retrieve event info from store gate!");
        return m_resultDummy;
    }
    
    int tileFlags=evt->eventFlags(EventInfo::Tile);
    int tileError=evt->errorState(EventInfo::Tile);
    return m_tripReader->calculate(evt->runNumber(),
				   evt->lumiBlock(),
				   part->eta(),
				   part->phi(),
				   m_dR,
				   tileError,
				   tileFlags);
}

bool AthTileTripReader::checkEvent(){
    const EventInfo* evt=0;
    StatusCode sc=evtStore()->retrieve(evt,"ByteStreamEventInfo");
    if(sc.isFailure()){
        ATH_MSG_ERROR ( "ERROR! Could not retrieve event info from store gate!");
        return m_resultDummy;
    }

    return m_tripReader->checkEvent(evt->runNumber(),
				    evt->lumiBlock(),
				    evt->eventNumber());
    
}
