/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <boost/io/ios_state.hpp>

#include "TrkSurfaces/PlaneSurface.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTrajectory_xk.h"

///////////////////////////////////////////////////////////////////
// Set work information to trajectory
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectory_xk::setTools(const InDet::SiTools_xk* t)
{
  m_tools = t;
  for(int i=0; i!=300; ++i) m_elements[i].setTools(t);
} 

void InDet::SiTrajectory_xk::setParameters()
{
  for(int i=0; i!=300; ++i) m_elements[i].setParameters();
} 

///////////////////////////////////////////////////////////////////
// Erase trajector element
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectory_xk::erase(int n) 
{
  if(n>=0 && n<m_nElements) {
    for(int i=n; i!=m_nElements-1; ++i) m_elementsMap[i] = m_elementsMap[i+1]; 
    --m_nElements;
  }
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to TrackStateOnSurface  
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToTrackStateOnSurface(int cosmic)
{
  if(!cosmic ||  m_elements[m_elementsMap[m_firstElement]].parametersUB().par()[2] < 0.) {
    return convertToTrackStateOnSurface();
  }
  return convertToTrackStateOnSurfaceWithNewDirection();
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to TrackStateOnSurface  with old direction
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToTrackStateOnSurface()
{

  DataVector<const Trk::TrackStateOnSurface>* 
    dtsos = new DataVector<const Trk::TrackStateOnSurface>;

  bool multi = m_tools->multiTrack(); if(m_nclusters <= m_tools->clustersmin() || pTfirst() < m_tools->pTmin()) multi = false;
 
  int i = m_firstElement;
  
  const Trk::TrackStateOnSurface* 
    tsos = m_elements[m_elementsMap[i]].trackStateOnSurface(false,true,multi,1);

  if(tsos) dtsos->push_back(tsos);

  for(++i; i!=m_lastElement; ++i) {

    int m = m_elementsMap[i];
    if(m_elements[m].cluster() || m_elements[m].clusterNoAdd() ) {
      tsos = m_elements[m].trackStateOnSurface(false,false,multi,0);
      if(tsos) dtsos->push_back(tsos);
    }
  }

  i = m_lastElement;
  tsos = m_elements[m_elementsMap[i]].trackStateOnSurface(false,false,multi,2);
  if(tsos) dtsos->push_back(tsos);

  if(multi) {
    m_ntos = 0;
    for(int i=m_firstElement; i<=m_lastElement; ++i) {
      
      int m = m_elementsMap[i];
      if(!m_elements[m].ntsos()) continue;
      m_atos[m_ntos  ] = m;
      m_itos[m_ntos++] = 0;
    }
  }
  return dtsos;
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to TrackStateOnSurface  with new direction
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToTrackStateOnSurfaceWithNewDirection()
{
  DataVector<const Trk::TrackStateOnSurface>* 
    dtsos = new DataVector<const Trk::TrackStateOnSurface>;

  bool multi = m_tools->multiTrack(); if(pTfirst() < m_tools->pTmin()) multi = false;

  int i = m_lastElement;

  const Trk::TrackStateOnSurface* 
    tsos = m_elements[m_elementsMap[i]].trackStateOnSurface(true,true,multi,2);

  if(tsos) dtsos->push_back(tsos);

  for(--i; i!=m_firstElement; --i) {

    int m = m_elementsMap[i];
    if(m_elements[m].cluster() || m_elements[m].clusterNoAdd() ) {
      tsos = m_elements[m].trackStateOnSurface(true,false,multi,0);
      if(tsos) dtsos->push_back(tsos);
    }
  }

  i = m_firstElement;
  tsos = m_elements[m_elementsMap[i]].trackStateOnSurface(true,false,multi,1);
  if(tsos) dtsos->push_back(tsos);

  return dtsos;
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to simple TrackStateOnSurface   
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToSimpleTrackStateOnSurface(int cosmic)
{
  if(!cosmic ||  m_elements[m_elementsMap[m_firstElement]].parametersUB().par()[2] < 0.) {
    return convertToSimpleTrackStateOnSurface();
  }
  return convertToSimpleTrackStateOnSurfaceWithNewDirection();
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to simple TrackStateOnSurface  with old direction
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToSimpleTrackStateOnSurface()
{
  DataVector<const Trk::TrackStateOnSurface>* 
    dtsos = new DataVector<const Trk::TrackStateOnSurface>;

  int i = m_firstElement;
  
  const Trk::TrackStateOnSurface* 
    tsos = m_elements[m_elementsMap[i]].trackPerigeeStateOnSurface();

  if(tsos) dtsos->push_back(tsos);
  
  tsos = m_elements[m_elementsMap[i]].trackSimpleStateOnSurface(false,false,0);

  if(tsos) dtsos->push_back(tsos);
  
  for(++i; i!=m_lastElement; ++i) {
    
    int m = m_elementsMap[i];
    if(m_elements[m].cluster()) {
      tsos = m_elements[m].trackSimpleStateOnSurface(false,false,0);
      if(tsos) dtsos->push_back(tsos);
    }
  }

  i =m_lastElement;
  tsos = m_elements[m_elementsMap[i]].trackSimpleStateOnSurface(false,true,2);
  if(tsos) dtsos->push_back(tsos);

  return dtsos;
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to simple TrackStateOnSurface with new direction
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToSimpleTrackStateOnSurfaceWithNewDirection()
{
  DataVector<const Trk::TrackStateOnSurface>* 
    dtsos = new DataVector<const Trk::TrackStateOnSurface>;

  int i = m_lastElement;

  const Trk::TrackStateOnSurface* 
    tsos = m_elements[m_elementsMap[i]].trackSimpleStateOnSurface(true,true,2);

  if(tsos) dtsos->push_back(tsos);

  for(--i; i!=m_firstElement; --i) {

    int m = m_elementsMap[i];
    if(m_elements[m].cluster() || m_elements[m].clusterNoAdd() ) {
      tsos = m_elements[m].trackSimpleStateOnSurface(true,false,0);
      if(tsos) dtsos->push_back(tsos);
    }
  }

  i = m_firstElement;
  tsos = m_elements[m_elementsMap[i]].trackSimpleStateOnSurface(true,false,1);
  if(tsos) dtsos->push_back(tsos);

  return dtsos;
}

///////////////////////////////////////////////////////////////////
// FitQuality production
///////////////////////////////////////////////////////////////////

Trk::FitQuality*  
InDet::SiTrajectory_xk::convertToFitQuality()
{
  double xi2 = m_elements[m_elementsMap[m_firstElement]].xi2totalB();
  return  new Trk::FitQuality(xi2,(m_ndf-5));
}

///////////////////////////////////////////////////////////////////
// Test is it new track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::isNewTrack
(std::multimap<const Trk::PrepRawData*,const Trk::Track*>& map)
{
  const Trk::PrepRawData* prd   [100];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    ti,t[100],te = map.end();

  int     n   = 0    ;
  for(int i=m_firstElement; i<=m_lastElement; ++i) {
   
    int m = m_elementsMap[i];

    if     (m_elements[m].cluster()     ) {
      
      prd[n] = m_elements[m].cluster();
      t  [n] = map.find(prd[n]); if(t[n]==te) return true; ++n;
    }
    else if(m_elements[m].clusterNoAdd()) {
      
      prd[n] = m_elements[m].clusterNoAdd();
      t  [n] = map.find(prd[n]); if(t[n]==te) return true; ++n;
    }
  }

  int nclt = m_nclusters + m_nclustersNoAdd;
  
  for(int i=0; i!=n; ++i) {

    int nclmax = 0;

    for(ti=t[i]; ti!=te; ++ti) {

      if( (*ti).first != prd[i] ) break;
      int ncl = (*ti).second->measurementsOnTrack()->size();
      if(ncl > nclmax) nclmax = ncl;
    }   
    if(nclt > nclmax) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiTrajectory_xk& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiTrajectory_xk::dump( std::ostream& out ) const
{
  boost::io::ios_all_saver ias(out);
  
  if(m_nElements <=0 ) {
    out<<"Trajectory does not exist"<<std::endl; ias.restore();
    return out;
  }
  if(m_firstElement >= m_lastElement ) {
    out<<"Trajectory is wrong"<<std::endl; ias.restore();    
    return out;
  }

  out<<"|--------------------------------------------------------------------------------------------------------|"
     <<std::endl;
  out<<"|                       TRAJECTORY "
     <<"                                                                      |"
     <<std::endl;

  out<<"| Has"<<std::setw(3)<<m_nElements
     <<" ("
     <<std::setw(3)<<m_naElements
     <<")"
     <<" elements and "
     <<std::setw(2)<<m_nclusters+m_nclustersNoAdd<<" ("
     <<std::setw(2)<<m_nclustersNoAdd<<") clusters and "
     <<std::setw(2)<<m_ndf<<" weighted clusters and quality = "<<std::setw(12)<<std::setprecision(5)<<quality()
     <<"         |"
     <<std::endl;
  out<<"| Has number of holes before, inside, after and gap= "
     <<std::setw(2)<<m_nholesb
     <<std::setw(2)<<m_nholes
     <<std::setw(2)<<m_nholese
     <<std::setw(3)<<m_dholes<<"                                           |"
     <<std::endl;
  out<<"|                                                                                        F   B           |"
     <<std::endl;

  out<<"|---|--|---|-----|-|-|---------|---------|---------|---------|----------|---------|-|--|--|--|-|-|-|-|-|-|---------|"
     <<std::endl;
  out<<"| # |SS|  D| Ncl |C|O|   Xi2F  |   Xi2B  | Az.Angle| Radius  |  pT(GeV) |  dZ/dR  |N|In|Lf|Lb|S|D|H|G|H|G|   Step  |"
     <<std::endl;
  out<<"|---|--|---|-----|-|-|---------|---------|---------|---------|----------|---------|-|--|--|--|-|-|-|-|-|-|---------|"
     <<std::endl;

  for(int i=0; i!=m_nElements; ++i) {
    
    int m = m_elementsMap[i];

    std::string DET = "D ";
    const InDetDD::SiDetectorElement* D = m_elements[m].detElement(); 
    std::string DE = " ";
    if(m_elements[m].detstatus() < 0) DE = "-";
    if(D) {
      if(D->isPixel()) {
	if(D->isBarrel()) DET = "Pb"; else DET = "Pe";
      }
      else if(D->isSCT()) {
	if(D->isBarrel()) DET = "Sb"; else DET = "Se";
      }
    }
    int c = 0;
    if(m_elements[m].detstatus() > 0) c = m_elements[m].numberClusters(); 

    out<<"|"<<std::setw(3)<<unsigned(i);

    std::string S0="  ";
    if(m_firstElement == i) S0="=>";
    if(m_lastElement  == i) S0="=>";
 
    std::string S1=" "; 
    std::string S2=" "; 
    if(m_elements[m].cluster     ()) S1="+"; 
    if(m_elements[m].clusterNoAdd()) S2="+";

    out<<"|"
       <<S0<<"|"
       <<std::setw(1)<<DE
       <<std::setw(2)<<DET        <<"|"
       <<std::setw(5)<<c          <<"|"
       <<S1<<"|"
       <<S2<<"|";

    if(m_elements[m].status()) {

      out<<std::setw(9)<<std::setprecision(3)<<m_elements[m].xi2F()<<"|";
      out<<std::setw(9)<<std::setprecision(3)<<m_elements[m].xi2B()<<"|";

      double ra = 0.;
      double pt = 0.;
      double tz = 0.;
      double fa = 0.;

      if(m_elements[m].status()==1) {

	if(m_elements[m].cluster()) {

	  Amg::Vector3D gp = m_elements[m].parametersUF().position();
	  ra = sqrt(gp.x()*gp.x()+gp.y()*gp.y());
	  fa = atan2(gp.y(),gp.x()); 
	  pt = m_elements[m].parametersUF().pT      ();
	  tz = m_elements[m].parametersUF().cotTheta();
	}
	else {

	  Amg::Vector3D gp = m_elements[m].parametersPF().position();
	  ra = sqrt(gp.x()*gp.x()+gp.y()*gp.y());
	  fa = atan2(gp.y(),gp.x()); 
	  pt = m_elements[m].parametersPF().pT      ();
	  tz = m_elements[m].parametersPF().cotTheta();
	}
      }
      else if(m_elements[m].status()==2) {

	if(m_elements[m].cluster()) {

	  Amg::Vector3D gp = m_elements[m].parametersUB().position();
	  ra = sqrt(gp.x()*gp.x()+gp.y()*gp.y());
	  fa = atan2(gp.y(),gp.x()); 
	  pt = m_elements[m].parametersUB().pT      ();
	  tz = m_elements[m].parametersUB().cotTheta();
	}
	else {

	  Amg::Vector3D gp = m_elements[m].parametersPB().position();
	  ra = sqrt(gp.x()*gp.x()+gp.y()*gp.y());
	  fa = atan2(gp.y(),gp.x()); 
	  pt = m_elements[m].parametersPB().pT      ();
	  tz = m_elements[m].parametersPB().cotTheta();
	}
      }
      else {

	bool QA; Trk::PatternTrackParameters S1,SM,S2(m_elements[m].parametersPF()); 
 
	if(m_elements[m].cluster()) S1 = m_elements[m].parametersUB();
	else                        S1 = m_elements[m].parametersPB();

	QA = m_tools->updatorTool()->combineStates(S1,S2,SM);

	if(QA) {

	  Amg::Vector3D gp = SM.position();
	  ra = sqrt(gp.x()*gp.x()+gp.y()*gp.y());
	  fa = atan2(gp.y(),gp.x()); 
	  pt = SM.pT      ();
	  tz = SM.cotTheta();
	}
      }
     out<<std::setw( 9)<<std::setprecision(4)<<fa     <<"|";
     out<<std::setw( 9)<<std::setprecision(4)<<ra     <<"|";
     out<<std::setw(10)<<std::setprecision(4)<<pt*.001<<"|";
     out<<std::setw( 9)<<std::setprecision(4)<<tz     <<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].noiseModel())<<"|";
     out<<std::setw(2)<<m_elements[m].inside()<<"|";
     out<<std::setw(2)<<unsigned(m_elements[m].nlinksF())<<"|";
     out<<std::setw(2)<<unsigned(m_elements[m].nlinksB())<<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].status())<<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].difference())<<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].nholesF())<<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].dholesF())<<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].nholesB())<<"|";
     out<<std::setw(1)<<unsigned(m_elements[m].dholesB())<<"|";
     out<<std::setw(9)<<std::setprecision(4)<<m_elements[m].step()<<"|";
    }
    else                       {
      out<<"         |";
      out<<"         |";
      out<<"         |";
      out<<"         |";
      out<<"          |";
      out<<"         |";
      out<<" |";
      out<<"  |";
      out<<"  |";
      out<<"  |";
      out<<" |";
      out<<" |";
      out<<" |";
      out<<" |";
      out<<" |";
      out<<" |";
      out<<std::setw(9)<<std::setprecision(4)<<m_elements[m].step();
      out<<"|";
  }
    out<<std::endl;
  }
  out<<"|---|--|---|-----|-|-|---------|---------|---------|---------|----------|---------|-|--|--|--|-|-|-|-|-|-|---------|"
     <<std::endl;
  ias.restore();
  return out;
}   
  
///////////////////////////////////////////////////////////////////
// Build initiate trajectory
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::initialize
(bool PIX,bool SCT, 
 const InDet::SiClusterContainer*                     PIXc      ,
 const InDet::SiClusterContainer*                     SCTc      ,
 const Trk::TrackParameters                          & Tp        ,
 std::list<const InDet::SiCluster*>                  & lSiCluster, 
 std::list<const InDet::SiDetElementBoundaryLink_xk*>& DE        ,
 bool                                                & rquality   )
{
  m_nholes          =    0;
  m_nholesb         =    0;
  m_nholese         =    0;
  m_dholes          =    0;
  m_nclusters       =    0;
  m_nclustersNoAdd  =    0;
  m_nElements       =    0;
  m_naElements      =    0;
  m_firstElement    = -100;
  m_lastElement     =    0;
  m_ndfcut          =    0;
  rquality          = true;     
  m_ntos            =    0;
  int    ndfwrong   =    0;
  double Xi2cut     = 2.*m_tools->xi2max(); 

  std::list<const InDet::SiCluster*>::iterator c;
  if(lSiCluster.size() < 2) return false;

  std::list<const InDet::SiDetElementBoundaryLink_xk*>::iterator r,re=DE.end();
  InDet::SiClusterCollection::const_iterator  sib,sie;

  int up    = 0;
  int last  = 0;

  for(r=DE.begin(); r!=re; ++r) {

    const InDetDD::SiDetectorElement* de = (*r)->detElement();
    IdentifierHash           id = de->identifyHash();
    const InDet::SiCluster* sic = 0;

    if(de->isPixel()) {

      if(PIX) {

	InDet::SiClusterContainer::const_iterator w = (*PIXc).indexFind(id);

	if(w!=(*PIXc).end() && (*w)->begin()!=(*w)->end()) {

	  sib = (*w)->begin(); sie = (*w)->end();

	  for(c=lSiCluster.begin(); c!=lSiCluster.end(); ++c) {
	    if((*c)->detectorElement()==de) {
	      if(!m_nclusters) m_firstElement = m_nElements;
	      else             m_lastElement  = m_nElements;
	      ++m_nclusters; m_ndfcut+=2; sic=(*c); lSiCluster.erase(c); break;
	    }
	  }
	  m_elements[m_nElements].set(1,(*r),sib,sie,sic);
	  ++m_naElements;
	}
	else if(m_naElements) {
	  m_elements[m_nElements].set(0,(*r),sib,sie,sic);
	}
	else continue;

	m_elementsMap[m_nElements] = m_nElements; if(++m_nElements==300) break; 
      }
    } 
    else if(SCT)       {

      InDet::SiClusterContainer::const_iterator w = (*SCTc).indexFind(id);

      if(w!=(*SCTc).end() && (*w)->begin()!=(*w)->end()) {

	sib = (*w)->begin(); sie = (*w)->end(); 

	for(c=lSiCluster.begin(); c!=lSiCluster.end(); ++c) {
	  if((*c)->detectorElement()==de) {
	    if(!m_nclusters) m_firstElement = m_nElements;
	    else             m_lastElement  = m_nElements;
	    ++m_nclusters; m_ndfcut+=1; sic=(*c); lSiCluster.erase(c); break;
	  }
	}
	m_elements[m_nElements].set(1,(*r),sib,sie,sic);
	++m_naElements;
      }
      else if(m_naElements) {
        m_elements[m_nElements].set(0,(*r),sib,sie,sic);
      }
      else continue;

      m_elementsMap[m_nElements] = m_nElements; if(++m_nElements==300) break;
    }    

    if(m_firstElement == m_nElements-1) {
      up = m_nElements-1;
      if(!m_elements[up].firstTrajectorElement(Tp)) return false;
    }
    else if(sic) {

      if(!m_elements[m_nElements-1].ForwardPropagationWithoutSearch(m_elements[up])) 
	return false;

      up = m_nElements-1;
      if(m_elements[m_nElements-1].xi2F() <= Xi2cut) {
	last=up;
      }
      else                                           {

	ndfwrong+=m_elements[m_nElements-1].ndf(); if(ndfwrong > 3) return false;
	m_ndfcut-=m_elements[m_nElements-1].ndf(); --m_nclusters;
	m_elements[m_nElements-1].eraseClusterForwardPropagation();
      }
    }
    else if(m_nclusters && lSiCluster.size()) {

      if(!m_elements[m_nElements-1].ForwardPropagationWithoutSearch(m_elements[up])) 
	{
	  if(m_elements[m_nElements-1].cluster()) return false;
	  --m_nElements; if(m_elements[m_nElements-1].detstatus()) --m_naElements;
	}
      else {
	if(m_elements[m_nElements-1].inside()<0) ++m_nholes;
	up = m_nElements-1;
      }
    }
  }

  if( lSiCluster.size()      ) {rquality = false; return false;}
  if(ndfwrong && m_ndfcut < 6) return false;

  m_ndf = m_ndfcut; if(m_ndfcut > 6) m_ndfcut = 6;

  // Kill empty trajectory elements in end of trajectory
  //
  int n = m_nElements-1;
  for(; n>0; --n) {if(m_elements[n].detstatus()>=0) break;} 
  m_nElements = n+1;
  
  // Find last detector element with clusters
  //
  for(; n>0; --n) {if(m_elements[n].detstatus() == 1) {m_elements[n].lastActive(); break;}}

  // Kill uncrossed detector elements
  //
  int m         = m_firstElement+1;
  m_lastElement = last            ;
  for(n = m; n!=m_lastElement; ++n) {
      
    InDet::SiTrajectoryElement_xk& En = m_elements[m_elementsMap[n]];
    if(En.cluster() || En.inside() <= 0) m_elementsMap[m++] = m_elementsMap[n]; 
  }

  if(m!=n) {
    m_lastElement = m;
    for(; n!=m_nElements; ++n) m_elementsMap[m++] = m_elementsMap[n]; 
    m_nElements = m;
  }
  if(!m_tools->bremNoise()) return true;

  // Change noise model for last trajectory elements
  //
  for(n=m_lastElement; n!=m_nElements; ++n) {
    m_elements[m_elementsMap[n]].bremNoiseModel();
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Seacrh cluster compatible with track parameters
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::trackParametersToClusters
(const InDet::SiClusterContainer*                         PIXc      ,
 const InDet::SiClusterContainer*                         SCTc      ,
 const Trk::TrackParameters                              & Tp        ,
 std::list<const InDet::SiDetElementBoundaryLink_xk*>    & DE        ,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT        ,
 std::list<const InDet::SiCluster*>                      & lSiCluster) 
{
  m_nElements = 0;
  m_ndf       = 0;  

  std::multimap<double,const InDet::SiCluster*> xi2cluster;

  InDet::SiClusterCollection::const_iterator  sib,sie;
  std::list<const InDet::SiDetElementBoundaryLink_xk*>::iterator r,re=DE.end();
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator t, te =PT.end();

  double xi2Cut = .5;
  int    ndfCut =  6;

  for(r=DE.begin(); r!=re; ++r) {

    const InDetDD::SiDetectorElement* de = (*r)->detElement();
    IdentifierHash           id = de->identifyHash();

    bool sct = de->isSCT();

    if(!sct) {

      InDet::SiClusterContainer::const_iterator w = (*PIXc).indexFind(id);

      if(w!=(*PIXc).end() && (*w)->begin()!=(*w)->end()) {
	sib = (*w)->begin();
	sie = (*w)->end  ();
      }
      else continue;
    }
    else     {

      InDet::SiClusterContainer::const_iterator w = (*SCTc).indexFind(id);

      if(w!=(*SCTc).end() && (*w)->begin()!=(*w)->end()) {
	sib = (*w)->begin();
	sie = (*w)->end  ();
      }
      else continue;
    }
    if(!m_elements[0].ForwardPropagationForClusterSeach(m_nElements,Tp,(*r),sib,sie)) return false;

    for(int i=0; i!=m_elements[0].nlinksF(); ++i) {
    
      double x = m_elements[0].linkF(i).xi2();

      if(sct) {
	t = PT.find(m_elements[0].linkF(i).cluster());
	if(t!=te && (*t).second->measurementsOnTrack()->size() >= 10) continue;
      }
      else x*=.5;
 
      if(x <= xi2Cut) xi2cluster.insert(std::make_pair(x,m_elements[0].linkF(i).cluster()));
      break;
    }
    ++m_nElements;
  }
  
  if(xi2cluster.size() < 3) return false;

  std::multimap<double,const InDet::SiCluster*>::iterator xc = xi2cluster.begin(), xce = xi2cluster.end();

  for(; xc!=xce; ++xc) {
    lSiCluster.push_back((*xc).second);
    (*xc).second->detectorElement()->isSCT() ? m_ndf+=1 : m_ndf+=2;
    if( m_ndf >= ndfCut ) break;
  }

  if(m_ndf < 6) return false;

  return true;
}

///////////////////////////////////////////////////////////////////
// Seacrh cluster compatible with global positions
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::globalPositionsToClusters
 	(const InDet::SiClusterContainer*                         PIXc      ,
	 const InDet::SiClusterContainer*                         SCTc      ,  
	 const std::list<Amg::Vector3D>                          & Gp        ,
	 std::list<const InDet::SiDetElementBoundaryLink_xk*>    & DE        ,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT        ,
	 std::list<const InDet::SiCluster*>                      & lSiCluster)
{
  std::list<const InDet::SiDetElementBoundaryLink_xk*>::iterator r = DE.begin(), re = DE.end();
  std::list<Amg::Vector3D>::const_iterator                    g,gb = Gp.begin(), ge = Gp.end();
  InDet::SiClusterCollection::const_iterator                  sib,sie;
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator t, te =PT.end();

  Trk::PatternTrackParameters Tp;
  
  double pv[ 5]={0.,0.,0.,0.,0.};
  double cv[15]={ .1 ,
		  0. , .1,
		  0. , 0.,.001,
		  0. , 0.,  0.,.001,
		  0. , 0.,  0.,  0.,.00001};

  double xi2Cut = 10.;
  m_ndf         = 0  ;  

  for(; r!=re; ++r) {

    const InDetDD::SiDetectorElement* d  = (*r)->detElement();
    IdentifierHash                    id = d->identifyHash ();
    const Trk::Surface*               su = &d->surface();
    const Trk::PlaneSurface* pla = static_cast<const Trk::PlaneSurface*>(su);
    if(!pla) continue;

    const Amg::Transform3D&  T = pla->transform();
    double Ax[3] = {T(0,0),T(1,0),T(2,0)};
    double Ay[3] = {T(0,1),T(1,1),T(2,1)};
    double Az[3] = {T(0,2),T(1,2),T(2,2)};
    double x0    = T(0,3);
    double y0    = T(1,3);
    double z0    = T(2,3);
    double zcut  = .001  ; 
    
    bool sct = d->isSCT();
    if(!sct) {

      InDet::SiClusterContainer::const_iterator w = (*PIXc).indexFind(id);
      if(w!=(*PIXc).end() && (*w)->begin()!=(*w)->end()) {
	sib = (*w)->begin();
	sie = (*w)->end  ();
      }
      else continue;
    }
    else {

      zcut = 1.;
      InDet::SiClusterContainer::const_iterator w = (*SCTc).indexFind(id);
      if(w!=(*SCTc).end() && (*w)->begin()!=(*w)->end()) {
	sib = (*w)->begin();
	sie = (*w)->end  ();
      }
      else continue;
    }

    for(g=gb; g!=ge; ++g) {
      
      double dx = (*g).x()-x0;
      double dy = (*g).y()-y0;
      double dz = (*g).z()-z0;
      double z  = dx*Az[0]+dy*Az[1]+dz*Az[2];  if(fabs(z) > zcut) continue;
      pv[0]     = dx*Ax[0]+dy*Ax[1]+dz*Ax[2];
      pv[1]     = dx*Ay[0]+dy*Ay[1]+dz*Ay[2];

      Tp.setParametersWithCovariance(su,pv,cv);

      m_elements[0].CloseClusterSeach(Tp,(*r),sib,sie);
      const InDet::SiCluster* c =  m_elements[0].cluster(); 
      if(!c || m_elements[0].xi2F() > xi2Cut) continue;
      if(sct) {
	t = PT.find(c);
	if(t!=te && (*t).second->measurementsOnTrack()->size() >= 10) continue;
      }
      sct ?  m_ndf+=1 : m_ndf+=2;
      lSiCluster.push_back(c);
    }
  }
  if(m_ndf < 6) return false;
  return true;
} 

///////////////////////////////////////////////////////////////////
// Backward test initial trajectory
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::backwardSmoother(bool TWO)
{
  if(m_firstElement >= m_lastElement) return false;

  // Trajectory difference test
  //
  int m = m_lastElement;
  for(; m>=m_firstElement; --m) {
    if(m_elements[m_elementsMap[m]].difference()) break;
  }
  if(m < m_firstElement) return true;

  if(!m_elements[m_elementsMap[m_lastElement]].lastTrajectorElement()) return false;
  
  int firstElement = m_lastElement       ;
  int maxholes     = m_tools->maxholes ();
  int maxdholes    = m_tools->maxdholes();
  m_nclustersNoAdd = 0                   ;
  m_difference     = 0                   ;

  m     = m_lastElement-1;
  int n = m              ;

  for(; m>=m_firstElement; --m) {

    InDet::SiTrajectoryElement_xk& En = m_elements[m_elementsMap[m+1]];
    InDet::SiTrajectoryElement_xk& Em = m_elements[m_elementsMap[m  ]];

    if(!Em.BackwardPropagationSmoother(En,TWO)) {
      
      if(m == m_firstElement) break;

      for(int i=m+1; i!=m_nElements;  ++i) m_elementsMap[i-1] = m_elementsMap[i];
      --m_lastElement; --m_nElements; --firstElement; continue;
    }

    if((Em.cluster() && Em.clusterOld()) && (Em.cluster()!=Em.clusterOld())) ++m_difference;

    if     (Em.cluster()) {
      firstElement  = m; 
    }
    else                  {
      n=m;
      if(Em.clusterNoAdd()) ++m_nclustersNoAdd;
      if(Em.nholesB() > maxholes || Em.dholesB() > maxdholes) {++m_difference; break;}
    }
  } 

  m_firstElement = firstElement                       ;
  m              = m_elementsMap[firstElement]        ;
  n              = m_elementsMap[     n      ]        ;   
  m_nclusters    = m_elements[m].nclustersB()         ;
  m_nholes       = m_elements[m].nholesB   ()         ;
  m_nholesb      = m_elements[n].nholesB   ()-m_nholes;
  m_ndf          = m_elements[m].ndfB()               ;
  if(m_ndf < m_ndfcut) return false;

  // Erase trajectory elements with big distance from the track
  //
  m = firstElement+1;
  n = m;
  for(; m!=m_lastElement; ++m) {

    InDet::SiTrajectoryElement_xk& Em = m_elements[m_elementsMap[m]];
    if(Em.inside() > 0) {if(Em.detstatus() > 0) --m_naElements;   }
    else                {m_elementsMap[n++] = m_elementsMap[m];}
  }
  m_lastElement = n;

  // Test number trajector elemenst with clusters
  //
  if(m_naElements < m_tools->clustersmin() && m_nholes+m_nholese) return false;
  if(n!=m) {
    for(; m!=m_nElements; ++m) m_elementsMap[n++] =  m_elementsMap[m]; 
    m_nElements = n;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Backward trajectory extension
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::backwardExtension(int itmax)
{
  if(m_firstElement >= m_lastElement) return false;
  int L = m_firstElement; if(L==0) return true;

  int                     MPbest[300]         ;
  int                     TE    [100]         ;  
  const InDet::SiCluster* CL    [100]         ;
  Trk::PatternTrackParameters  PA             ;
 
  int    maxholes       = m_tools->maxholes ();
  int    maxdholes      = m_tools->maxdholes();
  const int itm         = itmax-1             ;
  int    it             = 0                   ;
  int    itbest         = 0                   ;
  int    qbest          =-100                 ;   
  int    nbest          = 0                   ;
  int    ndfbest        = 0                   ; 
  int    lbest          = L                   ;
  int    hbest          = 0                   ;
  int    hbestb         = 0                   ;
  int    nclbest        = 0                   ;
  int    ndcut          = 3                   ;
  int    F              = L                   ;
  double Xi2best        = 0.                  ;

  m_elements[m_elementsMap[F]].setNdist(0);

  for(; it!=itmax; ++it) {

    int  l = F;
    int  p = F; 

    for(--F; F>=0; --F) {
      
      InDet::SiTrajectoryElement_xk& El = m_elements[m_elementsMap[F+1]];
      InDet::SiTrajectoryElement_xk& Ef = m_elements[m_elementsMap[F  ]];

      if(!Ef.BackwardPropagationFilter(El))  break;
      
      if     (Ef.cluster()) {
	p=F; l=F; 
      }
      else if(Ef.inside() < 0  ) {
	p=F; if(Ef.nholesB() > maxholes || Ef.dholesB() > maxdholes) break;
      }
      int nm = Ef.nclustersB()+F;
      if(Ef.ndist() >  ndcut || nm < nclbest || (nm == nclbest && Ef.xi2totalB() > Xi2best) ) break;
    } 

    int    fl = F; if(fl<0) fl=0;

    int    m  = m_elementsMap[l];
    int    nc = m_elements[m].nclustersB();

    if(it==0 && nc==m_nclusters) return true;

    int    np = m_elements[m].npixelsB();
    int    nh = m_elements[m].nholesB();
    int    nd = m_elements[m_elementsMap[fl]].ndist();
    int    q  = nc-nh;
    double X  = m_elements[m].xi2totalB();

    if     ( (q > qbest) || (q==qbest && X < Xi2best ) ) {

      qbest   = q                                          ; 
      nbest   = 0                                          ;
      ndfbest = 0                                          ;
      hbest   = nh                                         ;
      hbestb  = m_elements[m_elementsMap[p]].nholesB()-nh  ; 
      itbest  = it                                         ;
      Xi2best = X                                          ;
      PA      = m_elements[m_elementsMap[l]].parametersUB();
 
      if(fl==0 && nd < ndcut) ndcut = nd;

      if(fl!=0 || nd > 0 || np < 3) {

	lbest = l-1;
	for(int i=l; i!=L; ++i) {

	  InDet::SiTrajectoryElement_xk& Ei = m_elements[m_elementsMap[i]];
	  
	  if(Ei.inside() <= 0 ) {
	    MPbest[++lbest] = i;
	    if(Ei.cluster()) {CL[nbest]=Ei.cluster(); TE[nbest++]=lbest; ndfbest+=Ei.ndf();}
	  }
	}
      }
      else    {

	l     =   -1;
	lbest = fl-1;
	for(int i=fl; i!=L; ++i) {

	  InDet::SiTrajectoryElement_xk& Ei = m_elements[m_elementsMap[i]];

	  if(Ei.inside() <= 0 && ++lbest >=0 ) {
	    MPbest[lbest] = lbest;
	    if(Ei.cluster()) {CL[nbest]=Ei.cluster(); TE[nbest++]=lbest; ndfbest+=Ei.ndf(); if(l<0) l=lbest;}
	    m_elementsMap[lbest] = m_elementsMap[i];
	  }
	  
	}

	int dn = L-1-lbest;

	if(dn!=0) {

	  for(int i=L; i!= m_nElements; ++i) {
	    m_elementsMap[i-dn]=m_elementsMap[i];
	  }

	  L            -=dn;
	  m_nElements  -=dn;
	  m_lastElement-=dn;
	}
      }
      nclbest = m_nclusters+nbest;
    }
    
    F = -1; if(l<=0) l=1; bool cl = false; double Xn = 0.;

    for(; l < L; ++l) {
      InDet::SiTrajectoryElement_xk& Ei = m_elements[m_elementsMap[l]];

      if(Ei.cluster() && Ei.isNextClusterHoleB(cl,Xn))  {

	int nm = l+Ei.nclustersB();
	if(!cl) {if(Ei.dist() < -2. && Ei.ndist() > ndcut-1 ) continue; --nm;}
	if(nm < nclbest || (nm == nclbest && Xn > Xi2best)) continue;
	F=l; break;
      }
    }
    if(F < 0 ) break;
    if(it!=itm) if(!m_elements[m_elementsMap[F]].addNextClusterB()) break; 
  }
  if(it == itmax) --it;
  if(!nbest) return true;

  m_nholes       = hbest ;
  m_nholesb      = hbestb;
  m_nclusters   += nbest ; 
  m_ndf         += ndfbest;
  m_firstElement = TE[0] ;
  m_elements[m_elementsMap[TE[0]]].setParametersB(PA);

  int dn = L-1-lbest;
 
  if(dn != 0) {

    m_nElements  -=dn;
    m_lastElement-=dn;

    int n = m_firstElement;
    for(; n <= lbest     ; ++n) m_elementsMap[n]=m_elementsMap[MPbest[n]];
    for(; n!= m_nElements; ++n) m_elementsMap[n]=m_elementsMap[n    +dn ];
  }

  if(itbest==it) return true;
  
  for(int n = L-1-dn; n>=0; --n) {

    InDet::SiTrajectoryElement_xk& En = m_elements[m_elementsMap[n]];
    int m = nbest-1;
    for(; m>=0; --m) if(TE[m]==n) break;

    if(m>=0) {
      En.setCluster(CL[m]); if(--nbest==0) break;
    }
    else     {
      En.setCluster(  0  );                      
    }
  } 
  return true;
}

///////////////////////////////////////////////////////////////////
// Forward trajectory extension
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::forwardExtension(bool smoother,int itmax)
{
  const double pi2 = 2.*M_PI, pi = M_PI;

  if(m_firstElement >= m_lastElement) return false;

  int L = m_lastElement, lElement = m_nElements-1;

  if(smoother) {

    L = m_firstElement;

    if(m_elements[m_elementsMap[L]].difference()) {
  
      if(!m_elements[m_elementsMap[L]].firstTrajectorElement()) return false;

      for(++L; L<=m_lastElement; ++L) {

	InDet::SiTrajectoryElement_xk& El = m_elements[m_elementsMap[L-1]];
	InDet::SiTrajectoryElement_xk& Ef = m_elements[m_elementsMap[L  ]];
	if(!Ef.ForwardPropagationWithoutSearch(El)) return false; 
      }
    }
    else                                          {
      
      bool diff = false;
      for(++L; L<=m_lastElement; ++L) {
	
	InDet::SiTrajectoryElement_xk& El = m_elements[m_elementsMap[L-1]];
	InDet::SiTrajectoryElement_xk& Ef = m_elements[m_elementsMap[L  ]];
	
	if(!diff) {
	  if((diff = Ef.difference())) {
	    if(!Ef.addNextClusterF(El,Ef.cluster())) return false;
	  }
	}
	else      {
	  if(!Ef.ForwardPropagationWithoutSearch(El)) return false; 
	}
      }
    }
    --L;
  }
  if( L== lElement) return true;

  // Search best forward trajectory prolongation
  //  
  int                     MP    [300]                              ;
  int                     MPbest[300]                              ;
  int                     TE    [100]                              ;  
  const InDet::SiCluster* CL    [100]                              ;

  int    maxholes       = m_tools->maxholes ()                     ;
  int    maxdholes      = m_tools->maxdholes()                     ;
  const int itm         = itmax-1                                  ;
  int    it             = 0                                        ;
  int    itbest         = 0                                        ;
  int    qbest          =-100                                      ;   
  int    nbest          = 0                                        ;
  int    ndfbest        = 0                                        ;
  int    hbest          = 0                                        ;
  int    hbeste         = 0                                        ;
  int    ndbest         = 0                                        ;
  int    ndcut          = 3                                        ;
  int    nclbest        = 0                                        ;
  int    lbest          = L                                        ;
  int    F              = L                                        ;
  int    M              = L                                        ;
  MP    [M]             = L                                        ;
  double Xi2best        = 0.                                       ;
  const double dfmax    = 2.2                                      ;
  double f0             = m_elements[m_elementsMap[m_firstElement]].parametersUF().par()[2];

  m_elements[m_elementsMap[F]].setNdist(0);

  for(; it!=itmax; ++it) {

    int  l  = F;
    int  p  = F;
    int  Fs = F;
    int  Ml = M;
    int  Cm = nclbest-lElement;
    bool h  = false;

    for(++F; F!=m_nElements; ++F) {
      
      InDet::SiTrajectoryElement_xk& El = m_elements[m_elementsMap[Fs]];
      InDet::SiTrajectoryElement_xk& Ef = m_elements[m_elementsMap[F ]];

      if(!Ef.ForwardPropagationWithSearch(El)) {

	if(!Ef.isBarrel() || Fs!=F-1) break;

	int f = F;
	for(; f!=m_nElements; ++f) {
	  if(!m_elements[m_elementsMap[f]].isBarrel() ) break; 
	}
	if(f==m_nElements) break;

	F = f-1; continue;	
      }
      else {Fs = F;}  MP[++M] = F;

      if     (Ef.cluster()     ) {
	double df = fabs(Ef.parametersUF().par()[2]-f0); if(df > pi) df = pi2-df; 
	if(df > dfmax) break;
 	p=F; l=F; Ml = M; h=false; 
      }

      else if(Ef.inside() < 0  ) {
	p=F; if(Ef.nholesF() > maxholes || Ef.dholesF() > maxdholes) break; h=true;

	double df = fabs(Ef.parametersPF().par()[2]-f0); if(df > pi) df = pi2-df; 
	if(df > dfmax ) break;
      }
      else                       {
	double df = fabs(Ef.parametersPF().par()[2]-f0); if(df > pi) df = pi2-df; 
	if(df > dfmax) break;
      }
      int nm = Ef.nclustersF()-F; 
      if(Ef.ndist() > ndcut ||  nm < Cm || (nm == Cm && Ef.xi2totalF() > Xi2best)) break;
    }

    int    m  = m_elementsMap[l];
    int    nc = m_elements[m].nclustersF();
    int    nh = m_elements[m].nholesF();
    m_nholese = m_elements[m_elementsMap[p]].nholesF()-nh;

    if(it==0 && nc==m_nclusters) return true;

    int    fl = F; if(fl==m_nElements) --fl;
    int    nd =  m_elements[m_elementsMap[fl]].ndist();
    int    q  = nc-nh;
    double X  = m_elements[m].xi2totalF();
    if     ( (q > qbest) || (q==qbest && X < Xi2best ) ) {
      qbest   = q; 
      nbest   = 0;
      ndfbest = 0;
      hbest   = nh;
      hbeste  = m_elements[m_elementsMap[p]].nholesF()-nh; 
      itbest  = it; 
      lbest   = L ;
      Xi2best = X ;
      ndbest  = nd; if(fl==lElement && nd < ndcut) ndcut = nd;

      for(int j=L+1; j<=Ml; ++j) {

	int    i  = MP[j]; 
	InDet::SiTrajectoryElement_xk& Ei = m_elements[m_elementsMap[i]];

	if(Ei.inside() <= 0) {
	  MPbest[++lbest] = i;
	  if(Ei.cluster()) {CL[nbest]=Ei.cluster(); TE[nbest++]=lbest; ndfbest+=Ei.ndf();}
	}
      }
      nclbest = m_nclusters+nbest;
      if( (nclbest >= 14 && !h) || (fl==lElement && ndbest == 0)) break;
    }

    F = -1; bool cl = false; int nb = lElement-nclbest-1; double Xn;

    for(int j=Ml; j!=L; --j) {

      int i = MP[j]; 
      InDet::SiTrajectoryElement_xk& Ei = m_elements[m_elementsMap[i]];

      if(i!=lElement && Ei.cluster() && Ei.isNextClusterHoleF(cl,Xn)) {

	int nm = nb-i+Ei.nclustersF();

	if(!cl) {if(Ei.dist() < -2. && Ei.ndist() > ndcut-1) continue;}
	else  ++nm;

	if(nm < 0 || (nm == 0 &&   Xn > Xi2best)) continue;
	F=i; M=j; break;
      }
    }

    if(F < 0 ) break;
    if(it!=itm) if(!m_elements[m_elementsMap[F]].addNextClusterF()) break;
  }

  if(it == itmax) --it;

  if(!nbest) return true;

  m_nholes      = hbest      ;
  m_nholese     = hbeste     ;
  m_nclusters  += nbest      ; 
  m_ndf        += ndfbest    ;
  m_lastElement = TE[nbest-1];
  m_nElements   = m_lastElement+1;

  if(m_lastElement != MPbest[m_lastElement]) {
    for(int n = L+1; n<=m_lastElement; ++n) m_elementsMap[n]=m_elementsMap[MPbest[n]];
  }
  if(itbest==it) return true;

  int mb =  0;
  F      = -1;
  for(int n = L+1; n!=m_nElements; ++n) {

    InDet::SiTrajectoryElement_xk& En = m_elements[m_elementsMap[n]];

    int m = mb;
    for(; m!=nbest; ++m) if(TE[m]==n) break;
    
    if(m!=nbest) {
      if(CL[m]!=En.cluster()) {
	if(F<0) {F=n; En.addNextClusterF(m_elements[m_elementsMap[n-1]],CL[m]);} 
	else    {     En.setCluster(CL[m]);                                    }
      }
      if(++mb == nbest) break;
    }
    else {
      if(En.cluster()) {
	if(F<0) {F=n; En.addNextClusterF(m_elements[m_elementsMap[n-1]],0);}
	else    {     En.setCluster(0);                                    }
      }
    }
  } 
  if(F < 0 || m_lastElement == F) {
    return true;
  }  

  for(++F; F<=m_lastElement; ++F) {
    
    InDet::SiTrajectoryElement_xk& El = m_elements[m_elementsMap[F-1]];
    InDet::SiTrajectoryElement_xk& Ef = m_elements[m_elementsMap[F  ]];
    if(!Ef.ForwardPropagationWithoutSearch(El)) return false; 
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Get clusters list from trajectory
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectory_xk::getClusters
(std::list<const InDet::SiCluster*>& Cl)
{
  for(int i = m_firstElement; i<=m_lastElement; ++i) {
    int m = m_elementsMap[i];
    if(m_elements[m].cluster()) Cl.push_back(m_elements[m].cluster());
  }
}

///////////////////////////////////////////////////////////////////
// Forward filter without search
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::forwardFilter()
{
  int L = m_firstElement;
  
  if(!m_elements[m_elementsMap[L]].firstTrajectorElement()) return false;

  for(++L; L<=m_lastElement; ++L) {

    InDet::SiTrajectoryElement_xk& El = m_elements[m_elementsMap[L-1]];
    InDet::SiTrajectoryElement_xk& Ef = m_elements[m_elementsMap[L  ]];
    if(!Ef.ForwardPropagationWithoutSearch(El)) return false; 
  }
  return true;
}


///////////////////////////////////////////////////////////////////
// Test order of detector elements
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::goodOrder()
{
  int    L    = m_firstElement  ;
  int    n    = m_elementsMap[L];
  double step = 0.              ;
  bool  order = true            ;

  for(++L; L<=m_lastElement; ++L) {

    int m = m_elementsMap[L];
    if(!m_elements[m].cluster() && !m_elements[m].clusterNoAdd() &&  m_elements[m].inside()>=0) continue;
    double stp =  m_elements[m].step(m_elements[n]);
    if     ( step   ==   0.) step = stp  ;
    else if((step*stp) < 0.) {order = false; break;} 
    n = m;
  }
  if(order) return true;

  L  = m_firstElement ;
  n = m_elementsMap[L]; 
  Amg::Vector3D gp =  m_elements[n].globalPosition();
  double rad  =  gp.x()*gp.x()+gp.y()*gp.y();
  for(++L; L<=m_lastElement; ++L) {
  
   int m = m_elementsMap[L];
   if(!m_elements[m].cluster() && !m_elements[m].clusterNoAdd() &&  m_elements[m].inside()>=0) continue;
   gp =  m_elements[m].globalPosition();
   double R = gp.x()*gp.x()+gp.y()*gp.y(); 
   if(R < rad) return false;
   rad = R; n=m;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Sort of detector elements in step order
///////////////////////////////////////////////////////////////////

void  InDet::SiTrajectory_xk::sortStep()
{
  int    L  = m_firstElement;
  int    LA = m_firstElement; 

  for(++L; L<=m_lastElement; ++L) {
    
    int m = m_elementsMap[L];
    if(!m_elements[m].cluster() && !m_elements[m].clusterNoAdd() && m_elements[m].inside()>=0) continue;
    m_elementsMap[++LA] = m;
  }
  m_lastElement = LA; L = m_firstElement; m_nElements = LA+1;

  bool   nc = true;
  bool   so = true;
  double ds = m_elements[m_elementsMap[LA]].step()-m_elements[m_elementsMap[L]].step();
  
  if(ds > 0.) {                // Sort in increase order

    while(nc) {
      nc = false; int m = L, n = L+1;
      for(; n<=LA; ++n) {
	
	int Mn = m_elementsMap[n];
	int Mm = m_elementsMap[m];

	if(m_elements[Mn].step() < m_elements[Mm].step()) {
	  if(m_elements[Mn].step(m_elements[Mm]) < 0.) {
	    m_elementsMap[m] = Mn; m_elementsMap[n] = Mm; nc = true; so = false;
	  }
	}
	++m;
      }
    }
  }
  else {

    while(nc) {                  // Sort in decrease order
      nc = false; int m = L, n = L+1; 
      for(; n<=LA; ++n) {
	
	int Mn = m_elementsMap[n];
	int Mm = m_elementsMap[m];
	
	if(m_elements[Mn].step() > m_elements[Mm].step()) {
	  if(m_elements[Mn].step(m_elements[Mm]) > 0.) {
	    m_elementsMap[m] = Mn; m_elementsMap[n] = Mm; nc = true; so = false;
	  }
	}
	++m;
      }
    }
  }
  if(so) return;

  // Search first detector elements with cluster
  //
  int n = L;
  for(; n<= LA; ++n) {
    
    int e = m_elementsMap[n];
    if     (m_elements[e].cluster()) break;
    if     (m_elements[e].clusterNoAdd()) --m_nclustersNoAdd;
    else if(m_elements[e].inside() < 0 && m_elements[e].detstatus()>=0) {--m_nholes; ++m_nholesb;}
  }

  // Search last detector elements with cluster
  //
  int m = LA;
  for(; m>=n  ; --m) {

    int e = m_elementsMap[m];
    if     (m_elements[e].cluster()) break;
    if     (m_elements[e].clusterNoAdd()) --m_nclustersNoAdd;
    else if(m_elements[e].inside() < 0 && m_elements[e].detstatus()>=0) {--m_nholes; ++m_nholese;}
  }
  m_firstElement = n; 
  m_lastElement  = m;
  return;

}

///////////////////////////////////////////////////////////////////
// Test possibility for trajectory to jump through perigee 
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectory_xk::jumpThroughPerigee()
{
  int i = m_firstElement;
  double St = m_elements[m_elementsMap[m_lastElement]].step()-m_elements[m_elementsMap[i]].step();

  for(; i<=m_lastElement; ++i) {

    int m = m_elementsMap[i];
    if(m_elements[m].cluster() && (m_elements[m].ndf()!=2 || (m_elements[m].stepToPerigee()*St) <= 0.)) break;
    if     (m_elements[m].cluster     ()) {--m_nclusters     ; m_ndf-= m_elements[m].ndf();}
    else if(m_elements[m].clusterNoAdd())  --m_nclustersNoAdd;
    else                                   --m_nholes        ;
  }
  if(i == m_firstElement) return false;
  m_firstElement = i;
  return true;
}

///////////////////////////////////////////////////////////////////
// Trajectory quality without optimization
///////////////////////////////////////////////////////////////////

double InDet::SiTrajectory_xk::quality() const
{
  int    holes   = 0 ;
  double quality = 0.;

  for(int i = m_firstElement; i<=m_lastElement; ++i) {
    quality+=m_elements[m_elementsMap[i]].quality(holes);
  }
  return quality;
}

///////////////////////////////////////////////////////////////////
// Trajectory quality with otimization
///////////////////////////////////////////////////////////////////

double InDet::SiTrajectory_xk::qualityOptimization() 
{
  int    lE = m_firstElement;
  int    h  = 0             ;
  double q  = 0             ;
  double qM = 0.            ;

  for(int i = m_firstElement; i<=m_lastElement; ++i) {

    int  m = m_elementsMap[i]; q+=m_elements[m].quality(h);
    if(m_elements[m].cluster() && q > qM) {
      qM = q; lE = i;
    }   
  }

  if(lE == m_firstElement) return -100;

  int fE             = lE;
  int nclustersNoAdd = 0 ; 
  int nclusters      = 0 ; 
  int nholes         = 0 ;
  int dholes         = 0 ;
  int ndf            = 0 ;
  h                  = 0 ;
  q                  = 0.;
  qM                 = 0.;
  
  for(int i = lE; i>=m_firstElement; --i) {

    int  m = m_elementsMap[i]; q+=m_elements[m].quality(h);
    
    if     (m_elements[m].cluster()) {

      ++nclusters; ndf+=m_elements[m].ndf();

      if(q > qM) {

	qM = q; fE = i;
	m_nclusters      = nclusters     ;
	m_nclustersNoAdd = nclustersNoAdd;
	m_nholes         = nholes        ;
	m_dholes         = dholes        ;
	m_ndf            = ndf           ;
      }

    }
    else if(m_elements[m].clusterNoAdd()) {
	++nclustersNoAdd;                 
    }
    else if(m_elements[m].inside() < 0 && m_elements[m].detstatus() >=0) {
      ++nholes; if(h > dholes) dholes = h;
    }
  }

  if(fE==lE || m_nclusters+m_nclustersNoAdd < m_tools->clustersmin() ) return -100.;
  m_firstElement = fE;
  m_lastElement  = lE;
  return qM;
}

///////////////////////////////////////////////////////////////////
// Trajectory conversion to TrackStateOnSurface for next tracks
///////////////////////////////////////////////////////////////////

DataVector<const Trk::TrackStateOnSurface>* 
InDet::SiTrajectory_xk::convertToNextTrackStateOnSurface()
{
  int i=0;
  for(; i!=m_ntos; ++i) {
    if(m_itos[i]+1 < m_elements[m_atos[i]].ntsos()) {++m_itos[i]; break;}
    m_itos[i] = 0;
  }
  if(i==m_ntos) return 0;

  DataVector<const Trk::TrackStateOnSurface>* 
    dtsos = new DataVector<const Trk::TrackStateOnSurface>;

  for(i=0; i!=m_ntos; ++i) {

    Trk::TrackStateOnSurface* tsos = m_elements[m_atos[i]].tsos(m_itos[i]);
    if(tsos) dtsos->push_back(tsos);
  }
  return dtsos;
}

///////////////////////////////////////////////////////////////////
// pT of the first elementtrajectory
///////////////////////////////////////////////////////////////////

double  InDet::SiTrajectory_xk::pTfirst () 
{
  int n = m_firstElement  ; if(n <0 || n>=300) return 0.;
  n     = m_elementsMap[n]; if(n <0 || n>=300) return 0.;
  int s = m_elements[n].status();
  if(s<=1) return 0.;
  return m_elements[n].parametersUB().pT();
  /*
  int s = m_elements[m_elementsMap[m_firstElement]].status();
  if(s<=1) return 0;
  return m_elements[m_elementsMap[m_firstElement]].parametersUB().pT();
  */
}
