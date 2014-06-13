/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIPixelTracklets.h"
#include "TMath.h"
HIPixelTracklets::HIPixelTracklets():
  m_eta(0.0),m_phi(0.0),m_pt(0.0),m_etaS(0.0),m_phiS(0.0),m_vrt(HepGeom::Point3D<double>(0,0,0)),m_clusters(std::vector<PixelCluster*>(0))
{
}

HIPixelTracklets::HIPixelTracklets(const HepGeom::Point3D<double>& vrt, const std::vector<PixelCluster*>& clusters):
  m_vrt(vrt), m_clusters(clusters)
{
  double cx0 = clusters[0]->globalPosition().x()-vrt.x();
  double cy0 = clusters[0]->globalPosition().y()-vrt.y();
  double cz0 = clusters[0]->globalPosition().z()-vrt.z();
  double phi0 = atan2(cy0,cx0);
  double r0 = sqrt(cx0*cx0+cy0*cy0);
  double theta0 = atan2(r0,cz0);
  double eta0 = -TMath::Log(TMath::Tan(theta0/2));
  m_eta = eta0;
  m_phi = phi0;
  double cx1 = clusters[1]->globalPosition().x()-vrt.x();
  double cy1 = clusters[1]->globalPosition().y()-vrt.y();
  double cz1 = clusters[1]->globalPosition().z()-vrt.z();
  double phi1 = atan2(cy1,cx1);
  double r1 = sqrt(cx1*cx1+cy1*cy1);
  double theta1 = atan2(r1,cz1);
  double eta1 = -TMath::Log(TMath::Tan(theta1/2));
  m_etaS = eta1;
  m_phiS = phi1;
  double O1 = 0.5*(cy1-cy0+cx1*cx1/cy1-cx0*cx0/cy0)/(cx1/cy1-cx0/cy0);
  double O2 = 0.5*(cx1-cx0+cy1*cy1/cx1-cy0*cy0/cx0)/(cy1/cx1-cy0/cx0);
  double radius = sqrt(O1*O1+O2*O2);
  double pt = 0.0006*radius;
  m_pt = pt;
}


void HIPixelTracklets::Merge(HIPixelTracklets* trkl) {
  const std::vector<PixelCluster*> *mClus = trkl->GetClusters();
  for(unsigned int i=0; i<mClus->size(); i++) {
    PixelCluster* clusi = (*mClus)[i];
    bool same = false;
    for(unsigned int j=0; j<m_clusters.size(); j++) {
      if( clusi->identify()== m_clusters[j]->identify() ) {
	same = true;
	break;
      }
    }
    if( !same )  m_clusters.push_back(clusi);
  }
  
}
