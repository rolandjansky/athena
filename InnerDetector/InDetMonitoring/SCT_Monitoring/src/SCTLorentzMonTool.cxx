/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTLorentzMonTool.cxx
 *
 *    @author Elias Coniavitis based on code from Luca Fiorini,
 *    Shaun Roe, Manuel Diaz, Rob McPherson & Richard Batley
 *    Modified by Yuta, Arka Santra
 */
#include "SCT_Monitoring/SCTLorentzMonTool.h"
#include "deletePointers.h"
#include "SCT_NameFormatter.h"
#include <cmath>
#include <type_traits>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

// for sct residuals
#include "TrkTrackSummary/TrackSummary.h"

using namespace std;
using namespace Rec;
using namespace SCT_Monitoring;

namespace{//anonymous namespace for functions at file scope
  template< typename T > Identifier surfaceOnTrackIdentifier(T & tsos, const bool useTrackParameters=true){
    Identifier result(0); //default constructor produces invalid value
    const Trk::MeasurementBase* mesb = tsos->measurementOnTrack();
    if (mesb and mesb->associatedSurface().associatedDetectorElement())
      result = mesb->associatedSurface().associatedDetectorElement()->identify();
    else if (useTrackParameters and tsos->trackParameters()){
      result = tsos->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
    }
    return result;
  }
}//namespace end
// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms etc
 */
// ====================================================================================================
SCTLorentzMonTool::SCTLorentzMonTool(const string &type, const string &name,
                                     const IInterface *parent) : SCTMotherTrigMonTool(type, name, parent),
                                                                 m_trackToVertexTool("Reco::TrackToVertex", this), // for TrackToVertexTool
                                                                 m_getTrackHoles(true),
                                                                 m_phiVsNstrips{},
  m_phiVsNstrips_100{},
  m_phiVsNstrips_111{},
  m_phiVsNstrips_Side{},
  m_phiVsNstrips_Side_100{},
  m_phiVsNstrips_Side_111{},
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool"),
  m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels"), // This has to be public tool (without this pointer)
  m_rejectSharedHit(true),
  m_pSCTHelper(nullptr),
  m_sctmgr(nullptr) {
  m_stream = "/stat";
  declareProperty("tracksName", m_tracksName = "CombinedInDetTracks"); // this recommended
  declareProperty("TrackToVertexTool", m_trackToVertexTool); // for TrackToVertexTool
  m_numberOfEvents = 0;
  declareProperty("HoleSearch", m_holeSearchTool);
  declareProperty("GetTrackHoles", m_getTrackHoles);
  declareProperty("AssociationTool", m_assoTool);
  declareProperty("RejectSharedHit", m_rejectSharedHit);
}

// ====================================================================================================
// ====================================================================================================
SCTLorentzMonTool::~SCTLorentzMonTool() {
  // nada
}

// ====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
// ====================================================================================================
// StatusCode SCTLorentzMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool isNewLumiBlock, bool isNewRun
// )//suppress 'unused' compiler warning     // hidetoshi 14.01.21
StatusCode
SCTLorentzMonTool::bookHistogramsRecurrent() {                                                                                              //
                                                                                                                                             // hidetoshi
                                                                                                                                             // 14.01.21
  CHECK (m_holeSearchTool.retrieve());
  CHECK (m_assoTool.retrieve());
  m_path = "";
  if (newRunFlag()) {
    m_numberOfEvents = 0;                                                                                                                        //
                                                                                                                                                 // hidetoshi
                                                                                                                                                 // 14.01.21
  }
  ATH_MSG_DEBUG("initialize being called");
  detStore()->retrieve(m_pSCTHelper, "SCT_ID");
  ATH_CHECK(detStore()->retrieve(m_sctmgr, "SCT"));
  ATH_MSG_DEBUG("SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"");
  /* Retrieve TrackToVertex extrapolator tool */
  ATH_CHECK(m_trackToVertexTool.retrieve());
  // Booking  Track related Histograms
  if (bookLorentzHistos().isFailure()) {
    ATH_MSG_WARNING("Error in bookLorentzHistos()");                                // hidetoshi 14.01.22
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTLorentzMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::bookHistograms() {                                                                                                      //
                                                                                                                                            // hidetoshi
                                                                                                                                            // 14.01.21
  CHECK (m_assoTool.retrieve());
  CHECK (m_holeSearchTool.retrieve());
  m_path = "";
  m_numberOfEvents = 0;                                                                                                                                  //
                                                                                                                                                         // hidetoshi
                                                                                                                                                         // 14.01.21
  ATH_MSG_DEBUG("initialize being called");
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_sctmgr, "SCT"));
  ATH_MSG_DEBUG("SCT detector manager found: layout is \"" << m_sctmgr->getLayout() << "\"");
  /* Retrieve TrackToVertex extrapolator tool */
  ATH_CHECK(m_trackToVertexTool.retrieve());
  // Booking  Track related Histograms
  if (bookLorentzHistos().isFailure()) {
    ATH_MSG_WARNING("Error in bookLorentzHistos()");                                // hidetoshi 14.01.22
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                        SCTLorentzMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTLorentzMonTool::fillHistograms() {
  // should use database for this!
  constexpr int layer100[] = {
    2, 2, 3, 2, 2, 2, 0, 2, 3, 2, 0, 2, 3, 2, 3, 2, 0, 2, 3, 0, 2, 0, 2, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3, 0, 3, 2, 0, 2,
    2, 0, 3, 3, 3, 0, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3, 3, 2, 2, 2, 2, 2, 3, 3, 2, 3, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 3, 3,
    2, 3, 2, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2, 2
  };
  constexpr int phi100[] = {
    29, 29, 6, 13, 23, 13, 14, 29, 9, 29, 14, 29, 9, 29, 39, 32, 21, 32, 13, 22, 32, 22, 32, 13, 32, 32, 32, 20, 20, 20,
    20, 20, 20, 13, 21, 17, 33, 5, 33, 33, 31, 6, 19, 47, 21, 37, 37, 37, 37, 33, 37, 37, 24, 33, 33, 47, 19, 33, 33,
    37, 37, 37, 55, 9, 38, 24, 37, 38, 8, 9, 9, 26, 38, 38, 38, 38, 39, 39, 38, 11, 45, 54, 54, 24, 31, 14, 47, 45, 47,
    47, 47, 47
  };
  constexpr int eta100[] = {
    3, -4, -6, 2, 6, 3, -5, -1, 6, -2, -6, -5, 5, -3, 2, 6, -3, 5, 5, 3, 4, 2, 2, 2, -1, -3, -4, 1, -1, -2, -3, -4, 4,
    -1, -5, 6, 2, 4, 3, 1, 6, -2, 6, 3, -6, -1, 2, 1, 3, -5, 4, 5, -3, -4, -3, -5, -2, -1, -2, -3, -2, -4, -3, 2, 3, -6,
    -5, 4, 6, 1, -6, 1, 1, -5, -4, -3, -3, -5, -2, 1, 5, 5, 4, 4, 5, 4, -1, -5, 3, 4, 1, -5
  };
  constexpr unsigned int layer100_n = sizeof(layer100) / sizeof(*layer100);
  constexpr unsigned int phi100_n = sizeof(phi100) / sizeof(*phi100);
  constexpr unsigned int eta100_n = sizeof(eta100) / sizeof(*eta100);
  constexpr bool theseArraysAreEqualInLength = ((layer100_n == phi100_n)and(phi100_n == eta100_n));

  static_assert(theseArraysAreEqualInLength, "Coordinate arrays for <100> wafers are not of equal length");

  ATH_MSG_DEBUG("enters fillHistograms");

  const TrackCollection *tracks(0);
  if (evtStore()->contains<TrackCollection> (m_tracksName)) {
    if (evtStore()->retrieve(tracks, m_tracksName).isFailure()) {
      ATH_MSG_WARNING(" TrackCollection not found: Exit SCTLorentzTool" << m_tracksName);
      return StatusCode::SUCCESS;
    }
  } else {
    ATH_MSG_WARNING("Container " << m_tracksName << " not found.  Exit SCTLorentzMonTool");
    return StatusCode::SUCCESS;
  }
  
  
  // Prepare AssociationTool
  m_assoTool->reset();
  for (const Trk::Track* track : *tracks) {
    ATH_CHECK(m_assoTool->addPRDs(*track));
  }


  TrackCollection::const_iterator trkitr = tracks->begin();
  TrackCollection::const_iterator trkend = tracks->end();

  for (; trkitr != trkend; ++trkitr) {
    // get track
    const Trk::Track * origTrack = (*trkitr);
    if (not origTrack) {
        ATH_MSG_ERROR("no pointer to origTrack!!!");
        continue;
    }
    
    const Trk::TrackSummary *summary = origTrack->trackSummary();
    if (not summary){
        ATH_MSG_WARNING(" null trackSummary");
        continue;
    }
    
    const Trk::Perigee *perigee = origTrack->perigeeParameters();
    if(not perigee)continue;
    /// selecting the tracks, the pT cut on tracks is applied later
    bool passesCuts = true;
    if ((AthenaMonManager::dataType() == AthenaMonManager::cosmics) &&
        (summary->get(Trk::numberOfSCTHits) > 6)//  SCTHits >6
    ){
        passesCuts = true;
    }// 01.02.2015
    else if ((perigee->parameters()[Trk::qOverP] < 0.) && // use negative track only
        (std::abs(perigee->parameters()[Trk::d0]) < 1.) && // d0 < 1mm
        //(std::abs( perigee->parameters()[Trk::z0] * sin(perigee->parameters()[Trk::theta]) ) < 1.) // another way to implement d0 < 1mm
        (summary->get(Trk::numberOfPixelHits) > 1) // nPixelHits > 1
    ){
        passesCuts = true;
    }else{
        passesCuts = false;
    }
      
    if(not passesCuts)continue;
      
    const Trk::Track *track = nullptr;
    if(m_getTrackHoles){
        // Get track with holes for the tracks passing the selection
        track = m_holeSearchTool->getTrackWithHoles(*origTrack);
    }
    else{
        // get track without holes
        track = origTrack;
    }
    
    if (not track) {
      ATH_MSG_WARNING("track pointer is invalid");
      continue;
    }
    
    
    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface> *trackStates = track->trackStateOnSurfaces();
    if (not trackStates) {
      ATH_MSG_WARNING("for current track, TrackStateOnSurfaces == Null, no data will be written for this track");
      continue;
    }

    /// counting the hits in SCT
    unsigned int nSCTBarrel = 0;
    for (const Trk::TrackStateOnSurface* tsos : *trackStates) {
      if (not tsos->type(Trk::TrackStateOnSurface::Measurement)) continue; // Not a hit

      const InDet::SiClusterOnTrack* rot = dynamic_cast<const InDet::SiClusterOnTrack*>(tsos->measurementOnTrack());
      if (rot==nullptr) continue; // Not a valid SiClusterOnTrack

      const InDet::SiCluster* clus = dynamic_cast<const InDet::SiCluster*>(rot->prepRawData());
      if (clus==nullptr) continue; // Not a valid SiCluster

      if (not clus->detectorElement()->isSCT()) continue; // Not an SCT hit

      const Identifier& sct_id = clus->identify();
      const int bec = m_pSCTHelper->barrel_ec(sct_id);

      if (bec==0) nSCTBarrel++; // This SCT hit is in Barrel
    }

    DataVector<const Trk::TrackStateOnSurface>::const_iterator endit = trackStates->end();
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackStates->begin(); it != endit; ++it) {
      /// selection of tracks
      const Trk::TrackParameters *trkp = dynamic_cast<const Trk::TrackParameters *>((*it)->trackParameters());
      if (not trkp){
          ATH_MSG_DEBUG(" Null pointer to MeasuredTrackParameters");
          continue;
      }
      
      /// selecting track momentum greater than 500 and nSCTBarrel > 7 and use only |localX| < 28 mm and 2 <|localY| < 58 mm tracks
      if(!(trkp->momentum().perp() > 500. && nSCTBarrel > 7 && std::abs(trkp->localPosition()[0]) < 28 && std::abs(trkp->localPosition()[1]) > 2 && std::abs(trkp->localPosition()[1]) < 58)) continue;
      
      //// get the pT index of the tracks
      int pTIndex(-1);
      if(trkp->momentum().perp() >= 500. && trkp->momentum().perp() < 600.)
          pTIndex = 0;
      else if(trkp->momentum().perp() >= 600. && trkp->momentum().perp() < 700.)
          pTIndex = 1;
      else if(trkp->momentum().perp() >= 700. && trkp->momentum().perp() < 1000.)
          pTIndex = 2;
      else if(trkp->momentum().perp() >= 1000. && trkp->momentum().perp() < 1500.)
          pTIndex = 3;
      else if(trkp->momentum().perp() >= 1500. && trkp->momentum().perp() < 2000.)
          pTIndex = 4;
      else if(trkp->momentum().perp() >= 2000. && trkp->momentum().perp() < 3000.)
          pTIndex = 5;
      else if(trkp->momentum().perp() >= 3000. && trkp->momentum().perp() < 5000.)
          pTIndex = 6;
      else
          pTIndex = 7;
          
      // working on hits
      if ((*it)->type(Trk::TrackStateOnSurface::Measurement)) {
        const InDet::SiClusterOnTrack *clus =
        dynamic_cast<const InDet::SiClusterOnTrack *>((*it)->measurementOnTrack());
        if (clus) { // Is it a SiCluster? If yes...
            const InDet::SiCluster *RawDataClus = dynamic_cast<const InDet::SiCluster *>(clus->prepRawData());
            if (not RawDataClus) {
                continue; // Continue if dynamic_cast returns null
            }
            if (RawDataClus->detectorElement()->isSCT()) {
                const Identifier sct_id = clus->identify();
                const int bec(m_pSCTHelper->barrel_ec(sct_id));
                const int layer(m_pSCTHelper->layer_disk(sct_id));
                const int side(m_pSCTHelper->side(sct_id));
                const int eta(m_pSCTHelper->eta_module(sct_id));
                const int phi(m_pSCTHelper->phi_module(sct_id));
                bool in100 = false;
                if (bec != 0) {
                    continue; // We only care about the barrel
                }
                for (unsigned int i = 0; i < layer100_n; i++) {
                    if (layer100[i] == layer && eta100[i] == eta && phi100[i] == phi) {
                        in100 = true;
                        break;
                    }
                }
                // find cluster size
                const std::vector<Identifier> &rdoList = RawDataClus->rdoList();
                int nStrip = rdoList.size();
                
                /// rejecing shared hits
                if (m_rejectSharedHit and m_assoTool->isShared(*(clus->prepRawData()))) {
                   continue;
                }

                
                // Get angle to wafer surface
                float phiToWafer(90.), thetaToWafer(90.);
                float sinAlpha = 0.; // for barrel, which is the only thing considered here
                float pTrack[3];
                pTrack[0] = trkp->momentum().x();
                pTrack[1] = trkp->momentum().y();
                pTrack[2] = trkp->momentum().z();
                int iflag = findAnglesToWaferSurface(pTrack, sinAlpha, clus->identify(), thetaToWafer, phiToWafer);
                if (iflag < 0) {
                    ATH_MSG_WARNING("Error in finding track angles to wafer surface");
                    continue; // Let's think about this (later)... continue, break or return?
                }
                
                // Fill profile
                m_phiVsNstrips[layer]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side[layer][side]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                if (in100) {
                    m_phiVsNstrips_100[layer]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_Side_100[layer][side]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_100_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_Side_100_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                }else {
                    m_phiVsNstrips_111[layer]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_Side_111[layer][side]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_111_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_Side_111_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                }
                
                /// first change the negative eta to positive eta index. This is needed to fill the array of histograms.
                int etaIndex(-1);
                if(eta == -1 ){
                    etaIndex = 0;
                }
                else if(eta == 1){
                    etaIndex = 1;
                }
                
                if(std::abs(eta)==1){
                    m_phiVsNstrips_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_Side_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_Side_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    if (in100) {
                        m_phiVsNstrips_100_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_Side_100_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_100_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_Side_100_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    }else {
                        m_phiVsNstrips_111_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_Side_111_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_111_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_Side_111_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    }
                }
                
                /// selecting only eta < 0.5 cases
                if(std::abs(trkp->eta())>0.5)continue;
                
                m_phiVsNstrips_eta0p5[layer]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_Side[layer][side]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_Side_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                if (in100) {
                    m_phiVsNstrips_eta0p5_100[layer]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_Side_100[layer][side]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_100_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_Side_100_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                }else {
                    m_phiVsNstrips_eta0p5_111[layer]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_Side_111[layer][side]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_111_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_Side_111_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
                }
                if(std::abs(eta)==1){
                    m_phiVsNstrips_eta0p5_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_Side_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    m_phiVsNstrips_eta0p5_Side_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    if (in100) {
                        m_phiVsNstrips_eta0p5_100_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_eta0p5_Side_100_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_eta0p5_100_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_eta0p5_Side_100_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    }else {
                        m_phiVsNstrips_eta0p5_111_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_eta0p5_Side_111_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_eta0p5_111_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                        m_phiVsNstrips_eta0p5_Side_111_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                    }
                }
            }// end if SCT
        } // end if clus..
      } // end if((*it)->type(Trk::TrackStateOnSurface::Measurement)){
      
      // working on holes only if we want to
      else if(m_getTrackHoles && (*it)->type(Trk::TrackStateOnSurface::Hole)) {
        Identifier surfaceID;
        surfaceID = surfaceOnTrackIdentifier(*it);
        if(not m_pSCTHelper->is_sct(surfaceID)) continue; //We only care about SCT
        const int bec(m_pSCTHelper->barrel_ec(surfaceID));
        const int layer(m_pSCTHelper->layer_disk(surfaceID));
        const int side(m_pSCTHelper->side(surfaceID));
        const int eta(m_pSCTHelper->eta_module(surfaceID));
        const int phi(m_pSCTHelper->phi_module(surfaceID));
        bool in100 = false;
        if(bec!=0) {
            continue; //We only care about the barrel
        }
        for (unsigned int i=0 ; i<layer100_n ; i++){
            if (layer100[i]==layer && eta100[i]==eta && phi100[i]==phi){
                in100=true;
                break;
            }
        }
        // find cluster size
        int nStrip = 0;
        //Get angle to wafer surface
        float phiToWafer(90.),thetaToWafer(90.);
        float sinAlpha = 0.; //for barrel, which is the only thing considered here
        float pTrack[3];
        pTrack[0] = trkp->momentum().x();
        pTrack[1] = trkp->momentum().y();
        pTrack[2] = trkp->momentum().z();
        int iflag = findAnglesToWaferSurface (pTrack, sinAlpha, surfaceID, thetaToWafer, phiToWafer );
        if ( iflag < 0) {
            ATH_MSG_WARNING("Error in finding track angles to wafer surface");
            continue; // Let's think about this (later)... continue, break or return?
        }
        // Fill profile
        m_phiVsNstrips[layer]->Fill(phiToWafer, nStrip, 1.);
        m_phiVsNstrips_Side[layer][side]->Fill(phiToWafer, nStrip, 1.);
        m_phiVsNstrips_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        m_phiVsNstrips_Side_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        if (in100) {
            m_phiVsNstrips_100[layer]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_Side_100[layer][side]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_100_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_Side_100_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        }else {
            m_phiVsNstrips_111[layer]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_Side_111[layer][side]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_111_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_Side_111_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        }
        /// first change the negative eta to positive eta index. This is needed to fill the array of histograms.
        int etaIndex(-1);
        if(eta == -1 ){
            etaIndex = 0;
        }
        else if(eta == 1){
            etaIndex = 1;
        }
        
        if(std::abs(eta)==1){
            m_phiVsNstrips_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_Side_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_Side_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            if (in100) {
                m_phiVsNstrips_100_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side_100_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_100_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side_100_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            }else {
                m_phiVsNstrips_111_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side_111_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_111_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_Side_111_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            }
        }
        
        /// selecting only eta < 0.5 cases
        if(std::abs(trkp->eta())>0.5)continue;
        m_phiVsNstrips_eta0p5[layer]->Fill(phiToWafer, nStrip, 1.);
        m_phiVsNstrips_eta0p5_Side[layer][side]->Fill(phiToWafer, nStrip, 1.);
        m_phiVsNstrips_eta0p5_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        m_phiVsNstrips_eta0p5_Side_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        if (in100) {
            m_phiVsNstrips_eta0p5_100[layer]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_Side_100[layer][side]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_100_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_Side_100_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        }else {
            m_phiVsNstrips_eta0p5_111[layer]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_Side_111[layer][side]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_111_pT[layer][pTIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_Side_111_pT[layer][side][pTIndex]->Fill(phiToWafer, nStrip, 1.);
        }
        if(std::abs(eta)==1){
            m_phiVsNstrips_eta0p5_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_Side_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            m_phiVsNstrips_eta0p5_Side_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            if (in100) {
                m_phiVsNstrips_eta0p5_100_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_Side_100_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_100_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_Side_100_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            }else {
                m_phiVsNstrips_eta0p5_111_eta[layer][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_Side_111_eta[layer][side][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_111_pT_eta[layer][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
                m_phiVsNstrips_eta0p5_Side_111_pT_eta[layer][side][pTIndex][etaIndex]->Fill(phiToWafer, nStrip, 1.);
            }
        }
      } // if((*it)->type(Trk::TrackStateOnSurface::Measurement)){
    } // end of for loop trackStates
    if(m_getTrackHoles){
        delete track;
        track=0;
    }
  }// end of loop on tracks
  m_numberOfEvents++;
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                             SCTLorentzMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTLorentzMonTool::procHistograms() {                                                                                                                //
                                                                                                                                                     // hidetoshi
                                                                                                                                                     // 14.01.21
  if (endOfRunFlag()) {
    ATH_MSG_DEBUG("finalHists()");
    ATH_MSG_DEBUG("Total Rec Event Number: " << m_numberOfEvents);
    ATH_MSG_DEBUG("Calling checkHists(true); true := end of run");
    if (checkHists(true).isFailure()) {
      ATH_MSG_WARNING("Error in checkHists(true)");
    }
  }
  ATH_MSG_DEBUG("Exiting finalHists");
  return StatusCode::SUCCESS;
}

StatusCode
SCTLorentzMonTool::checkHists(bool /*fromFinalize*/) {
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                              SCTLorentzMonTool :: bookLorentzHistos
// ====================================================================================================
StatusCode
SCTLorentzMonTool::bookLorentzHistos() {                                                                                                                
  const int nLayers(4);
  const int nSides(2);
  const int nEta(2);
  const int nPt(8);
  string stem = m_path + "/SCT/GENERAL/lorentz/";
  //    MonGroup Lorentz(this,m_path+"SCT/GENERAL/lorentz",expert,run);        // hidetoshi 14.01.21
  MonGroup Lorentz(this, m_path + "SCT/GENERAL/lorentz", run, ATTRIB_UNMANAGED);     // hidetoshi 14.01.21

  string hNum[nLayers] = {
    "0", "1", "2", "3"
  };
  string hNumS[nSides] = {
    "0", "1"
  };
  string hEtaS[nEta] = {
    "Minus1", "Plus1"
  };
  string hPt[nPt] = {
    "0", "1", "2", "3", "4", "5", "6", "7"   
  };
  string hPtTitle[nPt] = {
    "(0.5, 0.6)", "(0.6, 0.7)", "(0.7, 1.0)", "(1.0, 1.5)", "(1.5, 2.0)", "(2.0, 3.0)", "(3.0, 5.0)", "(5.0, inf)"   
  };
  
  int nProfileBins = 360;
  int success = 1;

  for (int l = 0; l != nLayers; ++l) {
    // granularity set to one profile/layer for now
    int iflag = 0;
    m_phiVsNstrips_100[l] = pFactory("h_phiVsNstrips_100" + hNum[l], "100 - Inc. Angle vs nStrips for Layer " + hNum[l],
                                    nProfileBins, -18., 18., Lorentz, iflag);
    m_phiVsNstrips_111[l] = pFactory("h_phiVsNstrips_111" + hNum[l], "111 - Inc. Angle vs nStrips for Layer " + hNum[l],
                                     nProfileBins, -18., 18., Lorentz, iflag);

    m_phiVsNstrips[l] = pFactory("h_phiVsNstrips" + hNum[l], "Inc. Angle vs nStrips for Layer" + hNum[l], nProfileBins,
                                -18., 18., Lorentz, iflag);
    m_phiVsNstrips[l]->GetXaxis()->SetTitle("#phi to Wafer");
    m_phiVsNstrips[l]->GetYaxis()->SetTitle("Num of Strips");


    //// old histograms/TProfiles
    m_phiVsNstrips_100[l]->GetXaxis()->SetTitle("#phi to Wafer");
    m_phiVsNstrips_100[l]->GetYaxis()->SetTitle("Num of Strips");

    m_phiVsNstrips_111[l]->GetXaxis()->SetTitle("#phi to Wafer");
    m_phiVsNstrips_111[l]->GetYaxis()->SetTitle("Num of Strips");
    
    /// defining profiles for etaModule
    for (int etaModule=0; etaModule != nEta; ++etaModule){
                
          // granularity set to eta for now
          m_phiVsNstrips_100_eta[l][etaModule] = pFactory("h_phiVsNstrips_100" + hNum[l] + "_EtaModule" + hEtaS[etaModule], "100 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and EtaModule " + hEtaS[etaModule], nProfileBins, -18., 18., Lorentz, iflag);
                
          m_phiVsNstrips_111_eta[l][etaModule] = pFactory("h_phiVsNstrips_111" + hNum[l] + "_EtaModule" + hEtaS[etaModule], "111 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and EtaModule " + hEtaS[etaModule], nProfileBins, -18., 18., Lorentz, iflag);
          //// old histograms/TProfiles
          m_phiVsNstrips_100_eta[l][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
          m_phiVsNstrips_100_eta[l][etaModule]->GetYaxis()->SetTitle("Num of Strips");

          m_phiVsNstrips_111_eta[l][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
          m_phiVsNstrips_111_eta[l][etaModule]->GetYaxis()->SetTitle("Num of Strips");
                

          m_phiVsNstrips_eta[l][etaModule] = pFactory("h_phiVsNstrips" + hNum[l] + "_EtaModule" + hEtaS[etaModule], "Inc. Angle vs nStrips for Layer" + hNum[l] + " and EtaModule " + hEtaS[etaModule], nProfileBins, -18., 18., Lorentz, iflag);
          m_phiVsNstrips_eta[l][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
          m_phiVsNstrips_eta[l][etaModule]->GetYaxis()->SetTitle("Num of Strips");
    }
    
    /// defining profiles for each pT chunk
    for(int pTChunk=0; pTChunk != nPt; ++pTChunk){
          m_phiVsNstrips_100_pT[l][pTChunk] = pFactory("h_phiVsNstrips_100" + hNum[l]+ "_pT" +hPt[pTChunk], "100 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk],
                                     nProfileBins, -18., 18., Lorentz, iflag);
          m_phiVsNstrips_111_pT[l][pTChunk] = pFactory("h_phiVsNstrips_111" + hNum[l]+ "_pT" +hPt[pTChunk], "111 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk],
                                     nProfileBins, -18., 18., Lorentz, iflag);

          m_phiVsNstrips_pT[l][pTChunk] = pFactory("h_phiVsNstrips" + hNum[l] + "_pT" +hPt[pTChunk], "Inc. Angle vs nStrips for Layer" + hNum[l] + " and pT "+hPtTitle[pTChunk], nProfileBins,
                                 -18., 18., Lorentz, iflag);
          m_phiVsNstrips_pT[l][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
          m_phiVsNstrips_pT[l][pTChunk]->GetYaxis()->SetTitle("Num of Strips");


          //// old histograms/TProfiles
          m_phiVsNstrips_100_pT[l][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
          m_phiVsNstrips_100_pT[l][pTChunk]->GetYaxis()->SetTitle("Num of Strips");

          m_phiVsNstrips_111_pT[l][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
          m_phiVsNstrips_111_pT[l][pTChunk]->GetYaxis()->SetTitle("Num of Strips");
          
          /// for each eta module
          for (int etaModule=0; etaModule!=nEta;++etaModule){
              m_phiVsNstrips_pT_eta[l][pTChunk][etaModule] = pFactory("h_phiVsNstrips" + hNum[l] + "_pT" +hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule], "Inc. Angle vs nStrips for Layer" + hNum[l] + " and pT "+hPtTitle[pTChunk] + " and EtaModule "+ hEtaS[etaModule], nProfileBins,
                                 -18., 18., Lorentz, iflag);
              m_phiVsNstrips_pT_eta[l][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
              m_phiVsNstrips_pT_eta[l][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
         
         
              m_phiVsNstrips_100_pT_eta[l][pTChunk][etaModule] = pFactory("h_phiVsNstrips_100" + hNum[l]+ "_pT" +hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule], "100 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk]+" and EtaModule "+hEtaS[etaModule],
                                     nProfileBins, -18., 18., Lorentz, iflag);
              m_phiVsNstrips_111_pT_eta[l][pTChunk][etaModule] = pFactory("h_phiVsNstrips_111" + hNum[l]+ "_pT" +hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule], "111 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk] + " EtaModule "+hEtaS[etaModule],
                                     nProfileBins, -18., 18., Lorentz, iflag);
              m_phiVsNstrips_100_pT_eta[l][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
              m_phiVsNstrips_100_pT_eta[l][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
              m_phiVsNstrips_111_pT_eta[l][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
              m_phiVsNstrips_111_pT_eta[l][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
          }
    }
        
    
    /// defining profiles for each side
    for (int side = 0; side < nSides; ++side) {
           m_phiVsNstrips_Side_100[l][side] = pFactory("h_phiVsNstrips_100_" + hNum[l] + "Side" + hNumS[side],
                                                  "100 - Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                  nProfileBins, -18., 18., Lorentz, iflag);
           m_phiVsNstrips_Side_111[l][side] = pFactory("h_phiVsNstrips_111_" + hNum[l] + "Side" + hNumS[side],
                                                  "111 - Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                  nProfileBins, -18., 18., Lorentz, iflag);
           m_phiVsNstrips_Side[l][side] = pFactory("h_phiVsNstrips" + hNum[l] + "Side" + hNumS[side],
                                              "Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                              nProfileBins, -18., 18., Lorentz, iflag);

           m_phiVsNstrips_Side[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
           m_phiVsNstrips_Side[l][side]->GetYaxis()->SetTitle("Num of Strips");

           m_phiVsNstrips_Side_100[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
           m_phiVsNstrips_Side_100[l][side]->GetYaxis()->SetTitle("Num of Strips");

           m_phiVsNstrips_Side_111[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
           m_phiVsNstrips_Side_111[l][side]->GetYaxis()->SetTitle("Num of Strips");

           for (int etaModule=0; etaModule != nEta; ++etaModule){
               m_phiVsNstrips_Side_100_eta[l][side][etaModule] = pFactory("h_phiVsNstrips_100_" + hNum[l] + "Side" + hNumS[side] + "_EtaModule" + hEtaS[etaModule],
                                                                   "100 - Inc. Angle vs nStrips for Layer Side EtaModule " + hNum[l] + hNumS[side] + hEtaS[etaModule],
                                                                   nProfileBins, -18., 18., Lorentz, iflag);
               m_phiVsNstrips_Side_111_eta[l][side][etaModule] = pFactory("h_phiVsNstrips_111_" + hNum[l] + "Side" + hNumS[side] + "_EtaModule" + hEtaS[etaModule],
                                                                   "111 - Inc. Angle vs nStrips for Layer Side EtaModule " + hNum[l] + hNumS[side] + hEtaS[etaModule],
                                                                   nProfileBins, -18., 18., Lorentz, iflag);
               m_phiVsNstrips_Side_eta[l][side][etaModule] = pFactory("h_phiVsNstrips" + hNum[l] + "Side" + hNumS[side] + "_EtaModule" + hEtaS[etaModule],
                                                               "Inc. Angle vs nStrips for Layer Side EtaModule " + hNum[l] + hNumS[side] + hEtaS[etaModule],
                                                               nProfileBins, -18., 18., Lorentz, iflag);

               m_phiVsNstrips_Side_eta[l][side][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
               m_phiVsNstrips_Side_eta[l][side][etaModule]->GetYaxis()->SetTitle("Num of Strips");

               m_phiVsNstrips_Side_100_eta[l][side][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
               m_phiVsNstrips_Side_100_eta[l][side][etaModule]->GetYaxis()->SetTitle("Num of Strips");

               m_phiVsNstrips_Side_111_eta[l][side][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
               m_phiVsNstrips_Side_111_eta[l][side][etaModule]->GetYaxis()->SetTitle("Num of Strips");
           }
      
           //// pT dependent profile plots
           for(int pTChunk=0; pTChunk != nPt; ++pTChunk){
                m_phiVsNstrips_Side_100_pT[l][side][pTChunk] = pFactory("h_phiVsNstrips_100_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk],
                                                  "100 - Inc. Angle vs nStrips for Layer Side pT " + hNum[l] + hNumS[side] + hPtTitle[pTChunk],
                                                  nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_Side_111_pT[l][side][pTChunk] = pFactory("h_phiVsNstrips_111_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk],
                                                  "111 - Inc. Angle vs nStrips for Layer Side pT " + hNum[l] + hNumS[side] + hPtTitle[pTChunk],
                                                  nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_Side_pT[l][side][pTChunk] = pFactory("h_phiVsNstrips" + hNum[l] + "Side" + hNumS[side] + "_pT" + hPt[pTChunk],
                                              "Inc. Angle vs nStrips for Layer Side pT " + hNum[l] + hNumS[side]+hPtTitle[pTChunk],
                                              nProfileBins, -18., 18., Lorentz, iflag);

                m_phiVsNstrips_Side_pT[l][side][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_Side_pT[l][side][pTChunk]->GetYaxis()->SetTitle("Num of Strips");

                m_phiVsNstrips_Side_100_pT[l][side][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_Side_100_pT[l][side][pTChunk]->GetYaxis()->SetTitle("Num of Strips");

                m_phiVsNstrips_Side_111_pT[l][side][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_Side_111_pT[l][side][pTChunk]->GetYaxis()->SetTitle("Num of Strips");
        
                /// pT dependent and eta module dependent profiles
                for (int etaModule = 0; etaModule != nEta; ++etaModule){
                    m_phiVsNstrips_Side_100_pT_eta[l][side][pTChunk][etaModule] = pFactory("h_phiVsNstrips_100_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule],
                                                  "100 - Inc. Angle vs nStrips for Layer Side pT EtaModule " + hNum[l] + hNumS[side] + hPtTitle[pTChunk]+hEtaS[etaModule],
                                                  nProfileBins, -18., 18., Lorentz, iflag);
                    m_phiVsNstrips_Side_111_pT_eta[l][side][pTChunk][etaModule] = pFactory("h_phiVsNstrips_111_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule],
                                                  "111 - Inc. Angle vs nStrips for Layer Side pT EtaModule " + hNum[l] + hNumS[side] + hPtTitle[pTChunk]+hEtaS[etaModule],
                                                  nProfileBins, -18., 18., Lorentz, iflag);
                    m_phiVsNstrips_Side_pT_eta[l][side][pTChunk][etaModule] = pFactory("h_phiVsNstrips" + hNum[l] + "Side" + hNumS[side] + "_pT" + hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule],
                                              "Inc. Angle vs nStrips for Layer Side pT EtaModule " + hNum[l] + hNumS[side]+hPtTitle[pTChunk]+hEtaS[etaModule],
                                              nProfileBins, -18., 18., Lorentz, iflag);

                    m_phiVsNstrips_Side_pT_eta[l][side][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                    m_phiVsNstrips_Side_pT_eta[l][side][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");

                    m_phiVsNstrips_Side_100_pT_eta[l][side][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                    m_phiVsNstrips_Side_100_pT_eta[l][side][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");

                    m_phiVsNstrips_Side_111_pT_eta[l][side][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                    m_phiVsNstrips_Side_111_pT_eta[l][side][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
          
          
                }
           }
    }
    
    
    /// for eta<0.5 histograms
    
    for (int l = 0; l != nLayers; ++l) {
        // granularity set to one profile/layer for now
        int iflag = 0;
        m_phiVsNstrips_eta0p5_100[l] = pFactory("h_phiVsNstrips_eta0p5_100" + hNum[l], "100 - Inc. Angle vs nStrips for Layer " + hNum[l],
                                        nProfileBins, -18., 18., Lorentz, iflag);
        m_phiVsNstrips_eta0p5_111[l] = pFactory("h_phiVsNstrips_eta0p5_111" + hNum[l], "111 - Inc. Angle vs nStrips for Layer " + hNum[l],
                                        nProfileBins, -18., 18., Lorentz, iflag);

        m_phiVsNstrips_eta0p5[l] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l], "Inc. Angle vs nStrips for Layer" + hNum[l], nProfileBins,
                                    -18., 18., Lorentz, iflag);
        m_phiVsNstrips_eta0p5[l]->GetXaxis()->SetTitle("#phi to Wafer");
        m_phiVsNstrips_eta0p5[l]->GetYaxis()->SetTitle("Num of Strips");


        //// old histograms/TProfiles
        m_phiVsNstrips_eta0p5_100[l]->GetXaxis()->SetTitle("#phi to Wafer");
        m_phiVsNstrips_eta0p5_100[l]->GetYaxis()->SetTitle("Num of Strips");

        m_phiVsNstrips_eta0p5_111[l]->GetXaxis()->SetTitle("#phi to Wafer");
        m_phiVsNstrips_eta0p5_111[l]->GetYaxis()->SetTitle("Num of Strips");
        
        /// defining profiles for etaModule
        for (int etaModule=0; etaModule != nEta; ++etaModule){
                    
            // granularity set to eta for now
            m_phiVsNstrips_eta0p5_100_eta[l][etaModule] = pFactory("h_phiVsNstrips_eta0p5_100" + hNum[l] + "_EtaModule" + hEtaS[etaModule], "100 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and EtaModule " + hEtaS[etaModule], nProfileBins, -18., 18., Lorentz, iflag);
                        
            m_phiVsNstrips_eta0p5_111_eta[l][etaModule] = pFactory("h_phiVsNstrips_eta0p5_111" + hNum[l] + "_EtaModule" + hEtaS[etaModule], "111 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and EtaModule " + hEtaS[etaModule], nProfileBins, -18., 18., Lorentz, iflag);
            //// old histograms/TProfiles
            m_phiVsNstrips_eta0p5_100_eta[l][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_100_eta[l][etaModule]->GetYaxis()->SetTitle("Num of Strips");

            m_phiVsNstrips_eta0p5_111_eta[l][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_111_eta[l][etaModule]->GetYaxis()->SetTitle("Num of Strips");
                        

            m_phiVsNstrips_eta0p5_eta[l][etaModule] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "_EtaModule" + hEtaS[etaModule], "Inc. Angle vs nStrips for Layer" + hNum[l] + " and EtaModule " + hEtaS[etaModule], nProfileBins, -18., 18., Lorentz, iflag);
            m_phiVsNstrips_eta0p5_eta[l][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_eta[l][etaModule]->GetYaxis()->SetTitle("Num of Strips");
        }
        
        /// defining profiles for each pT chunk
        for(int pTChunk=0; pTChunk != nPt; ++pTChunk){
            m_phiVsNstrips_eta0p5_100_pT[l][pTChunk] = pFactory("h_phiVsNstrips_eta0p5_100" + hNum[l]+ "_pT" +hPt[pTChunk], "100 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk],
                                            nProfileBins, -18., 18., Lorentz, iflag);
            m_phiVsNstrips_eta0p5_111_pT[l][pTChunk] = pFactory("h_phiVsNstrips_eta0p5_111" + hNum[l]+ "_pT" +hPt[pTChunk], "111 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk],
                                            nProfileBins, -18., 18., Lorentz, iflag);

            m_phiVsNstrips_eta0p5_pT[l][pTChunk] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "_pT" +hPt[pTChunk], "Inc. Angle vs nStrips for Layer" + hNum[l] + " and pT "+hPtTitle[pTChunk], nProfileBins,
                                        -18., 18., Lorentz, iflag);
            m_phiVsNstrips_eta0p5_pT[l][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_pT[l][pTChunk]->GetYaxis()->SetTitle("Num of Strips");


            //// old histograms/TProfiles
            m_phiVsNstrips_eta0p5_100_pT[l][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_100_pT[l][pTChunk]->GetYaxis()->SetTitle("Num of Strips");

            m_phiVsNstrips_eta0p5_111_pT[l][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_111_pT[l][pTChunk]->GetYaxis()->SetTitle("Num of Strips");
        
            /// for each eta module
            for (int etaModule=0; etaModule!=nEta;++etaModule){
                m_phiVsNstrips_eta0p5_pT_eta[l][pTChunk][etaModule] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "_pT" +hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule], "Inc. Angle vs nStrips for Layer" + hNum[l] + " and pT "+hPtTitle[pTChunk] + " and EtaModule "+ hEtaS[etaModule], nProfileBins,
                                        -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_pT_eta[l][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_pT_eta[l][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
                
                
                m_phiVsNstrips_eta0p5_100_pT_eta[l][pTChunk][etaModule] = pFactory("h_phiVsNstrips_eta0p5_100" + hNum[l]+ "_pT" +hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule], "100 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk]+" and EtaModule "+hEtaS[etaModule],
                                            nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_111_pT_eta[l][pTChunk][etaModule] = pFactory("h_phiVsNstrips_eta0p5_111" + hNum[l]+ "_pT" +hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule], "111 - Inc. Angle vs nStrips for Layer " + hNum[l] + " and pT " + hPtTitle[pTChunk] + " EtaModule "+hEtaS[etaModule],
                                            nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_100_pT_eta[l][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_100_pT_eta[l][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
                m_phiVsNstrips_eta0p5_111_pT_eta[l][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_111_pT_eta[l][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
            }
        }
        
    
        /// defining profiles for each side
        for (int side = 0; side < nSides; ++side) {
            m_phiVsNstrips_eta0p5_Side_100[l][side] = pFactory("h_phiVsNstrips_eta0p5_100_" + hNum[l] + "Side" + hNumS[side],
                                                        "100 - Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                        nProfileBins, -18., 18., Lorentz, iflag);
            m_phiVsNstrips_eta0p5_Side_111[l][side] = pFactory("h_phiVsNstrips_eta0p5_111_" + hNum[l] + "Side" + hNumS[side],
                                                        "111 - Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                        nProfileBins, -18., 18., Lorentz, iflag);
            m_phiVsNstrips_eta0p5_Side[l][side] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "Side" + hNumS[side],
                                                    "Inc. Angle vs nStrips for Layer Side " + hNum[l] + hNumS[side],
                                                    nProfileBins, -18., 18., Lorentz, iflag);

            m_phiVsNstrips_eta0p5_Side[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_Side[l][side]->GetYaxis()->SetTitle("Num of Strips");

            m_phiVsNstrips_eta0p5_Side_100[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_Side_100[l][side]->GetYaxis()->SetTitle("Num of Strips");

            m_phiVsNstrips_eta0p5_Side_111[l][side]->GetXaxis()->SetTitle("#phi to Wafer");
            m_phiVsNstrips_eta0p5_Side_111[l][side]->GetYaxis()->SetTitle("Num of Strips");

            for (int etaModule=0; etaModule != nEta; ++etaModule){
                m_phiVsNstrips_eta0p5_Side_100_eta[l][side][etaModule] = pFactory("h_phiVsNstrips_eta0p5_100_" + hNum[l] + "Side" + hNumS[side] + "_EtaModule" + hEtaS[etaModule],
                                                                        "100 - Inc. Angle vs nStrips for Layer Side EtaModule " + hNum[l] + hNumS[side] + hEtaS[etaModule],
                                                                        nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_Side_111_eta[l][side][etaModule] = pFactory("h_phiVsNstrips_eta0p5_111_" + hNum[l] + "Side" + hNumS[side] + "_EtaModule" + hEtaS[etaModule],
                                                                        "111 - Inc. Angle vs nStrips for Layer Side EtaModule " + hNum[l] + hNumS[side] + hEtaS[etaModule],
                                                                        nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_Side_eta[l][side][etaModule] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "Side" + hNumS[side] + "_EtaModule" + hEtaS[etaModule],
                                                                    "Inc. Angle vs nStrips for Layer Side EtaModule " + hNum[l] + hNumS[side] + hEtaS[etaModule],
                                                                    nProfileBins, -18., 18., Lorentz, iflag);

                m_phiVsNstrips_eta0p5_Side_eta[l][side][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_Side_eta[l][side][etaModule]->GetYaxis()->SetTitle("Num of Strips");

                m_phiVsNstrips_eta0p5_Side_100_eta[l][side][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_Side_100_eta[l][side][etaModule]->GetYaxis()->SetTitle("Num of Strips");

                m_phiVsNstrips_eta0p5_Side_111_eta[l][side][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_Side_111_eta[l][side][etaModule]->GetYaxis()->SetTitle("Num of Strips");
            }
            
            //// pT dependent profile plots
            for(int pTChunk=0; pTChunk != nPt; ++pTChunk){
                m_phiVsNstrips_eta0p5_Side_100_pT[l][side][pTChunk] = pFactory("h_phiVsNstrips_eta0p5_100_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk],
                                                        "100 - Inc. Angle vs nStrips for Layer Side pT " + hNum[l] + hNumS[side] + hPtTitle[pTChunk],
                                                        nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_Side_111_pT[l][side][pTChunk] = pFactory("h_phiVsNstrips_eta0p5_111_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk],
                                                        "111 - Inc. Angle vs nStrips for Layer Side pT " + hNum[l] + hNumS[side] + hPtTitle[pTChunk],
                                                        nProfileBins, -18., 18., Lorentz, iflag);
                m_phiVsNstrips_eta0p5_Side_pT[l][side][pTChunk] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "Side" + hNumS[side] + "_pT" + hPt[pTChunk],
                                                    "Inc. Angle vs nStrips for Layer Side pT " + hNum[l] + hNumS[side]+hPtTitle[pTChunk],
                                                    nProfileBins, -18., 18., Lorentz, iflag);

                m_phiVsNstrips_eta0p5_Side_pT[l][side][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_Side_pT[l][side][pTChunk]->GetYaxis()->SetTitle("Num of Strips");

                m_phiVsNstrips_eta0p5_Side_100_pT[l][side][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_Side_100_pT[l][side][pTChunk]->GetYaxis()->SetTitle("Num of Strips");

                m_phiVsNstrips_eta0p5_Side_111_pT[l][side][pTChunk]->GetXaxis()->SetTitle("#phi to Wafer");
                m_phiVsNstrips_eta0p5_Side_111_pT[l][side][pTChunk]->GetYaxis()->SetTitle("Num of Strips");
                
                /// pT dependent and eta module dependent profiles
                for (int etaModule = 0; etaModule != nEta; ++etaModule){
                    m_phiVsNstrips_eta0p5_Side_100_pT_eta[l][side][pTChunk][etaModule] = pFactory("h_phiVsNstrips_eta0p5_100_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule],
                                                            "100 - Inc. Angle vs nStrips for Layer Side pT EtaModule " + hNum[l] + hNumS[side] + hPtTitle[pTChunk]+hEtaS[etaModule],
                                                            nProfileBins, -18., 18., Lorentz, iflag);
                    m_phiVsNstrips_eta0p5_Side_111_pT_eta[l][side][pTChunk][etaModule] = pFactory("h_phiVsNstrips_eta0p5_111_" + hNum[l] + "Side" + hNumS[side] + "_pT"+hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule],
                                                            "111 - Inc. Angle vs nStrips for Layer Side pT EtaModule " + hNum[l] + hNumS[side] + hPtTitle[pTChunk]+hEtaS[etaModule],
                                                            nProfileBins, -18., 18., Lorentz, iflag);
                    m_phiVsNstrips_eta0p5_Side_pT_eta[l][side][pTChunk][etaModule] = pFactory("h_phiVsNstrips_eta0p5" + hNum[l] + "Side" + hNumS[side] + "_pT" + hPt[pTChunk]+"_EtaModule"+hEtaS[etaModule],
                                                        "Inc. Angle vs nStrips for Layer Side pT EtaModule " + hNum[l] + hNumS[side]+hPtTitle[pTChunk]+hEtaS[etaModule],
                                                        nProfileBins, -18., 18., Lorentz, iflag);

                    m_phiVsNstrips_eta0p5_Side_pT_eta[l][side][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                    m_phiVsNstrips_eta0p5_Side_pT_eta[l][side][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");

                    m_phiVsNstrips_eta0p5_Side_100_pT_eta[l][side][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                    m_phiVsNstrips_eta0p5_Side_100_pT_eta[l][side][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");

                    m_phiVsNstrips_eta0p5_Side_111_pT_eta[l][side][pTChunk][etaModule]->GetXaxis()->SetTitle("#phi to Wafer");
                    m_phiVsNstrips_eta0p5_Side_111_pT_eta[l][side][pTChunk][etaModule]->GetYaxis()->SetTitle("Num of Strips");
                }
            }
        }
    }
    success *= iflag;
  }
  if (success == 0){
    return StatusCode::FAILURE;
  }                                                                                                                 //
  return StatusCode::SUCCESS;
}

TProfile *
SCTLorentzMonTool::pFactory(const std::string &name, const std::string &title, int nbinsx, float xlow, float xhigh,
                            MonGroup &registry, int &iflag) {
  Prof_t tmp = new TProfile(TString(name), TString(title), nbinsx, xlow, xhigh);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    if (msgLvl(MSG::ERROR)) {
      msg(MSG::ERROR) << "Cannot book SCT histogram: " << name << endmsg;
    }
    iflag = 0;
  }else {
    iflag = 1;
  }

  return tmp;
}

bool
SCTLorentzMonTool::h1Factory(const std::string &name, const std::string &title, const float extent, MonGroup &registry,
                             VecH1_t &storageVector) {
  const unsigned int nbins(100);
  const float lo(-extent), hi(extent);
  H1_t tmp = new TH1F(TString(name), TString(title), nbins, lo, hi);
  bool success(registry.regHist(tmp).isSuccess());

  if (not success) {
    if (msgLvl(MSG::ERROR)) {
      msg(MSG::ERROR) << "Cannot book SCT histogram: " << name << endmsg;
    }
  }
  storageVector.push_back(tmp);
  return success;
}

int
SCTLorentzMonTool::findAnglesToWaferSurface(const float (&vec)[3], const float &sinAlpha, const Identifier &id,
                                            float &theta, float &phi) {
  int iflag(-1);

  phi = 90.;
  theta = 90.;

  InDetDD::SiDetectorElement *element = m_sctmgr->getDetectorElement(id);
  if (!element) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "findAnglesToWaferSurface:  failed to find detector element for id=" <<
      m_pSCTHelper->show_to_string(id) << endmsg;
    return iflag;
  }

  float cosAlpha = sqrt(1. - sinAlpha * sinAlpha);
  float phix = cosAlpha * element->phiAxis().x() + sinAlpha * element->phiAxis().y();
  float phiy = -sinAlpha *element->phiAxis().x() + cosAlpha * element->phiAxis().y();

  float pNormal = vec[0] * element->normal().x() + vec[1] * element->normal().y() + vec[2] * element->normal().z();
  float pEta = vec[0] * element->etaAxis().x() + vec[1] * element->etaAxis().y() + vec[2] * element->etaAxis().z();
  float pPhi = vec[0] * phix + vec[1] * phiy + vec[2] * element->phiAxis().z();

  if (pPhi < 0.) {
    phi = -90.;
  }
  if (pEta < 0.) {
    theta = -90.;
  }
  if (pNormal != 0.) {
    phi = atan(pPhi / pNormal) / CLHEP::deg;
    theta = atan(pEta / pNormal) / CLHEP::deg;
  }
  iflag = 1;
  return iflag;
}
