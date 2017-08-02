/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

     
///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ITK
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
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITK.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ITK::SiSpacePointsSeedMaker_ITK
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n),    
    m_spacepointsSCT("SCT_SpacePoints"),
    m_spacepointsPixel("PixelSpacePoints"),
    m_spacepointsOverlap("OverlapSpacePoints"),
    m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels")
{

  m_useassoTool = false ;
  m_useOverlap= true    ;
  m_state     = 0       ;
  m_pixel     = true    ;
  m_sct       = true    ;
  m_trigger   = false   ;
  m_checketa  = false   ;
  m_dbm       = false   ;
  m_state     = 0       ;
  m_nspoint   = 2       ;
  m_mode      = 0       ;
  m_nlist     = 0       ;
  m_endlist   = true    ;
  m_maxsize   = 10000   ;
  m_ptmin     =  500.   ;
  m_etamin    = 0.      ; m_etamax     = 2.7 ;
  m_r1min     = 0.      ; m_r1minv     = 0.  ; 
  m_r1max     = 600.    ; m_r1maxv     = 60. ;
  m_r2min     = 0.      ; m_r2minv     = 70. ;
  m_r2max     = 600.    ; m_r2maxv     = 200.;
  m_r3min     = 0.      ;
  m_r3max     = 600.    ;
  m_drmin     = 5.      ; m_drminv     = 20. ;    
  m_drmax     = 300.    ;    
  m_rapcut    = 2.7     ;
  m_zmin      = -250.   ;
  m_zmax      = +250.   ;
  m_dzver     = 5.      ;
  m_dzdrver   = .02     ;
  m_diver     = 10.     ;
  m_diverpps  = 5.      ;
  m_diversss  =  20     ;
  m_divermax  =  20.    ;
  m_dazmax    = .02     ;
  r_rmax      = 1100.   ;
  r_rmin      = 0.      ;
  m_umax      = 0.      ;
  r_rstep     =  2.     ;
  m_dzmaxPPP  = 600.    ; 
  r_Sorted    = 0       ;
  r_index     = 0       ;
  r_map       = 0       ;    
  m_maxsizeSP = 5000    ;
  m_maxOneSize= 5       ;
  m_SP        = 0       ;
  m_R         = 0       ;
  m_Tz        = 0       ;
  m_Er        = 0       ;
  m_U         = 0       ;
  m_V         = 0       ;
  m_X         = 0       ;
  m_Y         = 0       ;
  m_L         = 0       ;
  m_Im        = 0       ;
  m_OneSeeds  = 0       ;
  m_seedOutput= 0       ;
  m_maxNumberVertices = 99;
  
  m_radiusPPSmin = 200. ;
  m_radiusPPSmax = 600. ;
  m_rapydityPPSmax = 2.6;
  m_iminPPS        = 0  ;
  m_imaxPPS        = 1  ;

  m_xbeam[0]  = 0.      ; m_xbeam[1]= 1.; m_xbeam[2]=0.; m_xbeam[3]=0.;
  m_ybeam[0]  = 0.      ; m_ybeam[1]= 0.; m_ybeam[2]=1.; m_ybeam[3]=0.;
  m_zbeam[0]  = 0.      ; m_zbeam[1]= 0.; m_zbeam[2]=0.; m_zbeam[3]=1.;
  m_beamconditions         = "BeamCondSvc"       ;

  declareInterface<ISiSpacePointsSeedMaker>(this);

  declareProperty("AssociationTool"       ,m_assoTool              );
  declareProperty("usePixel"              ,m_pixel                 );
  declareProperty("useSCT"                ,m_sct                   );
  declareProperty("checkEta"              ,m_checketa              );
  declareProperty("useDBM"                ,m_dbm                   );
  declareProperty("etaMin"                ,m_etamin                );
  declareProperty("etaMax"                ,m_etamax                );  
  declareProperty("pTmin"                 ,m_ptmin                 );
  declareProperty("radMax"                ,r_rmax                  );
  declareProperty("radMin"                ,r_rmin                  );
  declareProperty("radStep"               ,r_rstep                 );
  declareProperty("maxSize"               ,m_maxsize               );
  declareProperty("maxSizeSP"             ,m_maxsizeSP             );
  declareProperty("minZ"                  ,m_zmin                  );
  declareProperty("maxZ"                  ,m_zmax                  );
  declareProperty("minRadius1"            ,m_r1min                 );
  declareProperty("minRadius2"            ,m_r2min                 );
  declareProperty("minRadius3"            ,m_r3min                 );
  declareProperty("maxRadius1"            ,m_r1max                 );
  declareProperty("maxRadius2"            ,m_r2max                 );
  declareProperty("maxRadius3"            ,m_r3max                 );
  declareProperty("mindRadius"            ,m_drmin                 );
  declareProperty("maxdRadius"            ,m_drmax                 );
  declareProperty("minVRadius1"           ,m_r1minv                );
  declareProperty("maxVRadius1"           ,m_r1maxv                );
  declareProperty("minVRadius2"           ,m_r2minv                );
  declareProperty("maxVRadius2"           ,m_r2maxv                );
  declareProperty("RapidityCut"           ,m_rapcut                );
  declareProperty("maxdZver"              ,m_dzver                 );
  declareProperty("maxdZdRver"            ,m_dzdrver               );
  declareProperty("maxdImpact"            ,m_diver                 );
  declareProperty("maxdImpactPPS"         ,m_diverpps              );
  declareProperty("maxdImpactSSS"         ,m_diversss              );
  declareProperty("maxdImpactForDecays"   ,m_divermax              );
  declareProperty("minSeedsQuality"       ,m_umax                  );
  declareProperty("dZmaxForPPPSeeds"      ,m_dzmaxPPP              );
  declareProperty("maxSeedsForSpacePoint" ,m_maxOneSize            );
  declareProperty("maxNumberVertices"     ,m_maxNumberVertices     );
  declareProperty("SpacePointsSCTName"    ,m_spacepointsSCT        );
  declareProperty("SpacePointsPixelName"  ,m_spacepointsPixel      );
  declareProperty("SpacePointsOverlapName",m_spacepointsOverlap    );
  declareProperty("BeamConditionsService" ,m_beamconditions        ); 
  declareProperty("useOverlapSpCollection",m_useOverlap            );
  declareProperty("UseAssociationTool"    ,m_useassoTool           ); 
  declareProperty("MagFieldSvc"           ,m_fieldServiceHandle    );

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ITK::~SiSpacePointsSeedMaker_ITK()
{
  if(r_index ) delete [] r_index ;
  if(r_map   ) delete [] r_map   ; 
  if(r_Sorted) delete [] r_Sorted;

  // Delete seeds
  //
  for(i_seed=l_seeds.begin(); i_seed!=l_seeds.end (); ++i_seed) {
    delete *i_seed;
  }
  // Delete space points for reconstruction
  //
  i_spforseed=l_spforseed.begin();
  for(; i_spforseed!=l_spforseed.end(); ++i_spforseed) {
    delete *i_spforseed;
  } 
  if(m_seedOutput) delete m_seedOutput; 

  if(m_SP) delete [] m_SP;
  if(m_R ) delete [] m_R ;
  if(m_X ) delete [] m_X ;
  if(m_Y ) delete [] m_Y ;
  if(m_Tz) delete [] m_Tz;
  if(m_Er) delete [] m_Er;
  if(m_U ) delete [] m_U ;
  if(m_V ) delete [] m_V ;
  if(m_L ) delete [] m_L ;
  if(m_Im) delete [] m_Im ;
  if(m_OneSeeds) delete [] m_OneSeeds;
 }

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITK::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get beam geometry
  //
  p_beam = 0;
  if(m_beamconditions!="") {
    sc = service(m_beamconditions,p_beam);
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

  if(r_rmax < 1100.) r_rmax = 1100.; 

  // Build framework
  //
  buildFrameWork();
  m_CmSp .reserve(500); 
  m_CmSpn.reserve(500); 

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
  m_umax = 100.-fabs(m_umax)*300.;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITK::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newEvent(int iteration) 
{
  m_iteration0 = iteration;
  m_trigger = false;
  if(!m_pixel && !m_sct) return; 

  iteration <=0 ? m_iteration = 0 : m_iteration = iteration;
  erase();
  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;

  if(!m_iteration) {
    buildBeamFrameWork();

    double f[3], gP[3] ={10.,10.,0.}; 
    if(m_fieldService->solenoidOn()) {
      m_fieldService->getFieldZR(gP,f); m_K = 2./(300.*f[2]);
    }
    else m_K = 2./(300.* 5. );

    m_ipt2K     = m_ipt2/(m_K*m_K);
    m_ipt2C     = m_ipt2*m_COF    ;
    m_COFK      = m_COF*(m_K*m_K) ;  
    i_spforseed = l_spforseed.begin();
  }
  else {
    r_first = 0; fillLists(); return;
  }

  m_checketa = m_dzdrmin > 1.;

  float irstep = 1./r_rstep;
  int   irmax  = r_size-1  ;
  for(int i=0; i!=m_nr; ++i) {int n = r_index[i]; r_map[n] = 0; r_Sorted[n].clear();}
  m_ns = m_nr = 0;

  // Get pixels space points containers from store gate 
  //
  r_first = 0;
  if(m_pixel) {

    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax || (*sp)->r() < r_rmin ) continue;
	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); if(!sps) continue;
	  int   ir = int(sps->radius()*irstep); if(ir>irmax) continue;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  if(ir > r_first) r_first = ir;
	  ++m_ns;
	}
      }
    }
    ++r_first;
  }

  // Get sct space points containers from store gate 
  //
  if(m_sct) {

    if(m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsSCT->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax || (*sp)->r() < r_rmin ) continue;

	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) continue;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if(m_useOverlap) {

      if(m_spacepointsOverlap.isValid()) {
	
	SpacePointOverlapCollection::const_iterator sp  = m_spacepointsOverlap->begin();
	SpacePointOverlapCollection::const_iterator spe = m_spacepointsOverlap->end  ();
	
	for (; sp!=spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax || (*sp)->r() < r_rmin) continue;

	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) continue;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  if(iteration < 0) r_first = 0;
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{
  m_iteration  = 0    ;
  m_trigger    = false;
  erase();
  if(!m_pixel && !m_sct) return;

  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;

  buildBeamFrameWork();

  double f[3], gP[3] ={10.,10.,0.}; 

  if(m_fieldService->solenoidOn()) {
      m_fieldService->getFieldZR(gP,f); m_K = 2./(300.*f[2]);
    }
  else m_K = 2./(300.* 5. );

  m_ipt2K     = m_ipt2/(m_K*m_K);
  m_ipt2C     = m_ipt2*m_COF    ;
  m_COFK      = m_COF*(m_K*m_K) ;  

  i_spforseed = l_spforseed.begin();

  float irstep = 1./r_rstep;
  int   irmax  = r_size-1  ;

  r_first      = 0         ;
  m_checketa   = false     ;

  for(int i=0; i!=m_nr; ++i) {int n = r_index[i]; r_map[n] = 0; r_Sorted[n].clear();}
  m_ns = m_nr = 0;

  // Get pixels space points containers from store gate 
  //
  if(m_pixel && vPixel.size()) {
    
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

	  float r = (*sp)->r(); if(r > r_rmax || r < r_rmin) continue;
	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); 
	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
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

	  float r = (*sp)->r(); if(r > r_rmax || r < r_rmin) continue;
	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp));
	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
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

void InDet::SiSpacePointsSeedMaker_ITK::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor& IRD)
{
  newRegion(vPixel,vSCT);
  m_trigger = true;

  double dzdrmin = 1./tan(2.*atan(exp(-IRD.etaMinus())));
  double dzdrmax = 1./tan(2.*atan(exp(-IRD.etaPlus ())));
 
  m_zminB        = IRD.zedMinus()-m_zbeam[0];    // min bottom Z
  m_zmaxB        = IRD.zedPlus ()-m_zbeam[0];    // max bottom Z
  m_zminU        = m_zminB+550.*dzdrmin;
  m_zmaxU        = m_zmaxB+550.*dzdrmax;
  double fmax    = IRD.phiPlus ();
  double fmin    = IRD.phiMinus();
  if(fmin > fmax) fmin-=(2.*M_PI);
  m_ftrig        = (fmin+fmax)*.5;
  m_ftrigW       = (fmax-fmin)*.5;
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::find2Sp(const std::list<Trk::Vertex>& lv) 
{
  m_zminU     = m_zmin;
  m_zmaxU     = m_zmax;

  int mode; lv.begin()!=lv.end() ?  mode = 1 : mode = 0;
  bool newv = newVertices(lv);
  
  if(newv || !m_state || m_nspoint!=2 || m_mode!=mode || m_nlist) {

    i_seede   = l_seeds.begin();
    m_state   = 1   ;
    m_nspoint = 2   ;
    m_nlist   = 0   ;
    m_mode    = mode;
    m_endlist = true;
    m_fvNmin  = 0   ;
    m_fNmin   = 0   ;
    production2Sp ();
  }
  i_seed  = l_seeds.begin();
  
  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::find3Sp(const std::list<Trk::Vertex>& lv) 
{
  m_zminU     = m_zmin;
  m_zmaxU     = m_zmax;

  int mode; lv.begin()!=lv.end() ? mode = 3 : mode = 2; 
  bool newv = newVertices(lv);

  if(newv || !m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {
    i_seede   = l_seeds.begin() ;
    m_state   = 1               ;
    m_nspoint = 3               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fvNmin  = 0               ;
    m_fNmin   = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::find3Sp(const std::list<Trk::Vertex>& lv,const double* ZVertex) 
{
  m_zminU     = ZVertex[0]; if(m_zminU < m_zmin) m_zminU = m_zmin; 
  m_zmaxU     = ZVertex[1]; if(m_zmaxU > m_zmax) m_zmaxU = m_zmax;

  int mode; lv.begin()!=lv.end() ? mode = 3 : mode = 2; 
  bool newv = newVertices(lv);

  if(newv || !m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {
    i_seede   = l_seeds.begin() ;
    m_state   = 1               ;
    m_nspoint = 3               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fvNmin  = 0               ;
    m_fNmin   = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::findVSp (const std::list<Trk::Vertex>& lv)
{
  m_zminU     = m_zmin;
  m_zmaxU     = m_zmax;

  int mode; lv.begin()!=lv.end() ? mode = 6 : mode = 5; 
  bool newv = newVertices(lv);
  
  if(newv || !m_state || m_nspoint!=4 || m_mode!=mode || m_nlist) {

    i_seede   = l_seeds.begin() ;
    m_state   = 1               ;
    m_nspoint = 4               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fvNmin  = 0               ;
    m_fNmin   = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITK::dump( MsgStream& out ) const
{
  if(m_nprint)  return dumpEvent(out); return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITK::dumpConditions( MsgStream& out ) const
{
  int n = 42-m_spacepointsPixel.name().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 42-m_spacepointsSCT.name().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlap.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 42-m_beamconditions.size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");


  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.name() <<s2
     <<std::endl;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.name()<<s3
     <<std::endl;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.name()<<s4
     <<std::endl;
  out<<"| BeamConditionsService   | "<<m_beamconditions<<s5
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
  out<<"| min radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<r_rmin 
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
  out<<"| min radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1min
     <<"                              |"<<std::endl;
  out<<"| min radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2min
     <<"                              |"<<std::endl;
  out<<"| min radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3min
     <<"                              |"<<std::endl;
  out<<"| max radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(4)<<m_r1max
     <<"                              |"<<std::endl;
  out<<"| max radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2max
     <<"                              |"<<std::endl;
  out<<"| max radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3max
     <<"                              |"<<std::endl;
  out<<"| min radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1minv
     <<"                              |"<<std::endl;
  out<<"| min radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2minv
     <<"                              |"<<std::endl;
  out<<"| max radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1maxv
     <<"                              |"<<std::endl;
  out<<"| max radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2maxv
     <<"                              |"<<std::endl;
  out<<"| min space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmin
     <<"                              |"<<std::endl;
  out<<"| max space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmax
     <<"                              |"<<std::endl;
  out<<"| max dZ    impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzver 
     <<"                              |"<<std::endl;
  out<<"| max dZ/dR impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrver 
     <<"                              |"<<std::endl;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_diver
     <<"                              |"<<std::endl;
  out<<"| max       impact pps    | "
     <<std::setw(12)<<std::setprecision(5)<<m_diverpps
     <<"                              |"<<std::endl;
  out<<"| max       impact sss    | "
    <<std::setw(12)<<std::setprecision(5)<<m_diversss
    <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Beam X center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[0]
     <<"                              |"<<std::endl;
  out<<"| Beam Y center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[0]
     <<"                              |"<<std::endl;
  out<<"| Beam Z center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[0]
     <<"                              |"<<std::endl;
  out<<"| Beam X-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[3]
     <<"      |"<<std::endl;
  out<<"| Beam Y-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[3]
     <<"      |"<<std::endl;
  out<<"| Beam Z-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[3]
     <<"      |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITK::dumpEvent( MsgStream& out ) const
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
  out<<"| m_nsazv                 | "
     <<std::setw(12)<<m_nsazv
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

std::ostream& InDet::SiSpacePointsSeedMaker_ITK::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiSpacePointsSeedMaker_ITK& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiSpacePointsSeedMaker_ITK& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::findNext () 
{
  if(m_endlist) return;

  i_seede = l_seeds.begin();

  if     (m_mode==0 || m_mode==1) production2Sp ();
  else if(m_mode==2 || m_mode==3) production3Sp ();
  else if(m_mode==5 || m_mode==6) production3Sp ();

  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin(); 
  ++m_nlist;
}                       

///////////////////////////////////////////////////////////////////
// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITK::newVertices(const std::list<Trk::Vertex>& lV)
{
  unsigned int s1 = l_vertex.size(); 
  unsigned int s2 = lV      .size(); 

  m_isvertex = false;
  if(s1==0 && s2==0) return false;

  std::list<Trk::Vertex>::const_iterator v;
  l_vertex.clear();
  if(s2 == 0) return false;

  m_isvertex = true;
  for(v=lV.begin(); v!=lV.end(); ++v) {l_vertex.insert(float((*v).position().z()));}

  m_zminU = (*l_vertex. begin())-20.; if( m_zminU < m_zmin) m_zminU = m_zmin;
  m_zmaxU = (*l_vertex.rbegin())+20.; if( m_zmaxU > m_zmax) m_zmaxU = m_zmax;

  return false;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::buildFrameWork() 
{
  m_ptmin     = fabs(m_ptmin);  
  
  if(m_ptmin < 100.) m_ptmin = 100.;

  if(m_diversss < m_diver   ) m_diversss = m_diver   ; 
  if(m_diverpps < m_diver   ) m_diverpps = m_diver   ;
  if(m_divermax < m_diversss) m_divermax = m_diversss;

  if(fabs(m_etamin) < .1) m_etamin = -m_etamax ;
  m_dzdrmax0  = 1./tan(2.*atan(exp(-m_etamax)));
  m_dzdrmin0  = 1./tan(2.*atan(exp(-m_etamin)));

  m_dzdrmaxPPS= 1./tan(2.*atan(exp(-m_rapydityPPSmax)));
  
  m_COF       =  134*.05*9.                    ;
  m_ipt       = 1./fabs(m_ptmin)               ;
  m_ipt2      = m_ipt*m_ipt                    ;
  m_K         = 0.                             ;

  m_ns = m_nsaz = m_nsazv = m_nr = m_nrfz = m_nrfzv = 0;

  // Build radius sorted containers
  //
  r_size = int((r_rmax+.1)/r_rstep);
  r_Sorted = new std::list<InDet::SiSpacePointForSeedITK*>[r_size];
  r_index  = new int[r_size];
  r_map    = new int[r_size];  
  m_nr   = 0; for(int i=0; i!=r_size; ++i) {r_index[i]=0; r_map[i]=0;}

  m_Bot.reserve(1000);
  m_Top.reserve(1000);

  // Build radius-azimuthal sorted containers
  //
  const float pi2     = 2.*M_PI            ;
  float ptm = 400.; if(m_ptmin < ptm) ptm = m_ptmin;

  int   NFmax    = 200            ;

  // Build radius-azimuthal-Z sorted containers
  //
  m_nrfz  = 0; for(int i=0; i!=2211; ++i) {rfz_index [i]=0; rfz_map [i]=0;}

  m_fNmax[0] = int(pi2/AzimuthalStep(m_ptmin,m_diversss,400.,1000.)); 
  if(m_fNmax[0] > NFmax) m_fNmax[0] = NFmax;  
  if(m_fNmax[0] < 10   ) m_fNmax[0] = 10   ;  
  m_sF[0] = float((m_fNmax[0]+1))/pi2;
  
  // Build maps for radius-azimuthal-Z sorted collections for SSS seeds
  //
  for(int f=0; f<=m_fNmax[0]; ++f) {

    int fb = f-1; if(fb<0         ) fb=m_fNmax[0]; 
    int ft = f+1; if(ft>m_fNmax[0]) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [0][a]    = 3; rfz_t [0][a]    = 3;
      rfz_ib[0][a][0] = a; rfz_it[0][a][0] = a; 
      rfz_ib[0][a][1] = b; rfz_it[0][a][1] = b; 
      rfz_ib[0][a][2] = c; rfz_it[0][a][2] = c; 
      if     (z==5) {

	rfz_t [0][a]    = 9 ;
	rfz_it[0][a][3] = a+1; 
	rfz_it[0][a][4] = b+1; 
	rfz_it[0][a][5] = c+1; 
	rfz_it[0][a][6] = a-1; 
	rfz_it[0][a][7] = b-1; 
	rfz_it[0][a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [0][a]    = 6 ;
	rfz_ib[0][a][3] = a-1; 
	rfz_ib[0][a][4] = b-1; 
	rfz_ib[0][a][5] = c-1; 

	if(z<10) {

	  rfz_t [0][a]    = 6 ;
	  rfz_it[0][a][3] = a+1; 
	  rfz_it[0][a][4] = b+1; 
	  rfz_it[0][a][5] = c+1; 
	}
      }
      else {

	rfz_b [0][a]    = 6 ;
	rfz_ib[0][a][3] = a+1; 
	rfz_ib[0][a][4] = b+1; 
	rfz_ib[0][a][5] = c+1; 

	if(z>0) {

	  rfz_t [0][a]    = 6 ;
	  rfz_it[0][a][3] = a-1; 
	  rfz_it[0][a][4] = b-1; 
	  rfz_it[0][a][5] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b [0][a]    = 9;
	rfz_ib[0][a][6] = a+2; 
	rfz_ib[0][a][7] = b+2; 
	rfz_ib[0][a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b [0][a]      = 9;
	rfz_ib[0][a][6] = a-2; 
	rfz_ib[0][a][7] = b-2; 
	rfz_ib[0][a][8] = c-2; 
      }
    }
  }
  m_fNmax[1] = int(pi2/AzimuthalStep(m_ptmin,m_diver,40.,320.));
  if(m_fNmax[1] > NFmax) m_fNmax[1] = NFmax; 
  if(m_fNmax[1] < 10   ) m_fNmax[1] = 10   ;  
  m_sF[1] = float(m_fNmax[1]+1)/pi2;

  // Build maps for radius-azimuthal-Z sorted collections for PPP seeds
  //
  for(int f=0; f<=m_fNmax[1]; ++f) {

    int fb = f-1; if(fb<0         ) fb=m_fNmax[1]; 
    int ft = f+1; if(ft>m_fNmax[1]) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [1][a]    = 3; rfz_t [1][a]    = 3;
      rfz_ib[1][a][0] = a; rfz_it[1][a][0] = a; 
      rfz_ib[1][a][1] = b; rfz_it[1][a][1] = b; 
      rfz_ib[1][a][2] = c; rfz_it[1][a][2] = c; 
      if     (z==5) {

	rfz_t [1][a]    = 9 ;
	rfz_it[1][a][3] = a+1; 
	rfz_it[1][a][4] = b+1; 
	rfz_it[1][a][5] = c+1; 
	rfz_it[1][a][6] = a-1; 
	rfz_it[1][a][7] = b-1; 
	rfz_it[1][a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [1][a]    = 6 ;
	rfz_ib[1][a][3] = a-1; 
	rfz_ib[1][a][4] = b-1; 
	rfz_ib[1][a][5] = c-1; 

	if(z<10) {

	  rfz_t [1][a]    = 6 ;
	  rfz_it[1][a][3] = a+1; 
	  rfz_it[1][a][4] = b+1; 
	  rfz_it[1][a][5] = c+1; 
	}
      }
      else {

	rfz_b [1][a]    = 6 ;
	rfz_ib[1][a][3] = a+1; 
	rfz_ib[1][a][4] = b+1; 
	rfz_ib[1][a][5] = c+1; 

	if(z>0) {

	  rfz_t [1][a]    = 6 ;
	  rfz_it[1][a][3] = a-1; 
	  rfz_it[1][a][4] = b-1; 
	  rfz_it[1][a][5] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b [1][a]    = 9;
	rfz_ib[1][a][6] = a+2; 
	rfz_ib[1][a][7] = b+2; 
	rfz_ib[1][a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b [1][a]      = 9;
	rfz_ib[1][a][6] = a-2; 
	rfz_ib[1][a][7] = b-2; 
	rfz_ib[1][a][8] = c-2; 
      }
    }
  }

  // Build maps for radius-azimuthal-Z sorted collections for PPS seeds
  //
  m_iminPPS = int(m_radiusPPSmin/r_rstep);
  m_imaxPPS = int(m_radiusPPSmax/r_rstep);

  m_fNmax[2] = int(pi2/AzimuthalStep(m_ptmin,m_diverpps,m_radiusPPSmin,m_radiusPPSmax));
  if(m_fNmax[2] > NFmax) m_fNmax[2] = NFmax; 
  if(m_fNmax[2] < 10   ) m_fNmax[2] = 10   ;  
  m_sF[2] = float(m_fNmax[2]+1)/pi2;

  for(int f=0; f<=m_fNmax[2]; ++f) {

    int fb = f-1; if(fb<0         ) fb=m_fNmax[2]; 
    int ft = f+1; if(ft>m_fNmax[2]) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [2][a]    = 3; rfz_t [2][a]    = 3;
      rfz_ib[2][a][0] = a; rfz_it[2][a][0] = a; 
      rfz_ib[2][a][1] = b; rfz_it[2][a][1] = b; 
      rfz_ib[2][a][2] = c; rfz_it[2][a][2] = c; 
      if     (z==5) {

	rfz_t [2][a]    = 9 ;
	rfz_it[2][a][3] = a+1; 
	rfz_it[2][a][4] = b+1; 
	rfz_it[2][a][5] = c+1; 
	rfz_it[2][a][6] = a-1; 
	rfz_it[2][a][7] = b-1; 
	rfz_it[2][a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [2][a]    = 6 ;
	rfz_ib[2][a][3] = a-1; 
	rfz_ib[2][a][4] = b-1; 
	rfz_ib[2][a][5] = c-1; 

	if(z<10) {

	  rfz_t [2][a]    = 6 ;
	  rfz_it[2][a][3] = a+1; 
	  rfz_it[2][a][4] = b+1; 
	  rfz_it[2][a][5] = c+1; 
	}
      }
      else {

	rfz_b [2][a]    = 6 ;
	rfz_ib[2][a][3] = a+1; 
	rfz_ib[2][a][4] = b+1; 
	rfz_ib[2][a][5] = c+1; 

	if(z>0) {

	  rfz_t [2][a]    = 6 ;
	  rfz_it[2][a][3] = a-1; 
	  rfz_it[2][a][4] = b-1; 
	  rfz_it[2][a][5] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b [2][a]    = 9;
	rfz_ib[2][a][6] = a+2; 
	rfz_ib[2][a][7] = b+2; 
	rfz_ib[2][a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b [2][a]      = 9;
	rfz_ib[2][a][6] = a-2; 
	rfz_ib[2][a][7] = b-2; 
	rfz_ib[2][a][8] = c-2; 
      }
    }
  }

  // Build radius-azimuthal-Z sorted containers for Z-vertices
  //
  const int   NFtmax  = 100               ;
  const float sFvmax = float(NFtmax)/pi2;
  m_sFv       = m_ptmin/120.  ; if(m_sFv   >sFvmax)  m_sFv    = sFvmax; 
  m_fvNmax    = int(pi2*m_sFv); if(m_fvNmax>=NFtmax) m_fvNmax = NFtmax-1;
  m_nrfzv = 0; for(int i=0; i!=300; ++i) {rfzv_index[i]=0; rfzv_map[i]=0;}

  // Build maps for radius-azimuthal-Z sorted collections for Z
  //
  for(int f=0; f<=m_fvNmax; ++f) {

    int fb = f-1; if(fb<0       ) fb=m_fvNmax; 
    int ft = f+1; if(ft>m_fvNmax) ft=0; 
    
    // For each azimuthal region loop through central Z regions
    //
    for(int z=0; z!=3; ++z) {
      
      int a  = f *3+z; 
      int b  = fb*3+z;
      int c  = ft*3+z;
      rfzv_n[a]    = 3;
      rfzv_i[a][0] = a;
      rfzv_i[a][1] = b;
      rfzv_i[a][2] = c;
      if     (z>1) {
	rfzv_n[a]    = 6;
	rfzv_i[a][3] = a-1;
	rfzv_i[a][4] = b-1;
	rfzv_i[a][5] = c-1;
      }
      else if(z<1) {
	rfzv_n[a]    = 6;
	rfzv_i[a][3] = a+1;
	rfzv_i[a][4] = b+1;
	rfzv_i[a][5] = c+1;
      }
    }
  }
  
  if(!m_SP) m_SP   = new InDet::SiSpacePointForSeedITK*[m_maxsizeSP];
  if(!m_R ) m_R    = new                          float[m_maxsizeSP];
  if(!m_X ) m_X    = new                          float[m_maxsizeSP];
  if(!m_Y ) m_Y    = new                          float[m_maxsizeSP];
  if(!m_Tz) m_Tz   = new                          float[m_maxsizeSP];
  if(!m_Er) m_Er   = new                          float[m_maxsizeSP];
  if(!m_U ) m_U    = new                          float[m_maxsizeSP]; 
  if(!m_V ) m_V    = new                          float[m_maxsizeSP];
  if(!m_L ) m_L    = new                          float[m_maxsizeSP];
  if(!m_Im) m_Im   = new                          float[m_maxsizeSP];
  if(!m_OneSeeds) m_OneSeeds  = new InDet::SiSpacePointsProSeedITK [m_maxOneSize];  

  if(!m_seedOutput) m_seedOutput = new InDet::SiSpacePointsSeed();

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();
}

////////////////////////////////////////////////////////////////////
// Claculation azimuthal angle step as function of
// pTmin(MeV),Imax,Rbegin(R1),Rstop(R2)(mm)
///////////////////////////////////////////////////////////////////

float InDet::SiSpacePointsSeedMaker_ITK::AzimuthalStep(float pTmin,float Imax,float R1,float R2)
{
  float Rm = pTmin/.6;
  float sI = fabs(asin(Imax/R1)-asin(Imax/R2));
  float sF = fabs(asin(R2/(2.*Rm))-asin(R1/(2.*Rm)));
  return ((sI+sF)/3.);
}

//////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::buildBeamFrameWork() 
{ 
  if(!p_beam) return;

  Amg::Vector3D cb =     p_beam->beamPos();
  double     tx = tan(p_beam->beamTilt(0));
  double     ty = tan(p_beam->beamTilt(1));

  double ph   = atan2(ty,tx);
  double th   = acos(1./sqrt(1.+tx*tx+ty*ty));
  double sint = sin(th);
  double cost = cos(th);
  double sinp = sin(ph);
  double cosp = cos(ph);
  
  m_xbeam[0] = float(cb.x())                  ; 
  m_xbeam[1] = float(cost*cosp*cosp+sinp*sinp);
  m_xbeam[2] = float(cost*sinp*cosp-sinp*cosp);
  m_xbeam[3] =-float(sint*cosp               );
  
  m_ybeam[0] = float(cb.y())                  ; 
  m_ybeam[1] = float(cost*cosp*sinp-sinp*cosp);
  m_ybeam[2] = float(cost*sinp*sinp+cosp*cosp);
  m_ybeam[3] =-float(sint*sinp               );
  
  m_zbeam[0] = float(cb.z())                  ; 
  m_zbeam[1] = float(sint*cosp)               ;
  m_zbeam[2] = float(sint*sinp)               ;
  m_zbeam[3] = float(cost)                    ;
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////
void  InDet::SiSpacePointsSeedMaker_ITK::convertToBeamFrameWork
(Trk::SpacePoint*const& sp,float* r) 
{
  r[0] = float(sp->globalPosition().x())-m_xbeam[0];
  r[1] = float(sp->globalPosition().y())-m_ybeam[0];
  r[2] = float(sp->globalPosition().z())-m_zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::fillLists() 
{
  if      (m_iteration == 0) fillListsSSS();
  else if (m_iteration == 1) fillListsPPP();
  else if (m_iteration == 2) fillListsPPS();
  m_state = 0;
}

///////////////////////////////////////////////////////////////////
// Initiate SSS space points seed maker
///////////////////////////////////////////////////////////////////

 void InDet::SiSpacePointsSeedMaker_ITK::fillListsSSS() 
 {
  const float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeedITK*>::iterator r,re;

  float sF   = m_sF   [0];
  int  fNmax = m_fNmax[0];
  int  ir0   = 0         ;
  int  irm   = 0         ;
  for(int i=r_first; i!=r_size;  ++i) {
    
    if(!r_map[i]) continue; r = r_Sorted[i].begin(); re = r_Sorted[i].end();
    
    if(!ir0) ir0 = i; irm = i; 
    
    for(; r!=re; ++r) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;
      int   f = int(F*sF); if(f<0) f=fNmax; else if(f>fNmax) f=0;

      // Azimuthal angle and Z-coordinate sort
      //
      float Z = (*r)->z(); int z;
      if(Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      }
      else     {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++]=n;
    }
  }
  m_RTmin = r_rstep*ir0+30. ;
  m_RTmax = r_rstep*irm-150.;
 }

///////////////////////////////////////////////////////////////////
// Initiate PPP space points seed maker
///////////////////////////////////////////////////////////////////

 void InDet::SiSpacePointsSeedMaker_ITK::fillListsPPP() 
 {
   const float pi2 = 2.*M_PI;
   std::list<InDet::SiSpacePointForSeedITK*>::iterator r,re;

   float sF   = m_sF   [1];
   int  fNmax = m_fNmax[1];
   int  ir0   = 0         ;
   int  irm   = 0         ;
  
   for(int i=r_first; i!=r_size;  ++i) {
     
     if(!r_map[i]) continue; r = r_Sorted[i].begin(); re = r_Sorted[i].end();
     
     if((*r)->spacepoint->clusterList().second) break;
     
     if(!ir0) ir0 = i; irm = i; 
     
     for(; r!=re; ++r) {
      
       // Azimuthal angle sort
       //
       float F = (*r)->phi(); if(F<0.) F+=pi2;
       int   f = int(F*sF); if(f<0) f=fNmax; else if(f>fNmax) f=0;
				    
       // Azimuthal angle and Z-coordinate sort
       //
       float Z = (*r)->z(); int z;
       
       if(Z>0.) {
	 Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
       }
       else     {
	 Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
       }
       int n(f*11+z); ++m_nsaz;
       rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++]=n;
     }
   }
   m_RTmin = r_rstep*ir0+10. ;
   m_RTmax = r_rstep*irm-10.;
 }

///////////////////////////////////////////////////////////////////
// Initiate PPS space points seed maker
///////////////////////////////////////////////////////////////////

 void InDet::SiSpacePointsSeedMaker_ITK::fillListsPPS() 
 {
  const float pi2 = 2.*M_PI;
  
  float Tpps  = 1./m_dzdrmaxPPS;
  float sF    = m_sF   [2]     ;
  int  fNmax  = m_fNmax[2]     ;
  int   ir0   = 0              ;
  int   irm   = 0              ;

  for(int i=m_iminPPS; i!=m_imaxPPS;  ++i) {

    if(!r_map[i]) continue;
    std::list<InDet::SiSpacePointForSeedITK*>::iterator r = r_Sorted[i].begin();
    
    if(!ir0) ir0 = i; irm = i; 
    
    std::list<InDet::SiSpacePointForSeedITK*>::iterator re = r_Sorted[i].end();

    for(; r!=re; ++r) {
      
 
      float Z = (*r)->z(); 
      if((*r)->radius() < (fabs(Z)-m_zmaxU)*Tpps) continue;
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;
      int   f = int(F*sF); if(f<0) f = fNmax; else if(f>fNmax) f=0;
      
      // Azimuthal angle and Z-coordinate sort
      //
      int z;				   
      if(Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      }
      else     {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++]=n;
    }
  }
  m_RTmin = r_rstep*ir0+10.;
  m_RTmax = r_rstep*irm-10.;
}
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::erase()
{
  for(int i=0; i!=m_nrfz;  ++i) {
    int n = rfz_index[i]; rfz_map[n] = 0;
    rfz_Sorted[n].clear();
  }
  
  for(int i=0; i!=m_nrfzv; ++i) {
    int n = rfzv_index[i]; rfzv_map[n] = 0;
    rfzv_Sorted[n].clear();
  }
  m_state = 0;
  m_nsaz  = 0;
  m_nsazv = 0;
  m_nrfz  = 0;
  m_nrfzv = 0;
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITK::isUsed(const Trk::SpacePoint* sp)
{
  const Trk::PrepRawData* d = sp->clusterList().first ; 
  if(!m_assoTool->isUsed(*d)) return false;

  d = sp->clusterList().second;
  if(!d || m_assoTool->isUsed(*d)) return true;

  return false;
}

///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production2Sp()
{
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3Sp()
{ 
  if(m_nsaz<3) return; 
  
  m_seeds.clear();
  m_CmSpn.clear();
 
  if     (m_iteration == 0) production3SpSSS();
  else if(m_iteration == 1) production3SpPPP();
  else if(m_iteration == 2) production3SpPPS();
  else return;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds SSS 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpSSS()
{ 
  const int   ZI[11]= {5,6,4,7,3,8,2,9,1,10,0};

  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  m_endlist = true;

  // Loop thorugh all azimuthal regions
  //
  for(int f(m_fNmin); f<=m_fNmax[0]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z(0); z!=11; ++z) {

      int a(f*11+ZI[z]);  if(!rfz_map[a]) continue;

      int NB(0), NT(0);
      for(int i(0); i!=rfz_b[0][a]; ++i) {
	
	int an(rfz_ib[0][a][i]);
	if(rfz_map[an]) {rb[NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();}
      } 
      if(!NB) continue;
      for(int i(0); i!=rfz_t[0][a]; ++i) {
	
	int an(rfz_it[0][a][i]);
	if(rfz_map[an]) {rt[NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();}
      } 
      production3SpSSS(rb,rbe,rt,rte,NB,NT,nseed);
    }
    if(nseed >= m_maxsize) {m_endlist=false; m_fNmin=f+1; return;} 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds PPP
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPP()
{ 
  const int   ZI[11]= {0,1,2,3,10,9,8,7,5,4,6};

  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  m_endlist = true;

  // Loop thorugh all azimuthal regions
  //
  for(int f(m_fNmin); f<=m_fNmax[1]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z(0); z!=11; ++z) {

      int a(f*11+ZI[z]);  if(!rfz_map[a]) continue;

      int NB(0), NT(0);
      for(int i(0); i!=rfz_b[1][a]; ++i) {
	
	int an(rfz_ib[1][a][i]); 
	if(rfz_map[an]) {rb[NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();}
      } 
      if(!NB) continue;
      for(int i(0); i!=rfz_t[1][a]; ++i) {
	
	int an(rfz_it[1][a][i]); 
	if(rfz_map[an]) {rt[NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();}
      } 
      production3SpPPP(rb,rbe,rt,rte,NB,NT,nseed);
    }
    if(nseed >= m_maxsize) {m_endlist=false; m_fNmin=f+1; return;} 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds PSS
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPS()
{ 
  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 
  
  m_endlist = true;

  // Loop thorugh all azimuthal regions
  //
  for(int f(m_fNmin); f<=m_fNmax[2]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z(0); z!=11; ++z) {

      int a(f*11+ZI[z]);  if(!rfz_map[a]) continue;

      int NB(0), NT(0);
      for(int i(0); i!=rfz_b[2][a]; ++i) {
	
	int an(rfz_ib[2][a][i]);
	if(rfz_map[an]) {rb[NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();}
      } 
      if(!NB) continue;
      for(int i(0); i!=rfz_t[2][a]; ++i) {
	
	int an(rfz_it[2][a][i]);
	if(rfz_map[an]) {rt[NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();}
      } 
      production3SpPPS(rb,rbe,rt,rte,NB,NT,nseed);
    }
    if(nseed >= m_maxsize) {m_endlist=false; m_fNmin=f+1; return;} 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 pixel space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPP
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],re0=rbe[0];
  
  for(; r0!=re0; ++r0) {if((*r0)->radius() > m_RTmin) break;}
  rt[0] = r0; ++rt[0];
  
  // Loop through all trigger space points
  //
  for(; r0!=re0; ++r0) {

    float R     =(*r0)->radius(); if(R       > m_RTmax) break;
    float Z     =(*r0)->     z(); if(fabs(Z) > 2700.  ) continue;
    float X     = (*r0)->    x();
    float Y     = (*r0)->    y();
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    float Ri    = 1./R          ;
    float ax    = X*Ri          ;
    float ay    = Y*Ri          ;
    float VR    = (m_diver*Ri)*Ri;
    int   Ntm   = 2; if(R > 140.) Ntm = 1; 

    // Top   links production
    //
    int Nt(0); m_Top.clear();
    for(int i(0); i!=NT; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rt[i], re = rte[i];

      for(; r!=re; ++r) {if(((*r)->radius()-R) >= m_drmin) break;} rt[i]=r;
      
      for(; r!=re; ++r) {
	
	InDet::SiSpacePointForSeedITK* sp = (*r);  
	float dR(sp->radius()-R); 

	// Comparison with vertices Z coordinates
	//
	float dz  = sp->z()-Z   ; if(fabs(Z*dR-R*dz) > m_zmaxU*dR) continue;  
	float dx  = sp->x()-X   ;
	float dy  = sp->y()-Y   ;
	float x   = dx*ax+dy*ay ;
	float y   = dy*ax-dx*ay ;
	float dxy = x*x+y*y     ;
	float r2  = 1./dxy      ;
	float u   = x*r2        ;
	float v   = y*r2        ;
	
	if(fabs(R*y) > m_diver*x) {

	  float V0; y < 0. ? V0 = VR : V0=-VR;
	  float A   = (v-V0)/(u+Ri)          ;
	  float B   = V0+A*Ri                ;
	  if((B*B) > (m_ipt2K*(1.+A*A))) continue;
	}
	float dr  = sqrt(r2)    ;
	float tz  = dz*dr       ; if(fabs(tz) > m_dzdrmax || Nt==m_maxsizeSP) continue;
	m_SP[Nt ] = sp          ;
	m_R [Nt ] = dr          ;
	m_U [Nt ] = u           ;
	m_V [Nt ] = v           ;
	float cz  = sp->covz()  ;
	float cr  = sp->covr()  ; 
	m_Er[Nt ] = ((covz0+cz)+(tz*tz)*(covr0+cr))*r2;
	m_L [Nt ] = sqrt(dxy+dz*dz); 
	m_Top.push_back(std::make_pair(tz,Nt++));
      }
    }
    if(Nt < Ntm || Nt==m_maxsizeSP) continue;

    // Bottom links production
    //
    int Nb(Nt); m_Bot.clear();

    for(int i(0); i!=NB; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rb[i], re = rbe[i];
 
      for(; r!=re; ++r) {

 	InDet::SiSpacePointForSeedITK* sp = (*r); 

	float dR(R-sp->radius()); if(dR < m_drmin) break;

	// Comparison with vertices Z coordinates
	//
	float dz  = Z-sp->z()   ; if(fabs(Z*dR-R*dz) > m_zmaxU*dR) continue;
	float dx  = sp->x()-X   ; 
	float dy  = sp->y()-Y   ;
	float x   = dx*ax+dy*ay ;
	float y   = dy*ax-dx*ay ;
	float r2  = 1./(x*x+y*y);
	float u   = x*r2        ;
	float v   = y*r2        ;
	
	if(fabs(R*y) > -m_diver*x) {

	  float V0; y > 0. ? V0 = VR : V0=-VR;
	  float A   = (v-V0)/(u+Ri)          ;
	  float B   = V0+A*Ri                ;
	  if((B*B) > (m_ipt2K*(1.+A*A))) continue;
	}
	float dr  = sqrt(r2)    ;
	float tz  = dz*dr       ; if(fabs(tz) > m_dzdrmax || Nb==m_maxsizeSP) continue;
	m_SP[Nb ] = sp          ;
	m_R [Nb ] = dr          ;
	m_U [Nb ] = u           ;
	m_V [Nb ] = v           ;
	float cz  = sp->covz()  ;
	float cr  = sp->covr()  ; 
	m_Er[Nb ] = ((covz0+cz)+(tz*tz)*(covr0+cr))*r2;
	m_Bot.push_back(std::make_pair(tz,Nb++));
     }
    }
    if(!(Nb-Nt)) continue; Nb-=Nt;

    std::sort(m_Bot.begin(),m_Bot.end(),comDzDrITK());
    std::sort(m_Top.begin(),m_Top.end(),comDzDrITK());

    covr0      *= .5;
    covz0      *= 2.;

    m_nOneSeeds = 0; m_mapOneSeeds.clear();
     
    // Three space points comparison
    //
    int it0 = 0;
    for(int ib(0); ib!=Nb; ++ib) {

      if(it0==Nt) break;

      float  Tzb  = m_Bot[ib].first ;
      int    b    = m_Bot[ib].second; 

      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float  CSA  = Tzb2*m_COFK  ;
      float ICSA  = Tzb2*m_ipt2C ;

      unsigned int Nc = 1; if(m_SP[b]->radius() > 140.) Nc = 0;

      for(int it(it0);  it!=Nt; ++it) {

	int  t    = m_Top[it].second;

	float DT  = Tzb-m_Top[it].first;
	float ST  = Tzb+m_Top[it].first;
	float dT  = (DT*DT-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*(ST*ST);

	if( dT > ICSA) {if(DT < 0.) break; it0 = it+1; continue;}

	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > m_ipt2K*S2) continue;
	if(dT*S2 > B2*CSA) {if(DT < 0.) break; it0 = it; continue;}

	float Im  = fabs((A-B*R)*R)                  ; 

	if(Im <= m_diver) {
	  m_Im[t] = Im; m_CmSpn.push_back(std::make_pair(B/sqrt(S2),t));
	}
      }
      if(m_CmSpn.size() > Nc) newOneSeedWithCurvaturesComparisonPPP(m_SP[b],(*r0),Z-R*Tzb); m_CmSpn.clear();
    }
    fillSeeds(); nseed+=m_fillOneSeeds;
  }
 }

///////////////////////////////////////////////////////////////////
// Production 3 SCT space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpSSS
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],re0=rbe[0];

  for(; r0!=re0; ++r0) {if((*r0)->radius() > m_RTmin) break;}
  rt[0] = r0; ++rt[0];

  // Loop through all trigger space points
  //
  for(; r0!=re0; ++r0) {

    float               R    = (*r0)->radius(); if(R       > m_RTmax) break;
    float               Z    = (*r0)->z()     ; if(fabs(Z) >  2700. ) continue;
    
    // Top links selection
    //
    int Nt(0);
    for(int i(0); i!=NT; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rt[i], re = rte[i];

      for(; r!=re; ++r) {if(((*r)->radius()-R) >= 20.) break;} rt[i]=r;
      
      for(; r!=re; ++r) {
	
	float dR((*r)->radius()-R); if(dR > m_drmax) break;

	// Comparison with vertices Z coordinates
	//
	float dz((*r)->z()-Z); 
 	if(fabs(dz) <= 900. && fabs(Z*dR-R*dz) <= m_zmaxU*dR) {m_SP[Nt]=(*r); if(Nt!=m_maxsizeSP) ++Nt;}
     }
    }
    if(!Nt || Nt==m_maxsizeSP) continue;

    // Bottom links selection
    //
    int Nb(Nt);
    for(int i(0); i!=NB; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rb[i], re = rbe[i];

      for(; r!=re; ++r) {if((R-(*r)->radius()) <= m_drmax) break;}  rb[i]=r;

      for(; r!=re; ++r) {
	
	float dR(R-(*r)->radius()); if(dR < 20.) break;

	// Comparison with vertices Z coordinates
	//
	float dz(Z-(*r)->z()); 
  	if(fabs(dz) <= 900. && fabs(Z*dR-R*dz) <= m_zmaxU*dR) {m_SP[Nb]=(*r); if(Nb!=m_maxsizeSP) ++Nb;}
     }
    }
    if(!(Nb-Nt)) continue; 

    float X     = (*r0)->x()    ;
    float Y     = (*r0)->y()    ;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    float ax    = X/R           ;
    float ay    = Y/R           ;

    // Top and bottom links production
    //
    for(int i(0); i!=Nb; ++i) {

      InDet::SiSpacePointForSeedITK* sp = m_SP[i];  

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float cvz = sp->covz()  ;
      float cvr = sp->covr()  ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(i >= Nt) tz = -tz;
      m_X [i]   = x           ;
      m_Y [i]   = y           ;
      m_Tz[i]   = tz          ;
      m_R [i]   = dr          ;
      m_U [i]   = x*r2        ;
      m_V [i]   = y*r2        ;
      m_Er[i]   = ((covz0+cvz)+(tz*tz)*(covr0+cvr))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;
    m_nOneSeeds = 0; m_mapOneSeeds.clear();
   
    // Three space points comparison
    //
    for(int b(Nt); b!=Nb; ++b) {
    
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*m_COFK  ;
      float ICSA  = Tzb2*m_ipt2C ;
      float Se    = 1./sTzb2     ;
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      for(int t(0);  t!=Nt; ++t) {

	// Trigger point
	//	
	float dU0   =  m_U[t]-Ub       ;  if(dU0 == 0.) continue; 
	float A0    = (m_V[t]-Vb)/dU0  ;
	float C0    = 1./sqrt(1.+A0*A0); 
	float S0    = A0*C0            ;
	float d0[3] = {Sx*C0-Sy*S0,Sx*S0+Sy*C0,Ce};  
	float rn[3];  if(!  (*r0)->coordinates(d0,rn)) continue;

	// Bottom  point
	//
	float B0    = 2.*(Vb-A0*Ub);
	float Cb    = (1.-B0*m_Y[b])*C0;
	float Sb    = (A0+B0*m_X[b])*C0;
	float db[3] = {Sx*Cb-Sy*Sb,Sx*Sb+Sy*Cb,Ce};  
	float rb[3];  if(!m_SP[b]->coordinates(db,rb)) continue;

	// Top     point
	//
	float Ct    = (1.-B0*m_Y[t])*C0;
	float St    = (A0+B0*m_X[t])*C0;
	float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Ce};  
	float rt[3];  if(!m_SP[t]->coordinates(dt,rt)) continue;

	float xb    = rb[0]-rn[0];
	float yb    = rb[1]-rn[1];
	float xt    = rt[0]-rn[0];
	float yt    = rt[1]-rn[1];
	float dz    = rt[2]-rn[2];
	float dxy   = xt*xt+yt*yt;

	float rb2   = 1./(xb*xb+yb*yb);
	float rt2   = 1./dxy          ;
	
	float tb    = (rn[2]-rb[2])*sqrt(rb2);
	float tz    = dz           *sqrt(rt2);

	float dT  = ((tb-tz)*(tb-tz)-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((tb+tz)*(tb+tz));
	if( dT > ICSA) continue;

	float Rn    = sqrt(rn[0]*rn[0]+rn[1]*rn[1]);
	float Ax    = rn[0]/Rn;
	float Ay    = rn[1]/Rn;

	float ub    = (xb*Ax+yb*Ay)*rb2;
	float vb    = (yb*Ax-xb*Ay)*rb2;
	float ut    = (xt*Ax+yt*Ay)*rt2;
	float vt    = (yt*Ax-xt*Ay)*rt2;
	
	float dU  = ut-ub; if(dU == 0.) continue;	
	float A   = (vt-vb)/dU;
	float S2  = 1.+A*A                           ;
	float B   = vb-A*ub                          ;
	float B2  = B*B                              ;
	if(B2  > m_ipt2K*S2 || dT*S2 > B2*CSA) continue;

	float Im  = fabs((A-B*Rn)*Rn)                ; 

	if(Im <= m_diversss) {
	  m_Im[t] = Im;
	  m_L [t] = sqrt(dxy+dz*dz); 
	  m_CmSpn.push_back(std::make_pair(B/sqrt(S2),t));
	}
      }
      if(!m_CmSpn.empty()) {newOneSeedWithCurvaturesComparisonSSS(m_SP[b],(*r0),Z-R*Tzb); m_CmSpn.clear();}
    }
    fillSeeds();  nseed += m_fillOneSeeds;
  } 
}

///////////////////////////////////////////////////////////////////
// Production PPS space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPS
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],re0=rbe[0];

  for(; r0!=re0; ++r0) {if((*r0)->radius() > m_RTmin) break;}
  rt[0] = r0; ++rt[0];

  // Loop through all trigger space points
  //
  for(; r0!=re0; ++r0) {

    if((*r0)->spacepoint->clusterList().second) break;

    float R     = (*r0)->radius();
    float Ri    = 1./R           ;
    float X     = (*r0)->x()     ;
    float Y     = (*r0)->y()     ;
    float Z     = (*r0)->z()     ;
    float covr0 = (*r0)->covr () ;
    float covz0 = (*r0)->covz () ;
    float ax    = X*Ri           ;
    float ay    = Y*Ri           ;
    float U0    =  -Ri           ;
    float VR = (m_diverpps*Ri)*Ri;
    
    // Top links selection
    //
    int Nt(0);
    for(int i(0); i!=NT; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rt[i], re = rte[i];

      for(; r!=re; ++r) {if((*r)->spacepoint->clusterList().second) break;} rt[i]=r;
     
      for(; r!=re; ++r) {

	float dR = (*r)->radius()-R; 
	if(fabs(Z*dR-R*((*r)->z()-Z )) <= m_zmaxU*dR) {m_SP[Nt]=(*r); if(Nt!=m_maxsizeSP) ++Nt;}
      }
    }
    if(Nt < 2 || Nt==m_maxsizeSP) continue;

    // Bottom links production
    //
    int Nb(Nt);
    for(int i(0); i!=NB; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rb[i], re = rbe[i];

      for(; r!=re; ++r) {
	
	float dR = R-(*r)->radius(); 
	if(dR < m_drmin) break;

	float dz = Z-(*r)->z();
	if(fabs(Z*dR-R*dz) > m_zmaxU*dR || fabs(dz) > m_dzdrmaxPPS*dR) continue;
	
	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float cvz= (*r)->covz();
	float cvr= (*r)->covr();
	float x  = dx*ax+dy*ay ; 
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(fabs(R*y) > -m_diverpps*x) {

	  float V0;   y > 0. ? V0 = VR : V0=-VR;
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (m_ipt2K*(1.+A*A))) continue;
	}

	float dr  = sqrt(r2)   ;
	float tz  = dz*dr      ; 
	m_X [Nb]  = x          ;
	m_Y [Nb]  = y          ;
	m_Tz[Nb]  = tz         ;
	m_R [Nb]  = dr         ;
	m_U [Nb]  = u          ;
	m_V [Nb]  = v          ;
	m_Er[Nb]  = ((covz0+cvz)+(tz*tz)*(covr0+cvr))*r2;
	m_SP[Nb]  = (*r); if(Nb!=m_maxsizeSP) ++Nb;
      }
    }
    if(!(Nb-Nt)) continue;

    // Top links production
    //
    for(int i(0); i!=Nt; ++i) {

      InDet::SiSpacePointForSeedITK* sp = m_SP[i];
  
      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float cvz = sp->covz()  ;
      float cvr = sp->covr()  ;
      float x   = dx*ax+dy*ay ; 
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float u   = x*r2        ;
      float v   = y*r2        ;
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; 
      m_X [i]   = x           ;
      m_Y [i]   = y           ;
      m_Tz[i]   = tz          ;
      m_R [i]   = dr          ;
      m_U [i]   = u           ;
      m_V [i]   = v           ;
      m_Er[i]   = ((covz0+cvz)+(tz*tz)*(covr0+cvr))*r2;
    }

    covr0      *= .5;
    covz0      *= 2.;
    m_nOneSeeds = 0; m_mapOneSeeds.clear();
   
    // Three space points comparison
    //
    for(int b(Nt); b!=Nb; ++b) {
    
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*m_COFK  ;
      float ICSA  = Tzb2*m_ipt2C ;
      float Se    = 1./sTzb2     ;
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      for(int t(0);  t!=Nt; ++t) {

	// Trigger point
	//	
	float dU0   =  m_U[t]-Ub       ;  if(dU0 == 0.) continue; 
	float A0    = (m_V[t]-Vb)/dU0  ;
	float C0    = 1./sqrt(1.+A0*A0); 
	  
	// Bottom  point
	//
	float B0    = 2.*(Vb-A0*Ub);

	// Top     point
	//
	float Ct    = (1.-B0*m_Y[t])*C0;
	float St    = (A0+B0*m_X[t])*C0;
	float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Ce};
	float rt[3];  if(!m_SP[t]->coordinates(dt,rt)) continue;

	float xt    = rt[0]-X;
	float yt    = rt[1]-Y;
	float zt    = rt[2]-Z;
	float dxy   = xt*xt+yt*yt;
	float rt2   = 1./dxy     ;
	float tz    = zt*sqrt(rt2);
	
	float dT  = ((Tzb-tz)*(Tzb-tz)-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+tz)*(Tzb+tz));
	if( dT > ICSA) continue;

	float ut  = (xt*ax+yt*ay)*rt2;
	float vt  = (yt*ax-xt*ay)*rt2;
	float dU  = ut-Ub; if(dU == 0.) continue;	
	float A   = (vt-Vb)/dU;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > m_ipt2K*S2 || dT*S2 > B2*CSA) continue;
	
	float Im  = fabs((A-B*R)*R)                  ; 

	if(Im <= m_diverpps) {

	  m_Im[t] = Im;
	  m_L [t] = sqrt(dxy+zt*zt);
	  m_CmSpn.push_back(std::make_pair(B/sqrt(S2),t));
	}
       }
      if(m_CmSpn.size() > 1) newOneSeedWithCurvaturesComparisonPPS(m_SP[b],(*r0),Z-R*Tzb); m_CmSpn.clear();
    }
    fillSeeds();  nseed += m_fillOneSeeds;
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds in ROI
///////////////////////////////////////////////////////////////////

 
void InDet::SiSpacePointsSeedMaker_ITK::production3SpTrigger
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator*  ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator*  ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator*  ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator*  ,
  int , int , int& ) 
{
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeed
(InDet::SiSpacePointForSeedITK*& p1, InDet::SiSpacePointForSeedITK*& p2,
 InDet::SiSpacePointForSeedITK*& p3,float z,float q)
{
  if(m_nOneSeeds < m_maxOneSize) {

    m_OneSeeds[m_nOneSeeds].set(p1,p2,p3,z); 
    m_mapOneSeeds.insert(std::make_pair(q,m_OneSeeds+m_nOneSeeds));
    ++m_nOneSeeds;
  }
  else                     {
    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::reverse_iterator 
      l = m_mapOneSeeds.rbegin();

    if((*l).first <= q) return;
    
    InDet::SiSpacePointsProSeedITK* s = (*l).second; s->set(p1,p2,p3,z);

    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator 
      i = m_mapOneSeeds.insert(std::make_pair(q,s));
	
    for(++i; i!=m_mapOneSeeds.end(); ++i) {
      if((*i).second==s) {m_mapOneSeeds.erase(i); return;}
    }
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for SSS seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonSSS
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  std::sort(m_CmSpn.begin(),m_CmSpn.end(),comCurvatureITK());
  std::vector<std::pair<float,int>>::iterator j,jn,i = m_CmSpn.begin(),ie = m_CmSpn.end(); jn=i;

  float Lt[4];

  for(; i!=ie; ++i) {

    int NT     = 1             ;
    int n      = (*i).second   ;
    Lt  [0]    = m_L [n]       ;
    float Cmin = (*i).first-dC ;
    float Cmax = (*i).first+dC ;
    
    for(j=jn;  j!=ie; ++j) {

      if(      j == i      ) continue;
      if((*j).first < Cmin ) {(jn=j)++; continue;}
      if((*j).first > Cmax ) break;
      
      float L = m_L[(*j).second];

      int k = 0;
      for(; k!=NT; ++k) {if(fabs(L-Lt[k]) < 20.) break;}
      if( k==NT ) {Lt[NT]= L; if(++NT==4) break;}
    }
    float Q = m_Im[n]-float(NT)*100.; if(NT > 2) Q-=100000.;
    newOneSeed(SPb,SP0,m_SP[n],Zob,Q);
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for PPP seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonPPP
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  std::sort(m_CmSpn.begin(),m_CmSpn.end(),comCurvatureITK());
  std::vector<std::pair<float,int>>::iterator j,jn,i = m_CmSpn.begin(),ie = m_CmSpn.end(); jn=i; 

  float Lt[4];

  float Qmin = 1.e20;
  float Rb   = 2.*SPb->radius();
  int   NTc(2); if(Rb > 280.) NTc = 1;
  int   nmin = -1;

  for(; i!=ie; ++i) {

    int NT     = 1             ;
    int n      = (*i).second   ;
    Lt  [0]    = m_L [n]       ;
    float Cmin = (*i).first-dC ;
    float Cmax = (*i).first+dC ;

    for(j=jn;  j!=ie; ++j) {

      if(      j == i      ) continue;
      if((*j).first < Cmin ) {(jn=j)++; continue;}
      if((*j).first > Cmax ) break;

      float L = m_L[(*j).second];

      int k=0;
      for(; k!=NT; ++k) {if(fabs(L-Lt[k]) < 20.) break;}
      if( k==NT ) {Lt[NT]= L; if(++NT==4) break;}
    }
    if(NT < NTc) continue;

    SiSpacePointForSeedITK* SPt = m_SP[n];

    float Q = 100.*m_Im[n]+((SPt->radius()-Rb)-float(NT)*100.);
    
    if(Q > SPb->quality() && Q > SP0->quality() && Q > SPt->quality()) continue;
    if     (NT-NTc > 0   )  newOneSeed(SPb,SP0,SPt,Zob,Q-100000.);
    else if(    Q  < Qmin) {Qmin=Q; nmin=n;}
  }
  if(nmin >=0) newOneSeed(SPb,SP0,m_SP[nmin],Zob,Qmin);
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for PPS seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonPPS
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  std::sort(m_CmSpn.begin(),m_CmSpn.end(),comCurvatureITK());
  std::vector<std::pair<float,int>>::iterator j,jn,i = m_CmSpn.begin(),ie = m_CmSpn.end(); jn=i;

  float Lt[4];

  for(; i!=ie; ++i) {

    int NT     = 1             ;
    int n      = (*i).second   ;
    Lt  [0]    = m_L [n]       ;
    float Cmin = (*i).first-dC ;
    float Cmax = (*i).first+dC ;

    for(j=jn;  j!=ie; ++j) {

      if(      j == i      ) continue;
      if((*j).first < Cmin ) {(jn=j)++; continue;}
      if((*j).first > Cmax ) break;
      
      float L = m_L[(*j).second];

      int k=0;
      for(; k!=NT; ++k) {if(fabs(L-Lt[k]) < 20.) break;}
      if( k==NT ) {Lt[NT]= L; if(++NT==4) break;}
    }
    if(NT < 2) continue;
    float Q = m_Im[n]-float(NT)*100.;
    newOneSeed(SPb,SP0,m_SP[n],Zob,Q);
  }
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::fillSeeds ()
{
  m_fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator 
    lf = m_mapOneSeeds.begin(),
    l  = m_mapOneSeeds.begin(),
    le = m_mapOneSeeds.end  ();
  
  if(l==le) return;

  SiSpacePointsProSeedITK* s;

  bool Q = (*l).first < -90000.;

  for(; l!=le; ++l) {

    float w = (*l).first ; if(Q && w > -90000. ) break;
    s       = (*l).second;
    if(!s->setQuality(w)) continue;
     
    if(i_seede!=l_seeds.end()) {
      s  = (*i_seede++);
      *s = *(*l).second;
    }
    else                  {
      s = new SiSpacePointsProSeedITK(*(*l).second);
      l_seeds.push_back(s);
      i_seede = l_seeds.end(); 
    }
    ++m_fillOneSeeds;
  }
}

///////////////////////////////////////////////////////////////////
// Pixels information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::pixInform(Trk::SpacePoint*const& sp,float* r)
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

void InDet::SiSpacePointsSeedMaker_ITK::sctInform(Trk::SpacePoint*const& sp,float* r) 
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

  Amg::Vector3D b0 (e0.second-e0.first);
  Amg::Vector3D b1 (e1.second-e1.first);
  Amg::Vector3D d02(e0.first -e1.first);

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
  r[12] = float(e0.first[0])-m_xbeam[0];
  r[13] = float(e0.first[1])-m_ybeam[0];
  r[14] = float(e0.first[2])-m_zbeam[0];
}
