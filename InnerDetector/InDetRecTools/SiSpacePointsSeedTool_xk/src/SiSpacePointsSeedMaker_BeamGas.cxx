/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_BeamGas
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_BeamGas.h"

#include "TrkToolInterfaces/IPRD_AssociationTool.h"

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_BeamGas::SiSpacePointsSeedMaker_BeamGas
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)
{
  declareInterface<ISiSpacePointsSeedMaker>(this);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_BeamGas::~SiSpacePointsSeedMaker_BeamGas()
{
  delete [] m_r_index ;
  delete [] m_r_map   ; 
  delete [] m_r_Sorted;

  // Delete seeds
  //
  for (InDet::SiSpacePointsSeed* seed: m_l_seeds) {
    delete seed;
  }
  // Delete space points for reconstruction
  //
  for (InDet::SiSpacePointForSeed* spforseed: m_l_spforseed) {
    delete spforseed;
  }

  delete [] m_SP;
  delete [] m_R ;
  delete [] m_Tz;
  delete [] m_Er;
  delete [] m_U ;
  delete [] m_V ;
  delete [] m_Zo;
  delete [] m_OneSeeds;
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_BeamGas::initialize()
{
  StatusCode sc = AlgTool::initialize();

  ATH_CHECK(m_spacepointsPixel.initialize(m_pixel));
  ATH_CHECK(m_spacepointsSCT.initialize(m_sct));
  ATH_CHECK(m_spacepointsOverlap.initialize(m_useOverlap));

  // Get beam geometry
  //
  ATH_CHECK(m_beamSpotKey.initialize());

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

StatusCode InDet::SiSpacePointsSeedMaker_BeamGas::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::newEvent (int)
{
  if (!m_pixel && !m_sct) return;
  erase();
  buildBeamFrameWork();

  double f[3], gP[3] ={10.,10.,0.};

  if (m_fieldServiceHandle->solenoidOn()) {
    m_fieldServiceHandle->getFieldZR(gP,f);
    m_K = 2./(300.*f[2]);
  } else {
    m_K = 2./(300.* 5. );
  }

  m_i_spforseed   = m_l_spforseed.begin();

  float irstep = 1./m_r_rstep;
  
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

	  int ir = static_cast<int>(r*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp);
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

	  int ir = static_cast<int>(r*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp);
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

	  int ir = static_cast<int>(r*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp);
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

void InDet::SiSpacePointsSeedMaker_BeamGas::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{

  if (!m_pixel && !m_sct) return;
  erase();
  buildBeamFrameWork();

  double f[3], gP[3] ={10.,10.,0.};

  if (m_fieldServiceHandle->solenoidOn()) {
    m_fieldServiceHandle->getFieldZR(gP,f);
    m_K = 2./(300.*f[2]);
  } else {
    m_K = 2./(300.* 5. );
  }

  m_i_spforseed = m_l_spforseed.begin();

  float irstep = 1./m_r_rstep;

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
	  int ir = static_cast<int>(r*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp);
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
      for (const IdentifierHash& l: vPixel) {
	SpacePointContainer::const_iterator w = spacepointsSCT->indexFind(l);
	if (w==spce) continue;
        for (const Trk::SpacePoint* sp: **w) {
	  float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
	  if (m_useassoTool && isUsed(sp)) continue;
	  int ir = static_cast<int>(r*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint(sp);
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

void InDet::SiSpacePointsSeedMaker_BeamGas::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor&) 
{
  newRegion(vPixel,vSCT);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::find2Sp(const std::list<Trk::Vertex>& lv) 
{
  int mode;
  lv.begin()!=lv.end() ?  mode = 1 : mode = 0;
  
  if (!m_state || m_nspoint!=2 || m_mode!=mode || m_nlist) {

    m_i_seede   = m_l_seeds.begin();
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
  
  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::find3Sp(const std::list<Trk::Vertex>& lv) 
{
  int mode;
  lv.begin()!=lv.end() ? mode = 3 : mode = 2;

  if (!m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {

    m_i_seede   = m_l_seeds.begin() ;
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

  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}
void InDet::SiSpacePointsSeedMaker_BeamGas::find3Sp(const std::list<Trk::Vertex>& lv,const double*) 
{
  find3Sp(lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::findVSp (const std::list<Trk::Vertex>& lv)
{
  int mode;
  lv.begin()!=lv.end() ? mode = 6 : mode = 5;
  
  if (!m_state || m_nspoint!=4 || m_mode!=mode || m_nlist) {

    m_i_seede   = m_l_seeds.begin() ;
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

  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_BeamGas::dump( MsgStream& out ) const
{
  if (m_nprint)  return dumpEvent(out);
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_BeamGas::dumpConditions( MsgStream& out ) const
{
  int n = 42-m_spacepointsPixel.key().size();
  std::string s2; for (int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 42-m_spacepointsSCT.key().size();
  std::string s3; for (int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlap.key().size();
  std::string s4; for (int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 42-m_beamSpotKey.key().size();
  std::string s5; for (int i=0; i<n; ++i) s5.append(" "); s5.append("|");


  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.key() <<s2
     <<endmsg;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.key() <<s3
     <<endmsg;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.key() <<s4
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
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Beam X center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam Y center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam Z center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam X-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[3]
     <<"      |"<<endmsg;
  out<<"| Beam Y-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[3]
     <<"      |"<<endmsg;
  out<<"| Beam Z-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[3]
     <<"      |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_BeamGas::dumpEvent( MsgStream& out ) const
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

std::ostream& InDet::SiSpacePointsSeedMaker_BeamGas::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
(MsgStream& sl,const InDet::SiSpacePointsSeedMaker_BeamGas& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
(std::ostream& sl,const InDet::SiSpacePointsSeedMaker_BeamGas& se)
{ 
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::findNext () 
{
  if (m_endlist) return;

  m_i_seede = m_l_seeds.begin();
  if      (m_mode==0 || m_mode==1) production2Sp ();
  else if (m_mode==2 || m_mode==3) production3Sp ();
  else if (m_mode==5 || m_mode==6) production3Sp ();

  m_i_seed  = m_l_seeds.begin();
  ++m_nlist;
}                       

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::buildFrameWork() 
{
  m_ptmin     = fabs(m_ptmin)                  ;  if (m_ptmin < 300.) m_ptmin = 300.;
  m_rapcut    = fabs(m_rapcut)                 ;
  m_dzdrmax   = 1./tan(2.*atan(exp(-m_rapcut)));
  m_dzdrmin   =-m_dzdrmax                      ;
  m_COF       =  134*.05*9.                    ;
  m_ipt       = 1./fabs(.9*m_ptmin)            ;
  m_ipt2      = m_ipt*m_ipt                    ;
  m_K         = 0.                             ;

  m_ns = m_nsaz = m_nr = m_nrf = m_nrfz = 0;

  // Build radius sorted containers
  //
  m_r_size = static_cast<int>((m_r_rmax+.1)/m_r_rstep);
  m_r_Sorted = new std::list<InDet::SiSpacePointForSeed*>[m_r_size];
  m_r_index  = new int[m_r_size];
  m_r_map    = new int[m_r_size];
  m_nr   = 0; for (int i=0; i!=m_r_size; ++i) {m_r_index[i]=0; m_r_map[i]=0;}

  // Build radius-azimuthal sorted containers
  //
  const float pi2     = 2.*M_PI            ;
  const int   NFmax    = 53                ;
  const float sFmax   = static_cast<float>(NFmax )/pi2;
  const float sFmin = 100./60.          ;

  m_sF        = m_ptmin /60. ;
  if (m_sF    >sFmax ) m_sF    = sFmax  ;
  else if (m_sF < sFmin) m_sF = sFmin;
  m_fNmax     = static_cast<int>(pi2*m_sF);
  if (m_fNmax >=NFmax) m_fNmax = NFmax-1;

  m_nrf   = 0; for (int i=0; i!= 53; ++i) {m_rf_index  [i]=0; m_rf_map  [i]=0;}

  // Build radius-azimuthal-Z sorted containers
  //
  m_nrfz  = 0; for (int i=0; i!=583; ++i) {m_rfz_index [i]=0; m_rfz_map [i]=0;}

  // Build maps for radius-azimuthal-Z sorted collections 
  //
  for (int f=0; f<=m_fNmax; ++f) {

    int fb = f-1; if (fb<0      ) fb=m_fNmax;
    int ft = f+1; if (ft>m_fNmax) ft=0;
    
    // For each azimuthal region loop through all Z regions
    //
    for (int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      m_rfz_b [a]    = 3; m_rfz_t [a]    = 3;
      m_rfz_ib[a][0] = a; m_rfz_it[a][0] = a;
      m_rfz_ib[a][1] = b; m_rfz_it[a][1] = b;
      m_rfz_ib[a][2] = c; m_rfz_it[a][2] = c;
      if     (z==5) {

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
    }
  }
  
  if (!m_SP) m_SP   = new InDet::SiSpacePointForSeed*[m_maxsizeSP];
  if (!m_R ) m_R    = new                      float[m_maxsizeSP];
  if (!m_Tz) m_Tz   = new                      float[m_maxsizeSP];
  if (!m_Er) m_Er   = new                      float[m_maxsizeSP];
  if (!m_U ) m_U    = new                      float[m_maxsizeSP];
  if (!m_V ) m_V    = new                      float[m_maxsizeSP];
  if (!m_Zo) m_Zo   = new                      float[m_maxsizeSP];
  if (!m_OneSeeds) m_OneSeeds  = new InDet::SiSpacePointsSeed [m_maxOneSize];

  m_i_seed  = m_l_seeds.begin();
  m_i_seede = m_l_seeds.end  ();
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::buildBeamFrameWork() 
{ 
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };

  const Amg::Vector3D &cb =     beamSpotHandle->beamPos();
  double     tx = tan(beamSpotHandle->beamTilt(0));
  double     ty = tan(beamSpotHandle->beamTilt(1));

  double ph   = atan2(ty,tx);
  double th   = acos(1./sqrt(1.+tx*tx+ty*ty));
  double sint = sin(th);
  double cost = cos(th);
  double sinp = sin(ph);
  double cosp = cos(ph);
  
  m_xbeam[0] = static_cast<float>(cb.x())                  ;
  m_xbeam[1] = static_cast<float>(cost*cosp*cosp+sinp*sinp);
  m_xbeam[2] = static_cast<float>(cost*sinp*cosp-sinp*cosp);
  m_xbeam[3] =-static_cast<float>(sint*cosp               );
  
  m_ybeam[0] = static_cast<float>(cb.y())                  ;
  m_ybeam[1] = static_cast<float>(cost*cosp*sinp-sinp*cosp);
  m_ybeam[2] = static_cast<float>(cost*sinp*sinp+cosp*cosp);
  m_ybeam[3] =-static_cast<float>(sint*sinp               );
  
  m_zbeam[0] = static_cast<float>(cb.z())                  ;
  m_zbeam[1] = static_cast<float>(sint*cosp)               ;
  m_zbeam[2] = static_cast<float>(sint*sinp)               ;
  m_zbeam[3] = static_cast<float>(cost)                    ;
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void  InDet::SiSpacePointsSeedMaker_BeamGas::convertToBeamFrameWork
(const Trk::SpacePoint*const& sp,float* r) 
{
  
  float x = static_cast<float>(sp->globalPosition().x())-m_xbeam[0];
  float y = static_cast<float>(sp->globalPosition().y())-m_ybeam[0];
  float z = static_cast<float>(sp->globalPosition().z())-m_zbeam[0];
  r[0]     = m_xbeam[1]*x+m_xbeam[2]*y+m_xbeam[3]*z;
  r[1]     = m_ybeam[1]*x+m_ybeam[2]*y+m_ybeam[3]*z;
  r[2]     = m_zbeam[1]*x+m_zbeam[2]*y+m_zbeam[3]*z;
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::fillLists() 
{
  const float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeed*>::iterator r;
  
  for (int i=0; i!= m_r_size;  ++i) {

    if (!m_r_map[i]) continue;
    r = m_r_Sorted[i].begin();

    while (r!=m_r_Sorted[i].end()) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if (F<0.) F+=pi2;

      int   f = static_cast<int>(F*m_sF);
      if (f < 0) f = m_fNmax;
      else if (f > m_fNmax) f = 0;

      m_rf_Sorted[f].push_back(*r); if (!m_rf_map[f]++) m_rf_index[m_nrf++] = f;

      int z; float Z = (*r)->z();

      // Azimuthal angle and Z-coordinate sort
      //
      if (Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      } else {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      m_rfz_Sorted[n].push_back(*r); if (!m_rfz_map[n]++) m_rfz_index[m_nrfz++] = n;
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

void InDet::SiSpacePointsSeedMaker_BeamGas::erase()
{
  for (int i=0; i!=m_nr;    ++i) {
    int n = m_r_index[i]; m_r_map[n] = 0;
    m_r_Sorted[n].erase(m_r_Sorted[n].begin(),m_r_Sorted[n].end());
  }

  for (int i=0; i!=m_nrf;   ++i) {
    int n = m_rf_index[i]; m_rf_map[n] = 0;
    m_rf_Sorted[n].erase(m_rf_Sorted[n].begin(),m_rf_Sorted[n].end());
  }

  for (int i=0; i!=m_nrfz;  ++i) {
    int n = m_rfz_index[i]; m_rfz_map[n] = 0;
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

void InDet::SiSpacePointsSeedMaker_BeamGas::production2Sp()
{
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::production3Sp()
{ 
  if (m_nsaz<3) return;

  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::list<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0;

  // Loop thorugh all azimuthal regions
  //
  for (int f=m_fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if (!m_endlist) z = m_zMin;

    for (; z!=11; ++z) {

      int a  = f *11+ZI[z];  if (!m_rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for (int i=0; i!=m_rfz_b[a]; ++i) {
	
	int an =  m_rfz_ib[a][i];
	if (!m_rfz_map[an]) continue;
	rb [NB] = m_rfz_Sorted[an].begin(); rbe[NB++] = m_rfz_Sorted[an].end();
      } 
      for (int i=0; i!=m_rfz_t[a]; ++i) {
	
	int an =  m_rfz_it[a][i];
	if (!m_rfz_map[an]) continue; 
	rt [NT] = m_rfz_Sorted[an].begin(); rte[NT++] = m_rfz_Sorted[an].end();
      } 
      production3Sp(rb,rbe,rt,rte,NB,NT,nseed);
      if (!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds (new version)
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::production3Sp
( std::list<InDet::SiSpacePointForSeed*>::iterator* rb ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rt ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::list<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!m_endlist) {r0 = m_rMin; m_endlist = true;}

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.erase(m_mapOneSeeds.begin(), m_mapOneSeeds.end());
	
    float R = (*r0)->radius();
    if (R<m_r2min) continue;
    if (R>m_r2max) break;

    const Trk::SpacePoint* SP0 = (*r0)->spacepoint;

    bool pix = true;
    if (SP0->clusterList().second) pix = false;
    const Trk::Surface* sur0 = (*r0)->sur();
    float               X    = (*r0)->x()  ;
    float               Y    = (*r0)->y()  ;
    float               Z    = (*r0)->z()  ;
    int                 Nb   = 0           ;

    // Bottom links production
    //
    for (int i=0; i!=NB; ++i) {

      for (r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();
	if (Rb<m_r1min) {rb[i]=r; continue;}
        if (Rb>m_r1max) break;
	
	float dR = R-Rb;
	if (dR<m_drmin) break;

	if (dR > m_drmax || (*r)->sur()==sur0) continue;

	if ( !pix && !(*r)->spacepoint->clusterList().second) continue;
	
	float Tz = (Z-(*r)->z())/dR;

	if (Tz < m_dzdrmin || Tz > m_dzdrmax) continue;
	
	// Comparison with vertices Z coordinates
	//
	if (pix) {
	  float Zo = Z-R*Tz;
          if (!isZCompatible(Zo)) continue;
	}
	m_SP[Nb] = (*r); if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i!=NT; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R;
        if (dR<m_drmin || Rt<m_r3min) {rt[i]=r; continue;}
	if (Rt>m_r3max || dR>m_drmax) break;

	if ( (*r)->sur()==sur0) continue;

	float Tz = ((*r)->z()-Z)/dR;

	if (Tz < m_dzdrmin || Tz > m_dzdrmax) continue;

	// Comparison with vertices Z coordinates
	//
	if (pix) {
	  float Zo = Z-R*Tz; if (!isZCompatible(Zo)) continue;
	}
	m_SP[Nt] = (*r); if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if (!(Nt-Nb)) continue;

    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();

    float ax   = X/R;
    float ay   = Y/R;
    
    for (int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = m_SP[i];

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float x   = dx*ax+dy*ay ;
      float y   =-dx*ay+dy*ax ;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if (i < Nb) tz = -tz;

      m_Tz[i]   = tz                                            ;
      m_Zo[i]   = Z-R*tz                                        ;
      m_R [i]   = dr                                            ;
      m_U [i]   = x*r2                                          ;
      m_V [i]   = y*r2                                          ;
      m_Er[i]   = (covz0+sp->covz()+tz*tz*(covr0+sp->covr()))*r2;
    }

    float imc   = m_diver   ;
    float imcs  = m_diverpps;
    float ipt2  = m_ipt2    ;
    float K     = m_K       ;
    float K2    = K*K       ;
    float COF   = m_COF     ;
    float ipt2K = ipt2/K2   ;
    float ipt2C = ipt2*COF  ;
    float COFK  = COF*K2    ;
    covr0      *= 2.        ;
    covz0      *= 2.        ;
    
    // Three space points comparison
    //
    for (int b=0; b!=Nb; ++b) {
    
      const Trk::SpacePoint* SPb = m_SP[b]->spacepoint;
 
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;

      for (int t=Nb;  t!=Nt; ++t) {
	
	float Ts  = .5*(Tzb+m_Tz[t])                          ;
	float dt  =     Tzb-m_Tz[t]                           ;
	float dT  = dt*dt-Erb-m_Er[t]-m_R[t]*(Ts*Ts*Rb2r+Rb2z);
	if ( dT > ICSA) continue;
	float dU  = m_U[t]-Ub; if (dU == 0. ) continue;
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if (B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;
	float Im  = fabs((A-B*R)*R)                  ;
	
	if (pix) {
	  if (                                             Im > imc ) continue;
	  if (m_SP[t]->spacepoint->clusterList().second && Im > imcs) continue;
	}
	newOneSeed(SPb,SP0,m_SP[t]->spacepoint,Zob,Im);
      }
    }
    nseed += m_mapOneSeeds.size(); fillSeeds();
    if (nseed>=m_maxsize) {m_endlist=false; ++r0; m_rMin = r0;  return;} 
  }
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_BeamGas::isUsed(const Trk::SpacePoint* sp)
{
  const Trk::PrepRawData* d = sp->clusterList().first;
  if (!d || !m_assoTool->isUsed(*d)) return false;

  d = sp->clusterList().second;
  if (!d || m_assoTool->isUsed(*d)) return true;

  return false;
}

///////////////////////////////////////////////////////////////////
// New 3 space points seeds from one space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::newOneSeed
(const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const Trk::SpacePoint*& p3,const float& z,const float& q) 
{
  if (m_nOneSeeds < m_maxOneSize) {

    m_OneSeeds [m_nOneSeeds].erase     (  );
    m_OneSeeds [m_nOneSeeds].add       (p1);
    m_OneSeeds [m_nOneSeeds].add       (p2);
    m_OneSeeds [m_nOneSeeds].add       (p3);
    m_OneSeeds [m_nOneSeeds].setZVertex(static_cast<double>(z));
    m_mapOneSeeds.insert(std::make_pair(q,m_OneSeeds+m_nOneSeeds));
    ++m_nOneSeeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator 
      l = m_mapOneSeeds.rbegin();
    if ((*l).first <= q) return;
    
    InDet::SiSpacePointsSeed* s = (*l).second;
    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->add       (p3);
    s->setZVertex(static_cast<double>(z));
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator 
      i = m_mapOneSeeds.insert(std::make_pair(q,s));
	
    for (++i; i!=m_mapOneSeeds.end(); ++i) {
      if ((*i).second==s) {m_mapOneSeeds.erase(i); return;}
    }
  }
}

const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_BeamGas::next()
{
  if (m_i_seed==m_i_seede) {
    findNext();
    if (m_i_seed==m_i_seede) return nullptr;
  } 
  return(*m_i_seed++);
}
    
bool InDet::SiSpacePointsSeedMaker_BeamGas::isZCompatible  
(float& Zv)
{
  return Zv > m_zmin && Zv < m_zmax;
}
  
///////////////////////////////////////////////////////////////////
// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_BeamGas::newSpacePoint
(const Trk::SpacePoint*const& sp) 
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3];
  convertToBeamFrameWork(sp, r);

  if (m_i_spforseed!=m_l_spforseed.end()) {
    sps = (*m_i_spforseed++);
    sps->set(sp, r);
  } else {
    sps = new InDet::SiSpacePointForSeed(sp, r);
    m_l_spforseed.push_back(sps);
    m_i_spforseed = m_l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New 2 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::newSeed
(const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const float& z) 
{
  if (m_i_seede!=m_l_seeds.end()) {
    InDet::SiSpacePointsSeed* s = (*m_i_seede++);
    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->setZVertex(static_cast<double>(z));
  } else {
    m_l_seeds.push_back(new InDet::SiSpacePointsSeed(p1,p2,z));
    m_i_seede = m_l_seeds.end();
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::newSeed
(const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const Trk::SpacePoint*& p3,const float& z) 
{
  if (m_i_seede!=m_l_seeds.end()) {
    InDet::SiSpacePointsSeed* s = (*m_i_seede++);
    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->add       (p3);
    s->setZVertex(static_cast<double>(z));
  } else {
    m_l_seeds.push_back(new InDet::SiSpacePointsSeed(p1,p2,p3,z));
    m_i_seede = m_l_seeds.end();
  }
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_BeamGas::fillSeeds ()
{
  std::multimap<float,InDet::SiSpacePointsSeed*>::iterator 
    l  = m_mapOneSeeds.begin(),
    le = m_mapOneSeeds.end  ();

  for (; l!=le; ++l) {
    if (m_i_seede!=m_l_seeds.end()) {
      InDet::SiSpacePointsSeed* s = (*m_i_seede++);
      *s = *(*l).second;
    } else {
      m_l_seeds.push_back(new InDet::SiSpacePointsSeed(*(*l).second));
      m_i_seede = m_l_seeds.end();
    }
  }
}
  
