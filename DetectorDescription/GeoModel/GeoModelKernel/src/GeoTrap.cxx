/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A51701E2.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A51701E2.cm

//## begin module%3CD2A51701E2.cp preserve=no
//## end module%3CD2A51701E2.cp

//## Module: GeoTrap%3CD2A51701E2; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTrap.cxx

//## begin module%3CD2A51701E2.additionalIncludes preserve=no
//## end module%3CD2A51701E2.additionalIncludes

//## begin module%3CD2A51701E2.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD2A51701E2.includes

// GeoTrap
#include "GeoModelKernel/GeoTrap.h"
//## begin module%3CD2A51701E2.additionalDeclarations preserve=yes
//## end module%3CD2A51701E2.additionalDeclarations


// Class GeoTrap 

//## begin GeoTrap::s_classType%3CD2A51701E3.attr preserve=no  public: static const std::string {U} "Trap"
const std::string GeoTrap::s_classType = "Trap";
//## end GeoTrap::s_classType%3CD2A51701E3.attr

//## begin GeoTrap::s_classTypeID%3CD2A51701E4.attr preserve=no  public: static const ShapeType {U} 0x15
const ShapeType GeoTrap::s_classTypeID = 0x15;
//## end GeoTrap::s_classTypeID%3CD2A51701E4.attr

GeoTrap::GeoTrap (double ZHalfLength, double Theta, double Phi, double Dydzn, double Dxdyndzn, double Dxdypdzn, double Angleydzn, double Dydzp, double Dxdyndzp, double Dxdypdzp, double Angleydzp)
  //## begin GeoTrap::GeoTrap%3CD5BEDD0276.hasinit preserve=no
  //## end GeoTrap::GeoTrap%3CD5BEDD0276.hasinit
  //## begin GeoTrap::GeoTrap%3CD5BEDD0276.initialization preserve=yes
  :
m_zHalfLength (ZHalfLength),
m_theta (Theta),
m_phi (Phi),
m_dydzn (Dydzn),
m_dxdyndzn (Dxdyndzn),
m_dxdypdzn (Dxdypdzn),
m_angleydzn (Angleydzn),
m_dydzp (Dydzp),
m_dxdyndzp (Dxdyndzp),
m_dxdypdzp (Dxdypdzp),
m_angleydzp (Angleydzp)
  //## end GeoTrap::GeoTrap%3CD5BEDD0276.initialization
{
  //## begin GeoTrap::GeoTrap%3CD5BEDD0276.body preserve=yes
  //## end GeoTrap::GeoTrap%3CD5BEDD0276.body
}


GeoTrap::~GeoTrap()
{
  //## begin GeoTrap::~GeoTrap%3CD2A51701E2_dest.body preserve=yes
  //## end GeoTrap::~GeoTrap%3CD2A51701E2_dest.body
}



//## Other Operations (implementation)
double GeoTrap::volume () const
{
  //## begin GeoTrap::volume%3CD2A6EF014B.body preserve=yes
  double fDz = m_zHalfLength;
  double fDy1 = m_dydzn;		//              
  double pDx1 = m_dxdyndzn;	//              
  double pDx2 = m_dxdypdzn;	//              
  double fDy2 = m_dydzp;		//              
  double pDx3 = m_dxdyndzp;	//              
  double pDx4 = m_dxdypdzp;	//                  


  double fDx1 = 0.5 * (pDx1 + pDx2);
  double fDx2 = 0.5 * (pDx3 + pDx4);

  return 4.0 * ((fDx1 + fDx2) * (fDy1 + fDy2) * (fDz * 0.5) +
		(fDx2 - fDx1) * (fDy2 - fDy1) * (fDz * (1./6)));

  //## end GeoTrap::volume%3CD2A6EF014B.body
}

const std::string & GeoTrap::type () const
{
  //## begin GeoTrap::type%3CD2A8570149.body preserve=yes
  return s_classType;
  //## end GeoTrap::type%3CD2A8570149.body
}

ShapeType GeoTrap::typeID () const
{
  //## begin GeoTrap::typeID%3CD2A8570167.body preserve=yes
  return s_classTypeID;
  //## end GeoTrap::typeID%3CD2A8570167.body
}

void GeoTrap::exec (GeoShapeAction *action) const
{
  //## begin GeoTrap::exec%3DB96A76015C.body preserve=yes
	action->handleTrap(this);
  //## end GeoTrap::exec%3DB96A76015C.body
}

// Additional Declarations
  //## begin GeoTrap%3CD2A51701E2.declarations preserve=yes
  //## end GeoTrap%3CD2A51701E2.declarations

//## begin module%3CD2A51701E2.epilog preserve=yes
//## end module%3CD2A51701E2.epilog
