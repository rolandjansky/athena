/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

     
///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ATLxk
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ATLxk.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
/// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ATLxk::SiSpacePointsSeedMaker_ATLxk
(const std::string& t,const std::string& n,const IInterface* p)
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
/// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ATLxk::initialize()
{
  StatusCode sc = AlgTool::initialize();

  ATH_CHECK(m_spacepointsPixel.initialize(m_pixel));
  ATH_CHECK(m_spacepointsSCT.initialize(m_sct));
  ATH_CHECK(m_spacepointsOverlap.initialize(m_useOverlap));

  /// Get beam geometry
  if (not m_beamSpotKey.empty()) {
    ATH_CHECK(m_beamSpotKey.initialize());
  }

  ATH_CHECK( m_fieldCondObjInputKey.initialize() );

  /// PRD-to-track association (optional)
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty()));
  
  /// Build framework. Will set up internal variables 
  /// *not* related to the event data object 
  buildFrameWork();

  if (msgLvl(MSG::DEBUG)) { 
    EventData data;
    initializeEventData(data);
    data.nprint=0;
    dump(data, msg(MSG::DEBUG));
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
/// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ATLxk::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
/// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newEvent(const EventContext& ctx, EventData& data, int iteration) const
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
    if (not m_beamSpotKey.empty()) {
      buildBeamFrameWork(data);
    }

    /// Read the field information 
    double magField[3]{0,0,0};
    double globalPos[3] ={10.,10.,0.};

    MagField::AtlasFieldCache    fieldCache;
    /// Get field cache object
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("SiSpacePointsSeedMaker_ATLxk: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return;
    }
    fieldCondObj->getInitializedCache (fieldCache);

    if (fieldCache.solenoidOn()) {
      /// retrieve field
      fieldCache.getFieldZR(globalPos,magField);
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
    data.i_spforseed = data.l_spforseed.begin();
  } ///< end if-statement for iteration 0 
  else {  /// for the second iteration (PPP pass), don't redo the full init required the first time 
    data.r_first = 0;     ///< reset the first radial bin 
    /// call fillLists to repopulate the candidate space points and exit 
    fillLists(data);
    return;
  }

  /// the following will only happen in the first iteration 

  data.checketa = data.dzdrmin > 1.;
  if (m_dbm) data.checketa = false;

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
    data.r_Sorted[n].clear();
  }
  data.ns = data.nr = 0;

  /// read the prd to track map, in case we want to use it. 
  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }

  /////////////////////////////
  /// Now, we will populate the space point list in the event data object once. 
  /////////////////////////////

  /// reset the first r index 
  data.r_first = 0;

  /// Get pixels space points containers from store gate 
  if (!m_dbm && m_pixel) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {
      /// loop over the pixel space points 
      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {
          
          /// if we use the PRD to track map and this SP has already been used in a track, bail out 
          /// also skip any SP outside the r binning 
          if ((prd_to_track_map_cptr &&  isUsed(sp,*prd_to_track_map_cptr)) || sp->r() > m_r_rmax) continue;

          /// Remove DBM space points
          ///
          const InDetDD::SiDetectorElement* de= 
            static_cast<const InDetDD::SiDetectorElement*>(sp->clusterList().first->detectorElement());
          if (!de || de->isDBM()) continue;

          /** create a SiSpacePointForSeed from the space point. 
          * This will also add the point to the l_spforseed list and update 
          * the i_spforseed iterator
          **/ 
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          /// this can occur if we fail the eta cut 
          if (!sps) continue;

          /// determine the r-bin of this SP. 
          /// done by dividing the radius by the bin size. 
          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          /// catch outliers
          if (radiusBin>maxBinR) radiusBin = maxBinR;

          /// now add the SP to the r-binned vector 
          data.r_Sorted[radiusBin].push_back(sps);
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
  if (!m_dbm && m_sct) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsSCT) {
        for (const Trk::SpacePoint* sp: *spc) {
          /// as for the pixel, veto already used SP if we are using the PRD to track map in later passes of track finding. 
          /// Also, veto SP outside the maximum radius 
          if ((prd_to_track_map_cptr &&  isUsed(sp,*prd_to_track_map_cptr)) || sp->r() > m_r_rmax) continue;

          /// create a space point and write it into the data object's list of points 
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          /// as for PIX, determine the radial bin.
          /// Note that for the SCT we do not update data.r_first. 
          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (radiusBin>maxBinR) radiusBin = maxBinR;
          /// again store the SP in the r-binned vectors 
          data.r_Sorted[radiusBin].push_back(sps);
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

      SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap{m_spacepointsOverlap, ctx};
      if (spacepointsOverlap.isValid()) {
  
        for (const Trk::SpacePoint* sp: *spacepointsOverlap) {
          /// usual rejection of SP used in previous track finding passes if we run with the PRT to track map + check of the max radius 
          if ((prd_to_track_map_cptr &&  isUsed(sp, *prd_to_track_map_cptr)) || sp->r() > m_r_rmax) continue;

          /// SP creation, entry into list of the data object 
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          /// radial bin determination 
          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (radiusBin>maxBinR) radiusBin = maxBinR;
          /// insert into the "histogram" vector
          data.r_Sorted[radiusBin].push_back(sps);
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
  
  /// Get pixels space points containers from store gate for DBM reconstruction
  if (m_dbm) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {

          /// Keep only DBM space points
          const InDetDD::SiDetectorElement* de= 
            static_cast<const InDetDD::SiDetectorElement*>(sp->clusterList().first->detectorElement());
          if (!de || !de->isDBM() || sp->r() > m_r_rmax) continue;

          /// the following logic follows what is done above for the other species, 
          /// please see there for detailed explanation 
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;


          int radiusBin = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (radiusBin>maxBinR) radiusBin = maxBinR;
          data.r_Sorted[radiusBin].push_back(sps);
          ++data.r_map[radiusBin];
          if (data.r_map[radiusBin]==1) data.r_index[data.nr++] = radiusBin;
          if (radiusBin > data.r_first) data.r_first = radiusBin;
          ++data.ns;
        }
      }
    }
    ++data.r_first;
  }
  /// negative iterations are not used in the current run-3 reco
  if (iteration < 0) data.r_first = 0;

  /// populates the phi-z sorted histograms using the spacepoint lists and r-binning.
  /// after this call, we have a 3D binning
  fillLists(data);
}

///////////////////////////////////////////////////////////////////
/// Initialize tool for new region
///////////////////////////////////////////////////////////////////

/// not used in offline reco (trigger specific)
void InDet::SiSpacePointsSeedMaker_ATLxk::newRegion
(const EventContext& ctx, EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const
{
  if (not data.initialized) initializeEventData(data);

  data.iteration  = 0;
  data.trigger = false;
  erase(data);
  if (!m_pixel && !m_sct) return;

  data.dzdrmin = m_dzdrmin0;
  data.dzdrmax = m_dzdrmax0;
  data.maxScore = m_maxScore;

  if (not m_beamSpotKey.empty()) {
    buildBeamFrameWork(data);
  }

  double magField[3]{0,0,0};
  double globalPos[3] ={10.,10.,0.};

  MagField::AtlasFieldCache    fieldCache;
  // Get field cache object
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("SiSpacePointsSeedMaker_ATLxk: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return;
  }
  fieldCondObj->getInitializedCache (fieldCache);

  // initialise pt cuts and conversion factors 
  if (fieldCache.solenoidOn()) {
    fieldCache.getFieldZR(globalPos,magField);

    data.K = 2./(300.*magField[2]);
  } else {
    data.K = 2./(300.* 5. );
  }

  data.ipt2K = m_ipt2/(data.K*data.K);
  data.ipt2C = m_ipt2*m_COF;
  data.COFK = m_COF*(data.K*data.K);

  data.i_spforseed = data.l_spforseed.begin();

  float oneOverBinSizeR = 1./m_binSizeR;
  int   maxBinR  = m_nBinsR-1;

  data.r_first = 0;
  data.checketa = false;

  for (int i=0; i<data.nr; ++i) {
    int n = data.r_index[i];
    data.r_map[n] = 0;
    data.r_Sorted[n].clear();
  }
  data.ns = data.nr = 0;

  // Get pixels space points containers from store gate 
  //
  if (m_pixel && !vPixel.empty()) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if ( spacepointsPixel.isValid() ) {

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vPixel) {
        auto w = spacepointsPixel->indexFindPtr(l);
        if (w==nullptr) continue;
        for (const Trk::SpacePoint* sp: *w) {
          float r = sp->r();
          if (r > m_r_rmax) continue;
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          int ir = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (ir>maxBinR) ir = maxBinR;
          data.r_Sorted[ir].push_back(sps);
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

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vSCT) {
        auto w = spacepointsSCT->indexFindPtr(l);
        if (w==nullptr) continue;
        for (const Trk::SpacePoint* sp: *w) {
          float r = sp->r();
          if (r > m_r_rmax) continue;
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          int ir = static_cast<int>(sps->radius()*oneOverBinSizeR);
          if (ir>maxBinR) ir = maxBinR;
          data.r_Sorted[ir].push_back(sps);
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

void InDet::SiSpacePointsSeedMaker_ATLxk::newRegion
(const EventContext& ctx, EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
 const IRoiDescriptor& IRD) const
{
  constexpr float twoPi = 2.*M_PI;

  if (not data.initialized) initializeEventData(data);

  newRegion(ctx, data, vPixel, vSCT);
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
/// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find2Sp(EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  data.zminU = m_zmin;
  data.zmaxU = m_zmax;

  int mode = 0;
  if (lv.begin()!=lv.end()) mode = 1;
  bool newv = newVertices(data, lv);
  
  if (newv || !data.state || data.nspoint!=2 || data.mode!=mode || data.nlist) {
    data.i_seede_Pro = data.l_seeds_Pro.begin();
    data.state = 1;
    data.nspoint = 2;
    data.nlist = 0;
    data.mode = mode;
    data.endlist = true;
    data.fvNmin = 0;
    data.fNmin = 0;
    data.zMin = 0;
    production2Sp(data);
  }
  data.i_seed_Pro = data.l_seeds_Pro.begin();
  

  if (msgLvl(MSG::DEBUG)) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
/// Methods to initilize different strategies of seeds production
/// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find3Sp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv) const
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
    data.i_seede_Pro = data.l_seeds_Pro.begin();
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
  /// reset the i_seed_Pro iterator - this is used to return the seeds to the 
  /// consumer when they call next()
  data.i_seed_Pro = data.l_seeds_Pro.begin();

  
  if (msgLvl(MSG::DEBUG)) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
/// Methods to initilize different strategies of seeds production
/// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find3Sp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv, const double* ZVertex) const
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
    data.i_seede_Pro = data.l_seeds_Pro.begin();
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
  /// reset the i_seed_Pro iterator - this is used to return the seeds to the 
  /// consumer when they call next()
  data.i_seed_Pro = data.l_seeds_Pro.begin();

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

void InDet::SiSpacePointsSeedMaker_ATLxk::findVSp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  data.zminU = m_zmin;
  data.zmaxU = m_zmax;

  int mode = 5;
  if (lv.begin()!=lv.end()) mode = 6;
  bool newv = newVertices(data, lv);
  
  if (newv || !data.state || data.nspoint!=4 || data.mode!=mode || data.nlist) {
    data.i_seede_Pro = data.l_seeds_Pro.begin();
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
  data.i_seed_Pro = data.l_seeds_Pro.begin();

  if (msgLvl(MSG::DEBUG)) { 
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
/// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dump(EventData& data, MsgStream& out) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.nprint) return dumpEvent(data, out);
  return dumpConditions(data, out);
}

///////////////////////////////////////////////////////////////////
/// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dumpConditions(EventData &data, MsgStream& out) const
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
  n     = 42-m_beamSpotKey.key().size();
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
  out<<"| BeamConditionsService   | "<<m_beamSpotKey.key()<<s5
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
  out<<"| |rapidity|          <=  | " 
     <<std::setw(12)<<std::setprecision(5)<<m_rapcut
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
  out<<"| min radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1min
     <<"                              |"<<endmsg;
  out<<"| min radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2min
     <<"                              |"<<endmsg;
  out<<"| min radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3min
     <<"                              |"<<endmsg;
  out<<"| max radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(4)<<m_r1max
     <<"                              |"<<endmsg;
  out<<"| max radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2max
     <<"                              |"<<endmsg;
  out<<"| max radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3max
     <<"                              |"<<endmsg;
  out<<"| min radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1minv
     <<"                              |"<<endmsg;
  out<<"| min radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2minv
     <<"                              |"<<endmsg;
  out<<"| max radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1maxv
     <<"                              |"<<endmsg;
  out<<"| max radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2maxv
     <<"                              |"<<endmsg;
  out<<"| min space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmin
     <<"                              |"<<endmsg;
  out<<"| max space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmax
     <<"                              |"<<endmsg;
  out<<"| max dZ    impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzver 
     <<"                              |"<<endmsg;
  out<<"| max dZ/dR impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrver 
     <<"                              |"<<endmsg;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_maxdImpact
     <<"                              |"<<endmsg;
  out<<"| max       impact pps    | "
     <<std::setw(12)<<std::setprecision(5)<<m_maxdImpactPPS
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

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dumpEvent(EventData& data, MsgStream& out) const
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
     <<std::setw(12)<<data.l_seeds_Pro.size()
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
/// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::findNext(EventData& data) const
{
  if (data.endlist) return;

  data.i_seede_Pro = data.l_seeds_Pro.begin();

  if      (data.mode==0 or data.mode==1) production2Sp(data);
  else if (data.mode==2 or data.mode==3) production3Sp(data);
  else if (data.mode==5 or data.mode==6) production3Sp(data);

  data.i_seed_Pro = data.l_seeds_Pro.begin();
  ++data.nlist;
}                       

///////////////////////////////////////////////////////////////////
/// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ATLxk::newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const
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

  /// Ich bin der Geist, der stets verneint!
  /// are we sure this is not a bug?
  return false;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::buildFrameWork() 
{
  m_ptmin = std::abs(m_ptmin);
  
  /// For DBM reconstruction we use new parameters
  if (m_dbm) {
    m_r_rmax  = 150.;
    m_binSizeR = .5;
    m_drmin = 2.;
    m_ptmin = 10.;
    m_maxdImpact = 150.;
  } else if (m_ptmin < 100.) {
    m_ptmin = 100.;
  }
  /// ensure consistency in the transverse IP cuts 
  if (m_maxdImpactSSS < m_maxdImpact   ) m_maxdImpactSSS = m_maxdImpact;
  if (m_maxdImpactDecays < m_maxdImpactSSS) m_maxdImpactDecays = m_maxdImpactSSS;

  /// symmetrise eta cut if eta min not explicitly set 
  if (std::abs(m_etamin) < .1) m_etamin = -m_etamax;
  /// set dz/dr cut values based on eta cuts
  m_dzdrmax0  = 1./std::tan(2.*std::atan(std::exp(-m_etamax)));
  m_dzdrmin0  = 1./std::tan(2.*std::atan(std::exp(-m_etamin)));
  
  m_r3max     = m_r_rmax;
  /// cache inverse pt cuts 
  m_ipt       = 1./std::abs(.9*m_ptmin);
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
  const float inverseSizePhiMax = static_cast<float>(nPhiBinsMax)/twoPi; ///< for run-3: 53 : 6.28 ~ 8.43, bin size 0.118

  /// the minumum allowed granularity is given by a magic number. 
  /// best guess: 100 MeV min track pt? 
  const float inverseSizePhiMin = 100./60.;     ///< 1.67, bin size 0.6

  /// minimum expected pt. 
  float ptm = 400.;
  /// if we cut below 400 MeV, adapt the ptm 
  if (!m_dbm && m_ptmin < ptm) ptm = m_ptmin;

  /// binning driven by the min pt we expect  
  /// likely driven by the phi rotation we expect between 2 SP at miminum pt (max curvature) 
  m_inverseBinSizePhi = ptm /60.;
  /// for example 400. / 60 = 6.67, bin size 0.15

  /// truncate the bin size to fall within our thresholds
  if      (m_inverseBinSizePhi > inverseSizePhiMax) m_inverseBinSizePhi = inverseSizePhiMax;
  else if (m_inverseBinSizePhi < inverseSizePhiMin) m_inverseBinSizePhi = inverseSizePhiMin;

  /// now we can determine the number of bins by dividing the interval by the bin size 
  m_maxPhiBin = static_cast<int>(twoPi*m_inverseBinSizePhi);
  /// additional protection against too many bins. Should not happen given constraints above 
  if (m_maxPhiBin >=nPhiBinsMax) m_maxPhiBin = nPhiBinsMax-1;

  /// Build radius-azimuthal-Z sorted containers for Z-vertices
  /// same logic as for the space points above 
  const int   nPhiBinsVertexMax  = arraySizePhiV;
  const float inverseBinSizePhiVertexMax = static_cast<float>(nPhiBinsVertexMax)/twoPi;
  m_inverseBinSizePhiVertex = m_ptmin/120.;
  if (m_inverseBinSizePhiVertex > inverseBinSizePhiVertexMax) m_inverseBinSizePhiVertex = inverseBinSizePhiVertexMax;
  m_maxBinPhiVertex = static_cast<int>(twoPi*m_inverseBinSizePhiVertex);
  if (m_maxBinPhiVertex>=nPhiBinsVertexMax) m_maxBinPhiVertex = nPhiBinsVertexMax-1;


  /// Build maps for radius-azimuthal-Z sorted collections. 
  /// Here, we associate which bins are 'connected' to a given phi-Z bin 
  /// for the seeding 

  for (int phiBin=0; phiBin<=m_maxPhiBin; ++phiBin) {

    int phiBelow = phiBin-1;
    if (phiBelow<0) phiBelow=m_maxPhiBin; ///< loop around at edges of range

    int phiAbove = phiBin+1;
    if (phiAbove>m_maxPhiBin) phiAbove=0; ///< loop around at edges of range
    
    /// For each azimuthal region loop through all Z regions
    for (int z=0; z<arraySizeZ; ++z) {

      /// we always include the two neighbouring phi bins for the top / bottom 
      /// space point search 
 
      int twoDbinSamePhi        = phiBin * arraySizeZ+z;
      int twoDbinLowerPhi        = phiBelow*arraySizeZ+z;
      int twoDbinHigherPhi        = phiAbove*  arraySizeZ+z;

      m_nNeighbourCellsBottom [twoDbinSamePhi]    = 3; 
      m_nNeighbourCellsTop [twoDbinSamePhi]    = 3;

      m_neighbourCellsBottom[twoDbinSamePhi][0] = twoDbinSamePhi; 
      m_neighbourCellsTop[twoDbinSamePhi][0] = twoDbinSamePhi;

      m_neighbourCellsBottom[twoDbinSamePhi][1] = twoDbinLowerPhi; 
      m_neighbourCellsTop[twoDbinSamePhi][1] = twoDbinLowerPhi;

      m_neighbourCellsBottom[twoDbinSamePhi][2] = twoDbinHigherPhi; 
      m_neighbourCellsTop[twoDbinSamePhi][2] = twoDbinHigherPhi;

      /** in addition, we usually add at least one neighbouring slice 
      * in Z. This depends on where we are in the detector. 
      * Guide for the following: 
      * z == 5: central z region, |z|<250mm
      * 0  1  2  3  4    5    6  7  8  9  10  z bin index
      * --------------------------------------> Z[mm]
      *  Z=-2500       IP,Z=0            Z=+2500
      **/
      if (z==5) {
        m_nNeighbourCellsTop [twoDbinSamePhi]    = 9;
        // in the central z region, we include the two neighbouring 
        // z slices for the top neighbour search   

        m_neighbourCellsTop[twoDbinSamePhi][3] = twoDbinSamePhi+1;
        m_neighbourCellsTop[twoDbinSamePhi][4] = twoDbinLowerPhi+1;
        m_neighbourCellsTop[twoDbinSamePhi][5] = twoDbinHigherPhi+1;
        m_neighbourCellsTop[twoDbinSamePhi][6] = twoDbinSamePhi-1;
        m_neighbourCellsTop[twoDbinSamePhi][7] = twoDbinLowerPhi-1;
        m_neighbourCellsTop[twoDbinSamePhi][8] = twoDbinHigherPhi-1;
      } 
      // z > 5: positive z values, |z| > 250mm 
      else if (z> 5) {
        // for the bottom SP search in positive non-central z, we include the 
        // neighbouring Z region on the left (towards the IP) in the bottom 
        // neighbour search 
        m_nNeighbourCellsBottom [twoDbinSamePhi]    = 6;
        m_neighbourCellsBottom[twoDbinSamePhi][3] = twoDbinSamePhi-1;
        m_neighbourCellsBottom[twoDbinSamePhi][4] = twoDbinLowerPhi-1;
        m_neighbourCellsBottom[twoDbinSamePhi][5] = twoDbinHigherPhi-1;

        if (z<10) {
        /** for the top SP search in positive z, 
        * if we are not in the outermost z region, 
        * we include the neighbouring Z region on the right (away from the IP). 
        * In z = 10, the most forward, we do not have such a 'right side' neighbour we can add
        **/
          m_nNeighbourCellsTop [twoDbinSamePhi]    = 6;
          m_neighbourCellsTop[twoDbinSamePhi][3] = twoDbinSamePhi+1;
          m_neighbourCellsTop[twoDbinSamePhi][4] = twoDbinLowerPhi+1;
          m_neighbourCellsTop[twoDbinSamePhi][5] = twoDbinHigherPhi+1;
        }
      } 
      // z < 5: negative z values, |z| > 250mm 
      else {
        /** for the bottom SP in negative non-central z, we include 
        * the neighbouring z region on the right (towards the IP) in the 
        * bottom neighbour search 
        **/
        m_nNeighbourCellsBottom [twoDbinSamePhi]    = 6;
        m_neighbourCellsBottom[twoDbinSamePhi][3] = twoDbinSamePhi+1;
        m_neighbourCellsBottom[twoDbinSamePhi][4] = twoDbinLowerPhi+1;
        m_neighbourCellsBottom[twoDbinSamePhi][5] = twoDbinHigherPhi+1;

        if (z>0) {
          // if there is a z region on the left (away from the IP), we include it in the top
          // neighbour search 
          m_nNeighbourCellsTop [twoDbinSamePhi]    = 6;
          m_neighbourCellsTop[twoDbinSamePhi][3] = twoDbinSamePhi-1;
          m_neighbourCellsTop[twoDbinSamePhi][4] = twoDbinLowerPhi-1;
          m_neighbourCellsTop[twoDbinSamePhi][5] = twoDbinHigherPhi-1;
        }
      }

      /** 
      * z bins 3 / 7: 450mm < |z| < 925mm.: 
      * also include the central z region in the bottom SP search.  
      * likely for PPP seeds with hits in pixel barrel + endcaps
      **/
      if (z==3) {
        m_nNeighbourCellsBottom[twoDbinSamePhi]      = 9;
        m_neighbourCellsBottom[twoDbinSamePhi][6] = twoDbinSamePhi+2;
        m_neighbourCellsBottom[twoDbinSamePhi][7] = twoDbinLowerPhi+2;
        m_neighbourCellsBottom[twoDbinSamePhi][8] = twoDbinHigherPhi+2;
      } else if (z==7) {
        m_nNeighbourCellsBottom[twoDbinSamePhi]      = 9;
        m_neighbourCellsBottom[twoDbinSamePhi][6] = twoDbinSamePhi-2;
        m_neighbourCellsBottom[twoDbinSamePhi][7] = twoDbinLowerPhi-2;
        m_neighbourCellsBottom[twoDbinSamePhi][8] = twoDbinHigherPhi-2;
      }
    }
  }

  /// Build maps for radius-azimuthal-Z sorted collections for Z
  /// Similar logic to the above, just simplified as only 3 
  /// regions in z exist!
  for (int phiBin=0; phiBin<=m_maxBinPhiVertex; ++phiBin) {

    int phiBinBelow = phiBin-1;
    if (phiBinBelow<0) phiBinBelow=m_maxBinPhiVertex;

    int phiBinTop = phiBin+1;
    if (phiBinTop>m_maxBinPhiVertex) phiBinTop=0;
    
    /// For each azimuthal region loop through central Z regions
    for (int zbin=0; zbin<arraySizeZV; ++zbin) {
      
      int twoDbinSamePhi  = phiBin *arraySizeZV+zbin;
      int twoDbinLowePhi  = phiBinBelow*arraySizeZV+zbin;
      int twoDbinHigherPhi  = phiBinTop*arraySizeZV+zbin;

      /// always include the two neighbour bins in phi
      m_nNeighboursVertexPhiZ[twoDbinSamePhi]    = 3;
      m_neighboursVertexPhiZ[twoDbinSamePhi][0] = twoDbinSamePhi;
      m_neighboursVertexPhiZ[twoDbinSamePhi][1] = twoDbinLowePhi;
      m_neighboursVertexPhiZ[twoDbinSamePhi][2] = twoDbinHigherPhi;

      /// for the positive z bin, include the central z slice as well 
      if (zbin>1) {
        m_nNeighboursVertexPhiZ[twoDbinSamePhi]    = 6;
        m_neighboursVertexPhiZ[twoDbinSamePhi][3] = twoDbinSamePhi-1;
        m_neighboursVertexPhiZ[twoDbinSamePhi][4] = twoDbinLowePhi-1;
        m_neighboursVertexPhiZ[twoDbinSamePhi][5] = twoDbinHigherPhi-1;
      } 
      /// for the negative z bin, include the central z slice as well 
      else if (zbin<1) {
        m_nNeighboursVertexPhiZ[twoDbinSamePhi]    = 6;
        m_neighboursVertexPhiZ[twoDbinSamePhi][3] = twoDbinSamePhi+1;
        m_neighboursVertexPhiZ[twoDbinSamePhi][4] = twoDbinLowePhi+1;
        m_neighboursVertexPhiZ[twoDbinSamePhi][5] = twoDbinHigherPhi+1;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
/// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::buildBeamFrameWork(EventData& data) const
{ 
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };

  const Amg::Vector3D &bsCentre = beamSpotHandle->beamPos();
  double     tx = std::tan(beamSpotHandle->beamTilt(0));
  double     ty = std::tan(beamSpotHandle->beamTilt(1));

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

void  InDet::SiSpacePointsSeedMaker_ATLxk::convertToBeamFrameWork
(EventData& data, const Trk::SpacePoint*const& sp, std::array<float,3> & r) const
{
  r[0] = static_cast<float>(sp->globalPosition().x())-data.xbeam[0];
  r[1] = static_cast<float>(sp->globalPosition().y())-data.ybeam[0];
  r[2] = static_cast<float>(sp->globalPosition().z())-data.zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
/// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::fillLists(EventData& data) const
{
  constexpr float twoPi = 2.*M_PI;

  int  firstRadialBin =     0;
  bool ibl = false;

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
  
  for (int radialBin=data.r_first; radialBin<m_nBinsR;  ++radialBin) {

    /// skip empty radial bins
    if (!data.r_map[radialBin]) continue;
    /// remember the first non-empty bin we encounter 
    if (firstRadialBin == 0) firstRadialBin = radialBin;

    /// if we are in the second iteration (PPP pass), we have some special exit conditions. 
    if (data.iteration) {
      /** 
       * if we are dealing with a pixel hit 
      * and the radial bin index is below 20, we are in the IBL. 
      * this seems to be based on the default radial step of 2mm / bin, 
      * where this is equivalent to 40mm. 
      **/
      if (!data.r_Sorted[radialBin].front()->spacepoint->clusterList().second) {
        if (radialBin * m_binSizeR < m_radiusCutIBL) ibl = true;
      } 

      /// if we reach a *strip* hit in the PPP pass, we have two conditions for bailing out: 

      /// a) if we saw IBL hits in the past, we stop as soon as we see the first strip hit - pure PPP
      else if (ibl) {
        break;
      } 
      /** b) in case we did not see any IBL hits, we stop after bin index 175.
      * using the default r bin size of 2mm, this is equivalent to 350mm. 
      * So the idea seems to be to include the first strip layer at 299mm before we stop. 
      * This implies that for the non-IBL running, we include PPS seeds in this pass. 
      **/
      else if (radialBin > 175) {
        break;
      }
    }
    // loop over the space points in the r-bin and sort them into the 2d phi-z binning 
    for (InDet::SiSpacePointForSeed* SP : data.r_Sorted[radialBin]) {
      
      /// Azimuthal angle sort
      /// find the bin by dividing phi in 0...2pi by the bin size 
      float Phi = SP->phi();
      if (Phi<0.) Phi+=twoPi; // phi is defined in [0..2pi] for the binning 
      int phiBin = static_cast<int>(Phi*m_inverseBinSizePhi);
      /// handle overflows
      if (phiBin < 0) {
        phiBin = m_maxPhiBin;
      } else if (phiBin > m_maxPhiBin) {
        phiBin = 0;
      }

      float Z = SP->z();
      /** z-coordinate sort. 
      * Here, we have a variable bin size. 
      * Use a map to replace 5 levels of nested ternaries 
      * for a somewhat more readable notation while retaining
      * a logN lookup speed 
      **/
      int zBin{0};
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
      data.rfz_Sorted[twoDbin].push_back(SP);
      /// the conditional seems to always be true. The rfz_index vector stores 
      /// the 2D bin for each SP in the radius-sorted map. This way, 
      /// we obtain effectively a *3D binning* in r(via the r-sorted vector), phi and z (via the 2D index)
      if (!data.rfz_map[twoDbin]++) data.rfz_index[data.nrfz++] = twoDbin;
    }
  }
  /// if we do not use the SCT, and did see some hits somewhere below 43mm (meaning we have the IBL installed), 
  /// apply stricter requirements on the seed score later on 
  if (!m_sct && firstRadialBin && static_cast<float>(firstRadialBin)*m_binSizeR < m_radiusCutIBL) {
    data.maxScore = m_seedScoreThresholdPPPConfirmationSeed;
  }

  data.state = 0;
}
   
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::erase(EventData& data) const
{
  for (int i=0; i<data.nrfz;  ++i) {
    int n = data.rfz_index[i];
    data.rfz_map[n] = 0;
    data.rfz_Sorted[n].clear();
  }
  
  for (int i=0; i<data.nrfzv; ++i) {
    int n = data.rfzv_index[i];
    data.rfzv_map[n] = 0;
    data.rfzv_Sorted[n].clear();
  }
  data.state = 0;
  data.nsaz  = 0;
  data.nsazv = 0;
  data.nrfz  = 0;
  data.nrfzv = 0;
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production2Sp(EventData& data) const
{
  if (data.nsazv<2) return;

  std::vector<InDet::SiSpacePointForSeed*>::iterator r0,r0e,r,re;
  int nseed = 0;

  // Loop thorugh all azimuthal regions
  //
  for (int f=data.fvNmin; f<=m_maxBinPhiVertex; ++f) {

    // For each azimuthal region loop through Z regions
    //
    int z = 0;
    if (!data.endlist) z = data.zMin;
    for (; z<arraySizeZV; ++z) {
      
      int a = f*arraySizeZV+z;
      if (!data.rfzv_map[a]) continue;
      r0  = data.rfzv_Sorted[a].begin();
      r0e = data.rfzv_Sorted[a].end  ();

      if (!data.endlist) {
        r0 = data.rMin;
        data.endlist = true;
      }

      // Loop through trigger space points
      //
      for (; r0!=r0e; ++r0) {

        float X  = (*r0)->x();
        float Y  = (*r0)->y();
        float R  = (*r0)->radius();
        if (R<m_r2minv) continue;
        if (R>m_r2maxv) break;
        float Z  = (*r0)->z();
        float ax = X/R;
        float ay = Y/R;

        // Bottom links production
        //
        int numberBottomCells = m_nNeighboursVertexPhiZ[a];
        for (int i=0; i<numberBottomCells; ++i) {
    
          int an = m_neighboursVertexPhiZ[a][i];
          if (!data.rfzv_map[an]) continue;

          r  = data.rfzv_Sorted[an].begin();
          re = data.rfzv_Sorted[an].end();
    
          for (; r!=re; ++r) {
      
            float Rb =(*r)->radius();
            if (Rb<m_r1minv) continue;
            if (Rb>m_r1maxv) break;
            float dR = R-Rb;
            if (dR<m_drminv) break;
            if (dR>m_drmax) continue;
            float dZ = Z-(*r)->z();
            float Tz = dZ/dR;
            if (Tz<data.dzdrmin || Tz>data.dzdrmax) continue;
            float Zo = Z-R*Tz;

            // Comparison with vertices Z coordinates
            //
            if (!isZCompatible(data, Zo, Rb, Tz)) continue;

            // Momentum cut
            //
            float dx =(*r)->x()-X;
            float dy =(*r)->y()-Y;
            float x  = dx*ax+dy*ay;
            float y  =-dx*ay+dy*ax;
            float xy = x*x+y*y;
            if (xy == 0.) continue;
            float r2 = 1./xy;
            float Ut = x*r2;
            float Vt = y*r2;
            float UR = Ut*R+1.;
            if (UR == 0.) continue;
            float A  = Vt*R/UR;
            float B  = Vt-A*Ut;
            if (std::abs(B*data.K) > m_ipt*std::sqrt(1.+A*A)) continue;
            ++nseed;
            newSeed(data, (*r), (*r0), Zo);
          }
        }
        if (nseed < m_maxsize) continue;
        data.endlist=false;
        data.rMin = (++r0);
        data.fvNmin=f;
        data.zMin=z;
        return;
      }
    }
  }
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production3Sp(EventData& data) const
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
  * So we first go from the IP to the right, then from the IP to the left, 
  * each time working our way outward
  **/ 
  const std::array<int,arraySizeZ> zBinIndex {5,6,7,8,9,10,4,3,2,1,0};

  /// prepare arrays to store the iterators over the SP containers for all 
  /// neighbouring cells we wish to consider in the seed formation
  std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator,arraySizeNeighbourBins> iter_topCands;
  std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator,arraySizeNeighbourBins> iter_endTopCands;
  std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator,arraySizeNeighbourBins> iter_bottomCands;
  std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator,arraySizeNeighbourBins> iter_endBottomCands;
  
  /// counter for the found
  int    nseed =    0;
  /// prevent another pass from being run when we run out of Seeds
  data.endlist = true;

  /// Loop thorugh all azimuthal regions
  for (int phiBin=data.fNmin; phiBin<=m_maxPhiBin; ++phiBin) {
    
    /// For each azimuthal region loop through all Z regions
    int z = 0;
    /// If we had to abort a previous run, continue where we left off 
    if (!data.endlist) z = data.zMin;

    /// note that this loop follows the order within 'zBinIndex',
    /// not the ascending order of z regions. We start in the centre, 
    /// not at -2500 mm, and then move outward. 
    for (; z<arraySizeZ; ++z) {
      
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
      for (int neighbourCellNumber=0; neighbourCellNumber<m_nNeighbourCellsBottom[phiZbin]; ++neighbourCellNumber) {
  
        int theNeighbourCell =  m_neighbourCellsBottom[phiZbin][neighbourCellNumber];
        /// only do something if this cell is populated 
        if (!data.rfz_map[theNeighbourCell]) continue;
        /// plug the begin and end iterators to the SP in the cell into our array 
        iter_bottomCands [numberBottomCells] = data.rfz_Sorted[theNeighbourCell].begin();
        iter_endBottomCands[numberBottomCells++] = data.rfz_Sorted[theNeighbourCell].end();
      } 

      /// walk through the cells in phi-z we wish to consider for the top SP search. 
      /// Typically, this will be 3 adjacent phi bins (including the one of the central SP) 
      /// and possibly neighbours in z on the side opposed to the IP or on both sides, 
      /// depdending on the z region we are in
      for (int neighbourCellNumber=0; neighbourCellNumber<m_nNeighbourCellsTop[phiZbin]; ++neighbourCellNumber) {

        int theNeighbourCell =  m_neighbourCellsTop[phiZbin][neighbourCellNumber];
        /// only do something if this cell is populated 
        if (!data.rfz_map[theNeighbourCell]) continue;
        /// plug the begin and end iterators to the SP in the cell into our array 
        iter_topCands [numberTopCells] = data.rfz_Sorted[theNeighbourCell].begin();
        iter_endTopCands[numberTopCells++] = data.rfz_Sorted[theNeighbourCell].end();
      } 

      /// now run the seed search for the current phi-z bin.
      if (!data.trigger) production3Sp       (data, iter_bottomCands, iter_endBottomCands, iter_topCands, iter_endTopCands, numberBottomCells, numberTopCells, nseed);
      else               production3SpTrigger(data, iter_bottomCands, iter_endBottomCands, iter_topCands, iter_endTopCands, numberBottomCells, numberTopCells, nseed);
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

void InDet::SiSpacePointsSeedMaker_ATLxk::production3Sp
(EventData& data,
 std::array <std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_bottomCands ,
 std::array <std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_endBottomCands,
 std::array <std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_topCands ,
 std::array <std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & iter_endTopCands,
 const int numberBottomCells, const int numberTopCells, int& nseed) const
{
  /** 
   * This methid implements the seed search for a single phi-Z region of the detector. 
   * The central SP is taken from the region, while the top and bottom SP are allowed 
   * to come from either the same or a range of neighbouring cells. 
   **/ 

  /// iterator across the candidates for the central space point. 
  std::vector<InDet::SiSpacePointForSeed*>::iterator iter_centralSP=iter_bottomCands[0];
  std::vector<InDet::SiSpacePointForSeed*>::iterator iter_otherSP;  ///< will be used for iterating over top/bottom SP

  /// radial interval for the central SP.
  /// For the PPP (PPS) pass, take Pixel layers 1,2,3
  float rmin =  40.;
  float rmax = 140.;
  /// for strip seed formation, allow most of the strip volume
  if((*iter_centralSP)->spacepoint->clusterList().second) {
    rmin = 280.;
    rmax = 540.;
  }

  /// find the first central SP candidate above the minimum radius. 
  for(; iter_centralSP!=iter_endBottomCands[0]; ++iter_centralSP) {
    if((*iter_centralSP)->radius() > rmin) break;
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
  const float& maxd0cutstrips = m_maxdImpactDecays;
  const float& zmin  = data.zminU;
  const float& zmax  = data.zmaxU;
  const float& dzdrmax = data.dzdrmax;
  const float& dzdrmin = data.dzdrmin;
  data.CmSp.clear();

  /// Loop through all central space point candidates
  for (; iter_centralSP!=iter_endBottomCands[0]; ++iter_centralSP) {

    const float& R  = (*iter_centralSP)->radius();
    if(R > rmax) break; ///< stop if we have moved outside our radial region of interest. 

    /// global coordinates of the central SP
    const float&        X    = (*iter_centralSP)->x();
    const float&        Y    = (*iter_centralSP)->y();
    const float&        Z    = (*iter_centralSP)->z();

    /// initialise a counter for found bottom links
    /// This also serves as an index in the data.SP vector
    int                 Nb   = 0;

    /// Bottom links production
    /// Loop over all the cells where we expect to find such SP 
    bool abortBottomLoop=false;
    for (int cell=0; !abortBottomLoop && cell<numberBottomCells; ++cell) {
      /// in each cell, loop over the space points
      for (iter_otherSP=iter_bottomCands[cell]; !abortBottomLoop && iter_otherSP!=iter_endBottomCands[cell]; ++iter_otherSP) {
        
        /// evaluate the radial distance between the central and bottom SP
        const float& Rb =(*iter_otherSP)->radius();
        float dR = R-Rb;

        /// if the bottom SP is too far, remember this for future iterations and 
        /// don't bother starting from the beginning again 
        if (dR > m_drmax) {
          iter_bottomCands[cell]=iter_otherSP;
          continue;
        }
        /// if the points are too close in r, abort (future ones will be even closer). 
        /// If we are in the second pass (PPP) and starting to reach strip spacepoints, also time to stop! 
        if (dR < m_drmin || (data.iteration && (*iter_otherSP)->spacepoint->clusterList().second)) break;

        /// dZ/dR
        const float dZdR = (Z-(*iter_otherSP)->z())/dR;
        /// and abs value 
        const float absdZdR = std::abs(dZdR);
        if (absdZdR < dzdrmin or absdZdR > dzdrmax) continue;
  
        /// Comparison with vertices Z coordinates
        /// straight line extrapolation to r=0 
        const float z0 = Z-R*dZdR;
	      if(z0 > zmax || z0 < zmin) continue;
        /// found a bottom SP candidate, write it into the data object
        data.SP[Nb] = (*iter_otherSP);
        /// abort if we exceed the capacity for saving space points
        if (++Nb==m_maxsizeSP) abortBottomLoop=true;
      } ///< end of loop over SP in bottom candidate cell
    } ///< end of loop over bottom candidate cells

    /// if we did not find ANY bottom SP, or if we exceed the storage capacity, we abort this seed candidate. 
    if (!Nb || Nb==m_maxsizeSP) continue;

    /// now continue with the top SP search. 
    /// Make the counter start at Nb, as this serves as a running 
    /// index in the SP list for this seed. 
    int Nt = Nb;
    
    /// Top links production
    bool abortTopLoop=false;

    /// again, loop over cells of interest, this time for the top SP candidate
    for (int cell=0; !abortTopLoop && cell<numberTopCells; ++cell) {
      /// loop over each SP in each cell 
      for (iter_otherSP=iter_topCands[cell]; !abortTopLoop && iter_otherSP!=iter_endTopCands[cell]; ++iter_otherSP) {
  
        /// evaluate the radial distance, 
        float Rt =(*iter_otherSP)->radius();
        float dR = Rt-R;

        /// and continue if we are too close 
        if (dR<m_drmin) {
          iter_topCands[cell]=iter_otherSP;
          continue;
        }
        /// if we are to far, the next ones will be even farther, so abort 
        if (dR>m_drmax) break;

        /// evaluate (and cut on) dZ/dR
        float dZdR = ((*iter_otherSP)->z()-Z)/dR;
        float absdZdR = std::abs(dZdR);
	      if (absdZdR < dzdrmin or absdZdR > dzdrmax) continue;

        /// Comparison with vertices Z coordinates
        /// via straight line extrapolation to r=0 
        float z0 = Z-R*dZdR;
 	      if(z0 > zmax || z0 < zmin) continue;
        /// add SP to the list
        data.SP[Nt] = (*iter_otherSP);
        /// check capacity exceeding abort condition
        if (++Nt==m_maxsizeSP) abortTopLoop=true;
      } ///< end of loop over SP within top candidate cell
    } ///< end of loop over top candidate cells
    
    /// if we found no top candidates (remember, Nt starts counting at Nb), abort 
    if (!(Nt-Nb)) continue;

    /// get covariance on r and z for the central SP
    float covr0 = (*iter_centralSP)->covr ();
    float covz0 = (*iter_centralSP)->covz ();

    /// build a unit direction vector pointing from the IP to the central SP
    float ax    = X/R;
    float ay    = Y/R;

    /// check all SP candidates we found during our loop and 
    /// compute geometrical variables w.r.t the central point. 
    for (int i=0; i<Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = data.SP[i];

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
      float dr  = sqrt(r2);
      /// estimate slope in z - distance traveled in transverse plane vs z direction. 
      /// rough estimate of 1/sin theta from 2 points
      float tz  = dz*dr;

      /// if we are looking at a bottom SP candidate, flip the sign to account for 
      /// different direction of flight (from bottom to central) 
      if (i < Nb) tz = -tz;

      /// save this into our data object
      data.Tz[i]   = tz;      ///< 1/ sin theta
      data.Zo[i]   = Z-R*tz;  ///< z0 estimate. 
      data.R [i]   = dr;      ///< inverse distance to central SP
      data.U [i]   = x*r2;    ///< transformed U coordinate  
      data.V [i]   = y*r2;    ///< transformed V coordinate 
      data.Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;    ///< 1/r² (cov(z)) + 1/r²sin²theta cov(r)
    }

    /// tweak covariance matrix elements
    covr0      *= .5;
    covz0      *= 2.;

    data.nOneSeeds = 0;
    data.mapOneSeeds_Pro.clear();

    /// Three space points comparison
    /// first, loop over the bottom point candidates
    for (int b=0; b<Nb; ++b) {
      
      /// retrieve the geometrical paranmeters w.r.t the central SP for this candidate
      float  Zob  = data.Zo[b];       ///< z0 estimate from central+bottom SP
      float  Tzb  = data.Tz[b];       ///< 1/sintheta estimate from central+bottom SP
      float  Rb2r = data.R [b]*covr0; ///< inverse distance times covariance on r coordinate
      float  Rb2z = data.R [b]*covz0; ///< inverse distance times covariance on z coordinate
      float  Erb  = data.Er[b];       ///< 1/r² (cov(z)) + 1/r²sin²theta cov(r)
      float  Vb   = data.V [b];       ///< v-coordinate of bottom SP  
      float  Ub   = data.U [b];       ///< u-coordinate of bottom SP
      float  Tzb2 = (1.+Tzb*Tzb);     ///< 1+1/sin²theta 
      float sTzb2 = sqrt(Tzb2);       ///< sqrt (1+1/sin²theta)
      float  CSA  = Tzb2*COFK;        ///< magic number x (1+1+sin²theta) 
      float ICSA  = Tzb2*ipt2C;       ///< magic number x inverse of 90% of the pt cut, squared x (1+1+sin²theta) 
      /// max IP
      float d0max  = maxd0cut;
      /// for strips, apply the strip version of the IP cut 
      if (data.SP[b]->spacepoint->clusterList().second) d0max = maxd0cutstrips;

      /// inner loop over the top point candidates
      for (int t=Nb;  t<Nt; ++t) {

        /// this appears to be an eta-compatibility cut between the two segments 
        float dT  = ((Tzb-data.Tz[t])*(Tzb-data.Tz[t])-data.R[t]*Rb2z-(Erb+data.Er[t]))
                  -(data.R[t]*Rb2r)*((Tzb+data.Tz[t])*(Tzb+data.Tz[t]));
        if (dT > ICSA) continue;    

        /** 
        * The following exploits the transformation u:=x/(x²+y²); v:=y/(x²+y²); 
        * This is applied on the x,y coordinates in the frame described above, where the 
        * origin is put in the central SP and the x axis defined to point directly away from the IP.
        * 
        * In this transformed u,v frame, what would be our circle in x-y space takes the form of  
        * a linear function V = (-x0/y0) x U + 1/(2y0) =: A x U + B.
        * Here, x0 and y0 describe the center point of the circle in the x-y frame. 
        * As the origin of the x-y frame (the middle space point of our seed) is on the circle, 
        * we have x0²+y0²=R² with circle radius R. 
        * 
        * For our seed, we can experimentally obtain A as the slope of the linear function, 
        * delta V / delta U, 
        * estimated using the delta U and delta V between the top and bottom space point. 
        * 
        * B is then obtained by inserting the obtained A into the 
        * linear equation for the bottom SP, A x U + B = V --> B = V - A x U 
        * 
        * With x0²+y0²=R², and x0=-A/2B and y0=1/2B, the radius of the circle is 
        * then obtained as (2R)²=(1+A²)/B². 
        **/ 
        float deltaU  = data.U[t]-Ub;
        if (deltaU == 0.) continue;       ///< delta U = 0 blocks the evaluation of A. 
        float A   = (data.V[t]-Vb)/deltaU;  ///< A parameter, slope of the seed point distribution in the U,V plane 
        float B   = Vb-A*Ub;            ///< B parameter, V axis intercept of the seed point distribution in the U,V plane 
        float onePlusAsquare  = 1.+A*A;
        float BSquare  = B*B;
        /** With this radius (and pT) estimate, we can apply our pt cut. 
         * Reminder, ipt2K is 1 / (K x 0.9 x pt-cut)², where K translates pt into 2R. 
         * So here we can apply the pt cut directly on the (2R)² estimate without
         * the extra overhead of conversion / division
         **/ 
        if (BSquare  > ipt2K*onePlusAsquare || dT*onePlusAsquare > BSquare*CSA) continue;

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
	float d0 = 0;         
	if(std::abs(B) < 1e-10) d0  = std::abs((A-B*R)*R);         
	else{
	  float x0 = -A/(2.*B);
	  float rTrack = std::sqrt(onePlusAsquare/BSquare)*.5;
	  d0 = std::abs(-rTrack + std::sqrt(rTrack*rTrack +2*x0*R +R*R));
	}

        /// apply d0 cut to seed 
        if (d0 <= d0max) {
          /// evaluate distance the two closest-by SP in this seed candidate 
          float dr = data.R[b];
          if (data.R[t] < data.R[b]) dr = data.R[t];
          /// update the d0 estimate
          d0+=std::abs((Tzb-data.Tz[t])/(dr*sTzb2));
          /// record one possible seed candidate, sort by the curvature 
          data.CmSp.emplace_back(std::make_pair(B/std::sqrt(onePlusAsquare), data.SP[t]));
          /// store the transverse IP, will later be used as a quality estimator 
          data.SP[t]->setParam(d0);

        }
      }   ///< end loop over top space point candidates
      /// now apply further cleaning on the seed candidates for this central+bottom pair. 
      if (!data.CmSp.empty()) {
        newOneSeedWithCurvaturesComparison(data, data.SP[b], (*iter_centralSP), Zob);
      }
    } ///< end loop over bottom space points
    ///record seeds found in this run  
    fillSeeds(data);
    nseed += data.fillOneSeeds;
  } ///< end loop over central SP 
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds in ROI
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production3SpTrigger
(EventData& data,
 std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rb ,
 std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rbe,
 std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rt ,
 std::array<std::vector<InDet::SiSpacePointForSeed*>::iterator, arraySizeNeighbourBins> & rte,
 const int numberBottomCells, const int numberTopCells, int& nseed) const
{
  constexpr float twoPi = 2.*M_PI;

  std::vector<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;

  float rmin =  40.;
  float rmax = 140.;
  if((*r0)->spacepoint->clusterList().second) {
    rmin = 280.;
    rmax = 540.;
  }

  for(; r0!=rbe[0]; ++r0) {if((*r0)->radius() > rmin) break;}
  rt[0] = r0; ++rt[0];

  float ipt2K = data.ipt2K;
  float ipt2C = data.ipt2C;
  float COFK  = data.COFK;
  float maxd0cut = m_maxdImpact;
  float maxd0cutstrips = m_maxdImpactSSS;

  data.CmSp.clear();

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {

    data.nOneSeeds = 0;
    data.mapOneSeeds_Pro.clear();
  
    float R  = (*r0)->radius();
    if(R>rmax) break;

    const Trk::Surface* sur0 = (*r0)->sur();
    float               X    = (*r0)->x();
    float               Y    = (*r0)->y();
    float               Z    = (*r0)->z();
    int                 Nb   = 0;

    // Bottom links production
    //
    for (int i=0; i<numberBottomCells; ++i) {

      for (r=rb[i]; r!=rbe[i]; ++r) {
  
        float Rb =(*r)->radius();

        float dR = R-Rb;
        if (dR > m_drmax) {
          rb[i]=r;
          continue;
        }
        if ((*r)->sur()==sur0) continue;

        if (dR < m_drmin || (data.iteration && (*r)->spacepoint->clusterList().second)) break;

        // Comparison with  bottom and top Z 
        //
        float Tz = (Z-(*r)->z())/dR;
        float Zo = Z-R*Tz;
        if (Zo < data.zminB || Zo > data.zmaxB) continue;
        float Zu = Z+(550.-R)*Tz;
        if (Zu < data.zminU || Zu > data.zmaxU) continue;
        data.SP[Nb] = (*r);
        if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i<numberTopCells; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
  
        float Rt =(*r)->radius();
        float dR = Rt-R;
  
        if (dR<m_drmin) {
          rt[i]=r;
          continue;
        }
        if (dR>m_drmax) break;
        if ((*r)->sur()==sur0) continue;

        // Comparison with  bottom and top Z 
        //
        float Tz = ((*r)->z()-Z)/dR;
        float Zo = Z-R*Tz;
        if (Zo < data.zminB || Zo > data.zmaxB) continue;
        float Zu = Z+(550.-R)*Tz;
        if (Zu < data.zminU || Zu > data.zmaxU) continue;
        data.SP[Nt] = (*r);
        if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if (!(Nt-Nb)) continue;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();

    float ax   = X/R;
    float ay   = Y/R;
    
    for (int i=0; i<Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = data.SP[i];

      float dx  = sp->x()-X;
      float dy  = sp->y()-Y;
      float dz  = sp->z()-Z;
      float x   = dx*ax+dy*ay;
      float y   = dy*ax-dx*ay;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2);
      float tz  = dz*dr;
      if (i < Nb) tz = -tz;

      data.Tz[i]   = tz;
      data.Zo[i] = Z-R*tz;
      data.R [i]   = dr;
      data.U [i]   = x*r2;
      data.V [i]   = y*r2;
      data.Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for (int b=0; b<Nb; ++b) {
    
      float  Zob  = data.Zo[b];
      float  Tzb  = data.Tz[b];
      float  Rb2r = data.R [b]*covr0;
      float  Rb2z = data.R [b]*covz0;
      float  Erb  = data.Er[b];
      float  Vb   = data.V [b];
      float  Ub   = data.U [b];
      float  Tzb2 = (1.+Tzb*Tzb);
      float  CSA  = Tzb2*COFK;
      float ICSA  = Tzb2*ipt2C;
      float d0max  = maxd0cut;
      if (data.SP[b]->spacepoint->clusterList().second) d0max = maxd0cutstrips;
      
      for (int t=Nb;  t<Nt; ++t) {
  
        float dT  = ((Tzb-data.Tz[t])*(Tzb-data.Tz[t])-data.R[t]*Rb2z-(Erb+data.Er[t]))-(data.R[t]*Rb2r)*((Tzb+data.Tz[t])*(Tzb+data.Tz[t]));
        if ( dT > ICSA) continue;

        float deltaU  = data.U[t]-Ub;
        if (deltaU == 0.) continue;
        float A   = (data.V[t]-Vb)/deltaU;
        float onePlusAsquare  = 1.+A*A;
        float B   = Vb-A*Ub;
        float BSquare  = B*B;
        if (BSquare  > ipt2K*onePlusAsquare || dT*onePlusAsquare > BSquare*CSA) continue;

        float Im  = std::abs((A-B*R)*R);
        if (Im > d0max) continue;

        // Azimuthal angle test
        //
        float y  = 1.;
        float x  = 2.*B*R-A;
        float df = std::abs(std::atan2(ay*y-ax*x,ax*y+ay*x)-data.ftrig);
        if (df > M_PI) df = twoPi-df;
        if (df > data.ftrigW) continue;
        data.CmSp.emplace_back(std::make_pair(B/std::sqrt(onePlusAsquare), data.SP[t]));
        data.SP[t]->setParam(Im);
      }
      if (!data.CmSp.empty()) {
        newOneSeedWithCurvaturesComparison(data, data.SP[b], (*r0), Zob);
      }
    }
    fillSeeds(data);
    nseed += data.fillOneSeeds;
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newOneSeed
(EventData& data,
 InDet::SiSpacePointForSeed*& p1, InDet::SiSpacePointForSeed*& p2,
 InDet::SiSpacePointForSeed*& p3, float z, float seedCandidateQuality) const
{
  /// get the worst seed so far 
  float worstQualityInMap = std::numeric_limits<float>::min();
  InDet::SiSpacePointsProSeed* worstSeedSoFar = nullptr;
  if (!data.mapOneSeeds_Pro.empty()) {
    std::multimap<float,InDet::SiSpacePointsProSeed*>::reverse_iterator l = data.mapOneSeeds_Pro.rbegin();
    worstQualityInMap = (*l).first;
    worstSeedSoFar = (*l).second;
  }
  /// There are three cases where we simply add our new seed to the list and push it into the map: 
    /// a) we have not yet reached our max number of seeds 
  if (data.nOneSeeds < m_maxOneSize
    /// b) we have reached the max number but always want to keep confirmed seeds 
    /// and the new seed is a confirmed one, with worse quality than the worst one so far 
      || (m_alwaysKeepConfirmedSeeds && worstQualityInMap <= seedCandidateQuality  && isConfirmedSeed(p1,p3,seedCandidateQuality) && data.nOneSeeds < data.seedPerSpCapacity)
    /// c) we have reached the max number but always want to keep confirmed seeds 
    ///and the new seed of higher quality than the worst one so far, with the latter however being confirmed 
      || (m_alwaysKeepConfirmedSeeds && worstQualityInMap >  seedCandidateQuality  && isConfirmedSeed(worstSeedSoFar->spacepoint0(),worstSeedSoFar->spacepoint2(),worstQualityInMap) && data.nOneSeeds < data.seedPerSpCapacity)
    ){
    data.OneSeeds_Pro[data.nOneSeeds].set(p1,p2,p3,z);
    data.mapOneSeeds_Pro.insert(std::make_pair(seedCandidateQuality, &data.OneSeeds_Pro[data.nOneSeeds]));
    ++data.nOneSeeds;
  } 
  /// otherwise, we check if there is a poorer-quality seed that we can kick out
  else if (worstQualityInMap > seedCandidateQuality){
      /// Overwrite the parameters of the worst seed with the new one 
      worstSeedSoFar->set(p1,p2,p3,z);
      /// re-insert it with its proper quality to make sure it ends up in the right place 
      std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator 
        i = data.mapOneSeeds_Pro.insert(std::make_pair(seedCandidateQuality,worstSeedSoFar));
      /// and remove the entry with the old quality to avoid duplicates
      for (++i; i!=data.mapOneSeeds_Pro.end(); ++i) {
        if ((*i).second==worstSeedSoFar) {
          data.mapOneSeeds_Pro.erase(i);
          return;
        }
      }
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newOneSeedWithCurvaturesComparison
(EventData& data, SiSpacePointForSeed*& SPb, SiSpacePointForSeed*& SP0, float Zob) const
{
  static const float curvatureInterval = .00003;

  bool  bottomSPisPixel = !SPb->spacepoint->clusterList().second;
  float bottomSPQuality   = SPb->quality();
  float centralSPQuality   = SP0->quality();

  /// sort common SP by curvature 
  if(data.CmSp.size() > 2) std::sort(data.CmSp.begin(), data.CmSp.end(), comCurvature());
      
  float bottomR=SPb->radius();
  float bottomZ=SPb->z();

  std::vector<std::pair<float,InDet::SiSpacePointForSeed*>>::iterator it_otherSP;
  std::vector<std::pair<float,InDet::SiSpacePointForSeed*>>::iterator it_commonTopSP = data.CmSp.begin(), ie = data.CmSp.end();
  std::vector<std::pair<float,InDet::SiSpacePointForSeed*>>::iterator it_startInnerLoop=it_commonTopSP;

  /// check all possible common top SP
  for (; it_commonTopSP!=ie; ++it_commonTopSP) {

    /// the seed quality is set to d0 initially 
    float seedQuality    = (*it_commonTopSP).second->param();
    float originalSeedQuality   = (*it_commonTopSP).second->param();

    if(m_maxdImpact > 50){      //This only applies to LRT

      float topR=(*it_commonTopSP).second->radius();
      float topZ=(*it_commonTopSP).second->z();

      float theta1=std::atan2(topR-bottomR,topZ-bottomZ);
      float eta1=-std::log(std::tan(.5*theta1));

      float Zot=bottomZ - (bottomR-originalSeedQuality) * ((topZ-bottomZ)/(topR-bottomR));
      float theta0=std::atan2((*it_commonTopSP).second->param(),Zot);
      float eta0=-std::log(std::tan(.5*theta0));

      float deltaEta=std::abs(eta1-eta0); //For LLP daughters, the direction of the track is correlated with the direction of the LLP (which is correlated with the direction of the point of closest approach
      //calculate weighted average of d0 and deltaEta, normalized by their maximum values
      float f=std::min(0.5,originalSeedQuality/200.);  //0.5 and 200 are parameters chosen from a grid scan to optimize efficiency
      seedQuality*=(1-f)/300.;
      seedQuality+=f*deltaEta/2.5;
    }

    bool                topSPisPixel = !(*it_commonTopSP).second->spacepoint->clusterList().second;
    
    /// check the surface the hit is on 
    const Trk::Surface* surfaceTopSP  = (*it_commonTopSP).second->sur   ();
    float               radiusTopSP   = (*it_commonTopSP).second->radius();
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
      if (std::abs(radiusOtherSP-radiusTopSP) < m_drmin) continue;
      /// if we have a confirmation seed, we improve the score of the seed. 
      seedQuality += m_seedScoreBonusConfirmationSeed;
      /// only look for one confirmation seed! 
      break;
    }
    /// kick this seed candidate if the score is too high (lower values = better) 
    if (seedQuality > data.maxScore) continue;
    
    /// if we have PPS seeds and no confirmation SP exists (which would give the -200 bonus)
    /// or the seed quality is worse than the quality of the individual SP, skip this seed
    if (bottomSPisPixel!=topSPisPixel) {
      if (seedQuality > 0. || 
        (seedQuality > bottomSPQuality && seedQuality > centralSPQuality && seedQuality > (*it_commonTopSP).second->quality()) 
      ) continue;
    }
    /// if we have a SSS seed, apply the d0 cut. 
    /// Exception: If the SSS seed has a confirmation seed (-400 from SSS and -200 from confirmation), 
    /// then we skip the d0 cut  
    if (!bottomSPisPixel && (originalSeedQuality > m_maxdImpactSSS) && (seedQuality > m_seedScoreThresholdSSSConfirmationSeed)) continue;
    /// this is a good seed, save it (unless we have too many seeds per SP)
    newOneSeed(data, SPb, SP0, (*it_commonTopSP).second, Zob, seedQuality);
  } ///< end of loop over top SP candidates
  data.CmSp.clear();
}

///////////////////////////////////////////////////////////////////
/// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::fillSeeds(EventData& data) const
{
  data.fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator it_firstSeedCandidate = data.mapOneSeeds_Pro.begin();
  std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator it_seedCandidate  = data.mapOneSeeds_Pro.begin();
  std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator it_endSeedCandidates = data.mapOneSeeds_Pro.end();
  
  /// no seeds - nothing to do. 
  if (it_seedCandidate==it_endSeedCandidates) return;

  SiSpacePointsProSeed* theSeed{nullptr};

  /// loop over the seed candidates we have stored in the event data
  for (; it_seedCandidate!=it_endSeedCandidates; ++it_seedCandidate) {

    /// quality score of the seed, lower = better, list is sorted by quality 
    float quality = (*it_seedCandidate).first;
    theSeed       = (*it_seedCandidate).second;
    /// if this is not the highest-quality seed in the list and we have the first hit in the IBL, require a confirmation seed (score is then boosted by -200 for PPP + -200 for confirmation --> below -200)
    if (it_seedCandidate!=it_firstSeedCandidate && theSeed->spacepoint0()->radius() < m_radiusCutIBL && quality > m_seedScoreThresholdPPPConfirmationSeed) continue;
    /// this will return false for strip seeds if the quality of the seed is worse than the one of all of the space points on the seed. 
    if (!theSeed->setQuality(quality)) continue;
    
    /// if we have space, write the seed directly into an existing slot 
    if (data.i_seede_Pro!=data.l_seeds_Pro.end()) {
      theSeed  = &(*data.i_seede_Pro++);
      *theSeed = *(*it_seedCandidate).second;
    } else {
      /// otherwise, extend the seed list and update the iterators 
      data.l_seeds_Pro.emplace_back(SiSpacePointsProSeed(*(*it_seedCandidate).second));
      theSeed = &(data.l_seeds_Pro.back());
      data.i_seede_Pro = data.l_seeds_Pro.end();
    }
    
   ++data.fillOneSeeds;
  } ///< end loop over seed candidates 
}

const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_ATLxk::next(const EventContext&, EventData& data) const
{
  /// This only holds if we call next() without manually calling newEvent/find3Sp
  if (not data.initialized) initializeEventData(data);
  
  if (data.nspoint==3) {
    do {
      /// If we are out of seeds, call findNext to see if we can find more. 
      if (data.i_seed_Pro==data.i_seede_Pro) {
        /// findNext will call production3Sp again IF data.endlist is false, 
        /// which is only the case if the last run of production3Sp did not run to the end
        /// or if we did not run seed finding before 
        /// For run-3 offline, this will not do anything. 
        findNext(data);
        /// if no new seeds were found, exit  
        if (data.i_seed_Pro==data.i_seede_Pro) return nullptr;
      }
      /// iterate until we find a valid seed satisfying certain quality cuts in set3 
    } while (!(*data.i_seed_Pro++).set3(data.seedOutput));
    /// then return this next seed candidate 
    return &data.seedOutput;
  } else {
    /// same as above for 2SP
    if (data.i_seed_Pro==data.i_seede_Pro) {
      findNext(data);
      if (data.i_seed_Pro==data.i_seede_Pro) return nullptr;
    } 
    (*data.i_seed_Pro++).set2(data.seedOutput);
    return &data.seedOutput;
  }
  return nullptr;
}

bool InDet::SiSpacePointsSeedMaker_ATLxk::isZCompatible  
(EventData& data, const float& Zv, const float& R, const float& T) const
{
  if (Zv < data.zminU or Zv > data.zmaxU) return false;
  if (not data.isvertex) return true;
  if (data.l_vertex.empty()) return false;

  float dZmin = std::numeric_limits<float>::max();
  for (const float& v: data.l_vertex) {
    float dZ = std::abs(v-Zv);
    if (dZ >= dZmin) break;
    dZmin = dZ;
  }

  //return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
  //(Minor) speed-up: Avoid calculation of sqrt, compare squares
  return dZmin*dZmin < (m_dzver+m_dzdrver*R)*(m_dzver+m_dzdrver*R)*(1.+T*T);
}

///////////////////////////////////////////////////////////////////
/// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_ATLxk::newSpacePoint
(EventData& data, const Trk::SpacePoint*const& sp) const
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  /// r will store the coordinates of the space point relative 
  /// to the beam spot 
  std::array<float,3> r{0,0,0};
  convertToBeamFrameWork(data, sp, r);

  /// if needed, apply eta criterion 
  if (data.checketa) {
    float z = (std::abs(r[2])+m_zmax); 
    float x = r[0]*data.dzdrmin;
    float y = r[1]*data.dzdrmin;
    if ((z*z )<(x*x+y*y)) return sps;
  }
  /// If we have previously populated the list and just reset 
  /// the iterator when re-initialising the data object,
  /// then we re-use existing entries 
  if (data.i_spforseed!=data.l_spforseed.end()) {
    /// re-use existing entry at the current location 
    sps = &(*data.i_spforseed++);
    /// and then update the existing entry with the new SP and location.
    /// Unfortunately, set still relies on C-arrays... 
    sps->set(sp,&(r[0]));
  } else {
    /// otherwise, the list needs to grow
    data.l_spforseed.emplace_back(InDet::SiSpacePointForSeed(sp, &(r[0])));
    /// set our return pointer 
    sps = &(data.l_spforseed.back());
    /// and make sure to update the iterator 
    data.i_spforseed = data.l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New 2 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newSeed
(EventData& data, InDet::SiSpacePointForSeed*& p1, InDet::SiSpacePointForSeed*& p2, float z) const
{
  InDet::SiSpacePointForSeed* p3 = nullptr;

  if (data.i_seede_Pro!=data.l_seeds_Pro.end()) {
    SiSpacePointsProSeed* s = &(*data.i_seede_Pro++);
    s->set(p1, p2, p3, z);
  } else {
    data.l_seeds_Pro.emplace_back(SiSpacePointsProSeed(p1, p2, p3, z));
    data.i_seede_Pro = data.l_seeds_Pro.end();
  }
}

void InDet::SiSpacePointsSeedMaker_ATLxk::initializeEventData(EventData& data) const {
  int seedArrayPerSPSize = m_maxOneSize; 
  if (m_alwaysKeepConfirmedSeeds)  seedArrayPerSPSize = 50; 
  data.initialize(EventData::ATLxk,
                  m_maxsizeSP,
                  seedArrayPerSPSize,
                  0, /// maxsize not used
                  m_nBinsR,
                  0, /// sizeRF not used
                  arraySizePhiZ,
                  arraySizePhiZV,
                  m_checketa);
}

bool InDet::SiSpacePointsSeedMaker_ATLxk::isConfirmedSeed(const InDet::SiSpacePointForSeed* bottomSP, const InDet::SiSpacePointForSeed* topSP, float quality) const{

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
