/*
 *  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#include "SiSpacePointTool/SiSpacePointMakerTool.h"

// Cluster and space point collections
#include "TrkSpacePoint/SpacePointCollection.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 

// For processing clusters
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
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
    
    ///////////////////////////////////////////////////////////////////
    // Constructor with parameters:
    ///////////////////////////////////////////////////////////////////
    
    SiSpacePointMakerTool::SiSpacePointMakerTool(const std::string &type,
                                                       const std::string &name,
                                                       const IInterface *parent) 
    : AthAlgTool(type,name,parent) {
        declareInterface< SiSpacePointMakerTool>( this );
        declareProperty("StripLengthTolerance"      , m_stripLengthTolerance = 0.01 );
        // Recommend  m_SCTgapParameter = .001 - .0015 for ITK geometry
        declareProperty("SCTGapParameter"           , m_SCTgapParameter      = 0.   ); 
        m_idHelper=nullptr;
    }
    
    ///////////////////////////////////////////////////////////////////
    // Destructor
    ///////////////////////////////////////////////////////////////////
    
    SiSpacePointMakerTool::~SiSpacePointMakerTool(){}

    ///////////////////////////////////////////////////////////////////
    // Initialize
    ///////////////////////////////////////////////////////////////////
    
    StatusCode SiSpacePointMakerTool::initialize()  {
        // Get the SCT Helper
        ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));
        m_SCTgapParameter = fabs(m_SCTgapParameter); if(m_SCTgapParameter > .004) m_SCTgapParameter = .004;
        
        m_elementOLD = 0;

        m_SCT0.reserve(200);
        m_SCT1.reserve(200);  
        
        ATH_MSG_INFO( "SiSpacePointMakerTool::initialized");
        
        return StatusCode::SUCCESS;
    }
    
    ///////////////////////////////////////////////////////////////////
    // Finalize
    ///////////////////////////////////////////////////////////////////
    
    StatusCode SiSpacePointMakerTool::finalize() {
        return StatusCode::SUCCESS;
    }
    
    ///////////////////////////////////////////////////////////////////
    // Work before new event
    ///////////////////////////////////////////////////////////////////
        
    void SiSpacePointMakerTool::newEvent() {
        m_elementOLD = 0;
    }
    
    ///////////////////////////////////////////////////////////////////
    // Fill pixels collection
    ///////////////////////////////////////////////////////////////////
    
    void SiSpacePointMakerTool::fillPixelSpacePointCollection (const InDet::PixelClusterCollection* clusters, 
                                                                  SpacePointCollection* spacepointCollection) {
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
    
    ///////////////////////////////////////////////////////////////////
    // Compare SCT strips and space points production  
    ///////////////////////////////////////////////////////////////////
    
    void SiSpacePointMakerTool::makeSCT_SpacePoints() {
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
        
        m_tmpSpacePoints.clear();
        
        std::vector<SCTinformation>::iterator I    = m_SCT0.begin(), IE = m_SCT0.end();
        std::vector<SCTinformation>::iterator J,JB = m_SCT1.begin(), JE = m_SCT1.end();
        
        double limit  = 1. + m_stripLengthTolerance;
        
        for(; I!=IE; ++I) {
            for(J=JB; J!=JE; ++J) {
                double qm     = (*I).qm();
                double limitm = limit+(m_stripLengthGapTolerance*qm);
                
                double a =-(*I).s().dot((*J).qs());
                double b = (*I).q().dot((*J).qs()); if(fabs(a) > fabs(b)*limitm) continue;
                
                double qn     = (*J).qm();
                double limitn = limit+(m_stripLengthGapTolerance*qn);
                
                double c =-(*J).s().dot((*I).qs());
                double d = (*J).q().dot((*I).qs()); if(fabs(c) > fabs(d)*limitn) continue;
                
                double m = a/b;
                double n = c/d;
                
                if     (m >  limit || n >  limit) {
                    
                    double cs  = (*I).q().dot((*J).q())*(qm*qm);
                    double dm  = (m-1.);
                    double dmn = (n-1.)*cs; 
                    if(dmn > dm) dm = dmn;
                    m-=dm; n-=(dm/cs);
                
                } else if (m < -limit || n < -limit) {
                    
                    double cs = (*I).q().dot((*J).q())*(qm*qm);
                    double dm  = -(1.+m);
                    double dmn = -(1.+n)*cs; 
                    if(dmn > dm) dm = dmn;
                    m+=dm; n+=(dm/cs);                    
                
                }
                
                if(fabs(m) > limit || fabs(n) > limit) continue;
                
                Amg::Vector3D point((*I).position(m));
                
                ATH_MSG_VERBOSE( "SpacePoint generated at: ( " <<  point.x() << " , " << point.y() << " , " << point.z() << " )   " );       
                const std::pair<IdentifierHash,IdentifierHash> elementIdList(m_element0->identifyHash(),m_element1->identifyHash() );
                const std::pair<const Trk::PrepRawData*,const Trk::PrepRawData*>*
                clusList = new std::pair<const Trk::PrepRawData*,const Trk::PrepRawData*>((*I).cluster(),(*J).cluster());
                m_tmpSpacePoints.push_back( new InDet::SCT_SpacePoint(elementIdList,new Amg::Vector3D(point),clusList) );
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////
    // Fill SCT strip information
    ///////////////////////////////////////////////////////////////////
    
    bool SiSpacePointMakerTool::fillSCT_Information (const InDet::SCT_ClusterCollection* clusters0,
                                                        const InDet::SCT_ClusterCollection* clusters1,
                                                        const Amg::Vector3D& vertexVec, 
                                                        const InDetDD::SCT_DetectorManager *SCT_Manager) {
        
        if(clusters0->empty() || clusters1->empty()) return false;
        InDet::SCT_ClusterCollection::const_iterator c0 = clusters0->begin(),c0e = clusters0->end();
        InDet::SCT_ClusterCollection::const_iterator c1 = clusters1->begin(),c1e = clusters1->end();
        
        m_element0 = SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*c0)->identify()));
        m_element1 = SCT_Manager->getDetectorElement(m_idHelper->wafer_id((*c1)->identify()));
        if(!m_element0 || !m_element1) return false;
        
        m_stripLengthGapTolerance = 0.;
        if(m_SCTgapParameter!=0.) offset(m_element0,m_element1);
        
        if(m_element0 !=  m_elementOLD) {
            
            m_elementOLD = m_element0;
            
            m_SCT0.clear();
            
            for(; c0!=c0e; ++c0) {
                Amg::Vector2D locpos = (*c0)->localPosition(); 
                std::pair<Amg::Vector3D, Amg::Vector3D > ends(m_element0->endsOfStrip(InDetDD::SiLocalPosition(locpos.y(),
                                                                                                               locpos.x(),
                                                                                                               0)));
                InDet::SCTinformation sct((*c0),ends.first, ends.second, vertexVec); 
                m_SCT0.push_back(sct);
            }
        }
        
        m_SCT1.clear();
        
        for(; c1!=c1e; ++c1) {
            Amg::Vector2D locpos = (*c1)->localPosition(); 
            std::pair<Amg::Vector3D, Amg::Vector3D > ends(m_element1->endsOfStrip(InDetDD::SiLocalPosition(locpos.y(),
                                                                                                           locpos.x(),
                                                                                                           0)));
            
            InDet::SCTinformation sct((*c1),ends.first, ends.second, vertexVec); 
            m_SCT1.push_back(sct);
        }
        return true;
    }
    
    
    ///////////////////////////////////////////////////////////////////
    // Fill main SCT collection
    ///////////////////////////////////////////////////////////////////
  
    void SiSpacePointMakerTool::fillSCT_SpacePointCollection (const InDet::SCT_ClusterCollection* clusters1,
                                                                 const InDet::SCT_ClusterCollection* clusters2,
                                                                 double, double, bool,
                                                                 const Amg::Vector3D& vertexVec, 
                                                                 const InDetDD::SCT_DetectorManager *SCT_Manager,
                                                                 SpacePointCollection* spacepointCollection) {
        
        if(!fillSCT_Information(clusters1,clusters2,vertexVec,SCT_Manager)) return;
        makeSCT_SpacePoints();
        
        spacepointCollection->reserve(spacepointCollection->size() + m_tmpSpacePoints.size());
        std::vector<Trk::SpacePoint*>::const_iterator it(m_tmpSpacePoints.begin()),itE(m_tmpSpacePoints.end());
        for (; it!=itE;++it) spacepointCollection->push_back(*it);
        m_tmpSpacePoints.clear();
    }
    
    
    ///////////////////////////////////////////////////////////////////
    // Fill Eta SCT overlap collection
    ///////////////////////////////////////////////////////////////////
  
    void SiSpacePointMakerTool::fillSCT_SpacePointEtaOverlapCollection (const InDet::SCT_ClusterCollection* clusters1,
                                                                           const InDet::SCT_ClusterCollection* clusters2, 
                                                                           double, double, bool,
                                                                           const Amg::Vector3D& vertexVec, 
                                                                           const InDetDD::SCT_DetectorManager *SCT_Manager,
                                                                           SpacePointOverlapCollection* m_spacepointoverlapCollection) {
      
        if(!fillSCT_Information(clusters1,clusters2,vertexVec,SCT_Manager)) return;
        makeSCT_SpacePoints();
        
        std::vector<Trk::SpacePoint*>::const_iterator it(m_tmpSpacePoints.begin()),itE(m_tmpSpacePoints.end());
        for (; it!=itE; ++it) m_spacepointoverlapCollection->push_back(*it);
        m_tmpSpacePoints.clear();
    }
    
    ///////////////////////////////////////////////////////////////////
    // Fill Phi SCT overlap collection
    ///////////////////////////////////////////////////////////////////
  
    void SiSpacePointMakerTool::fillSCT_SpacePointPhiOverlapCollection (const InDet::SCT_ClusterCollection* clusters1,
                                                                           const InDet::SCT_ClusterCollection* clusters2, 
                                                                           double, double, double, double, bool,
                                                                           const Amg::Vector3D& vertexVec, 
                                                                           const InDetDD::SCT_DetectorManager *SCT_Manager,
                                                                           SpacePointOverlapCollection* m_spacepointoverlapCollection) {
        
        if(!fillSCT_Information(clusters1,clusters2,vertexVec,SCT_Manager)) return;
        makeSCT_SpacePoints();
        
        std::vector<Trk::SpacePoint*>::const_iterator it(m_tmpSpacePoints.begin()),itE(m_tmpSpacePoints.end());
        for (; it!=itE; ++it) m_spacepointoverlapCollection->push_back(*it);
        m_tmpSpacePoints.clear();
    }
    
    ///////////////////////////////////////////////////////////////////
    // Possible offset estimation in Z or R direction due to gap size 
    ///////////////////////////////////////////////////////////////////
    
    void SiSpacePointMakerTool::offset (const InDetDD::SiDetectorElement *element1, 
                                        const InDetDD::SiDetectorElement *element2) {
        
        const Amg::Transform3D& T1  =  element1->transform();
        const Amg::Transform3D& T2  =  element2->transform();
        Amg::Vector3D           C   =  element1->center()   ;
        
        double r   = sqrt(C[0]*C[0]+C[1]*C[1])                                                    ;
        double x12 = T1(0,0)*T2(0,0)+T1(1,0)*T2(1,0)+T1(2,0)*T2(2,0)                              ;
        double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);
        
        double dm  = (m_SCTgapParameter*r)*fabs(s*x12);
        
        double d;
        if(element1->design().shape() == InDetDD::Trapezoid || element1->design().shape() == InDetDD::Annulus) 
            d = dm*(1./.04);
        else                                                                                                   
            d = dm/sqrt((1.-x12)*(1+x12));
        
        if(fabs(T1(2,2)) > .7) d*=(r/fabs(T1(2,3))); // endcap d = d*R/Z
        m_stripLengthGapTolerance = d;
        return;
    }
}
