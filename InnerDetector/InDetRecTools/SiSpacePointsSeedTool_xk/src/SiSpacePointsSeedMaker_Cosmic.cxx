/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_Cosmic
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Cosmic.h"

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_Cosmic::SiSpacePointsSeedMaker_Cosmic
(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_Cosmic::initialize()
{
  StatusCode sc = AlgTool::initialize();

  ATH_CHECK(m_spacepointsPixel.initialize(m_pixel));
  ATH_CHECK(m_spacepointsSCT.initialize(m_sct));
  ATH_CHECK(m_spacepointsOverlap.initialize(m_useOverlap));

  ATH_CHECK( m_fieldCondObjInputKey.initialize() );

  // PRD-to-track association (optional)
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty()));

  // Build framework
  //
  buildFrameWork();
  if ( m_ptmin < 300.) m_ptmin = 300.;

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if (m_outputlevel<=0) {
    EventData data;
    initializeEventData(data);
    data.nprint=0;
    dump(data, msg(MSG::DEBUG));
  }

  m_initialized = true;
  
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_Cosmic::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newEvent(const EventContext& ctx, EventData& data, int) const
{
  if (!m_pixel && !m_sct) return;

  if (not data.initialized) initializeEventData(data);

  erase(data);
  data.i_spforseed = data.l_spforseed.begin();

  float irstep = 1./m_r_rstep;
  float errorsc[4] = {16.,16.,100.,16.};

  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr=prd_to_track_map.cptr();
  }

  // Get pixels space points containers from store gate 
  //
  if (m_pixel) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {
	  
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (prd_to_track_map_cptr && isUsed(sp,*prd_to_track_map_cptr)) continue;

	  int ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp, errorsc);
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
  if (m_sct) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsSCT) {
        for (const Trk::SpacePoint* sp: *spc) {

	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (prd_to_track_map_cptr && isUsed(sp,*prd_to_track_map_cptr)) continue;
	  
	  int ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp, errorsc);
	  data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
	  if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
	  ++data.ns;
	}
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if (m_useOverlap) {

      SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap{m_spacepointsOverlap, ctx};
      if (spacepointsOverlap.isValid()) {
	
        for (const Trk::SpacePoint* sp: *spacepointsOverlap) {

	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (prd_to_track_map_cptr && isUsed(sp, *prd_to_track_map_cptr)) continue;

	  int ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp, errorsc);
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

void InDet::SiSpacePointsSeedMaker_Cosmic::newRegion
(const EventContext& ctx, EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const
{
  if (not data.initialized) initializeEventData(data);

  if (!m_pixel && !m_sct) return;
  erase(data);
  data.i_spforseed = data.l_spforseed.begin();

  float irstep = 1./m_r_rstep;
  float errorsc[4] = {16.,16.,100.,16.};

  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }

  // Get pixels space points containers from store gate 
  //
  if (m_pixel && vPixel.size()) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {
      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vPixel) {
	auto w = spacepointsPixel->indexFindPtr(l);
	if (w==nullptr) continue;
        for (const Trk::SpacePoint* sp: *w) {
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (prd_to_track_map_cptr && isUsed(sp,*prd_to_track_map_cptr)) continue;

	  int ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp, errorsc);
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
  if (m_sct && vSCT.size()) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vSCT) {
	auto w = spacepointsSCT->indexFindPtr(l);
	if (w==nullptr) continue;
        for (const Trk::SpacePoint* sp: *w) {
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (prd_to_track_map_cptr && isUsed(sp,*prd_to_track_map_cptr)) continue;

	  int ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp, errorsc);
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

void InDet::SiSpacePointsSeedMaker_Cosmic::newRegion
(const EventContext& ctx, EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT, const IRoiDescriptor&) const
{
  newRegion(ctx, data, vPixel, vSCT);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::find2Sp(EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  int mode = 0;
  if (lv.begin()!=lv.end()) mode = 1;

  data.nseeds = 0;
  data.l_seeds_map.erase(data.l_seeds_map.begin(),data.l_seeds_map.end());
  
  if ( !data.state || data.nspoint!=2 || data.mode!=mode || data.nlist) {

    data.state   = 1   ;
    data.nspoint = 2   ;
    data.nlist   = 0   ;
    data.mode    = mode;
    data.endlist = true;
    data.fNmin   = 0   ;
    data.zMin    = 0   ;
    production2Sp(data);
  }

  data.i_seed_map  = data.l_seeds_map.begin();
  data.i_seede_map = data.l_seeds_map.end  ();

  if (m_outputlevel<=0) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  int mode = 2;
  if (lv.begin()!=lv.end()) mode = 3;

  data.nseeds = 0;
  data.l_seeds_map.erase(data.l_seeds_map.begin(),data.l_seeds_map.end());

  if (!data.state || data.nspoint!=3 || data.mode!=mode || data.nlist) {

    data.state   = 1               ;
    data.nspoint = 3               ;
    data.nlist   = 0               ;
    data.mode    = mode            ;
    data.endlist = true            ;
    data.fNmin   = 0               ;
    data.zMin    = 0               ;
    production3Sp(ctx, data);
  }

  data.i_seed_map  = data.l_seeds_map.begin();
  data.i_seede_map = data.l_seeds_map.end  ();

  if (m_outputlevel<=0) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

void InDet::SiSpacePointsSeedMaker_Cosmic::find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv, const double*) const
{
  find3Sp(ctx, data, lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::findVSp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  int mode = 5;
  if (lv.begin()!=lv.end()) mode = 6;
  
  if (!data.state || data.nspoint!=4 || data.mode!=mode || data.nlist) {

    data.state   = 1               ;
    data.nspoint = 4               ;
    data.nlist   = 0               ;
    data.mode    = mode            ;
    data.endlist = true            ;
    data.fNmin   = 0               ;
    data.zMin    = 0               ;
    production3Sp(ctx, data);
  }

  data.i_seed_map  = data.l_seeds_map.begin();
  data.i_seede_map = data.l_seeds_map.end  ();

  if (m_outputlevel<=0) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dump(EventData& data, MsgStream& out) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.nprint) return dumpEvent(data, out);
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dumpConditions(MsgStream& out) const
{
  int n = 42-m_spacepointsPixel.key().size();
  std::string s2; for (int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 42-m_spacepointsSCT.key().size();
  std::string s3; for (int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlap.key().size();
  std::string s4; for (int i=0; i<n; ++i) s4.append(" "); s4.append("|");


  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.key() <<s2
     <<endmsg;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.key()<<s3
     <<endmsg;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.key() <<s4
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
  out<<"| min space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmin
     <<"                              |"<<endmsg;
  out<<"| max space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmax
     <<"                              |"<<endmsg;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_diver
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dumpEvent(EventData& data, MsgStream& out) const
{
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| data.ns                    | "
     <<std::setw(12)<<data.ns
     <<"                              |"<<endmsg;
  out<<"| data.nsaz                  | "
     <<std::setw(12)<<data.nsaz
     <<"                              |"<<endmsg;
  out<<"| seeds                   | "
     <<std::setw(12)<<data.l_seeds_map.size()
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::buildFrameWork() 
{
  m_rapcut  = fabs(m_rapcut);
  m_dzdrmax = 1./tan(2.*atan(exp(-m_rapcut)));
  m_dzdrmin = -m_dzdrmax;

  // Build radius sorted containers
  //
  m_r_size = static_cast<int>((m_r_rmax+.1)/m_r_rstep)*2;

  // Build radius-azimuthal sorted containers
  //
  constexpr float pi2 = 2.*M_PI;
  const int   NFmax = SizeRF;
  const float sFmax = static_cast<float>(NFmax)/pi2;
  const float sFmin = 50./60.;

  m_sF = 50./60.;
  if (m_sF    >sFmax ) m_sF    = sFmax  ;
  else if (m_sF < sFmin) m_sF = sFmin;
  m_fNmax = static_cast<int>(pi2*m_sF);
  if (m_fNmax >=NFmax) m_fNmax = NFmax-1;

  // Build maps for radius-azimuthal-Z sorted collections 
  //
  for (int f=0; f<=m_fNmax; ++f) {
    int fb = f-1; if (fb<0      ) fb=m_fNmax;
    int ft = f+1; if (ft>m_fNmax) ft=0;
    
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

	m_rfz_t [a]    = 9 ;   m_rfz_b [a]    = 9 ;
	m_rfz_it[a][3] = a+1;  m_rfz_ib[a][3] = a+1;
	m_rfz_it[a][4] = b+1;  m_rfz_ib[a][4] = b+1;
	m_rfz_it[a][5] = c+1;  m_rfz_ib[a][5] = c+1;
	m_rfz_it[a][6] = a-1;  m_rfz_ib[a][6] = a-1;
	m_rfz_it[a][7] = b-1;  m_rfz_ib[a][7] = b-1;
	m_rfz_it[a][8] = c-1;  m_rfz_ib[a][8] = c-1;
      } else if (z> 5) {

	m_rfz_b [a]    = 6 ;   m_rfz_t [a]    = 6  ;
	m_rfz_ib[a][3] = a-1;  m_rfz_it[a][3] = a-1;
	m_rfz_ib[a][4] = b-1;  m_rfz_it[a][4] = b-1;
	m_rfz_ib[a][5] = c-1;  m_rfz_it[a][5] = c-1;

	if (z<10) {

	  m_rfz_t [a]    = 9 ;  m_rfz_b [a]    = 9  ;
	  m_rfz_it[a][6] = a+1; m_rfz_ib[a][6] = a+1;
	  m_rfz_it[a][7] = b+1; m_rfz_ib[a][7] = b+1;
	  m_rfz_it[a][8] = c+1; m_rfz_ib[a][8] = c+1;
	}
      } else {

	m_rfz_b [a]    = 6  ;   m_rfz_t [a]    = 6 ;
	m_rfz_ib[a][3] = a+1;   m_rfz_it[a][3] = a+1;
	m_rfz_ib[a][4] = b+1;   m_rfz_it[a][4] = b+1;
	m_rfz_ib[a][5] = c+1;   m_rfz_it[a][5] = c+1;

	if (z>0) {

	  m_rfz_t [a]    = 9  ; m_rfz_b [a]    = 9 ;
	  m_rfz_it[a][6] = a-1; m_rfz_ib[a][6] = a-1;
	  m_rfz_it[a][7] = b-1; m_rfz_ib[a][7] = b-1;
	  m_rfz_it[a][8] = c-1; m_rfz_ib[a][8] = c-1;
	}
      }
      
      if (z==3) {
	m_rfz_b[a]      = 12;  m_rfz_t [a]     = 12;
	m_rfz_ib[a][ 9] = a+2; m_rfz_it[a][ 9] = a+2;
	m_rfz_ib[a][10] = b+2; m_rfz_it[a][10] = b+2;
	m_rfz_ib[a][11] = c+2; m_rfz_it[a][11] = c+2;
      } else if (z==7) {
	m_rfz_b[a]      = 12;  m_rfz_t [a]     = 12;
	m_rfz_ib[a][ 9] = a-2; m_rfz_it[a][ 9] = a-2;
	m_rfz_ib[a][10] = b-2; m_rfz_it[a][10] = b-2;
	m_rfz_ib[a][11] = c-2; m_rfz_it[a][11] = c-2;
      }
    }
  }
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::fillLists(EventData& data) const
{
  constexpr float pi2 = 2.*M_PI;
  
  for (int i=0; i<m_r_size; ++i) {
    if (!data.r_map[i]) continue;

    for (InDet::SiSpacePointForSeed* r  : data.r_Sorted[i]) {
      
      // Azimuthal angle sort
      //
      float F = r->phi();
      if (F<0.) F+=pi2;

      //int   f = static_cast<int>(F*m_sF); f<0 ? f = m_fNmax : f>m_fNmax ? f = 0 : f=f;
      int f = 1;
      data.rf_Sorted[f].push_back(r);
      if (!data.rf_map[f]++) data.rf_index[data.nrf++] = f;

      float Z = r->z();
      // Azimuthal angle and Z-coordinate sort
      //
      int z;
      if (Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      } else {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*SizeZ+z;
      ++data.nsaz;
      data.rfz_Sorted[n].push_back(r);
      if (!data.rfz_map[n]++) data.rfz_index[data.nrfz++] = n;
    }
    data.r_Sorted[i].clear();
    data.r_map[i] = 0;
  }
  data.nr    = 0;
  data.state = 0;
}
 
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::erase(EventData& data) const
{
  for (int i=0; i<data.nr; ++i) {
    int n = data.r_index[i];
    data.r_map[n] = 0;
    data.r_Sorted[n].erase(data.r_Sorted[n].begin(), data.r_Sorted[n].end());
  }

  for (int i=0; i>data.nrf; ++i) {
    int n = data.rf_index[i];
    data.rf_map[n] = 0;
    data.rf_Sorted[n].erase(data.rf_Sorted[n].begin(), data.rf_Sorted[n].end());
  }

  for (int i=0; i<data.nrfz; ++i) {
    int n = data.rfz_index[i];
    data.rfz_map[n] = 0;
    data.rfz_Sorted[n].erase(data.rfz_Sorted[n].begin(), data.rfz_Sorted[n].end());
  }

  data.state = 0;
  data.ns    = 0;
  data.nsaz  = 0;
  data.nr    = 0;
  data.nrf   = 0;
  data.nrfz  = 0;
}

///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production2Sp(EventData& data) const
{
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3Sp(const EventContext& ctx, EventData& data) const
{
  if (data.nsaz<3) return;

  float K   = 0.;

  double f[3], gP[3] ={10.,10.,0.};

  MagField::AtlasFieldCache    fieldCache;
  // Get field cache object
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("SiSpacePointsSeedMaker_Cosmic: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return;
  }
  fieldCondObj->getInitializedCache (fieldCache);

  if (fieldCache.solenoidOn()) {
    fieldCache.getFieldZR(gP,f);

    K = 2./(300.*f[2]);
  }
  if (!K) return production3SpWithoutField(data);

  float ipt = 100000000.;
  if (m_ptmin!=0.) ipt= 1./fabs(.9*m_ptmin);

  const int   ZI[SizeZ]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];

  // Loop thorugh all azimuthal regions
  //
  for (int f=data.fNmin; f<=m_fNmax; ++f) {
    // For each azimuthal region loop through all Z regions
    //
    int z = 0;
    if (!data.endlist) z = data.zMin;

    for (; z<SizeZ; ++z) {
      int a = f *SizeZ+ZI[z];
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
      production3Sp(data, rb, rbe, rt, rte, NB, NT, K, ipt);
      if (!data.endlist) {data.fNmin=f; data.zMin = z; return;} 
    }
  }
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds without magnetic field
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3SpWithoutField(EventData& data) const
{ 

  float ipt = 100000000.;
  if (m_ptmin!=0.) ipt= 1./fabs(.9*m_ptmin);
  const int   ZI[SizeZ]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];

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
	int an = m_rfz_ib[a][i];
	if (!data.rfz_map[an]) continue;
	rb [NB] = data.rfz_Sorted[an].begin();
        rbe[NB++] = data.rfz_Sorted[an].end();
      } 
      for (int i=0; i<m_rfz_t[a]; ++i) {
	int an = m_rfz_it[a][i];
	if (!data.rfz_map[an]) continue;
	rt [NT] = data.rfz_Sorted[an].begin();
        rte[NT++] = data.rfz_Sorted[an].end();
      } 
      production3SpWithoutField(data, rb, rbe, rt, rte, NB, NT, ipt);
      if (!data.endlist) {data.fNmin=f; data.zMin = z; return;} 
    }
  }
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds (new version)
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3Sp
(EventData& data,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rb ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rbe,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rt ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rte,
 int NB, int NT, float K, float ipt) const
{
  const float COF = 134*.05*9.;

  std::vector<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!data.endlist) {r0 = data.rMin; data.endlist = true;}

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {	
    bool  pix   = true;
    if ((*r0)->spacepoint->clusterList().second) pix = false;
    float R     = (*r0)->radius();
    const Trk::Surface* sur0 = (*r0)->sur();
    float X     = (*r0)->x()    ;
    float Y     = (*r0)->y()    ;
    float Z     = (*r0)->z()    ;
    float ax    = X/R           ;
    float ay    = Y/R           ;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    int   Nb    = 0;

    // Bottom links production
    //
    for (int i=0; i<NB; ++i) {
      for (r=rb[i]; r!=rbe[i]; ++r) {
	float dy = (*r)->y()-Y ;
	if (-dy < m_drmin) break;
	if (-dy > m_drmax || (*r)->sur()==sur0) continue;

	if (!pix && !(*r)->spacepoint->clusterList().second) continue;
	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X;
	float dz = (*r)->z()-Z;

	float x  = dx*ax+dy*ay ;
	float y  =-dx*ay+dy*ax ;
	float r2 = 1./(x*x+y*y);
	float dr = sqrt(r2);
	data.Tz[Nb] = -dz*dr;
        
	if (data.Tz[Nb]<m_dzdrmin || data.Tz[Nb]>m_dzdrmax) continue;
	
	data.SP[Nb] = (*r);
	data.U [Nb] = x*r2;
	data.V [Nb] = y*r2;
	data.Er[Nb] = (covz0+data.SP[Nb]->covz()+data.Tz[Nb]*data.Tz[Nb]*(covr0+data.SP[Nb]->covr()))*r2;
	data.R [Nb] = dr  ;
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
	float dy = (*r)->y()-Y ;
	if (dy < m_drmin) {rt[i]=r; continue;}
	if (dy>m_drmax) break;
	if ((*r)->sur()==sur0) continue;

	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;
	if (!pix && !(*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X;
	float dz = (*r)->z()-Z;

	float x  = dx*ax+dy*ay;
	float y  =-dx*ay+dy*ax;
	float r2 = 1./(x*x+y*y);
	float dr = sqrt(r2);
	data.Tz[Nt] = dz*dr;
	if (data.Tz[Nt]<m_dzdrmin || data.Tz[Nt]>m_dzdrmax) continue;
	
	data.SP[Nt] = (*r);
	data.U [Nt] = x*r2;
	data.V [Nt] = y*r2;
	data.Er[Nt] = (covz0+data.SP[Nt]->covz()+data.Tz[Nt]*data.Tz[Nt]*(covr0+data.SP[Nt]->covr()))*r2;
	data.R [Nt] = dr;
	if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:

    if (!(Nt-Nb)) continue;

    // Three space points comparison
    //
    for (int b=Nb-1; b>=0; --b) {
      float SA  = 1.+data.Tz[b]*data.Tz[b];
      for (int t=Nb; t<Nt; ++t) {
	float Ts = .5*(data.Tz[b]+data.Tz[t]);
	float dt =  data.Tz[b]-data.Tz[t]    ;
	float dT = dt*dt-data.Er[b]-data.Er[t]-2.*data.R[b]*data.R[t]*(Ts*Ts*covr0+covz0);
	if ( dT > 0. && dT > (ipt*ipt)*COF*SA ) continue;
	float dU = data.U[t]-data.U[b]; if (dU == 0.) continue;
	float A  = (data.V[t]-data.V[b])/dU                  ;
	float B  =  data.V[t]-A*data.U[t]                    ;
	float S2 = 1.+A*A                              ;
	float S  = sqrt(S2)                            ;
	float BK = fabs(B*K)                           ;
	if (BK > ipt*S) continue                       ; // Momentum    cut
	dT       -= ((BK*BK)*COF*SA/S2)                ;
	if (dT > 0.) continue                          ; // Polar angle cut
	dT*= ((data.R[b]*data.R[t])/(data.R[b]+data.R[t]));

	newSeed(data, data.SP[b]->spacepoint,(*r0)->spacepoint,data.SP[t]->spacepoint,dT);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds without magnetic field
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3SpWithoutField
(EventData& data,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rb ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rbe,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rt ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rte,
 int NB, int NT,float ipt) const
{
  const float COF    = 134*.05*9.;
  const float dFcut  = .96       ;

  std::vector<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!data.endlist) {r0 = data.rMin; data.endlist = true;}

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {	
    bool                pix  = true;
    if ((*r0)->spacepoint->clusterList().second) pix = false;
    const Trk::Surface* sur0 = (*r0)->sur()  ;
    float              X    = (*r0)->x()    ;
    float              Y    = (*r0)->y()    ;
    float              Z    = (*r0)->z()    ;
    float             covr0 = (*r0)->covr ();
    float             covz0 = (*r0)->covz ();

    int                 Nb   = 0;

    // Bottom links production
    //
    for (int i=0; i<NB; ++i) {
      for (r=rb[i]; r!=rbe[i]; ++r) {
	float dy = Y-(*r)->y() ;
	if ( dy < m_drmin) break;
	if ( dy > m_drmax || (*r)->sur()==sur0) continue;

	if (!pix && !(*r)->spacepoint->clusterList().second) continue;
	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;

	float dx = X-(*r)->x();
	float dz = Z-(*r)->z();
	float r2 = 1./(dx*dx+dy*dy);
	float dr = sqrt(r2);
	data.Tz[Nb]  = dz*dr;
	if (data.Tz[Nb]<m_dzdrmin || data.Tz[Nb]>m_dzdrmax) continue;

	data.SP[Nb] = (*r) ;
	data.U [Nb] = dx*dr;
	data.V [Nb] = dy*dr;
	data.Er[Nb] = (covz0+data.SP[Nb]->covz()+data.Tz[Nb]*data.Tz[Nb]*(covr0+data.SP[Nb]->covr()))*r2;
	data.R [Nb] = dr   ;
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
	float dy = (*r)->y()-Y ;
	if (dy < m_drmin) {rt[i]=r; continue;}
	if (dy>m_drmax) break;
	if ((*r)->sur()==sur0) continue;

	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;
	if (!pix && !(*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X;
	float dz = (*r)->z()-Z;
	float r2 = 1./(dx*dx+dy*dy);
	float dr = sqrt(r2);
	data.Tz[Nt]  = dz*dr;
	if (data.Tz[Nt]<m_dzdrmin || data.Tz[Nt]>m_dzdrmax) continue;

	data.SP[Nt] = (*r) ;
	data.U [Nt] = dx*dr;
	data.V [Nt] = dy*dr;
	data.Er[Nt] = (covz0+data.SP[Nt]->covz()+data.Tz[Nt]*data.Tz[Nt]*(covr0+data.SP[Nt]->covr()))*r2;
	data.R [Nt] = dr   ;
	if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:

    if (!(Nt-Nb)) continue;

    // Three space points comparison
    //
    for (int b=Nb-1; b>=0; --b) {
      float SA = 1.+data.Tz[b]*data.Tz[b];
      for (int t=Nb; t<Nt; ++t) {
	// Azimuth angle cut
	//
	float dF = data.U[b]*data.U[t]+data.V[b]*data.V[t];
	if (dF < dFcut) continue;
	
	// Polar angle cut
	//
	float Ts = .5*(data.Tz[b]+data.Tz[t]);
	float dT =  data.Tz[b]-data.Tz[t]    ;
	dT        = dT*dT-data.Er[b]-data.Er[t]-2.*data.R[b]*data.R[t]*(Ts*Ts*covr0+covz0);
	dT       -= (ipt*ipt)*COF*SA    ;
	if ( dT > 0. ) continue          ;

	dT*= ((data.R[b]*data.R[t])/(data.R[b]+data.R[t]));

	newSeed(data, data.SP[b]->spacepoint,(*r0)->spacepoint,data.SP[t]->spacepoint,dT);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_Cosmic::next(const EventContext&, EventData& data) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.i_seed_map==data.i_seede_map) return nullptr;
  InDet::SiSpacePointsSeed* sp = (*data.i_seed_map).second;
  ++data.i_seed_map;
  return sp;
}
    
///////////////////////////////////////////////////////////////////
// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_Cosmic::newSpacePoint
(EventData& data, const Trk::SpacePoint*const& sp) const
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3] = {static_cast<float>(sp->globalPosition().x()),
                static_cast<float>(sp->globalPosition().y()),
                static_cast<float>(sp->globalPosition().z())};

  if (data.i_spforseed!=data.l_spforseed.end()) {
    sps = &(*data.i_spforseed++);
    sps->set(sp, r);
  } else {
    data.l_spforseed.emplace_back(InDet::SiSpacePointForSeed(sp, r));
    sps = &(data.l_spforseed.back());
    data.i_spforseed = data.l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New space point for seeds with error correction
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_Cosmic::newSpacePoint
(EventData& data, const Trk::SpacePoint*const& sp,const float* sc) const
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3];
  r[0]=sp->globalPosition().x();
  r[1]=sp->globalPosition().y();
  r[2]=sp->globalPosition().z();

  if (data.i_spforseed!=data.l_spforseed.end()) {
    sps = &(*data.i_spforseed++);
    sps->set(sp, r, sc);
  } else {
    data.l_spforseed.emplace_back(InDet::SiSpacePointForSeed(sp, r, sc));
    sps = &(data.l_spforseed.back());
    data.i_spforseed = data.l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New 2 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newSeed
(EventData& data,
 const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const float& z) const
{
  if (data.nseeds < m_maxsize) {
    data.seeds[data.nseeds].erase     (  );
    data.seeds[data.nseeds].add       (p1);
    data.seeds[data.nseeds].add       (p2);
    data.seeds[data.nseeds].setZVertex(0.);
    data.l_seeds_map.insert(std::make_pair(z, &(data.seeds[data.nseeds])));
    ++data.nseeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = data.l_seeds_map.rbegin();
    if ((*l).first <= z) return;
    InDet::SiSpacePointsSeed* s = (*l).second;
    data.l_seeds_map.erase((*l).first);

    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->setZVertex(0.);
    data.l_seeds_map.insert(std::make_pair(z,s));
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newSeed
(EventData& data,
 const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const Trk::SpacePoint*& p3,const float& z) const
{
  if (data.nseeds < m_maxsize) {
    data.seeds[data.nseeds].erase     (  );
    data.seeds[data.nseeds].add       (p1);
    data.seeds[data.nseeds].add       (p2);
    data.seeds[data.nseeds].add       (p3);
    data.seeds[data.nseeds].setZVertex(0.);
    data.l_seeds_map.insert(std::make_pair(z, &(data.seeds[data.nseeds])));
    ++data.nseeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = data.l_seeds_map.rbegin();
    if ((*l).first <= z) return;
    InDet::SiSpacePointsSeed* s = (*l).second;
    data.l_seeds_map.erase((*l).first);

    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->add       (p3);
    s->setZVertex(0.);
    data.l_seeds_map.insert(std::make_pair(z,s));
  }
}

void InDet::SiSpacePointsSeedMaker_Cosmic::initializeEventData(EventData& data) const {
  data.initialize(EventData::Cosmic,
                  m_maxsizeSP,
                  0, // maxOneSize not used
                  m_maxsize,
                  m_r_size,
                  SizeRF,
                  SizeRFZ,
                  0, // sizeRFZV not used
                  false); // checkEta not used
}
