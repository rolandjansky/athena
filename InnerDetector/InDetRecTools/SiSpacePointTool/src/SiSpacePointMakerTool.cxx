/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SiSpacePointTool/SiSpacePointMakerTool.h"

// Cluster and space point collections
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

// For processing clusters
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

// Space points
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"

namespace InDet {

  static const InterfaceID IID_ISiSpacePointMakerTool
  ("SiSpacePointMakerTool", 252891434, 0);

  const InterfaceID& SiSpacePointMakerTool::interfaceID() { 
    return IID_ISiSpacePointMakerTool; 
  }

  // Constructor with parameters:
  SiSpacePointMakerTool::SiSpacePointMakerTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent) :
    AthAlgTool(type, name, parent) {
    declareInterface< SiSpacePointMakerTool>(this);
  }
  //--------------------------------------------------------------------------
  StatusCode SiSpacePointMakerTool::initialize()  {
    // Get the SCT Helper
    ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));
    m_SCTgapParameter = fabs(m_SCTgapParameter);
    if (m_SCTgapParameter > 0.002) m_SCTgapParameter = 0.002;
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode SiSpacePointMakerTool::finalize() {
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  void SiSpacePointMakerTool::newEvent() const {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    std::lock_guard<std::mutex> lock{m_mutex};
    CacheEntry* ent{m_cache.get(ctx)};
    if (ent->m_evt!=ctx.evt()) { // New event in this slot
      ent->clear();
      ent->m_evt = ctx.evt();
    } else {
      ent->m_elementOLD = nullptr;
    }
  }    
  //--------------------------------------------------------------------------
  Trk::SpacePoint* SiSpacePointMakerTool::makeSCT_SpacePoint(const InDet::SiCluster& cluster1, 
                                                             const InDet::SiCluster& cluster2,
                                                             const Amg::Vector3D& vertexVec, 
                                                             const InDetDD::SiDetectorElement* element1,
                                                             const InDetDD::SiDetectorElement* element2,
                                                             double stripLengthGapTolerance) const {

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
    std::pair<Amg::Vector3D, Amg::Vector3D> 
      ends1(element1->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.))); 
  
    locpos = cluster2.localPosition();  
    localPos = Amg::Vector2D(locpos[0], locpos[1]);
    std::pair<Amg::Vector3D, Amg::Vector3D>
      ends2(element2->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.))); 
  
    Amg::Vector3D a(ends1.first);   // Top end, first cluster
    Amg::Vector3D b(ends1.second);  // Bottom end, first cluster
    Amg::Vector3D c(ends2.first);   // Top end, second cluster
    Amg::Vector3D d(ends2.second);  // Bottom end, second cluster
    Amg::Vector3D q(a-b);           // vector joining ends of line
    Amg::Vector3D r(c-d);           // vector joining ends of line

    Amg::Vector3D point;

    bool ok(true);  
    if (m_usePerpProj) {
      /* a simple hack for the case the origin of the particle is completely unknown:
         The closest approach of element1 to element2 is used (perpendicular projection)
         to determine the position of the SpacePoint on element 1. 
         This option is especially aimed at the use with cosmics data.
      */
      Amg::Vector3D mab(c - a);
      double eaTeb = q.dot(r);
      double denom = 1. - eaTeb*eaTeb;
      if (fabs(denom)>10e-7){
        double lambda0 = (mab.dot(q) - mab.dot(r)*eaTeb)/denom;
        point = a+lambda0*q;    
        ATH_MSG_VERBOSE( "Endpoints 1 : ( " <<  a.x() << " , " << a.y() << " , " << a.z() << " )   to   (" << b.x() << " , " << b.y() << " , " << b.z() << " ) " );
        ATH_MSG_VERBOSE( "Endpoints 2 : ( " <<  c.x() << " , " << c.y() << " , " << c.z() << " )   to   (" << d.x() << " , " << d.y() << " , " << d.z() << " )  " );
        ATH_MSG_VERBOSE( "Intersection: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );
      } else {
        ATH_MSG_WARNING("Intersection failed");
        ok = false;
      }
    } else {   
      Amg::Vector3D s(a+b-2.*vertexVec);  // twice the vector from vertex to midpoint
      Amg::Vector3D t(c+d-2.*vertexVec);  // twice the vector from vertex to midpoint
      Amg::Vector3D qs(q.cross(s));  
      Amg::Vector3D rt(r.cross(t));  
      double m(-s.dot(rt)/q.dot(rt));    // ratio for first  line
      double n(0.);                      // ratio for second line

      // We increase the length of the strip by 1%. This a fudge which allows
      // us to recover space-points from tracks pointing back to an interaction
      // point up to around z = +- 20 cm

      double limit = 1. + m_stripLengthTolerance;

      if      (fabs(            m             ) > limit) ok = false;
      else if (fabs((n=-(t.dot(qs)/r.dot(qs)))) > limit) ok = false;
 
      if ((not ok) and (stripLengthGapTolerance != 0.)) {

        double qm     = q.mag()                             ;
        double limitn = limit+(stripLengthGapTolerance/qm);

        if (fabs(m) <= limitn)  {
   
          if (n==0.) n = -(t.dot(qs)/r.dot(qs));

          if (fabs(n) <= limitn) {
            double mn  = q.dot(r)/(qm*qm);
            if ((m >  1.) or (n >  1.)) {
              double dm = (m-1.)   ;
              double dn = (n-1.)*mn;
              double sm = dm;
              if (dm < dn) sm = dn;
              m -= sm;
              n -= (sm*mn);
            } else if ((m < -1.) or (n < -1.)) {
              double dm = -(m+1.)   ;
              double dn = -(n+1.)*mn;
              double sm = dm;
              if (dm < dn) sm = dn;
              m += sm;
              n += (sm*mn);
            }
            if (fabs(m) < limit and fabs(n) < limit) ok = true;
          }
        }
      }
      if (ok) point = 0.5*(a + b + m*q);
    }
    if (ok) {
      ATH_MSG_VERBOSE( "SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );       
      const std::pair<IdentifierHash,IdentifierHash> elementIdList( element1->identifyHash() , element2->identifyHash() ); 
      const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>*
        clusList = new std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>(&cluster1, &cluster2);
      return new InDet::SCT_SpacePoint(elementIdList, new Amg::Vector3D(point), clusList);
    }

    return nullptr;
  }

  //--------------------------------------------------------------------------
  void SiSpacePointMakerTool::fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1, 
                                                           const InDet::SCT_ClusterCollection* clusters2,
                                                           double min, double max, bool allClusters, 
                                                           const Amg::Vector3D& vertexVec,
                                                           const InDetDD::SiDetectorElementCollection* elements,
                                                           SpacePointCollection* spacepointCollection) const {
    double stripLengthGapTolerance = 0.;

    // Try all combinations of clusters for space points
    InDet::SCT_ClusterCollection::const_iterator clusters1Next = clusters1->begin();
    InDet::SCT_ClusterCollection::const_iterator clusters1Finish = clusters1->end();

    // -ME fixme- get first element
    const InDetDD::SiDetectorElement* element1 = nullptr;
 
    if ((*clusters1Next) and (clusters1Next != clusters1Finish)) {
      element1 = elements->getDetectorElement(clusters1->identifyHash());
    }

    if (element1==nullptr) {
      ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()));
      return;
    }
 
    //tmpSpacePoints changed to local variable to enable rentrancy
    std::vector<Trk::SpacePoint*> tmpSpacePoints;

    for (; clusters1Next!=clusters1Finish; ++clusters1Next){   
      Amg::Vector2D locpos = (*clusters1Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double xPhi1 = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();    
      InDet::SCT_ClusterCollection::const_iterator clusters2Next=(*clusters2).begin();
      InDet::SCT_ClusterCollection::const_iterator clusters2Finish=(*clusters2).end();
    
      // -ME fixme- get first element
      const InDetDD::SiDetectorElement* element2 = nullptr;
      if (*clusters2Next and (clusters2Next != clusters2Finish)) {
        element2= elements->getDetectorElement(clusters2->identifyHash());
      }

      if (element2==nullptr) {
        ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()));
        break;
      } 

      if (m_SCTgapParameter != 0.) {
        double dm = offset(element1, element2, stripLengthGapTolerance);
        min -= dm;
        max += dm;
      }
   
      for (; clusters2Next!=clusters2Finish; ++clusters2Next){
        Amg::Vector2D locpos = (*clusters2Next)->localPosition();
        Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
        double diff = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi() - xPhi1;
        if (((min <= diff) and (diff <= max)) or allClusters) {
          Trk::SpacePoint* sp =
            makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
          if (sp) {
            tmpSpacePoints.push_back(sp);
          }
        }
      }
    }

    spacepointCollection->reserve(spacepointCollection->size() + tmpSpacePoints.size());
    for (Trk::SpacePoint* sp: tmpSpacePoints) {
      spacepointCollection->push_back(sp);
    }
  }

  //--------------------------------------------------------------------------
  void SiSpacePointMakerTool::fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters, 
                                                            SpacePointCollection* spacepointCollection) const {
                                                              
    InDet::PixelClusterCollection::const_iterator clusStart = clusters->begin(); 
    InDet::PixelClusterCollection::const_iterator clusFinish = clusters->end(); 
    
    if ((*clusStart)->detectorElement()) {
      IdentifierHash idHash = clusters->identifyHash(); 
      const Amg::Transform3D& T = (*clusStart)->detectorElement()->surface().transform();
      double Ax[3] = {T(0,0),T(1,0),T(2,0)};
      double Ay[3] = {T(0,1),T(1,1),T(2,1)};
      double Az[3] = {T(0,2),T(1,2),T(2,2)};
      double R [3] = {T(0,3),T(1,3),T(2,3)};
      
      spacepointCollection->reserve(spacepointCollection->size()+clusters->size());
      
      for(; clusStart!=clusFinish; ++clusStart){    
        const InDet::SiCluster* c = (*clusStart);
        const Amg::Vector2D&    M = c->localPosition();
        const Amg::MatrixX&     V = c->localCovariance();
        
        Amg::Vector3D  pos(M[0]*Ax[0]+M[1]*Ay[0]+R[0],M[0]*Ax[1]+M[1]*Ay[1]+R[1],M[0]*Ax[2]+M[1]*Ay[2]+R[2]);
        
        double B0[2] = {Ax[0]*V(0,0)+Ax[1]*V(1,0),Ax[0]*V(1,0)+Ax[1]*V(1,1)};
        double B1[2] = {Ay[0]*V(0,0)+Ay[1]*V(1,0),Ay[0]*V(1,0)+Ay[1]*V(1,1)};
        double B2[2] = {Az[0]*V(0,0)+Az[1]*V(1,0),Az[0]*V(1,0)+Az[1]*V(1,1)};
        
        double C01 = B1[0]*Ax[0]+B1[1]*Ax[1];
        double C02 = B2[0]*Ax[0]+B2[1]*Ax[1];
        double C12 = B2[0]*Ay[0]+B2[1]*Ay[1];
        
        Amg::MatrixX   cov(3,3);
        cov<<B0[0]*Ax[0]+B0[1]*Ax[1], C01                    , C02                    ,
             C01                    , B1[0]*Ay[0]+B1[1]*Ay[1], C12                    ,
             C02                    , C12                    , B2[0]*Az[0]+B2[1]*Az[1];
             
        spacepointCollection->push_back( new InDet::PixelSpacePoint(idHash,c,pos,cov) );
      }
    }
  }


  //--------------------------------------------------------------------------
  void SiSpacePointMakerTool::fillSCT_SpacePointEtaOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, 
                                                                     const InDet::SCT_ClusterCollection* clusters2,
                                                                     double min, double max, bool allClusters, 
                                                                     const Amg::Vector3D& vertexVec,
                                                                     const InDetDD::SiDetectorElementCollection* elements,
                                                                     SpacePointOverlapCollection* spacepointoverlapCollection) const {

    double stripLengthGapTolerance = 0.; 

    // Require that (xPhi2 - xPhi1) lie in the range specified.
    // Used eta modules
    // Try all combinations of clusters for space points
    InDet::SCT_ClusterCollection::const_iterator clusters1Next = clusters1->begin();
    InDet::SCT_ClusterCollection::const_iterator clusters1Finish = clusters1->end();

    const InDetDD::SiDetectorElement* element1 = nullptr;
   
    if ((*clusters1Next) and (clusters1Next != clusters1Finish)) {
      element1 = elements->getDetectorElement(clusters1->identifyHash());
    }
    if (element1==nullptr) {
      ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()));
      return;
    } 
    for (; clusters1Next!=clusters1Finish; ++clusters1Next) {
   
      Amg::Vector2D locpos = (*clusters1Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double xPhi1 = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();
 
      InDet::SCT_ClusterCollection::const_iterator clusters2Next = (*clusters2).begin();
      InDet::SCT_ClusterCollection::const_iterator clusters2Finish = (*clusters2).end();
    
      const InDetDD::SiDetectorElement* element2 = nullptr;
      if (*clusters2Next and (clusters2Next != clusters2Finish)) {
        element2 = elements->getDetectorElement(clusters2->identifyHash());
      }
      if (element2==nullptr) {
        ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()));
        break;
      } 
      if (m_SCTgapParameter != 0.) {
        double dm = offset(element1, element2, stripLengthGapTolerance);
        min -= dm;
        max += dm;
      }
   
      for (; clusters2Next!=clusters2Finish; ++clusters2Next){
        Amg::Vector2D locpos = (*clusters2Next)->localPosition();
        Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
        double diff = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi() - xPhi1;
        if (((min <= diff) and (diff <= max)) or allClusters){
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
                                                                     const InDet::SCT_ClusterCollection* clusters2,
                                                                     double min1, double max1, double min2, double max2, 
                                                                     bool allClusters,
                                                                     const Amg::Vector3D& vertexVec,
                                                                     const InDetDD::SiDetectorElementCollection* elements,
                                                                     SpacePointOverlapCollection* spacepointoverlapCollection) const {

    double stripLengthGapTolerance = 0.;
    if (m_SCTgapParameter != 0.) {
      min1 -= 20.;
      max1 += 20.;
    }

    // Clus1 must lie
    // within min1 and max1 and clus between min2 and max2. Used for phi
    // overlaps.

    // Try all combinations of clusters for space points
    InDet::SCT_ClusterCollection::const_iterator clusters1Next = clusters1->begin();
    InDet::SCT_ClusterCollection::const_iterator clusters1Finish = clusters1->end();

    const InDetDD::SiDetectorElement* element1 = nullptr;
    if ((*clusters1Next) and (clusters1Next != clusters1Finish)) {
      element1 = elements->getDetectorElement(clusters1->identifyHash());
    }
    if (element1==nullptr) {
      ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters1Next)->identify()));
      return;
    } 
  
    for (; clusters1Next!=clusters1Finish; ++clusters1Next){
      Amg::Vector2D locpos = (*clusters1Next)->localPosition();
      Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
      double xPhi1 = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();
      if (((min1 <= xPhi1) and (xPhi1 <= max1)) or allClusters) {
        InDet::SCT_ClusterCollection::const_iterator clusters2Next = (*clusters2).begin();
        InDet::SCT_ClusterCollection::const_iterator clusters2Finish = (*clusters2).end();
      
        const InDetDD::SiDetectorElement* element2 = nullptr;
        if (*clusters2Next and (clusters2Next != clusters2Finish)) {
          element2 = elements->getDetectorElement(clusters2->identifyHash());
        }
        if (element2==nullptr) {
          ATH_MSG_ERROR("Bad cluster identifier  " << m_idHelper->show_to_string((*clusters2Next)->identify()));
          break;
        }
 
        if (m_SCTgapParameter != 0.) {
          double dm = offset(element1, element2, stripLengthGapTolerance);
          min2 -= dm;
          max2 += dm;
        }

        for (; clusters2Next!=clusters2Finish; ++clusters2Next) {
          Amg::Vector2D locpos = (*clusters2Next)->localPosition(); 
          Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
          double xPhi2 = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi();
          if (((min2<= xPhi2) and (xPhi2 <= max2)) or allClusters) {
            Trk::SpacePoint* sp =
              makeSCT_SpacePoint(**clusters1Next, **clusters2Next, vertexVec, element1, element2, stripLengthGapTolerance);
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
  (const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double& stripLengthGapTolerance) const
  {
    const Amg::Transform3D& T1 = element1->transform();
    const Amg::Transform3D& T2 = element2->transform();

    double x12 = T1(0,0)*T2(0,0)+T1(1,0)*T2(1,0)+T1(2,0)*T2(2,0)                              ;
    double r   = sqrt(T1(0,3)*T1(0,3)+T1(1,3)*T1(1,3))                                        ;
    double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);

    double dm  = (m_SCTgapParameter*r)*fabs(s*x12);
    double d   = dm/sqrt((1.-x12)*(1.+x12));
    
    if (fabs(T1(2,2)) > 0.7) d*=(r/fabs(T1(2,3))); // endcap d = d*R/Z

    stripLengthGapTolerance = d; 
    return dm;
  }

  void SiSpacePointMakerTool::offset(double& stripLengthGapTolerance,
                                     const InDetDD::SiDetectorElement* element1,
                                     const InDetDD::SiDetectorElement* element2) const {
    const Amg::Transform3D& T1 = element1->transform();
    const Amg::Transform3D& T2 = element2->transform();
    Amg::Vector3D           C  = element1->center()   ;

    double r   = sqrt(C[0]*C[0]+C[1]*C[1])                                                    ;
    double x12 = T1(0,0)*T2(0,0) + T1(1,0)*T2(1,0) + T1(2,0)*T2(2,0)                              ;
    double s   = (T1(0,3)-T2(0,3))*T1(0,2) + (T1(1,3)-T2(1,3))*T1(1,2) + (T1(2,3)-T2(2,3))*T1(2,2);

    double dm  = (m_SCTgapParameter*r)*fabs(s*x12);

    double d   = 0.;
    if (element1->design().shape() == InDetDD::Trapezoid || element1->design().shape() == InDetDD::Annulus) {
      d = dm*(1./0.04);
    } else {
      d = dm/sqrt((1.-x12)*(1.+x12));
    }

    if (fabs(T1(2,2)) > 0.7) d *= (r/fabs(T1(2,3))); // endcap d = d*R/Z
    stripLengthGapTolerance = d;
  }

  ///////////////////////////////////////////////////////////////////
  // Compare SCT strips and space points production
  ///////////////////////////////////////////////////////////////////

  void SiSpacePointMakerTool::makeSCT_SpacePoints(const double stripLengthGapTolerance) const {
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

    const EventContext& ctx{Gaudi::Hive::currentContext()};
    std::lock_guard<std::mutex> lock{m_mutex};
    CacheEntry* ent{m_cache.get(ctx)};
    if (ent->m_evt!=ctx.evt()) { // New event in this slot
      ent->clear();
      ent->m_evt = ctx.evt();
    } else {
      if (ent->m_tmpSpacePoints.size()) {
        for (Trk::SpacePoint* sp : ent->m_tmpSpacePoints) {
          delete sp;
        }
      }
      ent->m_tmpSpacePoints.clear();
    }

    std::vector<SCTinformation>::iterator I    = ent->m_SCT0.begin(), IE = ent->m_SCT0.end();
    std::vector<SCTinformation>::iterator J,JB = ent->m_SCT1.begin(), JE = ent->m_SCT1.end();

    double limit = 1. + m_stripLengthTolerance;

    for (; I!=IE; ++I) {
      double qm     = (*I).qm();
      for (J=JB; J!=JE; ++J) {
        double limitm = limit+(stripLengthGapTolerance*qm);

        double a =-(*I).s().dot((*J).qs());
        double b = (*I).q().dot((*J).qs());
        if (fabs(a) > fabs(b)*limitm) continue;

        double qn     = (*J).qm();
        double limitn = limit+(stripLengthGapTolerance*qn);

        double c =-(*J).s().dot((*I).qs());
        double d = (*J).q().dot((*I).qs());
        if (fabs(c) > fabs(d)*limitn) continue;

        double m = a/b;
        double n = c/d;

        if (m >  limit or n >  limit) {

          double cs  = (*I).q().dot((*J).q())*(qm*qm);
          double dm  = (m-1.);
          double dmn = (n-1.)*cs;
          if (dmn > dm) dm = dmn;
          m-=dm; n-=(dm/cs);

        } else if (m < -limit or n < -limit) {

          double cs = (*I).q().dot((*J).q())*(qm*qm);
          double dm  = -(1.+m);
          double dmn = -(1.+n)*cs;
          if (dmn > dm) dm = dmn;
          m+=dm; n+=(dm/cs);

        }

        if (fabs(m) > limit or fabs(n) > limit) continue;

        Amg::Vector3D point((*I).position(m));

        ATH_MSG_VERBOSE("SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   ");
        const std::pair<IdentifierHash, IdentifierHash> elementIdList(ent->m_element0->identifyHash(), ent->m_element1->identifyHash());
        const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>*
          clusList = new std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>((*I).cluster(), (*J).cluster());
        ent->m_tmpSpacePoints.push_back(new InDet::SCT_SpacePoint(elementIdList, new Amg::Vector3D(point), clusList));
      }
    }
  }
  
}
