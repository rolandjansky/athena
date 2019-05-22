/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_SGTrackOutput.h"
#include "TrigFTKSim/FTKSetup.h"

#include <sstream>
using namespace std;

FTK_SGTrackOutput::FTK_SGTrackOutput(const std::string& algname, const std::string &name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc),
  m_nbanks(0),
  m_tracks(0x0), m_tracks_pre_hw(0x0), m_TFstats(0x0),
  m_storeIncomplete(false),
  m_tracksI(0x0), m_tracksI_pre_hw(0x0), m_TFstatsI(0x0),
  m_tracks_pattern(0x0), m_tracks_hits(0x0),
  m_log( msgSvc() , name )
{
  declareInterface<FTK_SGTrackOutputI>(this);

  declareProperty("StoreIncomplete",m_storeIncomplete,"Store incomplete tracks information");
}

FTK_SGTrackOutput::~FTK_SGTrackOutput()
{
}

void FTK_SGTrackOutput::notValid(const char *method)
{
  FTKSetup::PrintMessageFmt(ftk::info,"Method %s not implemented in FTK_SGRoadOutput",method);
}


StatusCode FTK_SGTrackOutput::initialize()
{

  m_log << MSG::INFO << "FTK_SGTrackOutput::initialize()" << endmsg;
  
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    m_log << MSG::FATAL << "StoreGate service not found" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode FTK_SGTrackOutput::finalize()
{
  m_log << MSG::INFO << "FTK_SGTrackOutput::finalize()" << endmsg;

  if (m_tracks) delete [] m_tracks;
  if (m_tracks_pre_hw) delete [] m_tracks_pre_hw;
  if (m_TFstats) delete [] m_TFstats;
  if (m_tracksI) delete [] m_tracksI;
  if (m_tracksI_pre_hw) delete [] m_tracksI_pre_hw;
  if (m_TFstatsI) delete [] m_TFstatsI;
  if (m_tracks_pattern) delete [] m_tracks_pattern;
  if (m_tracks_hits) delete [] m_tracks_hits;

  return StatusCode::SUCCESS;
}


/** This method prepares the structure used to save the results in the
    StoreGate */
void FTK_SGTrackOutput::init(int, bool*)
{
  // create the array of track arrays
  m_tracks = new FTKAthTrackContainer*[m_nbanks];
  // create the array of track arrays before Hit Warrior
  m_tracks_pre_hw = new FTKAthTrackContainer*[m_nbanks];
  // create an array of TF stats
  m_TFstats = new FTKTrackFitterStats*[m_nbanks];
  // create the array of track arrays
  m_tracksI = new FTKAthTrackContainer*[m_nbanks];
  // create the array of track arrays before Hit Warrior
  m_tracksI_pre_hw = new FTKAthTrackContainer*[m_nbanks];
  // create an array of TF stats
  m_TFstatsI = new FTKTrackFitterStats*[m_nbanks];
  // create the array of track arrays with pattern
  m_tracks_pattern = new FTKAthTrackContainer*[m_nbanks];
  // create the array of track arrays that pass the hits requirements
  m_tracks_hits = new FTKAthTrackContainer*[m_nbanks];
}


void FTK_SGTrackOutput::beginFile(const char*)
{
}

void FTK_SGTrackOutput::eventBegin() 
{
  /* At the begin of each event a new container is created, the memory
     should be freed by the SG  */
  for (int ib=0;ib!=m_nbanks;++ib) {
    m_tracks[ib] = new FTKAthTrackContainer();
    m_tracks_pre_hw[ib] = new FTKAthTrackContainer();
    m_TFstats[ib] = new FTKTrackFitterStats();
    m_tracksI[ib] = new FTKAthTrackContainer();
    m_tracksI_pre_hw[ib] = new FTKAthTrackContainer();
    m_TFstatsI[ib] = new FTKTrackFitterStats();
    m_tracks_pattern[ib] = new FTKAthTrackContainer();
    m_tracks_hits[ib] = new FTKAthTrackContainer();
  }
}

void FTK_SGTrackOutput::eventEnd()
{
  for (int ibank=0;ibank!=m_nbanks;++ibank) { // bank loop

    m_log << MSG::DEBUG << "Number of tracks region " << ibank <<": " << m_tracks[ibank]->size() << endmsg;

    ostringstream track_contname;
    track_contname << "FTKTracks" << ibank << ends;
    StatusCode sc = m_storeGate->record(m_tracks[ibank],track_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
	    << "Could not register FTK track container data"
	    << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track container data class " << endmsg;
    }

    ostringstream track_pre_hw_contname;
    track_pre_hw_contname << "FTKTracks_pre_hw" << ibank << ends;
    sc = m_storeGate->record(m_tracks_pre_hw[ibank],track_pre_hw_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
      << "Could not register FTK track container data"
      << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track container data class " << endmsg;
    }

    ostringstream info_contname;
    info_contname << "FTKTFSummary" << ibank << ends;
    sc = m_storeGate->record(m_TFstats[ibank],info_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
	    << "Could not register FTK track fitter data"
	    << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track fitter data class " << endmsg;
    }


    // check if the blocks for the intermediate tracks have to be written
    if (!m_storeIncomplete) continue;

    ostringstream trackI_contname;
    trackI_contname << "FTKTracksI" << ibank << ends;
    sc = m_storeGate->record(m_tracksI[ibank],trackI_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
	    << "Could not register FTK track container data"
	    << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track container data class " << endmsg;
    }

    ostringstream trackI_pre_hw_contname;
    trackI_pre_hw_contname << "FTKTracksI_pre_hw" << ibank << ends;
    sc = m_storeGate->record(m_tracksI_pre_hw[ibank],trackI_pre_hw_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
      << "Could not register FTK track container data"
      << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track container data class " << endmsg;
    }

    ostringstream infoI_contname;
    infoI_contname << "FTKTFSummaryI" << ibank << ends;
    sc = m_storeGate->record(m_TFstatsI[ibank],infoI_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
	    << "Could not register FTK track fitter data"
	    << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track fitter data class " << endmsg;
    }

    ostringstream track_pattern_contname;
    track_pattern_contname << "FTKTracks_pattern" << ibank << ends;
    sc = m_storeGate->record(m_tracks_pattern[ibank],track_pattern_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
      << "Could not register FTK track container data"
      << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track container data class " << endmsg;
    }

    ostringstream track_hits_contname;
    track_hits_contname << "FTKTracks_hits" << ibank << ends;
    sc = m_storeGate->record(m_tracks_hits[ibank],track_hits_contname.str().c_str());
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR
      << "Could not register FTK track container data"
      << endmsg;
      return;
    }
    else {
      m_log << MSG::DEBUG << "Wrote FTK track container data class " << endmsg;
    }

  } // end bank loop
}

void FTK_SGTrackOutput::endFile()
{
}

unsigned long FTK_SGTrackOutput::runNumber(const unsigned int&) const { return 0;}
unsigned long FTK_SGTrackOutput::eventNumber(const unsigned int&) const { return 0;}
void FTK_SGTrackOutput::setRunNumber(const unsigned int&,const unsigned long&) {}
void FTK_SGTrackOutput::setEventNumber(const unsigned int&,const unsigned long&) {}


/** The method converts the original FTKTrack in a FTKAthTrack
    and add the latter in the list */
void FTK_SGTrackOutput::addTrack(int bank, const FTKTrack &track)
{
  //track.Print();

  // create the track
  FTKAthTrack *athtrack = new FTKAthTrack(track.getNCoords());
  // copy the FTKTrack content into the FTKAthTrack class
  // copy the IDs that identify the algorithm flow
  athtrack->setBankID(track.getBankID());
  athtrack->setRoadID(track.getRoadID());
  athtrack->setPatternID(track.getPatternID());
  athtrack->setSectorID(track.getSectorID());
  athtrack->setTrackID(track.getTrackID());
  // copy the helix parameters
  athtrack->setHalfInvPt(track.getHalfInvPt());
  athtrack->setIP(track.getIP());
  athtrack->setPhi(track.getPhi());
  athtrack->setZ0(track.getZ0());
  athtrack->setCotTheta(track.getCotTheta());
  athtrack->setChi2(track.getChi2());
  athtrack->setOrigChi2(track.getOrigChi2());
  // copy the coordinates
  for (int ix=0;ix!=track.getNCoords();++ix)
    athtrack->setCoord(ix,track.getCoord(ix));
  // copy the quality fields
  athtrack->setNMissing(track.getNMissing());
  athtrack->setTypeMask(track.getTypeMask());
  athtrack->setBitmask(track.getBitmask());
  athtrack->setHFRejected(track.getHFRejected());
  athtrack->setHWRejected(track.getHWRejected());
  athtrack->setHWTrackID(track.getHWTrackID());
  // copy the G4 truth matching fields
  athtrack->setEventIndex(track.getEventIndex());
  athtrack->setBarcode(track.getBarcode());
  athtrack->setBarcodeFrac(track.getBarcodeFrac());

  // FTKAthTrackContainer is defined to own the pointers
  m_tracks[bank]->push_back(athtrack);
}

/** The method converts the original FTKTrack in a FTKAthTrack
    and add the latter in the list */
void FTK_SGTrackOutput::addTrack_pre_hw(int bank, const FTKTrack &track)
{
  //track.Print();

  // create the track
  FTKAthTrack *athtrack = new FTKAthTrack(track.getNCoords());
  // copy the FTKTrack content into the FTKAthTrack class
  // copy the IDs that identify the algorithm flow
  athtrack->setBankID(track.getBankID());
  athtrack->setRoadID(track.getRoadID());
  athtrack->setPatternID(track.getPatternID());
  athtrack->setSectorID(track.getSectorID());
  athtrack->setTrackID(track.getTrackID());
  // copy the helix parameters
  athtrack->setHalfInvPt(track.getHalfInvPt());
  athtrack->setIP(track.getIP());
  athtrack->setPhi(track.getPhi());
  athtrack->setZ0(track.getZ0());
  athtrack->setCotTheta(track.getCotTheta());
  athtrack->setChi2(track.getChi2());
  athtrack->setOrigChi2(track.getOrigChi2());
  // copy the coordinates
  for (int ix=0;ix!=track.getNCoords();++ix)
    athtrack->setCoord(ix,track.getCoord(ix));
  // copy the quality fields
  athtrack->setNMissing(track.getNMissing());
  athtrack->setTypeMask(track.getTypeMask());
  athtrack->setBitmask(track.getBitmask());
  athtrack->setHFRejected(track.getHFRejected());
  athtrack->setHWRejected(track.getHWRejected());
  athtrack->setHWTrackID(track.getHWTrackID());
  // copy the G4 truth matching fields
  athtrack->setEventIndex(track.getEventIndex());
  athtrack->setBarcode(track.getBarcode());
  athtrack->setBarcodeFrac(track.getBarcodeFrac());

  // FTKAthTrackContainer is defined to own the pointers
  m_tracks_pre_hw[bank]->push_back(athtrack);
}

void FTK_SGTrackOutput::addNCombs(int ib, int v)
{
  m_TFstats[ib]->addNCombs(v);
}

void FTK_SGTrackOutput::addNFits(int ib, int v)
{
  m_TFstats[ib]->addNFits(v);
}

void FTK_SGTrackOutput::addNConnections(int ib, int v)
{
  m_TFstats[ib]->addNConnections(v);
}


void FTK_SGTrackOutput::addNExtrapolatedTracks(int ib, int v)
{
  m_TFstats[ib]->addNExtrapolatedTracks(v);
}

void FTK_SGTrackOutput::addNFitsMajority(int ib, int v)
{
  m_TFstats[ib]->addNFitsMajority(v);
}

void FTK_SGTrackOutput::addNFitsMajority_SCT(int /*ib*/, int /*v*/)
{
   //m_TFstats[ib]->addNFitsMajority_SCT(v);
}

void FTK_SGTrackOutput::addNFitsMajority_pix(int /*ib*/, int /*v*/)
{
   //m_TFstats[ib]->addNFitsMajority_pix(v);
}


void FTK_SGTrackOutput::addNFitsRecovery(int ib, int v)
{
  m_TFstats[ib]->addNFitsRecovery(v);
}


void FTK_SGTrackOutput::addNAddFitsRecovery(int ib, int v)
{
  m_TFstats[ib]->addNAddFitsRecovery(v);
}


void FTK_SGTrackOutput::addNFitsBad(int ib, int v) 
{
  m_TFstats[ib]->addNFitsBad(v);
}


void FTK_SGTrackOutput::addNFitsHWRejected(int ib, int v) 
{
  m_TFstats[ib]->addNFitsHWRejected(v);
}


void FTK_SGTrackOutput::addNFitsBadMajority(int ib, int v)
{
  m_TFstats[ib]->addNFitsBadMajority(v);
}


void FTK_SGTrackOutput::addNFitsHWRejectedMajority(int ib, int v) 
{
  m_TFstats[ib]->addNFitsHWRejectedMajority(v);
}



void FTK_SGTrackOutput::addTrackI(int bank, const FTKTrack &track)
{
  //track.Print();

  // create the track
  FTKAthTrack *athtrack = new FTKAthTrack(track.getNCoords());
  // copy the FTKTrack content into the FTKAthTrack class
  // copy the IDs that identify the algorithm flow
  athtrack->setBankID(track.getBankID());
  athtrack->setRoadID(track.getRoadID());
  athtrack->setPatternID(track.getPatternID());
  athtrack->setSectorID(track.getSectorID());
  athtrack->setTrackID(track.getTrackID());
  // copy the helix parameters
  athtrack->setHalfInvPt(track.getHalfInvPt());
  athtrack->setIP(track.getIP());
  athtrack->setPhi(track.getPhi());
  athtrack->setZ0(track.getZ0());
  athtrack->setCotTheta(track.getCotTheta());
  athtrack->setChi2(track.getChi2());
  athtrack->setOrigChi2(track.getOrigChi2());
  // copy the coordinates
  for (int ix=0;ix!=track.getNCoords();++ix)
    athtrack->setCoord(ix,track.getCoord(ix));
  // copy the quality fields
  athtrack->setNMissing(track.getNMissing());
  athtrack->setTypeMask(track.getTypeMask());
  athtrack->setBitmask(track.getBitmask());
  athtrack->setHFRejected(track.getHFRejected());
  athtrack->setHWRejected(track.getHWRejected());
  athtrack->setHWTrackID(track.getHWTrackID());
  // copy the G4 truth matching fields
  athtrack->setEventIndex(track.getEventIndex());
  athtrack->setBarcode(track.getBarcode());
  athtrack->setBarcodeFrac(track.getBarcodeFrac());

  // FTKAthTrackContainer is defined to own the pointers
  m_tracksI[bank]->push_back(athtrack);
}

void FTK_SGTrackOutput::addTrackI_pre_hw(int bank, const FTKTrack &track)
{
  //track.Print();

  // create the track
  FTKAthTrack *athtrack = new FTKAthTrack(track.getNCoords());
  // copy the FTKTrack content into the FTKAthTrack class
  // copy the IDs that identify the algorithm flow
  athtrack->setBankID(track.getBankID());
  athtrack->setRoadID(track.getRoadID());
  athtrack->setPatternID(track.getPatternID());
  athtrack->setSectorID(track.getSectorID());
  athtrack->setTrackID(track.getTrackID());
  // copy the helix parameters
  athtrack->setHalfInvPt(track.getHalfInvPt());
  athtrack->setIP(track.getIP());
  athtrack->setPhi(track.getPhi());
  athtrack->setZ0(track.getZ0());
  athtrack->setCotTheta(track.getCotTheta());
  athtrack->setChi2(track.getChi2());
  athtrack->setOrigChi2(track.getOrigChi2());
  // copy the coordinates
  for (int ix=0;ix!=track.getNCoords();++ix)
    athtrack->setCoord(ix,track.getCoord(ix));
  // copy the quality fields
  athtrack->setNMissing(track.getNMissing());
  athtrack->setTypeMask(track.getTypeMask());
  athtrack->setBitmask(track.getBitmask());
  athtrack->setHFRejected(track.getHFRejected());
  athtrack->setHWRejected(track.getHWRejected());
  athtrack->setHWTrackID(track.getHWTrackID());
  // copy the G4 truth matching fields
  athtrack->setEventIndex(track.getEventIndex());
  athtrack->setBarcode(track.getBarcode());
  athtrack->setBarcodeFrac(track.getBarcodeFrac());

  // FTKAthTrackContainer is defined to own the pointers
  m_tracksI_pre_hw[bank]->push_back(athtrack);
}

void FTK_SGTrackOutput::addTrack_pattern(int bank, const FTKTrack &track)
{
  //track.Print();

  // create the track
  FTKAthTrack *athtrack = new FTKAthTrack(track.getNCoords());
  // copy the FTKTrack content into the FTKAthTrack class
  // copy the IDs that identify the algorithm flow
  athtrack->setBankID(track.getBankID());
  athtrack->setRoadID(track.getRoadID());
  athtrack->setPatternID(track.getPatternID());
  athtrack->setSectorID(track.getSectorID());
  athtrack->setTrackID(track.getTrackID());
  // copy the helix parameters
  athtrack->setHalfInvPt(track.getHalfInvPt());
  athtrack->setIP(track.getIP());
  athtrack->setPhi(track.getPhi());
  athtrack->setZ0(track.getZ0());
  athtrack->setCotTheta(track.getCotTheta());
  athtrack->setChi2(track.getChi2());
  athtrack->setOrigChi2(track.getOrigChi2());
  // copy the coordinates
  for (int ix=0;ix!=track.getNCoords();++ix)
    athtrack->setCoord(ix,track.getCoord(ix));
  // copy the quality fields
  athtrack->setNMissing(track.getNMissing());
  athtrack->setTypeMask(track.getTypeMask());
  athtrack->setBitmask(track.getBitmask());
  athtrack->setHFRejected(track.getHFRejected());
  athtrack->setHWRejected(track.getHWRejected());
  athtrack->setHWTrackID(track.getHWTrackID());
  // copy the G4 truth matching fields
  athtrack->setEventIndex(track.getEventIndex());
  athtrack->setBarcode(track.getBarcode());
  athtrack->setBarcodeFrac(track.getBarcodeFrac());

  // FTKAthTrackContainer is defined to own the pointers
  m_tracks_pattern[bank]->push_back(athtrack);
}

void FTK_SGTrackOutput::addTrack_hits(int bank, const FTKTrack &track)
{
  //track.Print();

  // create the track
  FTKAthTrack *athtrack = new FTKAthTrack(track.getNCoords());
  // copy the FTKTrack content into the FTKAthTrack class
  // copy the IDs that identify the algorithm flow
  athtrack->setBankID(track.getBankID());
  athtrack->setRoadID(track.getRoadID());
  athtrack->setPatternID(track.getPatternID());
  athtrack->setSectorID(track.getSectorID());
  athtrack->setTrackID(track.getTrackID());
  // copy the helix parameters
  athtrack->setHalfInvPt(track.getHalfInvPt());
  athtrack->setIP(track.getIP());
  athtrack->setPhi(track.getPhi());
  athtrack->setZ0(track.getZ0());
  athtrack->setCotTheta(track.getCotTheta());
  athtrack->setChi2(track.getChi2());
  athtrack->setOrigChi2(track.getOrigChi2());
  // copy the coordinates
  for (int ix=0;ix!=track.getNCoords();++ix)
    athtrack->setCoord(ix,track.getCoord(ix));
  // copy the quality fields
  athtrack->setNMissing(track.getNMissing());
  athtrack->setTypeMask(track.getTypeMask());
  athtrack->setBitmask(track.getBitmask());
  athtrack->setHFRejected(track.getHFRejected());
  athtrack->setHWRejected(track.getHWRejected());
  athtrack->setHWTrackID(track.getHWTrackID());
  // copy the G4 truth matching fields
  athtrack->setEventIndex(track.getEventIndex());
  athtrack->setBarcode(track.getBarcode());
  athtrack->setBarcodeFrac(track.getBarcodeFrac());

  // FTKAthTrackContainer is defined to own the pointers
  m_tracks_hits[bank]->push_back(athtrack);
}

void FTK_SGTrackOutput::addNCombsI(int ib, int v)
{
  m_TFstatsI[ib]->addNCombs(v);
}

void FTK_SGTrackOutput::addNFitsI(int ib, int v)
{
  m_TFstatsI[ib]->addNFits(v);
}

void FTK_SGTrackOutput::addNFitsMajorityI(int ib, int v)
{
  m_TFstatsI[ib]->addNFitsMajority(v);
}
void FTK_SGTrackOutput::addNFitsMajorityI_pix(int /*ib*/, int /*v*/)
{
   //m_TFstatsI[ib]->addNFitsMajority_pix(v);
}
void FTK_SGTrackOutput::addNFitsMajorityI_SCT(int /*ib*/, int /*v*/)
{
   //m_TFstatsI[ib]->addNFitsMajority_SCT(v);
}


void FTK_SGTrackOutput::addNFitsRecoveryI(int ib, int v)
{
  m_TFstatsI[ib]->addNFitsRecovery(v);
}


void FTK_SGTrackOutput::addNAddFitsRecoveryI(int ib, int v)
{
  m_TFstatsI[ib]->addNAddFitsRecovery(v);
}


void FTK_SGTrackOutput::addNFitsBadI(int ib, int v) 
{
  m_TFstatsI[ib]->addNFitsBad(v);
}


void FTK_SGTrackOutput::addNFitsHWRejectedI(int ib, int v) 
{
  m_TFstatsI[ib]->addNFitsHWRejected(v);
}


void FTK_SGTrackOutput::addNFitsBadMajorityI(int ib, int v)
{
  m_TFstatsI[ib]->addNFitsBadMajority(v);
}


void FTK_SGTrackOutput::addNFitsHWRejectedMajorityI(int ib, int v) 
{
  m_TFstatsI[ib]->addNFitsHWRejectedMajority(v);
}
