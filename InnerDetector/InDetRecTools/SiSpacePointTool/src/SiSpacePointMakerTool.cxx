/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SiSpacePointTool/SiSpacePointMakerTool.h"

// Cluster and space point collections
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

// For processing clusters
#include "InDetIdentifier/SCT_ID.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
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
  void SiSpacePointMakerTool::fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters, 
                                                            SpacePointCollection* spacepointCollection) {
                                                              
    InDet::PixelClusterCollection::const_iterator clusStart = clusters->begin(); 
    InDet::PixelClusterCollection::const_iterator clusFinish = clusters->end(); 

    if ((*clusStart)->detectorElement()) {
      IdentifierHash idHash = clusters->identifyHash(); 
      const Amg::Transform3D& T = (*clusStart)->detectorElement()->surface().transform();
      double Ax[3] = {T(0,0),T(1,0),T(2,0)};
      double Ay[3] = {T(0,1),T(1,1),T(2,1)};
      double Az[3] = {T(0,2),T(1,2),T(2,2)};
      
      spacepointCollection->reserve(spacepointCollection->size()+clusters->size());
      
      for(; clusStart!=clusFinish; ++clusStart){    
        const InDet::SiCluster* c = (*clusStart);
        const Amg::MatrixX&     V = c->localCovariance();
        
	// Global position is already computed during pixel cluster creation and cached in the SiCluster object
        const Amg::Vector3D&  pos = c->globalPosition();
        
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

  ///////////////////////////////////////////////////////////////////
  // Possible offset estimation in Z or R direction due to gap size 
  ///////////////////////////////////////////////////////////////////

  double SiSpacePointMakerTool::offset
  (const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double& stripLengthGapTolerance) const
  {
    const Amg::Transform3D& T1 = element1->transform();
    const Amg::Transform3D& T2 = element2->transform();
    Amg::Vector3D           C  = element1->center() ;
    bool isAnnulus = (element1->design().shape() == InDetDD::Annulus);
    // bool isPolar = (element1->design().shape() == InDetDD::PolarAnnulus);

    double x12 = T1(0,0)*T2(0,0)+T1(1,0)*T2(1,0)+T1(2,0)*T2(2,0)                              ;
    double r   = isAnnulus ? std::sqrt(C[0]*C[0]+C[1]*C[1]) : std::sqrt(T1(0,3)*T1(0,3)+T1(1,3)*T1(1,3));
    double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);

    double dm  = (m_SCTgapParameter*r)*std::abs(s*x12);
    double d   = isAnnulus ? dm/.04 : dm/std::sqrt((1.-x12)*(1.+x12));
    
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
    for (const auto *const cluster : *clusters[0]) {
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
        for (const auto *const cluster : *clusters[currentIndex]) {
          
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
           
        for (const auto *const cluster : *clusters[currentIndex]) {
            
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
     
      for (const auto *const cluster : *clusters[currentIndex]) {

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
  (InDet::SCTinformation& In0,InDet::SCTinformation& In1,IdentifierHash ID0,IdentifierHash ID1,double limit,double slimit) {


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
