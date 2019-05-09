/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "TrkToolInterfaces/IPRD_AssociationTool.h"

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_Cosmic::SiSpacePointsSeedMaker_Cosmic
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)
{
  declareInterface<ISiSpacePointsSeedMaker>(this);
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

  // Get magnetic field service
  //
  if ( !m_fieldServiceHandle.retrieve() ){
    ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
    return StatusCode::FAILURE;
  }    
  ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );

  // Get tool for track-prd association
  //
  if ( m_useassoTool ) {
    if ( m_assoTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool "<< m_assoTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_assoTool);
    }
  } else {
    m_assoTool.disable();
  }

  // Build framework
  //
  buildFrameWork();
  if ( m_ptmin < 300.) m_ptmin = 300.;

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if (m_outputlevel<=0) {
    m_nprint=0;
    ATH_MSG_DEBUG(*this);
  }

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

void InDet::SiSpacePointsSeedMaker_Cosmic::newEvent (int)
{
  if (!m_pixel && !m_sct) return;
  erase();
  m_i_spforseed = m_l_spforseed.begin();

  float irstep = 1./m_r_rstep;
  float errorsc[4] = {16.,16.,100.,16.};

  // Get pixels space points containers from store gate 
  //
  if (m_pixel) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel};
    if (spacepointsPixel.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {
	  
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (m_useassoTool &&  isUsed(sp)) continue;

	  int   ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);

	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp, errorsc);
	  m_r_Sorted[ir].push_back(sps);
          ++m_r_map[ir];
	  if (m_r_map[ir]==1) m_r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if (m_sct) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT};
    if (spacepointsSCT.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsSCT) {
        for (const Trk::SpacePoint* sp: *spc) {

	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (m_useassoTool && isUsed(sp)) continue;
	  
	  int   ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);

	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp, errorsc);
	  m_r_Sorted[ir].push_back(sps);
          ++m_r_map[ir];
	  if (m_r_map[ir]==1) m_r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if (m_useOverlap) {

      SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap{m_spacepointsOverlap};
      if (spacepointsOverlap.isValid()) {
	
        for (const Trk::SpacePoint* sp: *spacepointsOverlap) {

	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (m_useassoTool && isUsed(sp)) continue;

	  int   ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);

	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp, errorsc);
	  m_r_Sorted[ir].push_back(sps);
          ++m_r_map[ir];
	  if (m_r_map[ir]==1) m_r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{

  if (!m_pixel && !m_sct) return;
  erase();
  m_i_spforseed = m_l_spforseed.begin();

  float irstep = 1./m_r_rstep;
  float errorsc[4] = {16.,16.,100.,16.};

  // Get pixels space points containers from store gate 
  //
  if (m_pixel && vPixel.size()) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel};
    if (spacepointsPixel.isValid()) {
      SpacePointContainer::const_iterator spce = spacepointsPixel->end();

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vPixel) {
	SpacePointContainer::const_iterator w = spacepointsPixel->indexFind(l);
	if (w==spce) continue;
        for (const Trk::SpacePoint* sp: **w) {
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (m_useassoTool && isUsed(sp)) continue;

	  int   ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp, errorsc);
	  m_r_Sorted[ir].push_back(sps);
          ++m_r_map[ir];
	  if (m_r_map[ir]==1) m_r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if (m_sct && vSCT.size()) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT};
    if (spacepointsSCT.isValid()) {
      SpacePointContainer::const_iterator spce = spacepointsSCT->end();

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vSCT) {
	SpacePointContainer::const_iterator w = spacepointsSCT->indexFind(l);
	if (w==spce) continue;
        for (const Trk::SpacePoint* sp: **w) {
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (m_useassoTool && isUsed(sp)) continue;

	  int ir = static_cast<int>((sp->globalPosition().y()+m_r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp, errorsc);
	  m_r_Sorted[ir].push_back(sps);
          ++m_r_map[ir];
	  if (m_r_map[ir]==1) m_r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newRegion
(const std::vector<IdentifierHash>& vPixel,const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor&) 
{
  newRegion(vPixel,vSCT);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::find2Sp(const std::list<Trk::Vertex>& lv) 
{
  int mode;
  lv.begin()!=lv.end() ?  mode = 1 : mode = 0;

  m_nseeds = 0;
  m_l_seeds.erase(m_l_seeds.begin(),m_l_seeds.end());
  
  if ( !m_state || m_nspoint!=2 || m_mode!=mode || m_nlist) {

    m_state   = 1   ;
    m_nspoint = 2   ;
    m_nlist   = 0   ;
    m_mode    = mode;
    m_endlist = true;
    m_fNmin   = 0   ;
    m_zMin    = 0   ;
    production2Sp ();
  }

  m_i_seed  = m_l_seeds.begin();
  m_i_seede = m_l_seeds.end  ();

  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::find3Sp(const std::list<Trk::Vertex>& lv) 
{
  int mode;
  lv.begin()!=lv.end() ? mode = 3 : mode = 2;

  m_nseeds = 0;
  m_l_seeds.erase(m_l_seeds.begin(),m_l_seeds.end());

  if (!m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {

    m_state   = 1               ;
    m_nspoint = 3               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }

  m_i_seed  = m_l_seeds.begin();
  m_i_seede = m_l_seeds.end  ();

  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

void InDet::SiSpacePointsSeedMaker_Cosmic::find3Sp(const std::list<Trk::Vertex>& lv,const double*) 
{
  find3Sp(lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::findVSp (const std::list<Trk::Vertex>& lv)
{
  int mode;
  lv.begin()!=lv.end() ? mode = 6 : mode = 5;
  
  if (!m_state || m_nspoint!=4 || m_mode!=mode || m_nlist) {

    m_state   = 1               ;
    m_nspoint = 4               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }

  m_i_seed  = m_l_seeds.begin();
  m_i_seede = m_l_seeds.end  ();

  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dump( MsgStream& out ) const
{
  if (m_nprint)  return dumpEvent(out);
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dumpConditions( MsgStream& out ) const
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

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dumpEvent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| m_ns                    | "
     <<std::setw(12)<<m_ns
     <<"                              |"<<endmsg;
  out<<"| m_nsaz                  | "
     <<std::setw(12)<<m_nsaz
     <<"                              |"<<endmsg;
  out<<"| seeds                   | "
     <<std::setw(12)<<m_l_seeds.size()
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiSpacePointsSeedMaker_Cosmic::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
(MsgStream& sl,const InDet::SiSpacePointsSeedMaker_Cosmic& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
(std::ostream& sl,const InDet::SiSpacePointsSeedMaker_Cosmic& se)
{ 
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::buildFrameWork() 
{
  m_rapcut    = fabs(m_rapcut)                 ;
  m_dzdrmax   = 1./tan(2.*atan(exp(-m_rapcut)));
  m_dzdrmin   =-m_dzdrmax                      ;

  m_ns = m_nsaz = m_nr = m_nrf = m_nrfz = 0;

  // Build radius sorted containers
  //
  m_r_size = static_cast<int>((m_r_rmax+.1)/m_r_rstep)*2;
  m_r_Sorted.resize(m_r_size);
  m_r_index.resize(m_r_size, 0);
  m_r_map.resize(m_r_size, 0);
  m_nr = 0;

  // Build radius-azimuthal sorted containers
  //
  constexpr float pi2 = 2.*M_PI;
  const int   NFmax = SizeRF;
  const float sFmax = static_cast<float>(NFmax)/pi2;
  const float sFmin = 50./60.;

  m_sF = 50./60.;
  if (m_sF    >sFmax ) m_sF    = sFmax  ;
  else if (m_sF < sFmin) m_sF = sFmin;
  m_fNmax     = static_cast<int>(pi2*m_sF);
  if (m_fNmax >=NFmax) m_fNmax = NFmax-1;

  m_nrf = 0;
  for (int i=0; i<SizeRF; ++i) {
    m_rf_index[i]=0;
    m_rf_map[i]=0;
  }

  // Build radius-azimuthal-Z sorted containers
  //
  m_nrfz = 0;
  for (int i=0; i!=SizeRFZ; ++i) {
    m_rfz_index[i]=0;
    m_rfz_map[i]=0;
  }

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
  
  m_SP.resize(m_maxsizeSP, nullptr);
  m_R.resize(m_maxsizeSP, 0.);
  m_Tz.resize(m_maxsizeSP, 0.);
  m_Er.resize(m_maxsizeSP, 0.);
  m_U.resize(m_maxsizeSP, 0.);
  m_V.resize(m_maxsizeSP, 0.);
  m_seeds.resize(m_maxsize+5);
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::fillLists() 
{
  constexpr float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeed*>::iterator r;
  
  for (int i=0; i!= m_r_size;  ++i) {

    if (!m_r_map[i]) continue;
    r = m_r_Sorted[i].begin();

    while (r!=m_r_Sorted[i].end()) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi();
      if (F<0.) F+=pi2;

      //int   f = static_cast<int>(F*m_sF); f<0 ? f = m_fNmax : f>m_fNmax ? f = 0 : f=f;
      int f = 1;
      m_rf_Sorted[f].push_back(*r);
      if (!m_rf_map[f]++) m_rf_index[m_nrf++] = f;

      int z;
      float Z = (*r)->z();

      // Azimuthal angle and Z-coordinate sort
      //
      if (Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      } else {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*SizeZ+z;
      ++m_nsaz;
      m_rfz_Sorted[n].push_back(*r);
      if (!m_rfz_map[n]++) m_rfz_index[m_nrfz++] = n;
      m_r_Sorted[i].erase(r++);
    }
    m_r_map[i] = 0;
  }
  m_nr    = 0;
  m_state = 0;
}
 
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::erase()
{
  for (int i=0; i!=m_nr;    ++i) {
    int n = m_r_index[i];
    m_r_map[n] = 0;
    m_r_Sorted[n].erase(m_r_Sorted[n].begin(),m_r_Sorted[n].end());
  }

  for (int i=0; i!=m_nrf;   ++i) {
    int n = m_rf_index[i];
    m_rf_map[n] = 0;
    m_rf_Sorted[n].erase(m_rf_Sorted[n].begin(),m_rf_Sorted[n].end());
  }

  for (int i=0; i!=m_nrfz;  ++i) {
    int n = m_rfz_index[i];
    m_rfz_map[n] = 0;
    m_rfz_Sorted[n].erase(m_rfz_Sorted[n].begin(),m_rfz_Sorted[n].end());
  }

  m_state = 0;
  m_ns    = 0;
  m_nsaz  = 0;
  m_nr    = 0;
  m_nrf   = 0;
  m_nrfz  = 0;
}

///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production2Sp()
{
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3Sp()
{ 
  if (m_nsaz<3) return;

  float K   = 0.;

  double f[3], gP[3] ={10.,10.,0.};

  if (m_fieldServiceHandle->solenoidOn()) {
    m_fieldServiceHandle->getFieldZR(gP,f);
    K = 2./(300.*f[2]);
  }
  if (!K) return production3SpWithoutField();

  float ipt = 100000000.;
  if (m_ptmin!=0.) ipt= 1./fabs(.9*m_ptmin);

  const int   ZI[SizeZ]= {5,6,7,8,9,10,4,3,2,1,0};
  std::list<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];

  // Loop thorugh all azimuthal regions
  //
  for (int f=m_fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0;
    if (!m_endlist) z = m_zMin;

    for (; z<SizeZ; ++z) {

      int a = f *SizeZ+ZI[z];
      if (!m_rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for (int i=0; i!=m_rfz_b[a]; ++i) {
	
	int an =  m_rfz_ib[a][i];
	if (!m_rfz_map[an]) continue;
	rb [NB] = m_rfz_Sorted[an].begin();
        rbe[NB++] = m_rfz_Sorted[an].end();
      } 
      for (int i=0; i!=m_rfz_t[a]; ++i) {
	
	int an =  m_rfz_it[a][i];
	if (!m_rfz_map[an]) continue;
	rt [NT] = m_rfz_Sorted[an].begin();
        rte[NT++] = m_rfz_Sorted[an].end();
      } 
      production3Sp(rb,rbe,rt,rte,NB,NT,K,ipt);
      if (!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds without magnetic field
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3SpWithoutField()
{ 

  float ipt = 100000000.;
  if (m_ptmin!=0.) ipt= 1./fabs(.9*m_ptmin);
  const int   ZI[SizeZ]= {5,6,7,8,9,10,4,3,2,1,0};
  std::list<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];

  // Loop thorugh all azimuthal regions
  //
  for (int f=m_fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0;
    if (!m_endlist) z = m_zMin;

    for (; z<SizeZ; ++z) {

      int a  = f *SizeZ+ZI[z];
      if (!m_rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for (int i=0; i!=m_rfz_b[a]; ++i) {
	
	int an =  m_rfz_ib[a][i];
	if (!m_rfz_map[an]) continue;
	rb [NB] = m_rfz_Sorted[an].begin();
        rbe[NB++] = m_rfz_Sorted[an].end();
      } 
      for (int i=0; i!=m_rfz_t[a]; ++i) {
	
	int an =  m_rfz_it[a][i];
	if (!m_rfz_map[an]) continue;
	rt [NT] = m_rfz_Sorted[an].begin();
        rte[NT++] = m_rfz_Sorted[an].end();
      } 
      production3SpWithoutField(rb,rbe,rt,rte,NB,NT,ipt);
      if (!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds (new version)
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3Sp
( std::list<InDet::SiSpacePointForSeed*>::iterator* rb ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rt ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
  int NB, int NT, float K, float ipt) 
{
  const float COF = 134*.05*9.;

  std::list<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!m_endlist) {r0 = m_rMin; m_endlist = true;}

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {
	
    bool              pix   = true;
    if ((*r0)->spacepoint->clusterList().second) pix = false;
    float              R    = (*r0)->radius();
    const Trk::Surface* sur0 = (*r0)->sur();
    float              X    = (*r0)->x()    ;
    float              Y    = (*r0)->y()    ;
    float              Z    = (*r0)->z()    ;
    float              ax   = X/R           ;
    float              ay   = Y/R           ;
    float             covr0 = (*r0)->covr ();
    float             covz0 = (*r0)->covz ();

    int                 Nb   = 0;

    // Bottom links production
    //
    for (int i=0; i!=NB; ++i) {

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
	float dr = sqrt(r2)    ;
	m_Tz[Nb]  =-dz*dr       ;
        
	if (m_Tz[Nb]<m_dzdrmin || m_Tz[Nb]>m_dzdrmax) continue;
	
	m_SP[Nb] = (*r);
	m_U [Nb] = x*r2;
	m_V [Nb] = y*r2;
	m_Er[Nb] = (covz0+m_SP[Nb]->covz()+m_Tz[Nb]*m_Tz[Nb]*(covr0+m_SP[Nb]->covr()))*r2;
	m_R [Nb] = dr  ;
	if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:

    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i!=NT; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
	
	float dy = (*r)->y()-Y ;
	if (dy < m_drmin) {rt[i]=r; continue;}
	if (dy>m_drmax) break;
	if ((*r)->sur()==sur0) continue;

	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;
	if (!pix && !(*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X ;
	float dz = (*r)->z()-Z ;

	float x  = dx*ax+dy*ay ;
	float y  =-dx*ay+dy*ax ;
	float r2 = 1./(x*x+y*y);
	float dr = sqrt(r2)    ;
	m_Tz[Nt]  = dz*dr       ;
	if (m_Tz[Nt]<m_dzdrmin || m_Tz[Nt]>m_dzdrmax) continue;
	
	m_SP[Nt] = (*r);
	m_U [Nt] = x*r2;
	m_V [Nt] = y*r2;
	m_Er[Nt] = (covz0+m_SP[Nt]->covz()+m_Tz[Nt]*m_Tz[Nt]*(covr0+m_SP[Nt]->covr()))*r2;
	m_R [Nt] = dr;
	if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:

    if (!(Nt-Nb)) continue;

    // Three space points comparison
    //
    for (int b=Nb-1; b>=0; --b) {
      
      float SA  = 1.+m_Tz[b]*m_Tz[b];
      
      for (int t=Nb;  t!=Nt; ++t) {

	float Ts = .5*(m_Tz[b]+m_Tz[t]);
	float dt =  m_Tz[b]-m_Tz[t]    ;
	float dT = dt*dt-m_Er[b]-m_Er[t]-2.*m_R[b]*m_R[t]*(Ts*Ts*covr0+covz0);
	if ( dT > 0. && dT > (ipt*ipt)*COF*SA ) continue;
	float dU = m_U[t]-m_U[b]; if (dU == 0.) continue;
	float A  = (m_V[t]-m_V[b])/dU                  ;
	float B  =  m_V[t]-A*m_U[t]                    ;
	float S2 = 1.+A*A                              ;
	float S  = sqrt(S2)                            ;
	float BK = fabs(B*K)                           ;
	if (BK > ipt*S) continue                       ; // Momentum    cut
	dT       -= ((BK*BK)*COF*SA/S2)                ;
	if (dT > 0.) continue                          ; // Polar angle cut
	dT*= ((m_R[b]*m_R[t])/(m_R[b]+m_R[t]));

	newSeed(m_SP[b]->spacepoint,(*r0)->spacepoint,m_SP[t]->spacepoint,dT);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds without magnetic field
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3SpWithoutField
( std::list<InDet::SiSpacePointForSeed*>::iterator* rb ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rt ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
  int NB, int NT,float ipt) 
{
  const float COF    = 134*.05*9.;
  const float dFcut  = .96       ;

  std::list<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!m_endlist) {r0 = m_rMin; m_endlist = true;}

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
    for (int i=0; i!=NB; ++i) {

      for (r=rb[i]; r!=rbe[i]; ++r) {
	
	
	float dy = Y-(*r)->y() ;
	if ( dy < m_drmin) break;
	if ( dy > m_drmax || (*r)->sur()==sur0) continue;

	if (!pix && !(*r)->spacepoint->clusterList().second) continue;
	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;

	float dx = X-(*r)->x()     ;
	float dz = Z-(*r)->z()     ;
	float r2 = 1./(dx*dx+dy*dy);
	float dr = sqrt(r2)        ;
	m_Tz[Nb]  = dz*dr           ;
	
	if (m_Tz[Nb]<m_dzdrmin || m_Tz[Nb]>m_dzdrmax) continue;

	m_SP[Nb] = (*r) ;
	m_U [Nb] = dx*dr;
	m_V [Nb] = dy*dr;
	m_Er[Nb] = (covz0+m_SP[Nb]->covz()+m_Tz[Nb]*m_Tz[Nb]*(covr0+m_SP[Nb]->covr()))*r2;
	m_R [Nb] = dr   ;
	if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:

    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i!=NT; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
	
	float dy = (*r)->y()-Y ;
	if (dy < m_drmin) {rt[i]=r; continue;}
	if (dy>m_drmax) break;
	if ((*r)->sur()==sur0) continue;

	if ( pix &&  (*r)->spacepoint->clusterList().second) continue;
	if (!pix && !(*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X     ;
	float dz = (*r)->z()-Z     ;
	float r2 = 1./(dx*dx+dy*dy);
	float dr = sqrt(r2)        ;
	m_Tz[Nt]  = dz*dr           ;

	if (m_Tz[Nt]<m_dzdrmin || m_Tz[Nt]>m_dzdrmax) continue;

	m_SP[Nt] = (*r) ;
	m_U [Nt] = dx*dr;
	m_V [Nt] = dy*dr;
	m_Er[Nt] = (covz0+m_SP[Nt]->covz()+m_Tz[Nt]*m_Tz[Nt]*(covr0+m_SP[Nt]->covr()))*r2;
	m_R [Nt] = dr   ;
	if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:

    if (!(Nt-Nb)) continue;

    // Three space points comparison
    //
    for (int b=Nb-1; b>=0; --b) {
      
      float SA  = 1.+m_Tz[b]*m_Tz[b];

      for (int t=Nb;  t!=Nt; ++t) {

	// Azimuth angle cut
	//
	float dF = m_U[b]*m_U[t]+m_V[b]*m_V[t];
	if (dF < dFcut) continue;
	
	// Polar angle cut
	//
	float Ts = .5*(m_Tz[b]+m_Tz[t]);
	float dT =  m_Tz[b]-m_Tz[t]    ;
	dT        = dT*dT-m_Er[b]-m_Er[t]-2.*m_R[b]*m_R[t]*(Ts*Ts*covr0+covz0);
	dT       -= (ipt*ipt)*COF*SA    ;
	if ( dT > 0. ) continue          ;

	dT*= ((m_R[b]*m_R[t])/(m_R[b]+m_R[t]));

	newSeed(m_SP[b]->spacepoint,(*r0)->spacepoint,m_SP[t]->spacepoint,dT);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_Cosmic::isUsed(const Trk::SpacePoint* sp)
{
  const Trk::PrepRawData* d = sp->clusterList().first;
  if (!d || !m_assoTool->isUsed(*d)) return false;

  d = sp->clusterList().second;
  if (!d || m_assoTool->isUsed(*d)) return true;

  return false;
}

const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_Cosmic::next()
{
  if (m_i_seed==m_i_seede) return nullptr;
  InDet::SiSpacePointsSeed* sp = (*m_i_seed).second;
  ++m_i_seed;
  return(sp);
}
    
///////////////////////////////////////////////////////////////////
// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_Cosmic::newSpacePoint
(const Trk::SpacePoint*const& sp) 
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3] = {static_cast<float>(sp->globalPosition().x()),
                static_cast<float>(sp->globalPosition().y()),
                static_cast<float>(sp->globalPosition().z())};

  if (m_i_spforseed!=m_l_spforseed.end()) {
    sps = &(*m_i_spforseed++);
    sps->set(sp, r);
  } else {
    m_l_spforseed.push_back(InDet::SiSpacePointForSeed(sp, r));
    sps = &(m_l_spforseed.back());
    m_i_spforseed = m_l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New space point for seeds with error correction
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_Cosmic::newSpacePoint
(const Trk::SpacePoint*const& sp,const float* sc) 
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3];
  r[0]=sp->globalPosition().x();
  r[1]=sp->globalPosition().y();
  r[2]=sp->globalPosition().z();

  if (m_i_spforseed!=m_l_spforseed.end()) {
    sps = &(*m_i_spforseed++);
    sps->set(sp, r, sc);
  } else {
    m_l_spforseed.push_back(InDet::SiSpacePointForSeed(sp, r, sc));
    sps = &(m_l_spforseed.back());
    m_i_spforseed = m_l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New 2 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newSeed
(const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const float& z) 
{
  if (m_nseeds < m_maxsize) {
    m_seeds[m_nseeds].erase     (  );
    m_seeds[m_nseeds].add       (p1);
    m_seeds[m_nseeds].add       (p2);
    m_seeds[m_nseeds].setZVertex(0.);
    m_l_seeds.insert(std::make_pair(z, &(m_seeds[m_nseeds])));
    ++m_nseeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = m_l_seeds.rbegin();
    if ((*l).first <= z) return;
    InDet::SiSpacePointsSeed* s = (*l).second;
    m_l_seeds.erase((*l).first);

    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->setZVertex(0.);
    m_l_seeds.insert(std::make_pair(z,s));
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newSeed
(const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const Trk::SpacePoint*& p3,const float& z) 
{
  if (m_nseeds < m_maxsize) {
    m_seeds[m_nseeds].erase     (  );
    m_seeds[m_nseeds].add       (p1);
    m_seeds[m_nseeds].add       (p2);
    m_seeds[m_nseeds].add       (p3);
    m_seeds[m_nseeds].setZVertex(0.);
    m_l_seeds.insert(std::make_pair(z, &(m_seeds[m_nseeds])));
    ++m_nseeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = m_l_seeds.rbegin();
    if ((*l).first <= z) return;
    InDet::SiSpacePointsSeed* s = (*l).second;
    m_l_seeds.erase((*l).first);

    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->add       (p3);
    s->setZVertex(0.);
    m_l_seeds.insert(std::make_pair(z,s));
  }
}
