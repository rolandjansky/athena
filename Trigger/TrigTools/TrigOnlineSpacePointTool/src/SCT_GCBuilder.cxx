/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineSpacePointTool/SCT_GCBuilder.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"

#include <cmath>

//#define GCDEBUG 

#define WAFER_WIDTH 0.1

SCT_GCBuilder::SCT_GCBuilder(const SCT_ID* sctId, bool useOffline,
			     int offsetBarrelSCT, int offsetEndcapSCT) {
  m_sctID = sctId;
  m_useOfflineAlgorithm=useOffline;
  m_OffsetBarrelSCT = offsetBarrelSCT;
  m_OffsetEndcapSCT = offsetEndcapSCT;
}

void SCT_GCBuilder::formSpacePoints (const InDet::SCT_ClusterCollection& phi_clusterColl, 
                                     const InDetDD::SiDetectorElementCollection* elements,
				     std::vector<TrigSiSpacePoint*>& space_points) {
  double locT, locL, errLocT, errLocL, x, y;
  double r, dr, phi, dphi, z, dz;
  double sint;

  // Get element id    
  const Identifier& waferId = phi_clusterColl.identify();
  const IdentifierHash& waferIdHash = phi_clusterColl.identifyHash();

  const InDetDD::SiDetectorElement* element=elements->getDetectorElement(waferIdHash);

  double pitchPhi = element->phiPitch();
  double pitchPhiRadians = pitchPhi/element->center().mag();


  const Amg::Vector3D& posn =  element->center();
  const Amg::Vector3D& dirT =  element->phiAxis();
  const Amg::Vector3D& dirL =  element->etaAxis();
       
  sint = element->sinTilt();
      
  errLocT= pitchPhi * sqrt(1./12.); 
  errLocL = element->length()*sqrt(1./12.); 

  locL = 0;
  locT = 0;

  // Loop over all clusters in the phi-layer detector 
  InDet::SCT_ClusterCollection::const_iterator iter = phi_clusterColl.begin();

  long layerId = m_sctID->layer_disk(waferId);

  bool isBarrel = m_sctID->is_barrel(waferId);
  if (isBarrel){
    layerId += m_OffsetBarrelSCT;
  }
  else{
    layerId += m_OffsetEndcapSCT;
  }

  for (; iter != phi_clusterColl.end(); iter++) {
    const InDet::SiCluster* cluster =  *iter;
           
    const Amg::Vector2D phiPos = cluster->localPosition();

    if (isBarrel) { // Barrel

      // Calculate Local Co-Ordinates 
      // Strip numbering from 0 - hence the (n-1)
      locT = phiPos[0];
 
      // Transform to global co-ordinates

      x = posn.x() + locT * dirT.x() + locL * dirL.x();

      y = posn.x() + locT * dirT.y() + locL * dirL.y();

      r = sqrt(x*x+y*y)+0.5*WAFER_WIDTH;
      dr = errLocT*sint;

      phi = atan2(y,x);
      dphi = errLocT/r;

      z = posn.x() + locT * dirT.z() + locL * dirL.z();
      dz = fabs(errLocT * dirT.z() + errLocL * dirL.z());

    } else {
      r = posn.mag();
      dr = errLocL;
      
      locT = phiPos[0]/posn.mag();
      dphi= pitchPhiRadians* sqrt(1./12.);

      phi = locT + posn.phi();

      if (phi < -M_PI) phi += 2*M_PI;
      else if (phi > M_PI) phi -= 2*M_PI;
		
      z = posn.z();
      /*
      int signZ=(z<0.0)?-1:1;
      z+=0.5*WAFER_WIDTH*signZ;
      */
      dz = 0.5*WAFER_WIDTH/sqrt(3.0);
    }
 
    // Create the TrigSiSpacePoint object
    TrigSiSpacePoint* sp = new TrigSiSpacePoint(cluster, waferId, layerId,
                                                r, phi, z, dr, dphi, dz);
    
    sp->locL(locL);
    sp->locT(locT);

#ifdef GCDEBUG 
    std::cout.precision(8);
    std::cout << " posn (X,Y,Z) " << posn.z() << " ,  " << posn.z() << " ,  " << posn.z() << std::endl; 
    std::cout << " dirT (X,Y,Z) " << dirT.x() << " ,  " << dirT.y() << " ,  " << dirT.z() << std::endl; 
    std::cout << " dirL (X,Y,Z) " << dirL.x() << " ,  " << dirL.y() << " ,  " << dirL.z() << std::endl; 
    std::cout << "locT = " << locT 
	      << " r = " << r
	      << " dr = " << dr
	      << " phi = " << phi   
	      << " dphi = " << dphi 
	      << " z = " << z 
	      << " dz = " << dz
	      << std::endl;
#endif   

    // Put it into the vector
    space_points.push_back(sp);
  }

}

void SCT_GCBuilder::formSpacePoints (const InDet::SCT_ClusterCollection& phi_clusterColl, 
				     const InDet::SCT_ClusterCollection& uv_clusterColl, 
				     const bool allowPhiOnly,
                                     const InDetDD::SiDetectorElementCollection* elements,
				     std::vector<TrigSiSpacePoint*>& space_points) 
{
  double errLocT,errLocL,x,y;
  double dZ,dr,dphi,dz;
  double Phiphi,Phistereo,zsp,rsp,phisp;
  double alpha, denom;
  bool associationFound;

  InDet::SCT_ClusterCollection::const_iterator phi_begin = phi_clusterColl.begin(),
    phi_end = phi_clusterColl.end(),
    uv_begin = uv_clusterColl.begin(),
    uv_end = uv_clusterColl.end();

  //std::cout << "SCT_GCBuilder::formSpacePoints" << std::endl;

  // Get element id    

  Identifier phi_waferId = phi_clusterColl.identify();  
  Identifier uv_waferId  = uv_clusterColl.identify();
  IdentifierHash phi_waferIdHash = phi_clusterColl.identifyHash();  
  IdentifierHash uv_waferIdHash  = uv_clusterColl.identifyHash();

  const InDetDD::SiDetectorElement* phi_element=elements->getDetectorElement(phi_waferIdHash);
  const InDetDD::SiDetectorElement* uv_element=elements->getDetectorElement(uv_waferIdHash);

  if(phi_element->isStereo())
    {
      uv_waferId = phi_clusterColl.identify(); 
      phi_waferId  = uv_clusterColl.identify();
      uv_waferIdHash = phi_clusterColl.identifyHash(); 
      phi_waferIdHash  = uv_clusterColl.identifyHash();
      phi_begin = uv_clusterColl.begin();
      phi_end = uv_clusterColl.end();
      uv_begin = phi_clusterColl.begin();
      uv_end = phi_clusterColl.end();
      const InDetDD::SiDetectorElement* tmp=phi_element;
      phi_element=uv_element;uv_element=tmp;
    }

  //std::cout << "got Identifiers " << std::endl;
  //std::cout << " useing TrigSiGeo at address " << m_TrigSiGeoSvc << std::endl;

  double pitchPhi = phi_element->phiPitch();
  double pitchPhiRadians = pitchPhi/phi_element->center().mag();

  //std::cout << "pitchPhi " << pitchPhi << "pitchPhiRadians " << pitchPhiRadians<< std::endl;

  errLocT= pitchPhi * sqrt(1./12.);

  const Amg::Vector3D& posPhi = phi_element->center();
  const double zphi=posPhi.z();
  const double inv_zphi = 1. / zphi;
  const double Rphi=posPhi.perp();
  Phiphi=posPhi.phi();
  const Amg::Vector3D& posStereo =  uv_element->center();
  Phistereo=posStereo.phi();
  const double Rstereo=posStereo.perp();
  const double inv_Rstereo = 1. / Rstereo;

  //std::cout << "Phi element extends " << phi_element->rMin() << "\t" << phi_element->rMax() << std::endl;
  
  const Amg::Vector3D& dirT = phi_element->phiAxis();
  const double maxLocL = phi_element->length()*0.5;

  long layerId = m_sctID->layer_disk(phi_waferId);
  const double sina = uv_element->sinStereo();
  const double cosa = sqrt(1.0-sina*sina);
  const double inv_sina = 1. / sina;
  if (m_sctID->is_barrel(phi_waferId)) {
    layerId += m_OffsetBarrelSCT;    
    //    alpha=asin(sina); // Coverity CID 105824 (#1 of 1): Unused value (UNUSED_VALUE)
    errLocL=pitchPhi/(sqrt(6.)*fabs(sina));
    dr=0.5*WAFER_WIDTH/sqrt(3.0);
    dz=errLocL;
    dphi=errLocT*(1./500.0);
  }
  else {
    layerId += m_OffsetEndcapSCT;
    dz=0.5*WAFER_WIDTH/sqrt(3.0);
    dphi=pitchPhiRadians*sqrt(1./12.0);
    dr=pitchPhi/(sqrt(6.)*fabs(sina));
  }

  Amg::Vector3D vertex(0.0,0.0,0.0);

  const double inv_Rphi = 1. / Rphi;
  if(m_useOfflineAlgorithm) {

    InDet::SCT_ClusterCollection::const_iterator iter_phi = phi_begin;
    for(;iter_phi!=phi_end;iter_phi++) 
      {
	const InDet::SiCluster* cluster1=(*iter_phi);
	Amg::Vector2D localPos = cluster1->localPosition(); 
	std::pair<Amg::Vector3D, Amg::Vector3D > ends1(phi_element->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0)));  
	Amg::Vector3D a(ends1.first);   // Top end, first cluster 
	Amg::Vector3D b(ends1.second);  // Bottom end, first cluster 
	Amg::Vector3D q(a-b);          // vector joining ends of line
	InDet::SCT_ClusterCollection::const_iterator iter_uv = uv_begin;
	for(;iter_uv!=uv_end;iter_uv++) 
	  {
	    const InDet::SiCluster* cluster2=(*iter_uv);
	    localPos = cluster2->localPosition(); 
	    std::pair<Amg::Vector3D, Amg::Vector3D > ends2(uv_element->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0)));  
	    Amg::Vector3D c(ends2.first);   // Top end, second cluster 
	    Amg::Vector3D d(ends2.second);  // Bottom end, second cluster 
	    Amg::Vector3D r(c-d);          // vector joining ends of line  
	    bool ok(true);   
	    Amg::Vector3D s(a+b-2*vertex);   // twice the vector from vertex to midpoint 
	    Amg::Vector3D t(c+d-2*vertex);   // twice the vector from vertex to midpoint 
	    Amg::Vector3D qs(q.cross(s));   
	    Amg::Vector3D rt(r.cross(t));   
	    double m = -(s.dot(rt))/(q.dot(rt)); // ratio for first line 
  
	    // We increase the length of the strip by 1%. This a fudge which allows 
	    // us to recover space-points from tracks pointing back to an interaction 
	    // point up to around z = +- 20 cm 
	    
	    double limit = 1.01;
	    if (m>limit || m<-1.*limit) ok=false; 
	    else 
	      { 
		double n = -(t.dot(qs))/(r.dot(qs)); // ratio for second line 
		if (n>limit || n<-1.*limit) ok=false; 
	      }   
	    if (cluster1->detectorElement() == 0 || cluster2->detectorElement() == 0  )  ok=false;         
	    if (ok) { 
	      Amg::Vector3D point( 0.5*(a + b + m*q) ); 

	      if (m_sctID->is_barrel(phi_waferId)) {    
		dphi=errLocT/point.perp();
	      }

	      TrigSiSpacePoint* sp=new TrigSiSpacePoint(cluster1,cluster2, 
							phi_waferId,layerId,point.perp(),point.phi(),
							point.z(),dr,dphi,dz);
	      space_points.push_back(sp);
	    }
	  }
      }     
  }
  else {
    if (m_sctID->is_barrel(phi_waferId)) {

      const double sint = phi_element->sinTilt();
      const double cost = sqrt(1.0-sint*sint);
      const double inv_cost = 1. / cost;
      InDet::SCT_ClusterCollection::const_iterator iter_phi = phi_begin;
      for(;iter_phi!=phi_end;iter_phi++) 
	{
	  const InDet::SiCluster* cluster_phi=(*iter_phi);
	  double xphi=cluster_phi->localPosition()[0];
	  associationFound=false;
	  InDet::SCT_ClusterCollection::const_iterator iter_uv = uv_begin;
	  for(;iter_uv!=uv_end;iter_uv++) 
	    {
	      const InDet::SiCluster* cluster_uv=(*iter_uv);
	      double xs = cluster_uv->localPosition()[0];
	      double coeff=1.0+(Rstereo-Rphi)*inv_Rphi;
	      double centerShift=Rstereo*cosa*(Phistereo-Phiphi)*inv_cost;
	      double localZ=(xs-xphi*coeff*cosa+centerShift)*inv_sina;
	      if(fabs(localZ)<maxLocL) 
		{
		  associationFound = true;
		  zsp=(zphi+localZ)/coeff;
		  rsp=sqrt(xphi*xphi+Rphi*Rphi-2.0*xphi*Rphi*sint);
		  double relX=xphi/(Rphi-WAFER_WIDTH);
		  phisp=atan(relX*sqrt(1.0-sint*sint)/(1.0-relX*sint))+Phiphi;
		  if (phisp < -M_PI) phisp += 2*M_PI;
		  else if (phisp > M_PI) phisp -= 2*M_PI;
		  
		  dphi=errLocT/rsp;

		  TrigSiSpacePoint* sp=new TrigSiSpacePoint(cluster_phi,cluster_uv, 
							    phi_waferId,layerId,rsp,phisp,
							    zsp,dr,dphi,dz);
		  sp->locL(localZ);
		  sp->locT(xphi);
#ifdef GCDEBUG 
		  std::cout.precision(8);
		  std::cout<<" posPhi (X,Y,Z) "<<posPhi.x()<<" ,  "<<posPhi.y()
			   <<" ,  "<<posPhi.z()<<std::endl; 
		  std::cout<<" dirT (X,Y,Z) "<<dirT.x()<<" ,  "<<dirT.y()<<" ,  "
			   <<dirT.z()<<std::endl; 
		  std::cout << "locT = " << xphi 
			    << " r = " << rsp
			    << " dr = " << dr
			    << " phi = " << phisp   
			    << " dphi = " << dphi 
			    << " z = " << zsp 
			    << " dz = " << dz
			    << std::endl;
#endif
		  space_points.push_back(sp);
		}
	    }
	  if(!associationFound && allowPhiOnly) 
	    {
	      x=posPhi.x()+xphi*dirT.x();
	      y=posPhi.y()+xphi*dirT.y();
	      rsp=sqrt(x*x+y*y);
	      dr=0.5*WAFER_WIDTH/sqrt(3.0);
	      phisp=atan2(y,x);

	      dphi = errLocT/rsp;
	      zsp=posPhi.z()+xphi*dirT.z();
	      dz=0.5*maxLocL*sqrt(1/3.0);
	      TrigSiSpacePoint* sp=new TrigSiSpacePoint(cluster_phi, phi_waferId,
							layerId, rsp, phisp, zsp, 
							dr, dphi, dz);
	      sp->locT(xphi);
#ifdef GCDEBUG 
	      std::cout.precision(8);
	      std::cout<<" posn (X,Y,Z) "<<posPhi.x()<<" ,  "<<posPhi.y()<<" ,  "<<
		posPhi.z()<<std::endl; 
	      std::cout<<" dirT (X,Y,Z) "<<dirT.x()<<" ,  "<<dirT.y()<<" ,  "<< 
		dirT->z()<<std::endl; 
	      std::cout << "locT = " << xphi 
			<< " r = " << rsp
			<< " dr = " << dr
			<< " phi = " << phisp   
			<< " dphi = " << dphi 
			<< " z = " << zsp 
			<< " dz = " << dz
			<< std::endl;
#endif
	      space_points.push_back(sp);
	    }
	}
    }
  else
    // In the endcap
    {
      dZ=posStereo.z()-zphi;
      InDet::SCT_ClusterCollection::const_iterator iter_phi = phi_begin;
      // Compute alpha as the angle between the tranverse axes of the phi and uv
      // elements. uv_element->sinStereo() does not return the relative stereo angle.
      const Amg::Vector3D& dirT2 = uv_element->phiAxis();
      alpha = asin(dirT.y()*dirT2.x()-dirT.x()*dirT2.y());
      for(;iter_phi!=phi_end;iter_phi++) 
	{
	  const InDet::SiCluster* cluster_phi=(*iter_phi);
	  double xphi=cluster_phi->localPosition()[0];
	  double PHI0=xphi*inv_Rphi;
	  associationFound=false;
	  InDet::SCT_ClusterCollection::const_iterator iter_uv = uv_begin;
	  for(;iter_uv!=uv_end;iter_uv++)
	    {
	      const InDet::SiCluster* cluster_uv=(*iter_uv);
	      double xs = cluster_uv->localPosition()[0];
	      double PHIS=xs*inv_Rstereo;
	      denom=sin(PHI0-PHIS+alpha);
	      if(fabs(denom)<1.e-10) continue;
	      double B=(sin(PHIS)-sin(PHIS-alpha))/denom;
	      if (maxLocL<35 && Rphi>400) { // short middle-ring strips (length~60, radius>400)
		double posDiff = std::sqrt(std::pow(posPhi.x()-posStereo.x(),2)+std::pow(posPhi.y()-posStereo.y(),2));
		double shortMidCorr = posDiff*0.5/sin(fabs(alpha)*0.5);
		// Assuming that the point of rotation is below the center of module.
		rsp=(Rphi-shortMidCorr)*B; }
	      else rsp=Rphi*B;
	      double dL=rsp*cos(PHI0)-Rphi;

	      if(fabs(dL)<maxLocL)
		{
		  associationFound = true;

		  // The "stereo" angle of the phi element, due to misalignment etc.
	 	  double rotAng = -asin((dirT.x()*posPhi.x()+dirT.y()*posPhi.y())*inv_Rphi);

		  // Correct rsp so that the  coordinates are calculated on the phi-layer
		  rsp=rsp/(1.0+dZ*inv_zphi);

		  phisp=PHI0+Phiphi;

		  // Correct for the "stereo" angle of the phi element:
		  double xsp = rsp*cos(phisp), ysp = rsp*sin(phisp);
		  xsp -= posPhi.x(); ysp -= posPhi.y();
		  double tlsp = atan2(ysp,xsp), rlsp = sqrt(xsp*xsp+ysp*ysp);
		  xsp = rlsp*cos(tlsp+rotAng); ysp = rlsp*sin(tlsp+rotAng);
		  xsp += posPhi.x(); ysp += posPhi.y();
		  phisp=atan2(ysp,xsp);

		  // We have corrected the phi coordinate. Now we will correct the r
		  // coordinate. We could in principle drop that, because the effect
		  // on r is minimal.
		  rsp = sqrt(xsp*xsp+ysp*ysp);

		  zsp=zphi;
		  // Correct for the slight slope with respect to r-axis.
		  // This is a tiny effect, so commenting out for now:
		  //zsp += ((rsp-Rphi) * (phi_element->etaAxis().z()));

		  // Now calculate the errors:
		  
		  double K=(cos(PHIS)-cos(PHIS-alpha))/denom;
		  double M=B*cos(PHI0-PHIS+alpha)/denom;
		  dr=pitchPhiRadians*sqrt(1./12.0);
		  dr*=Rphi*sqrt(M*M+(K+M)*(K+M));
		  //int signZ=(zphi<0.0)?-1:1;
		  //zsp+=0.5*WAFER_WIDTH*signZ;
		  
		  if(phisp<-M_PI) phisp+=2.0*M_PI;
		  else if(phisp>M_PI) phisp-=2.0*M_PI;
		  TrigSiSpacePoint* sp=new TrigSiSpacePoint(cluster_phi,cluster_uv, 
							    phi_waferId,layerId,rsp,phisp,
							    zsp,dr,dphi,dz);
		  sp->locL(dL);
		  sp->locT(PHI0);

#ifdef GCDEBUG
		  std::cout.precision(8);
		  std::cout<<" posPhi (X,Y,Z) "<<posPhi.x()<<" ,  "<<posPhi.y()
			   <<" ,  "<<posPhi.z()<<std::endl; 
		  std::cout<<" dirT (X,Y,Z) "<<dirT.x()<<" ,  "<<dirT.y()<<" ,  "
			   <<dirT.z()<<std::endl;
		  std::cout << "locT = " << PHI0 
			    << " r = " << rsp
			    << " dr = " << dr
			    << " phi = " << phisp   
			    << " dphi = " << dphi 
			    << " z = " << zsp 
			    << " dz = " << dz
			    << std::endl;
#endif
		  space_points.push_back(sp);
		}
	    }
	  if(!associationFound && allowPhiOnly)
	    {
	      rsp=Rphi;
	      dr=0.5*maxLocL*sqrt(1./3.0);
	      phisp=PHI0;
	      dphi=pitchPhiRadians*sqrt(1./12.0);
	      phisp+=Phiphi;
	      if(phisp<-M_PI) phisp+=2.0*M_PI;
	      else if(phisp>M_PI) phisp-=2.0*M_PI;
	      zsp=zphi;
	      //int signZ=(zphi<0.0)?-1:1;
	      //zsp+=0.5*WAFER_WIDTH*signZ;
	      dz=0.5*WAFER_WIDTH/sqrt(3.0);
	      TrigSiSpacePoint* sp=new TrigSiSpacePoint(cluster_phi, phi_waferId,
							layerId, rsp, phisp, zsp, 
							dr, dphi, dz);
#ifdef GCDEBUG 
	      sp->locT(xphi);
	      std::cout.precision(8);
	      std::cout<<" posn (X,Y,Z) "<<posPhi.x()<<" ,  "<<posPhi.y()<<" ,  "<<
		posPhi.z()<<std::endl; 
	      std::cout<<" dirT (X,Y,Z) "<<dirT.x()<<" ,  "<<dirT.y()<<" ,  "<< 
		dirT.z()<<std::endl; 
	      std::cout << "locT = " << xphi 
			<< " r = " << rsp
			<< " dr = " << dr
			<< " phi = " << phisp   
			<< " dphi = " << dphi 
			<< " z = " << zsp 
			<< " dz = " << dz
			<< std::endl;
#endif
	      space_points.push_back(sp);
	    }
	}
    }
  }
}


