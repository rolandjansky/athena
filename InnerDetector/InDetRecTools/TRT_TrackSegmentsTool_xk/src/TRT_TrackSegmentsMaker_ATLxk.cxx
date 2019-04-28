/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_TrackSegmentsMaker_ATLxk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_ATLxk.h"

#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"
#include "EventInfo/TagInfo.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkExInterfaces/IPropagator.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackSegmentsMaker_ATLxk::TRT_TrackSegmentsMaker_ATLxk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                                ,
    m_propTool     ("Trk::RungeKuttaPropagator"                  ),
    m_extensionTool("InDet::TRT_TrackExtensionTool_xk"           ),
    m_assoTool     ("InDet::InDetPRD_AssociationToolGangedPixels") 
{
  m_trtname     =  "TRT_DriftCircles";
  m_fieldmode   =      "MapSolenoid" ;
  m_pTmin       =                500.;
  m_sharedfrac  =                0.3 ;
  m_nPhi        =                500 ;
  m_nMom        =                 70 ;
  m_slope       =                  0 ;
  m_islope      =                  0 ;
  m_ndzdr       =                  0 ;
  m_circles     =                  0 ;
  m_clustersCut =                 10 ;
  m_useassoTool =                true;
  m_removeNoise =                true;
  m_build       =               false;
  m_gupdate     =               false;

  declareInterface<ITRT_TrackSegmentsMaker>(this);

  declareProperty("PropagatorTool"         ,m_propTool     );
  declareProperty("AssosiationTool"        ,m_assoTool     );
  declareProperty("TrackExtensionTool"     ,m_extensionTool);
  declareProperty("MagneticFieldMode"      ,m_fieldmode    );
  declareProperty("TrtManagerLocation"     ,m_ntrtmanager  );
  declareProperty("TRT_ClustersContainer"  ,m_trtname      ); 
  declareProperty("NumberAzimuthalChannel" ,m_nPhi         ); 
  declareProperty("NumberMomentumChannel"  ,m_nMom         ); 
  declareProperty("MinNumberDriftCircles"  ,m_clustersCut  );
  declareProperty("UseAssosiationTool"     ,m_useassoTool  ); 
  declareProperty("RemoveNoiseDriftCircles",m_removeNoise  );
  declareProperty("pTmin"                  ,m_pTmin        );
  declareProperty("sharedFrac"             ,m_sharedfrac   ); 
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackSegmentsMaker_ATLxk::~TRT_TrackSegmentsMaker_ATLxk()
{ 
  if(m_ndzdr  ) delete [] m_ndzdr  ;
  if(m_slope  ) delete [] m_slope  ;
  if(m_islope ) delete [] m_islope ;
  if(m_circles) delete [] m_circles;
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMaker_ATLxk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL)<<"Toll service not found !"<<endreq; return sc;
  }

  // Initiate magnetic field properties
  //
  magneticFieldInit();

  // Get propagator tool
  //
  if( m_propTool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_propTool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_propTool << endreq;
  }

  // Get tool for track extension to TRT
  //
  if( m_extensionTool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_extensionTool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extensionTool << endreq;
  }

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

  // Get  TRT Detector Manager
  //
  m_trtmgr   = 0;
  sc = detStore()->retrieve(m_trtmgr,m_ntrtmanager);
  if (sc.isFailure()) {
    msg(MSG::FATAL)<<"Could not get TRT_DetectorManager"<<endreq; return sc;
  }

  //m_trtid = m_trtmgr->getIdHelper();

  // TRT
  if (detStore()->retrieve(m_trtid, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  // get the key -- from StoreGate (DetectorStore)
  //
  std::vector< std::string > tagInfoKeys =  detStore()->keys<TagInfo> ();
  std::string tagInfoKey = "";

  if(tagInfoKeys.size()==0)
    msg(MSG::WARNING) << " No TagInfo keys in DetectorStore "<< endreq;
   else {
     if(tagInfoKeys.size() > 1) {
       msg(MSG::WARNING) <<"More than one TagInfo key in the DetectorStore, using the first one "<< endreq;
     }
     tagInfoKey = tagInfoKeys[0];
   }

  m_callbackString = tagInfoKey;

  const DataHandle<TagInfo> tagInfoH;
  
  // register the Callback
  //
  sc = detStore()->regFcn(&InDet::TRT_TrackSegmentsMaker_ATLxk::mapStrawsUpdate,
			  this,tagInfoH,m_callbackString);

  if (sc.isFailure()) {
    msg(MSG::FATAL)<< "Failed to register the callback " << name() << endreq;
  }
  else {
    msg(MSG::INFO) << "Register the callback" << name() << endreq;
  }

  
  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMaker_ATLxk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::newEvent ()
{
  const float pi2 = 2.*M_PI;

  m_clusters     = 0;
  m_nlocal       = 0;
  m_nsegments    = 0;
  m_trtcontainer = 0; 
  m_bincluster .erase(m_bincluster .begin(),m_bincluster .end());
  m_sizebin    .erase(m_sizebin    .begin(),m_sizebin    .end());
  m_segments   .erase(m_segments.begin()   ,m_segments.end()   );
  m_segiterator   = m_segments.begin();

  if(!m_build) return;

  // Get drift circles collection
  //
  StatusCode s = evtStore()->retrieve(m_trtcontainer,m_trtname);
  if(s.isFailure() && m_outputlevel<=0) {
    msg(MSG::DEBUG)<<"Could not get TRT_DriftCircleContainer"<<endreq;
    return;
  }
  if(!m_trtcontainer) return;

  // Initiate extension tool
  //
  m_extensionTool->newEvent();

  InDet::TRT_DriftCircleContainer::const_iterator
    w = m_trtcontainer->begin(),we = m_trtcontainer->end();
  if(w==we) return;
  
  eraseHistogramm(); 

  int    n = 0;
  for(; w!=we; ++w) {

    if(n >= m_cirsize) break;

    Identifier ID = (*w)->identify();
    int be = m_trtid->barrel_ec     (ID);
    int lw = m_trtid->layer_or_wheel(ID);
    int sl = m_trtid->straw_layer   (ID);
    
    int b; be < 0 ?  b = be+2 : b = be+1;
    int l  = m_flayers[b][lw]+sl;
    unsigned int sb  = m_begin[b][l];
    unsigned int se  = m_end  [b][l];
    unsigned int ad  = 1000*b+l;

    // Loop through all clusters from given detector element
    //
    InDet::TRT_DriftCircleCollection::const_iterator 
      c  = (*w)->begin(), ce = (*w)->end();
    for(; c!=ce; ++c) {

      if(m_useassoTool &&  m_assoTool->isUsed(*(*c))) continue;
      if(m_removeNoise && (*c)->isNoise()           ) continue;

      if(n >= m_cirsize) break;

      int               ns = m_trtid->straw((*c)->identify());
      const Amg::Vector3D& sc = (*c)->detectorElement()->strawCenter(ns);
      float             Fs = atan2(sc.y(),sc.x()); if(Fs<0.) Fs+=pi2;
      m_circles[n].set((*c),Fs,ad);
      
      // Loop through all dz/dr for given cluster 
      //
      for(unsigned int s=sb; s<=se; ++s) fillHistogramm (Fs,s);
      ++n;
    }
  }
  m_clusters = n;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::newRegion
(const std::vector<IdentifierHash>& vTRT)
{
  const float pi2 = 2.*M_PI;

  m_clusters     = 0;
  m_nlocal       = 0;
  m_nsegments    = 0;
  m_trtcontainer = 0; 
  m_bincluster .erase(m_bincluster .begin(),m_bincluster .end());
  m_sizebin    .erase(m_sizebin    .begin(),m_sizebin    .end());
  m_segments   .erase(m_segments.begin()   ,m_segments.end()   );
  m_segiterator   = m_segments.begin();

  if(!m_build) return;

  // Get drift cilrcles collection
  //
  StatusCode s = evtStore()->retrieve(m_trtcontainer,m_trtname);
  if(s.isFailure() && m_outputlevel<=0) {
    msg(MSG::DEBUG)<<"Could not get TRT_DriftCircleContainer"<<endreq;
    return;
  }
  if(!m_trtcontainer) return;

  // Initiate extension tool
  //
  m_extensionTool->newEvent();

  InDet::TRT_DriftCircleContainer::const_iterator  
    w,we = m_trtcontainer->end();

  eraseHistogramm(); 

  std::vector<IdentifierHash>::const_iterator d=vTRT.begin(),de=vTRT.end();
  int n = 0;
  for(; d!=de; ++d) {

    w = m_trtcontainer->indexFind((*d));

    for(; w!=we; ++w) {

      Identifier ID = (*w)->identify();
      int be = m_trtid->barrel_ec     (ID);
      int lw = m_trtid->layer_or_wheel(ID);
      int sl = m_trtid->straw_layer   (ID);
      
      int b; be < 0 ?  b = be+2 : b = be+1;
      int l  = m_flayers[b][lw]+sl;
      unsigned int sb  = m_begin[b][l];
      unsigned int se  = m_end  [b][l];
      unsigned int ad  = 1000*b+l;

      InDet::TRT_DriftCircleCollection::const_iterator 
	c  = (*w)->begin(), ce = (*w)->end();

      for(; c!=ce; ++c) {

	if(m_useassoTool &&  m_assoTool->isUsed(*(*c))) continue;
	if(m_removeNoise && (*c)->isNoise()           ) continue;
	
	if(n >= m_cirsize) break;
	
	int               ns = m_trtid->straw((*c)->identify());
	const Amg::Vector3D& sc = (*c)->detectorElement()->strawCenter(ns);
	float             Fs = atan2(sc.y(),sc.x()); if(Fs<0.) Fs+=pi2;
	m_circles[n].set((*c),Fs,ad);
	
	// Loop through all dz/dr for given cluster 
	//
	for(unsigned int s=sb; s<=se; ++s) fillHistogramm (Fs,s);
	++n;
      }
    }
  }
  m_clusters = n;
}

///////////////////////////////////////////////////////////////////
// Inform tool about end of event or region investigation
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::endEvent ()
{
  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }

}

///////////////////////////////////////////////////////////////////
// Methods for seeds production without vertex constraint
///////////////////////////////////////////////////////////////////
void InDet::TRT_TrackSegmentsMaker_ATLxk::find() 
{
  m_sizebin_iterator = m_sizebin.rbegin();

  if(m_clusters<m_clustersCut) return;

  m_clusterSegment.erase(m_clusterSegment.begin(),m_clusterSegment.end());
  m_qualitySegment.erase(m_qualitySegment.begin(),m_qualitySegment.end());

  unsigned int mc = m_clusters;
  for(unsigned int n=0; n!=mc; ++n) {

    
    unsigned int b  = m_circles[n].buffer();
    unsigned int l  = m_circles[n].layer ();
    unsigned int sb = m_begin[b][l];
    unsigned int se = m_end  [b][l];
    
    // Loop through all dz/dr for given cluster 
    //
    unsigned char max    = 0;
    unsigned int  maxbin = 0;

    for(unsigned int s=sb; s<=se; ++s) {
      analyseHistogramm(max,maxbin,m_circles[n].phi(),s);
    }

    if(int(max) > m_clustersCut) {
      m_bincluster.insert(std::make_pair(localMaximum(maxbin),n));
    }
  }

  
  std::multimap<unsigned int,unsigned int>::iterator
    bc,bce =m_bincluster.end();

  unsigned int nbins = 0       ;
  unsigned int fbin  = 99999999; 
  for(bc = m_bincluster.begin(); bc!=bce; ++bc) {

    if((*bc).first==fbin) ++nbins;
    else     {
      if(fbin!=99999999 && nbins>=5) m_sizebin.insert(std::make_pair(nbins,fbin));
      fbin=(*bc).first; nbins = 1;
    }
  }
  if(fbin!=99999999 && nbins>=5) m_sizebin.insert(std::make_pair(nbins,fbin));
  m_sizebin_iterator = m_sizebin.rbegin();

  // Local reconstruction and track segments production
  //
  while(m_sizebin_iterator!=m_sizebin.rend()) {

    unsigned int bin =(*m_sizebin_iterator++).second; 
    findLocaly(bin); 
  }

  // Final segments preparation
  //
  segmentsPreparation();

  m_segiterator = m_segments.begin();
}

///////////////////////////////////////////////////////////////////
// Pseudo iterator
///////////////////////////////////////////////////////////////////

Trk::TrackSegment* InDet::TRT_TrackSegmentsMaker_ATLxk::next()
{
  return (m_segiterator!=m_segments.end()) ? (*m_segiterator++) : 0;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsMaker_ATLxk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)
    return dumpEvent(out);
  else
    return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsMaker_ATLxk::dumpConditions( MsgStream& out ) const
{

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  int n  = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_trtname.size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  n     = 62-m_propTool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

  n     = 62-m_extensionTool.type().size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");

  n     = 62-m_assoTool.type().size();
  std::string s8; for(int i=0; i<n; ++i) s8.append(" "); s8.append("|");


  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  out<<"| Tool for propagation    | "<<m_propTool     .type()<<s1<<std::endl;
  out<<"| Tool tracks extension   | "<<m_extensionTool.type()<<s7<<std::endl;    
  out<<"| Tool track-prd associa  | "<<m_assoTool     .type()<<s8<<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]       <<s3<<std::endl;
  out<<"| TRT container           | "<<m_trtname             <<s4<<std::endl;
  out<<"| Min. number straws      | "
     <<std::setw(12)<<m_clustersCut
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. bar. layers | "
     <<std::setw(12)<<m_nlayers[1]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. bar. layers | "
     <<std::setw(12)<<m_nlayers[2]
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. end. layers | "
     <<std::setw(12)<<m_nlayers[0]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. end. layers | "
     <<std::setw(12)<<m_nlayers[3]
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. bar. straws | "
     <<std::setw(12)<<m_nstraws[1]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. bar. straws | "
     <<std::setw(12)<<m_nstraws[2]
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. end. straws | "
     <<std::setw(12)<<m_nstraws[0]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. end. straws | "
     <<std::setw(12)<<m_nstraws[3]
     <<"                                                  |"<<std::endl;
  out<<"| Number azimut. channels | "
     <<std::setw(12)<<m_nPhi
     <<"                                                  |"<<std::endl;
  out<<"| Number moment. channels | "
     <<std::setw(12)<<m_nMom
     <<"                                                  |"<<std::endl;
  out<<"| Number histog. channels | "
     <<std::setw(12)<<m_histsize
     <<"                                                  |"<<std::endl;
  out<<"| Number cluster links    | "
     <<std::setw(12)<<m_cirsize
     <<"                                                  |"<<std::endl;
  out<<"| Use association tool ?  | "
     <<std::setw(12)<<m_useassoTool 
     <<"                                                  |"<<std::endl;
  out<<"| Remove noise   ?        | "
     <<std::setw(12)<<m_removeNoise
     <<"                                                  |"<<std::endl;     
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsMaker_ATLxk::dumpEvent( MsgStream& out ) const
{
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Number drift circles    | "
     <<std::setw(12)<<m_clusters
     <<"                                                  |"<<std::endl;
  out<<"| Number local calls      | "
     <<std::setw(12)<<m_nlocal
     <<"                                                  |"<<std::endl;
  out<<"| Number found segments   | "
     <<std::setw(12)<<m_nsegments
     <<"                                                  |"<<std::endl;
  out<<"| Number save  segments   | "
     <<std::setw(12)<<m_segments.size()
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackSegmentsMaker_ATLxk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_TrackSegmentsMaker_ATLxk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_TrackSegmentsMaker_ATLxk& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Map of straws production
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::mapStrawsProduction()
{
  m_build = true;

  if(m_ndzdr  ) delete [] m_ndzdr  ;
  if(m_slope  ) delete [] m_slope  ;
  if(m_islope ) delete [] m_islope ;
  if(m_circles) delete [] m_circles;

  m_Ts          = m_nPhi*m_nMom                       ;
  m_Psi         = 2./float(m_nMom-1)                  ;
  m_Psi128      = m_Psi*128.                          ;
  m_Ns128       = m_nPhi*128                          ;
  m_A           = float(m_nPhi)/(2.*M_PI)             ;
  m_Ai          = 1./m_A                              ;
  m_histsize    = (m_nPhi*m_nMom*26/4)                ;
  
  m_nlayers[0]=0; m_nstraws[0]=0; 
  m_nlayers[1]=0; m_nstraws[1]=0; 
  m_nlayers[2]=0; m_nstraws[2]=0; 
  m_nlayers[3]=0; m_nstraws[3]=0; 

  float RZ   [4][200];
  float Tmin [4][200];
  float Tmax [4][200];

  // Map straws production for barrel geometry
  //
  int Rings  = m_trtmgr->getNumerology()->getNBarrelRings();
  int NPhi   = m_trtmgr->getNumerology()->getNBarrelPhi(); 
  int n      = 0;
 
  for(int ring = 0; ring!=Rings; ++ring) {
    
    int NSlayers = m_trtmgr->getNumerology()->getNBarrelLayers(ring);

    m_flayers[1][ring] = m_nlayers[1]; m_flayers[2][ring] = m_nlayers[2]; 
    m_nlayers[1]      += NSlayers    ; m_nlayers[2]      += NSlayers    ;

    for(int nsl=0; nsl!=NSlayers; ++nsl) {
      

      for(int f=0; f!=NPhi; ++f) {

	const InDetDD::TRT_BaseElement*   base1 = m_trtmgr->getBarrelElement(0,ring,f,nsl);
	const InDetDD::TRT_BaseElement*   base2 = m_trtmgr->getBarrelElement(1,ring,f,nsl);
	if(!base1 || !base2) continue;
	const InDetDD::TRT_BarrelElement* bael1 =  
	  dynamic_cast<const InDetDD::TRT_BarrelElement*>(base1);
	const Trk::RectangleBounds* rb1 = 
	  dynamic_cast<const Trk::RectangleBounds*>(&base1->bounds());
	const Trk::RectangleBounds* rb2 = 
	  dynamic_cast<const Trk::RectangleBounds*>(&base2->bounds());
	if(!bael1 || !rb1 || !rb2) continue;
	float rmean = 0;
	if(f==0) {

	  Amg::Vector3D  C1  = base1->center(); 
	  Amg::Vector3D  C2  = base2->center();
 	  RZ   [1][n] = sqrt(C1.x()*C1.x()+C1.y()*C1.y());
	  //Tmin [1][n] = (C1.z()-rb1->halflengthY())/RZ[1][n];
	  //Tmax [1][n] = (C1.z()+rb1->halflengthY())/RZ[1][n];

	  Tmin [1][n] = (C1.z()-rb1->halflengthY())/RZ[1][n];
	  Tmax [1][n] = -.001;

	  RZ   [2][n] = sqrt(C2.x()*C2.x()+C2.y()*C2.y());
	  //Tmin [2][n] = (C2.z()-rb2->halflengthY())/RZ[2][n];
	  //Tmax [2][n] = (C2.z()+rb2->halflengthY())/RZ[2][n];

	  Tmin [2][n] = +.001;
	  Tmax [2][n] = (C2.z()+rb2->halflengthY())/RZ[2][n];
	  ++n;
	}
	int ns = bael1->nStraws();
	for(int s=0; s!=ns; ++s) {
	  
	  const Identifier        id1 = m_trtid->straw_id(-1,f,ring,nsl,s );
	  const Amg::Vector3D   * sc1 = &(base1->strawCenter           (s));
	  const Amg::Transform3D* st1 = &(base1->strawTransform        (s)); 
	  const Amg::Transform3D* tr1 = &(base1->surface(id1).transform() );
	  if(f==0) rmean+=sqrt(sc1->x()*sc1->x()+sc1->y()*sc1->y());

	  if(!sc1 || !st1 || !tr1 ) {std::cout<<"problem with TRT geometry"<<std::endl;}
	  ++m_nstraws[1];
	  const Identifier        id2 = m_trtid->straw_id(+1,f,ring,nsl,s );
	  const Amg::Vector3D   * sc2 = &(base2->strawCenter           (s));
	  const Amg::Transform3D* st2 = &(base2->strawTransform        (s)); 
	  const Amg::Transform3D* tr2 = &(base2->surface(id2).transform() );
	  if(!sc2 || !st2 || !tr2)  {std::cout<<"problem with TRT geometry"<<std::endl;}
	  ++m_nstraws[2];
	}
	if(f==0) { RZ[1][n-1] = RZ[2][n-1] = rmean/float(ns);}
      }
    }
  }

  // Endcap
  //
  int Wheels = m_trtmgr->getNumerology()->getNEndcapWheels(); if(!Wheels) return;
  NPhi       = m_trtmgr->getNumerology()->getNEndcapPhi   (); 
  n          = 0; 
  for(int wh = 0; wh!=Wheels; ++wh) {

    int NSlayers = m_trtmgr->getNumerology()->getNEndcapLayers(wh);
    m_flayers[0][wh] = m_nlayers[0]; m_flayers[3][wh] = m_nlayers[3];
    m_nlayers[0]    += NSlayers    ; m_nlayers[3]     += NSlayers   ;

    for(int nsl = 0; nsl!=NSlayers; ++nsl) {

      for(int f=0; f!=NPhi; ++f) {

	const InDetDD::TRT_BaseElement* base1 = m_trtmgr->getEndcapElement(0,wh,nsl,f);
	const InDetDD::TRT_BaseElement* base2 = m_trtmgr->getEndcapElement(1,wh,nsl,f);
	if(!base1 || !base2) continue;

	const InDetDD::TRT_EndcapElement* enel1 =  
	  dynamic_cast<const InDetDD::TRT_EndcapElement*>(base1);
	const Trk::DiscBounds* db1 = 
	  dynamic_cast<const Trk::DiscBounds*>(&base1->bounds());
	const Trk::DiscBounds* db2 = 
	  dynamic_cast<const Trk::DiscBounds*>(&base2->bounds());
	if(!enel1 || !db1 || !db2) continue;
	if(f==0) {

	  Amg::Vector3D  C1  = base1->center(); 
	  Amg::Vector3D  C2  = base2->center(); 
	  RZ   [0][n] = C1.z();
	  Tmin [0][n] = RZ[0][n]/db1->rMin(); 
	  Tmax [0][n] = RZ[0][n]/db1->rMax(); 
	  RZ   [3][n] = C2.z();
	  Tmin [3][n] = RZ[3][n]/db2->rMax();
	  Tmax [3][n] = RZ[3][n]/db2->rMin();
	  ++n;
	}

	int ns = enel1->nStraws();

	for(int s=0; s!=ns; ++s) {

	  const Identifier        id1 = m_trtid->straw_id(-2,f,wh,nsl,s  );
	  const Amg::Vector3D   * sc1 = &(base1->strawCenter         (s) );
	  const Amg::Transform3D* st1 = &(base1->strawTransform      (s) ); 
	  const Amg::Transform3D* tr1 = &(base1->surface(id1).transform());

	  if(!sc1 || !st1 || !tr1) {std::cout<<"problem with TRT geometry"<<std::endl;}
	  ++m_nstraws[0];

	  const Identifier        id2 = m_trtid->straw_id(+2,f,wh,nsl,s  );
	  const Amg::Vector3D   * sc2 = &(base2->strawCenter         (s) );
	  const Amg::Transform3D* st2 = &(base2->strawTransform      (s) ); 
	  const Amg::Transform3D* tr2 = &(base2->surface(id2).transform());

	  if(!sc2 || !st2 || !tr2) {std::cout<<"problem with TRT geometry"<<std::endl;}
	  ++m_nstraws[3];
	}
      }
    }
  }
  
  float rapidity[26]=
    {-2.05,-1.95,-1.84,-1.72,-1.62,-1.53,-1.43,-1.33,-1.21,-1.00,-.94, -.85,-.32,
       .32,  .85,  .94, 1.00, 1.21, 1.33, 1.43, 1.53, 1.62, 1.72,1.84, 1.95,2.05};  
  

  double zmax = RZ[3][m_nlayers[3]-1]+10.;
  double rmax = RZ[2][m_nlayers[2]-1]+10.;
  const Trk::CylinderBounds CB(rmax,zmax);

  std::list<Amg::Vector3D> G [26]; 
  Amg::Vector3D PSV(0.,0.,0.); Trk::PerigeeSurface PS(PSV);

  for(int r=0; r!=26; ++r) {
 
    m_dzdr[r]   = 1./tan(2.*atan(exp(-rapidity[r])));
    double pinv =-1./(m_pTmin*sqrt(1.+m_dzdr[r]*m_dzdr[r]));

    const Trk::TrackParameters* Tp = PS.createTrackParameters(0.,0.,0.,atan2(1.,double(m_dzdr[r])),pinv,0);
    m_propTool->globalPositions(G[r],*Tp,m_fieldprop,CB,5.,Trk::pion);
    delete Tp;
  }
  
  n = 0;
  for(int b=0; b!=4; ++b) {

    for(unsigned int i=0; i!=m_nlayers[b]; ++i) {

      m_begin[b][i] = n;
      for(int r=0; r!=26; ++r) {
	if( m_dzdr[r] >= Tmin[b][i] && m_dzdr[r] <= Tmax[b][i] ) {
	  m_end[b][i] = n++;
	}
      }
    }
  }

  m_ndzdr    = new unsigned int[n];
  m_islope   = new          int[n];
  m_slope    = new float       [n];
  m_cirsize  = m_nstraws[0]+m_nstraws[1];
  m_circles  = new TRT_DriftCircleLinkN_xk[m_cirsize];

  n          = 0;
  for(int b=0; b!=4; ++b) {

    for(unsigned int i=0; i!=m_nlayers[b]; ++i) {

      for(int r=0; r!=26; ++r) {

	if( m_dzdr[r] >= Tmin[b][i] && m_dzdr[r] <= Tmax[b][i] ) {
	  m_ndzdr[n] = r;
	  std::list<Amg::Vector3D>::iterator gp0,gp1, gp=G[r].begin(),gpe=G[r].end();
	  if   (b==0 || b==3) {

	    gp0 = gp;
	    for(++gp; gp!=gpe; ++gp) {
	      if(b==3 && (*gp).z() >= RZ[b][i]) break; 
	      if(b==0 && (*gp).z() <= RZ[b][i]) break;
	      gp1 = gp0;
	      gp0 = gp; 
	    }
	  }
	  else {
	    gp0 = gp;
	    for(++gp; gp!=gpe; ++gp) {
	      if(sqrt((*gp).x()*(*gp).x()+(*gp).y()*(*gp).y()) > RZ[b][i]) break;
	      gp1 = gp0;
	      gp0 = gp; 
	    }
	  }
	  double x,y,z,ax,ay,az;
	  if(gp!=gpe) {
	    x = (*gp0).x(); ax = (*gp ).x()-x;
	    y = (*gp0).y(); ay = (*gp ).y()-y;
	    z = (*gp0).z(); az = (*gp ).z()-z;
	    double as = 1./sqrt(ax*ax+ay*ay+az*az); ax*=as; ay*=as; az*=as;
	  }
	  else       {
	    x = (*gp1).x(); ax = (*gp0).x()-x;
	    y = (*gp1).y(); ay = (*gp0).y()-y;
	    z = (*gp1).z(); az = (*gp0).z()-z;
	    double as = 1./sqrt(ax*ax+ay*ay+az*az); ax*=as; ay*=as; az*=as;
	  }
	  double S  = 0;

	  if (b==0 || b==3) {
	    S = (RZ[b][i]-z)/az;
	  }
	  else              {
	    double A  = (ax*x+ay*y)*2.;
	    double D  = (RZ[b][i]-x-y)*(RZ[b][i]+x+y)+2.*x*y;
	    S         = D/A;
	    double B  = 2.*(ax*ax+ay*ay);
	    double Sq = A*A+2.*D*B;  Sq>0. ? Sq=sqrt(Sq) : Sq=0.;
	    double S1 =-(A+Sq)/B;
	    double S2 =-(A-Sq)/B;
	    S>S2 ? S=S2 : S<S1 ? S=S1 : S=S;
	  }
	  m_slope [n] = atan2(y+S*ay,x+S*ax)*m_A; 
	  m_islope[n] = int(m_slope[n]*m_Psi128);
	  ++n;
	}
      }
    }
  }
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Erase histogramm
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::eraseHistogramm()
{
  for(int i=0; i!=m_histsize; i+=10) {

    m_U.H4[i  ]=0;

    m_U.H4[i+1]=0;
    m_U.H4[i+2]=0;
    m_U.H4[i+3]=0;
    m_U.H4[i+4]=0;
    m_U.H4[i+5]=0;
    m_U.H4[i+6]=0;
    m_U.H4[i+7]=0;
    m_U.H4[i+8]=0;
    m_U.H4[i+9]=0;
  }
}

///////////////////////////////////////////////////////////////////
// Fill histogramm
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::fillHistogramm 
(float Fs,int s)
{
  int s0 = m_ndzdr[s]*m_Ts;
  int f  = int((Fs*m_A-m_slope[s])*128.); if(f<0) f+=m_Ns128;
  int sf = m_islope[s];

  // Loop through all momentum slopes
  //
  for(int i=s0; i!=s0+m_Ts; i+=m_nPhi) {
    int k =(f>>7); f+=sf; k<m_nPhi ? ++m_U.H[k+i] : ++m_U.H[k+i-m_nPhi];
  }
}


///////////////////////////////////////////////////////////////////
// Analyse histogramm
///////////////////////////////////////////////////////////////////
 
void InDet::TRT_TrackSegmentsMaker_ATLxk::analyseHistogramm 
(unsigned char& max,unsigned int& maxbin,float Fs,int s)
{
  int s0     = m_ndzdr[s]*m_Ts;
  int f      = int((Fs*m_A-m_slope[s])*128.); if(f<0) f+=m_Ns128;
  int sf     = m_islope[s];

  // Loop through all momentum slopes
  //
  for(int i=s0; i!=s0+m_Ts; i+=m_nPhi) {
    int k =(f>>7); f+=sf; if(k>=m_nPhi) k-=m_nPhi;
    if(m_U.H[k+i] > max) max = m_U.H[maxbin = k+i];
  }
}

///////////////////////////////////////////////////////////////////
// TRT seeds production
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::findLocaly(unsigned int bin)
{
  const double pi=M_PI, pi2 = 2.*M_PI;
   
  std::multimap<const InDet::TRT_DriftCircle*,Trk::TrackSegment*>::const_iterator
    cse = m_clusterSegment.end();

  std::multimap<unsigned int,unsigned int>::iterator 
    bc                          ,
    bcb = m_bincluster.find(bin),
    bce = m_bincluster.end()    ;
  int nfree = 0;
  for(bc=bcb; bc!=bce; ++bc) {

    if((*bc).first!=bin) break;
    if(m_clusterSegment.find(m_circles[(*bc).second].circle())==cse) ++nfree;
  }
  if(nfree<5) return;
  unsigned int ndzdr = bin/m_Ts;
  unsigned int b     = bin-ndzdr*m_Ts;
  unsigned int m     = b/m_nPhi;
  float        c0    = (1.-float(m)*m_Psi)*m_Ai;
  
  bce=bc;
  float fm    = 0.   ;
  float Fo    = 0.   ;
  bool  first = false;
  for(bc=bcb; bc!=bce; ++bc) {

    unsigned int n = (*bc).second;
    
    unsigned int b  = m_circles[n].buffer();
    unsigned int l  = m_circles[n].layer ();
    unsigned int s  = m_begin[b][l];
    unsigned int se = m_end  [b][l];
    
    for(; s<= se; ++s) {if(m_ndzdr[s]==ndzdr) break;}
    if(s>se) continue;
    float F  = m_circles[n].phi()-m_slope[s]*c0;

    if(!first) {
      Fo = F; first = true;
    }
    else       {
      float df = F-Fo;
      if     (df > pi) df-=pi2; 
      else if(df <-pi) df+=pi2; 
      fm+=df;
    }
  }
  fm = Fo+fm/float(nfree);

  if     (fm > pi) fm = fmod(fm+pi,pi2)-pi;
  else if(fm <-pi) fm = fmod(fm-pi,pi2)+pi;

  double pT = m_pTmin/(double(m)*m_Psi-1.);

  double pin = 1./(pT*sqrt((1.+m_dzdr[ndzdr]*m_dzdr[ndzdr])));

  Amg::Vector3D PSV(0.,0.,0.); Trk::PerigeeSurface PS(PSV);
  const Trk::TrackParameters* Tp = PS.createTrackParameters(0.,0.,fm,atan2(1.,m_dzdr[ndzdr]),pin,0);
    ++m_nlocal;

  Trk::TrackSegment* seg = m_extensionTool->findSegment(*Tp);
  delete Tp;
  if(!seg) return;

  // Momentum cut
  //
  double T  = seg->localParameters().get(Trk::theta );
  double iP = seg->localParameters().get(Trk::qOverP);

  // ME: let's not use a soft cut here
  if(sin(T) < 0.9*m_pTmin*fabs(iP)) {delete seg; return;}


  ++m_nsegments;

  bool isbarrel=false;
  const Trk::MeasurementBase *lastmeas=seg->containedMeasurements().back();
  if (std::abs(lastmeas->globalPosition().z())<750.) isbarrel=true;
  // Number drift circles test
  //
  unsigned int size = isbarrel ? seg->numberOfMeasurementBases()-1 : seg->numberOfMeasurementBases()-2;

  if(int(size) < m_clustersCut) {delete seg; return;}

  std::vector<const Trk::MeasurementBase*>::const_iterator
    s = seg->containedMeasurements().begin(), se = seg->containedMeasurements().end();

  if(m_useassoTool) {
    
    // Test number unused drift circles 
    //
    int nu = 0, ntot = 0;
    for(++s; s!=se; ++s) {

      const Trk::RIO_OnTrack*  rio = dynamic_cast<const Trk::RIO_OnTrack*>(*s);
      if(rio) {
	++ntot;
	if (m_assoTool->isUsed(*rio->prepRawData())) ++nu;
      }
    }
    // ME: use fraction cut
    if( nu > int(ntot*m_sharedfrac))  {delete seg; return;}
  }

  // Save information about qiality and segment
  //
  int    quality = size;
  double Xi2     = seg->fitQuality()->chiSquared()/double(seg->fitQuality()->numberDoF());  

  if(Xi2 > .5) {
    if     (Xi2 > 6)  quality-=9;
    else if(Xi2 > 5.) quality-=7;
    else if(Xi2 > 3.) quality-=5;
    else if(Xi2 > 2.) quality-=3;
    else if(Xi2 > 1.) quality-=1;
  }
 
  m_qualitySegment.insert(std::make_pair(quality,seg));

  s = seg->containedMeasurements().begin();

  // Save information about TRT clusters and segments
  //
  for(++s; s!=se; ++s) {
    const Trk::RIO_OnTrack*  rio = dynamic_cast<const Trk::RIO_OnTrack*>(*s);
    if (!rio) continue;
    const InDet::TRT_DriftCircle* dc = ((const InDet::TRT_DriftCircleOnTrack*)(*s))->prepRawData();
    if(dc) m_clusterSegment.insert(std::make_pair(dc,seg));
  }
}

///////////////////////////////////////////////////////////////////
// Remove fake TRT segments
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::segmentsPreparation()
{
  std::multimap<int,Trk::TrackSegment*>::reverse_iterator 
    qs = m_qualitySegment.rbegin();

  for(; qs!=m_qualitySegment.rend(); ++qs) {

    int nfree = 0;

    std::vector<const Trk::MeasurementBase*>::const_iterator
      s  =(*qs).second->containedMeasurements().begin(), 
      se =(*qs).second->containedMeasurements().end  ();

    for(++s; s!=se; ++s) {
      const Trk::RIO_OnTrack*  rio = dynamic_cast<const Trk::RIO_OnTrack*>(*s);
    if (!rio) continue;

      const InDet::TRT_DriftCircle* dc = ((const InDet::TRT_DriftCircleOnTrack*)(*s))->prepRawData();
      if(dc && m_clusterSegment.erase(dc)) ++nfree; 
    }
    if(nfree >= 7) m_segments.push_back((*qs).second); 
    else           delete               (*qs).second;
  }
}

///////////////////////////////////////////////////////////////////
// Local maimum
///////////////////////////////////////////////////////////////////

unsigned int InDet::TRT_TrackSegmentsMaker_ATLxk::localMaximum
(unsigned int bin)
{
  int           b    = bin-(bin/m_Ts)*m_Ts;
  int           m    = b/m_nPhi           ;
  int           f    = b-m*m_nPhi         ;
  unsigned int  maxb = bin                ;
  unsigned char max  = m_U.H[bin]         ;

  int a1 = bin-1; if(f==   0    ) a1+=m_nPhi; if(m_U.H[a1]>max) {max=m_U.H[a1]; maxb=a1;}  
  int a2 = bin+1; if(f==m_nPhi-1) a2-=m_nPhi; if(m_U.H[a2]>max) {max=m_U.H[a2]; maxb=a2;}   

  if ( m < m_nMom-1) {

    int a = bin+m_nPhi; if(m_U.H[a]>max) {max=m_U.H[a]; maxb=a;} 
    a     = a1 +m_nPhi; if(m_U.H[a]>max) {max=m_U.H[a]; maxb=a;} 
    a     = a2 +m_nPhi; if(m_U.H[a]>max) {max=m_U.H[a]; maxb=a;} 
    
  }
  if ( m  >  0     ) {

    int a = bin-m_nPhi; if(m_U.H[a]>max) {max=m_U.H[a]; maxb=a;} 
    a     = a1 -m_nPhi; if(m_U.H[a]>max) {max=m_U.H[a]; maxb=a;} 
    a     = a2 -m_nPhi; if(m_U.H[a]>max) {max=m_U.H[a]; maxb=a;} 
  }
  return maxb;
}

///////////////////////////////////////////////////////////////////
// Map straws update
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMaker_ATLxk::mapStrawsUpdate
(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  (void) I;

  bool needsUpdate = false;
  for (std::list<std::string>::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
    if ((*k) == m_callbackString) {needsUpdate = true; break;}
  } 
  if(!needsUpdate) return StatusCode::SUCCESS;

  m_gupdate = true; mapStrawsProduction();
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::magneticFieldInit()
{
  // Build MagneticFieldProperties 
  //
  Trk::MagneticFieldProperties* pMF = 0;
  if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
  m_fieldprop = *pMF; delete pMF;
}
