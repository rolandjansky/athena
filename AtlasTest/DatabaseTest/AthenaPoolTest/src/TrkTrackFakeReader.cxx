/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file TrkTrackFakeReader.cxx
*
* @brief Test Algorithm for POOL I/O uses TrkTracks as test data
*
* @author Ed Moyse <Edward.Moyse@cern.ch>
* @author RD Schaffer <R.D.Schaffer@cern.ch>
*
* $Id: TrkTrackFakeReader.cxx,v 1.4 2006-09-17 15:21:23 schaffer Exp $
*
*/

// Silicon trackers includes
#include "TrkTrackFakeReader.h"

//InDetRawData includes
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"

//  #include "InDetRawData/InDetRawDataContainer.h"
//  #include "InDetRawData/InDetRawDataCollection.h"
//  #include "InDetRawData/Pixel1RawData.h"
//  #include "InDetRawData/InDetRawDataCLASS_DEF.h"
//  #include "InDetRawData/PixelRDORawData.h"
//  #include "InDetRawUtils/PixelHid2RESrcID.h"

// Id includes
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
//#include "InDetDetDescr/InDet_DetDescrManager.h"

// TES include
#include "StoreGate/StoreGateSvc.h"

// test includes
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiCluster.h"

#include <map>
#include "TrkTrackFakeWriter.h"

// Constructor with parameters:
TrkTrackFakeReader::TrkTrackFakeReader(const std::string &name, 
ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_pixMgr(nullptr)
  {}

// Initialize method:
StatusCode TrkTrackFakeReader::initialize()
{
  ATH_MSG_INFO( "TrkTrackFakeReader::initialize()"  );
  ATH_CHECK( detStore()->retrieve(m_pixMgr, m_pixMgrLocation) );
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode TrkTrackFakeReader::execute() 
{
  ATH_MSG_DEBUG( "TrkTrackFakeReader::execute()"  );

  using namespace Trk;
  using namespace InDet;

    //Get tracks
  const TrackCollection* tracks = nullptr;
  ATH_CHECK( evtStore()->retrieve(tracks,"Tracks") );

  for (TrackCollection::const_iterator it = tracks->begin(); it!=tracks->end(); ++it)
  {
      //compare tracks
    Trk::Track* track = FakeTrackBuilder::buildTrack(m_pixMgr);
      // compareTracks(track,*it);

    delete track;
  }
  return StatusCode::SUCCESS;
}


// Finalize method:
StatusCode TrkTrackFakeReader::finalize() 
{
  ATH_MSG_INFO( "TrkTrackFakeReader::finalize()"  );
  return StatusCode::SUCCESS;
}

void TrkTrackFakeReader::compareTracks(const Trk::Track* ref, const Trk::Track* readTrk) {
      // if (**it!=*track) {
      //   log<<MSG::ERROR<<"Tracks differ!"<<endreq;
      //   
      //   log<<MSG::INFO<<"Reference track: "<<(*track)<<endreq;
      //   log<<MSG::INFO<<"Track from ESD: "<<(**it)<<endreq;
      // }
  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  if ( !(ref->fitQuality () && readTrk->fitQuality() 
    && ref->fitQuality()->chiSquared()==readTrk->fitQuality ()->chiSquared()
  && ref->fitQuality()->numberDoF()==readTrk->fitQuality ()->numberDoF())) {
    ATH_MSG_WARNING( "Track fitqualities differ"  );
  } 
}

