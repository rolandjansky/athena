/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTLorentzMonTool.cxx
*
*    @author Elias Coniavitis based on code from Luca Fiorini,
*    Shaun Roe, Manuel Diaz, Rob McPherson & Richard Batley
*    Modified by Yuta
*/
#include "SCT_Monitoring/SCTLorentzMonTool.h"
#include "deletePointers.h"
#include "SCT_NameFormatter.h"
#include "boost/lexical_cast.hpp"
#include <cmath>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"

// for sct residuals
//#include "TrkParameters/MeasuredAtaPlane.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "VxVertex/VxCandidate.h"

using namespace std;
using namespace Rec;
using namespace SCT_Monitoring;

namespace{ //anon. namespace for file-scoped functions

  template<class T, int Size1> 
  int byte_extent( T (&)[Size1]){
    return Size1*sizeof(T);
  }

  // Simple method for initalizing arrays of pointers to null
  template <class T, int Size1>
  void clear(T (&parray)[Size1]){
    memset(parray,0,byte_extent(parray)); 
  }

}

//====================================================================================================
/** Constructor, calls base class constructor with parameters
*
*  several properties are "declared" here, allowing selection
*  of the filepath for histograms, the first and second plane
*  numbers to be used, and the timebin.
*/
//====================================================================================================
SCTLorentzMonTool::SCTLorentzMonTool(const string & type,const string & name,const IInterface* parent):SCTMotherTrigMonTool(type, name, parent),
m_trackToVertexTool("Reco::TrackToVertex") //for TrackToVertexTool
{
  declareProperty("histoPathBase", m_stream = "/stat");
  declareProperty("tracksName",m_tracksName="InDetTrackParticles"); //this recommended
  declareProperty("TrackToVertexTool",m_trackToVertexTool); //for TrackToVertexTool
  m_numberOfEvents=0;
  clear(m_phiVsNstrips);
  clear(m_phiVsNstrips_Side);
}

//====================================================================================================
//====================================================================================================
SCTLorentzMonTool::~SCTLorentzMonTool(){
  //nada
}

//====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
//====================================================================================================
//StatusCode SCTLorentzMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool isNewLumiBlock, bool isNewRun )//suppress 'unused' compiler warning     // hidetoshi 14.01.21
StatusCode SCTLorentzMonTool::bookHistogramsRecurrent( )                                                                                                // hidetoshi 14.01.21
{
  m_path= "";
  if(newRun) m_numberOfEvents=0;                                                                                                                        // hidetoshi 14.01.21
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "initialize being called" << endreq;
  detStore()->retrieve(m_pSCTHelper,"SCT_ID");
  StatusCode sc = detStore()->retrieve(m_sctmgr,"SCT");
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL)<< "SCT detector manager not found !" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::INFO)) msg(MSG::INFO)<< "SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"" << endreq;

   /* Retrieve TrackToVertex extrapolator tool */
  if ( m_trackToVertexTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "failed to retrieve tool " << endreq;
    return StatusCode::FAILURE;
  }

  //Booking  Track related Histograms
  //  if(bookLorentzHistos(isNewRun,isNewLumiBlock).isFailure() ) msg(MSG::WARNING) << "Error in bookLorentzHistos()" << endreq;     // hidetoshi 14.01.21
  if(bookLorentzHistos().isFailure() ) msg(MSG::WARNING) << "Error in bookLorentzHistos()" << endreq;                                // hidetoshi 14.01.22

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
//====================================================================================================
//StatusCode SCTLorentzMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool isNewLumiBlock, bool isNewRun )//suppress 'unused' compiler warning    // hidetoshi 14.01.21
StatusCode SCTLorentzMonTool::bookHistograms( )                                                                                                        // hidetoshi 14.01.21
{
  m_path= "";
  //  if(isNewRun) m_numberOfEvents=0;                                                                                                                 // hidetoshi 14.01.21
  m_numberOfEvents=0;                                                                                                                                  // hidetoshi 14.01.21
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "initialize being called" << endreq;
  detStore()->retrieve(m_pSCTHelper,"SCT_ID");
  StatusCode sc = detStore()->retrieve(m_sctmgr,"SCT");
  if (sc.isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL)<< "SCT detector manager not found !" << endreq;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::INFO)) msg(MSG::INFO)<< "SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"" << endreq;

   /* Retrieve TrackToVertex extrapolator tool */
  if ( m_trackToVertexTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "failed to retrieve tool " << endreq;
    return StatusCode::FAILURE;
  }

  //Booking  Track related Histograms
  //  if(bookLorentzHistos(isNewRun,isNewLumiBlock).isFailure() ) msg(MSG::WARNING) << "Error in bookLorentzHistos()" << endreq;     // hidetoshi 14.01.21
  if(bookLorentzHistos().isFailure() ) msg(MSG::WARNING) << "Error in bookLorentzHistos()" << endreq;                                // hidetoshi 14.01.22

  return StatusCode::SUCCESS;
}


//====================================================================================================
//                        SCTLorentzMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
//====================================================================================================
StatusCode SCTLorentzMonTool::fillHistograms(){
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"enters fillHistograms"<< endreq;
  //const Rec::TrackParticleContainer *tracks ( 0 );
  const xAOD::TrackParticleContainer *tracks ( 0 );
  m_tracksName = "InDetTrackParticles";//test;
  if ( evtStore()->contains<xAOD::TrackParticleContainer> ( m_tracksName) ){ 
    if(evtStore()->retrieve(tracks, m_tracksName).isFailure()) {
      msg(MSG::WARNING) <<tracks<<" Tracks not found: Exit SCTTracksMonTool" << m_tracksName << endreq;
      return StatusCode::SUCCESS;
    }
  } else {
    msg(MSG::WARNING) <<"m_tracksName not set: Exit SCTTracksMonTool" << endreq;
    return StatusCode::SUCCESS;
  }

  msg(MSG::INFO) <<"track iterator retrieve start" << endreq;
  xAOD::TrackParticleContainer::const_iterator trkitr  = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator trkend = tracks->end();
  msg(MSG::INFO)  << "track iterator start " << endreq;

  for ( ; trkitr != trkend; ++trkitr) {
    // Get track                                                                                                                                                                                    
    msg(MSG::INFO) << "xAOD::TrackParticle retrieve start." << endreq;
    const xAOD::TrackParticle * trackParticle = (*trkitr);
    if (not trackParticle) {
      msg(MSG::ERROR) << "no pointer to trackParticle!!!" << endreq;
      break;
    }
    
    const Trk::Track *track= trackParticle->track();
    if (not track) {
      msg(MSG::ERROR) << "no pointer to track!!!" << endreq;
      break;
    }

    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
    if (not trackStates){
      msg(MSG::WARNING) << "for current track, TrackStateOnSurfaces == Null, no data will be written for this track" << endreq;
      break;
    }

    DataVector<const Trk::TrackStateOnSurface>::const_iterator endit=trackStates->end();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin(); it!=endit; ++it) {
      if((*it)->type(Trk::TrackStateOnSurface::Measurement)){
        const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack*>( (*it)->measurementOnTrack() );
        if(clus) { // Is it a SiCluster? If yes...
          const InDet::SiCluster *RawDataClus=dynamic_cast<const InDet::SiCluster*>( clus->prepRawData() );
          if (not RawDataClus) continue; // Continue if dynamic_cast returns null
          if (RawDataClus->detectorElement()->isSCT() ) {
            const Identifier sct_id = clus->identify();
            const int bec(m_pSCTHelper->barrel_ec(sct_id));
            const int layer(m_pSCTHelper->layer_disk(sct_id));
            const int side(m_pSCTHelper->side(sct_id));
            if(bec!=0) continue; //We only care about the barrel
            // find cluster size
            const std::vector<Identifier>& rdoList = RawDataClus->rdoList();
            int nStrip = rdoList.size();
            const Trk::TrackParameters *trkp = dynamic_cast<const Trk::TrackParameters*>( (*it)->trackParameters() );
	    //            const Trk::MeasuredTrackParameters *mtrkp = dynamic_cast<const Trk::MeasuredTrackParameters*>( (*it)->trackParameters() );
            const Trk::TrackSummary* summary = track->trackSummary();

            const xAOD::Vertex * vertex(0);
            ElementLink<xAOD::VertexContainer> v_link = trackParticle->vertexLink();
            if(v_link.isValid()) vertex = *v_link;
            const Trk::Perigee* perigee = 0;
            if (not vertex) {
              msg(MSG::WARNING) << " cannot get Trk::VxCandidate * vertex " << endreq;
            } else {
              perigee = m_trackToVertexTool->perigeeAtVertex(*trackParticle, vertex->position());
            }
      //      const Trk::RecVertex vtx = vertex->recVertex();
      //      const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*trackParticle, vtx.position());

	    //            if (mtrkp && perigee){
            if (trkp && perigee){
              //Get angle to wafer surface
              double phiToWafer(90.),thetaToWafer(90.);
              double sinAlpha = 0.; //for barrel, which is the only thing considered here
              double pTrack[3];
              pTrack[0] = trkp->momentum().x();
              pTrack[1] = trkp->momentum().y();
              pTrack[2] = trkp->momentum().z();
              int iflag = findAnglesToWaferSurface (pTrack, sinAlpha, clus->identify(), thetaToWafer, phiToWafer );
              if ( iflag < 0) {
                msg(MSG::WARNING) << "Error in finding track angles to wafer surface" << endreq;
                continue; // Let's think about this (later)... continue, break or return?
              }

              bool passesCuts = true;
              if( (track->perigeeParameters()->parameters()[Trk::qOverP] < 0.) && // use negative track only
                (fabs( perigee->parameters()[Trk::d0] ) < 1.) &&  // d0 < 1mm
                (fabs( perigee->parameters()[Trk::z0] * sin(perigee->parameters()[Trk::theta]) ) < 1.) &&  // d0 < 1mm
                (trkp->momentum().mag() > 500.) &&  // Pt > 500MeV
                (summary->get(Trk::numberOfSCTHits) > 6 ) && // #SCTHits >6
                (summary->get(Trk::numberOfPixelHits) > 1 ) // #pixelHits >1
              ){
                passesCuts=true;
              }else{ 
                passesCuts=false;
              }
              if(passesCuts){
              //Fill profile
                m_phiVsNstrips[layer]->Fill(phiToWafer,nStrip,1.);
                m_phiVsNstrips_Side[layer][side]->Fill(phiToWafer,nStrip,1.);
              }// end if passesCuts
            }// end if mtrkp
            delete perigee;perigee = 0;
          } // end if SCT..
        } // end if(clus)
      } // if((*it)->type(Trk::TrackStateOnSurface::Measurement)){
      }// end of loop on TrackStatesonSurface (they can be SiClusters, TRTHits,..)
    } // end of loop on tracks

    m_numberOfEvents++;
    return StatusCode::SUCCESS;
  }

//====================================================================================================
//                             SCTLorentzMonTool :: procHistograms
//====================================================================================================
//  StatusCode  SCTLorentzMonTool::procHistograms(bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool isEndOfRun){//suppress 'unused' compiler warning    // hidetoshi 14.01.21 
  StatusCode  SCTLorentzMonTool::procHistograms(){                                                                                                                 // hidetoshi 14.01.21
    
    //    if(isEndOfRun){                                                                                                                                          // hidetoshi 14.01.21
    if(endOfRun){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "finalHists()" << endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Total Rec Event Number: " << m_numberOfEvents<<endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling checkHists(true); true := end of run" << endreq ;
      if (checkHists(true).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Error in checkHists(true)" << endreq ;
      }
    }
    
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Exiting finalHists" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode  SCTLorentzMonTool::checkHists(bool /*fromFinalize*/){

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                              SCTLorentzMonTool :: bookLorentzHistos
//====================================================================================================
//StatusCode SCTLorentzMonTool::bookLorentzHistos(bool isNewRun,bool /*isNewLumiBlock*/){                                                                          // hidetoshi 14.01.22
StatusCode SCTLorentzMonTool::bookLorentzHistos(){                                                                                                                 // hidetoshi 14.01.22
  //  if(isNewRun){                                                                                                                                                // hidetoshi 14.01.22
    const int nLayers(4);
    const int nSides(2);
    string stem=m_path+"/SCT/GENERAL/lorentz/";
    //    MonGroup Lorentz(this,m_path+"SCT/GENERAL/lorentz",expert,run);        // hidetoshi 14.01.21
    MonGroup Lorentz(this,m_path+"SCT/GENERAL/lorentz",run, ATTRIB_UNMANAGED);     // hidetoshi 14.01.21

    string hNum[nLayers]={"0","1","2","3"};
    string hNumS[nSides]={"0","1"};
    int nProfileBins=360;

    int success=1;
    for(int l=0;l!=nLayers;++l){
      //granularity set to one profile/layer for now
      int iflag=0;
      m_phiVsNstrips[l] = pFactory("h_phiVsNstrips"+hNum[l],"Inc. Angle vs nStrips for Layer "+hNum[l],nProfileBins,-90.,90.,Lorentz,iflag);
      for(int side=0;side<nSides;++side){
        m_phiVsNstrips_Side[l][side] = pFactory("h_phiVsNstrips"+hNum[l]+"Side"+hNumS[side],"Inc. Angle vs nStrips for Layer Side "+hNum[l]+hNumS[side],nProfileBins,-90.,90.,Lorentz,iflag);
      }
      success*=iflag;
    }
    if (success==0 ) return StatusCode::FAILURE;
    //  }                                                                                                                                                           // hidetoshi 14.01.22
  return StatusCode::SUCCESS;
}



TProfile*
SCTLorentzMonTool::pFactory(const std::string & name, const std::string & title, int nbinsx, double xlow, double xhigh, MonGroup & registry, int& iflag){

  Prof_t tmp = new TProfile(TString(name), TString(title),nbinsx,xlow,xhigh);
  bool success( registry.regHist(tmp).isSuccess());
  if (not success){
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book SCT histogram: " << name << endreq;
    iflag=0;
  }else{
    iflag=1;
  }

  return tmp;
}
bool
SCTLorentzMonTool::h1Factory(const std::string & name, const std::string & title, const float extent, MonGroup & registry, VecH1_t & storageVector){
  const unsigned int nbins(100);
  const float lo(-extent), hi(extent);
  H1_t tmp = new TH1F(TString(name), TString(title), nbins, lo, hi);
  bool success( registry.regHist(tmp).isSuccess());
  if (not success) if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book SCT histogram: " << name << endreq;
  storageVector.push_back(tmp);
  return success;
}

int SCTLorentzMonTool::findAnglesToWaferSurface ( const double (&vec)[3], const double &sinAlpha, const Identifier &id, double &theta, double &phi ) {
  int iflag(-1);
  phi   = 90.;
  theta = 90.;

  InDetDD::SiDetectorElement* element = m_sctmgr->getDetectorElement(id);
  if (!element) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "findAnglesToWaferSurface:  failed to find detector element for id=" << m_pSCTHelper->show_to_string(id) << endreq;
    return iflag;
  }

  double cosAlpha = sqrt(1.-sinAlpha*sinAlpha);
  double phix =  cosAlpha*element->phiAxis().x() + sinAlpha*element->phiAxis().y();
  double phiy = -sinAlpha*element->phiAxis().x() + cosAlpha*element->phiAxis().y();

  double pNormal = vec[0]*element->normal().x()  + vec[1]*element->normal().y()  + vec[2]*element->normal().z();
  double pEta    = vec[0]*element->etaAxis().x() + vec[1]*element->etaAxis().y() + vec[2]*element->etaAxis().z();
  double pPhi    = vec[0]*phix                   + vec[1]*phiy                   + vec[2]*element->phiAxis().z();

  if (pPhi<0.) phi   = -90.;
  if (pEta<0.) theta = -90.;
  if (pNormal!=0.) {
    phi   = atan(pPhi/pNormal)/CLHEP::deg;
    theta = atan(pEta/pNormal)/CLHEP::deg;
  }
  iflag = 1;
  return iflag;
}
