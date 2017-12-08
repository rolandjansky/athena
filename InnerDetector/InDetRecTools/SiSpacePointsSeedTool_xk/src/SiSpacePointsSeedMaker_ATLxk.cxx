/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

     
///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ATLxk
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
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ATLxk.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ATLxk::SiSpacePointsSeedMaker_ATLxk
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
  m_maxsize   = 50000   ;
  m_ptmin     =  500.   ;
  m_etamin    = 0.      ; m_etamax     = 2.7 ;
  m_r1min     = 0.      ; m_r1minv     = 0.  ; 
  m_r1max     = 600.    ; m_r1maxv     = 60. ;
  m_r2min     = 0.      ; m_r2minv     = 70. ;
  m_r2max     = 600.    ; m_r2maxv     = 200.;
  m_r3min     = 0.      ;
  m_r3max     = 600.    ;
  m_drmin     = 5.      ; m_drminv     = 20. ;    
  m_drmax     = 270.    ;    
  m_rapcut    = 2.7     ;
  m_zmin      = -250.   ;
  m_zmax      = +250.   ;
  m_dzver     = 5.      ;
  m_dzdrver   = .02     ;
  m_diver     = 10.     ;
  m_diverpps  =  1.7    ;
  m_diversss  =  50     ;
  m_divermax  =  20.    ;
  m_dazmax    = .02     ;
  r_rmax      = 600.    ;
  r_rstep     =  2.     ;
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
  m_Zo        = 0       ;
  m_OneSeeds  = 0       ;
  m_seedOutput= 0       ;
  m_maxNumberVertices = 99;
  m_skipIBLcut = false  ;
 
  m_xbeam[0]  = 0.      ; m_xbeam[1]= 1.; m_xbeam[2]=0.; m_xbeam[3]=0.;
  m_ybeam[0]  = 0.      ; m_ybeam[1]= 0.; m_ybeam[2]=1.; m_ybeam[3]=0.;
  m_zbeam[0]  = 0.      ; m_zbeam[1]= 0.; m_zbeam[2]=0.; m_zbeam[3]=1.;
  
//  m_spacepointsSCTname     = "SCT_SpacePoints"   ;
//  m_spacepointsPixelname   = "PixelSpacePoints"  ;
//  m_spacepointsOverlapname = "OverlapSpacePoints"; 
  m_beamconditions         = "BeamCondSvc"       ;
//  m_spacepointsSCT         = 0                   ;
//  m_spacepointsPixel       = 0                   ;
//  m_spacepointsOverlap     = 0                   ;

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

  declareProperty("maxSeedsForSpacePoint" ,m_maxOneSize            );
  declareProperty("maxNumberVertices"     ,m_maxNumberVertices     );
  declareProperty("SpacePointsSCTName"    ,m_spacepointsSCT    );
  declareProperty("SpacePointsPixelName"  ,m_spacepointsPixel  );
  declareProperty("SpacePointsOverlapName",m_spacepointsOverlap);
  declareProperty("BeamConditionsService" ,m_beamconditions        ); 
  declareProperty("useOverlapSpCollection", m_useOverlap           );
  declareProperty("UseAssociationTool"    ,m_useassoTool           ); 
  declareProperty("MagFieldSvc"           , m_fieldServiceHandle   );
  declareProperty("SkipIBLcut"            , m_skipIBLcut           );
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ATLxk::~SiSpacePointsSeedMaker_ATLxk()
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
  if(m_Tz) delete [] m_Tz;
  if(m_Er) delete [] m_Er;
  if(m_U ) delete [] m_U ;
  if(m_V ) delete [] m_V ;
  if(m_Zo) delete [] m_Zo;
  if(m_OneSeeds) delete [] m_OneSeeds;
 }

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ATLxk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get beam geometry
  //
  p_beam = 0;
  if(!m_beamconditions.empty()) {
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
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_assoTool<<endreq; 
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endreq;
    }
  }
  
  // Build framework
  //
  buildFrameWork();
  m_CmSp.reserve(500); 

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

StatusCode InDet::SiSpacePointsSeedMaker_ATLxk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newEvent(int iteration) 
{
  m_trigger = false;
  if(!m_pixel && !m_sct) return; 

  iteration <=0 ? m_iteration = 0 : m_iteration = iteration;
  erase();
  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;
  m_umax    =  100.      ;
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

  m_dbm ?  m_checketa = false : m_checketa = m_dzdrmin > 1.;

  float irstep = 1./r_rstep;
  int   irmax  = r_size-1  ;
  for(int i=0; i!=m_nr; ++i) {int n = r_index[i]; r_map[n] = 0; r_Sorted[n].clear();}
  m_ns = m_nr = 0;

  // Get pixels space points containers from store gate 
  //
  r_first = 0;
  if(!m_dbm && m_pixel) {

//    m_spacepointsPixel = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);

    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax) continue;

	  // Remove DBM space points
	  //
	  const InDetDD::SiDetectorElement* de= 
	    static_cast<const InDetDD::SiDetectorElement*>((*sp)->clusterList().first->detectorElement());
	  if(!de || de->isDBM()) continue;

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
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
  if(!m_dbm && m_sct) {

//    m_spacepointsSCT = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsSCT,m_spacepointsSCTname);

    if(m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsSCT->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax) continue;

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if(m_useOverlap && !m_checketa) {

//      m_spacepointsOverlap = 0;
//      sc = evtStore()->retrieve(m_spacepointsOverlap,m_spacepointsOverlapname);
      if(m_spacepointsOverlap.isValid()) {
	
	SpacePointOverlapCollection::const_iterator sp  = m_spacepointsOverlap->begin();
	SpacePointOverlapCollection::const_iterator spe = m_spacepointsOverlap->end  ();
	
	for (; sp!=spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax) continue;

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  // Get pixels space points containers from store gate for DBM reconstruction
  //
  if(m_dbm) {

//    m_spacepointsPixel = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);

    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  // Keep only DBM space points
	  //
	  const InDetDD::SiDetectorElement* de= 
	    static_cast<const InDetDD::SiDetectorElement*>((*sp)->clusterList().first->detectorElement());
	  if(!de || !de->isDBM() || (*sp)->r() > r_rmax) continue;

	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  if(ir > r_first) r_first = ir;
	  ++m_ns;
	}
      }
    }
    ++r_first;
  }
  if(iteration < 0) r_first = 0;
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{
  m_iteration  = 0    ;
  m_trigger    = false;
  erase();
  if(!m_pixel && !m_sct) return;

  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;
  m_umax    =  100.      ;

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

//    m_spacepointsPixel   = 0;
//    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);
    
    if( m_spacepointsPixel.isValid() ) {

      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      std::vector<IdentifierHash>::const_iterator l = vPixel.begin(), le = vPixel.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {
	
	SpacePointContainer::const_iterator  w =  m_spacepointsPixel->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	for(; sp != spe; ++sp) {

	  float r = (*sp)->r(); if(r > r_rmax) continue;
	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp)); 
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

	  float r = (*sp)->r(); if(r > r_rmax) continue;
	  InDet::SiSpacePointForSeed* sps = newSpacePoint((*sp)); 
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

void InDet::SiSpacePointsSeedMaker_ATLxk::newRegion
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

void InDet::SiSpacePointsSeedMaker_ATLxk::find2Sp(const std::list<Trk::Vertex>& lv) 
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
    m_zMin    = 0   ;
    production2Sp ();
  }
  i_seed  = l_seeds.begin();
  
  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find3Sp(const std::list<Trk::Vertex>& lv) 
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
    m_zMin    = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::find3Sp(const std::list<Trk::Vertex>& lv,const double* ZVertex) 
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
    m_zMin    = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::findVSp (const std::list<Trk::Vertex>& lv)
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
    m_zMin    = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dump( MsgStream& out ) const
{
  if(m_nprint)  return dumpEvent(out); return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dumpConditions( MsgStream& out ) const
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

MsgStream& InDet::SiSpacePointsSeedMaker_ATLxk::dumpEvent( MsgStream& out ) const
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

std::ostream& InDet::SiSpacePointsSeedMaker_ATLxk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiSpacePointsSeedMaker_ATLxk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiSpacePointsSeedMaker_ATLxk& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::findNext () 
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

bool InDet::SiSpacePointsSeedMaker_ATLxk::newVertices(const std::list<Trk::Vertex>& lV)
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

void InDet::SiSpacePointsSeedMaker_ATLxk::buildFrameWork() 
{
  m_ptmin     = fabs(m_ptmin);  
  
  // For DBM reconstruction we use new parameters
  //
  if     (     m_dbm    ) {r_rmax  = 150.; r_rstep = .5; m_drmin = 2.; m_ptmin = 10.; m_diver = 150.;}
  else if(m_ptmin < 100.) m_ptmin = 100.;

  if(m_diversss < m_diver   ) m_diversss = m_diver   ; 
  if(m_divermax < m_diversss) m_divermax = m_diversss;

  if(fabs(m_etamin) < .1) m_etamin = -m_etamax ;
  m_dzdrmax0  = 1./tan(2.*atan(exp(-m_etamax)));
  m_dzdrmin0  = 1./tan(2.*atan(exp(-m_etamin)));
  
  m_r3max     = r_rmax                         ; 
  m_COF       =  134*.05*9.                    ;
  m_ipt       = 1./fabs(.9*m_ptmin)            ;
  m_ipt2      = m_ipt*m_ipt                    ;
  m_K         = 0.                             ;

  m_ns = m_nsaz = m_nsazv = m_nr = m_nrfz = m_nrfzv = 0;

  // Build radius sorted containers
  //
  r_size = int((r_rmax+.1)/r_rstep);
  r_Sorted = new std::list<InDet::SiSpacePointForSeed*>[r_size];
  r_index  = new int[r_size];
  r_map    = new int[r_size];  
  m_nr   = 0; for(int i=0; i!=r_size; ++i) {r_index[i]=0; r_map[i]=0;}

  // Build radius-azimuthal sorted containers
  //
  const float pi2     = 2.*M_PI            ;
  const int   NFmax    = 53                ;
  const float sFmax   = float(NFmax )/pi2;
  const float m_sFmin = 100./60.          ;

  float ptm = 400.; if(!m_dbm && m_ptmin < ptm) ptm = m_ptmin;

  m_sF        = ptm /60. ; if(m_sF    >sFmax ) m_sF    = sFmax  ; else if(m_sF < m_sFmin) m_sF = m_sFmin;
  m_fNmax     = int(pi2*m_sF); if(m_fNmax >=NFmax) m_fNmax = NFmax-1;

  // Build radius-azimuthal-Z sorted containers
  //
  m_nrfz  = 0; for(int i=0; i!=583; ++i) {rfz_index [i]=0; rfz_map [i]=0;}

  // Build radius-azimuthal-Z sorted containers for Z-vertices
  //
  const int   NFtmax  = 100               ;
  const float sFvmax = float(NFtmax)/pi2;
  m_sFv       = m_ptmin/120.  ; if(m_sFv   >sFvmax)  m_sFv    = sFvmax; 
  m_fvNmax    = int(pi2*m_sFv); if(m_fvNmax>=NFtmax) m_fvNmax = NFtmax-1;
  m_nrfzv = 0; for(int i=0; i!=300; ++i) {rfzv_index[i]=0; rfzv_map[i]=0;}

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

	rfz_t [a]    = 9 ;
	rfz_it[a][3] = a+1; 
	rfz_it[a][4] = b+1; 
	rfz_it[a][5] = c+1; 
	rfz_it[a][6] = a-1; 
	rfz_it[a][7] = b-1; 
	rfz_it[a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [a]    = 6 ;
	rfz_ib[a][3] = a-1; 
	rfz_ib[a][4] = b-1; 
	rfz_ib[a][5] = c-1; 

	if(z<10) {

	  rfz_t [a]    = 6 ;
	  rfz_it[a][3] = a+1; 
	  rfz_it[a][4] = b+1; 
	  rfz_it[a][5] = c+1; 
	}
      }
      else {

	rfz_b [a]    = 6 ;
	rfz_ib[a][3] = a+1; 
	rfz_ib[a][4] = b+1; 
	rfz_ib[a][5] = c+1; 

	if(z>0) {

	  rfz_t [a]    = 6 ;
	  rfz_it[a][3] = a-1; 
	  rfz_it[a][4] = b-1; 
	  rfz_it[a][5] = c-1; 
	}
      }

      if     (z==3) {
	rfz_b[a]      = 9;
	rfz_ib[a][6] = a+2; 
	rfz_ib[a][7] = b+2; 
	rfz_ib[a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b[a]      = 9;
	rfz_ib[a][6] = a-2; 
	rfz_ib[a][7] = b-2; 
	rfz_ib[a][8] = c-2; 
      }
    }
  }

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
  
  if(!m_SP) m_SP   = new InDet::SiSpacePointForSeed*[m_maxsizeSP];
  if(!m_R ) m_R    = new                      float[m_maxsizeSP];
  if(!m_Tz) m_Tz   = new                      float[m_maxsizeSP];
  if(!m_Er) m_Er   = new                      float[m_maxsizeSP];
  if(!m_U ) m_U    = new                      float[m_maxsizeSP]; 
  if(!m_V ) m_V    = new                      float[m_maxsizeSP];
  if(!m_Zo) m_Zo   = new                      float[m_maxsizeSP];
  if(!m_OneSeeds) m_OneSeeds  = new InDet::SiSpacePointsProSeed [m_maxOneSize];  

  if(!m_seedOutput) m_seedOutput = new InDet::SiSpacePointsSeed();

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::buildBeamFrameWork() 
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

void  InDet::SiSpacePointsSeedMaker_ATLxk::convertToBeamFrameWork
(Trk::SpacePoint*const& sp,float* r) 
{
  
  r[0] = float(sp->globalPosition().x())-m_xbeam[0];
  r[1] = float(sp->globalPosition().y())-m_ybeam[0];
  r[2] = float(sp->globalPosition().z())-m_zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::fillLists() 
{
  const float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeed*>::iterator r,re;

  int  ir0 =     0;
  bool ibl = false;

  for(int i=r_first; i!=r_size;  ++i) {

    if(!r_map[i]) continue; r = r_Sorted[i].begin(); re = r_Sorted[i].end();
    if(!ir0) ir0 = i;

    if( m_iteration) {

      if       (!(*r)->spacepoint->clusterList().second) {if(i < 20) ibl = true;}
      else  if (  ibl  ) break;
      else  if (i > 175) break;
    }

    for(; r!=re; ++r) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;

      int   f = int(F*m_sF); f<0 ? f = m_fNmax : f>m_fNmax ? f = 0 : f=f;

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
      
      if(!m_iteration && (*r)->spacepoint->clusterList().second == 0 && z>=3 && z<=7) { 
	z<=4 ? z=0 : z>=6 ? z=2 : z=1;

	// Azimutla angle and Z-coordinate sort for fast vertex search
	//
	f = int(F*m_sFv); f<0 ? f+=m_fvNmax : f> m_fvNmax ? f-=m_fvNmax : f=f;
	n = f*3+z; ++m_nsazv;
	rfzv_Sorted[n].push_back(*r); if(!rfzv_map[n]++) rfzv_index[m_nrfzv++] = n;
      }
    }
  }
  if(!m_skipIBLcut && !m_sct && ir0 && float(ir0)*r_rstep < 43.) m_umax = -200.;
  m_state = 0;
}
   
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::erase()
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

bool InDet::SiSpacePointsSeedMaker_ATLxk::isUsed(const Trk::SpacePoint* sp)
{
  const Trk::PrepRawData* d = sp->clusterList().first ; 
  if(!d || !m_assoTool->isUsed(*d)) return false;

  d = sp->clusterList().second;
  if(!d || m_assoTool->isUsed(*d)) return true;

  return false;
}


///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production2Sp()
{
  if(m_nsazv<2) return;

  std::list<InDet::SiSpacePointForSeed*>::iterator r0,r0e,r,re;
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fvNmin; f<=m_fvNmax; ++f) {

    // For each azimuthal region loop through Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;
    for(; z!=3; ++z) {
      
      int a  = f*3+z;  if(!rfzv_map[a]) continue; 
      r0  = rfzv_Sorted[a].begin(); 
      r0e = rfzv_Sorted[a].end  (); 

      if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

      // Loop through trigger space points
      //
      for(; r0!=r0e; ++r0) {

	float X  = (*r0)->x();
	float Y  = (*r0)->y();
	float R  = (*r0)->radius();
	if(R<m_r2minv) continue; if(R>m_r2maxv) break;
	float Z  = (*r0)->z();
	float ax = X/R;
	float ay = Y/R;

	// Bottom links production
	//
	int NB = rfzv_n[a];
	for(int i=0; i!=NB; ++i) {
	  
	  int an = rfzv_i[a][i];
	  if(!rfzv_map[an]) continue; 

	  r  =  rfzv_Sorted[an].begin();
	  re =  rfzv_Sorted[an].end  ();
	  
	  for(; r!=re; ++r) {
	    
	    float Rb =(*r)->radius();
	    if(Rb<m_r1minv) continue; if(Rb>m_r1maxv) break;
	    float dR = R-Rb; 
	    if(dR<m_drminv) break; if(dR>m_drmax) continue;
	    float dZ = Z-(*r)->z();
	    float Tz = dZ/dR; if(Tz<m_dzdrmin || Tz>m_dzdrmax) continue;
	    float Zo = Z-R*Tz;	          

	    // Comparison with vertices Z coordinates
	    //
	    if(!isZCompatible(Zo,Rb,Tz)) continue;

	    // Momentum cut
	    //
	    float dx =(*r)->x()-X; 
	    float dy =(*r)->y()-Y; 
	    float x  = dx*ax+dy*ay          ;
	    float y  =-dx*ay+dy*ax          ;
	    float xy = x*x+y*y              ; if(xy == 0.) continue;
	    float r2 = 1./xy                ;
	    float Ut = x*r2                 ;
	    float Vt = y*r2                 ;
	    float UR = Ut*R+1.              ; if(UR == 0.) continue;
	    float A  = Vt*R/UR              ;
	    float B  = Vt-A*Ut              ;
	    if(fabs(B*m_K) > m_ipt*sqrt(1.+A*A)) continue; ++nseed;
	    newSeed((*r),(*r0),Zo);
	  }
	}
	if(nseed < m_maxsize) continue; 
	m_endlist=false; m_rMin = (++r0); m_fvNmin=f; m_zMin=z; 
	return;
      }
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production3Sp()
{ 
  if(m_nsaz<3) return; 
  m_seeds.clear();

  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::list<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

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
      if(!m_trigger) production3Sp       (rb,rbe,rt,rte,NB,NT,nseed);
      else           production3SpTrigger(rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::production3Sp
( std::list<InDet::SiSpacePointForSeed*>::iterator* rb ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rt ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::list<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;
  float imaxs = m_divermax;

  m_CmSp.clear();

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();

    float R  = (*r0)->radius(); 

    const Trk::Surface* sur0 = (*r0)->sur();
    const Trk::Surface* surn = (*r0)->sun();
    float               X    = (*r0)->x()  ;
    float               Y    = (*r0)->y()  ;
    float               Z    = (*r0)->z()  ;
    int                 Nb   = 0           ;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR > m_drmax) {rb[i]=r; continue;}   
	if(dR < m_drmin || (m_iteration && (*r)->spacepoint->clusterList().second)) break;
	if((*r)->sur()==sur0 || (surn && surn==(*r)->sun())) continue;

	float Tz = (Z-(*r)->z())/dR, aTz =fabs(Tz);

	if(aTz < m_dzdrmin || aTz > m_dzdrmax) continue;
	
	// Comparison with vertices Z coordinates
	//
	float Zo = Z-R*Tz; if(!isZCompatible(Zo,Rb,Tz)) continue;
	m_SP[Nb] = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;  
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;

	if( (*r)->sur()==sur0 || (surn && surn==(*r)->sun())) continue;

	float Tz = ((*r)->z()-Z)/dR, aTz =fabs(Tz);  

	if(aTz < m_dzdrmin || aTz > m_dzdrmax) continue;

	// Comparison with vertices Z coordinates
	//
	float Zo = Z-R*Tz; if(!isZCompatible(Zo,R,Tz)) continue;
  	m_SP[Nt] = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    float ax    = X/R           ;
    float ay    = Y/R           ;

    for(int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = m_SP[i];  

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(i < Nb) tz = -tz;

      m_Tz[i]   = tz                                            ;
      m_Zo[i]   = Z-R*tz                                        ;
      m_R [i]   = dr                                            ;
      m_U [i]   = x*r2                                          ;
      m_V [i]   = y*r2                                          ;
      m_Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;
      float imax  = imaxp        ; if(m_SP[b]->spacepoint->clusterList().second) imax = imaxs;
  
      for(int t=Nb;  t!=Nt; ++t) {
	
	float dT  = ((Tzb-m_Tz[t])*(Tzb-m_Tz[t])-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+m_Tz[t])*(Tzb+m_Tz[t]));
	if( dT > ICSA) continue;

	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	float Im  = fabs((A-B*R)*R)                  ; 

	if(Im <= imax) {
	  float dr; m_R[t] < m_R[b] ? dr = m_R[t] : dr = m_R[b]; Im+=fabs((Tzb-m_Tz[t])/(dr*sTzb2));
	  m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);

	}
      }
      if(!m_CmSp.empty()) {newOneSeedWithCurvaturesComparison(m_SP[b],(*r0),Zob);}
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {m_endlist=false; ++r0; m_rMin = r0;  return;} 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds in ROI
///////////////////////////////////////////////////////////////////

 
void InDet::SiSpacePointsSeedMaker_ATLxk::production3SpTrigger
( std::list<InDet::SiSpacePointForSeed*>::iterator* rb ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rt ,
  std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::list<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  const float pi = M_PI, pi2 = 2.*pi; 

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;
  float imaxs = m_diversss;

  m_CmSp.clear();

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();
	
    float R  = (*r0)->radius(); 

    const Trk::Surface* sur0 = (*r0)->sur();
    float               X    = (*r0)->x()  ;
    float               Y    = (*r0)->y()  ;
    float               Z    = (*r0)->z()  ;
    int                 Nb   = 0           ;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  

	float dR = R-Rb; 
	if(dR > m_drmax) {rb[i]=r; continue;}   
	if((*r)->sur()==sur0) continue;

	if(dR < m_drmin || (m_iteration && (*r)->spacepoint->clusterList().second)) break;

	// Comparison with  bottom and top Z 
	//
	float Tz = (Z-(*r)->z())/dR;
	float Zo = Z-R*Tz          ; if(Zo < m_zminB || Zo > m_zmaxB) continue;
	float Zu = Z+(550.-R)*Tz   ; if(Zu < m_zminU || Zu > m_zmaxU) continue;
	m_SP[Nb] = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;  
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;
	if( (*r)->sur()==sur0) continue;

	// Comparison with  bottom and top Z 
	//
	float Tz = ((*r)->z()-Z)/dR;  
	float Zo = Z-R*Tz          ; if(Zo < m_zminB || Zo > m_zmaxB) continue;
	float Zu = Z+(550.-R)*Tz   ; if(Zu < m_zminU || Zu > m_zmaxU) continue;
  	m_SP[Nt] = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();

    float ax   = X/R;
    float ay   = Y/R;
    
    for(int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = m_SP[i];  

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(i < Nb) tz = -tz;

      m_Tz[i]   = tz                                            ;
      m_Zo[i]   = Z-R*tz                                        ;
      m_R [i]   = dr                                            ;
      m_U [i]   = x*r2                                          ;
      m_V [i]   = y*r2                                          ;
      m_Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
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
      float imax  = imaxp        ; if(m_SP[b]->spacepoint->clusterList().second) imax = imaxs;
      
      for(int t=Nb;  t!=Nt; ++t) {
	
	float dT  = ((Tzb-m_Tz[t])*(Tzb-m_Tz[t])-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+m_Tz[t])*(Tzb+m_Tz[t]));
	if( dT > ICSA) continue;

	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	float Im  = fabs((A-B*R)*R)                  ; 
	if(Im > imax) continue;

	// Azimuthal angle test
	//
	float y  = 1.;
	float x  = 2.*B*R-A;
	float df = fabs(atan2(ay*y-ax*x,ax*y+ay*x)-m_ftrig);
	if(df > pi      ) df=pi2-df;
	if(df > m_ftrigW) continue;
	m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
      }
      if(!m_CmSp.empty()) {newOneSeedWithCurvaturesComparison(m_SP[b],(*r0),Zob);}
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {m_endlist=false; ++r0; m_rMin = r0;  return;} 
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newOneSeed
(InDet::SiSpacePointForSeed*& p1, InDet::SiSpacePointForSeed*& p2,
 InDet::SiSpacePointForSeed*& p3,float z,float q)
{
  if(m_nOneSeeds < m_maxOneSize) {

    m_OneSeeds[m_nOneSeeds].set(p1,p2,p3,z); 
    m_mapOneSeeds.insert(std::make_pair(q,m_OneSeeds+m_nOneSeeds));
    ++m_nOneSeeds;
  }
  else                     {
    std::multimap<float,InDet::SiSpacePointsProSeed*>::reverse_iterator 
      l = m_mapOneSeeds.rbegin();

    if((*l).first <= q) return;
    
    InDet::SiSpacePointsProSeed* s = (*l).second; s->set(p1,p2,p3,z);

    std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator 
      i = m_mapOneSeeds.insert(std::make_pair(q,s));
	
    for(++i; i!=m_mapOneSeeds.end(); ++i) {
      if((*i).second==s) {m_mapOneSeeds.erase(i); return;}
    }
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::newOneSeedWithCurvaturesComparison
	(SiSpacePointForSeed*& SPb,SiSpacePointForSeed*& SP0,float Zob)
{
  const float dC = .00003;

  bool  pixb = !SPb->spacepoint->clusterList().second;
  float ub   = SPb->quality()                        ;
  float u0   = SP0->quality()                        ;

  std::sort(m_CmSp.begin(),m_CmSp.end(),comCurvature());
  std::vector<std::pair<float,InDet::SiSpacePointForSeed*>>::iterator j,jn,i = m_CmSp.begin(),ie = m_CmSp.end(); jn=i; 
      
  for(; i!=ie; ++i) {

    float u    = (*i).second->param(); 
    float Im   = (*i).second->param();

    bool                pixt = !(*i).second->spacepoint->clusterList().second;
    
    const Trk::Surface* Sui  = (*i).second->sur   ();
    float               Ri   = (*i).second->radius();  
    float               Ci1  =(*i).first-dC         ;
    float               Ci2  =(*i).first+dC         ;
    float               Rmi  = 0.                   ;
    float               Rma  = 0.                   ;
    bool                in   = false;
    
    if     (!pixb) u-=400.;
    else if( pixt) u-=200.;

    for(j=jn;  j!=ie; ++j) {
      
      if(       j == i           ) continue;
      if( (*j).first < Ci1       ) {jn=j; ++jn; continue;}
      if( (*j).first > Ci2       ) break;
      if( (*j).second->sur()==Sui) continue;
      
      float Rj = (*j).second->radius(); if(fabs(Rj-Ri) < m_drmin) continue;

      if(in) {
	if     (Rj > Rma) Rma = Rj;
	else if(Rj < Rmi) Rmi = Rj;
	else continue;
	if( (Rma-Rmi) > 20.) {u-=200.; break;}
      }
      else {
	in=true; Rma=Rmi=Rj; u-=200.;
      }
    }
    if(u > m_umax) continue;
    if(pixb!=pixt) {
      if(u > 0. || (u > ub && u > u0 && u > (*i).second->quality()) ) continue;
    }

    if(!pixb && Im > m_diversss && u > Im-500.) continue;

    newOneSeed(SPb,SP0,(*i).second,Zob,u);
  }
  m_CmSp.clear();
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ATLxk::fillSeeds ()
{
  m_fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator 
    lf = m_mapOneSeeds.begin(),
    l  = m_mapOneSeeds.begin(),
    le = m_mapOneSeeds.end  ();
  
  if(l==le) return;

  SiSpacePointsProSeed* s;

  for(; l!=le; ++l) {

    float w = (*l).first ;
    s       = (*l).second;
    if(!m_skipIBLcut && l!=lf && s->spacepoint0()->radius() < 43. && w > -200.) continue;
    if(!s->setQuality(w)) continue;
    
    if(i_seede!=l_seeds.end()) {
      s  = (*i_seede++);
      *s = *(*l).second;
    }
    else                  {
      s = new SiSpacePointsProSeed(*(*l).second);
      l_seeds.push_back(s);
      i_seede = l_seeds.end(); 
    }
    
    if     (s->spacepoint0()->spacepoint->clusterList().second) w-=3000.;
    else if(s->spacepoint1()->spacepoint->clusterList().second) w-=2000.;
    else if(s->spacepoint2()->spacepoint->clusterList().second) w-=1000.;

    m_seeds.insert(std::make_pair(w,s)); ++m_fillOneSeeds;
  }
}


