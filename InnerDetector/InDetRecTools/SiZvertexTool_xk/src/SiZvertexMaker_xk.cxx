/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Source file for interface SiZvertexMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 30/10/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiZvertexTool_xk/SiZvertexMaker_xk.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "VxVertex/Vertex.h"

#include <iomanip>
#include <map>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiZvertexMaker_xk::SiZvertexMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiZvertexMaker_xk::initialize()
{
  // Get tool for seed generator
  //
  ATH_CHECK(m_seedsgenerator.retrieve());

  dump(msg(MSG::DEBUG));

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiZvertexMaker_xk::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

std::list<Trk::Vertex> InDet::SiZvertexMaker_xk::newEvent(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data) const
{
  m_seedsgenerator->newEvent(ctx, data);
  return production(ctx, data);
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

std::list<Trk::Vertex> InDet::SiZvertexMaker_xk::newRegion
(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const
{
  m_seedsgenerator->newRegion(ctx, data, vPixel, vSCT);
  return production(ctx, data);
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region with ROI parameters (PhRt)
///////////////////////////////////////////////////////////////////

std::list<Trk::Vertex> InDet::SiZvertexMaker_xk::newRegion
(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
 const IRoiDescriptor& PhEt) const
{
  m_seedsgenerator->newRegion(ctx, data, vPixel, vSCT, PhEt);
  return production(ctx, data);
}

///////////////////////////////////////////////////////////////////
// Get list vertices Z-coordinates
///////////////////////////////////////////////////////////////////

std::list<Trk::Vertex> InDet::SiZvertexMaker_xk::production(const EventContext& ctx,
                                                            SiSpacePointsSeedMakerEventData& data) const
{
  std::list<Trk::Vertex> vertices;

  std::vector<int> histogram(m_histsize+1, 0);
  std::vector<double> z_histogram(m_histsize+1, 0.);

  double sZ = static_cast<double>(m_histsize)/(m_zmax-m_zmin);

  std::list<int>    NMAX;
  std::list<double> ZMAX;
  const Trk::SpacePoint* p0 = nullptr;

  std::list<Trk::Vertex> lv;  
  
  if      (m_nspoint==2) m_seedsgenerator->find2Sp(data, lv);
  else if (m_nspoint==3) m_seedsgenerator->find3Sp(ctx, data, lv);
  else                   m_seedsgenerator->findVSp(ctx, data, lv);

  const InDet::SiSpacePointsSeed* seed = nullptr;
  std::multimap<int,double> ver;

  int    nmax = 0 ;
  int    Hmax = 0 ;
  double zmax = 0.;

  while ((seed = m_seedsgenerator->next(ctx, data))) {

    std::vector<const Trk::SpacePoint*>::const_iterator
      s = seed->spacePoints().begin();
    ++s;
    if ((*s)!=p0) {
      if (Hmax!=0) {
        NMAX.push_back(nmax);
        ZMAX.push_back(zmax);
      }
      Hmax = nmax = 0;
      p0=(*s);
    }
    int n = static_cast<int>((seed->zVertex()-m_zmin)*sZ);
    if (n>=0 && n<=m_histsize) {
      if (++histogram[n]>Hmax) {
        Hmax = histogram[n];
        nmax = n;
        zmax = seed->zVertex();
      }
    } 
  } 

  // Clear histogram
  //
  for (int i=0; i<=m_histsize; ++i) histogram[i] = 0;

  // Fill histogram again using NMAX information
  //
  std::list<int>   ::iterator m, me = NMAX.end();
  std::list<double>::iterator z = ZMAX.begin();
  for (m=NMAX.begin(); m!=me; ++m) {
    ++histogram[(*m)];
    z_histogram[(*m)] += (*z++); 
  }
   
  // Max. content seach and output bank Z coordinates of vertices production
  //
  int HiZm = 0; 
  int im   = 0;
  int i    = 1;
  while (true) {

    if (histogram[i]>=histogram[i-1] && histogram[i]>=histogram[i+1]) {
      int H3 = histogram[i-1]+histogram[i]+histogram[i+1]; 
      if (H3>HiZm) {
        HiZm=H3;
        im=i;
      }
      if (++i==m_histsize) break;
    }
    if (++i==m_histsize) break;
  }

  if (im==0 || HiZm < m_mincontent) {

    return vertices;
  }
  double zv = (z_histogram[im]+z_histogram[im-1]+z_histogram[im+1])/static_cast<double>(HiZm);
  ver.insert(std::make_pair(-HiZm, zv));

  // Z-vertex coordinates 
  //
  HiZm = static_cast<int>(m_ratio*static_cast<double>(HiZm));
  if (HiZm<m_mincontent) HiZm = m_mincontent;
  i = 1;
  while (true) {
    if (histogram[i]>=histogram[i-1] && histogram[i]>=histogram[i+1] && i!=im) { 
      int H3 = histogram[i-1]+histogram[i]+histogram[i+1];
      if (H3>=HiZm) {
        zv = (z_histogram[i]+z_histogram[i-1]+z_histogram[i+1])/static_cast<double>(H3);
        ver.insert(std::make_pair(-H3, zv));
      }
      if (++i==m_histsize) break;
    }
    if (++i==m_histsize) break;
  }

  if (!ver.empty()) {
    std::multimap<int, double>::iterator v = ver.begin(), ve = ver.end();
    for (; v!=ve; ++v) {
      Amg::Vector3D Vp(0., 0., (*v).second);
      Trk::Vertex Ver(Vp);
      vertices.push_back(Ver);
    }
  }

  dump(msg(MSG::DEBUG));

  return vertices;
}


///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiZvertexMaker_xk::dump(MsgStream& out) const
{
  out << std::endl;
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiZvertexMaker_xk::dumpConditions(MsgStream& out) const
{
  int n = 62-m_seedsgenerator.type().size();
  std::string s1;
  for (int i=0; i<n; ++i) s1.append(" ");
  s1.append("|");

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
