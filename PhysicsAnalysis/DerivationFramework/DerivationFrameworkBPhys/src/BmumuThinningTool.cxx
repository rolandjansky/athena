/* 
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   BmumuThinningTool.cxx
 * @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 *
 */

#include "DerivationFrameworkBPhys/BmumuThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxTypeRegistry.h"

#include <string>
#include <sstream>
#include <istream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <regex>
#include <boost/algorithm/string.hpp>

namespace DerivationFramework {

  // static members
  // Note: may later be migrated to xAODBPhys/BPhysHelper
  std::map<xAOD::BPhysHelper::pv_type, std::string>
  BmumuThinningTool::PvTypeToVarNameMap =
    { {xAOD::BPhysHelper::PV_MAX_SUM_PT2, "MaxSumPt2"},
      {xAOD::BPhysHelper::PV_MIN_A0     , "MinA0"    },
      {xAOD::BPhysHelper::PV_MIN_Z0     , "MinZ0"    },
      {xAOD::BPhysHelper::PV_MIN_Z0_BA  , "MinZ0BA"  } };
  
  //--------------------------------------------------------------------------
  // Constructor
  //--------------------------------------------------------------------------
  BmumuThinningTool::BmumuThinningTool(const std::string& t,
				       const std::string& n,
				       const IInterface* p) :
    CfAthAlgTool(t, n, p),
    m_thinningSvc("ThinningSvc", n),
    m_doCloseTracks(false),
    m_doPVs(false),
    m_doRefPVs(false),
    m_doMuons(false),
    m_doCalMuons(false),
    m_doTracks(false) {

    declareInterface<DerivationFramework::IThinningTool>(this);

    // thinning service
    declareProperty("ThinningService"         , m_thinningSvc);
    // TrackParticle container name
    declareProperty("TrackParticleContainerName",
		    m_trkPartContName = "InDetTrackParticles"); 
    // list of secondary vertex container names
    declareProperty("VertexContainerNames"    ,  m_vtxContNames);
    // list of pass flags for the seconary vertices
    // empty list lets all vertices pass
    // list length needs to be identical to length of
    // VertexContainerNames list if AlignPassToVertexList is True
    declareProperty("VertexPassFlags"         ,  m_vtxPassFlags);
    // align VertexPassFlags to VertexContainerNames list?
    // This option causes a 1:1 correlation between the two lists,
    // i.e. a flag is only applied to the corresponding container
    // if this option is set to True. (default: false)
    declareProperty("AlignPassToVertexList",
		    m_alignPassToVertexList = false);
    // Primary vertex container name
    declareProperty("PVContainerName"         , m_PVContName);
    // Refitted primary vertex container names
    // This list must be of same length and order as the m_vtxContNames list
    // (or empty => no thinning of refitted primary vertex containers)
    declareProperty("RefPVContainerNames"     , m_refPVContNames);
    // name of the used muon container
    declareProperty("MuonContainerName"       , m_muonContName = "");
    // name of the calibrated muons container
    declareProperty("CalibMuonContainerName"  , m_calMuonContName = "");
    // closest track branch base name
    declareProperty("CloseTrackBranchBaseName", m_ctBranchBaseName);
    // closest track branch prefixes
    declareProperty("CloseTrackBranchPrefixes", m_ctBranchPrefixes);
    // keep tracks for selected (refitted) primary vertices
    declareProperty("KeepTracksForSelectedPVs", m_keepPVTracks = false);
    // match vertex muons with calibrated muons
    declareProperty("MatchCalibratedMuons"    , m_matchCalMuons = false);
    // mark orginal muons for matched calibrated muons as well
    // (only makes sense if MatchCalibratedMuons = True)
    declareProperty("MarkMatchedMuons"        , m_markMuons = false);
    // mark calibrated muons for matched original muons as well
    // (only makes sense if MatchCalibratedMuons = False)
    declareProperty("MarkMatchedCalMuons"     , m_markCalMuons = false);
    // sync marked muons both ways (forces it)
    declareProperty("SyncMatchedMuonsBothWays", m_syncMuonsBothWays = false);
    // allow fast sync of myon masks
    // (Set to 'False' to force in-depth synchronization of muon masks.)
    declareProperty("AllowFastMuonMaskSync"   , m_allowFastMuonMaskSync = true);
    // keep tracks for closest tracks
    declareProperty("KeepCloseTracks"         , m_keepCloseTracks = false);
    // keep tracks for selected muons
    declareProperty("KeepTracksForMuons"      , m_keepSelMuonTracks = false);
    // keep tracks for selected calibrated muons
    declareProperty("KeepTracksForCalMuons"   , m_keepSelCalMuonTracks = false);
    // keep (original) muons for selected tracks
    declareProperty("KeepMuonsForTracks"      , m_keepSelTrackMuons = false);
    // keep calibrated muons for selected tracks
    declareProperty("KeepCalMuonsForTracks"   , m_keepSelTrackCalMuons = false);
    // apply AND for mask matching for vertices (default: false)
    declareProperty("ApplyAndForVertices"     , m_vertexAnd = false);
    // apply AND for mask matching for tracks (default: false)
    declareProperty("ApplyAndForTracks"       , m_trackAnd = false);
    // apply AND for mask matching for muons (default: false)
    declareProperty("ApplyAndForMuons"        , m_muonAnd = false);
    // thin primary vertex collection
    declareProperty("ThinPVs"                 , m_thinPVs = true);
    // thin refittd primary vertex collections
    declareProperty("ThinRefittedPVs"         , m_thinRefPVs = true);
    // thin ID track collection
    declareProperty("ThinTracks"              , m_thinTracks = true);
    // thin muon collections
    declareProperty("ThinMuons"               , m_thinMuons = true);
  }
  //--------------------------------------------------------------------------
  // Destructor
  //--------------------------------------------------------------------------
  BmumuThinningTool::~BmumuThinningTool() {
  }
  //--------------------------------------------------------------------------
  // initialization
  //--------------------------------------------------------------------------
  StatusCode BmumuThinningTool::initialize() {
    
    ATH_MSG_INFO("BmumuThinningTool::initialize()");
    
    // check TrackParticle container name
    if ( m_trkPartContName == "" ) {
      ATH_MSG_INFO("No ID track collection provided for thinning.");
    } else {
      ATH_MSG_INFO("Using " << m_trkPartContName
		   << " as the source collection for ID track particles.");
      m_doTracks = true;
    }
    
    // check secondary vertex container names
    if ( m_vtxContNames.empty() ) {
      ATH_MSG_FATAL("No secondary vertex collections provided for thinning.");
      return StatusCode::FAILURE;
    } else {
      for (std::vector<std::string>::iterator it = m_vtxContNames.begin();
	   it != m_vtxContNames.end(); ++it) {
	ATH_MSG_INFO("Using " << *it
		     << " as a source collection for secondary vertices.");
      }
    }
    
    // check vertex pass flags
    if ( m_alignPassToVertexList ) {
      if ( m_vtxPassFlags.size() != m_vtxContNames.size() ) {
	ATH_MSG_FATAL("Size mismatch of VertexContainerNames ("
		      << m_vtxContNames.size()
		      << ") and VertexPassFlags ("
		      << m_vtxPassFlags.size() << ")");
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO(std::left << std::setw(35) << "VertexContainerNames"
		     << " : " << "VertexPassFlags");
	ATH_MSG_INFO(std::setfill('-') << std::setw(70) << ""
		     << std::setfill(' '));
	for (size_t i=0; i<m_vtxContNames.size(); ++i) {
	  ATH_MSG_INFO(std::left << std::setw(35) << m_vtxContNames[i]
		       << " : " << m_vtxPassFlags[i]);
	}
      }
    } else {
      if ( m_vtxPassFlags.empty() ) {
	ATH_MSG_INFO("No VertexPassFlags: all secondary vertices will be "
		     << "accepted.");
      } else {
	std::string str;
	for (size_t i=0; i < m_vtxPassFlags.size(); ++i) {
	  if (i > 0) str += ", ";
	  str += m_vtxPassFlags[i];
	}
	ATH_MSG_INFO("VertexPassFlags applied to all vertices:");
	ATH_MSG_INFO(str);
      }
    }
    
    // check primary vertex container name
    if ( m_PVContName == "" ) {
      ATH_MSG_FATAL("No primary vertex collection provided for thinning.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Using " << m_PVContName
		   << " as the source collection for primary vertices.");
      m_doPVs = true;
    }
    
    // check refitted primary vertex container names
    if ( m_refPVContNames.empty() ) {
      ATH_MSG_INFO("No refitted PV collections provided for thinning.");
    } else {
      if ( m_refPVContNames.size() != m_vtxContNames.size() ) {
	ATH_MSG_FATAL("Size mismatch of VertexContainerNames ("
		      << m_vtxContNames.size()
		      << ") and RefPVContainerNames ("
		      << m_refPVContNames.size() << ")");
	return StatusCode::FAILURE;
      } else {
	for (std::vector<std::string>::iterator it = m_refPVContNames.begin();
	     it != m_refPVContNames.end(); ++it) {
	  ATH_MSG_INFO("Using " << *it
		       << " as a source collection for refitted PVs.");
	}
	m_doRefPVs = true;
      }
    }

    // check muon container name
    if ( m_muonContName == "" ) {
      ATH_MSG_INFO("No (orginal) muon collection provided for thinning.");
    } else {
      ATH_MSG_INFO("Using " << m_muonContName
		   << " as a source collection for (original) muons.");
      m_doMuons = true;
    }
    
    // check calibrated muons container name
    if ( m_calMuonContName == "" ) {
      ATH_MSG_INFO("No calibrated muons collection provided for thinning.");
    } else {
      ATH_MSG_INFO("Using " << m_calMuonContName
		   << " as a source collection for calibrated muons.");
      m_doCalMuons = true;
    }

    // check muon thinning settings
    if ( m_thinMuons ) {
      if ( (m_matchCalMuons || m_markCalMuons) && !m_doCalMuons ) {
	ATH_MSG_ERROR("No container for calibrated muons given!");
      }
      if ( (!m_matchCalMuons || m_markMuons) && !m_doMuons ) {
	ATH_MSG_ERROR("No container for (original) muons given!");
      }
      if ( m_matchCalMuons && m_markCalMuons ) {
	ATH_MSG_WARNING("Configuration issue: both MatchWithCalMuons and "
			<< "MarkMatchedCalMuons set to true! "
			<< "Ignoring the second setting.");
      }
      if ( !m_matchCalMuons && m_markMuons ) {
	ATH_MSG_WARNING("Configuration issue: MatchWithCalMuons set to "
			<< "false and "
			<< "MarkMatchedMuons set to true! "
			<< "Ignoring the second setting.");
      }
      ATH_MSG_INFO("MatchWithCalMuons: " << m_matchCalMuons
		   << ", MarkMatchedMuons: " << m_markMuons
		   << ", MarkMatchedCalMuons: " << m_markCalMuons);
    }
    
    // check closest track settings
    m_doCloseTracks = (m_ctBranchBaseName != "" && !m_ctBranchPrefixes.empty());
    if ( m_doCloseTracks ) {
      for (std::vector<std::string>::iterator it = m_ctBranchPrefixes.begin();
	   it != m_ctBranchPrefixes.end(); ++it) {

	ATH_MSG_INFO("Keeping tracks for "
		   << *it << "_" << m_ctBranchBaseName << "_*");
      }
    } else {
      ATH_MSG_INFO("Not keeping anything for closest tracks in thinning.");
    }

    // Output of options
    ATH_MSG_INFO("=== Option settings - begin ===");
    ATH_MSG_INFO("KeepTracksForSelectedPVs : " << m_keepPVTracks);
    ATH_MSG_INFO("MatchCalibratedMuons     : " << m_matchCalMuons);
    ATH_MSG_INFO("MarkMatchedMuons         : " << m_markMuons);
    ATH_MSG_INFO("MarkMatchedCalMuons      : " << m_markCalMuons);
    ATH_MSG_INFO("SyncMatchedMuonsBothWays : " << m_syncMuonsBothWays);
    ATH_MSG_INFO("AllowFastMuonMaskSync    : " << m_allowFastMuonMaskSync);
    ATH_MSG_INFO("KeepCloseTracks          : " << m_keepCloseTracks);
    ATH_MSG_INFO("KeepTracksForMuons       : " << m_keepSelMuonTracks);
    ATH_MSG_INFO("KeepTracksForCalMuons    : " << m_keepSelCalMuonTracks);
    ATH_MSG_INFO("KeepMuonsForTracks       : " << m_keepSelTrackMuons);
    ATH_MSG_INFO("KeepCalMuonsForTracks    : " << m_keepSelTrackCalMuons);
    ATH_MSG_INFO("ApplyAndForVertices      : " << m_vertexAnd);
    ATH_MSG_INFO("ApplyAndForTracks        : " << m_trackAnd);
    ATH_MSG_INFO("ApplyAndForMuons         : " << m_muonAnd);
    ATH_MSG_INFO("ThinPVs                  : " << m_thinPVs);
    ATH_MSG_INFO("ThinRefittedPVs          : " << m_thinRefPVs);
    ATH_MSG_INFO("ThinTracks               : " << m_thinTracks);
    ATH_MSG_INFO("ThinMuons                : " << m_thinMuons);
    ATH_MSG_INFO("=== Option settings - end ===");

    
    // initialize cache vector-of-vectors (one per vertex container)
    for (size_t ivcname=0; ivcname < m_vtxContNames.size(); ++ivcname) {
      m_vvOrigPVLinkNames.push_back(std::vector<std::string>());
      m_vvOrigPVLinkTypes.push_back(std::vector<pv_type>());
      m_vvRefPVLinkNames.push_back(std::vector<std::string>());
      m_vvRefPVLinkTypes.push_back(std::vector<pv_type>());
      m_vvCtLinkNames.push_back(std::vector<std::string>());
      m_vvCtLinkTypes.push_back(std::vector<pv_type>());
    } // for ivcname
    
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // finalization
  //--------------------------------------------------------------------------
  StatusCode BmumuThinningTool::finalize() {
    
    ATH_MSG_INFO("BmumuThinningTool::finalize()");

    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // apply thinning
  //--------------------------------------------------------------------------
  StatusCode BmumuThinningTool::doThinning() const {

    ATH_MSG_DEBUG("BmumuThinningTool::doThinning()");

    // retrieve TrackParticle container
    const xAOD::TrackParticleContainer* trkPartCont = nullptr;
    std::vector<bool> trkMask;
    if ( m_doTracks && m_thinTracks ) {
      CHECK( evtStore()->retrieve(trkPartCont, m_trkPartContName) ); 
      // default: keep no track
      trkMask.assign(trkPartCont->size(), false);
    }
    
    // retrieve PV container
    const xAOD::VertexContainer* pvCont = nullptr;
    std::vector<bool> pvMask;
    if ( m_doPVs && (m_thinPVs || m_keepPVTracks) ) {
      CHECK( evtStore()->retrieve(pvCont, m_PVContName) );
      // default: keep no PV
      pvMask.assign(pvCont->size(), false);
    }
    // retrieve refitted PV container
    std::vector<const xAOD::VertexContainer*> vRefPvCont =
      std::vector<const xAOD::VertexContainer*>(m_refPVContNames.size(),
						nullptr);
    std::vector<std::vector<bool> > vRefPvMasks;
    if ( m_thinRefPVs && m_doRefPVs ) {
      for (size_t irpv = 0; irpv < m_refPVContNames.size(); ++irpv) {
	CHECK( evtStore()->retrieve(vRefPvCont[irpv], m_refPVContNames[irpv]) );
	// default: keep no refitted PV
	vRefPvMasks.push_back(std::vector<bool>(vRefPvCont[irpv]->size(),
						false));
      }
    }

    // retrieve container of (original) muons
    const xAOD::MuonContainer* muonCont = nullptr;
    std::vector<bool> muonMask;
    if ( m_thinMuons && m_doMuons && (!m_matchCalMuons || m_markMuons) ) {
      CHECK( evtStore()->retrieve(muonCont, m_muonContName) );
      // default: keep no muon
      muonMask.assign(muonCont->size(), false);
    }
    
    // retrieve container of calibrated muons
    const xAOD::MuonContainer* calMuonCont = nullptr;
    std::vector<bool> calMuonMask;
    if ( m_thinMuons && m_doCalMuons && (m_matchCalMuons || m_markCalMuons) ) {
      CHECK( evtStore()->retrieve(calMuonCont, m_calMuonContName) );
      // default: keep no muon
      calMuonMask.assign(calMuonCont->size(), false);
    }
    
    // retrieve vertex containers
    for (size_t ivcname=0; ivcname < m_vtxContNames.size(); ++ivcname) {
      auto &vtxContName = m_vtxContNames[ivcname];
      ATH_MSG_DEBUG("doThinning(): vtxContName: " << vtxContName);
      const xAOD::VertexContainer*    vtxCont    = nullptr;
      const xAOD::VertexAuxContainer* vtxAuxCont = nullptr;
      CHECK( evtStore()->retrieve(vtxCont   , vtxContName) );
      CHECK( evtStore()->retrieve(vtxAuxCont, vtxContName+"Aux.") );
      size_t vtxContSize = vtxCont->size();
      std::vector<bool> vtxMask;
      // default: keep no vertex
      vtxMask.assign(vtxContSize, false);
      ATH_MSG_DEBUG("doThinning(): vtxContSize: " << vtxContSize);
      // loop over vertices
      for (size_t ivtx = 0; ivtx < vtxContSize; ++ivtx) {
	xAOD::BPhysHelper vtx(vtxCont->at(ivtx));
	bool vtxPassed = false;
	addToCounter(vtxContName+"_allVertices");
	ATH_MSG_DEBUG("doThinning(): ivtx = " << ivtx);
	if ( m_alignPassToVertexList ) {
	  ATH_MSG_DEBUG("doThinning(): 1: passFlag(" << ivcname << ") = "
			<< m_vtxPassFlags[ivcname]);
	  vtxPassed = pass(*vtx.vtx(), m_vtxPassFlags[ivcname]);
	} else {
	  for (size_t ipass = 0; ipass < m_vtxPassFlags.size(); ++ipass) {
	    ATH_MSG_DEBUG("doThinning(): 2: passFlag(" << ipass << ") = "
			  << m_vtxPassFlags[ipass]);
	    if ( pass(*vtx.vtx(), m_vtxPassFlags[ipass]) ) {
	      vtxPassed = true;
	      break;
	    }
	  } // for ipass
	}
	if ( vtxPassed ) {
	  //
	  // vertex passed selection
	  //
	  ATH_MSG_DEBUG("doThinning(): ivtx " << ivtx << " passed selection");
	  vtxMask[ivtx] = true;
	  addToCounter(vtxContName+"_passedVertices");

	  // keep tracks from secondary vertices
	  if ( m_doTracks && m_thinTracks ) {
	    for (size_t itrk=0; itrk < trkPartCont->size(); ++itrk) {
	      // only consider if not yet kept
	      if ( !trkMask[itrk] ) {
		for (size_t ivt=0; ivt < vtx.vtx()->nTrackParticles(); ++ivt) {
		  if ( vtx.vtx()->trackParticle(ivt)
		       == trkPartCont->at(itrk) ) {
		    trkMask[itrk] = true;
		    addToCounter(vtxContName+"_accTracks");
		  }
		} // for trk
	      }
	    } // for itrk
	  } // if m_doTracks
	  
	  // find aux variable names for closest tracks
	  if ( m_thinTracks && m_keepCloseTracks && m_doCloseTracks
	       && m_vvCtLinkNames[ivcname].empty() ) {
	    std::string prefix =
	      m_vtxContNames[ivcname]+"_"+m_ctBranchBaseName;
	    ATH_MSG_DEBUG("doThinning(): CT basename: " << prefix);
	    selectAuxElements(vtxAuxCont, m_vvCtLinkNames[ivcname],
			      m_ctBranchPrefixes,
			      m_vvCtLinkTypes[ivcname],
			      "_"+m_ctBranchBaseName+".*._Link");
	    if ( msgLvl(MSG::INFO) ) {
	      std::string sAuxVars =
		dumpVS(m_vvCtLinkNames[ivcname],
		       "doThinning(): "+vtxContName+": CT aux vars: ("
		       +std::to_string(m_vvCtLinkNames[ivcname].size())+")",
		       4);
	      logWrappedMsg(sAuxVars, MSG::INFO);
	    }
	  }

	  // keep tracks identified as closest tracks
	  if ( m_thinTracks && m_doTracks && m_keepCloseTracks
	       && m_doCloseTracks ) {
	    for (size_t i = 0; i < m_vvCtLinkNames[ivcname].size(); ++i) {
	      const xAOD::TrackParticle* closeTrack =
		getTrackParticle(vtx.vtx(), m_vvCtLinkNames[ivcname][i]);
	      if ( closeTrack == nullptr ) continue;
	      auto tpit = std::find(trkPartCont->begin(), trkPartCont->end(),
				    closeTrack);
	      if ( tpit == trkPartCont->end() ) {
		ATH_MSG_WARNING("ClosestTrack not found in "
				<< m_trkPartContName
				<< " for " << m_vvCtLinkNames[ivcname][i]);
		continue;
	      }
	      size_t x = std::distance(trkPartCont->begin(), tpit);
	      if ( !trkMask.at(x) ) {
		  trkMask.at(x) = true;
		  addToCounter(vtxContName+"_addTracksByCT");
	      }
	    } // for i (CT link names)
	  } // if
	    
	  // find aux variable names for original PVs
	  if ( m_thinPVs && m_vvOrigPVLinkNames[ivcname].empty() ) {	    
	    selectAuxElements(vtxAuxCont, m_vvOrigPVLinkNames[ivcname],
			      m_vvOrigPVLinkTypes[ivcname], "OrigPv.*.Link");
	    if ( msgLvl(MSG::INFO) ) {
	      std::string sAuxVars =
		dumpVS(m_vvOrigPVLinkNames[ivcname],
		       "doThinning(): "+vtxContName+": OrigPV aux vars: ("
		       +std::to_string(m_vvOrigPVLinkNames[ivcname].size())+")",
		       4);
	      logWrappedMsg(sAuxVars, MSG::INFO);
	    }
	  }
	  // find aux variable names for refitted PVs
	  if ( m_thinRefPVs && m_vvRefPVLinkNames[ivcname].empty() ) {
	    selectAuxElements(vtxAuxCont, m_vvRefPVLinkNames[ivcname],
			      m_vvRefPVLinkTypes[ivcname], "Pv.*.Link");
	    if ( msgLvl(MSG::INFO) ) {
	      std::string sAuxVars =
		dumpVS(m_vvRefPVLinkNames[ivcname],
		       "doThinning() :"+vtxContName+": RefPV aux vars: ("
		       +std::to_string(m_vvRefPVLinkNames[ivcname].size())+")",
		       4);
	      logWrappedMsg(sAuxVars, MSG::INFO);
	    }
	  }

	  // debug stuff
	  if ( msgLvl(MSG::VERBOSE) ) {
	    std::vector<std::string> auxVars =
	      filterAuxElements(vtxAuxCont, ".*Link");
	    std::string sAuxVars =
	      dumpVS(auxVars,
		     "doThinning(): "+vtxContName+": vtxAuxContVarNames:", 4);
	    logWrappedMsg(sAuxVars, MSG::DEBUG);
	  }

	  // now mark associated PVs
	  if ( m_thinPVs && m_doPVs ) {
	    for (size_t i = 0; i < m_vvOrigPVLinkTypes[ivcname].size(); ++i) {
	      const xAOD::Vertex* origPv =
		vtx.origPv(m_vvOrigPVLinkTypes[ivcname][i]);
	      if ( origPv == nullptr ) continue;
	      auto pvit = std::find(pvCont->begin(), pvCont->end(), origPv);
	      if ( pvit == pvCont->end() ) {
		ATH_MSG_WARNING("PV not found in " << m_PVContName
				<< " for " << m_vvOrigPVLinkNames[ivcname][i]);
		continue;
	      }
	      size_t x = std::distance(pvCont->begin(), pvit);
	      if ( !pvMask.at(x) ) {
		pvMask.at(x) = true;
		addToCounter(vtxContName+"_accAssocPVs");
	      }
	      // keep tracks for selected PVs
	      if ( m_thinTracks && m_doTracks && m_keepPVTracks ) {
		for (size_t ipvt=0; ipvt < (*pvit)->nTrackParticles();
		     ++ipvt) {
		  const xAOD::TrackParticle* tp = (*pvit)->trackParticle(ipvt);
		  if ( tp == nullptr ) continue;
		  auto tpit = std::find(trkPartCont->begin(),
					trkPartCont->end(), tp);
		  if ( tpit == trkPartCont->end() ) {
		    ATH_MSG_WARNING("PV track not found in "
				    << m_trkPartContName << " for PV from "
				    << m_PVContName);
		    continue;
		  }
		  size_t x = std::distance(trkPartCont->begin(), tpit);
		  if ( !trkMask.at(x) ) {
		    trkMask.at(x) = true;
		    addToCounter(vtxContName+"_addTracksBySelPVs");
		  }
		} // for ipvt
	      } // if m_keepPVTracks
	    } // for i (PVs)
	  } // if m_thinPVs && m_doPVs
	  
	  // now mark associated refittedPVs
	  if ( m_thinRefPVs && m_doRefPVs ) {
	    for (size_t i = 0; i < m_vvRefPVLinkTypes[ivcname].size(); ++i) {
	      const xAOD::Vertex* refPv =
		vtx.pv(m_vvRefPVLinkTypes[ivcname][i]);
	      if ( refPv == nullptr ) continue;
	      auto pvit = std::find(vRefPvCont[ivcname]->begin(),
				    vRefPvCont[ivcname]->end(), refPv);
	      if ( pvit == vRefPvCont[ivcname]->end() ) {
		ATH_MSG_WARNING("PV not found in " << m_refPVContNames[ivcname]
				<< " for " << m_vvRefPVLinkNames[ivcname][i]);
		continue;
	      }
	      size_t x = std::distance(vRefPvCont[ivcname]->begin(), pvit);
	      if ( !vRefPvMasks[ivcname].at(x) ) {
		vRefPvMasks[ivcname].at(x) = true;
		addToCounter(vtxContName+"_accAssocRefPVs");
	      }
	      // keep tracks for associated refitted PVs
	      if ( m_thinTracks && m_doTracks && m_keepPVTracks ) {
		for (size_t ipvt=0; ipvt < (*pvit)->nTrackParticles();
		     ++ipvt) {
		  const xAOD::TrackParticle* tp = (*pvit)->trackParticle(ipvt);
		  if ( tp == nullptr ) continue;
		  auto tpit = std::find(trkPartCont->begin(),
					trkPartCont->end(), tp);
		  if ( tpit == trkPartCont->end() ) {
		    ATH_MSG_WARNING("Refitted PV track not found in "
				    << m_trkPartContName
				    << " for refitted PV from "
				    << m_refPVContNames[ivcname][i]);
		    continue;
		  }
		  size_t x = std::distance(trkPartCont->begin(), tpit);
		  if ( !trkMask.at(x) ) {
		    trkMask.at(x) = true;
		    addToCounter(vtxContName+"_addTracksByAssocRefPVs");
		  }
		} // for ipvt
	      } // if m_thinTracks && m_doTracks && m_keepPVTracks
	    } // for i (refPVs)
	  } // if m_thinRefPVs && m_doRefPVs
	  
	  // keep muons from secondary vertices
	  if ( m_thinMuons ) {
	    if ( m_matchCalMuons ) { // calibrated muons collection
	      if ( m_doCalMuons ) {
		addToCounter(vtxContName+"_accCalMuons_calls");
		CHECK( matchMuons(calMuonCont, calMuonMask, vtx,
			   vtxContName+"_accCalMuons") );
	      }
	    } else { // original muons collection
	      if ( m_doMuons ) {
		addToCounter(vtxContName+"_accMuons_calls");
		CHECK( matchMuons(muonCont, muonMask, vtx,
				  vtxContName+"_accMuons") );
	      }
	    } // if m_matchCalMuons
	  } // if m_thinMuons
	} else {
	  ATH_MSG_DEBUG("doThinning(): ivtx " << ivtx
			<< " did not pass selection");
	}
      } // for ivtx (B vertex)

      // Apply the thinning service based on vtxMask
      CHECK( applyThinMask(vtxCont, vtxMask, m_vertexAnd, vtxContName) );
			         
    } // for m_vtxContNames

    // Keep tracks for all PVs (i.e. if all PVs are kept)
    if ( m_thinTracks && m_doTracks && m_keepPVTracks && !m_thinPVs
	 && m_doPVs ) {
      for (auto &pv : *pvCont) {
	for (size_t ipvt=0; ipvt < pv->nTrackParticles(); ++ipvt) {
	  const xAOD::TrackParticle* tp = pv->trackParticle(ipvt);
	  if ( tp == nullptr ) continue;
	  auto tpit = std::find(trkPartCont->begin(), trkPartCont->end(), tp);
	  if ( tpit == trkPartCont->end() ) {
	    ATH_MSG_WARNING("PV track not found in "
			    << m_trkPartContName << " for PV from "
			    << m_PVContName);
	    continue;
	  }
	  size_t x = std::distance(trkPartCont->begin(), tpit);
	  if ( !trkMask.at(x) ) {
	    trkMask.at(x) = true;
	    addToCounter("addTracksByAllPVs");
	  }
	} // for ipvt
      } // for pv
    } // if m_keepPVTracks && m_doTracks && m_keepPVTracks && !m_thinPVs

    // Keep (original) muons for selected ID tracks
    if (m_keepSelTrackMuons && m_thinMuons && m_doMuons && m_doTracks) {
      CHECK( markMuonsForSelTracks(trkPartCont, trkMask, muonCont, muonMask,
				   "addMuonsBySelTracks") );
    }
    
    // Keep (original) muons for selected ID tracks
    if (m_keepSelTrackCalMuons && m_thinMuons && m_doCalMuons && m_doTracks) {
      CHECK( markMuonsForSelTracks(trkPartCont, trkMask, calMuonCont,
				   calMuonMask, "addCalMuonsBySelTracks") );
    }
    
    // mark 'other' muon container elements if requested
    if ( m_thinMuons ) {
      if ( m_syncMuonsBothWays || m_matchCalMuons ) {
	// calibrated muons -> original muons
	if ( (m_markMuons || m_syncMuonsBothWays)
	     && m_doMuons && m_doCalMuons) {
	  CHECK( markOrigMuons(muonCont, calMuonCont, muonMask, calMuonMask,
			       "addMarkedMuons", m_allowFastMuonMaskSync) );
	}
      } // if m_syncMuonsBothWays || m_matchCalMuons ) {
      if ( m_syncMuonsBothWays || !m_matchCalMuons ) {
	// 'orignal' muons -> calibrated muons
	if ( (m_markCalMuons || m_syncMuonsBothWays)
	     && m_doCalMuons && m_doMuons ) {
	  CHECK( markCalibMuons(muonCont, calMuonCont, muonMask, calMuonMask,
				"addMarkedCalMuons", m_allowFastMuonMaskSync) );
	}
      } // if m_syncMuonsBothWays || !m_matchCalMuons ) {
    } // if m_thinMuons

    // Keep tracks for selected (original) muons
    if ( m_thinTracks && m_doTracks && m_keepSelMuonTracks && m_doMuons ) {
      CHECK( markTrksForSelMuons(trkPartCont, trkMask, muonCont, muonMask,
				 "addTracksBySelMuons") );
    } // if m_thinTracks && m_doTracks && m_keepSelMuonTracks && m_doMuons
    
    // Keep tracks for selected (calibrated) muons
    if ( m_thinTracks && m_doTracks && m_keepSelCalMuonTracks
	 && m_doCalMuons ) {
      CHECK( markTrksForSelMuons(trkPartCont, trkMask, calMuonCont,
				 calMuonMask, "addTracksBySelCalMuons") );
    } // if m_thinTracks && m_doTracks && m_keepSelCalMuonTracks && m_doCalMuons
    
    // debug: check muon masks' consistency
    if ( msgLvl(MSG::DEBUG) ) {
      std::string msg =
	checkMaskConsistency(muonMask, calMuonMask,
			     m_muonContName+"Mask",
			     m_calMuonContName+"Mask",
			     "Muon mask consistency check:");
      logWrappedMsg(msg, MSG::DEBUG);
    }

    // Apply the thinning service for TrackParticles based on trkMask
    if ( m_thinTracks && m_doTracks ) {
      addToCounter(m_trkPartContName+"_allTracks", trkPartCont->size());
      addToCounter(m_trkPartContName+"_passedTracks",
		   std::accumulate(trkMask.begin(), trkMask.end(), 0));      
      CHECK( applyThinMask(trkPartCont, trkMask, m_trackAnd,
			   m_trkPartContName) );
    } // if m_doTracks

    // Apply the thinning service for PVs based on pvMask
    if ( m_thinPVs && m_doPVs ) {
      addToCounter(m_PVContName+"_allVertices", pvCont->size());
      addToCounter(m_PVContName+"_passedVertices",
		   std::accumulate(pvMask.begin(), pvMask.end(), 0));      
      CHECK( applyThinMask(pvCont, pvMask, m_vertexAnd, m_PVContName) );
    } // if m_doPVs

    // Apply the thinning service for refPVs based on vRefPvMasks
    if ( m_thinRefPVs && m_doRefPVs ) {
      for (size_t irpv = 0; irpv < m_refPVContNames.size(); ++irpv) {
	addToCounter(m_refPVContNames[irpv]+"_allVertices",
		     vRefPvCont[irpv]->size());
	addToCounter(m_refPVContNames[irpv]+"_passedVertices",
		     std::accumulate(vRefPvMasks[irpv].begin(),
				     vRefPvMasks[irpv].end(), 0));
	CHECK( applyThinMask(vRefPvCont[irpv], vRefPvMasks[irpv],
			     m_vertexAnd, m_refPVContNames[irpv]) );
      } // for irpv
    } // if m_doRefPVs
    
    // Apply the thinning service for (original) Muons based on muonMask
    if ( m_thinMuons && m_doMuons ) {
      addToCounter(m_muonContName+"_allMuons", muonCont->size());
      addToCounter(m_muonContName+"_passedMuons",
		   std::accumulate(muonMask.begin(), muonMask.end(), 0));      
      CHECK( applyThinMask(muonCont, muonMask, m_muonAnd,
			   m_muonContName) );
    } // if m_thinMuons && m_doMuons

    // Apply the thinning service for calibrated Muons based on calMuonMask
    if ( m_thinMuons && m_doCalMuons ) {
      addToCounter(m_calMuonContName+"_allMuons", calMuonCont->size());
      addToCounter(m_calMuonContName+"_passedMuons",
		   std::accumulate(calMuonMask.begin(), calMuonMask.end(),
				   0));      
      CHECK( applyThinMask(calMuonCont, calMuonMask, m_muonAnd,
			   m_calMuonContName) );
    } // if m_thinMuons && m_doCalMuons

    return StatusCode::SUCCESS;    
  }
  //--------------------------------------------------------------------------
  // Helper to apply thinning service mask -- for TrackParticles
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::applyThinMask(const xAOD::TrackParticleContainer* trkCont,
				   const std::vector<bool>& trkMask,
				   bool doAnd, std::string name) const {
    
    if (doAnd) {
      ATH_MSG_DEBUG("doThinning(): apply thinning (AND) for " << name);
      if ( m_thinningSvc->filter(*trkCont, trkMask,
				 IThinningSvc::Operator::And).isFailure() ) {
	ATH_MSG_ERROR("Application of thinning service for "<< name
		      << " failed!");
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("doThinning(): apply thinning (OR) for " << name);
      if ( m_thinningSvc->filter(*trkCont, trkMask,
				 IThinningSvc::Operator::Or).isFailure() ) {
	ATH_MSG_ERROR("Application of thinning service for "<< name
		      << " failed!");
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }					      
  //--------------------------------------------------------------------------
  // Helper to apply thinning service mask -- for vertices
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::applyThinMask(const xAOD::VertexContainer* vtxCont,
				   const std::vector<bool>& vtxMask,
				   bool doAnd, std::string name) const {
    
    if (doAnd) {
      ATH_MSG_DEBUG("doThinning(): apply thinning (AND) for " << name);
      if ( m_thinningSvc->filter(*vtxCont, vtxMask,
				 IThinningSvc::Operator::And).isFailure() ) {
	ATH_MSG_ERROR("Application of thinning service for "<< name
		      << " failed!");
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("doThinning(): apply thinning (OR) for " << name);
      if ( m_thinningSvc->filter(*vtxCont, vtxMask,
				 IThinningSvc::Operator::Or).isFailure() ) {
	ATH_MSG_ERROR("Application of thinning service for "<< name
		      << " failed!");
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }					      
  //--------------------------------------------------------------------------
  // Helper to apply thinning service mask -- for Muons
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::applyThinMask(const xAOD::MuonContainer* muCont,
				   const std::vector<bool>& muMask,
				   bool doAnd, std::string name) const {
    
    if (doAnd) {
      ATH_MSG_DEBUG("doThinning(): apply thinning (AND) for " << name);
      if ( m_thinningSvc->filter(*muCont, muMask,
				 IThinningSvc::Operator::And).isFailure() ) {
	ATH_MSG_ERROR("Application of thinning service for "<< name
		      << " failed!");
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_DEBUG("doThinning(): apply thinning (OR) for " << name);
      if ( m_thinningSvc->filter(*muCont, muMask,
				 IThinningSvc::Operator::Or).isFailure() ) {
	ATH_MSG_ERROR("Application of thinning service for "<< name
		      << " failed!");
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }					      
  //--------------------------------------------------------------------------
  // Helper to check whether an element is marked as passing a specific
  // hypothesis.
  //--------------------------------------------------------------------------
  bool BmumuThinningTool::pass(const SG::AuxElement& em, std::string hypo)
    const {
    
    if ( !boost::algorithm::starts_with(hypo, "passed_") )
      hypo = "passed_" + hypo;
    SG::AuxElement::Accessor<Char_t> flagAcc(hypo);
    return flagAcc.isAvailable(em) && flagAcc(em) != 0;
  }
  //--------------------------------------------------------------------------
  // Helper to get a TrackParticle link
  //-------------------- ------------------------------------------------------
  const xAOD::TrackParticle*
  BmumuThinningTool::getTrackParticle(const xAOD::Vertex* vtx,
				      std::string name) const {
    SG::AuxElement::Accessor<TrackParticleLink> tpLinkAcc(name);
    if (!tpLinkAcc.isAvailable(*vtx)) {
      return nullptr;
    }
    const TrackParticleLink& tpLink = tpLinkAcc(*vtx);
    if (!tpLink.isValid()) {
      return nullptr;
    }
    return *tpLink;
  }
  //--------------------------------------------------------------------------
  // Helper to filter all names of auxillary elements of an aux container
  // according to a certain pattern.  The pattern must be a regular
  // expression pattern.
  //--------------------------------------------------------------------------
  std::vector<std::string>
  BmumuThinningTool::filterAuxElements(const xAOD::AuxContainerBase* auxCont,
				       std::string pattern) const {
    
    SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

    std::vector<std::string> vElNames;
    std::regex re(pattern);

    const SG::auxid_set_t& auxids = auxCont->getAuxIDs();
    for ( SG::auxid_t auxid : auxids ) {
      const std::string name = reg.getName(auxid);
      if ( std::regex_match(name, re) ) {
	vElNames.push_back(name);
      }
    } // for auxids

    return vElNames;
  }
  //--------------------------------------------------------------------------
  // Determine aux elements to be looked at -- for (refitted) PVs
  //--------------------------------------------------------------------------
  void
  BmumuThinningTool::selectAuxElements(const xAOD::AuxContainerBase* auxCont,
				       std::vector<std::string>& vLinkNames,
				       std::vector<pv_type>&     vLinkTypes,
				       std::string pattern) const {
    // find aux element names matching pattern
    std::vector<std::string> vAuxNames =
      filterAuxElements(auxCont, pattern);

    // select aux element names matching our PV-to-SV association types
    for (auto &name : vAuxNames) {
      for (size_t ipvt=0; ipvt < xAOD::BPhysHelper::n_pv_types; ++ipvt) {
	if ( name.find(PvTypeToVarNameMap[(pv_type)ipvt]
		       +"Link") != std::string::npos) {
	  vLinkNames.push_back(name);
	  vLinkTypes.push_back((pv_type)ipvt);
	}
      } // for ipvt
    } // for name
  }  
  //--------------------------------------------------------------------------
  // Determine aux elements to be looked at -- for closest tracks
  //--------------------------------------------------------------------------
  void
  BmumuThinningTool::selectAuxElements(const xAOD::AuxContainerBase* auxCont,
				       std::vector<std::string>& vLinkNames,
				       std::vector<std::string>  vPrefixes,
				       std::vector<pv_type>&     vLinkTypes,
				       std::string pattern) const {
    // find aux element names matching pattern
    std::vector<std::string> vAuxNames;
    for (auto &prefix : vPrefixes) {
      std::string cpat = prefix+pattern;
      std::vector<std::string> vMoreAuxNames =
	filterAuxElements(auxCont, cpat);
      vAuxNames.insert(vAuxNames.end(), vMoreAuxNames.begin(),
		       vMoreAuxNames.end());
    } // for prefix

    // select aux element names matching our PV-to-SV association types
    for (auto &name : vAuxNames) {
      for (size_t ipvt=0; ipvt < xAOD::BPhysHelper::n_pv_types; ++ipvt) {
	if ( name.find(xAOD::BPhysHelper::pv_type_str[ipvt]
		       +"_Link") != std::string::npos) {
	  vLinkNames.push_back(name);
	  vLinkTypes.push_back((pv_type)ipvt);
	}
      } // for ipvt
    } // for name
  }  
  //--------------------------------------------------------------------------
  // Mark muons matched to secondary vertices
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::matchMuons(const xAOD::MuonContainer* muCont,
				std::vector<bool>& muMask,
				xAOD::BPhysHelper& vtx,
				std::string counterName) const {

    for (size_t imu=0; imu < muCont->size(); ++imu) {
      // only consider if not yet kept
      if ( !muMask[imu] ) {
	for (int ivm=0; ivm < vtx.nMuons(); ++ivm) {
	  if ( vtx.muon(ivm) == muCont->at(imu) ) {
	    muMask[imu] = true;
	    addToCounter(counterName);
	  }
	} // for ivm
      }
    } // for imu
    return StatusCode::SUCCESS;
  }					      
  //--------------------------------------------------------------------------
  // Mark original muons for accepted calibrated muons
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::markOrigMuons(const xAOD::MuonContainer* muCont,
				   const xAOD::MuonContainer* cmuCont,
				   std::vector<bool>& muMask,
				   std::vector<bool>& cmuMask,
				   std::string counterName,
				   bool allowFastSync) const {

    bool fastSync = allowFastSync;
    // go to slow sync if muon mask sizes do not match
    if ( muMask.size() != cmuMask.size() ) {
      fastSync = false;
      addToCounter(counterName+"_maskSizeMismatches");
    }
    
    for (size_t imu=0; imu < cmuMask.size(); ++imu) {
      if ( cmuMask[imu] ) {
	if ( fastSync ) {
	  if ( !muMask[imu] ) {
	    muMask[imu] = true;
	    addToCounter(counterName);
	  }	  
	} else {
	  const xAOD::Muon* cMuon = cmuCont->at(imu);
	  if ( cMuon != nullptr ) {
	    const xAOD::Muon* oMuon =
	      (const xAOD::Muon*)xAOD::getOriginalObject(*cMuon);
	    if ( oMuon != nullptr ) {
	      auto muit = std::find(muCont->begin(), muCont->end(), oMuon);
	      if ( muit == muCont->end() ) {
		ATH_MSG_WARNING("Muon not found in " << m_muonContName
				<< " for calibrated muon index " << imu);
	      } else {
		size_t x = std::distance(muCont->begin(), muit);
		if ( !muMask.at(x) ) {
		  muMask.at(x) = true;
		  addToCounter(counterName);
		}
	      }
	    } else {
	      ATH_MSG_WARNING("No orignal muon for calibrated muon index "
			      << imu);
	    }
	  } else {
	    ATH_MSG_WARNING("No calibrated muon for index " << imu);
	  }
	} // if fastSync
      }
    } // for imu
    return StatusCode::SUCCESS;
  }    
  //--------------------------------------------------------------------------
  // Mark calibrated muons for accepted (original) muons
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::markCalibMuons(const xAOD::MuonContainer* muCont,
				    const xAOD::MuonContainer* cmuCont,
				    std::vector<bool>& muMask,
				    std::vector<bool>& cmuMask,
				    std::string counterName,
				    bool allowFastSync) const {
    
    bool fastSync = allowFastSync;
    // go to slow sync if muon mask sizes do not match
    if ( muMask.size() != cmuMask.size() ) {
      fastSync = false;
      addToCounter(counterName+"_maskSizeMismatches");
    }
    
    for (size_t imu=0; imu < muMask.size(); ++imu) {
      if ( muMask[imu] ) {
	if ( fastSync ) {
	  if ( !cmuMask[imu] ) {
	    cmuMask[imu] = true;
	    addToCounter(counterName);
	  }	  
	} else {
	  const xAOD::Muon* oMuon = muCont->at(imu);
	  if ( oMuon != nullptr ) {
	    bool found = false;
	    for (size_t icmu = 0; icmu < cmuCont->size(); ++icmu) {
	      const xAOD::Muon* cMuon = cmuCont->at(icmu);
	      if ( cMuon != nullptr ) {
		const xAOD::Muon* aMuon =
		  (const xAOD::Muon*)xAOD::getOriginalObject(*cMuon);
		if ( aMuon == oMuon ) {
		  found = true;
		  if ( !cmuMask.at(icmu) ) {
		    cmuMask.at(icmu) = true;
		    addToCounter(counterName);
		  }
		}
	      } else {
		ATH_MSG_WARNING("No calibrated muon for calibrated "
			      << "muon index " << icmu);
	      }
	    } // for icmu
	    if ( !found ) {
	      ATH_MSG_WARNING("No calibrated muon found for orignal "
			      << "muon index " << imu);
	    }
	  } else {
	    ATH_MSG_WARNING("No (original) muon for index " << imu);
	  }
	} // if fastSync
      }
    } // for imu
    return StatusCode::SUCCESS;
  }    
  //--------------------------------------------------------------------------
  // Mark ID tracks of selected (original or calibrated) muons
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::markTrksForSelMuons(const xAOD::TrackParticleContainer*
					 trkPartCont,
					 std::vector<bool>& trkMask,
					 const xAOD::MuonContainer* muCont,
					 std::vector<bool>& muMask,
					 std::string counterName) const {
    
    for (size_t itrk=0; itrk < trkPartCont->size(); ++itrk) {
      if ( trkMask[itrk] ) continue;
      const xAOD::TrackParticle* tp = trkPartCont->at(itrk);
      if ( tp == nullptr ) continue;
      for (size_t imu=0; imu < muCont->size(); ++imu) {
	if ( muMask[imu] ) {
	  const xAOD::Muon* muon = muCont->at(imu);
	  if ( muon != nullptr ) {
	    const xAOD::TrackParticle* mutp =
	      muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	    if ( mutp == tp ) {
	      trkMask[itrk] = true;
	      addToCounter(counterName);
	    }
	  }
	}
      } // for imu
    } // for itrk
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  // Mark muons for selected ID tracks
  //--------------------------------------------------------------------------
  StatusCode
  BmumuThinningTool::markMuonsForSelTracks(const xAOD::TrackParticleContainer*
					   trkPartCont,
					   std::vector<bool>& trkMask,
					   const xAOD::MuonContainer* muCont,
					   std::vector<bool>& muMask,
					   std::string counterName) const {

    for (size_t imu=0; imu < muCont->size(); ++imu) {
      if ( muMask[imu] ) continue;
      const xAOD::Muon* muon = muCont->at(imu);
      if ( muon == nullptr ) continue;
      const xAOD::TrackParticle* mutp =
	muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      if ( mutp == nullptr) continue;
      auto tpit = std::find(trkPartCont->begin(), trkPartCont->end(), mutp);
      if ( tpit == trkPartCont->end() ) {
	ATH_MSG_WARNING("Muon track not found in " << m_trkPartContName
			<< " for counter " << counterName);
	addToCounter(counterName+"_missingTracksForMuons");
	continue;
      }
      size_t x = std::distance(trkPartCont->begin(), tpit);
      if ( trkMask.at(x) ) {
	muMask[imu] = true;
	addToCounter(counterName);
      }
    } // for imu
    return StatusCode::SUCCESS;
  }
  
  //--------------------------------------------------------------------------
  // Check two masks for consistency
  //--------------------------------------------------------------------------
  std::string
  BmumuThinningTool::checkMaskConsistency(const std::vector<bool>& mask1,
					  const std::vector<bool>& mask2,
					  const std::string name1,
					  const std::string name2,
					  const std::string header) const {

    bool sizesMatch = (mask1.size() == mask2.size());
    int  nTrueMask1 = std::accumulate(mask1.begin(), mask1.end(), 0);      
    int  nTrueMask2 = std::accumulate(mask2.begin(), mask2.end(), 0);      

    std::string basecname = name1+"_"+name2;
    
    int nEntryMismatches(0);
    int nMoreTrueMask1(0);
    int nMoreTrueMask2(0);
    for (size_t i=0; i < std::min(mask1.size(), mask2.size()); ++i) {
      if ( mask1[i] != mask2[i]  ) nEntryMismatches++;
      if ( mask1[i] && !mask2[i] ) nMoreTrueMask1++; 
      if ( !mask1[i] && mask2[i] ) nMoreTrueMask2++; 
    }
    
    std::string str(header);
    if ( str.length() > 0 ) str += "\n";
    if ( sizesMatch && nTrueMask1 == nTrueMask2 && nEntryMismatches == 0 ) {
      str += "Masks match OK: "+name1+" ("+mask1.size()+") : "
	+ name2+" ("+mask2.size()+")";
      addToCounter(basecname+"_matchedOK");
    } else {
      str += "Masks do NOT match: "+name1+" ("+mask1.size()+") : "
	+ name2+" ("+mask2.size()+")";
      str += "\nnTrueMask1: "+std::to_string(nTrueMask1)
	+", nTrueMask2: "+std::to_string(nTrueMask2);
      str += "\nnEntryMismatches: "+std::to_string(nEntryMismatches)
	+"nMoreTrueMas1: "+std::to_string(nMoreTrueMask1)
	+", nMoreTrueMask2: "+std::to_string(nMoreTrueMask2); 
      addToCounter(basecname+"_NOTmatched");
      if (!sizesMatch) addToCounter(basecname+"_sizeMismatch");
      addToCounter(basecname+"_nEntryMismatches", nEntryMismatches);
      addToCounter(basecname+"_nMoreTrueMask1", nMoreTrueMask1);
      addToCounter(basecname+"_nMoreTrueMask2", nMoreTrueMask2);
    }
    return str;
  }
  //--------------------------------------------------------------------------
  // Dump a vector<string> to string.
  //--------------------------------------------------------------------------
  std::string BmumuThinningTool::dumpVS(const std::vector<std::string>& vs,
					const std::string header,
					size_t nBlanks) const {
    std::string str(header);
    for (const std::string& s : vs) {
      if ( str.length() > 0 ) str += "\n"; 
      str += std::string(nBlanks, ' ') + s;
    } // for s
    
    return str;
  }					
  //--------------------------------------------------------------------------
  // Wrap string at line breaks and print with appropriate message level
  //--------------------------------------------------------------------------
  void BmumuThinningTool::logWrappedMsg(const std::string& str,
					const MSG::Level lvl) const {
    std::istringstream isstr(str);
    std::string s;
    while (std::getline(isstr, s)) {
      msg(lvl) << s << endreq;
    }    
  }
  //--------------------------------------------------------------------------

} // namespace DerivationFramework
