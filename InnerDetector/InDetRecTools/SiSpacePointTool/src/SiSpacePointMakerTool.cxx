/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SiSpacePointTool/SiSpacePointMakerTool.h"
// Cluster and space point collections

#include "TrkSpacePoint/SpacePointCollection.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 

// For processing clusters
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiLocalPosition.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h" 

// Space points
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"

namespace InDet{

static const InterfaceID IID_ISiSpacePointMakerTool
            ("SiSpacePointMakerTool", 252891434, 0);

const InterfaceID& SiSpacePointMakerTool::interfaceID() { 
  return IID_ISiSpacePointMakerTool; 
}

// Constructor with parameters:
SiSpacePointMakerTool::SiSpacePointMakerTool(const std::string &type,
				     const std::string &name,
				     const IInterface *parent) :
  AthAlgTool(type,name,parent){
  declareInterface< SiSpacePointMakerTool>( this );
  declareProperty("StripLengthTolerance",m_stripLengthTolerance = 0.01 );
  declareProperty("UsePerpendicularProjection",m_usePerpProj = false );
  declareProperty("SCTGapParameter",  m_SCTgapParameter = 0.  );    // Recommend  m_SCTgapParameter = .001 - .0015 for ITK geometry
  m_idHelper=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// virtual destructor
SiSpacePointMakerTool::~SiSpacePointMakerTool()
{}
//--------------------------------------------------------------------------
StatusCode SiSpacePointMakerTool::initialize()  {
  // Get the SCT Helper
  ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));
  m_SCTgapParameter = fabs(m_SCTgapParameter); if(m_SCTgapParameter > .002) m_SCTgapParameter = .002;
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------
StatusCode SiSpacePointMakerTool::finalize() {
  return StatusCode::SUCCESS;
}
				
//--------------------------------------------------------------------------
Trk::SpacePoint* SiSpacePointMakerTool::makeSCT_SpacePoint(const InDet::SiCluster& cluster1, 
  const InDet::SiCluster& cluster2, const Amg::Vector3D& vertexVec, 
  const InDetDD::SiDetectorElement *element1, const InDetDD::SiDetectorElement *element2, double stripLengthGapTolerance) const {
// -ME fixme- const InDetDD::SCT_DetectorManager *m_manager, const SCT_ID* m_idHelper) {

  // Find intersection of a line through a cluster on one sct detector and
  // a line through a cluster on its stereo pair. Return zero if lines 
  // don't intersect.
  
  // A general point on the line joining point a to point b is
  // x, where 2*x=(1+m)*a + (1-m)*b. Similarly for 2*y=(1+n)*c + (1-n)*d.
  // Suppose that v is the vertex, which we take as (0,0,0); this could
  // an input parameter later, if required. Requiring that the two 'general
  // points' lie on a straight through v means that the vector x-v is a 
  // multiple of y-v. This condition fixes the parameters m and n.
  // We then return the 'space-point' x, supposed to be the phi-layer. 
  // We require that -1<m<1, otherwise x lies 
  // outside the segment a to b; and similarly for n.

  Amg::Vector2D locpos = cluster1.localPosition();  
  Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
  std::pair<Amg::Vector3D, Amg::Vector3D > ends1
    (element1->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0))); 
  
  locpos = cluster2.localPosition();  
  localPos = Amg::Vector2D(locpos[0], locpos[1]);
  std::pair<Amg::Vector3D, Amg::Vector3D > ends2
    (element2->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0))); 
  
  Amg::Vector3D a(ends1.first);   // Top end, first cluster
  Amg::Vector3D b(ends1.second);  // Bottom end, first cluster
  Amg::Vector3D c(ends2.first);   // Top end, second cluster
  Amg::Vector3D d(ends2.second);  // Bottom end, second cluster
  Amg::Vector3D q(a-b);          // vector joining ends of line
  Amg::Vector3D r(c-d);          // vector joining ends of line

  Amg::Vector3D point;

  bool ok(true);  
  if ( m_usePerpProj){
      /* a simple hack for the case the origin of the particle is completely unknown:
	 The closest approach of element1 to element2 is used (perpendicular projection)
	 to determine the position of the SpacePoint on element 1. 
	 This option is especially aimed at the use with cosmics data.
      */
      //Amg::Vector3D center1(element1->center());
      //Amg::Vector3D center2(element2->center());
      //c -= (center2-center1);
      Amg::Vector3D  mab(c - a);
      double eaTeb = q.dot(r);
      double denom = 1 - eaTeb*eaTeb;
      if (fabs(denom)>10e-7){
	    double lambda0 = (mab.dot(q) - mab.dot(r)*eaTeb)/denom;
	    point = a+lambda0*q;    
	    ATH_MSG_VERBOSE( "Endpoints 1 : ( " <<  a.x() << " , " << a.y() << " , " << a.z() << " )   to   (" << b.x() << " , " << b.y() << " , " << b.z() << " ) " );
	    ATH_MSG_VERBOSE( "Endpoints 2 : ( " <<  c.x() << " , " << c.y() << " , " << c.z() << " )   to   (" << d.x() << " , " << d.y() << " , " << d.z() << " )  " );
	    ATH_MSG_VERBOSE( "Intersection: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );
      }else{
	    ATH_MSG_WARNING("Intersection failed");
	    ok = false;
      }
    } else {
   
      Amg::Vector3D s(a+b-2*vertexVec);  // twice the vector from vertex to midpoint
      Amg::Vector3D t(c+d-2*vertexVec);  // twice the vector from vertex to midpoint
      Amg::Vector3D qs(q.cross(s));  
      Amg::Vector3D rt(r.cross(t));  
      double m(-s.dot(rt)/q.dot(rt));    // ratio for first  line
      double n(0.);                      // ratio for second line

      // We increase the length of the strip by 1%. This a fudge which allows
      // us to recover space-points from tracks pointing back to an interaction
      // point up to around z = +- 20 cm

      double limit  = 1. + m_stripLengthTolerance;

      if      (fabs(            m             ) > limit) ok = false;
      else if (fabs((n=-(t.dot(qs)/r.dot(qs)))) > limit) ok = false;
 
      if(!ok && stripLengthGapTolerance !=0.) {

 	double qm     = q.mag()                             ;
	double limitn = limit+(stripLengthGapTolerance/qm);

	if(fabs(m) <= limitn)  {
	  
	  if(n==0.) n = -(t.dot(qs)/r.dot(qs));

	  if(fabs(n) <= limitn) {

	    double mn  = q.dot(r)/(qm*qm);

	    if     (m >  1. || n >  1.) {
	      double dm = (m-1.)   ;
	      double dn = (n-1.)*mn;
	      double sm = dm; if(dm < dn) sm = dn; m-=sm; n-=(sm*mn);
	    }
	    else if(m < -1. || n < -1.) {
	      double dm =-(m+1.)   ;
	      double dn =-(n+1.)*mn;
	      double sm = dm; if(dm < dn) sm = dn; m+=sm;  n+=(sm*mn);
	    }
	    if(fabs(m) < limit && fabs(n) < limit) ok = true;
	  }
	}
      }
      if(ok) point = 0.5*(a + b + m*q);
  }
  if (ok){

      ATH_MSG_VERBOSE( "SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );       
      const std::pair<IdentifierHash,IdentifierHash> elementIdList( element1->identifyHash() , element2->identifyHash() ); 
      const std::pair<const Trk::PrepRawData*,const Trk::PrepRawData*>* clusList = new std::pair<const Trk::PrepRawData*,const Trk::PrepRawData*>(&cluster1,&cluster2);

      return new InDet::SCT_SpacePoint(elementIdList,new Amg::Vector3D(point),clusList);
  }   

  return 0;
  }

//--------------------------------------------------------------------------
void SiSpacePointMakerTool::fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1, 
  const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters, 
  const Amg::Vector3D& vertexVec, const InDetDD::SCT_DetectorManager *SCT_Manager, SpacePointCollection* spacepointCollection) const {

  double stripLengthGapTolerance = 0.; 

  // Try all combinations of clusters for space points
  InDet::SCT_ClusterCollection::const_iterator
    clusters1Next = clusters1->begin();
  InDet::SCT_ClusterCollection::const_iterator
    clusters1Finish = clusters1->end();
  

  // -ME fixme- get first element
  const InDetDD::SiDetectorElement *element1 =0;
 
  if ((*clusters1Next) &&(clusters1Next!=clusters1Finish)) element1 = SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*clusters1Next)->identify()));

  if (!element1) {
    msg(MSG::ERROR) << "Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()) <<endmsg;
    return;
  }
 
  //tmpSpacePoints changed to local variable to enable rentrancy
  std::vector<Trk::SpacePoint*> tmpSpacePoints;

  for (; clusters1Next!=clusters1Finish; ++clusters1Next){   
    Amg::Vector2D locpos = (*clusters1Next)->localPosition();
    Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
    double xPhi1 = InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0).xPhi();    
    InDet::SCT_ClusterCollection::const_iterator clusters2Next=(*clusters2).begin();
    InDet::SCT_ClusterCollection::const_iterator clusters2Finish=(*clusters2).end();
    
    // -ME fixme- get first element
    const InDetDD::SiDetectorElement *element2 =0;
    if (*clusters2Next && (clusters2Next != clusters2Finish)) element2= SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*clusters2Next)->identify()));

    if (!element2) {
      msg(MSG::ERROR) << "Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()) <<endmsg;
      break;
    } 

    if(m_SCTgapParameter!=0.) {double dm = offset(element1,element2, stripLengthGapTolerance); min-=dm; max+=dm; }
   
    for (; clusters2Next != clusters2Finish; ++clusters2Next){
      Amg::Vector2D locpos = (*clusters2Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double diff = InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0).xPhi() - xPhi1;
      if ((min <= diff && diff <= max)||allClusters){
	
	Trk::SpacePoint* sp =
	  makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
	if (sp) {
	  tmpSpacePoints.push_back(sp);
	}
      }
    }
  }

  spacepointCollection->reserve(spacepointCollection->size() + tmpSpacePoints.size());
  std::vector<Trk::SpacePoint*>::const_iterator it(tmpSpacePoints.begin()),itE(tmpSpacePoints.end());
  for (;it!=itE;++it) spacepointCollection->push_back(*it);

}

//--------------------------------------------------------------------------
void SiSpacePointMakerTool::fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters, SpacePointCollection* spacepointCollection) const {
  IdentifierHash idHash = clusters->identifyHash(); 
  InDet::PixelClusterCollection::const_iterator clusStart = clusters->begin(); 
  InDet::PixelClusterCollection::const_iterator clusFinish = clusters->end(); 
  if ((*clusStart)->detectorElement()!=0) {
    // AA080506: since all the clusters in a PixelClusterCollection 
    // are from the same detector element, it is enough to test on the 
    // first cluster.
    spacepointCollection->reserve(spacepointCollection->size()+clusters->size());
    for(; clusStart!=clusFinish; ++clusStart){
      Trk::SpacePoint* sp(new InDet::PixelSpacePoint(idHash, *clusStart)); 
      spacepointCollection->push_back(sp);
    }
  }
}


//--------------------------------------------------------------------------
void SiSpacePointMakerTool::fillSCT_SpacePointEtaOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, 
  const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters, 
  const Amg::Vector3D& vertexVec, const InDetDD::SCT_DetectorManager *SCT_Manager, 
  SpacePointOverlapCollection* spacepointoverlapCollection) const {

  double stripLengthGapTolerance = 0.; 

  // Require that (xPhi2 - xPhi1) lie in the range specified.
  // Used eta modules
  // Try all combinations of clusters for space points
  InDet::SCT_ClusterCollection::const_iterator
    clusters1Next = clusters1->begin();
  InDet::SCT_ClusterCollection::const_iterator
    clusters1Finish = clusters1->end();

  const InDetDD::SiDetectorElement *element1 =0;
   
  if ((*clusters1Next) &&(clusters1Next!=clusters1Finish)) element1 = SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*clusters1Next)->identify()));
  if (!element1) {
    msg(MSG::ERROR) << "Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()) <<endmsg;
    return;
  } 
  for (; clusters1Next!=clusters1Finish; ++clusters1Next){
   
    Amg::Vector2D locpos = (*clusters1Next)->localPosition();
    Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
    double xPhi1 = InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0).xPhi();
    
    InDet::SCT_ClusterCollection::const_iterator
      clusters2Next=(*clusters2).begin();
    InDet::SCT_ClusterCollection::const_iterator
      clusters2Finish=(*clusters2).end();
    
    const InDetDD::SiDetectorElement *element2 =0;
    if (*clusters2Next && (clusters2Next != clusters2Finish)) element2= SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*clusters2Next)->identify()));
    if (!element2) {
      msg(MSG::ERROR) << "Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()) <<endmsg;
      break;
    } 
    if(m_SCTgapParameter!=0.) {double dm = offset(element1,element2, stripLengthGapTolerance); min-=dm; max+=dm; }
   
    for (; clusters2Next != clusters2Finish; ++clusters2Next){
      Amg::Vector2D locpos = (*clusters2Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double diff = InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0).xPhi() - xPhi1;
      if ((min <= diff && diff <= max)||allClusters){
	 
	Trk::SpacePoint* sp =
	  makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
	if (sp) {
	  spacepointoverlapCollection->push_back(sp);
	}
      }
    }
  }
  
}


//--------------------------------------------------------------------------
void SiSpacePointMakerTool::fillSCT_SpacePointPhiOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, 
  const InDet::SCT_ClusterCollection* clusters2, double min1, double max1, double min2, double max2, 
  bool allClusters, const Amg::Vector3D& vertexVec, const InDetDD::SCT_DetectorManager *SCT_Manager, 
  SpacePointOverlapCollection* spacepointoverlapCollection) const {

  double stripLengthGapTolerance = 0.; if(m_SCTgapParameter!=0.) {min1-=20.;  max1+=20.;}

  // Clus1 must lie
  // within min1 and max1 and clus between min2 and max2. Used for phi
  // overlaps.

  // Try all combinations of clusters for space points
  InDet::SCT_ClusterCollection::const_iterator
    clusters1Next = clusters1->begin();
  InDet::SCT_ClusterCollection::const_iterator
    clusters1Finish = clusters1->end();

  const InDetDD::SiDetectorElement *element1 =0;
  if ( (*clusters1Next) && (clusters1Next!=clusters1Finish)) element1= SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*clusters1Next)->identify()));
  if (!element1) {
    msg(MSG::ERROR) << "Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()) <<endmsg;
    return;
  } 
  
  for (; clusters1Next!=clusters1Finish; ++clusters1Next){
    Amg::Vector2D locpos = (*clusters1Next)->localPosition();
    Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
    double xPhi1 = InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0).xPhi();
    if ((min1 <= xPhi1 && xPhi1 <= max1)||allClusters){
      InDet::SCT_ClusterCollection::const_iterator
	clusters2Next=(*clusters2).begin();
      InDet::SCT_ClusterCollection::const_iterator
	clusters2Finish=(*clusters2).end();
      
      const InDetDD::SiDetectorElement *element2 =0;
      if (*clusters2Next&&(clusters2Next != clusters2Finish)) element2 = SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*clusters2Next)->identify()));
      if (!element2) {
	msg(MSG::ERROR) << "Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()) <<endmsg;
	break;
      }
 
      if(m_SCTgapParameter!=0.) {double dm = offset(element1,element2, stripLengthGapTolerance); min2-=dm; max2+=dm; }

      for (; clusters2Next != clusters2Finish; ++clusters2Next)
	{
	  Amg::Vector2D locpos = (*clusters2Next)->localPosition(); 
	  Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
	  double xPhi2 = InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0).xPhi();
	  if ((min2<= xPhi2 && xPhi2 <= max2)||allClusters){
	    Trk::SpacePoint* sp 
	      (makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance));
	    if (sp) {
	      spacepointoverlapCollection->push_back(sp);
	    }
	  }
	}
    }
    
  }
  
}

  ///////////////////////////////////////////////////////////////////
  // Possible offset estimation in Z or R direction due to gap size 
  ///////////////////////////////////////////////////////////////////

  double SiSpacePointMakerTool::offset
  (const InDetDD::SiDetectorElement *element1, const InDetDD::SiDetectorElement *element2, double &stripLengthGapTolerance) const
  {
    const Amg::Transform3D& T1  =  element1->transform();
    const Amg::Transform3D& T2  =  element2->transform();

    double x12 = T1(0,0)*T2(0,0)+T1(1,0)*T2(1,0)+T1(2,0)*T2(2,0)                              ;
    double r   = sqrt(T1(0,3)*T1(0,3)+T1(1,3)*T1(1,3))                                        ;
    double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);

    double dm  = (m_SCTgapParameter*r)*fabs(s*x12);
    double d   = dm/sqrt((1.-x12)*(1.+x12));
    
    if(fabs(T1(2,2)) > .7) d*=(r/fabs(T1(2,3))); // endcap d = d*R/Z

    stripLengthGapTolerance = d; 
    return dm;
  }

}
