/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

     
///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ITKNew
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITKNew.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsComparison.h"

#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ITKNew::SiSpacePointsSeedMaker_ITKNew
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_spacepointsSCT("SCT_SpacePoints"),
    m_spacepointsPixel("PixelSpacePoints"),
    m_spacepointsOverlap("OverlapSpacePoints"),
    m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels"),  
    m_fieldServiceHandle("AtlasFieldSvc",n),
    m_thistSvc(nullptr),
    m_outputTree(nullptr),
    m_treeName(""),
    m_treeFolder("/valNtuples/")
{

  declareInterface<ISiSpacePointsSeedMaker>(this);

  declareProperty("writeNtuple"           , m_writeNtuple  = false );
  declareProperty("UseAssociationTool"    , m_useassoTool  = false ); 
  declareProperty("usePixel"              , m_pixel        = true  );
  declareProperty("useSCT"                , m_sct          = true  );
  declareProperty("useOverlapSpCollection", m_useOverlap   = true  );
  declareProperty("checkEta"              , m_checketa     = false );
  declareProperty("useFastTracking"       , m_fastTracking = false );
  declareProperty("isLRT"                 , m_isLRT        = false );
  declareProperty("useSeedConfirmation"   , m_useSeedConfirmation = false );
  declareProperty("alwaysKeepConfirmedPixelSeeds", m_alwaysKeepConfirmedPixelSeeds = false);
  declareProperty("alwaysKeepConfirmedStripSeeds", m_alwaysKeepConfirmedStripSeeds = false);
  declareProperty("optimisePhiBinning"    , m_optimisePhiBinning = true);
  declareProperty("etaMin"                , m_etamin       = 0.    );
  declareProperty("etaMax"                , m_etamax       = 2.7   ); 
  declareProperty("pTmin"                 , m_ptmin        = 500.  );
  declareProperty("radMax"                , m_r_rmax       = 1100. );
  declareProperty("radMin"                , m_r_rmin       = 0.    );
  declareProperty("radStep"               , m_binSizeR     = 2.    );
  declareProperty("maxSize"               , m_maxsize      = 10000 );
  declareProperty("maxSizeSP"             , m_maxsizeSP    = 4096  );
  declareProperty("minZ"                  , m_zmin         = -250. );
  declareProperty("maxZ"                  , m_zmax         = +250. );
  declareProperty("mindRadiusPPP"         , m_drminPPP     = 6.    );
  declareProperty("maxdRadiusPPP"         , m_drmaxPPP     = 120.  );
  declareProperty("mindRadiusSSS"         , m_drminSSS     = 20.   );
  declareProperty("maxdRadiusSSS"         , m_drmaxSSS     = 300.  );
  declareProperty("maxdZver"              , m_dzver        = 5.    );
  declareProperty("maxdZdRver"            , m_dzdrver      = .02   );
  declareProperty("maxdImpact"            , m_maxdImpact   = 10.   );
  declareProperty("maxdImpactSSS"         , m_maxdImpactSSS = 20.  );
  declareProperty("radMinSSS"             , m_rminSSS      = 400.  );
  declareProperty("radMaxSSS"             , m_rmaxSSS      = 1000. );
  declareProperty("dImpactCutSlopeUnconfirmedSSS", m_dImpactCutSlopeUnconfirmedSSS = 1.0);
  declareProperty("dImpactCutSlopeUnconfirmedPPP", m_dImpactCutSlopeUnconfirmedPPP = 0.);
  declareProperty("maxSeedsForSpacePoint",       m_maxOneSize = -1);
  declareProperty("maxSeedsForSpacePointStrips", m_maxOneSizeSSS = 5);
  declareProperty("maxSeedsForSpacePointPixels", m_maxOneSizePPP = 5);
  declareProperty("seedScoreBonusPPP"     , m_seedScoreBonusPPP = -200.);
  declareProperty("seedScoreBonusSSS"     , m_seedScoreBonusSSS = -400.);
  declareProperty("seedScoreBonusConfirmationSeed", m_seedScoreBonusConfirmationSeed = -200.);
  declareProperty("maximumAcceptedSeedScore", m_maxScore    = 100. );
  declareProperty("SpacePointsSCTName"    , m_spacepointsSCT       );
  declareProperty("SpacePointsPixelName"  , m_spacepointsPixel     );
  declareProperty("SpacePointsOverlapName", m_spacepointsOverlap   );
  declareProperty("BeamConditionsService" , m_beamconditions = "BeamCondSvc");  
  declareProperty("MagFieldSvc"           , m_fieldServiceHandle   );

  if(m_maxOneSize>0){
    m_maxOneSizeSSS = m_maxOneSize;
    m_maxOneSizePPP = m_maxOneSize;
  }

}



///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITKNew::initialize()
{

  StatusCode sc = AlgTool::initialize(); 

  // Get beam geometry
  //
  m_beamCondService = 0;
  if(!m_beamconditions.empty()) {
    sc = service(m_beamconditions,m_beamCondService);
  }

  // Get magnetic field service
  //
  if( !m_fieldServiceHandle.retrieve() ){
    ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
    return StatusCode::FAILURE;
  }    
  ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
  m_fieldService = &*m_fieldServiceHandle;

  // Get tool for track-prd association
  //
  if( m_useassoTool ) {
    if( m_assoTool.retrieve().isFailure()) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_assoTool<<endmsg; 
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endmsg;
    }
  }

  /// Build framework. Will set up internal variables 
  /// *not* related to the event data object 
  buildFrameWork();
  
  if (m_writeNtuple) {

    ATH_CHECK( service("THistSvc",m_thistSvc)  );

    m_treeName = (std::string("SeedTree_")+name());
    std::replace( m_treeName.begin(), m_treeName.end(), '.', '_' );

    m_outputTree = new TTree( m_treeName.c_str() , "SeedMakerValTool");

    m_outputTree->Branch("eventNumber",    &m_eventNumber,"eventNumber/L");
    m_outputTree->Branch("d0",             &m_d0);
    m_outputTree->Branch("z0",             &m_z0);
    m_outputTree->Branch("pt",             &m_pt);
    m_outputTree->Branch("eta",            &m_eta);
    m_outputTree->Branch("x1",             &m_x1);
    m_outputTree->Branch("x2",             &m_x2);
    m_outputTree->Branch("x3",             &m_x3);
    m_outputTree->Branch("y1",             &m_y1);
    m_outputTree->Branch("y2",             &m_y2);
    m_outputTree->Branch("y3",             &m_y3);
    m_outputTree->Branch("z1",             &m_z1);
    m_outputTree->Branch("z2",             &m_z2);
    m_outputTree->Branch("z3",             &m_z3);
    m_outputTree->Branch("r1",             &m_r1);
    m_outputTree->Branch("r2",             &m_r2);
    m_outputTree->Branch("r3",             &m_r3);
    m_outputTree->Branch("quality",        &m_quality);
    m_outputTree->Branch("seedType",       &m_type);
    m_outputTree->Branch("givesTrack",     &m_givesTrack);
    m_outputTree->Branch("dzdr_b",         &m_dzdr_b);
    m_outputTree->Branch("dzdr_t",         &m_dzdr_t);
    m_outputTree->Branch("track_pt",       &m_trackPt);
    m_outputTree->Branch("track_eta",      &m_trackEta);

    TString fullTreeName = m_treeFolder + m_treeName;

    ATH_CHECK(  m_thistSvc->regTree( fullTreeName.Data(), m_outputTree )  );

  }

  if (msgLvl(MSG::DEBUG)) { 
    EventData data;
    initializeEventData(data);
    data.nprint=0;
    dump(msg(MSG::DEBUG));
  }

  return sc;

}


///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITKNew::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
/// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::newEvent(EventData& data, int iteration)
{

  /// if not done so, book the arrays etc inside the event data object
  if (not data.initialized) initializeEventData(data);

  data.trigger = false;
  if (!m_pixel && !m_sct) return;

  /// pass the iteration info into our data object
  data.iteration = iteration;
  if (iteration <=0) data.iteration = 0;
  /// Erase any existing entries in the data object 
  erase(data);

  data.dzdrmin = m_dzdrmin0;
  data.dzdrmax = m_dzdrmax0;
  data.maxScore = m_maxScore;     ///< max score, where low scores are "better".

  /// in the first iteration, initialise the beam framework - beam spot position and direction
  if (data.iteration == 0) {
    if (m_beamCondService != nullptr) {
      buildBeamFrameWork(data);
    }

    /// Read the field information 
    double magField[3]{0,0,0};
    double globalPos[3] ={10.,10.,0.};

    if (m_fieldService->solenoidOn()) {
      /// retrieve field
      m_fieldService->getFieldZR(globalPos,magField);
      /** 
      * Knowing the field (note that the field cache returns the field in units of kiloTesla!) 
      * allows to set the circle-radius to pT conversion factor.
      * 
      * See for example ATLAS-CONF-2010-072 
      * R[mm] =pT[GeV] / (3·10−4×B[T]) =  pT[MeV] / (300 *Bz[kT])
      * 
      * We actually estimate the circle diameter, 2R, in the seeding. 
      * So what we want is: 2R = pT[MeV] x 2  / (300 x Bz) = K x pT[MeV]. 
      **/
      data.K = 2./(300.*magField[2]);
    } else {
      data.K = 2./(300.* 5. );
    }
    /** helper variables allowing us to directly apply our pt cut on the variables
    * available at seed level. 
    * ipt2K is 1 / (K * 0.9 * pt cut)² 
    **/ 
    data.ipt2K = m_ipt2/(data.K*data.K);
    /// related to the mysterious magic number, m_COF{134*.05*9}
    data.ipt2C = m_ipt2*m_COF;
    data.COFK = m_COF*(data.K*data.K);

    /// set the spacepoint iterator to the beginning of the space-point list  
    data.i_spforseed_ITK = data.l_spforseed_ITK.begin();
    // Set the seed multiplicity strategy of the event data to the one configured
    // by the user for strip seeds
    data.maxSeedsPerSP = m_maxOneSizeSSS;
    data.keepAllConfirmedSeeds = m_alwaysKeepConfirmedStripSeeds;
  } ///< end if-statement for iteration 0 
  else {  /// for the second iteration (PPP pass), don't redo the full init required the first time 
    data.r_first = 0;     ///< reset the first radial bin 
    // Set the seed multiplicity strategy of the event data to the one configured
    // by the user for pixel seeds
    data.maxSeedsPerSP = m_maxOneSizePPP;
    data.keepAllConfirmedSeeds = m_alwaysKeepConfirmedPixelSeeds;

    /// call fillLists to repopulate the candidate space points and exit 
    fillLists(data);
    return;
  }

  if(m_fastTracking){
    fillListsFast(data); return;
  }


  /// the following will only happen in the first iteration 

  data.checketa = data.dzdrmin > 1.;

  /// build the r-binning. 
  float oneOverBinSizeR = 1./m_binSizeR;
  int   maxBinR  = m_nBinsR-1;

  /** This cleans up remaining entries in the data object. 
  * In standard execution, we only run this in the first 
  * iterations on a newly created data object, 
  * in which case this loop will not ever be entered. 
  * Leaving it in place for nonstandard use cases. 
  **/ 
  for (int i=0; i<data.nr; ++i) {
    int n = data.r_index[i];
    data.r_map[n] = 0;
    data.r_Sorted_ITK[n].clear();
  }
  data.ns = data.nr = 0;

  /////////////////////////////
  /// Now, we will populate the space point list in the event data object once. 
  /////////////////////////////

  /// reset the first r index 
  data.r_first = 0;

  /// Get pixels space points containers from store gate 
  if (m_pixel) {

    if (m_spacepointsPixel.isValid()) {
      /// loop over the pixel space points 
      for (const SpacePointCollection* spc: *m_spacepointsPixel) {
        for (Trk::SpacePoint* sp: *spc) {
          
          /// if we use the PRD to track map and this SP has already been used in a track, bail out 
          /// also skip any SP outside the r binning 
          if ((m_useassoTool &&  isUsed(sp)) || sp->r() > m_r_rmax || sp->r() < m_r_rmin) continue;

          /// Remove DBM space points
          ///
          const InDetDD::SiDetectorElement* de= 
            static_cast<const InDetDD::SiDetectorElement*>(sp->clusterList().first->detectorElement());
          if (!de || de->isDBM()) continue;

          /** create a SiSpacePointForSeed from the space point. 
          * This will also add the point to the l_spforseed list and update 
          * the i_spforseed iterator
          **/ 
          InDet::SiSpacePointForSeedITK* sps = newSpacePoint(data, sp);
          /// this can occur if we fail the eta cut 
          if (!sps) continue;

          /// determine the r-bin of this SP. 
          /// done by dividing the radius by the bin size. 
          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          /// catch outliers
          if (radiusBin>maxBinR) radiusBin = maxBinR;

          /// now add the SP to the r-binned vector 
          data.r_Sorted_ITK[radiusBin].push_back(sps);
          /// increment the counter for this bin 
          ++data.r_map[radiusBin];
          /// if this is the first time we see this bin in use, we update the index map for this bin 
          /// to the radius bin index 
          if (data.r_map[radiusBin]==1) data.r_index[data.nr++] = radiusBin;
          /// if this is the highest bin we saw so far, update the r_first member of the data object to this bin
          if (radiusBin > data.r_first) data.r_first = radiusBin;
          /// update the space point counter
          ++data.ns;
        } ///< end loop over space points in collection
      }///< end loop over pixel SP collections
    } ///< end if-statement on valid pixel SP container 
    ++data.r_first; ///< increment r_first past the last occupied bin we saw
  } ///< end pixel case
  
  /// Get sct space points containers from store gate 
  if (m_sct) {

    if (m_spacepointsSCT.isValid()) {

      for (const SpacePointCollection* spc: *m_spacepointsSCT) {
        for (Trk::SpacePoint* sp: *spc) {
          /// as for the pixel, veto already used SP if we are using the PRD to track map in later passes of track finding. 
          /// Also, veto SP outside the maximum and minimum radii
          if ((m_useassoTool &&  isUsed(sp)) || sp->r() > m_r_rmax || sp->r() < m_r_rmin) continue;

          /// create a space point and write it into the data object's list of points 
          InDet::SiSpacePointForSeedITK* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          /// as for PIX, determine the radial bin.
          /// Note that for the SCT we do not update data.r_first. 
          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (radiusBin>maxBinR) radiusBin = maxBinR;
          /// again store the SP in the r-binned vectors 
          data.r_Sorted_ITK[radiusBin].push_back(sps);
          /// update the count of SP in the given bin 
          ++data.r_map[radiusBin];
          /// update the r_index map and data.nr if needed 
          if (data.r_map[radiusBin]==1) data.r_index[data.nr++] = radiusBin;
          /// and increment the SP count too. 
          ++data.ns;
        }
      }
    }

    /// Get sct overlap space points containers from store gate 
    if (m_useOverlap && !data.checketa) {

      if (m_spacepointsOverlap.isValid()) {
  
        for (Trk::SpacePoint* sp: *m_spacepointsOverlap) {
          /// usual rejection of SP used in previous track finding passes if we run with the PRT to track map + check of the max and min radii
          if ((m_useassoTool &&  isUsed(sp)) || sp->r() > m_r_rmax || sp->r() < m_r_rmin) continue;

          /// SP creation, entry into list of the data object 
          InDet::SiSpacePointForSeedITK* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          /// radial bin determination 
          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (radiusBin>maxBinR) radiusBin = maxBinR;
          /// insert into the "histogram" vector
          data.r_Sorted_ITK[radiusBin].push_back(sps);
          /// update the counter for each bin content
          ++data.r_map[radiusBin];
          /// update the bin index list and occupied bin counter
          if (data.r_map[radiusBin]==1) data.r_index[data.nr++] = radiusBin;
          /// and the total SP count too.
          ++data.ns;
        }
      }
    }
  }

  /// negative iterations are not used in the current ITk reco
  if (iteration < 0) data.r_first = 0;

  /// populates the phi-z sorted histograms using the spacepoint lists and r-binning.
  /// after this call, we have a 3D binning
  fillLists(data);

}


///////////////////////////////////////////////////////////////////
/// Initialize tool for new region
///////////////////////////////////////////////////////////////////

/// not used in offline reco (trigger specific)
void InDet::SiSpacePointsSeedMaker_ITKNew::newRegion
(EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{
  if (not data.initialized) initializeEventData(data);

  data.iteration  = 0;
  data.trigger = false;
  erase(data);
  if (!m_pixel && !m_sct) return;

  data.dzdrmin = m_dzdrmin0;
  data.dzdrmax = m_dzdrmax0;
  data.maxScore = m_maxScore;

  if (m_beamCondService != nullptr) {
    buildBeamFrameWork(data);
  }


  double magField[3]{0,0,0};
  double globalPos[3] ={10.,10.,0.};

  // initialise pt cuts and conversion factors 
  if (m_fieldService->solenoidOn()) {
    /// retrieve field
    m_fieldService->getFieldZR(globalPos,magField);
    data.K = 2./(300.*magField[2]);
  } else {
    data.K = 2./(300.* 5. );
  }

  data.ipt2K = m_ipt2/(data.K*data.K);
  data.ipt2C = m_ipt2*m_COF;
  data.COFK = m_COF*(data.K*data.K);

  data.i_spforseed_ITK = data.l_spforseed_ITK.begin();

  float oneOverBinSizeR = 1./m_binSizeR;
  int   maxBinR  = m_nBinsR-1;

  data.r_first = 0;
  data.checketa = false;

  for (int i=0; i<data.nr; ++i) {
    int n = data.r_index[i];
    data.r_map[n] = 0;
    data.r_Sorted_ITK[n].clear();
  }
  data.ns = data.nr = 0;

  // Get pixels space points containers from store gate 
  //
  if (m_pixel && !vPixel.empty()) {

    if ( m_spacepointsPixel.isValid() ) {

      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vPixel) {
        auto w = m_spacepointsPixel->indexFind(l);
	if(w==spce) continue;       
	for (Trk::SpacePoint* sp: **w) {
          float r = sp->r();
          if ((m_useassoTool &&  isUsed(sp)) || r > m_r_rmax || r < m_r_rmin) continue;
          InDet::SiSpacePointForSeedITK* sps = newSpacePoint(data, sp);
          int ir = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (ir>maxBinR) ir = maxBinR;
          data.r_Sorted_ITK[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if (m_sct && !vSCT.empty()) {

    if (m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vSCT) {
	auto w = m_spacepointsSCT->indexFind(l);
	if(w==spce) continue;       
	for (Trk::SpacePoint* sp: **w) {        
          float r = sp->r();
          if ((m_useassoTool &&  isUsed(sp)) || r > m_r_rmax || r < m_r_rmin) continue;
          InDet::SiSpacePointForSeedITK* sps = newSpacePoint(data, sp);
          int ir = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (ir>maxBinR) ir = maxBinR;
          data.r_Sorted_ITK[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }
  fillLists(data);
}



///////////////////////////////////////////////////////////////////
/// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::newRegion
(EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
 const IRoiDescriptor& IRD)
{
  constexpr float twoPi = 2.*M_PI;

  if (not data.initialized) initializeEventData(data);

  newRegion(data, vPixel, vSCT);
  data.trigger = true;

  double dzdrmin = 1./std::tan(2.*std::atan(std::exp(-IRD.etaMinus())));
  double dzdrmax = 1./std::tan(2.*std::atan(std::exp(-IRD.etaPlus ())));
 
  data.zminB = IRD.zedMinus()-data.zbeam[0]; // min bottom Z
  data.zmaxB = IRD.zedPlus ()-data.zbeam[0]; // max bottom Z
  data.zminU = data.zminB+550.*dzdrmin;
  data.zmaxU = data.zmaxB+550.*dzdrmax;
  double fmax    = IRD.phiPlus ();
  double fmin    = IRD.phiMinus();
  if (fmin > fmax) fmin -= twoPi;
  data.ftrig  = (fmin+fmax)*.5;
  data.ftrigW = (fmax-fmin)*.5;
}


///////////////////////////////////////////////////////////////////
/// Methods to initilize different strategies of seeds production
/// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::find3Sp(EventData& data, const std::list<Trk::Vertex>& lv)
{
  if (not data.initialized) initializeEventData(data);

  /// reset the Z interval stored in the data object
  data.zminU = m_zmin;
  data.zmaxU = m_zmax;
  /// mode 2 if we have no vertices in the list, otherwise mode 3 
  int mode = 2;
  if (lv.begin()!=lv.end()) mode = 3;
  /** copy the vertices into the data object, if we have any.
  *   Note that by construction, newv will ALWAYS be false, also if we 
  *   pass vertices. 
  **/
  bool newv = newVertices(data, lv);
  /// update the data object's config 
  if (newv || !data.state || data.nspoint!=3 || data.mode!=mode || data.nlist) {
    data.i_seede_ITK = data.l_seeds_ITK.begin();
    data.state = 1;
    data.nspoint = 3;
    data.nlist = 0;
    data.mode = mode;
    data.endlist = true;
    data.fvNmin = 0;
    data.fNmin = 0;
    data.zMin = 0;
    production3Sp(data); ///< This performs the actual seed finding
  }
  /// reset the i_seed_ITK iterator - this is used to return the seeds to the
  /// consumer when they call next()
  data.i_seed_ITK = data.l_seeds_ITK.begin();

  
  if (msgLvl(MSG::DEBUG)) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}


///////////////////////////////////////////////////////////////////
/// Methods to initilize different strategies of seeds production
/// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::find3Sp(EventData& data, const std::list<Trk::Vertex>& lv, const double* ZVertex)
{
  if (not data.initialized) initializeEventData(data);

  /// Update the data object's Z interval based on the interval passed as arg to this 
  /// function. 
  data.zminU = ZVertex[0];
  if (data.zminU < m_zmin) data.zminU = m_zmin; ///< don't go beyond user-specified intervals
  data.zmaxU = ZVertex[1];
  if (data.zmaxU > m_zmax) data.zmaxU = m_zmax; ///< don't go beyond user-specified intervals

  /// mode 2 when working with the Z constraint
  int mode = 2;
  if (lv.begin()!=lv.end()) mode = 3;
  /** copy the vertices into the data object, if we have any.
  *   Note that by construction, newv will ALWAYS be false, also if we 
  *   pass vertices. 
  **/
  bool newv = newVertices(data, lv);
  /// update the data object's config 
  if (newv || !data.state || data.nspoint!=3 || data.mode!=mode || data.nlist) {
    data.i_seede_ITK = data.l_seeds_ITK.begin();
    data.state = 1;
    data.nspoint = 3;
    data.nlist = 0;
    data.mode = mode;
    data.endlist = true;
    data.fvNmin = 0;
    data.fNmin = 0;
    data.zMin = 0;
    production3Sp(data); ///< This performs the actual seed finding
  }
  /// reset the i_seed_ITK iterator - this is used to return the seeds to the
  /// consumer when they call next()
  data.i_seed_ITK = data.l_seeds_ITK.begin();

  if (msgLvl(MSG::DEBUG)) {  
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}


///////////////////////////////////////////////////////////////////
/// Methods to initilize different strategies of seeds production
/// with variable number space points with or without vertex constraint
/// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::findVSp(EventData& data, const std::list<Trk::Vertex>& lv)
{
  if (not data.initialized) initializeEventData(data);

  data.zminU = m_zmin;
  data.zmaxU = m_zmax;

  int mode = 5;
  if (lv.begin()!=lv.end()) mode = 6;
  bool newv = newVertices(data, lv);

  if (newv || !data.state || data.nspoint!=4 || data.mode!=mode || data.nlist) {
    data.i_seede_ITK = data.l_seeds_ITK.begin();
    data.state = 1;
    data.nspoint = 4;
    data.nlist = 0;
    data.mode = mode;
    data.endlist = true;
    data.fvNmin = 0;
    data.fNmin = 0;
    data.zMin = 0;
    production3Sp(data);
  }
  data.i_seed_ITK = data.l_seeds_ITK.begin();

  if (msgLvl(MSG::DEBUG)) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}



///////////////////////////////////////////////////////////////////
/// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITKNew::dump(EventData& data, MsgStream& out) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.nprint) return dumpEvent(data, out);
  return dumpConditions(data, out);
}

///////////////////////////////////////////////////////////////////
/// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITKNew::dumpConditions(EventData& data, MsgStream& out) const
{
  int n = 42-m_spacepointsPixel.key().size();
  std::string s2;
  for (int i=0; i<n; ++i) s2.append(" ");
  s2.append("|");
  n     = 42-m_spacepointsSCT.key().size();
  std::string s3;
  for (int i=0; i<n; ++i) s3.append(" ");
  s3.append("|");
  n     = 42-m_spacepointsOverlap.key().size();
  std::string s4;
  for (int i=0; i<n; ++i) s4.append(" ");
  s4.append("|");
  n     = 42-m_beamconditions.size();
  std::string s5;
  for (int i=0; i<n; ++i) s5.append(" ");
  s5.append("|");

  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.key() <<s2
     <<endmsg;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.key()<<s3
     <<endmsg;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.key()<<s4
     <<endmsg;
  out<<"| BeamConditionsService   | "<<m_beamconditions<<s5
     <<endmsg;
  out<<"| usePixel                | "
     <<std::setw(12)<<m_pixel 
     <<"                              |"<<endmsg;
  out<<"| useSCT                  | "
     <<std::setw(12)<<m_sct 
     <<"                              |"<<endmsg;
  out<<"| maxSize                 | "
     <<std::setw(12)<<m_maxsize 
     <<"                              |"<<endmsg;
  out<<"| maxSizeSP               | "
     <<std::setw(12)<<m_maxsizeSP
     <<"                              |"<<endmsg;
  out<<"| pTmin  (mev)            | "
     <<std::setw(12)<<std::setprecision(5)<<m_ptmin
     <<"                              |"<<endmsg;
  out<<"| max radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<m_r_rmax 
     <<"                              |"<<endmsg;
  out<<"| radius step             | "
     <<std::setw(12)<<std::setprecision(5)<<m_binSizeR
     <<"                              |"<<endmsg;
  out<<"| min Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmin
     <<"                              |"<<endmsg;
  out<<"| max Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmax
     <<"                              |"<<endmsg;
  out<<"| min space points dR SSS | "
     <<std::setw(12)<<std::setprecision(5)<<m_drminSSS
     <<"                              |"<<std::endl;
  out<<"| max space points dR SSS | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmaxSSS
     <<"                              |"<<std::endl;
  out<<"| min space points dR PPP | "
     <<std::setw(12)<<std::setprecision(5)<<m_drminPPP
     <<"                              |"<<std::endl;
  out<<"| max space points dR PPP | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmaxPPP
     <<"                              |"<<std::endl;
  out<<"| max dZ    impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzver 
     <<"                              |"<<endmsg;
  out<<"| max dZ/dR impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrver 
     <<"                              |"<<endmsg;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_maxdImpact
     <<"                              |"<<endmsg;
  out<<"| max       impact sss    | "
     <<std::setw(12)<<std::setprecision(5)<<m_maxdImpactSSS
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Beam X center           | "
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam Y center           | "
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam Z center           | "
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam X-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[3]
     <<"      |"<<endmsg;
  out<<"| Beam Y-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[1]
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[2]
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[3]
     <<"      |"<<endmsg;
  out<<"| Beam Z-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[3]
     <<"      |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
/// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITKNew::dumpEvent(EventData& data, MsgStream& out) const
{
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| ns                    | "
     <<std::setw(12)<<data.ns
     <<"                              |"<<endmsg;
  out<<"| nsaz                  | "
     <<std::setw(12)<<data.nsaz
     <<"                              |"<<endmsg;
  out<<"| nsazv                 | "
     <<std::setw(12)<<data.nsazv
     <<"                              |"<<endmsg;
  out<<"| seeds                   | "
     <<std::setw(12)<<data.l_seeds_ITK.size()
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}


///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::findNext (EventData& data)
{
  if(data.endlist) return;

  data.i_seede_ITK = data.l_seeds_ITK.begin();

  if(data.mode==2 || data.mode==3 || data.mode==5 || data.mode==6) production3Sp (data);

  data.i_seed_ITK = data.l_seeds_ITK.begin();
  ++data.nlist;
}      



///////////////////////////////////////////////////////////////////
/// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITKNew::newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const
{
  unsigned int s1 = data.l_vertex.size();
  unsigned int s2 = lV.size();

  /// reset the isvertex flag
  data.isvertex = false;
  /// if we had no vertices before and have none now,
  /// we can exit right away 
  if (s1==0 && s2==0) return false;

  /// clean up the vertex list 
  data.l_vertex.clear();
  /// if we have no vertices now, we can exit 
  if (s2 == 0) return false;

  /// otherwise, update the data with the new vertices
  data.isvertex = true;
  for (const Trk::Vertex& v: lV) {
    data.l_vertex.insert(static_cast<float>(v.position().z()));
  }

  /// and also update the z interval, adding 20mm before/after the first/last vertex in z 
  /// make sure not to extend the interval beyond the user-configured z interval. 
  data.zminU = (*data.l_vertex. begin())-20.;
  if (data.zminU < m_zmin) data.zminU = m_zmin;
  data.zmaxU = (*data.l_vertex.rbegin())+20.;
  if (data.zmaxU > m_zmax) data.zmaxU = m_zmax;

  return false;
}


///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::buildFrameWork() 
{
  m_ptmin = std::abs(m_ptmin);
  
  if (m_ptmin < 100.) m_ptmin = 100.;

  /// ensure consistency in the transverse IP cuts 
  if (m_maxdImpactSSS < m_maxdImpact   ) m_maxdImpactSSS = m_maxdImpact;

  /// symmetrise eta cut if eta min not explicitly set 
  if (std::abs(m_etamin) < .1) m_etamin = -m_etamax;
  /// set dz/dr cut values based on eta cuts
  m_dzdrmax0  = 1./std::tan(2.*std::atan(std::exp(-m_etamax)));
  m_dzdrmin0  = 1./std::tan(2.*std::atan(std::exp(-m_etamin)));
  
  /// cache inverse pt cuts 
  m_ipt       = 1./std::abs(m_ptmin);
  m_ipt2      = m_ipt*m_ipt;

  /// set up the score thresholds based on the user-supplied properties
  /// The score of any seeds will always be >= the bonus applied,
  /// since the starting value is |d0|. 
  /// Hence, by subtracting one, we get all seeds which have 
  /// received an additional bonus in addition to the PPP/SSS one, 
  /// which is the confirmation one by construction. 
  m_seedScoreThresholdPPPConfirmationSeed = m_seedScoreBonusPPP - 1.; 
  m_seedScoreThresholdSSSConfirmationSeed = m_seedScoreBonusSSS - 1.;

  /// Build radius sorted containers
  m_nBinsR = static_cast<int>((m_r_rmax+.1)/m_binSizeR);

  /** 
   * Now we construct the radius-azimuthal sorted containers.
   * Binning is determined semi-dynamically based on the tool config 
  **/

  /// determine the phi binning 
  constexpr float twoPi = 2.*M_PI;

  /// The max Nb. of bins possible is given by the binning enum 
  const int   nPhiBinsMax = arraySizePhi;
  const float inverseSizePhiMax = static_cast<float>(nPhiBinsMax)/twoPi; ///< for run-3: 200 : 6.28 ~ 31.8, bin size 0.0314
  constexpr float inverseSizePhiMin = 10./twoPi;

  /// derive the optimum bin size in phi for our needs. 
  /// We do this by checking the size needed for each of the passes we run, 
  /// and then pick the larger one (more conservative). 
  /// This is slightly less optimal than using separate settings per pass, but avoids 
  /// having to book and store in memory two distinct R-phi-Z maps.
  
  if (m_optimisePhiBinning){
    /// case 1: PPP seeds, if we use them
    const float radiusPixelStart = m_fastTracking ? 50. : 40.; /// approximate lowest R location of pixel hits (driven by barrel)
    const float radiusPixelEnd = m_fastTracking ? 250. : 320.;  /// approximate largest R location of pixel hits (driven by endcap)
    const float binSizePhi_PPP = m_pixel ? azimuthalStep(m_ptmin,m_maxdImpact,radiusPixelStart,radiusPixelEnd)/3. : 0.;
    /// case 2: SSS seeds, if we use them
    const float binSizePhi_SSS = m_sct ? azimuthalStep(m_ptmin,m_maxdImpactSSS,m_rminSSS,m_rmaxSSS)/3. : 0.;
    m_inverseBinSizePhiPPP = 1./binSizePhi_PPP;
    m_inverseBinSizePhiSSS = 1./binSizePhi_SSS;
  }
  else {
    /// this is the default phi binning as operated in release 21 - optimised for 
    /// a trajectory with 400 MeV, from the origin, and Rmin = 0 / Rmax = 600mm   float ptm = 400.;
    float ptm = 400.; 
    /// if we cut below 400 MeV, adapt the ptm 
    if (m_ptmin < ptm) ptm = m_ptmin;
    m_inverseBinSizePhiPPP = m_inverseBinSizePhiSSS = ptm /60.;
  }

  /// truncate the bin size to fall within our thresholds
  if      (m_inverseBinSizePhiPPP > inverseSizePhiMax) m_inverseBinSizePhiPPP = inverseSizePhiMax;
  else if (m_inverseBinSizePhiPPP < inverseSizePhiMin) m_inverseBinSizePhiPPP = inverseSizePhiMin;
  if      (m_inverseBinSizePhiSSS > inverseSizePhiMax) m_inverseBinSizePhiSSS = inverseSizePhiMax;
  else if (m_inverseBinSizePhiSSS < inverseSizePhiMin) m_inverseBinSizePhiSSS = inverseSizePhiMin;

  /// now we can determine the number of bins by dividing the interval by the bin size 
  m_maxPhiBinPPP = static_cast<int>(twoPi*m_inverseBinSizePhiPPP);
  /// additional protection against too many bins. Should not happen given constraints above 
  if (m_maxPhiBinPPP >=nPhiBinsMax) m_maxPhiBinPPP = nPhiBinsMax-1;
  m_maxPhiBinSSS = static_cast<int>(twoPi*m_inverseBinSizePhiSSS);
  if (m_maxPhiBinSSS >=nPhiBinsMax) m_maxPhiBinSSS = nPhiBinsMax-1;
  /// recompute inverse bin size, taking into account rounding + truncation
  m_inverseBinSizePhiPPP = (m_maxPhiBinPPP + 1) / twoPi;
  m_inverseBinSizePhiSSS = (m_maxPhiBinSSS + 1) / twoPi;

  buildConnectionMaps(m_nNeighbourCellsBottomPPP,m_nNeighbourCellsTopPPP,
		      m_neighbourCellsBottomPPP, m_neighbourCellsTopPPP,
		      m_maxPhiBinPPP, false);

  buildConnectionMaps(m_nNeighbourCellsBottomSSS,m_nNeighbourCellsTopSSS,
		      m_neighbourCellsBottomSSS, m_neighbourCellsTopSSS,
		      m_maxPhiBinSSS, true);

}


void InDet::SiSpacePointsSeedMaker_ITKNew::buildConnectionMaps(std::array<int,arraySizePhiZ>& nNeighbourCellsBottom,
							       std::array<int,arraySizePhiZ>& nNeighbourCellsTop,
							       std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ>& neighbourCellsBottom,
							       std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ>& neighbourCellsTop,
							       int maxPhiBin, bool isSSS){

  /// Build maps for radius-azimuthal-Z sorted collections. 
  /// Here, we associate which bins are 'connected' to a given phi-Z bin 
  /// for the seeding 

  for (int phiBin=0; phiBin<=maxPhiBin; ++phiBin) {

    int phiBelow = phiBin-1;
    if (phiBelow<0) phiBelow=maxPhiBin; ///< loop around at edges of range

    int phiAbove = phiBin+1;
    if (phiAbove>maxPhiBin) phiAbove=0; ///< loop around at edges of range
    
    /// For each azimuthal region loop through all Z regions
    for (int z=0; z<arraySizeZ; ++z) {

      /// we always include the two neighbouring phi bins for the top / bottom 
      /// space point search 
 
      int twoDbinSamePhi          = phiBin * arraySizeZ+z;
      int twoDbinLowerPhi         = phiBelow*arraySizeZ+z;
      int twoDbinHigherPhi        = phiAbove*  arraySizeZ+z;

      nNeighbourCellsBottom [twoDbinSamePhi]    = 3;
      nNeighbourCellsTop [twoDbinSamePhi]    = 3;

      neighbourCellsBottom[twoDbinSamePhi][0] = twoDbinSamePhi;
      neighbourCellsTop[twoDbinSamePhi][0] = twoDbinSamePhi;

      neighbourCellsBottom[twoDbinSamePhi][1] = twoDbinLowerPhi;
      neighbourCellsTop[twoDbinSamePhi][1] = twoDbinLowerPhi;

      neighbourCellsBottom[twoDbinSamePhi][2] = twoDbinHigherPhi;
      neighbourCellsTop[twoDbinSamePhi][2] = twoDbinHigherPhi;

      /** in addition, we usually add at least one neighbouring slice 
      * in Z. This depends on where we are in the detector. 
      * Guide for the following: 
      * z == 5: central z region, |z|<250mm
      * 0  1  2  3  4    5    6  7  8  9  10  z bin index
      * --------------------------------------> Z[mm]
      *  Z=-2500       IP,Z=0            Z=+2500
      **/
      if (z==5) {
        nNeighbourCellsTop [twoDbinSamePhi]    = 9;
        // in the central z region, we include the two neighbouring 
        // z slices for the top neighbour search   

        neighbourCellsTop[twoDbinSamePhi][3] = twoDbinSamePhi+1;
        neighbourCellsTop[twoDbinSamePhi][4] = twoDbinLowerPhi+1;
        neighbourCellsTop[twoDbinSamePhi][5] = twoDbinHigherPhi+1;
        neighbourCellsTop[twoDbinSamePhi][6] = twoDbinSamePhi-1;
        neighbourCellsTop[twoDbinSamePhi][7] = twoDbinLowerPhi-1;
        neighbourCellsTop[twoDbinSamePhi][8] = twoDbinHigherPhi-1;
      } 
      // z > 5: positive z values, |z| > 250mm 
      else if (z> 5) {
        // for the bottom SP search in positive non-central z, we include the 
        // neighbouring Z region on the left (towards the IP) in the bottom 
        // neighbour search 
        nNeighbourCellsBottom [twoDbinSamePhi]    = 6;
        neighbourCellsBottom[twoDbinSamePhi][3] = twoDbinSamePhi-1;
        neighbourCellsBottom[twoDbinSamePhi][4] = twoDbinLowerPhi-1;
        neighbourCellsBottom[twoDbinSamePhi][5] = twoDbinHigherPhi-1;

        if (z<10) {
        /** for the top SP search in positive z, 
        * if we are not in the outermost z region, 
        * we include the neighbouring Z region on the right (away from the IP). 
        * In z = 10, the most forward, we do not have such a 'right side' neighbour we can add
        **/
          nNeighbourCellsTop [twoDbinSamePhi]    = 6;
          neighbourCellsTop[twoDbinSamePhi][3] = twoDbinSamePhi+1;
          neighbourCellsTop[twoDbinSamePhi][4] = twoDbinLowerPhi+1;
          neighbourCellsTop[twoDbinSamePhi][5] = twoDbinHigherPhi+1;
        }
      } 
      // z < 5: negative z values, |z| > 250mm 
      else {
        /** for the bottom SP in negative non-central z, we include 
        * the neighbouring z region on the right (towards the IP) in the 
        * bottom neighbour search 
        **/
        nNeighbourCellsBottom [twoDbinSamePhi]    = 6;
        neighbourCellsBottom[twoDbinSamePhi][3] = twoDbinSamePhi+1;
        neighbourCellsBottom[twoDbinSamePhi][4] = twoDbinLowerPhi+1;
        neighbourCellsBottom[twoDbinSamePhi][5] = twoDbinHigherPhi+1;

        if (z>0) {
          // if there is a z region on the left (away from the IP), we include it in the top
          // neighbour search 
          nNeighbourCellsTop [twoDbinSamePhi]    = 6;
          neighbourCellsTop[twoDbinSamePhi][3] = twoDbinSamePhi-1;
          neighbourCellsTop[twoDbinSamePhi][4] = twoDbinLowerPhi-1;
          neighbourCellsTop[twoDbinSamePhi][5] = twoDbinHigherPhi-1;
        }
      }

      /** 
      * z bins 3 / 7: 450mm < |z| < 925mm.: 
      * also include the central z region in the bottom SP search.  
      * likely for PPP seeds with hits in pixel barrel + endcaps
      **/
      /// Only used for strip seeds
      if(isSSS){
	if (z==3) {
	  nNeighbourCellsBottom[twoDbinSamePhi]      = 9;
	  neighbourCellsBottom[twoDbinSamePhi][6] = twoDbinSamePhi+2;
	  neighbourCellsBottom[twoDbinSamePhi][7] = twoDbinLowerPhi+2;
	  neighbourCellsBottom[twoDbinSamePhi][8] = twoDbinHigherPhi+2;
	} else if (z==7) {
	  nNeighbourCellsBottom[twoDbinSamePhi]      = 9;
	  neighbourCellsBottom[twoDbinSamePhi][6] = twoDbinSamePhi-2;
	  neighbourCellsBottom[twoDbinSamePhi][7] = twoDbinLowerPhi-2;
	  neighbourCellsBottom[twoDbinSamePhi][8] = twoDbinHigherPhi-2;
	}
      }

    }
  }

}


///////////////////////////////////////////////////////////////////
/// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::buildBeamFrameWork(EventData& data) const
{ 
  const Amg::Vector3D &bsCentre = m_beamCondService->beamPos();
  double     tx = std::tan(m_beamCondService->beamTilt(0));
  double     ty = std::tan(m_beamCondService->beamTilt(1));

  double phi   = std::atan2(ty,tx);
  double theta   = std::acos(1./std::sqrt(1.+tx*tx+ty*ty));
  double sinTheta = std::sin(theta);
  double cosTheta = std::cos(theta);
  double sinPhi = std::sin(phi);
  double cosPhi = std::cos(phi);
  
  data.xbeam[0] = static_cast<float>(bsCentre.x());
  data.xbeam[1] = static_cast<float>(cosTheta*cosPhi*cosPhi+sinPhi*sinPhi);
  data.xbeam[2] = static_cast<float>(cosTheta*sinPhi*cosPhi-sinPhi*cosPhi);
  data.xbeam[3] =-static_cast<float>(sinTheta*cosPhi               );

  data.ybeam[0] = static_cast<float>(bsCentre.y());
  data.ybeam[1] = static_cast<float>(cosTheta*cosPhi*sinPhi-sinPhi*cosPhi);
  data.ybeam[2] = static_cast<float>(cosTheta*sinPhi*sinPhi+cosPhi*cosPhi);
  data.ybeam[3] =-static_cast<float>(sinTheta*sinPhi               );
  
  data.zbeam[0] = static_cast<float>(bsCentre.z());
  data.zbeam[1] = static_cast<float>(sinTheta*cosPhi);
  data.zbeam[2] = static_cast<float>(sinTheta*sinPhi);
  data.zbeam[3] = static_cast<float>(cosTheta);
}

///////////////////////////////////////////////////////////////////
/// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void  InDet::SiSpacePointsSeedMaker_ITKNew::convertToBeamFrameWork
(EventData& data, const Trk::SpacePoint*const& sp, float* r) const
{
  r[0] = static_cast<float>(sp->globalPosition().x())-data.xbeam[0];
  r[1] = static_cast<float>(sp->globalPosition().y())-data.ybeam[0];
  r[2] = static_cast<float>(sp->globalPosition().z())-data.zbeam[0];
}

///////////////////////////////////////////////////////////////////
/// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::fillLists(EventData& data)
{
  constexpr float twoPi = 2.*M_PI;

  int  firstRadialBin =     0;
  int  lastRadialBin  =     0;
  bool endcap = false    ;

  /** 
  * The following is done separately for each iteration. 
  * We sort the hits in our radially sorted lists into the 
  * z-phi binning, keeping only those we want to consider to reduce 
  * combinatorics
  *
  * Note that the use of r_first to start the loop is what ensures that 
  * the first iteration is a pure SSS pass. 
  * In newEvent, r_first is set to the bin after the last 
  * radial bin containing Pixel space points. 
  * For the second iteration, we reset it to zero and thus start in the pixels. 
  **/ 
  
  const std::map<float, int> ztoBin{
      {-2500.    ,0},
      {-1400.    ,1},
      {-925.     ,2},
      {-450.     ,3},
      {-250      ,4},
      { 250      ,5},
      { 450      ,6},
      { 925      ,7},
      { 1400     ,8},
      { 2500     ,9},
      { 100000   ,10},  ///< if we encounter Si hits at z > +100m, we are probably not in ATLAS anymore...
	};

  int nPhiBins            = data.iteration ? m_maxPhiBinPPP : m_maxPhiBinSSS;
  float inverseBinSizePhi = data.iteration ? m_inverseBinSizePhiPPP : m_inverseBinSizePhiSSS;

  for (int radialBin=data.r_first; radialBin<m_nBinsR;  ++radialBin) {

    /// skip empty radial bins
    if (!data.r_map[radialBin]) continue;

    // Stop when we reach strip SP in PPP iteration #1
    std::vector<InDet::SiSpacePointForSeedITK*>::iterator SP_first = data.r_Sorted_ITK[radialBin].begin();
    if(data.iteration && (*SP_first)->spacepoint->clusterList().second) break;

    /// remember the first non-empty bin we encounter 
    if (firstRadialBin == 0) firstRadialBin = radialBin;
    lastRadialBin = radialBin;

    // loop over the space points in the r-bin and sort them into the 2d phi-z binning 
    for (InDet::SiSpacePointForSeedITK* SP : data.r_Sorted_ITK[radialBin]) {
      
      /// Azimuthal angle sort
      /// find the bin by dividing phi in 0...2pi by the bin size 
      float Phi = SP->phi();
      if (Phi<0.) Phi+=twoPi; // phi is defined in [0..2pi] for the binning 
      int phiBin = static_cast<int>(Phi*inverseBinSizePhi);
      /// handle overflows
      if (phiBin < 0) {
        phiBin = nPhiBins;
      } else if (phiBin > nPhiBins) {
        phiBin = 0;
      }

      float Z = SP->z();
      endcap = (std::abs(Z)>1490);
      /** z-coordinate sort. 
      * Here, we have a variable bin size. 
      * Use a map to replace 5 levels of nested ternaries 
      * for a somewhat more readable notation while retaining
      * a logN lookup speed 
      **/
      int zBin{0};
      auto bound = ztoBin.lower_bound(Z); 
      /// some protection in case things go REALLY wrong
      if (bound == ztoBin.end()){
        --bound;  ///< z beyond the max: return last bin 
      }
      zBin=bound->second;

      /// 2D bin index - computed from the 1D using standard 2D array bin arithmetics  
      int twoDbin = phiBin*arraySizeZ+zBin;
      /// increment total counter of space points. 
      /// This is not reset between iterations. 
      ++data.nsaz;
      // push our space point into the 2D binned array
      data.rfz_Sorted_ITK[twoDbin].push_back(SP);
      /// the conditional seems to always be true. The rfz_index vector stores 
      /// the 2D bin for each SP in the radius-sorted map. This way, 
      /// we obtain effectively a *3D binning* in r(via the r-sorted vector), phi and z (via the 2D index)
      if (!data.rfz_map[twoDbin]++) data.rfz_index[data.nrfz++] = twoDbin;
    }
  }

  data.state = 0;

  if(data.iteration){ // PPP
    m_RTmin = m_binSizeR*firstRadialBin+10. ;
    m_RTmax = m_binSizeR*lastRadialBin-10.;
  }
  else{ //SSS
    if ( endcap and m_isLRT) {
      m_RTmin = m_binSizeR*firstRadialBin+10. ;
      m_RTmax = m_binSizeR*lastRadialBin-10.;
    }
    else{
      m_RTmin = m_binSizeR*firstRadialBin+30. ;
      m_RTmax = m_binSizeR*lastRadialBin-150.;
    }
  }

}



///////////////////////////////////////////////////////////////////
/// Initiate space points seed maker for fast tracking
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::fillListsFast(EventData& data)
{
  constexpr float twoPi = 2.*M_PI;

  /// Erase any existing entries in the data object 
  erase(data);

  /** 
  * The following is done separately for each iteration. 
  * We sort the hits in our radially sorted lists into the 
  * z-phi binning, keeping only those we want to consider to reduce 
  * combinatorics
  *
  * Note that the use of r_first to start the loop is what ensures that 
  * the first iteration is a pure SSS pass. 
  * In newEvent, r_first is set to the bin after the last 
  * radial bin containing Pixel space points. 
  * For the second iteration, we reset it to zero and thus start in the pixels. 
  **/ 
  
  const std::map<float, int> ztoBin{
      {-2500.    ,0},
      {-1400.    ,1},
      {-925.     ,2},
      {-450.     ,3},
      {-250      ,4},
      { 250      ,5},
      { 450      ,6},
      { 925      ,7},
      { 1400     ,8},
      { 2500     ,9},
      { 100000   ,10},  ///< if we encounter Si hits at z > +100m, we are probably not in ATLAS anymore...
	};
  
  SG::ReadHandle<SpacePointContainer> spacepoints;
  if(m_sct) spacepoints = m_spacepointsSCT;
  else if(m_pixel) spacepoints = m_spacepointsPixel;

  int nPhiBins            = m_pixel ? m_maxPhiBinPPP : m_maxPhiBinSSS;
  float inverseBinSizePhi = m_pixel ? m_inverseBinSizePhiPPP : m_inverseBinSizePhiSSS;
  
  if (spacepoints.isValid()) {
    /// loop over the space points 
    for (const SpacePointCollection* spc: *spacepoints) {

      SpacePointCollection::const_iterator spFirst  = spc->begin();
      float r[15];
      if(m_pixel) pixInform(*spFirst,r);
      else if(m_sct) sctInform(data,*spFirst,r);

      for (Trk::SpacePoint* sp: *spc) {          

	if(m_useassoTool && isUsed(sp)) continue;
          
	/** create a SiSpacePointForSeedITK from the space point.
	 * This will also add the point to the l_spforseed list and update
	 * the i_spforseed iterator
	 **/
	InDet::SiSpacePointForSeedITK* sps = newSpacePoint(data,sp,r);
	if (!sps) continue;
	
	/// Azimuthal angle sort
	/// find the bin by dividing phi in 0...2pi by the bin size
	float Phi = sps->phi();
	if (Phi<0.) Phi+=twoPi; // phi is defined in [0..2pi] for the binning
	int phiBin = static_cast<int>(Phi*inverseBinSizePhi);
	/// handle overflows
	if (phiBin < 0) {
	  phiBin = nPhiBins;
	} else if (phiBin > nPhiBins) {
	  phiBin = 0;
	}

	float Z = sps->z();
	/** z-coordinate sort.
	 * Here, we have a variable bin size.
	 * Use a map to replace 5 levels of nested ternaries
	 * for a somewhat more readable notation while retaining
	 * a logN lookup speed
	 **/
	int zBin{0};
	auto bound = ztoBin.lower_bound(Z);
	/// some protection in case things go REALLY wrong
	if (bound == ztoBin.end()){
	  --bound;  ///< z beyond the max: return last bin
	}
	zBin=bound->second;
	
	/// 2D bin index - computed from the 1D using standard 2D array bin arithmetics
	int twoDbin = phiBin*arraySizeZ+zBin;
	/// increment total counter of space points.
	/// This is not reset between iterations.
	++data.nsaz;
	// push our space point into the 2D binned array
	data.rfz_Sorted_ITK[twoDbin].push_back(sps);
	/// the conditional seems to always be true. The rfz_index vector stores
	/// the 2D bin for each SP in the radius-sorted map. This way,
	/// we obtain effectively a *3D binning* in r(via the r-sorted vector), phi and z (via the 2D index)
	if (!data.rfz_map[twoDbin]++) data.rfz_index[data.nrfz++] = twoDbin;
      }
    }
  }

  // Loop through all RZ collections and sort them in radius order
  //
  for(int twoDbin(0); twoDbin!=arraySizePhiZ; ++twoDbin) {
    if(data.rfz_Sorted_ITK[twoDbin].size() > 1 ){
      std::sort(data.rfz_Sorted_ITK[twoDbin].begin(), data.rfz_Sorted_ITK[twoDbin].end(), InDet::SiSpacePointsITKComparison_R());

    }
  }

  if(m_pixel){
    m_RTmin = m_rminPPPFast ;
    // m_RTmax set per zBin in production3Sp
  }
  else if(m_sct){
    m_RTmin = m_rminSSS ;
    m_RTmax = m_rmaxSSS ;
  }

}




float InDet::SiSpacePointsSeedMaker_ITKNew::azimuthalStep(const float pTmin,const float maxd0,const float Rmin,const float Rmax) const
{
  /// here we approximate the largest curvature
  /// that can be expected for the seeds we build
  /// using R[mm] ~ pT[MeV] / (0.3 * B[T]), with B == 2T
  float Rm = pTmin/.6;

  /// for LRT, the maximum allowed d0 may be larger than 
  /// the radius at which the innermost hit is possible.
  /// In that case, our "worst case" trajectory
  /// generating the largest phi displacement
  ///  will be the one with a hit at Rmin and 
  /// d0 == Rmin.
  float worstCaseD0 = maxd0;
  if (maxd0 > Rmin) worstCaseD0 = Rmin; 

  float sI = std::abs(std::asin(worstCaseD0/Rmin) - std::asin(worstCaseD0/Rmax)); 
  float sF = std::abs(std::asin(std::min(1.,Rmax/(2.*Rm))) -
                      std::asin(std::min(1.,Rmin/(2.*Rm))));
  return sI+sF; 
}


///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::erase(EventData& data) const
{
  for (int i=0; i<data.nrfz;  ++i) {
    int n = data.rfz_index[i];
    data.rfz_map[n] = 0;
    data.rfz_Sorted_ITK[n].clear();
  }
  
  for (int i=0; i<data.nrfzv; ++i) {
    int n = data.rfzv_index[i];
    data.rfzv_map[n] = 0;
    data.rfzv_Sorted_ITK[n].clear();
  }
  data.state = 0;
  data.nsaz  = 0;
  data.nsazv = 0;
  data.nrfz  = 0;
  data.nrfzv = 0;
}


///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::production3Sp(EventData& data)
{

  /// we need at least 3 SP in our phi-z binning, otherwise can't build 3-SP seeds
  if (data.nsaz<3) return;

  /**
   *  This method will run a separate seed formation round
   *  for each phi-Z region, taking the central SP from there
   *  and allowing the top/bottom SP to come from either the same
   *  or certain neighbouring bins. 
   *  
   *  The search in each region is performed in the overload of this 
   *  method with the extended signature below. 
   *  Here, we implement the loop over the 2D regions
   **/


  /** 
  * Order how we walk across z. 
  * 0-4 are negative z, 5 is central z, 6-10 are positive z.
  * 0  1  2  3  4    5    6  7  8  9  10  z bin index
  * --------------------------------------> Z[mm]
  *  Z=-2500       IP,Z=0            Z=+2500
  **/ 
  const std::array<int,arraySizeZ> zBinIndex_SSS {5,6,4,7,3,8,2,9,1,10,0};
  const std::array<int,arraySizeZ> zBinIndex_PPP_fast {0,10,1,9,2,8,5,3,7,4,6};
  const std::array<int,arraySizeZ> zBinIndex_PPP_long {0,1,2,3,10,9,8,7,5,4,6};
  const auto zBinIndex_PPP = m_fastTracking ? zBinIndex_PPP_fast : zBinIndex_PPP_long;
  // Fast tracking runs a single iteration, either pixel or strip
  // Default tracking runs a 0-th iteration for strip then a 1-st for pixel
  bool isPixel = (m_fastTracking && m_pixel) || data.iteration==1;
  const auto zBinIndex = isPixel ? zBinIndex_PPP : zBinIndex_SSS;

  const float   RTmax[11] = { 80.,200.,200.,200.,250.,250.,250.,200.,200.,200., 80.};
  const float   RTmin[11] = { 40., 40., 70., 70., 70., 70., 70., 70., 70., 40., 40.};

  /// prepare arrays to store the iterators over the SP containers for all 
  /// neighbouring cells we wish to consider in the seed formation
  std::array<std::vector<InDet::SiSpacePointForSeedITK*>::iterator,arraySizeNeighbourBins> iter_topCands;
  std::array<std::vector<InDet::SiSpacePointForSeedITK*>::iterator,arraySizeNeighbourBins> iter_endTopCands;
  std::array<std::vector<InDet::SiSpacePointForSeedITK*>::iterator,arraySizeNeighbourBins> iter_bottomCands;
  std::array<std::vector<InDet::SiSpacePointForSeedITK*>::iterator,arraySizeNeighbourBins> iter_endBottomCands;
  
  int nPhiBins;
  std::array<int,arraySizePhiZ> nNeighbourCellsBottom;
  std::array<int,arraySizePhiZ> nNeighbourCellsTop;
  std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ> neighbourCellsBottom;
  std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ> neighbourCellsTop;

  if(isPixel){
    nPhiBins = m_maxPhiBinPPP;
    nNeighbourCellsBottom = m_nNeighbourCellsBottomPPP;
    nNeighbourCellsTop = m_nNeighbourCellsTopPPP;
    neighbourCellsBottom = m_neighbourCellsBottomPPP;
    neighbourCellsTop = m_neighbourCellsTopPPP;
  }
  else{
    nPhiBins = m_maxPhiBinSSS;
    nNeighbourCellsBottom = m_nNeighbourCellsBottomSSS;
    nNeighbourCellsTop = m_nNeighbourCellsTopSSS;
    neighbourCellsBottom = m_neighbourCellsBottomSSS;
    neighbourCellsTop = m_neighbourCellsTopSSS;
  }

  /// counter for the found
  int    nseed =    0;
  /// prevent another pass from being run when we run out of Seeds
  data.endlist = true;

  /// Loop through all azimuthal regions
  for (int phiBin=data.fNmin; phiBin<=nPhiBins; ++phiBin) {
    
    /// For each azimuthal region loop through all Z regions
    int z = (m_fastTracking && m_pixel) ? 2 : 0;
    /// If we had to abort a previous run, continue where we left off 
    if (!data.endlist) z = data.zMin;

    /// note that this loop follows the order within 'zBinIndex',
    /// not the ascending order of z regions. We start in the centre, 
    /// not at -2500 mm, and then move outward. 
    for (; z<arraySizeZ; ++z) {
      
      if(m_fastTracking && m_pixel){
	    m_RTmax = RTmax[ zBinIndex[z] ];
            m_RTmin = RTmin[ zBinIndex[z] ];
      }

      int phiZbin  = phiBin *arraySizeZ+zBinIndex[z];

      /// can skip the rest if this particular 2D bin is empty 
      if (!data.rfz_map[phiZbin]) continue;

      /// count how many non-emtpy cells should be searched for the 
      /// top and bottom neighbour 
      int numberBottomCells = 0;
      int numberTopCells = 0;

      /// walk through the cells in phi-z we wish to consider for the bottom SP search. 
      /// Typically, this will be 3 adjacent phi bins (including the one of the central SP) 
      /// and possibly neighbours in z on side towards the IP or on both sides, 
      /// depdending on the z region we are in
      for (int neighbourCellNumber=0; neighbourCellNumber<nNeighbourCellsBottom[phiZbin]; ++neighbourCellNumber) {
  
        int theNeighbourCell =  neighbourCellsBottom[phiZbin][neighbourCellNumber];
        /// only do something if this cell is populated 
        if (!data.rfz_map[theNeighbourCell]) continue;
        /// plug the begin and end iterators to the SP in the cell into our array 
        iter_bottomCands [numberBottomCells] = data.rfz_Sorted_ITK[theNeighbourCell].begin();
        iter_endBottomCands[numberBottomCells++] = data.rfz_Sorted_ITK[theNeighbourCell].end();
      } 

      /// walk through the cells in phi-z we wish to consider for the top SP search. 
      /// Typically, this will be 3 adjacent phi bins (including the one of the central SP) 
      /// and possibly neighbours in z on the side opposed to the IP or on both sides, 
      /// depdending on the z region we are in
      for (int neighbourCellNumber=0; neighbourCellNumber<nNeighbourCellsTop[phiZbin]; ++neighbourCellNumber) {

        int theNeighbourCell =  neighbourCellsTop[phiZbin][neighbourCellNumber];
        /// only do something if this cell is populated 
        if (!data.rfz_map[theNeighbourCell]) continue;
        /// plug the begin and end iterators to the SP in the cell into our array 
        iter_topCands [numberTopCells] = data.rfz_Sorted_ITK[theNeighbourCell].begin();
        iter_endTopCands[numberTopCells++] = data.rfz_Sorted_ITK[theNeighbourCell].end();
      } 

      /// now run the seed search for the current phi-z bin.
      if (!data.trigger){
          if(isPixel) production3SpPPP    (data, iter_bottomCands, iter_endBottomCands, iter_topCands, iter_endTopCands, numberBottomCells, numberTopCells, nseed);
          else        production3SpSSS    (data, iter_bottomCands, iter_endBottomCands, iter_topCands, iter_endTopCands, numberBottomCells, numberTopCells, nseed);
      }
      else            production3SpTrigger(data, iter_bottomCands, iter_endBottomCands, iter_topCands, iter_endTopCands, numberBottomCells, numberTopCells, nseed);
    }

    /** If we exceed the seed capacity, we stop here. 
     * Save where we were in z and phi, and set endlist to false. 
     * This will trigger another run of production3Sp when 
     * The client calls next() after processing all vertices seen 
     * so far (freeing up capacity). 
     **/ 
    if (nseed>=m_maxsize) {
      data.endlist=false;
      data.fNmin  =  phiBin+1;
      return;
    }
  }
  /// Processed all seeds there are without aborting - no re-run needed!
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
/// Production 3 space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::production3SpSSS
(EventData& data,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_bottomCands ,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endBottomCands,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_topCands ,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endTopCands,
 const int numberBottomCells, const int numberTopCells, int& nseed) const
{

  /** 
   * This method implements the seed search for a single phi-Z region of the detector. 
   * The central SP is taken from the region, while the top and bottom SP are allowed 
   * to come from either the same or a range of neighbouring cells. 
   **/ 

  /// iterator across the candidates for the central space point. 
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator iter_centralSP=iter_bottomCands[0];
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator iter_otherSP;  ///< will be used for iterating over top/bottom SP

  /** 
   * Next, we work out where we are within the ATLAS geometry.
   * This will help us identify which space-points we need to 
   * consider as potential central points of a seed. 
   **/
 
  /// find the first central SP candidate above the minimum radius. 
  for(; iter_centralSP!=iter_endBottomCands[0]; ++iter_centralSP) {
    if((*iter_centralSP)->radius() > m_RTmin) break;
  }

  /// for the top candidates in the central phi-Z bin, we do not need to start at a smaller
  /// radius than the lowest-r valid central SP candidate  
  iter_topCands[0] = iter_centralSP; 
  ++iter_topCands[0];

  /// prepare cut values 
  const float& ipt2K = data.ipt2K;
  const float& ipt2C = data.ipt2C;
  const float& COFK  = data.COFK;
  const float& maxd0cut = m_maxdImpactSSS;
  const float& zmax  = data.zmaxU;
  data.CmSp_ITK.clear();

  /// keep track of the SP storace capacity. 
  /// Extend it needed (should rarely be the case)
  size_t SPcapacity = data.SP_ITK.size(); 

  /// Loop through all central space point candidates
  for (; iter_centralSP!=iter_endBottomCands[0]; ++iter_centralSP) {

    const float& R  = (*iter_centralSP)->radius();
    if(R > m_RTmax) break; ///< stop if we have moved outside our radial region of interest. 

    /// global coordinates of the central SP
    const float&        X    = (*iter_centralSP)->x();
    const float&        Y    = (*iter_centralSP)->y();
    const float&        Z    = (*iter_centralSP)->z();

    /// for the central SP, we veto locations on the last disk - 
    /// there would be no "outer" hits to complete a seed. 
    double absZ = std::abs(Z); 
    /// veto the last strip disk
    if (absZ > m_zmaxSSS) continue;

    /// initialise a counter for found bottom links
    /// This also serves as an index in the data.SP vector
    size_t Nt = 0;
    
    /// Top links production
    /// Loop over all the cells where we expect to find such SP 
    for (int cell=0; cell<numberTopCells; ++cell) {

      for (iter_otherSP=iter_topCands[cell];iter_otherSP!=iter_endTopCands[cell]; ++iter_otherSP) {
	/// evaluate the radial distance, 
        float Rt =(*iter_otherSP)->radius();
        float dR = Rt-R;
        if(dR >= m_drminSSS) break;
      }
      iter_topCands[cell] = iter_otherSP;

      /// loop over each SP in each cell 
      for (iter_otherSP=iter_topCands[cell];iter_otherSP!=iter_endTopCands[cell]; ++iter_otherSP) {
  
        /// evaluate the radial distance, 
        float Rt =(*iter_otherSP)->radius();
        float dR = Rt-R;
        /// if we are to far, the next ones will be even farther, so abort 
        if (dR>m_drmaxSSS) break;

	const float dz   = (*iter_otherSP)->z()-Z;
        const float dZdR = dz/dR;
  
        /// Comparison with vertices Z coordinates
        /// straight line extrapolation to r=0 
        const float z0 = Z-R*dZdR;
	if(std::abs(dz) > m_dzmaxSSS || std::abs(z0) > zmax) continue;

        /// add SP to the list
        data.SP_ITK[Nt] = (*iter_otherSP);
        data.SP_ITK[Nt]->setDZDR(dZdR);
        /// if we are exceeding the SP capacity of our data object,
        /// make it resize its vectors. Will add 50 slots by default,
        /// so rarely should happen more than once per event.  
        if (++Nt==SPcapacity) {
          data.resizeSPCont(); 
          SPcapacity=data.SP_ITK.size();
        }
      } ///< end of loop over SP within top candidate cell
    } ///< end of loop over top candidate cells

    /// if we did not find ANY top SP, or if we exceed the storage capacity, we abort this seed candidate. 
    if (!Nt) continue;

    /// now continue with the bottom SP search. 
    /// Make the counter start at Nt, as this serves as a running 
    /// index in the SP list for this seed. 
    size_t                 Nb   = Nt;

    /// Bottom links production
    /// Loop over all the cells where we expect to find such SP 
    for (int cell=0; cell<numberBottomCells; ++cell) {
        
      for(iter_otherSP=iter_bottomCands[cell]; iter_otherSP!=iter_endBottomCands[cell]; ++iter_otherSP) {
        if((R-(*iter_otherSP)->radius()) <= m_drmaxSSS) break;
      }  
      iter_bottomCands[cell]=iter_otherSP;
      /// in each cell, loop over the space points
      for (; iter_otherSP!=iter_endBottomCands[cell]; ++iter_otherSP) {
        /// evaluate the radial distance between the central and bottom SP
        const float& Rb =(*iter_otherSP)->radius();
        float dR = R-Rb;

        /// if the points are too close in r, abort (future ones will be even closer). 
        if (dR < m_drminSSS) break;

	const float dz   = Z-(*iter_otherSP)->z();
        const float dZdR = dz/dR;
  
        /// Comparison with vertices Z coordinates
        /// straight line extrapolation to r=0 
        const float z0 = Z-R*dZdR;
	if(std::abs(dz) > m_dzmaxSSS || std::abs(z0) > zmax) continue;
        /// found a bottom SP candidate, write it into the data object
        data.SP_ITK[Nb] = (*iter_otherSP);
        data.SP_ITK[Nb]->setDZDR(dZdR);
        /// if we are exceeding the SP capacity of our data object,
        /// make it resize its vectors. Will add 50 slots by default,
        /// so rarely should happen more than once per event.  
        if (++Nb==SPcapacity){
	  data.resizeSPCont();
	  SPcapacity=data.SP_ITK.size();
        }
      } ///< end of loop over SP in bottom candidate cell
    } ///< end of loop over bottom candidate cells

    /// if we found no bottom candidates (remember, Nb starts counting at Nt), abort 
    if (!(Nb-Nt)) continue;

    /// get covariance on r and z for the central SP
    float covr0 = (*iter_centralSP)->covr ();
    float covz0 = (*iter_centralSP)->covz ();

    /// build a unit direction vector pointing from the IP to the central SP
    float ax    = X/R;
    float ay    = Y/R;

    /// check all SP candidates we found during our loop and 
    /// compute geometrical variables w.r.t the central point. 
    for (size_t i=0; i<Nb; ++i) {

      InDet::SiSpacePointForSeedITK* sp = data.SP_ITK[i];

      /// transform the space point coordinates into a frame centered around the middle SP,
      /// where the x axis points away from the detector frame origin 
      float dx  = sp->x()-X;
      float dy  = sp->y()-Y;
      float dz  = sp->z()-Z;
      float x   = dx*ax+dy*ay;
      float y   = dy*ax-dx*ay;

      /// inverse square distance of the candidate space point to the central point 
      float r2  = 1./(x*x+y*y);
      /// inverse distance of the candidate space point to the central point 
      float dr  = std::sqrt(r2);
      /// estimate slope in z - distance traveled in transverse plane vs z direction. 
      /// rough estimate of 1/tan theta from 2 points
      float tz  = dz*dr;

      /// if we are looking at a bottom SP candidate, flip the sign to account for 
      /// different direction of flight (from bottom to central) 
      if (i >= Nt) tz = -tz;

      /// save this into our data object
      data.X [i]   = x           ;
      data.Y [i]   = y           ;
      data.Tz[i]   = tz;      ///< 1/ tan theta
      data.Zo[i]   = Z-R*tz;  ///< z0 estimate. 
      data.R [i]   = dr;      ///< inverse distance to central SP
      data.U [i]   = x*r2;    ///< transformed U coordinate  
      data.V [i]   = y*r2;    ///< transformed V coordinate 
      data.Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;    ///<squared Error on 1/tan theta coming from the space-point position errors
    }

    data.nOneSeeds = 0;
    data.nOneSeedsQ =0;
    data.mapOneSeeds_ITK.clear();
    data.mapOneSeeds_ITKQ.clear();

    /// Three space points comparison
    /// first, loop over the bottom point candidates
    for (size_t b=Nt; b<Nb; ++b) {
      
      /// retrieve the geometrical paranmeters w.r.t the central SP for this candidate
      float  Zob  = data.Zo[b];       ///< z0 estimate from central+bottom SP
      float  Tzb  = data.Tz[b];       ///< 1/tanTheta estimate from central+bottom SP      
      float  Rb2r = data.R[b]*covr0;
      float  Rb2z = data.R[b]*covz0;
      float  Erb  = data.Er[b];       ///< this is the uncertainty in 1/tanTheta on the bottom segment resulting from the position errors in the 2 SP
      float  Vb   = data.V [b];       ///< v-coordinate of bottom SP  
      float  Ub   = data.U [b];       ///< u-coordinate of bottom SP
      float  Tzb2 = (1.+Tzb*Tzb);     ///< 1+1/tan²theta - converts transverse to total squared pt
      float sTzb2 = std::sqrt(Tzb2);       ///< sqrt (1+1/tan²theta) - used to convert pt to |p|
      float Se    = 1./std::sqrt(Tzb2);
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      float sigmaSquaredScatteringPtDependent  = Tzb2*COFK;        ///< this, when divided by the 2R², yields an approximated multiple scattering term assuming the measured pt. 
      float sigmaSquaredScatteringMinPt  = Tzb2*ipt2C;       ///< this is an approximate worst case multiple scattering term assuming the lowest 
                                      ///  pt we allow and the estimated theta angle
      /// max IP
      float d0max  =  maxd0cut;

      /// inner loop over the top point candidates
      for (size_t t=0;  t<Nt; ++t) {

        /** 
        * The following exploits the transformation u:=x/(x²+y²); v:=y/(x²+y²); 
        * This is applied on the x,y coordinates in the frame described above, where the 
        * origin is put in the central SP and the x axis defined to point directly away from the IP.
        * 
        * In this transformed u,v frame, what would be our circle in x-y space takes the form of  
        * a linear function V = (-x0/y0) x U + 1/(2y0) =: A x U + B/2.
        * Here, x0 and y0 describe the center point of the circle in the x-y frame. 
        * As the origin of the x-y frame (the middle space point of our seed) is on the circle, 
        * we have x0²+y0²=R² with circle radius R. 
        * 
        * For our seed, we can experimentally obtain A as the slope of the linear function, 
        * delta V / delta U, 
        * estimated using the delta U and delta V between the top and bottom space point. 
        * 
        * B is then obtained by inserting the obtained A into the 
        * linear equation for the bottom SP, A x U + B/2 = V --> B = 2(V - A x U0 
        * 
        * With x0²+y0²=R², and x0=-A/B and y0=1/B, the radius of the circle is 
        * then obtained as R²=(1+A²)/B². 
        **/ 


	// Trigger point
	//
	float dU0   =  data.U[t]-Ub       ;  if(dU0 == 0.) continue;
	float A0    = (data.V[t]-Vb)/dU0  ;
	
	float Cn    = Ce*std::sqrt(1.+A0*A0);
	
	float dn[3] = {Sx-Sy*A0,Sx*A0+Sy,Cn};
	float rn[3];  if(!  (*iter_centralSP)->coordinates(dn,rn)) continue;
	
	// Bottom  point
	//
	float B0    = 2.*(Vb-A0*Ub);
	float Cb    =  1.-B0*data.Y[b];
	float Sb    =  A0+B0*data.X[b];
	float db[3] = {Sx*Cb-Sy*Sb,Sx*Sb+Sy*Cb,Cn};
	float rb[3];  if(!data.SP_ITK[b]->coordinates(db,rb)) continue;

	// Top     point
	//
	float Ct    =  1.-B0*data.Y[t];
	float St    =  A0+B0*data.X[t];
	float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Cn};
	float rt[3];  if(!data.SP_ITK[t]->coordinates(dt,rt)) continue;
	
	float xb    = rb[0]-rn[0];
	float yb    = rb[1]-rn[1];
	float zb    = rb[2]-rn[2];
	float xt    = rt[0]-rn[0];
	float yt    = rt[1]-rn[1];
	float zt    = rt[2]-rn[2];
	
	float rb2   = 1./(xb*xb+yb*yb);
	float rt2   = 1./(xt*xt+yt*yt);
	
	float tb    = -zb*std::sqrt(rb2);
	float tz    = zt*std::sqrt(rt2);

	
	/// Apply a cut on the compatibility between the r-z slope of the two seed segments.
	/// This is done by comparing the squared difference between slopes, and comparing
	/// to the squared uncertainty in this difference - we keep a seed if the difference
	/// is compatible within the assumed uncertainties.
        
	/// average value of 1/tan(theta), approximate the slope at the location of the central space point
	float meanOneOverTanTheta = (tb+tz)/2.;

	/// squared error on the difference in tan(theta) due to space point position errors.
	float sigmaSquaredSpacePointErrors = Erb+data.Er[t]   /// pre-computed individual squared errors on 1/tan(theta) for the two segments
	  + 2 * Rb2z * data.R[t]    /// mixed term with z-uncertainty on central SP
	  + 2 * Rb2r * data.R[t] * meanOneOverTanTheta * meanOneOverTanTheta; // mixed term with r-uncertainy on central SP	
	
	/// start out by subtracting from the squared difference in 1/tanTheta the space-point-related squared error
	float remainingSquaredDelta  = (tb-tz)*(tb-tz) - sigmaSquaredSpacePointErrors;
	
	/// First, we test using a generous scattering term calculated assuming the minimum pt we expect
	/// to reconstruct.
	if (remainingSquaredDelta  - sigmaSquaredScatteringMinPt > 0 ) continue;
	
	float Rn    = std::sqrt(rn[0]*rn[0]+rn[1]*rn[1]);
	float Ax    = rn[0]/Rn;
	float Ay    = rn[1]/Rn;
	
	float ub    = (xb*Ax+yb*Ay)*rb2;
	float vb    = (yb*Ax-xb*Ay)*rb2;
	float ut    = (xt*Ax+yt*Ay)*rt2;
	float vt    = (yt*Ax-xt*Ay)*rt2;
	
	float dU  = ut-ub; if(dU == 0.) continue;
	float A   = (vt-vb)/dU;
	float onePlusAsquare  = 1.+A*A                           ;
	float B   = vb-A*ub                          ;
	float BSquare  = B*B                              ;

	/** With this radius (and pT) estimate, we can apply our pt cut.
	 * Reminder, ipt2K is 1 / (K x 0.9 x pt-cut)², where K translates pt into 2R.
	 * So here we can apply the pt cut directly on the (2R)² estimate without
	 * the extra overhead of conversion / division.
	 * The second check is a refinement of the above Tz compatibility cut,
	 * replacing the sigmaSquaredScatteringMinPt scattering contribution which assumes the lowest pt
	 * by one based on the actual estimated pt.
	 *
	 * The second term in this if-statement applies a second version of the
	 * 1/tan(theta) compatibility, this time using a scattering term scaled by the actual measured
	 * pt. This refines the cut applied above, following the same logic ("delta² - sigma² ?<=0")
	 **/
	if (BSquare  > ipt2K*onePlusAsquare || remainingSquaredDelta*onePlusAsquare > BSquare*sigmaSquaredScatteringPtDependent) continue;

	/** This is an estimate of the transverse impact parameter.
	 * The reasoning is that, in the x-y frame with the central SP as origin and
	 * the x axis pointing away from the IP, we have for the distance between
	 * the IP and the middle of the circle:
	 * (x0 - r_central)²+y0² = (R + d0)²,
	 * with R being the circle radius and r_central
	 * the radial location of the central SP, placing the IP at IP at (-r_central, 0).
	 *
	 * First simplify using R² =x0²+y0², then apply the approximation d0²/R² ~ 0.
	 *
	 * Finally, consider that locally close to the central SP, the circle is parallel to the x axis,
	 * so A = 0 --> expand (2R)²=(1+A²)/B² around this point to obtain
	 * d0 = r_central x (r_central x B - A).
	 * Note that below, the variable R is the radial coordinate fo the central SP,
	 * corresponding to r_central in the notation above.
	 **/
	float d0 = std::abs((A-B*Rn)*Rn);

	/// apply d0 cut to seed
	if (d0 <= d0max) {
	  /// evaluate distance the two closest-by SP in this seed candidate
	  float dr = std::sqrt(1/rb2);
	  if (data.R[t] < data.R[b]) dr = std::sqrt(1/rt2);
	  /// obtain a quality score - start from the d0 estimate, and add
	  /// a penalty term corresponding to how far the seed segments
	  /// deviate from a straight line in r-z
	  data.SP_ITK[t]->setScorePenalty(std::abs((tb-tz)/(dr*sTzb2)));
	  data.SP_ITK[t]->setParam(d0);
	  float DR = std::sqrt(xt*xt+yt*yt+zt*zt); // distance between top and central SP
	  data.SP_ITK[t]->setDR(DR);
      
          if (std::abs(meanOneOverTanTheta)<1e-8) meanOneOverTanTheta = 1e-8;
          if (BSquare<1e-8) BSquare = 1e-8;
          if(data.SP_ITK[t]->z()<0) meanOneOverTanTheta = -meanOneOverTanTheta;
          float theta = std::atan(1./meanOneOverTanTheta);
          data.SP_ITK[t]->setEta(-std::log(std::tan(0.5*theta)));
          data.SP_ITK[t]->setPt(std::sqrt(onePlusAsquare/BSquare)/(1000*data.K));

	  /// record one possible seed candidate, sort by the curvature
	  data.CmSp_ITK.emplace_back(std::make_pair(B/std::sqrt(onePlusAsquare), data.SP_ITK[t]));
	  /// store the transverse IP, will later be used as a quality estimator
	  if(data.CmSp_ITK.size()==500) break;
	}
    
      }   ///< end loop over top space point candidates
      /// now apply further cleaning on the seed candidates for this central+bottom pair. 
      if (!data.CmSp_ITK.empty()) {
        newOneSeedWithCurvaturesComparisonSSS(data, data.SP_ITK[b], (*iter_centralSP), Zob);
      }
    } ///< end loop over bottom space points
    ///record seeds found in this run  
    fillSeeds(data);
    nseed += data.fillOneSeeds;

  } ///< end loop over central SP 
}


void InDet::SiSpacePointsSeedMaker_ITKNew::production3SpPPP
(EventData& data,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_bottomCands ,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endBottomCands,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_topCands ,
 std::array <std::vector<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endTopCands,
 const int numberBottomCells, const int numberTopCells, int& nseed)
{
  /** 
   * This method implements the seed search for a single phi-Z region of the detector. 
   * The central SP is taken from the region, while the top and bottom SP are allowed 
   * to come from either the same or a range of neighbouring cells. 
   **/ 

  /// iterator across the candidates for the central space point. 
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator iter_centralSP=iter_bottomCands[0];

  /** 
   * Next, we work out where we are within the ATLAS geometry.
   * This will help us identify which space-points we need to 
   * consider as potential central points of a seed. 
   **/
 
  /// find the first central SP candidate above the minimum radius. 
  for(; iter_centralSP!=iter_endBottomCands[0]; ++iter_centralSP) {
    if((*iter_centralSP)->radius() > m_RTmin) break;
  }

  /// for the top candidates in the central phi-Z bin, we do not need to start at a smaller
  /// radius than the lowest-r valid central SP candidate  
  iter_topCands[0] = iter_centralSP; 
  ++iter_topCands[0];

  /// prepare cut values 
  const float& ipt2K = data.ipt2K;
  const float& ipt2C = data.ipt2C;
  const float& COFK  = data.COFK;
  const float& maxd0cut = m_maxdImpact;
  const float& zmax  = data.zmaxU;
  const float& dzdrmax = data.dzdrmax; 
  data.CmSp_ITK.clear(); 

  /// keep track of the SP storace capacity. 
  /// Extend it needed (should rarely be the case)
  size_t SPcapacity = data.SP_ITK.size(); 

  /// Loop through all central space point candidates
  for (; iter_centralSP!=iter_endBottomCands[0]; ++iter_centralSP) {

    const float& R  = (*iter_centralSP)->radius();
    if(R > m_RTmax) break; ///< stop if we have moved outside our radial region of interest. 

    /// global coordinates of the central SP
    const float&        X    = (*iter_centralSP)->x();
    const float&        Y    = (*iter_centralSP)->y();
    const float&        Z    = (*iter_centralSP)->z();

    /// for the central SP, we veto locations on the last disk - 
    /// there would be no "outer" hits to complete a seed. 
    double absZ = std::abs(Z); 
    if (!m_fastTracking && absZ > m_zmaxPPP) continue;

    float covr0 = (*iter_centralSP)->covr ();
    float covz0 = (*iter_centralSP)->covz ();
    float Ri    = 1./R          ;
    float ax    = X*Ri           ;
    float ay    = Y*Ri           ;
    float VR    = maxd0cut/(R*R) ;
    size_t   Ntm   = 2; if(R > m_rmaxPPP) Ntm = 1;

    /// initialise a counter for found bottom links
    /// This also serves as an index in the data.SP vector
    size_t Nt = 0;
    /// Top links production
    /// Loop over all the cells where we expect to find such SP 
    for (int cell=0; cell<numberTopCells; ++cell) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator iter_otherSP = iter_topCands[cell], iter_otherSPend = iter_endTopCands[cell];
      if (iter_otherSP == iter_otherSPend) continue;

      for(; iter_otherSP!=iter_otherSPend; ++iter_otherSP) {
        if(( (*iter_otherSP)->radius()- R ) >= m_drminPPP) break;
      } 
      iter_topCands[cell]=iter_otherSP; 
      /// loop over each SP in each cell 
      for (;iter_otherSP!=iter_otherSPend; ++iter_otherSP) {

	/// evaluate the radial distance,
	float Rt =(*iter_otherSP)->radius();
	float dR = Rt-R;
                     
	const float dz   = (*iter_otherSP)->z()-Z;
	const float dZdR = dz/dR;
	/// Comparison with vertices Z coordinates
	/// straight line extrapolation to r=0
	const float z0 = Z-R*dZdR;
	if(std::abs(z0) > zmax) continue;
	
	float dx  = (*iter_otherSP)->x()-X   ;
	float dy  = (*iter_otherSP)->y()-Y   ;
	float x   = dx*ax+dy*ay ;
	float y   = dy*ax-dx*ay ;
	float dxy = x*x+y*y     ;
	float r2  = 1./dxy      ;
	float u   = x*r2        ;
	float v   = y*r2        ;
	
	if(std::abs(R*y) > maxd0cut*x) {
	  float V0; y < 0. ? V0 = VR : V0=-VR;
	  float A   = (v-V0)/(u+1./R);
	  float B   = V0+A/R;
	  if((B*B) > (ipt2K*(1.+A*A))) continue;
	}

	const float dr   = std::sqrt(r2);
	const float tz = dz*dr;
	/// this is effectively a segment-level eta cut - exclude too shallow seed segments
	if(std::abs(tz) > dzdrmax) continue;
	  
	/// add SP to the list
	data.SP_ITK[Nt] = (*iter_otherSP);
	data.R [Nt]   = dr;      ///< inverse distance to central SP
	data.U [Nt]   = u;    ///< transformed U coordinate
	data.V [Nt]   = v;    ///< transformed V coordinate
	data.Er[Nt]   = ((covz0+(*iter_otherSP)->covz())+(tz*tz)*(covr0+(*iter_otherSP)->covr()))*r2;    ///<squared Error on 1/tan theta coming from the space-point position errors
	data.SP_ITK[Nt]->setDR(std::sqrt(dxy+dz*dz));
	if (m_writeNtuple) data.SP_ITK[Nt]->setDZDR(dZdR);
	data.Tn[Nt].Fl = tz;
	data.Tn[Nt].In = Nt;

	/// if we are exceeding the SP capacity of our data object,
	/// make it resize its vectors. Will add 50 slots by default,
	/// so rarely should happen more than once per event.
	if (++Nt==SPcapacity) {
	  data.resizeSPCont();
	  SPcapacity=data.SP_ITK.size();
	}
      } ///< end of loop over SP within top candidate cell
    } ///< end of loop over top candidate cells

    if (Nt<Ntm) continue;

    /// now continue with the bottom SP search. 
    /// Make the counter start at Nt, as this serves as a running 
    /// index in the SP list for this seed. 
    size_t                 Nb   = Nt;

    /// Bottom links production
    /// Loop over all the cells where we expect to find such SP 
    for (int cell=0; cell<numberBottomCells; ++cell) {

        std::vector<InDet::SiSpacePointForSeedITK*>::iterator iter_otherSP = iter_bottomCands[cell], iter_otherSPend = iter_endBottomCands[cell];

        for(; iter_otherSP!=iter_endBottomCands[cell]; ++iter_otherSP) {
            if( (R - (*iter_otherSP)->radius()) <= m_drmaxPPP) break;
        } 
        iter_bottomCands[cell]=iter_otherSP; 

      /// in each cell, loop over the space points
      for (; iter_otherSP!=iter_endBottomCands[cell]; ++iter_otherSP) {
    
        /// evaluate the radial distance between the central and bottom SP
        const float& Rb =(*iter_otherSP)->radius();
        float dR = R-Rb;

        /// if the points are too close in r, abort (future ones will be even closer). 
        if (dR < m_drminPPP) break;

        const float dz   = Z-(*iter_otherSP)->z();
        const float dZdR = dz/dR;  
        /// Comparison with vertices Z coordinates
        /// straight line extrapolation to r=0 
        const float z0 = Z-R*dZdR;
        if(std::abs(z0) > zmax) continue;

        float dx  = (*iter_otherSP)->x()-X   ;
        float dy  = (*iter_otherSP)->y()-Y   ;
        float x   = dx*ax+dy*ay ;
        float y   = dy*ax-dx*ay ;
        float r2  = 1./(x*x+y*y);
        float u   = x*r2        ;
        float v   = y*r2        ;

        if(std::abs(R*y) > -maxd0cut*x) {
	  float V0; y > 0. ? V0 = VR : V0=-VR;
	  float A   = (v-V0)/(u+1./R);
	  float B   = V0+A/R;
	  if((B*B) > (ipt2K*(1.+A*A))) continue;
        }

        const float dr   = std::sqrt(r2);
        const float tz = dz*dr;
        /// this is effectively a segment-level eta cut - exclude too shallow seed segments
        if(std::abs(tz) > dzdrmax) continue;
        if(m_fastTracking && (*iter_otherSP)->radius() < 50. && std::abs(tz) > 1.5) continue;

        /// add SP to the list
        data.SP_ITK[Nb] = (*iter_otherSP);
        data.R [Nb]   = dr;      ///< inverse distance to central SP
        data.U [Nb]   = u;    ///< transformed U coordinate
        data.V [Nb]   = v;    ///< transformed V coordinate
        data.Er[Nb]   = ((covz0+(*iter_otherSP)->covz())+(tz*tz)*(covr0+(*iter_otherSP)->covr()))*r2;    ///<squared Error on 1/tan theta coming from the space-point position errors
        if (m_writeNtuple) data.SP_ITK[Nb]->setDZDR(dZdR);
        data.Tn[Nb].Fl = tz;
        data.Tn[Nb].In = Nb;

        /// if we are exceeding the SP capacity of our data object,
        /// make it resize its vectors. Will add 50 slots by default,
        /// so rarely should happen more than once per event.
        if (++Nb==SPcapacity) {
          data.resizeSPCont();
          SPcapacity=data.SP_ITK.size();
        }
      } ///< end of loop over SP within top candidate cell
    } ///< end of loop over top candidate cells


    /// if we found no bottom candidates (remember, Nb starts counting at Nt), abort 
    if (!(Nb-Nt)) continue;

    sort(data.Tn,0,Nt);
    sort(data.Tn,Nt,Nb-Nt);

    data.nOneSeeds = 0;
    data.nOneSeedsQ =0;
    data.mapOneSeeds_ITK.clear();
    data.mapOneSeeds_ITKQ.clear();

    /// Three space points comparison
    /// first, loop over the bottom point candidates
    size_t it0 = 0;
    for (size_t ib=Nt; ib<Nb; ++ib) {
      
      if(it0==Nt) break;

      /// retrieve the geometrical paranmeters w.r.t the central SP for this candidate
      float  Tzb  = data.Tn[ib].Fl;       ///< 1/tanTheta estimate from central+bottom SP      
      int    b    = data.Tn[ib].In;       /// bottom seed index after sorting

      float  Rb2r = data.R[b]*covr0;
      float  Rb2z = data.R[b]*covz0;
      float  Erb  = data.Er[b];       ///< this is the uncertainty in 1/tanTheta on the bottom segment resulting from the position errors in the 2 SP
      float  Vb   = data.V [b];       ///< v-coordinate of bottom SP  
      float  Ub   = data.U [b];       ///< u-coordinate of bottom SP
      float  Tzb2 = (1.+Tzb*Tzb);     ///< 1+1/tan²theta - converts transverse to total squared pt
      float sTzb2 = std::sqrt(Tzb2);       ///< sqrt (1+1/tan²theta) - used to convert pt to |p|

      float sigmaSquaredScatteringPtDependent  = Tzb2*COFK;        ///< this, when divided by the 2R², yields an approximated multiple scattering term assuming the measured pt. 
      float sigmaSquaredScatteringMinPt  = Tzb2*ipt2C;       ///< this is an approximate worst case multiple scattering term assuming the lowest 
                                      ///  pt we allow and the estimated theta angle
      /// max IP
      float d0max  =  maxd0cut;

      size_t Nc = 1; if(data.SP_ITK[b]->radius() > m_rmaxPPP) Nc = 0;
      if(data.nOneSeedsQ) ++Nc;      

      /// inner loop over the top point candidates
      for (size_t it=it0;  it<Nt; ++it) {

        int t = data.Tn[it].In; // index of top seed after sorting
        float Tzt = data.Tn[it].Fl;
	
        /// Apply a cut on the compatibility between the r-z slope of the two seed segments. 
        /// This is done by comparing the squared difference between slopes, and comparing 
        /// to the squared uncertainty in this difference - we keep a seed if the difference 
        /// is compatible within the assumed uncertainties. 
        
        /// average value of 1/tan(theta), approximate the slope at the location of the central space point
        float meanOneOverTanThetaSquare = Tzb*Tzt; // SSS uses arithmetic average, PPP geometric average

        /// squared error on the difference in tan(theta) due to space point position errors. 
        float sigmaSquaredSpacePointErrors = Erb+data.Er[t]   /// pre-computed individual squared errors on 1/tan(theta) for the two segments 
                        +2* Rb2z * data.R[t]    /// mixed term with z-uncertainty on central SP
                        +2* Rb2r * data.R[t] * meanOneOverTanThetaSquare; // mixed term with r-uncertainy on central SP

        /// start out by subtracting from the squared difference in 1/tanTheta the space-point-related squared error
        float remainingSquaredDelta  = (Tzb-Tzt)*(Tzb-Tzt) - sigmaSquaredSpacePointErrors; 

        /// First, we test using a generous scattering term calculated assuming the minimum pt we expect
        /// to reconstruct.  
        if (remainingSquaredDelta  - sigmaSquaredScatteringMinPt > 0 ){
	  if(Tzb-Tzt < 0.) break;
	  it0 = it+1;
	  continue;
        }

        /**
        * The following exploits the transformation u:=x/(x²+y²); v:=y/(x²+y²); 
        * This is applied on the x,y coordinates in the frame described above, where the 
        * origin is put in the central SP and the x axis defined to point directly away from the IP.
        * 
        * In this transformed u,v frame, what would be our circle in x-y space takes the form of  
        * a linear function V = (-x0/y0) x U + 1/(2y0) =: A x U + B/2.
        * Here, x0 and y0 describe the center point of the circle in the x-y frame. 
        * As the origin of the x-y frame (the middle space point of our seed) is on the circle, 
        * we have x0²+y0²=R² with circle radius R. 
        * 
        * For our seed, we can experimentally obtain A as the slope of the linear function, 
        * delta V / delta U, 
        * estimated using the delta U and delta V between the top and bottom space point. 
        * 
        * B is then obtained by inserting the obtained A into the 
        * linear equation for the bottom SP, A x U + B/2 = V --> B = 2(V - A x U0 
        * 
        * With x0²+y0²=R², and x0=-A/B and y0=1/B, the radius of the circle is 
        * then obtained as R²=(1+A²)/B². 
        **/ 

        float dU  = data.U[t]-Ub; if(dU == 0.) continue;
        float A   = (data.V[t]-Vb)/dU;
        float onePlusAsquare  = 1.+A*A;
        float B   = Vb-A*Ub;
        float BSquare  = B*B;

        /** With this radius (and pT) estimate, we can apply our pt cut.
         * Reminder, ipt2K is 1 / (K x 0.9 x pt-cut)², where K translates pt into 2R. 
         * So here we can apply the pt cut directly on the (2R)² estimate without
         * the extra overhead of conversion / division.
         * The second check is a refinement of the above Tz compatibility cut,
         * replacing the sigmaSquaredScatteringMinPt scattering contribution which assumes the lowest pt 
         * by one based on the actual estimated pt. 
         * 
         * The second term in this if-statement applies a second version of the 
         * 1/tan(theta) compatibility, this time using a scattering term scaled by the actual measured
         * pt. This refines the cut applied above, following the same logic ("delta² - sigma² ?<=0")
         **/
        if (BSquare  > ipt2K*onePlusAsquare) continue;
        if(remainingSquaredDelta*onePlusAsquare > BSquare*sigmaSquaredScatteringPtDependent){
	  if(Tzb-Tzt < 0.) break;
	  it0 = it;
	  continue;
        }

        /** This is an estimate of the transverse impact parameter.
        * The reasoning is that, in the x-y frame with the central SP as origin and 
        * the x axis pointing away from the IP, we have for the distance between
        * the IP and the middle of the circle: 
        * (x0 - r_central)²+y0² = (R + d0)², 
        * with R being the circle radius and r_central 
        * the radial location of the central SP, placing the IP at IP at (-r_central, 0). 
        * 
        * First simplify using R² =x0²+y0², then apply the approximation d0²/R² ~ 0. 
        * 
        * Finally, consider that locally close to the central SP, the circle is parallel to the x axis, 
        * so A = 0 --> expand (2R)²=(1+A²)/B² around this point to obtain 
        * d0 = r_central x (r_central x B - A). 
        * Note that below, the variable R is the radial coordinate fo the central SP, 
        * corresponding to r_central in the notation above. 
        **/
        float d0 = std::abs((A-B*R)*R);


        /// apply d0 cut to seed
        if (d0 <= d0max) {
          /// evaluate distance the two closest-by SP in this seed candidate
          float dr = data.R[b];
          if (data.R[t] < data.R[b]) dr = data.R[t];
	  /// obtain a quality score - start from the d0 estimate, and add
	  /// a penalty term corresponding to how far the seed segments
	  /// deviate from a straight line in r-z
	  data.SP_ITK[t]->setScorePenalty(std::abs((Tzb-Tzt)/(dr*sTzb2)));
	  data.SP_ITK[t]->setParam(d0);
          if (m_writeNtuple) {
            float meanOneOverTanTheta = std::sqrt(meanOneOverTanThetaSquare);
            if (meanOneOverTanTheta<1e-8) meanOneOverTanTheta = 1e-8;
            if (BSquare<1e-8) BSquare = 1e-8;
            if(data.SP_ITK[t]->z()<0) meanOneOverTanTheta = -meanOneOverTanTheta;
            float theta = std::atan(1./meanOneOverTanTheta);
        
            data.SP_ITK[t]->setEta(-std::log(std::tan(0.5*theta)));
            data.SP_ITK[t]->setPt(std::sqrt(onePlusAsquare/BSquare)/(1000*data.K));
          }
          /// record one possible seed candidate, sort by the curvature
          data.CmSp_ITK.emplace_back(std::make_pair(B/std::sqrt(onePlusAsquare), data.SP_ITK[t]));
          /// store the transverse IP, will later be used as a quality estimator
          if(data.CmSp_ITK.size()==500) break;
        }
   
      }   ///< end loop over top space point candidates
      /// now apply further cleaning on the seed candidates for this central+bottom pair.
      if (data.CmSp_ITK.size() > Nc) {
        newOneSeedWithCurvaturesComparisonPPP(data, data.SP_ITK[b], (*iter_centralSP), Z-R*Tzb);
      }
      data.CmSp_ITK.clear(); /// cleared in newOneSeedWithCurvaturesComparisonPPP but need to also be cleared in case previous conditional statement isn't fulfilled
    } ///< end loop over bottom space points
    ///record seeds found in this run  
    fillSeeds(data);
    nseed += data.fillOneSeeds;

  } ///< end loop over central SP 
}




///////////////////////////////////////////////////////////////////
/// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeedITK* InDet::SiSpacePointsSeedMaker_ITKNew::newSpacePoint
(EventData& data, Trk::SpacePoint*const& sp)
{

  float r[15];
  return newSpacePoint(data,sp,r,true);

}


InDet::SiSpacePointForSeedITK* InDet::SiSpacePointsSeedMaker_ITKNew::newSpacePoint
(EventData& data, Trk::SpacePoint*const& sp, float* r, bool usePixSctInform)
{

  InDet::SiSpacePointForSeedITK* sps = nullptr;

  /// r will store the coordinates of the space point relative 
  /// to the beam spot 
  convertToBeamFrameWork(data, sp, r);

  /// if needed, apply eta criterion 
  if (data.checketa) {
    float z = (std::abs(r[2])+m_zmax); 
    float x = r[0]*data.dzdrmin;
    float y = r[1]*data.dzdrmin;
    if ((z*z )<(x*x+y*y)) return sps;
  }

  if(m_fastTracking){
    float R2 =  r[0]*r[0]+r[1]*r[1];
    if(std::abs(r[2]) > m_dzMaxFast && R2 < m_R2MaxFast) return 0;
    if(std::abs(r[2])-m_zmax > data.dzdrmax*std::sqrt(R2)) return 0;
  }

  if(usePixSctInform){
    if(!sp->clusterList().second) pixInform(sp,r);
    else sctInform(data,sp,r);
  }

  /// If we have previously populated the list and just reset 
  /// the iterator when re-initialising the data object,
  /// then we re-use existing entries 
  if (data.i_spforseed_ITK!=data.l_spforseed_ITK.end()) {
    /// re-use existing entry at the current location 
    sps = &(*data.i_spforseed_ITK++);
    /// and then update the existing entry with the new SP and location.
    /// Unfortunately, set still relies on C-arrays... 
    sps->set(sp,r);
  } else {
    /// otherwise, the list needs to grow
    data.l_spforseed_ITK.emplace_back(InDet::SiSpacePointForSeedITK(sp, &(r[0])));
    /// set our return pointer 
    sps = &(data.l_spforseed_ITK.back());
    /// and make sure to update the iterator 
    data.i_spforseed_ITK = data.l_spforseed_ITK.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::newOneSeedQ(EventData& data,
 InDet::SiSpacePointForSeedITK*& p1, InDet::SiSpacePointForSeedITK*& p2,
 InDet::SiSpacePointForSeedITK*& p3, float z, float seedCandidateQuality) const
{
    /// get the worst seed so far
    float worstQualityInMap = std::numeric_limits<float>::min();
    InDet::SiSpacePointsProSeedITK* worstSeedSoFar = nullptr;

    if (!data.mapOneSeeds_ITKQ.empty()) {
        std::multimap<float,InDet::SiSpacePointsProSeedITK*>::reverse_iterator l = data.mapOneSeeds_ITKQ.rbegin();
        worstQualityInMap = (*l).first;
        worstSeedSoFar = (*l).second;
    }
    /// There are three cases where we simply add our new seed to the list and push it into the map:
    /// a) we have not yet reached our max number of seeds
    if (data.nOneSeedsQ < data.maxSeedsPerSP
        /// b) we have reached the max number but always want to keep confirmed seeds
        /// and the new seed is a confirmed one, with worse quality than the worst one so far
        || (m_useSeedConfirmation && data.keepAllConfirmedSeeds  && worstQualityInMap <= seedCandidateQuality  && isConfirmedSeed(p1,p3,seedCandidateQuality) && data.nOneSeedsQ < data.seedPerSpCapacity)
        /// c) we have reached the max number but always want to keep confirmed seeds
        ///and the new seed of higher quality than the worst one so far, with the latter however being confirmed
        || (m_useSeedConfirmation && data.keepAllConfirmedSeeds  && worstQualityInMap >  seedCandidateQuality  && isConfirmedSeed(worstSeedSoFar->spacepoint0(),worstSeedSoFar->spacepoint2(),worstQualityInMap) && data.nOneSeedsQ < data.seedPerSpCapacity)
    ){
        data.OneSeeds_ITKQ[data.nOneSeedsQ].set(p1,p2,p3,z);
        data.mapOneSeeds_ITKQ.insert(std::make_pair(seedCandidateQuality, &data.OneSeeds_ITKQ[data.nOneSeedsQ]));
        ++data.nOneSeedsQ;
    }
    /// otherwise, we check if there is a poorer-quality seed that we can kick out
    else if (worstQualityInMap > seedCandidateQuality){
        /// Overwrite the parameters of the worst seed with the new one
        worstSeedSoFar->set(p1,p2,p3,z);
        /// re-insert it with its proper quality to make sure it ends up in the right place
        std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator i = data.mapOneSeeds_ITKQ.insert(std::make_pair(seedCandidateQuality,worstSeedSoFar));
        /// and remove the entry with the old quality to avoid duplicates
        for (++i; i!=data.mapOneSeeds_ITKQ.end(); ++i) {
            if ((*i).second==worstSeedSoFar) {
                data.mapOneSeeds_ITKQ.erase(i);
                return;
            }
        }
    }
}

void InDet::SiSpacePointsSeedMaker_ITKNew::newOneSeed(EventData& data,
 InDet::SiSpacePointForSeedITK*& p1, InDet::SiSpacePointForSeedITK*& p2,
 InDet::SiSpacePointForSeedITK*& p3, float z, float seedCandidateQuality) const
{
    /// get the worst seed so far
    float worstQualityInMap = std::numeric_limits<float>::min();
    InDet::SiSpacePointsProSeedITK* worstSeedSoFar = nullptr;

    if (!data.mapOneSeeds_ITK.empty()) {
        std::multimap<float,InDet::SiSpacePointsProSeedITK*>::reverse_iterator l = data.mapOneSeeds_ITK.rbegin();
        worstQualityInMap = (*l).first;
        worstSeedSoFar = (*l).second;
    }
    /// There are three cases where we simply add our new seed to the list and push it into the map:
    /// a) we have not yet reached our max number of seeds
    if (data.nOneSeeds < data.maxSeedsPerSP
    /// b) we have reached the max number but always want to keep confirmed seeds
    /// and the new seed is a confirmed one, with worse quality than the worst one so far
        || (m_useSeedConfirmation && data.keepAllConfirmedSeeds  && worstQualityInMap <= seedCandidateQuality  && isConfirmedSeed(p1,p3,seedCandidateQuality) && data.nOneSeeds < data.seedPerSpCapacity)
    /// c) we have reached the max number but always want to keep confirmed seeds
    ///and the new seed of higher quality than the worst one so far, with the latter however being confirmed
        || (m_useSeedConfirmation && data.keepAllConfirmedSeeds  && worstQualityInMap >  seedCandidateQuality  && isConfirmedSeed(worstSeedSoFar->spacepoint0(),worstSeedSoFar->spacepoint2(),worstQualityInMap) && data.nOneSeeds < data.seedPerSpCapacity)
    ){
        data.OneSeeds_ITK[data.nOneSeeds].set(p1,p2,p3,z);
        data.mapOneSeeds_ITK.insert(std::make_pair(seedCandidateQuality, &data.OneSeeds_ITK[data.nOneSeeds]));
        ++data.nOneSeeds;
    }
    /// otherwise, we check if there is a poorer-quality seed that we can kick out
    else if (worstQualityInMap > seedCandidateQuality){
        /// Overwrite the parameters of the worst seed with the new one
        worstSeedSoFar->set(p1,p2,p3,z);
        /// re-insert it with its proper quality to make sure it ends up in the right place
        std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator i = data.mapOneSeeds_ITK.insert(std::make_pair(seedCandidateQuality,worstSeedSoFar));
        /// and remove the entry with the old quality to avoid duplicates
        for (++i; i!=data.mapOneSeeds_ITK.end(); ++i) {
            if ((*i).second==worstSeedSoFar) {
                data.mapOneSeeds_ITK.erase(i);
                return;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::newOneSeedWithCurvaturesComparisonSSS
(EventData& data, SiSpacePointForSeedITK*& SPb, SiSpacePointForSeedITK*& SP0, float Zob) const
{

  if(m_useSeedConfirmation){
    newOneSeedWithCurvaturesComparisonSeedConfirmation(data,SPb,SP0,Zob);
  }
    
  else{
    
    static const float curvatureInterval = .00003;
    
    /// sort common SP by curvature
    if(data.CmSp_ITK.size() > 2) std::sort(data.CmSp_ITK.begin(), data.CmSp_ITK.end(), comCurvatureITK());
    
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_otherSP;
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_commonTopSP = data.CmSp_ITK.begin(), ie = data.CmSp_ITK.end();
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_startInnerLoop=it_commonTopSP;
    
    float Lt[4];
    
    /// check all possible common top SP
    for (; it_commonTopSP!=ie; ++it_commonTopSP) {
      
      SiSpacePointForSeedITK* SPt = (*it_commonTopSP).second;
      int NT     = 1              ;
      Lt  [0]    = SPt->dR();
      float seedIP   = SPt->param();

      /// form a curvature interval cut
      float               minCurvature  =(*it_commonTopSP).first-curvatureInterval;
      float               maxCurvature  =(*it_commonTopSP).first+curvatureInterval;

      /**
       * Now we look at the other SP candidates and try to find a confirmation seed,
       * including the same centre/lower SP and giving a compatible curvature,
       * but with the top SP in a different layer
       **/

      for (it_otherSP=it_startInnerLoop;  it_otherSP!=ie; ++it_otherSP) {
	/// if we are looking at the same SP, skip it
	if (       it_otherSP == it_commonTopSP           ) continue;
	/// if we have a lower curvature than the minimum, skip - and remember to
	/// not bother with this candidate again later, as the vectors are curvature-sorted
	if ( (*it_otherSP).first < minCurvature       ) {
	  it_startInnerLoop=it_otherSP;
	  ++it_startInnerLoop;
	  continue;
	}
	/// abort once the the curvature gets too large
	if ( (*it_otherSP).first > maxCurvature       ) break;
        
	float L = (*it_otherSP).second->dR();
        
	int k = 0;
	for(; k!=NT; ++k) {
	  if(std::abs(L-Lt[k]) < 20.) break;
	}
	if( k==NT ) {
	  Lt[NT]= L;
	  if(++NT==4) break;
	}
      }
      
      // ITK seed quality used so far
      float Q = seedIP-float(NT)*100.; if(NT > 2) Q-=100000.;
      /// this is a good seed, save it (unless we have too many seeds per SP)
      newOneSeed(data, SPb, SP0, SPt, Zob, Q);
    } ///< end of loop over top SP candidates
    data.CmSp_ITK.clear();
  }
}


void InDet::SiSpacePointsSeedMaker_ITKNew::newOneSeedWithCurvaturesComparisonPPP
(EventData& data, SiSpacePointForSeedITK*& SPb, SiSpacePointForSeedITK*& SP0, float Zob) const
{

  if(m_useSeedConfirmation){
    newOneSeedWithCurvaturesComparisonSeedConfirmation(data,SPb,SP0,Zob);
  }
    
  else{
    
    static const float curvatureInterval = .00003;
    
    /// sort common SP by curvature
    if(data.CmSp_ITK.size() > 2) std::sort(data.CmSp_ITK.begin(), data.CmSp_ITK.end(), comCurvatureITK());
    
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_otherSP;
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_commonTopSP = data.CmSp_ITK.begin(), ie = data.CmSp_ITK.end();
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_startInnerLoop=it_commonTopSP;
    
    float Lt[4];
    
    float Qmin = 1.e20;
    float Rb   = 2.*SPb->radius();
    int   NTc(2); if(Rb > 280.) NTc = 1;
    SiSpacePointForSeedITK* SPmin = nullptr;
    bool  Qm   = Rb < 120. || std::abs(Zob) > 150.;
    
    /// check all possible common top SP
    for (; it_commonTopSP!=ie; ++it_commonTopSP) {

      SiSpacePointForSeedITK* SPt = (*it_commonTopSP).second;
      int NT     = 1              ;
      Lt  [0]    = SPt->dR();
      float seedIP = SPt->param();
      
      /// form a curvature interval cut
      float               minCurvature  =(*it_commonTopSP).first-curvatureInterval;
      float               maxCurvature  =(*it_commonTopSP).first+curvatureInterval;

      /**
       * Now we look at the other SP candidates and try to find a confirmation seed,
       * including the same centre/lower SP and giving a compatible curvature,
       * but with the top SP in a different layer
       **/

      for (it_otherSP=it_startInnerLoop;  it_otherSP!=ie; ++it_otherSP) {
	    /// if we are looking at the same SP, skip it
	    if (       it_otherSP == it_commonTopSP           ) continue;
	    /// if we have a lower curvature than the minimum, skip - and remember to
	    /// not bother with this candidate again later, as the vectors are curvature-sorted
	    if ( (*it_otherSP).first < minCurvature       ) {
	      it_startInnerLoop=it_otherSP;
	      ++it_startInnerLoop;
	      continue;
	    }
	    /// abort once the the curvature gets too large
	    if ( (*it_otherSP).first > maxCurvature       ) break;
	
	    float L = (*it_otherSP).second->dR();
	
	    int k = 0;
	    for(; k!=NT; ++k) {
	      if(std::abs(L-Lt[k]) < 20.) break;
	    }
	    if( k==NT ) {
	      Lt[NT]= L;
	      if(++NT==4) break;
	    }
      }
      
      int dN = NT-NTc;
      if(dN < 0 || (data.nOneSeedsQ && !dN)) continue;
      if(Qm && !dN && seedIP > 1.) continue;

      // ITK seed quality used so far
      float Q = 100.*seedIP+(std::abs(Zob)-float(NT)*100.);
      if(Q > SPb->quality() && Q > SP0->quality() && Q > SPt->quality()) continue;
      if ( dN )  newOneSeedQ(data,SPb,SP0,SPt,Zob,Q);
      else if(Q  < Qmin) {Qmin=Q; SPmin=SPt;}            
    } ///< end of loop over top SP candidates
    if(SPmin && !data.nOneSeedsQ) newOneSeed(data,SPb,SP0,SPmin,Zob,Qmin);
    data.CmSp_ITK.clear();
  }
}




void InDet::SiSpacePointsSeedMaker_ITKNew::newOneSeedWithCurvaturesComparisonSeedConfirmation
(EventData& data, SiSpacePointForSeedITK*& SPb, SiSpacePointForSeedITK*& SP0, float Zob) const
{
  static const float curvatureInterval = .00003;
  bool  bottomSPisPixel = !SPb->spacepoint->clusterList().second;
  float bottomSPQuality   = SPb->quality();
  float centralSPQuality   = SP0->quality();
    
  /// sort common SP by curvature
  if(data.CmSp_ITK.size() > 2) std::sort(data.CmSp_ITK.begin(), data.CmSp_ITK.end(), comCurvatureITK());
      
  float bottomR=SPb->radius();
  float bottomZ=SPb->z();

  std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_otherSP;
  std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_commonTopSP = data.CmSp_ITK.begin(), ie = data.CmSp_ITK.end();
  std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator it_startInnerLoop=it_commonTopSP;

  /// check all possible common top SP
  for (; it_commonTopSP!=ie; ++it_commonTopSP) {

    SiSpacePointForSeedITK* SPt = (*it_commonTopSP).second;
    /// the seed quality is set to d0 initially
    float seedIP = SPt->param();
    float seedQuality = seedIP + SPt->scorePenalty();
    float originalSeedQuality = seedQuality;

    if(m_maxdImpact > 50){      //This only applies to LRT

      float topR=SPt->radius();
      float topZ=SPt->z();

      float Zot = std::abs(topR - bottomR) > 10e-9 ?
	bottomZ - (bottomR - originalSeedQuality) * ((topZ - bottomZ) / (topR - bottomR)) : bottomZ;

      float theta1 = std::abs(topR - bottomR) > 10e-9 ?
	std::atan2(topR - bottomR, topZ - bottomZ) : 0.;
      float eta1 = theta1 > 0 ? -std::log(std::tan(.5 * theta1)) : 0.;
      
      float theta0 = seedIP > 0 ? std::atan2(seedIP, Zot) : 0;
      float eta0 = theta0 > 0 ? -std::log(std::tan(.5 * theta0)) : 0.;
      
      float deltaEta=std::abs(eta1-eta0); //For LLP daughters, the direction of the track is correlated with the direction of the LLP (which is correlated with the direction of the point of closest approach
      //calculate weighted average of d0 and deltaEta, normalized by their maximum values
      float f=std::min(0.5,originalSeedQuality/200.);  //0.5 and 200 are parameters chosen from a grid scan to optimize efficiency
      seedQuality*=(1-f)/300.;
      seedQuality+=f*deltaEta/2.5;
    }
    
    bool                topSPisPixel = !SPt->spacepoint->clusterList().second;
        
    /// check the surface the hit is on
    const Trk::Surface* surfaceTopSP  = SPt->sur   ();
    float               radiusTopSP   = SPt->radius();
    /// form a curvature interval cut
    float               minCurvature  =(*it_commonTopSP).first-curvatureInterval;
    float               maxCurvature  =(*it_commonTopSP).first+curvatureInterval;
        
    /** Note: The score modifiers used here have the purpose of separating the candidates into
    *        classes / groups disjoint from each other.
    *        So the score increment (200 by default) should exceed the maximum
    *        |d0| (base score) we expect to encounter to avoid overlap.
    *        For LRT, we may want to tune this!
    **/

    /// if we have a SSS seed, boost the quality score by 400
    if      (!bottomSPisPixel) seedQuality+=m_seedScoreBonusSSS;
    /// if we have a PPP, boost the quality by 200
    else if ( topSPisPixel) seedQuality+=m_seedScoreBonusPPP;

    /**
    * Now we look at the other SP candidates and try to find a confirmation seed,
    * including the same centre/lower SP and giving a compatible curvature,
    * but with the top SP in a different layer
    **/

    for (it_otherSP=it_startInnerLoop;  it_otherSP!=ie; ++it_otherSP) {
      /// if we are looking at the same SP, skip it
      if (       it_otherSP == it_commonTopSP           ) continue;
      /// if we have a lower curvature than the minimum, skip - and remember to
      /// not bother with this candidate again later, as the vectors are curvature-sorted
      if ( (*it_otherSP).first < minCurvature       ) {
	it_startInnerLoop=it_otherSP;
	++it_startInnerLoop;
	continue;
      }
      /// abort once the the curvature gets too large
      if ( (*it_otherSP).first > maxCurvature       ) break;
      /// if both SP are on the surface, skip it
      if ( (*it_otherSP).second->sur()==surfaceTopSP) continue;
      /// if the other SP is too close to the current top one, skip
      float radiusOtherSP = (*it_otherSP).second->radius();
      if (std::abs(radiusOtherSP-radiusTopSP) < m_drminSeedConf) continue;
      // if we have a confirmation seed, we improve the score of the seed.
      seedQuality += m_seedScoreBonusConfirmationSeed;
      // apply confirmation bonus only once
      break;
    }

    /// kick this seed candidate if the score is too high (lower values = better)
    if (seedQuality > data.maxScore) continue;
        
    /// if we have PPS seeds and no confirmation SP exists (which would give the -200 bonus)
    /// or the hits on this seed were already used on a higher quality PPP/SSS seed, kick this one
    if (bottomSPisPixel!=topSPisPixel) {
      if (seedQuality > 0. ||
	  (seedQuality > bottomSPQuality && seedQuality > centralSPQuality && seedQuality > SPt->quality())
	  ) continue;
    }
    /// If we have a non-confirmed seed, apply a stricter d0 cut.
    /// This, is determined using the original cut and the score penalty modifier.
    if (!isConfirmedSeed(SPb,SPt,seedQuality)){
        /// PPP seeds
      double                maxdImpact = m_maxdImpact    - (m_dImpactCutSlopeUnconfirmedPPP * SPt->scorePenalty());
      /// SSS seeds
      if (!bottomSPisPixel) maxdImpact = m_maxdImpactSSS - (m_dImpactCutSlopeUnconfirmedSSS * SPt->scorePenalty());
      if (seedIP > maxdImpact) continue;
    }
    /// this is a good seed, save it (unless we have too many seeds per SP)
    newOneSeed(data, SPb, SP0, SPt, Zob, seedQuality);
  } ///< end of loop over top SP candidates
  data.CmSp_ITK.clear();
}



///////////////////////////////////////////////////////////////////
/// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::fillSeeds(EventData& data) const
{

  data.fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator it_firstSeedCandidate = data.mapOneSeeds_ITK.begin();
  std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator it_seedCandidate  = data.mapOneSeeds_ITK.begin();
  std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator it_endSeedCandidates = data.mapOneSeeds_ITK.end();
    if (data.nOneSeedsQ){
      it_firstSeedCandidate = data.mapOneSeeds_ITKQ.begin();
      it_seedCandidate  = data.mapOneSeeds_ITKQ.begin();
      it_endSeedCandidates = data.mapOneSeeds_ITKQ.end();
    }
  /// no seeds - nothing to do. 
  if (it_seedCandidate==it_endSeedCandidates) return;

  SiSpacePointsProSeedITK* theSeed{nullptr};

  /// loop over the seed candidates we have stored in the event data
  for (; it_seedCandidate!=it_endSeedCandidates; ++it_seedCandidate) {

    /// quality score of the seed, lower = better, list is sorted by quality 
    float quality = (*it_seedCandidate).first;
    theSeed       = (*it_seedCandidate).second;

    /// this will set the quality member of all points on the seed to the quality score of this candidate
    if (!theSeed->setQuality(quality)) continue;

    /// if we have space, write the seed directly into an existing slot 
    if (data.i_seede_ITK!=data.l_seeds_ITK.end()) {
      theSeed  = &(*data.i_seede_ITK++);
      *theSeed = *(*it_seedCandidate).second;
    } else {
      /// otherwise, extend the seed list and update the iterators 
      data.l_seeds_ITK.emplace_back(SiSpacePointsProSeedITK(*(*it_seedCandidate).second));
      theSeed = &(data.l_seeds_ITK.back());
      data.i_seede_ITK = data.l_seeds_ITK.end();
    }

    ++data.fillOneSeeds;
  } ///< end loop over seed candidates 

}



const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_ITKNew::next(EventData& data)
{
  /// This only holds if we call next() without manually calling newEvent/find3Sp
  if (not data.initialized) initializeEventData(data);

  if (data.nspoint==3) {
    do {
      /// If we are out of seeds, call findNext to see if we can find more.
      if (data.i_seed_ITK==data.i_seede_ITK) {
        /// findNext will call production3Sp again IF data.endlist is false,
        /// which is only the case if the last run of production3Sp did not run to the end
        /// or if we did not run seed finding before
        /// For run-3 offline, this will not do anything.
        findNext(data);
        /// if no new seeds were found, exit
        if (data.i_seed_ITK==data.i_seede_ITK) return nullptr;
      }
      /// iterate until we find a valid seed satisfying certain quality cuts in set3
    } while (!(*data.i_seed_ITK++).set3(data.seedOutput));
    /// then return this next seed candidate
    return &data.seedOutput;
  } else {
    /// same as above for 2SP
    if (data.i_seed_ITK==data.i_seede_ITK) {
      findNext(data);
      if (data.i_seed_ITK==data.i_seede_ITK) return nullptr;
    }
    (*data.i_seed_ITK++).set2(data.seedOutput);
    return &data.seedOutput;
  }
  return nullptr;
}




void InDet::SiSpacePointsSeedMaker_ITKNew::initializeEventData(EventData& data) const {
  int seedArrayPerSPSize = (m_maxOneSizePPP>m_maxOneSizeSSS ? m_maxOneSizePPP : m_maxOneSizeSSS); 
  if (m_alwaysKeepConfirmedStripSeeds || m_alwaysKeepConfirmedPixelSeeds)  seedArrayPerSPSize = 50; 
  data.initialize(EventData::ITK,
                  m_maxsizeSP,
                  seedArrayPerSPSize,
                  0, /// maxsize not used
                  m_nBinsR,
                  0, /// sizeRF not used
                  arraySizePhiZ,
                  arraySizePhiZV,
                  m_checketa);
}

bool InDet::SiSpacePointsSeedMaker_ITKNew::isConfirmedSeed(const InDet::SiSpacePointForSeedITK* bottomSP, const InDet::SiSpacePointForSeedITK* topSP, float quality) const{

    /// SSS seeds
    if (bottomSP->spacepoint->clusterList().second){
      return (quality < m_seedScoreThresholdSSSConfirmationSeed);
    }
    /// PPP seeds
    else if (!topSP->spacepoint->clusterList().second){
      return (quality < m_seedScoreThresholdPPPConfirmationSeed);
    }
    /// PPS: the confirmation is the only quality modifier applied
    else return (quality < 0.);
}


///////////////////////////////////////////////////////////////////
// Pixels information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::pixInform(Trk::SpacePoint*const& sp,float* r)
{
  const InDet::SiCluster*           cl = static_cast<const InDet::SiCluster*>(sp->clusterList().first); 
  const InDetDD::SiDetectorElement* de = cl->detectorElement(); 
  const Amg::Transform3D& Tp = de->surface().transform();
  r[3] = float(Tp(0,2));
  r[4] = float(Tp(1,2));
  r[5] = float(Tp(2,2)); 
}

///////////////////////////////////////////////////////////////////
// SCT information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::sctInform(EventData& data,Trk::SpacePoint*const& sp,float* r) 
{
  const InDet::SiCluster* c0 = static_cast<const InDet::SiCluster*>(sp->clusterList().first );
  const InDet::SiCluster* c1 = static_cast<const InDet::SiCluster*>(sp->clusterList().second);
  const InDetDD::SiDetectorElement* d0 = c0->detectorElement(); 
  const InDetDD::SiDetectorElement* d1 = c1->detectorElement(); 

  Amg::Vector2D lc0 = c0->localPosition();  
  Amg::Vector2D lc1 = c1->localPosition();  
  
  std::pair<Amg::Vector3D, Amg::Vector3D > e0 =
    (d0->endsOfStrip(InDetDD::SiLocalPosition(lc0.y(),lc0.x(),0.))); 
  std::pair<Amg::Vector3D, Amg::Vector3D > e1 =
    (d1->endsOfStrip(InDetDD::SiLocalPosition(lc1.y(),lc1.x(),0.))); 

  Amg::Vector3D s0(.5*(e0.first+e0.second));
  Amg::Vector3D s1(.5*(e1.first+e1.second));

  Amg::Vector3D b0 (.5*(e0.second-e0.first));
  Amg::Vector3D b1 (.5*(e1.second-e1.first));
  Amg::Vector3D d02(s0 -s1);

  // b0
  r[ 3] = float(b0[0]);
  r[ 4] = float(b0[1]);
  r[ 5] = float(b0[2]);
  
  // b1
  r[ 6] = float(b1[0]);
  r[ 7] = float(b1[1]);
  r[ 8] = float(b1[2]);

  // r0-r2
  r[ 9] = float(d02[0]);
  r[10] = float(d02[1]);
  r[11] = float(d02[2]);

  // r0
  r[12] = float(s0[0])-data.xbeam[0];
  r[13] = float(s0[1])-data.ybeam[0];
  r[14] = float(s0[2])-data.zbeam[0];
}




///////////////////////////////////////////////////////////////////
// Quick sort with recursive implementation
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITKNew::QuickSort(std::vector<FloatInt>& A, int start, int size)
{
  if(size <= 50) SmallSort(A,start,size);
  else {
    int m(Partition(A,start,size));
    if(size-m > 0) QuickSort( A, start+m, size-m);
    if(m-2 > 0) QuickSort( A, start, m-2);
  }
}

void InDet::SiSpacePointsSeedMaker_ITKNew::SmallSort(std::vector<FloatInt>& A, int start, int size)
{
  FloatInt A0 = A[start];
  if(A0.Fl > A[start+1].Fl) {A[start]=A[start+1]; A[start+1]=A0; A0=A[start];}
  A[start].Fl = -1.e30;

  for(int i(start+1); i<start+size; ++i) {

    FloatInt U = A[i+1];
    int j(i);
    while(A[j].Fl > U.Fl) {A[j+1]=A[j]; --j;}
    A[j+1]=U;
  }
  int i(start+1);
  while (A[i].Fl < A0.Fl) {A[i-1]=A[i]; ++i;}
  A[i-1]=A0;
}

int InDet::SiSpacePointsSeedMaker_ITKNew::Partition(std::vector<FloatInt>& A, int start, int size)
{
  Middle(A,start,size);
  float Fn(A[start+size].Fl);
  int    m(start);
  int    i(start);
  while(true) {
    if(A[i].Fl <= Fn) {
      FloatInt Tm=A[i]; A[i]=A[m]; A[m++]=Tm; if(i==(start+size)) break;
    }
    ++i;
  }
  return m;
}

void InDet::SiSpacePointsSeedMaker_ITKNew::Middle(std::vector<FloatInt>& A, int start, int size)
{
  int m(start + (size>>1));
  int n(start + size);
    
  FloatInt A0 = A[start];  // 0
  FloatInt Am = A[m];  // n/2
  FloatInt An = A[n];  // n

  bool Q01 = A0.Fl < Am.Fl;
  bool Q02 = A0.Fl < An.Fl;
  bool Q12 = Am.Fl < An.Fl;

  if(Q01) {                           // Ao < Am
    if     ( Q12) {A[n]=Am; A[m]=An;} // Ao < Am < An
    else if(!Q02) {A[n]=A0; A[start]=An;} // Am > Ao > An
  }                                   // Ao > Am
  else   if( Q02) {A[n]=A0; A[0]=An;} // Am < Ao < An
  else   if(!Q12) {A[n]=Am; A[m]=An;} // An < Am < Ao
}


bool InDet::SiSpacePointsSeedMaker_ITKNew::getWriteNtupleBoolProperty() const{
  return m_writeNtuple;
}

///////////////////////////////////////////////////////////////////////////////////
/// Method to write ntuple entries for an input seed
///////////////////////////////////////////////////////////////////////////////////
void InDet::SiSpacePointsSeedMaker_ITKNew::writeNtuple(const SiSpacePointsSeed* seed,
						       const Trk::Track* track, int seedType,
						       long eventNumber) const{

  if(m_writeNtuple) {
    if(track != nullptr) {
      m_trackPt = (track->trackParameters()->front()->pT())/1000.;
      m_trackEta = std::abs(track->trackParameters()->front()->eta());
    } else {
      m_trackPt = -1.;
      m_trackEta = -1.;
    }
    m_d0           =   seed->d0();
    m_z0           =   seed->zVertex();
    m_eta          =   seed->eta();
    m_x1           =   seed->x1();
    m_x2           =   seed->x2();
    m_x3           =   seed->x3();
    m_y1           =   seed->y1();
    m_y2           =   seed->y2();
    m_y3           =   seed->y3();
    m_z1           =   seed->z1();
    m_z2           =   seed->z2();
    m_z3           =   seed->z3();
    m_r1           =   seed->r1();
    m_r2           =   seed->r2();
    m_r3           =   seed->r3();
    m_type         =   seedType;
    m_dzdr_b       =   seed->dzdr_b();
    m_dzdr_t       =   seed->dzdr_t();
    m_pt           =   seed->pt();
    m_givesTrack   =   !(track == nullptr);
    m_eventNumber  =   eventNumber;

    m_outputTree->Fill();
  }
}
