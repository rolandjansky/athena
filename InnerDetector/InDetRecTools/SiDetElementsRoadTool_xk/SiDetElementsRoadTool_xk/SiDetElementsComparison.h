/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  SiDetElementsComparison_h
#define  SiDetElementsComparison_h
#include "SiDetElementsRoadTool_xk/SiDetElementLink_xk.h"

namespace InDet{

  ///////////////////////////////////////////////////////////////////
  // Object function for ordering barrel pixels and silicons wafers 
  // in azimuthal angle order
  ///////////////////////////////////////////////////////////////////

  class compDetElements_A {

  public:

    bool operator () (const InDetDD::SiDetectorElement* e1,
		      const InDetDD::SiDetectorElement* e2) 
      {
      
	Amg::Vector3D p1  = e1->center();
	Amg::Vector3D p2  = e2->center();
	double f1 = atan2(p1.y(),p1.x());
	double f2 = atan2(p2.y(),p2.x());
	return(f1<f2);
      }
  };
 
  ///////////////////////////////////////////////////////////////////
  // Object function for ordering barrel pixels and silicons wafers 
  // in radius, azimuthal angle and Z order
  ///////////////////////////////////////////////////////////////////

  class compDetElements_RAZ {

  public:

    bool operator () (const InDetDD::SiDetectorElement* e1,
		      const InDetDD::SiDetectorElement* e2) 
      {
      
	Amg::Vector3D p1  = e1->center();
	Amg::Vector3D p2  = e2->center();
	double r1 = sqrt(p1.x()*p1.x()+p1.y()*p1.y());
	double r2 = sqrt(p2.x()*p2.x()+p2.y()*p2.y()); 
	if(fabs(r1-r2)>.1) return(r1<r2);
	double f1 = atan2(p1.y(),p1.x());
	double f2 = atan2(p2.y(),p2.x());
	return (fabs(f1-f2)>.04) ? (f1<f2) : (p1.z()<p2.z());
      }
  };
  
  ///////////////////////////////////////////////////////////////////
  // Object function for ordering endcap pixels and silicons wafers 
  // in Z, rings and azimuthal angle order
  ///////////////////////////////////////////////////////////////////

  class compDetElements_ZRA {
    
  public:
    
    bool operator () (const InDetDD::SiDetectorElement* e1,
		      const InDetDD::SiDetectorElement* e2) 
      {
      
	Amg::Vector3D p1  = e1->center();
	Amg::Vector3D p2  = e2->center();
	double z1 = p1.z();
	double z2 = p2.z(); 
	if(fabs(z1-z2)>.1) return(fabs(z1)<fabs(z2));
	
	if(e1->isPixel() && e2->isSCT  ()) return true ;
	if(e1->isSCT  () && e2->isPixel()) return false;
	
	double r1 = sqrt(p1.x()*p1.x()+p1.y()*p1.y());
	double r2 = sqrt(p2.x()*p2.x()+p2.y()*p2.y()); 
	if(r2-r1> 10.) return true;
	if(r1-r2> 10.) return false;
	
	double f1 = atan2(p1.y(),p1.x());
	double f2 = atan2(p2.y(),p2.x());
	return(f1<f2);
      }
  };

  ///////////////////////////////////////////////////////////////////
  // Object function for ordering InDet::SiDetElementLink_xk
  // in azimuthal angle order
  ///////////////////////////////////////////////////////////////////

  class compDetElements_Azimuthal {

  public:

    bool operator () (const InDet::SiDetElementLink_xk& l1,
		      const InDet::SiDetElementLink_xk& l2) 
      {
	return (l1.phi() < l2.phi());
      }
  };

}
#endif // SiDetElementsComparison_h
