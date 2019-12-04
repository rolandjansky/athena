     
///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ITkTrigger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITkTrigger.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsComparison.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ITkTrigger::SiSpacePointsSeedMaker_ITkTrigger
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
  m_drminPPP  = 6.      ;
  m_drmaxPPP  = 120.    ;
  m_rapcut    = 2.7     ;
  m_zmin      = -250.   ;
  m_zmax      = +250.   ;
  m_dzver     = 5.      ;
  m_dzdrver   = .02     ;
  m_diver     = 10.     ;
  m_dazmax    = .02     ;
  r_rmax      = 1100.   ;
  r_rmin      = 0.      ;
  m_umax      = 0.      ;
  r_rstep     =  2.     ;
  m_dzmaxPPP  = 600.    ; 
  m_maxsizeSP = 4096    ;
  m_maxOneSize= 3       ;
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
  m_Tn        = 0       ;
  m_OneSeeds  = 0       ;
  m_OneSeedsQ = 0       ;
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

InDet::SiSpacePointsSeedMaker_ITkTrigger::~SiSpacePointsSeedMaker_ITkTrigger()
{

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
  if(m_Im) delete [] m_Im;
  if(m_Tn) delete [] m_Tn;
  if(m_OneSeeds ) delete [] m_OneSeeds ;
  if(m_OneSeedsQ) delete [] m_OneSeedsQ;
 }

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITkTrigger::initialize()
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

  // Build framework
  //
  buildFrameWork();

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

StatusCode InDet::SiSpacePointsSeedMaker_ITkTrigger::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::newEvent(int iteration) 
{
  m_iteration0 = iteration;

  iteration <=0 ? m_iteration = 0 : m_iteration = iteration;

  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;

  if(!m_iteration) {
    buildBeamFrameWork();

    m_ipt2K     = m_ipt2/(m_K*m_K);
    m_ipt2C     = m_ipt2*m_COF    ;
    m_COFK      = m_COF*(m_K*m_K) ;  

    i_spforseed = l_spforseed.begin();
  }
  else {
    fillLists(); return;
  }

  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>&)
{
  m_iteration  = 0    ;
  erase();

  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;

  buildBeamFrameWork();

  m_ipt2K     = m_ipt2/(m_K*m_K);
  m_ipt2C     = m_ipt2*m_COF    ;
  m_COFK      = m_COF*(m_K*m_K) ;  

  i_spforseed = l_spforseed.begin();

  m_ns = 0;
  m_rSortedPIX.clear();

    
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
	
	InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp));
	if(sps) { 
	  m_rSortedPIX.push_back(sps); ++m_ns;
	}
      }
    }
  }
  std::sort(m_rSortedPIX.begin(),m_rSortedPIX.end(),InDet::SiSpacePointsITKComparison_R()); 
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor& IRD)
{
  newRegion(vPixel,vSCT);

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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::find2Sp(const std::list<Trk::Vertex>& lv) 
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::find3Sp(const std::list<Trk::Vertex>& lv) 
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::find3Sp(const std::list<Trk::Vertex>& lv,const double* ZVertex) 
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::findVSp (const std::list<Trk::Vertex>& lv)
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

MsgStream& InDet::SiSpacePointsSeedMaker_ITkTrigger::dump( MsgStream& out ) const
{
  if(m_nprint)  
    return dumpEvent(out); 
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITkTrigger::dumpConditions( MsgStream& out ) const
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

MsgStream& InDet::SiSpacePointsSeedMaker_ITkTrigger::dumpEvent( MsgStream& out ) const
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

std::ostream& InDet::SiSpacePointsSeedMaker_ITkTrigger::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiSpacePointsSeedMaker_ITkTrigger& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiSpacePointsSeedMaker_ITkTrigger& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::findNext () 
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

bool InDet::SiSpacePointsSeedMaker_ITkTrigger::newVertices(const std::list<Trk::Vertex>& lV)
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::buildFrameWork() 
{
  m_ptmin     = fabs(m_ptmin);  
  
  if(m_ptmin < 100.) m_ptmin = 100.;

  if(fabs(m_etamin) < .1) m_etamin = -m_etamax ;
  m_dzdrmax0  = 1./tan(2.*atan(exp(-m_etamax)));
  m_dzdrmin0  = 1./tan(2.*atan(exp(-m_etamin)));

  m_dzdrmaxPPS= 1./tan(2.*atan(exp(-m_rapydityPPSmax)));
  
  m_COF       =  134*.05*9.                    ;
  m_ipt       = 1./fabs(m_ptmin)               ;
  m_ipt2      = m_ipt*m_ipt                    ;
  
  double f[3], gP[3] ={10.,10.,0.};
  m_K = 2./(300.* 5. );  
  if(m_fieldService->solenoidOn()) {
    m_fieldService->getFieldZR(gP,f); 
    m_K = 2./(300.*f[2]);    
  }

  m_ns = m_nsaz = m_nsazv = 0;

  // Build radius-azimuthal sorted containers
  //
  const float pi2     = 2.*M_PI            ;
  float ptm = 400.; if(m_ptmin < ptm) ptm = m_ptmin;

  int   NFmax    = 200            ;

  // Build radius-azimuthal-Z sorted containers
  //
  m_fNmax = int(pi2/AzimuthalStep(m_ptmin,m_diver,50.,250.));

  if(m_fNmax > NFmax) m_fNmax = NFmax; 
  if(m_fNmax < 10   ) m_fNmax = 10   ;  
  m_sF  = float(m_fNmax+1)/pi2;

  // Build maps for radius-azimuthal-Z sorted collections for PPP seeds
  //
  for(int f=0; f<=m_fNmax; ++f) {

    int fb = f-1; if(fb<0     ) fb=m_fNmax; 
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
  if(!m_Tn) m_Tn   = new                       FloatIntT[m_maxsizeSP];
  if(!m_OneSeeds ) m_OneSeeds  = new InDet::SiSpacePointsProSeedITK [m_maxOneSize];  
  if(!m_OneSeedsQ) m_OneSeedsQ = new InDet::SiSpacePointsProSeedITK [m_maxOneSize];  

  if(!m_seedOutput) m_seedOutput = new InDet::SiSpacePointsSeed();

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();
}

////////////////////////////////////////////////////////////////////
// Claculation azimuthal angle step as function of
// pTmin(MeV),Imax,Rbegin(R1),Rstop(R2)(mm)
///////////////////////////////////////////////////////////////////

float InDet::SiSpacePointsSeedMaker_ITkTrigger::AzimuthalStep(float pTmin,float Imax,float R1,float R2)
{
  float Rm = pTmin/.6;
  float sI = fabs(asin(Imax/R1)-asin(Imax/R2));
  float sF = fabs(asin(R2/(2.*Rm))-asin(R1/(2.*Rm)));
  return ((sI+sF)/3.); 
}

//////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::buildBeamFrameWork() 
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
void  InDet::SiSpacePointsSeedMaker_ITkTrigger::convertToBeamFrameWork
(Trk::SpacePoint*const& sp,float* r) 
{
  r[0] = float(sp->globalPosition().x())-m_xbeam[0];
  r[1] = float(sp->globalPosition().y())-m_ybeam[0];
  r[2] = float(sp->globalPosition().z())-m_zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::fillLists() 
{

  if (m_iteration == 0) fillListsPPP();
  m_state = 0;
}

///////////////////////////////////////////////////////////////////
// Initiate PPP space points seed maker
///////////////////////////////////////////////////////////////////

  void InDet::SiSpacePointsSeedMaker_ITkTrigger::fillListsPPP() 
 {
   const float pi2 = 2.*M_PI;

   float sF   = m_sF   ;
   int  fNmax = m_fNmax;
   m_ns = 0; 
   erase();

   if(m_spacepointsPixel.isValid()) {

     SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
     SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

     for(; spc != spce; ++spc) {

       SpacePointCollection::const_iterator sp  = (*spc)->begin();
       SpacePointCollection::const_iterator spe = (*spc)->end  ();
      
       const InDet::SiCluster*           cl = static_cast<const InDet::SiCluster*>((*sp)->clusterList().first); 
       const Amg::Transform3D&           Tp = cl->detectorElement()->surface().transform();

       float r[15]; 

       r[3] = float(Tp(0,2));
       r[4] = float(Tp(1,2));
       r[5] = float(Tp(2,2)); 
       
       for(; sp != spe; ++sp) {

	 InDet::SiSpacePointForSeedITK* s = newSpacePoint((*sp),r); 
	
	 if(s) {
 
	   // Azimuthal angle sort
	   //
	   float F = s->phi(); if(F<0.) F+=pi2;
	   int   f = int(F*sF); if(f<0) f=fNmax; else if(f>fNmax) f=0;
	   
	   // Azimuthal angle and Z-coordinate sort
	   //
	   float Z = s->z(); int z;

	   if(Z>0.) {
	     Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
	   }
	   else     {
	     Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
	   }

	   int n(f*11+z); 
	   rfz_Sorted[n].push_back(s); 
	   ++m_ns;
	 }
       }
     }
   }
   // Loop through all RZ collections and sort them in radius order
   //
   for(int a(0); a!=2211; ++a) {
     if(rfz_Sorted[a].size() > 1 ) std::sort(rfz_Sorted[a].begin(),rfz_Sorted[a].end(),InDet::SiSpacePointsITKComparison_R());
   }
 }

///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::erase()
{
  for(int i=0; i!=2211;  ++i) {
    rfz_Sorted[i].clear();
  }
  m_state = 0;
  m_nsaz  = 0;
  m_nsazv = 0;
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITkTrigger::isUsed(const Trk::SpacePoint* sp)
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::production2Sp()
{
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::production3Sp()
{ 
  m_seeds.clear();
  if(m_iteration !=0 || m_ns < 3) return; 
  production3SpPPP();
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds PPP
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::production3SpPPP()
{ 
  //const int   ZI[11]= {0,1,2,3,10,9,8,7,5,4,6};
  const int     ZI[11]= {0,10,1,9,2,8,5,3,7,4,6};
  const float   RTmax[11] = {80.,100.,150.,200.,250., 250., 250.,200.,150.,100.,80.};;

  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  m_endlist = true;

  // Loop thorugh all azimuthal regions
  //
  for(int f(m_fNmin); f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z(2); z!=11; ++z) {

      m_RTmax = RTmax[ ZI[z] ];

      int a(f*11+ZI[z]);  if(rfz_Sorted[a].empty()) continue;

      int NB(0), NT(0);
      for(int i(0); i!=rfz_b[a]; ++i) {
	
	int an(rfz_ib[a][i]); 
	if(!rfz_Sorted[an].empty()) {rb[NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();}
      } 
      if(!NB) continue;
      for(int i(0); i!=rfz_t[a]; ++i) {
	
	int an(rfz_it[a][i]); 
	if(!rfz_Sorted[an].empty()) {rt[NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();}
       } 
      production3SpPPP(rb,rbe,rt,rte,NB,NT,nseed);
    }
    if(nseed >= m_maxsize) {m_endlist=false; m_fNmin=f+1; return;} 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 pixel space points seeds for full scan using 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::production3SpPPP
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],re0=rbe[0];
  m_RTmin        =  70.       ;
  float drminPPP =  m_drminPPP;
  float drmaxPPP =  m_drmaxPPP;

  for(; r0!=re0; ++r0) {if((*r0)->radius() > m_RTmin) break;}
  rt[0] = r0; ++rt[0];
  
  // Loop through all middle space points
  //
  for(; r0!=re0; ++r0) {

    float R     =(*r0)->radius(); if(R > m_RTmax) break;
    float Z     =(*r0)->     z();
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
    int Nt (0);
    for(int i(0); i!=NT; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rt[i], re = rte[i];
      if(r==re) continue;

      for(; r!=re; ++r) {if(((*r)->radius()-R) >= drminPPP) break;} rt[i]=r; 


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
	m_Tn[Nt].Fl = tz        ;
	m_Tn[Nt].In = Nt        ; ++Nt;
      }
    }

    if(Nt < Ntm || Nt==m_maxsizeSP) continue;

    // Bottom links production
    //
    int Nb(Nt); 

    for(int i(0); i!=NB; ++i) {

      std::vector<InDet::SiSpacePointForSeedITK*>::iterator r = rb[i], re = rbe[i];

      for(; r!=re; ++r) {if((R-(*r)->radius()) <= drmaxPPP) break;}  rb[i]=r;

      for(; r!=re; ++r) {
 	InDet::SiSpacePointForSeedITK* sp = (*r); 

	float Rb = sp->radius(); 
	float dR(R-Rb); if(dR < drminPPP) break;

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
	float tz  = dz*dr       ; if(fabs(tz) > m_dzdrmax ||  (Rb < 50. && fabs(tz) > 1.5) ||  Nb==m_maxsizeSP) continue;

	m_SP[Nb ] = sp          ;
	m_R [Nb ] = dr          ;
	m_U [Nb ] = u           ;
	m_V [Nb ] = v           ;
	float cz  = sp->covz()  ;
	float cr  = sp->covr()  ; 
	m_Er[Nb ] = ((covz0+cz)+(tz*tz)*(covr0+cr))*r2;
	m_Tn[Nb ].Fl = tz       ;
	m_Tn[Nb ].In = Nb       ; ++Nb;
       }
    }
    if(!(Nb-Nt)) continue; 

    sort(&m_Tn[ 0],Nt   );
    sort(&m_Tn[Nt],Nb-Nt);

    covr0      *= 2.;
    covz0      *= 2.;

    m_nOneSeeds  = 0; m_mapOneSeeds .clear();
    m_nOneSeedsQ = 0; m_mapOneSeedsQ.clear();

    float  ipt2K= m_ipt2K;
    float  diver= m_diver;


    // Three space points comparison
    //
    int it0 = 0;
    for(int ib(Nt); ib!=Nb; ++ib) {

      if(it0==Nt) break;

      m_nCmSp    = 0            ;

      int    b    = m_Tn[ib].In ;
      float  Tzb  = m_Tn[ib].Fl ;
      float  Rb2r = m_R [b]*covr0*Tzb;
      float  Rb2z = m_R [b]*covz0    ;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float  CSA  = Tzb2*m_COFK  ;
      float ICSA  = Tzb2*m_ipt2C ;

      int Nc = 1; if(m_SP[b]->radius() > 140.) Nc = 0;
      if(m_nOneSeedsQ) ++Nc;

      for(int it(it0);  it!=Nt; ++it) {

	int    t  = m_Tn[it].In;
	float DT  = Tzb-m_Tn[it].Fl;
	float dT  = (DT*DT-(Erb+m_Er[t]))-((Rb2z+Rb2r*m_Tn[it].Fl)*m_R[t]);

	if( dT > ICSA) {if(DT < 0.) break; it0 = it+1; continue;}

	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2) continue;
	if(dT*S2 > B2*CSA) {if(DT < 0.) break; it0 = it; continue;}

	float Im  = fabs((A-B*R)*R)                  ; 

	if(Im <= diver) {
	  m_Im[t] = Im; 
	  m_CmSp[m_nCmSp].Fl = B/sqrt(S2); m_CmSp[m_nCmSp].In = t; if(++m_nCmSp==500) break;
	}
      }
      if(m_nCmSp > Nc) newOneSeedWithCurvaturesComparisonPPP(m_SP[b],(*r0),Z-R*Tzb); 
    }
    fillSeeds(); nseed+=m_fillOneSeeds;
  }
}


///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::newOneSeed
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::newOneSeedQ
(InDet::SiSpacePointForSeedITK*& p1, InDet::SiSpacePointForSeedITK*& p2,
 InDet::SiSpacePointForSeedITK*& p3,float z,float q)
{
  
  if(m_nOneSeedsQ < m_maxOneSize) {

    m_OneSeedsQ[m_nOneSeedsQ].set(p1,p2,p3,z); 
    m_mapOneSeedsQ.insert(std::make_pair(q,m_OneSeedsQ+m_nOneSeedsQ));
    ++m_nOneSeedsQ;
  }
  else                     {
    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::reverse_iterator 
      l = m_mapOneSeedsQ.rbegin();

    if((*l).first <= q) return;
    
    InDet::SiSpacePointsProSeedITK* s = (*l).second; s->set(p1,p2,p3,z);

    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator 
      i = m_mapOneSeedsQ.insert(std::make_pair(q,s));
	
    for(++i; i!=m_mapOneSeedsQ.end(); ++i) {
      if((*i).second==s) {m_mapOneSeedsQ.erase(i); return;}
    }
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for PPP seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::newOneSeedWithCurvaturesComparisonPPP
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;
 
  if(m_nCmSp > 2) sort(m_CmSp,m_nCmSp);
 
  float Lt[4];

  int jn=0,i=0; 

  float Qmin = 1.e20;
  float Rb   = 2.*SPb->radius();
  int   NTc(2); if(Rb > 280.) NTc = 1;
  int   nmin = -1;
  bool  Qm   = Rb < 120. || fabs(Zob) > 150.;
  
  for(; i!=m_nCmSp; ++i) {

    int NT     = 1              ;
    int n      = m_CmSp[i].In   ;
    Lt  [0]    = m_L [n]        ;
    float Cmin = m_CmSp[i].Fl-dC;
    float Cmax = m_CmSp[i].Fl+dC;

    for(int j(jn);  j!=m_nCmSp; ++j) {

      if(      j == i      ) continue;
      if(m_CmSp[j].Fl < Cmin ) {(jn=j)++; continue;}
      if(m_CmSp[j].Fl > Cmax ) break;

      float L = m_L[m_CmSp[j].In];

      int k=0;
      for(; k!=NT; ++k) {if(fabs(L-Lt[k]) < 20.) break;}
      if( k==NT ) {Lt[NT]= L; if(++NT==4) break;}
    }
    int dN = NT-NTc;
    if(dN < 0 || (m_nOneSeedsQ && !dN)) continue;
    if(Qm && !dN && m_Im[n] > 1.) continue;

    SiSpacePointForSeedITK* SPt = m_SP[n];
    float Q = 100.*m_Im[n]+(fabs(Zob)-float(NT)*100.);  

    if(Q > SPb->quality() && Q > SP0->quality() && Q > SPt->quality()) continue;

    if     (   dN    )  newOneSeedQ(SPb,SP0,SPt,Zob,Q);
    else if(Q  < Qmin) {Qmin=Q; nmin=n;}
  }
  if(nmin >=0 && !m_nOneSeedsQ) newOneSeed(SPb,SP0,m_SP[nmin],Zob,Qmin);
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::fillSeeds ()
{
  m_fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator l,le;
  if     (m_nOneSeedsQ) {l=m_mapOneSeedsQ.begin(),le=m_mapOneSeedsQ.end();}
  else if(m_nOneSeeds ) {l=m_mapOneSeeds .begin(),le=m_mapOneSeeds .end();}
  else return;

  SiSpacePointsProSeedITK* s;

  for(; l!=le; ++l) {

    float w = (*l).first ;
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

void InDet::SiSpacePointsSeedMaker_ITkTrigger::pixInform(Trk::SpacePoint*const& sp,float* r)
{
  const InDet::SiCluster*           cl = static_cast<const InDet::SiCluster*>(sp->clusterList().first); 
  const InDetDD::SiDetectorElement* de = cl->detectorElement(); 
  const Amg::Transform3D& Tp = de->surface().transform();
  r[3] = float(Tp(0,2));
  r[4] = float(Tp(1,2));
  r[5] = float(Tp(2,2)); 
}

///////////////////////////////////////////////////////////////////
// Quick sort with recursive implementation
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITkTrigger::QuickSort(FloatIntT* A,int n)
{
  if(n <= 50) SmallSort(A,n);
  else {
    int m(Partition(A,n));
    if(n-m > 0) QuickSort(&A[m],n-m);
    if(m-2 > 0) QuickSort( A   ,m-2);
  }
}

void InDet::SiSpacePointsSeedMaker_ITkTrigger::SmallSort(FloatIntT* A,int n)
{
  FloatIntT A0 = A[0];
  if(A0.Fl > A[1].Fl) {A[0]=A[1]; A[1]=A0; A0=A[0];} 
  A[0].Fl = -1.e30;

  for(int i(1); i<n; ++i) {

    FloatIntT U = A[i+1];
    int j(i);
    while(A[j].Fl > U.Fl) {A[j+1]=A[j]; --j;} 
    A[j+1]=U;
  } 
  int i(1); 
  while (A[i].Fl < A0.Fl) {A[i-1]=A[i]; ++i;}
  A[i-1]=A0;
}

int InDet::SiSpacePointsSeedMaker_ITkTrigger::Partition(FloatIntT* A,int n)
{
  Middle(A,n);
  float Fn(A[n].Fl);
  int    m(0);
  int    i(0);
  while(true) {
    if(A[i].Fl <= Fn) {
      FloatIntT Tm=A[i]; A[i]=A[m]; A[m++]=Tm; if(i==n) break;
    } 
    ++i;
  }
  return m;
}

void InDet::SiSpacePointsSeedMaker_ITkTrigger::Middle(FloatIntT* A,int n)
{
  int m(n>>1);

  FloatIntT A0 = A[0];  // 0
  FloatIntT Am = A[m];  // n/2
  FloatIntT An = A[n];  // n

  bool Q01 = A0.Fl < Am.Fl;
  bool Q02 = A0.Fl < An.Fl;
  bool Q12 = Am.Fl < An.Fl;

  if(Q01) {                           // Ao < Am
    if     ( Q12) {A[n]=Am; A[m]=An;} // Ao < Am < An
    else if(!Q02) {A[n]=A0; A[0]=An;} // Am > Ao > An
  }                                   // Ao > Am
  else   if( Q02) {A[n]=A0; A[0]=An;} // Am < Ao < An
  else   if(!Q12) {A[n]=Am; A[m]=An;} // An < Am < Ao
}


  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeedITK* InDet::SiSpacePointsSeedMaker_ITkTrigger::newSpacePoint
    (Trk::SpacePoint*const& sp) 
{
  SiSpacePointForSeedITK* sps;

  float r[15]; convertToBeamFrameWork(sp,r);
  
  float R2 =  r[0]*r[0]+r[1]*r[1];

  if(fabs(r[2]) > 200. && R2 < 2500.       ) return 0;
  if(fabs(r[2])-m_zmax > m_dzdrmax*sqrt(R2)) return 0;

  
  pixInform(sp,r);
   
  if(i_spforseed!=l_spforseed.end()) {
    sps = (*i_spforseed++); sps->set(sp,r); 
  }
  else                               {
    l_spforseed.push_back((sps=new SiSpacePointForSeedITK(sp,r)));
    i_spforseed = l_spforseed.end();	
  }
      
  return sps;
}

InDet::SiSpacePointForSeedITK* InDet::SiSpacePointsSeedMaker_ITkTrigger::newSpacePoint
(Trk::SpacePoint*const& sp,float* r) 
{
  SiSpacePointForSeedITK* sps;

  convertToBeamFrameWork(sp,r);
  
  float R2 =  r[0]*r[0]+r[1]*r[1];

  if(fabs(r[2]) > 200. && R2 < 2500.       ) return 0;
  if(fabs(r[2])-m_zmax > m_dzdrmax*sqrt(R2)) return 0;

   
  if(i_spforseed!=l_spforseed.end()) {
    sps = (*i_spforseed++); sps->set(sp,r); 
  }
  else                               {
    l_spforseed.push_back((sps=new SiSpacePointForSeedITK(sp,r)));
    i_spforseed = l_spforseed.end();	
  }
      
  return sps;
}
