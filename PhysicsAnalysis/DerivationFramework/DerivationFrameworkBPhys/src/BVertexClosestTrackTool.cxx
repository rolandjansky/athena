/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BVertexClosestTrackTool.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add B vertex closest track information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
// Job options provided by this class:
// - CloseTrackChi2SetName      -- list with labels for the following
//                                 four settings (all five lists must
//                                 be of exactly same length)
// - CloseTrackCorrChi2         -- list with options for using the
//                                 SV uncertainties in the chi2 calculation
//                                 in addition to track uncertainties
//                                   0 : use old method
//                                       (only track uncertainties)
//                                   1 : from track perigee with
//                                       uncertainties from track and vertex
//                                   2 : simple extrapolation from track
//                                       parameters with uncertainties from
//                                       track and vertex (extrapolation
//                                       used for track swimming)
// - CloseTrackMinDCAin3D       -- use 3-dimensional information in
//                                 minimization (list)
// - CloseTrackMaxLogChi2       -- maximum chi2 distance of closest track
//                                 to B vertex (list)
// - NCloseTrackMaxLogChi2      -- maximum chi2 distance of track
//                                 to B vertex for track counting (list)
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/BVertexClosestTrackTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHelper.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "AthLinks/ElementLink.h"

#include "boost/format.hpp"
#include "TVector3.h"
#include <algorithm>
#include <sstream>

namespace DerivationFramework {

  //-------------------------------------------------------------------------
  //
  // helper class
  BVertexClosestTrackTool::CtItem::CtItem(std::string Name, std::string Prefix,
					  std::string Bname,
					  double Dca, double DcaErr,
					  double Zca, double ZcaErr,
            double VtxNDErr2, double TrkNDErr2, double Phi0Used,
					  int    NTrksChi2,
            xAOD::TrackParticle* CloseTrack,
            TrackBag Tracks,
            std::vector<std::vector<double> > Vtap,
            std::vector<unsigned short> Selpat)
    : BaseItem(Name, Bname, Prefix), dca(Dca), dcaErr(DcaErr),
      zca(Zca), zcaErr(ZcaErr), vtxNDErr2(VtxNDErr2), trkNDErr2(TrkNDErr2),
      phi0Used(Phi0Used),
      nTrksChi2(NTrksChi2), closeTrack(CloseTrack),
      tracks(Tracks), vtap(Vtap), selpat(Selpat) {
  }

  BVertexClosestTrackTool::CtItem::~CtItem() {
  }
  
  void BVertexClosestTrackTool::CtItem::setup(std::string Name,
					      std::string Bname,
					      std::string Prefix) {
    BaseItem::setup(Name, Bname, Prefix);
    dca        = -999.;
    dcaErr     = -99.;
    zca        = -999.;
    zcaErr     = -99.;
    vtxNDErr2  = -99.;
    trkNDErr2  = -99.;
    phi0Used   = -999.;
    nTrksChi2  = 0;
    closeTrack = NULL;
    tracks.clear();
    vtap.clear();
    selpat.clear();
  }
  
  void BVertexClosestTrackTool::CtItem::setup(std::string Name,
					      std::string Bname,
					      std::string Prefix, 
					      double Dca, double DcaErr,
					      double Zca, double ZcaErr,
                double VtxNDErr2, double TrkNDErr2, double Phi0Used,
					      int    NTrksChi2,
					      xAOD::TrackParticle*
                CloseTrack,
                TrackBag Tracks,
                std::vector<std::vector<double> > Vtap,
                std::vector<unsigned short> Selpat) {
    BaseItem::setup(Name, Bname, Prefix);
    dca        = Dca;
    dcaErr     = DcaErr;
    zca        = Zca;
    zcaErr     = ZcaErr;
    vtxNDErr2  = VtxNDErr2;
    trkNDErr2  = TrkNDErr2;
    phi0Used   = Phi0Used;
    nTrksChi2  = NTrksChi2;
    closeTrack = CloseTrack;
    tracks     = Tracks;
    vtap       = Vtap;
    selpat     = Selpat;
  }

  void BVertexClosestTrackTool::CtItem::resetVals() {
    dca        = -999.;
    dcaErr     =  -99.;
    zca        = -999.;
    zcaErr     =  -99.;
    vtxNDErr2  =  -99.;
    trkNDErr2  =  -99.;
    phi0Used   = -999.;
    nTrksChi2  =    0;
    closeTrack = NULL;
    tracks.clear();
    vtap.clear();
    selpat.clear();
  }

  void BVertexClosestTrackTool::CtItem::copyVals(const BaseItem& item) {
    copyVals((const CtItem&)item);
  }
    
  void BVertexClosestTrackTool::CtItem::copyVals(const CtItem& item) {
    dca        = item.dca;
    dcaErr     = item.dcaErr;
    zca        = item.zca;
    zcaErr     = item.zcaErr;
    vtxNDErr2  = item.vtxNDErr2;
    trkNDErr2  = item.trkNDErr2;
    phi0Used   = item.phi0Used;
    nTrksChi2  = item.nTrksChi2;
    closeTrack = item.closeTrack;
    tracks     = item.tracks;
    vtap       = item.vtap;
    selpat     = item.selpat;
  }
  
  std::string BVertexClosestTrackTool::CtItem::dcaName() {
    return buildName("DCA");
  }
  
  std::string BVertexClosestTrackTool::CtItem::dcaErrName() {
    return buildName("DCAError");
  }
  
  std::string BVertexClosestTrackTool::CtItem::zcaName() {
    return buildName("ZCA");
  }
  
  std::string BVertexClosestTrackTool::CtItem::zcaErrName() {
    return buildName("ZCAError");
  }
  
  std::string BVertexClosestTrackTool::CtItem::vtxNDErr2Name() {
    return buildName("VtxNDError2");
  }

  std::string BVertexClosestTrackTool::CtItem::trkNDErr2Name() {
    return buildName("TrkNDError2");
  }

  std::string BVertexClosestTrackTool::CtItem::phi0UsedName() {
    return buildName("Phi0Used");
  }

  std::string BVertexClosestTrackTool::CtItem::nTrksChi2Name() {
    return buildName("NTracksChi2");
  }

  std::string BVertexClosestTrackTool::CtItem::closeTrackName() {
    return buildName("CloseTrack", "_Link");
  }
  
  std::string BVertexClosestTrackTool::CtItem::toString() const {
    boost::format f1("dca: %10.6f %10.6f zca: %10.6f %10.6f nt: %10d");
    f1 % dca % dcaErr % zca % zcaErr % nTrksChi2;
    boost::format f2("%s\n  %s\n");
    f2 % BPhysVertexTrackBase::BaseItem::toString();
    f2 % f1.str();
    std::string rstr = f2.str();
    rstr += "per track: p(px, py, pz)\n";
    rstr += "           d(d0, z0, phi, theta, qoverp)\n";
    rstr += "           d0, d0Err, z0, z0Err, logChi2, dca, okFlag\n";
    rstr += "           vtxNDErr2, trkNDErr2, phi0Used\n";
    rstr += "           vtxNDErr, trkNDErr, log(chi2Err2Sum)\n";
    // loop over tracks
    if (tracks.size() == vtap.size() && vtap.size() == selpat.size()) {
      for (unsigned int i=0; i<tracks.size(); ++i) {
        boost::format f3("  %3d %2d ");
        f3 % i % selpat[i];
        std::string f3str = f3.str();
        // 0: d0, 1: d0Err, 2: z0, 3: z0Err, 4: logChi2, 5: dca, 6: okFlag
        // 7: vtxNDErr2, 8: trkNDErr2, 9: phi0Used
        boost::format f4("%s\nd0: %10.4f %10.4f z0: %10.4f %10.4f "
                         "lc2: %10.4f dca: %10.4f ok: %3f\n"
                         "vtxNDErr2: %10.4f trkNDErr2: %10.4f "
                         "phi0Used: %10.4f\n"
                         "vtxNDErr: %10.4f trkNDErr2 %10.4f "
                         "logChi2Err2Sum: %10.4f");
        f4 % trackToString(tracks[i]);
        f4 % vtap[i][0] % vtap[i][1] % vtap[i][2] % vtap[i][3];
        f4 % vtap[i][4] % vtap[i][5] % vtap[i][6];
        f4 % vtap[i][7] % vtap[i][8] % vtap[i][9];
        f4 % (vtap[i][7] < 0. ? -99. : sqrt(vtap[i][7]));
        f4 % (vtap[i][8] < 0. ? -99. : sqrt(vtap[i][8]));
        f4 % (vtap[i][7]+vtap[i][8] > 0. ?
              log(vtap[i][5]*vtap[i][5]/(vtap[i][7]+vtap[i][8])) : -999.);
        std::string tstr = wrapLines(f4.str(),
                                     std::string(f3str.length(), ' '));
        tstr.replace(0,f3str.length(),f3str);
        rstr.append(tstr+"\n");
      } // for i
    } else {
      boost::format f5("Mismatch: nTracks: %d nVtap: %d nSelpat: %d\n");
      f5 % tracks.size() % vtap.size() % selpat.size();
      rstr.append(f5.str());
    } // if sizes

    rstr.erase(rstr.length()-1);
    return rstr;
  }

  //--------------------------------------------------------------------------
  BVertexClosestTrackTool::BVertexClosestTrackTool(const std::string& t,
					   const std::string& n,
					   const IInterface*  p)
    : BPhysVertexTrackBase(t,n,p), m_lastRunNumber(0), m_lastEvtNumber(0),
      m_svIdx(0) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("CloseTrackChi2SetName", m_closeTrackChi2SetName = {"def"});
    declareProperty("CloseTrackCorrChi2"   , m_closeTrackCorrChi2    = {0});
    declareProperty("CloseTrackMinDCAin3D" , m_minDCAin3D            = {true});
    declareProperty("CloseTrackMaxLogChi2" , m_closeTrackMaxLogChi2  = {-1.});
    declareProperty("NCloseTrackMaxLogChi2", m_nCloseTrackMaxLogChi2 = {-1.});
  }
  //--------------------------------------------------------------------------
  StatusCode BVertexClosestTrackTool::initializeHook() {
  
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initializeHook() -- begin");

    // check job options
    if ( m_closeTrackChi2SetName.size() == 0 ) {
      ATH_MSG_ERROR("No chi2 set names provided!");
    }
    if ( m_closeTrackCorrChi2.size() == 0 ) {
      ATH_MSG_ERROR("No use of corrected chi2 settings provided!");
    }
    if ( m_minDCAin3D.size() == 0 ) {
      ATH_MSG_ERROR("No use of min DCA in 3D settings provided!");
    }
    if ( m_closeTrackMaxLogChi2.size() == 0 ) {
      ATH_MSG_ERROR("No cuts on max log chi2 for DOCA calculation provided!");
    }
    if ( m_nCloseTrackMaxLogChi2.size() == 0 ) {
      ATH_MSG_ERROR("No cuts on max log chi2 for nClosetTracks calculation "
                    "provided!");
    }
    if ( m_closeTrackCorrChi2.size()    != m_closeTrackChi2SetName.size() ||
         m_minDCAin3D.size()            != m_closeTrackChi2SetName.size() ||
         m_closeTrackMaxLogChi2.size()  != m_closeTrackChi2SetName.size() ||
         m_nCloseTrackMaxLogChi2.size() != m_closeTrackChi2SetName.size() ) {
      ATH_MSG_ERROR("Size mismatch of CloseTrackChi2SetName ("
                    <<  m_closeTrackChi2SetName.size() << "), "
                    << "CloseTrackCorrChi2 ("
                    << m_closeTrackCorrChi2 << "), "
                    << "CloseTrackMinDCAin3D ("
                    << m_minDCAin3D.size() << "), "
                    << "CloseTrackMaxLogChi2 ("
                    << m_closeTrackMaxLogChi2.size() << ") and/or "
                    << "NCloseTrackMaxLogChi2 ("
                    << m_nCloseTrackMaxLogChi2.size() << ")");
    }

    // initialize results array
    initResults();

    ATH_MSG_DEBUG("BVertexClosestTrackTool::initializeHook() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BVertexClosestTrackTool::finalizeHook() {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::finalizeHook()");

    // everything all right
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode
  BVertexClosestTrackTool::addBranchesVCSetupHook(size_t ivc) const {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesVCLoopHook() -- begin");

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesVCSetupHook: "
		  << "Vertex container index " << ivc
		  << " for collection " << m_vertexContainerNames[ivc]
		  << " with prefix " << m_branchPrefixes[ivc]);
    
    setResultsPrefix(m_branchPrefixes[ivc]);
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesVCSetupHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }  

  //--------------------------------------------------------------------------
  StatusCode
  BVertexClosestTrackTool::addBranchesSVLoopHook(const xAOD::Vertex* vtx) const {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook() -- begin");

    // calculate closest track values
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook(): "
		  "calculate closest track ...");
    CHECK(calculateValues(vtx));
      
    // save the closest track values
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook(): "
		  "save closest track ...");
    CHECK(saveClosestTrack(vtx));

    // dump close track item debugging information
    if (m_debugTracksInThisEvent) {
      CHECK(logCloseTracksDebugInfo());
    }
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::addBranchesSVLoopHook() -- end");
   
    // nothing to do here
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Calculate closest track variables -- method called from caching loop
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexClosestTrackTool::calcValuesHook(const xAOD::Vertex* vtx,
					  const unsigned int ipv,
					  const unsigned int its,
					  const unsigned int itt) const {
    
    ATH_MSG_DEBUG("calcValuesHook:  ipv: " << ipv
		  << ", its: " << its << ", itt: " << itt);
 
    // candidate tracks and momentum
    xAOD::BPhysHelper   cand(vtx);

    // tracks to be considered
    TrackBag tracks = selectTracks(m_tracks, cand, ipv, its, itt);
    
    // loop over chi2 setting sets
    unsigned int nChi2Sets = m_closeTrackChi2SetName.size();
    for (unsigned int ics = 0; ics < nChi2Sets; ++ics) {

      CtItem& cti = m_results[its][ipv][itt][ics];

      // presets
      cti.resetVals();

      double       closestTrkDCA = 9999.;
      int          closestTrkIdx(-1);
      unsigned int trkIdx(0);
      for (TrackBag::const_iterator trkItr = tracks.begin();
           trkItr != tracks.end(); ++trkItr, ++trkIdx) {

        //
        // track selection bit pattern:
        // bit 0 : included in number of close tracks
        // bit 1 : chosen as closest track
        //
        unsigned short selpat(0);

        // Returned vector components:
        // 0: d0, 1: d0Err, 2: z0, 3: z0Err, 4: logChi2, 5: dca, 6: okFlag
        // 7: vtxErrPart2, 8: trkErrPart2, 9: phi0Used
        std::vector<double> vtap =
          getTrackLogChi2DCA(*trkItr, cand.vtx(),
                             m_minDCAin3D[ics], m_closeTrackCorrChi2[ics]);
        ATH_MSG_DEBUG("calcValuesHook: track: " << *trkItr
                      << ", logChi2: " << vtap[4] << ", dca: " << vtap[5]);

        // track values at perigee found?
        if ( vtap[6] >= 0. ) {
          ATH_MSG_DEBUG("calcValuesHook: checking track count for "
                        "m_nCloseTrackMaxLogChi2[ics] = "
                        << m_nCloseTrackMaxLogChi2[ics]);
          // count tracks
          if ( vtap[4] < m_nCloseTrackMaxLogChi2[ics] ) {
            cti.nTrksChi2++;
            selpat |= 1;
            ATH_MSG_DEBUG("calcValuesHook: nTrksChi2++ for track " << *trkItr);
          }
          // find closest track
          ATH_MSG_DEBUG("calcValuesHook: log(chi2) check: "
                        "m_closeTrackMaxLogChi2[ics]: "
                        << m_closeTrackMaxLogChi2[ics]
                        << ", logChi2: " << vtap[4]
                        << ", closestTrkDCA: " << closestTrkDCA
		      << ", dca: " << fabs(vtap[5]));	
          if ( fabs(vtap[5]) < closestTrkDCA &&
               vtap[4] < m_closeTrackMaxLogChi2[ics] ) {
            closestTrkDCA  = fabs(vtap[5]);
            cti.dca        = vtap[0];
            cti.dcaErr     = vtap[1];
            cti.zca        = vtap[2];
            cti.zcaErr     = vtap[3];
            cti.vtxNDErr2  = vtap[7];
            cti.trkNDErr2  = vtap[8];
            cti.phi0Used   = vtap[9];
            cti.closeTrack = *trkItr;
            closestTrkIdx  = trkIdx;
            ATH_MSG_DEBUG("calcValuesHook: closestTrkDCA now: "
                          << closestTrkDCA
                          << " for track " << *trkItr);
          }
        } // if ok
        cti.tracks.push_back(*trkItr);
        cti.vtap.push_back(vtap);
        cti.selpat.push_back(selpat);
      } // for tracks
      // mark closest track
      if (closestTrkIdx > -1 && closestTrkIdx < (int)cti.selpat.size()) {
        cti.selpat[closestTrkIdx] |= 2;
      }
    } // for ics
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Fill closest track values from cache if found
  //--------------------------------------------------------------------------  
  bool BVertexClosestTrackTool::fastFillHook(const xAOD::Vertex* vtx,
					     const int ipv) const {

    ATH_MSG_DEBUG("fastFillHook: ipv: " << ipv);
    
    bool found(false);
    
    StringIntMap_t::iterator itpv =
      m_pvAssocResMap.find(buildPvAssocCacheName(vtx, ipv));
    if ( itpv != m_pvAssocResMap.end() ) {
      found = true;
      unsigned int nTrackSels  = m_trackSelectionTools.size();
      unsigned int nTrackTypes = m_useTrackTypes.size();
      unsigned int nChi2Sets   = m_closeTrackChi2SetName.size();
      for (unsigned int its = 0; its < nTrackSels; ++its) {
        for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
          for (unsigned int ics = 0; ics < nChi2Sets; ++ics) {
            m_results[its][ipv][itt][ics]
              .copyVals(m_results[its][itpv->second][itt][ics]);
          } // for ics
        } // for its
      } // for itt
    } // if found

    ATH_MSG_DEBUG("fastFillHook: cache index: "
		  << buildPvAssocCacheName(vtx, ipv)
		  << ", found ? " << found
		  << ", ipv_ref: "
		  << (found ? itpv->second : -1));

    return found;
  }
  //--------------------------------------------------------------------------  
  StatusCode
  BVertexClosestTrackTool::saveClosestTrack(const xAOD::Vertex* vtx) const {

    typedef ElementLink< xAOD::TrackParticleContainer > TrackParticleLink_t;
    
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();
    unsigned int nChi2Sets   = m_closeTrackChi2SetName.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
        for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
          for (unsigned int ics = 0; ics < nChi2Sets; ++ics) {
            CtItem result = m_results[its][ipv][itt][ics];
            SG::AuxElement::Decorator< float >
              d_dca_value(result.dcaName());
            SG::AuxElement::Decorator< float >
              d_dcaErr_value(result.dcaErrName());
            SG::AuxElement::Decorator< float >
              d_zca_value(result.zcaName());
            SG::AuxElement::Decorator< float >
              d_zcaErr_value(result.zcaErrName());
            SG::AuxElement::Decorator< int >
              d_nTrksChi2_value(result.nTrksChi2Name());
            d_dca_value(*vtx)       = result.dca;
            d_dcaErr_value(*vtx)    = result.dcaErr;
            d_zca_value(*vtx)       = result.zca;
            d_zcaErr_value(*vtx)    = result.zcaErr;
            d_nTrksChi2_value(*vtx) = result.nTrksChi2;
            ATH_MSG_DEBUG("BVertexClosestTrackTool::saveClosestTrack() "
                          << "-- dca: " << result.dcaName()
                          << ", dcaErr: " << result.dcaErrName()
                          << ", zca: " << result.zcaName()
                          << ", zcaErr: " << result.zcaErrName()
                          << ", nTrksChi2: " << result.nTrksChi2Name());
            ATH_MSG_DEBUG("BVertexClosestTrackTool::saveClosestTrack() "
                          << "-- vertex: ("
                          << vtx->x() << ", "
                          << vtx->y() << ", "
                          << vtx->z() << ")"
                          << ", dca: " << result.dca
                          << ", dcaErr: " << result.dcaErr
                          << ", zca: " << result.zca
                          << ", zcaErr: " << result.zcaErr
                          << ", nTrksChi2: " << result.nTrksChi2);
            // add ElementLink to closest track
            std::string linkName = result.closeTrackName();
            SG::AuxElement::Decorator<TrackParticleLink_t>
              tpLinkDecor(linkName);
            TrackParticleLink_t tpLink;
            if ( result.closeTrack != NULL ) {
              tpLink.toContainedElement( *m_tracks, result.closeTrack );
            }
            ATH_MSG_DEBUG("saveClosestTrack: Decorate vtx "
                          << vtx << " with " << linkName
                          << ", closeTrkPtr: " << result.closeTrack);
            tpLinkDecor(*vtx) = tpLink;
            if ( tpLink.isValid() ) {
              ATH_MSG_DEBUG("saveClosestTrack: Decorated vtx "
                            << vtx << " with " << linkName
                            << ", closeTrkPtr: " << result.closeTrack);
            } else {
              ATH_MSG_VERBOSE("saveClosestTrack: Failed to decorate vtx "
                              << vtx << " with " << linkName
                              << ", closeTrkPtr: "
                              << result.closeTrack << " !");
            }
            // if valid
          } // for ics
        } // for itt
      } // for ipv
    } // for its
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  void BVertexClosestTrackTool::setResultsPrefix(std::string prefix) const {

    ATH_MSG_DEBUG("BVertexClosestTrackTool::setResultsPrefix -- begin");

    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();
    unsigned int nChi2Sets   = m_closeTrackChi2SetName.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
        for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
          for (unsigned int ics = 0; ics < nChi2Sets; ++ics) {
            m_results[its][ipv][itt][ics].setPrefix(prefix);
          } // for ics
        } // for itt
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::setResultsPrefix -- end");
  }
  //--------------------------------------------------------------------------
  void BVertexClosestTrackTool::initResults() {
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- begin");

    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();
    unsigned int nChi2Sets   = m_closeTrackChi2SetName.size();
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : nTrackSels = "
		  << nTrackSels);
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : nPvAssocs = "
		  << nPvAssocs);
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : nTrackTypes = "
		  << nTrackTypes);
    m_results.resize(boost::extents[nTrackSels][nPvAssocs][nTrackTypes][nChi2Sets]);
    for (unsigned int its = 0; its < nTrackSels; ++its) {
      ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- its = " << its);
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
        ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- ipv = " << ipv);
        for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
            ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- itt = "
                          << itt);
          for (unsigned int ics = 0; ics < nChi2Sets; ++ics) {
            ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- ics = "
                          << ics);
            std::string csname = m_closeTrackChi2SetName[ics];
            ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults : "
                          << m_branchBaseName << ", "
                          << buildBranchBaseName(its, ipv, itt, csname));
            m_results[its][ipv][itt][ics].setup(buildBranchBaseName(its, ipv,
                                                                    itt,
                                                                    csname),
                                                m_branchBaseName);
          } // for ics
        } // for itt
      } // for ipv
    } // for its
    
    ATH_MSG_DEBUG("BVertexClosestTrackTool::initResults -- end");
  }
  //--------------------------------------------------------------------------
  // Dump CloseTracks debug information to log file
  //--------------------------------------------------------------------------  
  StatusCode BVertexClosestTrackTool::logCloseTracksDebugInfo() const {

    // Count candidates
    if (m_runNumber != m_lastRunNumber || m_evtNumber != m_lastEvtNumber) {
      m_lastRunNumber = m_runNumber;
      m_lastEvtNumber = m_evtNumber;
      m_svIdx = 0;
    } else {
      m_svIdx++;
    }

    std::string str(">>>>> logCloseTracksDebugInfo:\n");
    boost::format f("Run %d  Event %d  SV %d\n");
    f % m_runNumber % m_evtNumber % m_svIdx;
    str.append(f.str());
    
    unsigned int nTrackSels  = m_trackSelectionTools.size();
    unsigned int nPvAssocs   = m_pvAssocTypes.size();
    unsigned int nTrackTypes = m_useTrackTypes.size();
    unsigned int nChi2Sets   = m_closeTrackChi2SetName.size();

    for (unsigned int its = 0; its < nTrackSels; ++its) {
      for (unsigned int ipv = 0; ipv < nPvAssocs; ++ipv) {
        for (unsigned int itt = 0; itt < nTrackTypes; ++itt) {
          for (unsigned int ics = 0; ics < nChi2Sets; ++ics) {
            boost::format f1("its: %d ipv: %d itt: %d ics: %d\n");
            f1 % its % ipv % itt % its;
            str.append(f1.str()); 
            CtItem result = m_results[its][ipv][itt][ics];
            str.append(result.toString()+"\n");
          } // for ics
        } // for itt
      } // for ipv
    } // for its

    str.append("<<<<< logCloseTracksDebugInfo");
    ATH_MSG_INFO(str);
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
}
