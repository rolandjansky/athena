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
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ATLxk::SiSpacePointsSeedMaker_ATLxk
(const std::string& t,const std::string& n,const IInterface* p)
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ATLxk::initialize()
{
  StatusCode sc = AlgTool::initialize();

  ATH_CHECK(m_spacepointsPixel.initialize(m_pixel));
  ATH_CHECK(m_spacepointsSCT.initialize(m_sct));
  ATH_CHECK(m_spacepointsOverlap.initialize(m_useOverlap));

  // Get beam geometry
  //
  if (not m_beamSpotKey.empty()) {
    ATH_CHECK(m_beamSpotKey.initialize());
  }

  ATH_CHECK( m_fieldCondObjInputKey.initialize() );

  // PRD-to-track association (optional)
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty()));
  
  // Build framework
  //
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
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ATLxk::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newEvent(const EventContext& ctx, EventData& data, int iteration) const
{
  if (not data.initialized) initializeEventData(data);

  data.trigger = false;
  if (!m_pixel && !m_sct) return;

  data.iteration = iteration;
  if (iteration <=0) data.iteration = 0;
  erase(data);
  data.dzdrmin = m_dzdrmin0;
  data.dzdrmax = m_dzdrmax0;
  data.umax = 100.;
  if (!data.iteration) {
    if (not m_beamSpotKey.empty()) {
      buildBeamFrameWork(data);
    }

    double f[3], gP[3] ={10.,10.,0.};

    MagField::AtlasFieldCache    fieldCache;
    // Get field cache object
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("SiSpacePointsSeedMaker_ATLxk: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return;
    }
    fieldCondObj->getInitializedCache (fieldCache);

    if (fieldCache.solenoidOn()) {
      fieldCache.getFieldZR(gP,f);

      data.K = 2./(300.*f[2]);
    } else {
      data.K = 2./(300.* 5. );
    }

    data.ipt2K = m_ipt2/(data.K*data.K);
    data.ipt2C = m_ipt2*m_COF;
    data.COFK = m_COF*(data.K*data.K);
    data.i_spforseed = data.l_spforseed.begin();
  } else {
    data.r_first = 0;
    fillLists(data);
    return;
  }

  data.checketa = data.dzdrmin > 1.;
  if (m_dbm) data.checketa = false;

  float irstep = 1./m_r_rstep;
  int   irmax  = m_r_size-1;
  for (int i=0; i<data.nr; ++i) {
    int n = data.r_index[i];
    data.r_map[n] = 0;
    data.r_Sorted[n].clear();
  }
  data.ns = data.nr = 0;

  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }

  data.r_first = 0;
  // Get pixels space points containers from store gate 
  //
  if (!m_dbm && m_pixel) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {

          if ((prd_to_track_map_cptr &&  isUsed(sp,*prd_to_track_map_cptr)) || sp->r() > m_r_rmax) continue;

          // Remove DBM space points
          //
          const InDetDD::SiDetectorElement* de= 
            static_cast<const InDetDD::SiDetectorElement*>(sp->clusterList().first->detectorElement());
          if (!de || de->isDBM()) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          if (ir > data.r_first) data.r_first = ir;
          ++data.ns;
        }
      }
    }
    ++data.r_first;
  }
  
  // Get sct space points containers from store gate 
  //
  if (!m_dbm && m_sct) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsSCT) {
        for (const Trk::SpacePoint* sp: *spc) {

          if ((prd_to_track_map_cptr &&  isUsed(sp,*prd_to_track_map_cptr)) || sp->r() > m_r_rmax) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if (m_useOverlap && !data.checketa) {

      SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap{m_spacepointsOverlap, ctx};
      if (spacepointsOverlap.isValid()) {
  
        for (const Trk::SpacePoint* sp: *spacepointsOverlap) {

          if ((prd_to_track_map_cptr &&  isUsed(sp, *prd_to_track_map_cptr)) || sp->r() > m_r_rmax) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }
  
  // Get pixels space points containers from store gate for DBM reconstruction
  //
  if (m_dbm) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {

          // Keep only DBM space points
          //
          const InDetDD::SiDetectorElement* de= 
            static_cast<const InDetDD::SiDetectorElement*>(sp->clusterList().first->detectorElement());
          if (!de || !de->isDBM() || sp->r() > m_r_rmax) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          if (!sps) continue;

          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          if (ir > data.r_first) data.r_first = ir;
          ++data.ns;
        }
      }
    }
    ++data.r_first;
  }
  if (iteration < 0) data.r_first = 0;
  fillLists(data);
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

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
  data.umax = 100.;

  if (not m_beamSpotKey.empty()) {
    buildBeamFrameWork(data);
  }

  double f[3], gP[3] ={10.,10.,0.};

  MagField::AtlasFieldCache    fieldCache;
  // Get field cache object
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("SiSpacePointsSeedMaker_ATLxk: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return;
  }
  fieldCondObj->getInitializedCache (fieldCache);

  if (fieldCache.solenoidOn()) {
    fieldCache.getFieldZR(gP,f);

    data.K = 2./(300.*f[2]);
  } else {
    data.K = 2./(300.* 5. );
  }

  data.ipt2K = m_ipt2/(data.K*data.K);
  data.ipt2C = m_ipt2*m_COF;
  data.COFK = m_COF*(data.K*data.K);

  data.i_spforseed = data.l_spforseed.begin();

  float irstep = 1./m_r_rstep;
  int   irmax  = m_r_size-1;

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
          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
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
          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
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
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newRegion
(const EventContext& ctx, EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
 const IRoiDescriptor& IRD) const
{
  constexpr float pi2 = 2.*M_PI;

  if (not data.initialized) initializeEventData(data);

  newRegion(ctx, data, vPixel, vSCT);
  data.trigger = true;

  double dzdrmin = 1./tan(2.*atan(exp(-IRD.etaMinus())));
  double dzdrmax = 1./tan(2.*atan(exp(-IRD.etaPlus ())));
 
  data.zminB = IRD.zedMinus()-data.zbeam[0]; // min bottom Z
  data.zmaxB = IRD.zedPlus ()-data.zbeam[0]; // max bottom Z
  data.zminU = data.zminB+550.*dzdrmin;
  data.zmaxU = data.zmaxB+550.*dzdrmax;
  double fmax    = IRD.phiPlus ();
  double fmin    = IRD.phiMinus();
  if (fmin > fmax) fmin -= pi2;
  data.ftrig  = (fmin+fmax)*.5;
  data.ftrigW = (fmax-fmin)*.5;
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
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
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find3Sp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  data.zminU = m_zmin;
  data.zmaxU = m_zmax;

  int mode = 2;
  if (lv.begin()!=lv.end()) mode = 3;
  bool newv = newVertices(data, lv);

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
    production3Sp(data);
  }
  data.i_seed_Pro = data.l_seeds_Pro.begin();

  
  if (msgLvl(MSG::DEBUG)) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find3Sp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv, const double* ZVertex) const
{
  if (not data.initialized) initializeEventData(data);

  data.zminU = ZVertex[0];
  if (data.zminU < m_zmin) data.zminU = m_zmin;
  data.zmaxU = ZVertex[1];
  if (data.zmaxU > m_zmax) data.zmaxU = m_zmax;

  int mode = 2;
  if (lv.begin()!=lv.end()) mode = 3;
  bool newv = newVertices(data, lv);

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
    production3Sp(data);
  }
  data.i_seed_Pro = data.l_seeds_Pro.begin();

  if (msgLvl(MSG::DEBUG)) {  
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
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
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dump(EventData& data, MsgStream& out) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.nprint) return dumpEvent(data, out);
  return dumpConditions(data, out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
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
     <<std::setw(12)<<std::setprecision(5)<<m_r_rstep
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
     <<std::setw(12)<<std::setprecision(5)<<m_diver
     <<"                              |"<<endmsg;
  out<<"| max       impact pps    | "
     <<std::setw(12)<<std::setprecision(5)<<m_diverpps
     <<"                              |"<<endmsg;
  out<<"| max       impact sss    | "
     <<std::setw(12)<<std::setprecision(5)<<m_diversss
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
// Dumps event information into the MsgStream
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
// Find next set space points
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
// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ATLxk::newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const
{
  unsigned int s1 = data.l_vertex.size();
  unsigned int s2 = lV.size();

  data.isvertex = false;
  if (s1==0 && s2==0) return false;

  data.l_vertex.clear();
  if (s2 == 0) return false;

  data.isvertex = true;
  for (const Trk::Vertex& v: lV) {
    data.l_vertex.insert(static_cast<float>(v.position().z()));
  }

  data.zminU = (*data.l_vertex. begin())-20.;
  if (data.zminU < m_zmin) data.zminU = m_zmin;
  data.zmaxU = (*data.l_vertex.rbegin())+20.;
  if (data.zmaxU > m_zmax) data.zmaxU = m_zmax;

  return false;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::buildFrameWork() 
{
  m_ptmin = fabs(m_ptmin);
  
  // For DBM reconstruction we use new parameters
  //
  if (m_dbm) {
    m_r_rmax  = 150.;
    m_r_rstep = .5;
    m_drmin = 2.;
    m_ptmin = 10.;
    m_diver = 150.;
  } else if (m_ptmin < 100.) {
    m_ptmin = 100.;
  }
  if (m_diversss < m_diver   ) m_diversss = m_diver;
  if (m_divermax < m_diversss) m_divermax = m_diversss;

  if (fabs(m_etamin) < .1) m_etamin = -m_etamax;
  m_dzdrmax0  = 1./tan(2.*atan(exp(-m_etamax)));
  m_dzdrmin0  = 1./tan(2.*atan(exp(-m_etamin)));
  
  m_r3max     = m_r_rmax;
  m_COF       = 134*.05*9.;
  m_ipt       = 1./fabs(.9*m_ptmin);
  m_ipt2      = m_ipt*m_ipt;

  // Build radius sorted containers
  //
  m_r_size = static_cast<int>((m_r_rmax+.1)/m_r_rstep);

  // Build radius-azimuthal sorted containers
  //
  constexpr float pi2 = 2.*M_PI;
  const int   NFmax = SizeRF;
  const float sFmax = static_cast<float>(NFmax)/pi2;
  const float sFmin = 100./60.;

  float ptm = 400.;
  if (!m_dbm && m_ptmin < ptm) ptm = m_ptmin;

  m_sF = ptm /60.;
  if      (m_sF > sFmax) m_sF = sFmax;
  else if (m_sF < sFmin) m_sF = sFmin;
  m_fNmax = static_cast<int>(pi2*m_sF);
  if (m_fNmax >=NFmax) m_fNmax = NFmax-1;

  // Build radius-azimuthal-Z sorted containers for Z-vertices
  //
  const int   NFtmax  = SizeRFV;
  const float sFvmax = static_cast<float>(NFtmax)/pi2;
  m_sFv = m_ptmin/120.;
  if (m_sFv > sFvmax) m_sFv = sFvmax;
  m_fvNmax = static_cast<int>(pi2*m_sFv);
  if (m_fvNmax>=NFtmax) m_fvNmax = NFtmax-1;

  // Build maps for radius-azimuthal-Z sorted collections 
  //
  for (int f=0; f<=m_fNmax; ++f) {

    int fb = f-1;
    if (fb<0) fb=m_fNmax;
    int ft = f+1;
    if (ft>m_fNmax) ft=0;
    
    // For each azimuthal region loop through all Z regions
    //
    for (int z=0; z<SizeZ; ++z) {
 
      int a        = f *SizeZ+z;
      int b        = fb*SizeZ+z;
      int c        = ft*SizeZ+z;
      m_rfz_b [a]    = 3; m_rfz_t [a]    = 3;
      m_rfz_ib[a][0] = a; m_rfz_it[a][0] = a;
      m_rfz_ib[a][1] = b; m_rfz_it[a][1] = b;
      m_rfz_ib[a][2] = c; m_rfz_it[a][2] = c;
      if (z==5) {

        m_rfz_t [a]    = 9;
        m_rfz_it[a][3] = a+1;
        m_rfz_it[a][4] = b+1;
        m_rfz_it[a][5] = c+1;
        m_rfz_it[a][6] = a-1;
        m_rfz_it[a][7] = b-1;
        m_rfz_it[a][8] = c-1;
      } else if (z> 5) {

        m_rfz_b [a]    = 6;
        m_rfz_ib[a][3] = a-1;
        m_rfz_ib[a][4] = b-1;
        m_rfz_ib[a][5] = c-1;

        if (z<10) {

          m_rfz_t [a]    = 6;
          m_rfz_it[a][3] = a+1;
          m_rfz_it[a][4] = b+1;
          m_rfz_it[a][5] = c+1;
        }
      } else {

        m_rfz_b [a]    = 6;
        m_rfz_ib[a][3] = a+1;
        m_rfz_ib[a][4] = b+1;
        m_rfz_ib[a][5] = c+1;

        if (z>0) {

          m_rfz_t [a]    = 6;
          m_rfz_it[a][3] = a-1;
          m_rfz_it[a][4] = b-1;
          m_rfz_it[a][5] = c-1;
        }
      }

      if (z==3) {
        m_rfz_b[a]      = 9;
        m_rfz_ib[a][6] = a+2;
        m_rfz_ib[a][7] = b+2;
        m_rfz_ib[a][8] = c+2;
      } else if (z==7) {
        m_rfz_b[a]      = 9;
        m_rfz_ib[a][6] = a-2;
        m_rfz_ib[a][7] = b-2;
        m_rfz_ib[a][8] = c-2;
      }
    }
  }

  // Build maps for radius-azimuthal-Z sorted collections for Z
  //
  for (int f=0; f<=m_fvNmax; ++f) {

    int fb = f-1;
    if (fb<0) fb=m_fvNmax;
    int ft = f+1;
    if (ft>m_fvNmax) ft=0;
    
    // For each azimuthal region loop through central Z regions
    //
    for (int z=0; z<SizeZV; ++z) {
      
      int a  = f *SizeZV+z;
      int b  = fb*SizeZV+z;
      int c  = ft*SizeZV+z;
      m_rfzv_n[a]    = 3;
      m_rfzv_i[a][0] = a;
      m_rfzv_i[a][1] = b;
      m_rfzv_i[a][2] = c;
      if (z>1) {
        m_rfzv_n[a]    = 6;
        m_rfzv_i[a][3] = a-1;
        m_rfzv_i[a][4] = b-1;
        m_rfzv_i[a][5] = c-1;
      } else if (z<1) {
        m_rfzv_n[a]    = 6;
        m_rfzv_i[a][3] = a+1;
        m_rfzv_i[a][4] = b+1;
        m_rfzv_i[a][5] = c+1;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::buildBeamFrameWork(EventData& data) const
{ 
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };

  const Amg::Vector3D &cb = beamSpotHandle->beamPos();
  double     tx = tan(beamSpotHandle->beamTilt(0));
  double     ty = tan(beamSpotHandle->beamTilt(1));

  double ph   = atan2(ty,tx);
  double th   = acos(1./sqrt(1.+tx*tx+ty*ty));
  double sint = sin(th);
  double cost = cos(th);
  double sinp = sin(ph);
  double cosp = cos(ph);
  
  data.xbeam[0] = static_cast<float>(cb.x());
  data.xbeam[1] = static_cast<float>(cost*cosp*cosp+sinp*sinp);
  data.xbeam[2] = static_cast<float>(cost*sinp*cosp-sinp*cosp);
  data.xbeam[3] =-static_cast<float>(sint*cosp               );

  data.ybeam[0] = static_cast<float>(cb.y());
  data.ybeam[1] = static_cast<float>(cost*cosp*sinp-sinp*cosp);
  data.ybeam[2] = static_cast<float>(cost*sinp*sinp+cosp*cosp);
  data.ybeam[3] =-static_cast<float>(sint*sinp               );
  
  data.zbeam[0] = static_cast<float>(cb.z());
  data.zbeam[1] = static_cast<float>(sint*cosp);
  data.zbeam[2] = static_cast<float>(sint*sinp);
  data.zbeam[3] = static_cast<float>(cost);
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void  InDet::SiSpacePointsSeedMaker_ATLxk::convertToBeamFrameWork
(EventData& data, const Trk::SpacePoint*const& sp, float* r) const
{
  r[0] = static_cast<float>(sp->globalPosition().x())-data.xbeam[0];
  r[1] = static_cast<float>(sp->globalPosition().y())-data.ybeam[0];
  r[2] = static_cast<float>(sp->globalPosition().z())-data.zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::fillLists(EventData& data) const
{
  constexpr float pi2 = 2.*M_PI;
  std::vector<InDet::SiSpacePointForSeed*>::iterator r, re;

  int  ir0 =     0;
  bool ibl = false;
  
  for (int i=data.r_first; i<m_r_size;  ++i) {

    if (!data.r_map[i]) continue;
    r = data.r_Sorted[i].begin();
    re = data.r_Sorted[i].end();
    
    if (!ir0) ir0 = i;

    if (data.iteration) {
      if (!(*r)->spacepoint->clusterList().second) {
        if (i < 20) ibl = true;
      } else if (ibl) {
        break;
      } else if (i > 175) {
        break;
      }
    }

    for (; r!=re; ++r) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi();
      if (F<0.) F+=pi2;

      int f = static_cast<int>(F*m_sF);
      if (f < 0) {
        f = m_fNmax;
      } else if (f > m_fNmax) {
        f = 0;
      }

      float Z = (*r)->z();
      int z;
      // Azimuthal angle and Z-coordinate sort
      //
      if (Z>0.) {
        Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      } else {
        Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }

      int n = f*SizeZ+z;
      ++data.nsaz;
      data.rfz_Sorted[n].push_back(*r);
      if (!data.rfz_map[n]++) data.rfz_index[data.nrfz++] = n;
    }
  }
  if (!m_sct && ir0 && static_cast<float>(ir0)*m_r_rstep < 43.) {
    data.umax = -200.;
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
  for (int f=data.fvNmin; f<=m_fvNmax; ++f) {

    // For each azimuthal region loop through Z regions
    //
    int z = 0;
    if (!data.endlist) z = data.zMin;
    for (; z<SizeZV; ++z) {
      
      int a = f*SizeZV+z;
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
        int NB = m_rfzv_n[a];
        for (int i=0; i<NB; ++i) {
    
          int an = m_rfzv_i[a][i];
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
            if (fabs(B*data.K) > m_ipt*sqrt(1.+A*A)) continue;
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
  if (data.nsaz<3) return;

  const int   ZI[SizeZ]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int    nseed =    0;
  data.endlist = true;

  // Loop thorugh all azimuthal regions
  //
  for (int f=data.fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0;
    if (!data.endlist) z = data.zMin;

    for (; z<SizeZ; ++z) {

      int a  = f *SizeZ+ZI[z];
      if (!data.rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for (int i=0; i<m_rfz_b[a]; ++i) {
  
        int an =  m_rfz_ib[a][i];
        if (!data.rfz_map[an]) continue;
        rb [NB] = data.rfz_Sorted[an].begin();
        rbe[NB++] = data.rfz_Sorted[an].end();
      } 
      for (int i=0; i<m_rfz_t[a]; ++i) {
  
        int an =  m_rfz_it[a][i];
        if (!data.rfz_map[an]) continue;
        rt [NT] = data.rfz_Sorted[an].begin();
        rte[NT++] = data.rfz_Sorted[an].end();
      } 
      if (!data.trigger) production3Sp       (data, rb, rbe, rt, rte, NB, NT, nseed);
      else               production3SpTrigger(data, rb, rbe, rt, rte, NB, NT, nseed);
    }
    if (nseed>=m_maxsize) {
      data.endlist=false;
      data.fNmin  =  f+1;
      return;
    }
  }
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production3Sp
(EventData& data,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rb ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rbe,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rt ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rte,
 const int NB, const int NT, int& nseed) const
{
  std::vector<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;

  float rmin =  40.;
  float rmax = 140.;
  if((*r0)->spacepoint->clusterList().second) {
    rmin = 280.;
    rmax = 540.;
  }

  for(; r0!=rbe[0]; ++r0) {if((*r0)->radius() > rmin) break;}
  rt[0] = r0; ++rt[0];

  const float& ipt2K = data.ipt2K;
  const float& ipt2C = data.ipt2C;
  const float& COFK  = data.COFK;
  const float& imaxp = m_diver;
  const float& imaxs = m_divermax;
  const float& zmin  = data.zminU;
  const float& zmax  = data.zmaxU;
  const float& dzdrmax = data.dzdrmax;
  const float& dzdrmin = data.dzdrmin;
  data.CmSp.clear();

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {

    const float& R  = (*r0)->radius();
    if(R > rmax) break;

    const float&        X    = (*r0)->x();
    const float&        Y    = (*r0)->y();
    const float&        Z    = (*r0)->z();
    int                 Nb   = 0;

    // Bottom links production
    //
    for (int i=0; i<NB; ++i) {

      for (r=rb[i]; r!=rbe[i]; ++r) {
  
        const float& Rb =(*r)->radius();
        float dR = R-Rb;

        if (dR > m_drmax) {
          rb[i]=r;
          continue;
        }

        if (dR < m_drmin || (data.iteration && (*r)->spacepoint->clusterList().second)) break;

        const float Tz = (Z-(*r)->z())/dR;
        const float aTz = std::abs(Tz);
        if (aTz < dzdrmin or aTz > dzdrmax) continue;
  
        // Comparison with vertices Z coordinates
        //
        const float Zo = Z-R*Tz;
	if(Zo > zmax || Zo < zmin) continue;
        //if (!isZCompatible(data, Zo, Rb, Tz)) continue;
        data.SP[Nb] = (*r);
        if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i<NT; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
  
        float Rt =(*r)->radius();
        float dR = Rt-R;
  
        if (dR<m_drmin) {
          rt[i]=r;
          continue;
        }
        if (dR>m_drmax) break;


        float Tz = ((*r)->z()-Z)/dR;
        float aTz = std::abs(Tz);
	if (aTz < dzdrmin or aTz > dzdrmax) continue;

        // Comparison with vertices Z coordinates
        //
        float Zo = Z-R*Tz;
 	if(Zo > zmax || Zo < zmin) continue;
	//if (!isZCompatible(data, Zo, R, Tz)) continue;
        data.SP[Nt] = (*r);
        if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if (!(Nt-Nb)) continue;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    float ax    = X/R;
    float ay    = Y/R;

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
      data.Zo[i]   = Z-R*tz;
      data.R [i]   = dr;
      data.U [i]   = x*r2;
      data.V [i]   = y*r2;
      data.Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;

    data.nOneSeeds = 0;
    data.mapOneSeeds_Pro.clear();

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
      float sTzb2 = sqrt(Tzb2);
      float  CSA  = Tzb2*COFK;
      float ICSA  = Tzb2*ipt2C;
      float imax  = imaxp;
      if (data.SP[b]->spacepoint->clusterList().second) imax = imaxs;
  
      for (int t=Nb;  t<Nt; ++t) {
  
        float dT  = ((Tzb-data.Tz[t])*(Tzb-data.Tz[t])-data.R[t]*Rb2z-(Erb+data.Er[t]))-(data.R[t]*Rb2r)*((Tzb+data.Tz[t])*(Tzb+data.Tz[t]));
        if (dT > ICSA) continue;

        float dU  = data.U[t]-Ub;
        if (dU == 0.) continue;
        float A   = (data.V[t]-Vb)/dU;
        float S2  = 1.+A*A;
        float B   = Vb-A*Ub;
        float B2  = B*B;
        if (B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

        float Im  = fabs((A-B*R)*R);

        if (Im <= imax) {
          float dr = data.R[b];
          if (data.R[t] < data.R[b]) dr = data.R[t];
          Im+=fabs((Tzb-data.Tz[t])/(dr*sTzb2));
          data.CmSp.emplace_back(std::make_pair(B/sqrt(S2), data.SP[t]));
          data.SP[t]->setParam(Im);

        }
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
// Production 3 space points seeds in ROI
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production3SpTrigger
(EventData& data,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rb ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rbe,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rt ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rte,
 const int NB, const int NT, int& nseed) const
{
  constexpr float pi2 = 2.*M_PI;

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
  float imaxp = m_diver;
  float imaxs = m_diversss;

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
    for (int i=0; i<NB; ++i) {

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
    for (int i=0; i<NT; ++i) {
      
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
      float imax  = imaxp;
      if (data.SP[b]->spacepoint->clusterList().second) imax = imaxs;
      
      for (int t=Nb;  t<Nt; ++t) {
  
        float dT  = ((Tzb-data.Tz[t])*(Tzb-data.Tz[t])-data.R[t]*Rb2z-(Erb+data.Er[t]))-(data.R[t]*Rb2r)*((Tzb+data.Tz[t])*(Tzb+data.Tz[t]));
        if ( dT > ICSA) continue;

        float dU  = data.U[t]-Ub;
        if (dU == 0.) continue;
        float A   = (data.V[t]-Vb)/dU;
        float S2  = 1.+A*A;
        float B   = Vb-A*Ub;
        float B2  = B*B;
        if (B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

        float Im  = fabs((A-B*R)*R);
        if (Im > imax) continue;

        // Azimuthal angle test
        //
        float y  = 1.;
        float x  = 2.*B*R-A;
        float df = fabs(atan2(ay*y-ax*x,ax*y+ay*x)-data.ftrig);
        if (df > M_PI) df = pi2-df;
        if (df > data.ftrigW) continue;
        data.CmSp.emplace_back(std::make_pair(B/sqrt(S2), data.SP[t]));
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
 InDet::SiSpacePointForSeed*& p3, float z, float q) const
{
  if (data.nOneSeeds < m_maxOneSize) {

    data.OneSeeds_Pro[data.nOneSeeds].set(p1,p2,p3,z);
    data.mapOneSeeds_Pro.insert(std::make_pair(q, &data.OneSeeds_Pro[data.nOneSeeds]));
    ++data.nOneSeeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsProSeed*>::reverse_iterator 
      l = data.mapOneSeeds_Pro.rbegin();

    if ((*l).first <= q) return;
    
    InDet::SiSpacePointsProSeed* s = (*l).second;
    s->set(p1,p2,p3,z);

    std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator 
      i = data.mapOneSeeds_Pro.insert(std::make_pair(q,s));
  
    for (++i; i!=data.mapOneSeeds_Pro.end(); ++i) {
      if ((*i).second==s) {
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
  static const float dC = .00003;

  bool  pixb = !SPb->spacepoint->clusterList().second;
  float ub   = SPb->quality();
  float u0   = SP0->quality();

  if(data.CmSp.size() > 2) std::sort(data.CmSp.begin(), data.CmSp.end(), comCurvature());

  std::vector<std::pair<float,InDet::SiSpacePointForSeed*>>::iterator j,jn,i = data.CmSp.begin(), ie = data.CmSp.end();
  jn=i;

  for (; i!=ie; ++i) {

    float u    = (*i).second->param();
    float Im   = (*i).second->param();

    bool                pixt = !(*i).second->spacepoint->clusterList().second;
    
    const Trk::Surface* Sui  = (*i).second->sur   ();
    float               Ri   = (*i).second->radius();
    float               Ci1  =(*i).first-dC;
    float               Ci2  =(*i).first+dC;
    
    if      (!pixb) u-=400.;
    else if ( pixt) u-=200.;

    for (j=jn;  j!=ie; ++j) {
      
      if (       j == i           ) continue;
      if ( (*j).first < Ci1       ) {jn=j; ++jn; continue;}
      if ( (*j).first > Ci2       ) break;
      if ( (*j).second->sur()==Sui) continue;
      
      float Rj = (*j).second->radius();
      if (fabs(Rj-Ri) < m_drmin) continue;
      u -= 200.;
      break;
    }

    if (u > data.umax) continue;
    
    if (pixb!=pixt) {
      if (u > 0. || (u > ub && u > u0 && u > (*i).second->quality()) ) continue;
    }
    if (!pixb && Im > m_diversss && u > Im-500.) continue;

    newOneSeed(data, SPb, SP0, (*i).second, Zob, u);
  }
  data.CmSp.clear();
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::fillSeeds(EventData& data) const
{
  data.fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator 
    lf = data.mapOneSeeds_Pro.begin(),
    l  = data.mapOneSeeds_Pro.begin(),
    le = data.mapOneSeeds_Pro.end();
  
  if (l==le) return;

  SiSpacePointsProSeed* s;

  for (; l!=le; ++l) {

    float w = (*l).first;

    s       = (*l).second;
    if (l!=lf && s->spacepoint0()->radius() < 43. && w > -200.) continue;
    if (!s->setQuality(w)) continue;
    
    if (data.i_seede_Pro!=data.l_seeds_Pro.end()) {
      s  = &(*data.i_seede_Pro++);
      *s = *(*l).second;
    } else {
      data.l_seeds_Pro.emplace_back(SiSpacePointsProSeed(*(*l).second));
      s = &(data.l_seeds_Pro.back());
      data.i_seede_Pro = data.l_seeds_Pro.end();
    }
    
   ++data.fillOneSeeds;
  }
}

const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_ATLxk::next(const EventContext&, EventData& data) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.nspoint==3) {
    do {
      if (data.i_seed_Pro==data.i_seede_Pro) {
        findNext(data);
        if (data.i_seed_Pro==data.i_seede_Pro) return nullptr;
      }
    } while (!(*data.i_seed_Pro++).set3(data.seedOutput));
    return &data.seedOutput;
  } else {
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
    float dZ = fabs(v-Zv);
    if (dZ >= dZmin) break;
    dZmin = dZ;
  }

  //return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
  //(Minor) speed-up: Avoid calculation of sqrt, compare squares
  return dZmin*dZmin < (m_dzver+m_dzdrver*R)*(m_dzver+m_dzdrver*R)*(1.+T*T);
}

///////////////////////////////////////////////////////////////////
// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_ATLxk::newSpacePoint
(EventData& data, const Trk::SpacePoint*const& sp) const
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3];
  convertToBeamFrameWork(data, sp, r);

  if (data.checketa) {
    float z = (fabs(r[2])+m_zmax);
    float x = r[0]*data.dzdrmin;
    float y = r[1]*data.dzdrmin;
    if ((z*z )<(x*x+y*y)) return sps;
  }

  if (data.i_spforseed!=data.l_spforseed.end()) {
    sps = &(*data.i_spforseed++);
    sps->set(sp,r);
  } else {
    data.l_spforseed.emplace_back(InDet::SiSpacePointForSeed(sp, r));
    sps = &(data.l_spforseed.back());
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
  data.initialize(EventData::ATLxk,
                  m_maxsizeSP,
                  m_maxOneSize,
                  0, // maxsize not used
                  m_r_size,
                  0, // sizeRF not used
                  SizeRFZ,
                  SizeRFZV,
                  m_checketa);
}
