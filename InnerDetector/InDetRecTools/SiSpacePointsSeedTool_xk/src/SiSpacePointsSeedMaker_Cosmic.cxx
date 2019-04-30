/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <iostream>
#include <iomanip>

#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Cosmic.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_Cosmic::SiSpacePointsSeedMaker_Cosmic
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                         ,
    m_fieldServiceHandle("AtlasFieldSvc",n), 
    m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels"),
    m_spacepointsSCT("SCT_SpacePoints"),
    m_spacepointsPixel("PixelSpacePoints"),
    m_spacepointsOverlap("OverlapSpacePoints")
{
  m_useassoTool = false ;
  m_useOverlap= false   ;
  m_state     = 0       ;
  m_pixel     = true    ;
  m_sct       = true    ;
  m_state     = 0       ;
  m_nspoint   = 2       ;
  m_mode      = 0       ;
  m_nlist     = 0       ;
  m_endlist   = true    ;
  m_maxsize   = 100     ;
  m_ptmin     = 500.    ;
  m_drmin     = 5.      ;
  m_drmax     = 1000.   ; 
  m_rapcut    = 2.3     ;
  m_zmin      = -10000. ;
  m_zmax      = +10000. ;
  m_diver     = 10.     ;
  r_rmax      = 600.    ;
  r_rstep     =  5.     ;
  r_Sorted    = 0       ;
  r_index     = 0       ;
  r_map       = 0       ;    
  m_maxsizeSP = 1500    ;
  m_SP        = 0       ;
  m_R         = 0       ;
  m_Tz        = 0       ;
  m_Er        = 0       ;
  m_U         = 0       ;
  m_V         = 0       ;
  m_seeds     = 0       ;
  

//  m_spacepointsSCT         = 0                   ;
//  m_spacepointsPixel       = 0                   ;
//  m_spacepointsOverlap     = 0                   ;

  declareInterface<ISiSpacePointsSeedMaker>(this);

  declareProperty("AssociationTool"       ,m_assoTool              );
  declareProperty("usePixel"              ,m_pixel                 );
  declareProperty("useSCT"                ,m_sct                   );
  declareProperty("pTmin"                 ,m_ptmin                 );
  declareProperty("radMax"                ,r_rmax                  );
  declareProperty("radStep"               ,r_rstep                 );
  declareProperty("maxSize"               ,m_maxsize               );
  declareProperty("maxSizeSP"             ,m_maxsizeSP             );
  declareProperty("minZ"                  ,m_zmin                  );
  declareProperty("maxZ"                  ,m_zmax                  );
  declareProperty("mindRadius"            ,m_drmin                 );
  declareProperty("maxdRadius"            ,m_drmax                 );
  declareProperty("RapidityCut"           ,m_rapcut                );
  declareProperty("maxdImpact"            ,m_diver                 );
  declareProperty("maxdImpactPPS"         ,m_diverpps              );
  declareProperty("maxdImpactSSS"         ,m_diversss              );
  declareProperty("etaMin"                ,m_etamin                );
  declareProperty("etaMax"                ,m_etamax                );  
  declareProperty("checkEta"              ,m_checketa              );
  declareProperty("SpacePointsSCTName"    ,m_spacepointsSCT    );
  declareProperty("SpacePointsPixelName"  ,m_spacepointsPixel  );
  declareProperty("SpacePointsOverlapName",m_spacepointsOverlap);
  declareProperty("useOverlapSpCollection", m_useOverlap           );
  declareProperty("UseAssociationTool"    ,m_useassoTool           ); 
  declareProperty("MagFieldSvc"           , m_fieldServiceHandle   );
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_Cosmic::~SiSpacePointsSeedMaker_Cosmic()
{
  if(r_index ) delete [] r_index ;
  if(r_map   ) delete [] r_map   ; 
  if(r_Sorted) delete [] r_Sorted;

  // Delete space points for reconstruction
  //
  i_spforseed=l_spforseed.begin();
  for(; i_spforseed!=l_spforseed.end(); ++i_spforseed) {
    delete *i_spforseed;
  } 

  if(m_SP) delete [] m_SP;
  if(m_R ) delete [] m_R ;
  if(m_Tz) delete [] m_Tz;
  if(m_Er) delete [] m_Er;
  if(m_U ) delete [] m_U ;
  if(m_V ) delete [] m_V ;
  if(m_seeds) delete [] m_seeds;
 }

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_Cosmic::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

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
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_assoTool<<endreq; 
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endreq;
    }
  }

  // Build framework
  //
  buildFrameWork();
  if( m_ptmin < 300.) m_ptmin = 300.;

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endreq;
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_Cosmic::finalize()
{
  StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::newEvent (int)
{
  if(!m_pixel && !m_sct) return;

  erase();
  i_spforseed   = l_spforseed.begin();

  float irstep = 1./r_rstep;
  float errorsc[4] = {16.,16.,100.,16.};

  // Get pixels space points containers from store gate 
  //
  if(m_pixel) {

//    m_spacepointsPixel = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);

    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {
	  
	  float r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  if(m_useassoTool &&  isUsed(*sp)           ) continue;

	  int   ir = int(((*sp)->globalPosition().y()+r_rmax)*irstep);

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp),errorsc); 
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if(m_sct) {

//    m_spacepointsSCT = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsSCT,m_spacepointsSCTname);
    if(m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsSCT->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  float r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  if(m_useassoTool &&  isUsed(*sp)           ) continue;
	  
	  int   ir = int(((*sp)->globalPosition().y()+r_rmax)*irstep);

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp),errorsc); 
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if(m_useOverlap) {

//      m_spacepointsOverlap = 0;
//      sc = evtStore()->retrieve(m_spacepointsOverlap,m_spacepointsOverlapname);
      if(m_spacepointsOverlap.isValid()) {
	
	SpacePointOverlapCollection::const_iterator sp  = m_spacepointsOverlap->begin();
	SpacePointOverlapCollection::const_iterator spe = m_spacepointsOverlap->end  ();
	
	for (; sp!=spe; ++sp) {

	  float r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  if(m_useassoTool &&  isUsed(*sp)           ) continue;

	  int   ir = int(((*sp)->globalPosition().y()+r_rmax)*irstep);

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp),errorsc); 
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
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

  if(!m_pixel && !m_sct) return;

  erase();
  i_spforseed = l_spforseed.begin();

  float irstep = 1./r_rstep;
  float errorsc[4] = {16.,16.,100.,16.};

  // Get pixels space points containers from store gate 
  //
  if(m_pixel && vPixel.size()) {

//    m_spacepointsPixel   = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);
    
    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      std::vector<IdentifierHash>::const_iterator l = vPixel.begin(), le = vPixel.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {
	
	SpacePointContainer::const_iterator  w =  m_spacepointsPixel->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	for(; sp != spe; ++sp) {

	  float r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  if(m_useassoTool &&  isUsed(*sp)           ) continue;

	  int   ir = int(((*sp)->globalPosition().y()+r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp),errorsc); 
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if(m_sct && vSCT.size()) {

//    m_spacepointsSCT     = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsSCT,m_spacepointsSCTname);

    if(m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      std::vector<IdentifierHash>::const_iterator l = vSCT.begin(), le = vSCT.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {

	SpacePointContainer::const_iterator  w =  m_spacepointsSCT->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	for(; sp != spe; ++sp) {

	  float r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  if(m_useassoTool &&  isUsed(*sp)           ) continue;

	  int   ir = int(((*sp)->globalPosition().y()+r_rmax)*irstep);
	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp),errorsc); 
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
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
  int mode; lv.begin()!=lv.end() ?  mode = 1 : mode = 0;

  m_nseeds = 0; l_seeds.erase(l_seeds.begin(),l_seeds.end());
  
  if( !m_state || m_nspoint!=2 || m_mode!=mode || m_nlist) {

    m_state   = 1   ;
    m_nspoint = 2   ;
    m_nlist   = 0   ;
    m_mode    = mode;
    m_endlist = true;
    m_fNmin   = 0   ;
    m_zMin    = 0   ;
    production2Sp ();
  }

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::find3Sp(const std::list<Trk::Vertex>& lv) 
{
  int mode; lv.begin()!=lv.end() ? mode = 3 : mode = 2; 

  m_nseeds = 0; l_seeds.erase(l_seeds.begin(),l_seeds.end());

  if(!m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {

    m_state   = 1               ;
    m_nspoint = 3               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
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
  int mode; lv.begin()!=lv.end() ? mode = 6 : mode = 5; 
  
  if(!m_state || m_nspoint!=4 || m_mode!=mode || m_nlist) {

    m_state   = 1               ;
    m_nspoint = 4               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dump( MsgStream& out ) const
{
  if(m_nprint)
    return dumpEvent(out);
  else
    return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dumpConditions( MsgStream& out ) const
{
  int n = 42-m_spacepointsPixel.name().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 42-m_spacepointsSCT.name().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlap.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");


  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.name() <<s2
     <<std::endl;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.name()<<s3
     <<std::endl;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.name() <<s4
     <<std::endl;
  out<<"| usePixel                | "
     <<std::setw(12)<<m_pixel 
     <<"                              |"<<std::endl;
  out<<"| useSCT                  | "
     <<std::setw(12)<<m_sct 
     <<"                              |"<<std::endl;
  out<<"| maxSize                 | "
     <<std::setw(12)<<m_maxsize 
     <<"                              |"<<std::endl;
  out<<"| maxSizeSP               | "
     <<std::setw(12)<<m_maxsizeSP
     <<"                              |"<<std::endl;
  out<<"| pTmin  (mev)            | "
     <<std::setw(12)<<std::setprecision(5)<<m_ptmin
     <<"                              |"<<std::endl;
  out<<"| |rapidity|          <=  | " 
     <<std::setw(12)<<std::setprecision(5)<<m_rapcut
     <<"                              |"<<std::endl;
  out<<"| max radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<r_rmax 
     <<"                              |"<<std::endl;
  out<<"| radius step             | "
     <<std::setw(12)<<std::setprecision(5)<<r_rstep
     <<"                              |"<<std::endl;
  out<<"| min Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmin
     <<"                              |"<<std::endl;
  out<<"| max Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmax
     <<"                              |"<<std::endl;
  out<<"| min space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmin
     <<"                              |"<<std::endl;
  out<<"| max space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmax
     <<"                              |"<<std::endl;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_diver
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Cosmic::dumpEvent( MsgStream& out ) const
{
  //const float pi2    = 2.*M_PI;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| m_ns                    | "
     <<std::setw(12)<<m_ns
     <<"                              |"<<std::endl;
  out<<"| m_nsaz                  | "
     <<std::setw(12)<<m_nsaz
     <<"                              |"<<std::endl;
  out<<"| seeds                   | "
     <<std::setw(12)<<l_seeds.size()
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  /*
  if(m_outputlevel==0) return out; 

  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;

  out<<"|  Azimuthal  |    n   | z[ 0] | z[ 1] | z[ 2] | z[ 3] | z[4]  |";
  out<<" z[ 5] | z[ 6] | z[ 7] | z[ 8] | z[ 9] | z[10] |"
     <<std::endl;
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;
  
  float sF1 = pi2/float(m_fNmax+1);
  
  
  for(int f=0; f<=m_fNmax; ++f) {
    out<<"|  "
       <<std::setw(10)<<std::setprecision(4)<<sF1*float(f)<<" | "
       <<std::setw(6)<<rf_map[f]<<" |";
    for(int z=0; z!=11; ++z) {
      out<<std::setw(6)<<rfz_map[(f*11+z)]<<" |";
    }
    out<<std::endl;
  } 
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;
  out<<std::endl;
  */
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
  r_size = int((r_rmax+.1)/r_rstep)*2;
  r_Sorted = new std::list<InDet::SiSpacePointForSeed*>[r_size];
  r_index  = new int[r_size];
  r_map    = new int[r_size];  
  m_nr   = 0; for(int i=0; i!=r_size; ++i) {r_index[i]=0; r_map[i]=0;}

  // Build radius-azimuthal sorted containers
  //
  const float pi2     = 2.*M_PI            ;
  const int   NFmax    = 53                ;
  const float sFmax   = float(NFmax )/pi2;
  const float m_sFmin = 50./60.           ;

  m_sF        = 50./60. ; if(m_sF    >sFmax ) m_sF    = sFmax  ; else if(m_sF < m_sFmin) m_sF = m_sFmin;
  m_fNmax     = int(pi2*m_sF); if(m_fNmax >=NFmax) m_fNmax = NFmax-1;

  m_nrf   = 0; for(int i=0; i!= 53; ++i) {rf_index  [i]=0; rf_map  [i]=0;}

  // Build radius-azimuthal-Z sorted containers
  //
  m_nrfz  = 0; for(int i=0; i!=583; ++i) {rfz_index [i]=0; rfz_map [i]=0;}

  // Build maps for radius-azimuthal-Z sorted collections 
  //
  for(int f=0; f<=m_fNmax; ++f) {

    int fb = f-1; if(fb<0      ) fb=m_fNmax; 
    int ft = f+1; if(ft>m_fNmax) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [a]    = 3; rfz_t [a]    = 3;
      rfz_ib[a][0] = a; rfz_it[a][0] = a; 
      rfz_ib[a][1] = b; rfz_it[a][1] = b; 
      rfz_ib[a][2] = c; rfz_it[a][2] = c; 
      if     (z==5) {

	rfz_t [a]    = 9 ;   rfz_b [a]    = 9 ;
	rfz_it[a][3] = a+1;  rfz_ib[a][3] = a+1; 
	rfz_it[a][4] = b+1;  rfz_ib[a][4] = b+1; 
	rfz_it[a][5] = c+1;  rfz_ib[a][5] = c+1; 
	rfz_it[a][6] = a-1;  rfz_ib[a][6] = a-1; 
	rfz_it[a][7] = b-1;  rfz_ib[a][7] = b-1; 
	rfz_it[a][8] = c-1;  rfz_ib[a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [a]    = 6 ;   rfz_t [a]    = 6  ;
	rfz_ib[a][3] = a-1;  rfz_it[a][3] = a-1;
	rfz_ib[a][4] = b-1;  rfz_it[a][4] = b-1; 
	rfz_ib[a][5] = c-1;  rfz_it[a][5] = c-1; 

	if(z<10) {

	  rfz_t [a]    = 9 ;  rfz_b [a]    = 9  ;
	  rfz_it[a][6] = a+1; rfz_ib[a][6] = a+1; 
	  rfz_it[a][7] = b+1; rfz_ib[a][7] = b+1;
	  rfz_it[a][8] = c+1; rfz_ib[a][8] = c+1; 
	}
      }
      else {

	rfz_b [a]    = 6  ;   rfz_t [a]    = 6 ;
	rfz_ib[a][3] = a+1;   rfz_it[a][3] = a+1; 
	rfz_ib[a][4] = b+1;   rfz_it[a][4] = b+1; 
	rfz_ib[a][5] = c+1;   rfz_it[a][5] = c+1; 

	if(z>0) {

	  rfz_t [a]    = 9  ; rfz_b [a]    = 9 ;
	  rfz_it[a][6] = a-1; rfz_ib[a][6] = a-1; 
	  rfz_it[a][7] = b-1; rfz_ib[a][7] = b-1; 
	  rfz_it[a][8] = c-1; rfz_ib[a][8] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b[a]      = 12;  rfz_t [a]     = 12;
	rfz_ib[a][ 9] = a+2; rfz_it[a][ 9] = a+2; 
	rfz_ib[a][10] = b+2; rfz_it[a][10] = b+2;
	rfz_ib[a][11] = c+2; rfz_it[a][11] = c+2; 
      }
      else if(z==7) {
	rfz_b[a]      = 12;  rfz_t [a]     = 12;
	rfz_ib[a][ 9] = a-2; rfz_it[a][ 9] = a-2;  
	rfz_ib[a][10] = b-2; rfz_it[a][10] = b-2; 
	rfz_ib[a][11] = c-2; rfz_it[a][11] = c-2; 
      }
    }
  }
  
  if(!m_SP) m_SP       = new InDet::SiSpacePointForSeed*[m_maxsizeSP];
  if(!m_R ) m_R        = new                      float[m_maxsizeSP];
  if(!m_Tz) m_Tz       = new                      float[m_maxsizeSP];
  if(!m_Er) m_Er       = new                      float[m_maxsizeSP];
  if(!m_U ) m_U        = new                      float[m_maxsizeSP]; 
  if(!m_V ) m_V        = new                      float[m_maxsizeSP];
  if(!m_seeds) m_seeds = new InDet::SiSpacePointsSeed   [m_maxsize+5];  
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::fillLists() 
{
  const float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeed*>::iterator r;
  
  for(int i=0; i!= r_size;  ++i) {

    if(!r_map[i]) continue;

    r = r_Sorted[i].begin();

    while(r!=r_Sorted[i].end()) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;

      int   f = int(F*m_sF); f<0 ? f = m_fNmax : f>m_fNmax ? f = 0 : f=f;
      f = 1;
      rf_Sorted[f].push_back(*r); if(!rf_map[f]++) rf_index[m_nrf++] = f;

      int z; float Z = (*r)->z();

      // Azimuthal angle and Z-coordinate sort
      //
      if(Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      }
      else     {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++] = n;
      r_Sorted[i].erase(r++);
    }
    r_map[i] = 0;
  }
  m_nr    = 0;
  m_state = 0;
}
 
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::erase()
{
  for(int i=0; i!=m_nr;    ++i) {
    int n = r_index[i]; r_map[n] = 0;
    r_Sorted[n].erase(r_Sorted[n].begin(),r_Sorted[n].end());
  }

  for(int i=0; i!=m_nrf;   ++i) {
    int n = rf_index[i]; rf_map[n] = 0;
    rf_Sorted[n].erase(rf_Sorted[n].begin(),rf_Sorted[n].end());
  }

  for(int i=0; i!=m_nrfz;  ++i) {
    int n = rfz_index[i]; rfz_map[n] = 0;
    rfz_Sorted[n].erase(rfz_Sorted[n].begin(),rfz_Sorted[n].end());
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
  if(m_nsaz<3) return;

  float K   = 0.;

  double f[3], gP[3] ={10.,10.,0.}; 

  if(m_fieldService->solenoidOn()) {
    m_fieldService->getFieldZR(gP,f); K = 2./(300.*f[2]);
  }
  if(!K) return production3SpWithoutField();

  float ipt = 100000000.; if(m_ptmin!=0.) ipt= 1./fabs(.9*m_ptmin);

  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::list<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[a]; ++i) {
	
	int an =  rfz_ib[a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[a]; ++i) {
	
	int an =  rfz_it[a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3Sp(rb,rbe,rt,rte,NB,NT,K,ipt);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds without magnetic field
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Cosmic::production3SpWithoutField()
{ 

  float ipt = 100000000.; if(m_ptmin!=0.) ipt= 1./fabs(.9*m_ptmin);
  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::list<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[a]; ++i) {
	
	int an =  rfz_ib[a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[a]; ++i) {
	
	int an =  rfz_it[a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpWithoutField(rb,rbe,rt,rte,NB,NT,ipt);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
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
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {
	
    bool              pix   = true; if((*r0)->spacepoint->clusterList().second) pix = false;
    float              R    = (*r0)->radius() ; 
    const Trk::Surface* sur0 = (*r0)->sur()  ;
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
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	
	float dy = (*r)->y()-Y ;
	if(-dy < m_drmin) break; 
	if(-dy > m_drmax || (*r)->sur()==sur0) continue;

	if(!pix && !(*r)->spacepoint->clusterList().second) continue;
	if( pix &&  (*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X;
	float dz = (*r)->z()-Z;

	float x  = dx*ax+dy*ay ;
	float y  =-dx*ay+dy*ax ;
	float r2 = 1./(x*x+y*y);
	float dr = sqrt(r2)    ;
	m_Tz[Nb]  =-dz*dr       ;
        
	if(m_Tz[Nb]<m_dzdrmin || m_Tz[Nb]>m_dzdrmax) continue;
	
	m_SP[Nb] = (*r); 
	m_U [Nb] = x*r2;
	m_V [Nb] = y*r2;
	m_Er[Nb] = (covz0+m_SP[Nb]->covz()+m_Tz[Nb]*m_Tz[Nb]*(covr0+m_SP[Nb]->covr()))*r2;
	m_R [Nb] = dr  ;
	if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:

    if(!Nb || Nb==m_maxsizeSP) continue;

    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float dy = (*r)->y()-Y ;
	if(dy < m_drmin) {rt[i]=r; continue;}
	if(dy>m_drmax) break;
	if((*r)->sur()==sur0) continue;

	if( pix &&  (*r)->spacepoint->clusterList().second) continue;
	if(!pix && !(*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X ;
	float dz = (*r)->z()-Z ;

	float x  = dx*ax+dy*ay ;
	float y  =-dx*ay+dy*ax ;
	float r2 = 1./(x*x+y*y);
	float dr = sqrt(r2)    ;
	m_Tz[Nt]  = dz*dr       ;
	if(m_Tz[Nt]<m_dzdrmin || m_Tz[Nt]>m_dzdrmax) continue;
	
	m_SP[Nt] = (*r);
	m_U [Nt] = x*r2;
	m_V [Nt] = y*r2;
	m_Er[Nt] = (covz0+m_SP[Nt]->covz()+m_Tz[Nt]*m_Tz[Nt]*(covr0+m_SP[Nt]->covr()))*r2;
	m_R [Nt] = dr  ;  
	if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:

    if(!(Nt-Nb)) continue;

    // Three space points comparison
    //
    for(int b=Nb-1; b>=0; --b) {
      
      float SA  = 1.+m_Tz[b]*m_Tz[b];
      
      for(int t=Nb;  t!=Nt; ++t) {

	float Ts = .5*(m_Tz[b]+m_Tz[t]);
	float dt =  m_Tz[b]-m_Tz[t]    ;
	float dT = dt*dt-m_Er[b]-m_Er[t]-2.*m_R[b]*m_R[t]*(Ts*Ts*covr0+covz0);
	if( dT > 0. && dT > (ipt*ipt)*COF*SA ) continue;
	float dU = m_U[t]-m_U[b]; if(dU == 0.) continue;
	float A  = (m_V[t]-m_V[b])/dU                  ;
	float B  =  m_V[t]-A*m_U[t]                    ;
	float S2 = 1.+A*A                              ;
	float S  = sqrt(S2)                            ;
	float BK = fabs(B*K)                           ;
	if(BK > ipt*S) continue                        ; // Momentum    cut
	dT       -= ((BK*BK)*COF*SA/S2)                ;
	if(dT > 0.) continue                           ; // Polar angle cut
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
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {
	
    bool                pix  = true; if((*r0)->spacepoint->clusterList().second) pix = false;
    const Trk::Surface* sur0 = (*r0)->sur()  ;
    float              X    = (*r0)->x()    ;
    float              Y    = (*r0)->y()    ;
    float              Z    = (*r0)->z()    ;
    float             covr0 = (*r0)->covr ();
    float             covz0 = (*r0)->covz ();

    int                 Nb   = 0;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	
	float dy = Y-(*r)->y() ;
	if( dy < m_drmin) break; 
	if( dy > m_drmax || (*r)->sur()==sur0) continue;

	if(!pix && !(*r)->spacepoint->clusterList().second) continue;
	if( pix &&  (*r)->spacepoint->clusterList().second) continue;

	float dx = X-(*r)->x()     ;
	float dz = Z-(*r)->z()     ;
	float r2 = 1./(dx*dx+dy*dy);
	float dr = sqrt(r2)        ;
	m_Tz[Nb]  = dz*dr           ;
	
	if(m_Tz[Nb]<m_dzdrmin || m_Tz[Nb]>m_dzdrmax) continue;

	m_SP[Nb] = (*r) ; 
	m_U [Nb] = dx*dr;
	m_V [Nb] = dy*dr;
	m_Er[Nb] = (covz0+m_SP[Nb]->covz()+m_Tz[Nb]*m_Tz[Nb]*(covr0+m_SP[Nb]->covr()))*r2;
	m_R [Nb] = dr   ;
	if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:

    if(!Nb || Nb==m_maxsizeSP) continue;

    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float dy = (*r)->y()-Y ;
	if(dy < m_drmin) {rt[i]=r; continue;}
	if(dy>m_drmax) break;
	if((*r)->sur()==sur0) continue;

	if( pix &&  (*r)->spacepoint->clusterList().second) continue;
	if(!pix && !(*r)->spacepoint->clusterList().second) continue;

	float dx = (*r)->x()-X     ;
	float dz = (*r)->z()-Z     ;
	float r2 = 1./(dx*dx+dy*dy);
	float dr = sqrt(r2)        ;
	m_Tz[Nt]  = dz*dr           ;

	if(m_Tz[Nt]<m_dzdrmin || m_Tz[Nt]>m_dzdrmax) continue;

	m_SP[Nt] = (*r) ; 
	m_U [Nt] = dx*dr;
	m_V [Nt] = dy*dr;
	m_Er[Nt] = (covz0+m_SP[Nt]->covz()+m_Tz[Nt]*m_Tz[Nt]*(covr0+m_SP[Nt]->covr()))*r2;
	m_R [Nt] = dr   ;
	if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:

    if(!(Nt-Nb)) continue;

    // Three space points comparison
    //
    for(int b=Nb-1; b>=0; --b) {
      
      float SA  = 1.+m_Tz[b]*m_Tz[b];

      for(int t=Nb;  t!=Nt; ++t) {

	// Azimuth angle cut
	//
	float dF = m_U[b]*m_U[t]+m_V[b]*m_V[t];
	if(dF < dFcut) continue;
	
	// Polar angle cut
	//
	float Ts = .5*(m_Tz[b]+m_Tz[t]);
	float dT =  m_Tz[b]-m_Tz[t]    ;
	dT        = dT*dT-m_Er[b]-m_Er[t]-2.*m_R[b]*m_R[t]*(Ts*Ts*covr0+covz0);
	dT       -= (ipt*ipt)*COF*SA    ;
	if( dT > 0. ) continue          ;

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
  const Trk::PrepRawData* d = sp->clusterList().first ; 
  if(!d || !m_assoTool->isUsed(*d)) return false;

  d = sp->clusterList().second;
  if(!d || m_assoTool->isUsed(*d)) return true;

  return false;
}
