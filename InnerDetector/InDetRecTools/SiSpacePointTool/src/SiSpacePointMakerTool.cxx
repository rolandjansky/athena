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
#include <cmath>

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
    m_SCTgapParameter = std::abs(m_SCTgapParameter);
    if (m_SCTgapParameter > 0.002) m_SCTgapParameter = 0.002;
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode SiSpacePointMakerTool::finalize() {
    return StatusCode::SUCCESS;
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
  
    const Amg::Vector3D & endStrip1(ends1.first);   // Top end, first cluster
    const Amg::Vector3D & startStrip1(ends1.second);  // Bottom end, first cluster
    const Amg::Vector3D & endStrip2(ends2.first);   // Top end, second cluster
    const Amg::Vector3D & startStrip2(ends2.second);  // Bottom end, second cluster
    const Amg::Vector3D & strip1Direction(endStrip1-startStrip1);           // vector joining ends of line
    const Amg::Vector3D & strip2Direction(endStrip2-startStrip2);           // vector joining ends of line

    Amg::Vector3D point;

    bool ok(true);
    const double limit = 1. + m_stripLengthTolerance;

    if (m_usePerpProj) {
      /** This is simple hack for the case the origin of the particle is completely unknown:
          The closest approach of element1 to element2 is used (perpendicular projection)
          to determine the position of the SpacePoint on element 1. 
          This option is especially aimed at the use with cosmics data.
          Ref. https://its.cern.ch/jira/browse/ATLASRECTS-5394.
        */
      
      const double denom = Amg::Vector3D(strip1Direction.cross(strip2Direction)).z();
      if (std::abs(denom)>10e-7){
        Amg::Vector3D s(endStrip1+startStrip1);
        Amg::Vector3D t(endStrip2+startStrip2);
        const double lambda0 = strip2Direction.cross(Amg::Vector3D(strip1Direction-t)).z()/denom;
        if (std::abs(lambda0)>limit){
          ATH_MSG_WARNING("Intersection is outside strip bounds");
          ok = false;
        }
        point = (s + lambda0 * strip1Direction) * 0.5;    
        ATH_MSG_VERBOSE( "Endpoints 1 : ( " <<  endStrip1.x() << " , " << endStrip1.y() << " , " << endStrip1.z() << " )   to   (" << startStrip1.x() << " , " << startStrip1.y() << " , " << startStrip1.z() << " ) " );
        ATH_MSG_VERBOSE( "Endpoints 2 : ( " <<  endStrip2.x() << " , " << endStrip2.y() << " , " << endStrip2.z() << " )   to   (" << startStrip2.x() << " , " << startStrip2.y() << " , " << startStrip2.z() << " )  " );
        ATH_MSG_VERBOSE( "Intersection: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );
      } else {
        ATH_MSG_WARNING("Intersection failed");
        ok = false;
      }
    } else {   
      Amg::Vector3D midpoint1x2(endStrip1+startStrip1-2.*vertexVec);  // twice the vector from vertex to midpoint
      Amg::Vector3D midpoint2x2(endStrip2+startStrip2-2.*vertexVec);  // twice the vector from vertex to midpoint
      Amg::Vector3D qs(strip1Direction.cross(midpoint1x2));  
      Amg::Vector3D rt(strip2Direction.cross(midpoint2x2));  
      double m(-midpoint1x2.dot(rt)/strip1Direction.dot(rt));    // ratio for first  line
      double n(0.);                      // ratio for second line

      // We increase the length of the strip by 1%. This a fudge which allows
      // us to recover space-points from tracks pointing back to an interaction
      // point up to around z = +- 20 cm

      
      if      (std::abs(            m             ) > limit) ok = false;
      else if (std::abs((n=-(midpoint2x2.dot(qs)/strip2Direction.dot(qs)))) > limit) ok = false;
 
      if ((not ok) and (stripLengthGapTolerance != 0.)) {

        const double qm     = strip1Direction.mag()                             ;
        const double limitn = limit+(stripLengthGapTolerance/qm);

        if (std::abs(m) <= limitn)  {
   
          if (n==0.) n = -(midpoint2x2.dot(qs)/strip2Direction.dot(qs));

          if (std::abs(n) <= limitn) {
            double mn  = strip1Direction.dot(strip2Direction)/(qm*qm);
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
            if (std::abs(m) < limit and std::abs(n) < limit) ok = true;
          }
        }
      }
      if (ok) point = 0.5*(endStrip1 + startStrip1 + m * strip1Direction);
    }
    if (ok) {
      ATH_MSG_VERBOSE( "SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );       
      const std::pair<IdentifierHash,IdentifierHash> elementIdList( element1->identifyHash() , element2->identifyHash() ); 
      return new InDet::SCT_SpacePoint(elementIdList, point, {&cluster1, &cluster2});
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
        updateRange(element1, element2, stripLengthGapTolerance, min, max);
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
        updateRange(element1, element2, stripLengthGapTolerance, min, max);
      }
   
      for (; clusters2Next!=clusters2Finish; ++clusters2Next){
        Amg::Vector2D locpos = (*clusters2Next)->localPosition();
        Amg::Vector2D localPos = Amg::Vector2D(locpos[0], locpos[1]);
        const double diff = InDetDD::SiLocalPosition(localPos.y(), localPos.x(), 0.).xPhi() - xPhi1;
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
          updateRange(element1, element2, stripLengthGapTolerance, min2, max2);
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
    double r   = std::sqrt(T1(0,3)*T1(0,3)+T1(1,3)*T1(1,3))                                        ;
    double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);

    double dm  = (m_SCTgapParameter*r)*std::abs(s*x12);
    double d   = dm/std::sqrt((1.-x12)*(1.+x12));
    
    if (std::abs(T1(2,2)) > 0.7) d*=(r/std::abs(T1(2,3))); // endcap d = d*R/Z

    stripLengthGapTolerance = d; 
    
    return dm;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Updating the range used to search for overlaps
  ///////////////////////////////////////////////////////////////////

  void SiSpacePointMakerTool::updateRange(const InDetDD::SiDetectorElement* element1, 
                                          const InDetDD::SiDetectorElement* element2, 
                                          double& stripLengthGapTolerance, 
                                          double& min, double& max) const {
    double dm = offset(element1, element2, stripLengthGapTolerance);
    min -= dm;
    max += dm;   
  }
  

  ///////////////////////////////////////////////////////////////////
  // New methods for sct space points production
  ///////////////////////////////////////////////////////////////////

  void SiSpacePointMakerTool::fillSCT_SpacePointCollection (std::array<const InDetDD::SiDetectorElement*, nNeighbours>& elements,
                                                            std::array<const SCT_ClusterCollection*, nNeighbours>& clusters,
                                                            std::array<double, 14>& overlapExtents,
                                                            bool allClusters,
                                                            const Amg::Vector3D& vertexVec,
                                                            SpacePointCollection* spacepointCollection,
                                                            SpacePointOverlapCollection* spacepointoverlapCollection) const 
  {
                                                              
    // This function is called once all the needed quantities are collected.
    // It is used to build space points checking the compatibility of clusters on pairs of detector elements.
    // Detector elements and cluster collections are elements and clusters, respectively.
    // [0] is the trigger element
    // [1] is the opposite element
    // [2]-[3] are the elements tested for eta overlaps
    // [4]-[5] are the elements tested for phi overlaps
    //
    // To build space points:
    // - For the opposite element and the ones tested for eta overlaps, you have to check 
    //   if clusters are compatible with the local position of the trigger cluster
    //   requiring that the distance between the two clusters in phi is withing a specified range.
    //   - overlapExtents[0], overlapExtents[1] are filled for the opposite element
    //   - overlapExtents[2], overlapExtents[3], overlapExtents[4], overlapExtents[5] are filled for the eta overlapping elements
    // - For the elements tested for phi overlaps, you have to check 
    //   if clusters are compatible with the local position of the trigger cluster. 
    //   This needs that the trigger cluster is at the edge of the trigger module
    //   and that the other cluster is on the compatible edge of its module
    //   - overlapExtents[6], overlapExtents[7], overlapExtents[10], overlapExtents[11]
    //     overlapExtents[8], overlapExtents[9], overlapExtents[12], overlapExtents[13] are filled for the phi overlapping elements
  
    constexpr int otherSideIndex{1};
    constexpr int maxEtaIndex{3};
    std::array<int, nNeighbours-1> elementIndex{};
    int nElements = 0;

    // For the nNeighbours sides, fill elementIndex with the indices of the existing elements.
    // Same the number of elements in nElements to loop on the later on
    for(int n=1; n!=nNeighbours; ++n) {
      if(elements[n]) {
        elementIndex[nElements++] = n; 
      }
    }
    // return if all detector elements are nullptr
    if(!nElements) return;

    // trigger element and clusters
    const InDetDD::SiDetectorElement* element = elements[0];
    IdentifierHash                         Id = clusters[0]->identifyHash();

    std::vector<SCTinformation> sctInfos; 
    sctInfos.reserve(clusters[0]->size());
    
    // loop on all clusters on the trigger detector element and save the related information
    for (const auto& cluster : *clusters[0]) {
      Amg::Vector2D locpos = cluster->localPosition();
      std::pair<Amg::Vector3D, Amg::Vector3D > ends(element->endsOfStrip(InDetDD::SiLocalPosition(locpos.y(),locpos.x(),0.)));
      InDet::SCTinformation sct(cluster,ends.first, ends.second, vertexVec,locpos.x()); 
      sctInfos.push_back(sct);
    }

    double  limit  = 1. + m_stripLengthTolerance;
    double slimit  = 0.                         ;
    std::vector<Trk::SpacePoint*> tmpSpacePoints;
    tmpSpacePoints.reserve(sctInfos.size());

    if(!allClusters) {

      // Start processing the opposite side and the eta overlapping elements
      int n = 0;      
      for(; n < nElements; ++n) {
        
        int currentIndex = elementIndex[n];
        
        if(currentIndex > maxEtaIndex) break;

        // get the detector element and the IdentifierHash
        const InDetDD::SiDetectorElement* currentElement  = elements[currentIndex];
        IdentifierHash                    currentId       = clusters[currentIndex]->identifyHash();

        // retrieve the range 
        double min = overlapExtents[currentIndex*2-2];
        double max = overlapExtents[currentIndex*2-1];
        
        if (m_SCTgapParameter != 0.) {
          updateRange(element, currentElement, slimit, min, max);
        }
        
        InDet::SCTinformation sctInfo;
        
        for (const auto& cluster : *clusters[currentIndex]) {
          
          bool processed = false;        
          const Amg::Vector2D& locpos = cluster->localPosition();  
          double lx1 = locpos.x();
          
          for(auto& sct : sctInfos) {
            
            double diff = lx1-sct.locX();
            
            if(diff < min || diff > max) continue;
            
            if (not processed) {
              processed = true;
              std::pair<Amg::Vector3D, Amg::Vector3D > ends(currentElement->endsOfStrip(InDetDD::SiLocalPosition(locpos.y(),locpos.x(),0.)));
              sctInfo.set(cluster,ends.first, ends.second, vertexVec,lx1);
            }
        
            Trk::SpacePoint* sp = makeSCT_SpacePoint(sct,sctInfo,Id,currentId,limit,slimit);
            if(sp) tmpSpacePoints.push_back(sp);
          }
        }
        // If you are processing the opposite element, save the space points into 
        // the spacepointCollection and clear the temporary collection
        if( currentIndex==otherSideIndex && !tmpSpacePoints.empty() ) {
          spacepointCollection->reserve(tmpSpacePoints.size()+spacepointCollection->size());
          for (Trk::SpacePoint* sp: tmpSpacePoints) {
            spacepointCollection->push_back(sp);
          }
          tmpSpacePoints.clear();
        }
      }
      
      // process the phi overlapping elements
      // if possible n starts from 4
      for(; n < nElements; ++n) {
        
        int currentIndex = elementIndex[n];
        const InDetDD::SiDetectorElement* currentElement = elements[currentIndex];
        
        double min = overlapExtents[4*currentIndex-10];
        double max = overlapExtents[4*currentIndex- 9];
        
        if (m_SCTgapParameter != 0.) {
          updateRange(element, currentElement, slimit, min, max);
        }
        
        std::vector<SCTinformation*> sctPhiInfos; 
        sctPhiInfos.reserve(sctInfos.size());
             
        for(auto& sct : sctInfos) {
          double lx0 = sct.locX();
          if (min <= lx0 && lx0 <= max) {
            sctPhiInfos.push_back(&sct);
          }
        }
        // continue if you have no cluster from the phi overlapping region of the trigger element
        if(sctPhiInfos.empty()) continue;
        
        IdentifierHash currentId = clusters[currentIndex]->identifyHash();
                
        min = overlapExtents[4*currentIndex-8];
        max = overlapExtents[4*currentIndex-7];
        
        if (m_SCTgapParameter != 0.) {
          updateRange(element, currentElement, slimit, min, max);
        }
           
        for (const auto& cluster : *clusters[currentIndex]) {
            
          const Amg::Vector2D& locpos = cluster->localPosition();  
          double lx1 = locpos.x();
          if(lx1 < min || lx1 > max ) continue;   
         
          std::pair<Amg::Vector3D, Amg::Vector3D > ends(currentElement->endsOfStrip(InDetDD::SiLocalPosition(locpos.y(),locpos.x(),0.)));
          InDet::SCTinformation sctInfo(cluster,ends.first, ends.second, vertexVec,lx1);
         
          for(auto& sct : sctPhiInfos) {           
            Trk::SpacePoint* sp = makeSCT_SpacePoint(*sct,sctInfo,Id,currentId,limit,slimit);
            if(sp) tmpSpacePoints.push_back(sp);
          }
        }
      }
      
      // fill the space point collection for eta/phi overlapping clusters
      if(!tmpSpacePoints.empty()) {
        spacepointoverlapCollection->reserve(tmpSpacePoints.size()+spacepointoverlapCollection->size());
        for (Trk::SpacePoint* sp: tmpSpacePoints) {
          spacepointoverlapCollection->push_back(sp);
        }
      }
      return;
    }

    // the following code is used to create spacepoints processing all clusters without limits
    
    for(int n=0; n!=nElements; ++n) {
      
      int currentIndex = elementIndex[n];
      const InDetDD::SiDetectorElement* currentElement      = elements[currentIndex];
      IdentifierHash                    currentId           = clusters[currentIndex]->identifyHash();

      if (m_SCTgapParameter != 0.) {
        offset(element, currentElement, slimit);
      }
     
      for (const auto& cluster : *clusters[currentIndex]) {

        const Amg::Vector2D& locpos = cluster->localPosition();
        std::pair<Amg::Vector3D, Amg::Vector3D > ends(currentElement->endsOfStrip(InDetDD::SiLocalPosition(locpos.y(),locpos.x(),0.)));
        InDet::SCTinformation sctInfo(cluster,ends.first, ends.second,vertexVec,locpos.x()); 
        
        for(auto& sct : sctInfos) {          
          Trk::SpacePoint* sp = makeSCT_SpacePoint(sct,sctInfo,Id,currentId,limit,slimit);
          if(sp) tmpSpacePoints.push_back(sp);
        }
      }
      // If you are processing the opposite element, save the space points into 
      // the spacepointCollection and clear the temporary collection        
      if( currentIndex==otherSideIndex && !tmpSpacePoints.empty() ) {        
        spacepointCollection->reserve(tmpSpacePoints.size()+spacepointCollection->size());
        for (Trk::SpacePoint* sp: tmpSpacePoints) {
          spacepointCollection->push_back(sp);
        }
        tmpSpacePoints.clear();
      }
    }
    // fill the space point collection for eta/phi overlapping clusters
    if(!tmpSpacePoints.empty()) {
      spacepointoverlapCollection->reserve(tmpSpacePoints.size()+spacepointoverlapCollection->size());
      for (Trk::SpacePoint* sp: tmpSpacePoints) {
        spacepointoverlapCollection->push_back(sp);
      }
    }
  }

  //--------------------------------------------------------------------------
  //
  Trk::SpacePoint* SiSpacePointMakerTool::makeSCT_SpacePoint
  (InDet::SCTinformation& In0,InDet::SCTinformation& In1,IdentifierHash ID0,IdentifierHash ID1,double limit,double slimit) const {


    double a  =-In0.traj_direction().dot(In1.normal());
    double b  = In0.strip_direction().dot(In1.normal());
    double l0 = In0.oneOverStrip()*slimit+limit ;

    if(std::abs(a) > (std::abs(b)*l0)) return nullptr;

    double c  =-In1.traj_direction().dot(In0.normal());
    double d  = In1.strip_direction().dot(In0.normal()); 
    double l1 = In1.oneOverStrip()*slimit+limit ;

    if(std::abs(c) > (std::abs(d)*l1)) return nullptr;

    double m = a/b;

    if(slimit!=0.) {

      double n = c/d;

      if     (m >  limit || n >  limit) {

        double cs  = In0.strip_direction().dot(In1.strip_direction())*(In0.oneOverStrip()*In0.oneOverStrip());
        double dm  = (m-1);
        double dmn = (n-1.)*cs; 
        if(dmn > dm) dm = dmn;
        m-=dm; n-=(dm/cs);
        if(std::abs(m) > limit || std::abs(n) > limit) return nullptr;
      } else if(m < -limit || n < -limit) {
        
        double cs  = In0.strip_direction().dot(In1.strip_direction())*(In0.oneOverStrip()*In0.oneOverStrip());
        double dm  = -(1.+m);
        double dmn = -(1.+n)*cs; 
        if(dmn > dm) dm = dmn;
        m+=dm; n+=(dm/cs);
        if(std::abs(m) > limit || std::abs(n) > limit) return nullptr;
      }
    }
    Amg::Vector3D point(In0.position(m));
    
    const std::pair<IdentifierHash,IdentifierHash> elementIdList(ID0,ID1); 
    return new InDet::SCT_SpacePoint(elementIdList, point, {In0.cluster(),In1.cluster()});
  }
 
}
