/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArShape32MC.h" 

LArShape32MC::LArShape32MC() :CONTAINER()
{ } 


StatusCode LArShape32MC::initialize(){
  return (CONTAINER::initialize()); 
}

LArShape32MC::~LArShape32MC() {}


/* Fill transient object in ATHENA *****************************************
*/
	
void LArShape32MC::set(const HWIdentifier& CellID, int gain,
		       const std::vector<float>& vShape,
		       const std::vector<float>& vShapeDer){
  LArShapeP1 t;
  t.m_vShape    = vShape;
  t.m_vShapeDer = vShapeDer;
  setPdata(CellID, t, gain); 	
}

/* retrieve Shape ******************************************************
 */
LArShape32MC::ShapeRef_t
LArShape32MC::Shape(const HWIdentifier&,
                    int,
                    int /*tbin*/,
                    int /*mode*/) const 
{ 
  std::cout << "LArShape32MC is deprecated. Use LArShape32Sym" << std::endl;
  std::abort();
  return LArShape32MC::ShapeRef_t();
}

LArShape32MC::ShapeRef_t
LArShape32MC::ShapeDer(const HWIdentifier&,
                       int,
                       int /*tbin*/,
                       int /*mode*/) const 
{ 
  std::cout << "LArShape32MC is deprecated. Use LArShape32Sym" << std::endl;
  std::abort();
  return LArShape32MC::ShapeRef_t();
}
