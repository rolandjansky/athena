/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRT_DetElementsComparison_h
#define  TRT_DetElementsComparison_h

namespace InDet{

  ///////////////////////////////////////////////////////////////////
  // Object function for ordering barrel pixels and silicons wafers 
  // in azimuthal angle order
  ///////////////////////////////////////////////////////////////////

  class compTRTDetElements_A {

  public:

    bool operator () (const InDetDD::TRT_BaseElement* e1,
		      const InDetDD::TRT_BaseElement* e2) 
      {
	const Amg::Transform3D& T1 = e1->surface().transform();
	const Amg::Transform3D& T2 = e2->surface().transform();
	double f1 = atan2(T1(1,0),T1(0,0));
	double f2 = atan2(T2(1,0),T2(0,0));
	return(f1<f2);
      }
  };

  ///////////////////////////////////////////////////////////////////
  // Object function for ordering barrel pixels and silicons wafers 
  // in azimuthal angle and Z coordinates order
  ///////////////////////////////////////////////////////////////////

  class compTRTDetElements_AZ {

  public:

    bool operator () (const InDetDD::TRT_BaseElement* e1,
		      const InDetDD::TRT_BaseElement* e2) 
      {
	Amg::Vector3D p1  = e1->center();
	Amg::Vector3D p2  = e2->center();
	double f1 = atan2(p1.y(),p1.x());
	double f2 = atan2(p2.y(),p2.x());
	return (fabs(f1-f2)>.04) ? (f1<f2) : (p1.z()<p2.z());
      }
  };
}
#endif // TRT_DetElementsComparison_h
