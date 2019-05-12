/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Source file for interface SiZvertexMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 30/10/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "SiZvertexTool_xk/SiZvertexMaker_xk.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiZvertexMaker_xk::SiZvertexMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
  m_seedsgenerator("InDet::SiSpacePointsSeedMaker_ATLxk")
{
  m_nspoint   =  2                                   ;
  m_histsize  =  500                                 ;  
  m_histogram =  0                                   ;  
  z_histogram =  0                                   ;
  m_zmin      = -250.                                ;
  m_zmax      = +250.                                ;
  m_ratio     = .25                                  ;
  m_mincontent=   20                                 ;
  m_maxcontent=   0                                  ;

  declareInterface<ISiZvertexMaker>(this)            ;

  declareProperty("SeedMakerTool",m_seedsgenerator ) ;             
  declareProperty("SeedSize"     ,m_nspoint   )      ;             
  declareProperty("HistSize"     ,m_histsize  )      ;
  declareProperty("minContent"   ,m_mincontent)      ;
  declareProperty("minRatio"     ,m_ratio     )      ;
  declareProperty("Zmin"         ,m_zmin      )      ;
  declareProperty("Zmax"         ,m_zmax      )      ;
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiZvertexMaker_xk::~SiZvertexMaker_xk()
{
  if(m_histogram) delete [] m_histogram;
  if(z_histogram) delete [] z_histogram;
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiZvertexMaker_xk::initialize()
{

  // Get tool for seed generator
  //
  if ( m_seedsgenerator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_seedsgenerator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_seedsgenerator << endreq;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiZvertexMaker_xk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}
///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiZvertexMaker_xk::newEvent()
{
  if(!m_histogram) {
    m_histogram = new int   [m_histsize+1];
    z_histogram = new double[m_histsize+1];
  }
  m_vertex.erase(m_vertex.begin(),m_vertex.end());
  m_seedsgenerator->newEvent();
  production();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiZvertexMaker_xk::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT )
{
  if(!m_histogram) {
    m_histogram = new int   [m_histsize+1];
    z_histogram = new double[m_histsize+1];
  }
  m_vertex.erase(m_vertex.begin(),m_vertex.end());
  m_seedsgenerator->newRegion(vPixel,vSCT);
  production();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region with ROI parameters (PhRt)
///////////////////////////////////////////////////////////////////

void InDet::SiZvertexMaker_xk::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor& PhEt)
{
  if(!m_histogram) {
    m_histogram = new int   [m_histsize+1];
    z_histogram = new double[m_histsize+1];
  }
  m_vertex.erase(m_vertex.begin(),m_vertex.end());
  m_seedsgenerator->newRegion(vPixel,vSCT,PhEt);
  production();
}

///////////////////////////////////////////////////////////////////
// Get list vertices Z-coordinates
///////////////////////////////////////////////////////////////////

const std::list<Trk::Vertex>&  InDet::SiZvertexMaker_xk::getVertices () 
{
  return m_vertex;
}

///////////////////////////////////////////////////////////////////
// Get list vertices Z-coordinates
///////////////////////////////////////////////////////////////////

void InDet::SiZvertexMaker_xk::production() 
{
  double sZ = double(m_histsize)/(m_zmax-m_zmin);
  for(int i=0; i<=m_histsize; ++i) {m_histogram[i]=0; z_histogram[i]=0.;}

  std::list<int>    NMAX;
  std::list<double> ZMAX;
  const Trk::SpacePoint*  p0 = 0;

  std::list<Trk::Vertex> lv;  
  
  if     (m_nspoint==2) m_seedsgenerator->find2Sp(lv);
  else if(m_nspoint==3) m_seedsgenerator->find3Sp(lv);
  else                  m_seedsgenerator->findVSp(lv);

  const InDet::SiSpacePointsSeed* seed = 0;
  std::multimap<int,double> ver;

  int    nmax = 0 ;
  int    Hmax = 0 ;
  double zmax = 0.;

  while((seed = m_seedsgenerator->next())) {

    std::list<const Trk::SpacePoint*>::const_iterator 
      s = seed->spacePoints().begin(); ++s;
    if((*s)!=p0) {
      if(Hmax!=0) {NMAX.push_back(nmax); ZMAX.push_back(zmax);}
      Hmax=nmax=0; p0=(*s);
    }
    int n = int((seed->zVertex()-m_zmin)*sZ);
    if(n>=0 && n<=m_histsize) {
      if(++m_histogram[n]>Hmax) {Hmax=m_histogram[n]; nmax=n; zmax=seed->zVertex();}
    } 
  } 

  // Clear histogram
  //
  for(int i=0; i<=m_histsize; ++i) m_histogram[i]=0;

  // Fill histogram again using NMAX information
  //
  std::list<int>   ::iterator m,me=NMAX.end();
  std::list<double>::iterator z = ZMAX.begin();
  for(m=NMAX.begin(); m!=me; ++m) {
    ++m_histogram[(*m)]; z_histogram[(*m)]+=(*z++); 
  }
   
  // Max. content seach and output bank Z coordinates of vertices production
  //
  int HiZm = 0; 
  int im   = 0;
  int i    = 1;
  while(1) {

    if(m_histogram[i]>=m_histogram[i-1] && m_histogram[i]>=m_histogram[i+1]) {
      int H3 = m_histogram[i-1]+m_histogram[i]+m_histogram[i+1]; 
      if(H3>HiZm) {HiZm=H3; im=i;}
      if(++i==m_histsize) break;
    }
    if(++i==m_histsize) break;
  }

  m_maxcontent = HiZm;  
  if(im==0 || HiZm < m_mincontent) {

    if(m_outputlevel<=0) {
      m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
    }
    return;
  }
  double zv = (z_histogram[im]+z_histogram[im-1]+z_histogram[im+1])/double(HiZm);
  ver.insert(std::make_pair(-HiZm,zv));

  // Z-vertex coordinates 
  //
  HiZm = int(m_ratio*double(HiZm)); if(HiZm<m_mincontent) HiZm = m_mincontent;
  i    = 1;
  while(1) {

    if(m_histogram[i]>=m_histogram[i-1] && m_histogram[i]>=m_histogram[i+1] && i!=im) { 

      int H3 = m_histogram[i-1]+m_histogram[i]+m_histogram[i+1]; 
      if(H3>=HiZm) {
	zv = (z_histogram[i]+z_histogram[i-1]+z_histogram[i+1])/double(H3);
	ver.insert(std::make_pair(-H3,zv));
     }
      if(++i==m_histsize) break;
    }
    if(++i==m_histsize) break;
  }

  if(!ver.empty()) {

    std::multimap<int,double>::iterator v = ver.begin(), ve = ver.end();
      for(; v!=ve; ++v) {
	Amg::Vector3D Vp(0.,0.,(*v).second); Trk::Vertex Ver(Vp);
	m_vertex.push_back(Ver);
    }
  }

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}


///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiZvertexMaker_xk::dump( MsgStream& out ) const
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

MsgStream& InDet::SiZvertexMaker_xk::dumpConditions( MsgStream& out ) const
{
  int n = 62-m_seedsgenerator.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");


  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| SP seeds maker tool     | "
     <<m_seedsgenerator.type()<<s1<<std::endl;
  out<<"| Histogram size          | "
     <<std::setw(12)<<m_histsize
     <<"                                                  |"<<std::endl;
  out<<"| SP seed length (2 or 3) | "
     <<std::setw(12)<<m_nspoint
     <<"                                                  |"<<std::endl;
  out<<"| min content of hist.bin | "
     <<std::setw(12)<<m_mincontent
     <<"                                                  |"<<std::endl;
  out<<"| min vertex Z-coordinate | "
     <<std::setw(12)<<std::setprecision(4)<<m_zmin
     <<"                                                  |"<<std::endl;
  out<<"| max vertex Z-coordinate | "
     <<std::setw(12)<<std::setprecision(4)<<m_zmax
     <<"                                                  |"<<std::endl;
  out<<"| min. ratio to max bin   | "
     <<std::setw(12)<<std::setprecision(4)<<m_ratio
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiZvertexMaker_xk::dumpEvent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| max content of hist.bin | "
     <<std::setw(12)<<m_maxcontent
     <<"                              |"<<std::endl;
  out<<"| number of vertices      | "
     <<std::setw(12)<<m_vertex.size()
     <<"                              |"<<std::endl;
  std::list<Trk::Vertex>::const_iterator v,ve = m_vertex.end();  
  for(v=m_vertex.begin(); v!=ve; ++v) {
    out<<"| Vertex(x,y,z) mm        | "
       <<std::setw(12)<<std::setprecision(5)<<(*v).position().x()
       <<std::setw(12)<<std::setprecision(5)<<(*v).position().y()
       <<std::setw(12)<<std::setprecision(5)<<(*v).position().z()
      <<"      |"
      <<std::endl;
  }
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiZvertexMaker_xk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiZvertexMaker_xk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiZvertexMaker_xk& se)
{ 
  return se.dump(sl); 
}   


