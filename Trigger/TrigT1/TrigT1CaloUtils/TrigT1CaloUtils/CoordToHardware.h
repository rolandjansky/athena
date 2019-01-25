/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CoordToHardware.h  -  description
                             -------------------
    begin                : Fri Apr 19 2002
    email                : moyse@ph.qmw.ac.uk
 ***************************************************************************/


#ifndef COORDTOHARDWARE_H
#define COORDTOHARDWARE_H

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"
#else
// running in TRIGGERSPACE
#include "Coordinate.h"
#include "JetEnergyModuleKey.h"
#endif



/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {


/**returns the trigger hardware components associated with a given Coordinate
  *@author E.Moyse
	\todo this should probably be a static class.
  */
class CoordToHardware {
public: 
	CoordToHardware();
	~CoordToHardware();
  /** returns ID of JEP Crate that covers this coordinate */
  unsigned int jepCrate(const Coordinate& Coord);
  /** returns crate ID covering that coord. 
 */
  unsigned int cpCrate(const Coordinate& coord);
  /** returns Module ID of CP RoI ROD */
  unsigned int cpRoIROD(const Coordinate& coord);
  /** returns ID of JEP module (i.e. JEM) There are 16 JEMS per crate, and 8 JEMS per quadrant.
  The central 6 JEMS cover the barrel, JEMs 0,7,8 & 15 extend over the FCAL as well.
  (each crate contains JEMS for opposite quadrants, so one crate might have JEMs 0-7 in quadrant 0
  and JEMs 8-15 in quadrant 2)
  \todo IMPLEMENT*/
  unsigned int jepModule(const Coordinate& coord);
  /** return ID of CP module. According to the LVL1-LVL2 interface document: CPMs are numbered 1-14, with
	CPM1 covering eta=-2.6 to -2.2 and so on. This is implemented here.*/
  unsigned int cpModule(const Coordinate& coord);
  /** returns ID [0-7] of the CP FPGA. The FPGAs cover a region of 0.2 in phi and 0.4 in eta. FPGA 0 starts at phi=0 within its CPM. */
  unsigned int cpModuleFPGA(const Coordinate& coord);
  /** returns local RoI coordinate within FPGA */
  unsigned int cpModuleLocalRoI(const Coordinate& coord);
  /** return CP (Cluster Processing) hardware coordinate word for this coordinate. This word is formed as follows:
  -3 bits: RoI local coordinates with FPGA [0-7]
  -3 bits: CP FPGA position within CPM (Cluster Processing module) [0-7]
  -4 bits: CPM position within crate [1-14]
  -2 bits: CP crate number [0-3]
  */
  unsigned int cpCoordinateWord(const Coordinate& coord);
  /** return JEP (Jet Energy Processing) hardware coordinate word for this coordinate. This word is formed as follows:
  -4 bits: RoI local coordinates with FPGA [0-15]
  -4 bits: JEM number [0-15]
  -2 bits: JEP crate number [0-2]*/
  unsigned int jepCoordinateWord(const Coordinate& coord);
  /** returns the roi's local coord, in the form of a 5b word.
  - 3 bits: phi row
  - 2 bits: eta col */
  unsigned int jepLocalCoordinate(const Coordinate& coord);
  /** returns crate ID covering that overlap coord. */
  unsigned int cpCrateOverlap(const Coordinate& coord);
  /** return ID of CP module for overlap coord */
  unsigned int cpModuleOverlap(const Coordinate& coord);
  /** returns ID of JEP Crate that covers this overlap coordinate */
  unsigned int jepCrateOverlap(const Coordinate& coord);
  /** returns ID of JEP module (i.e. JEM) for overlap coord */
  unsigned int jepModuleOverlap(const Coordinate& coord);
public: // attributes
	enum SubDet_ID {PPr=0x71, CP=0x72, JEP=0x73};
	
private: // Private methods
  /** No descriptions */
  void fillRoILocalMap();
  /** returns the quadrant number associated with the phi coordinate, 
- 0  - 90 = 0
- 90 -180 = 1
- 180-270 = 2
- 270-360 = 3 */
  unsigned int phiQuadrant(const double phi) const;
  /** returns false if Coord is outside permitted region */
  bool cpCoordIsValid(const Coordinate& coord) const;
  /** returns a phi in core area for overlap CPM towers */
  double cpPhiOverlap(const Coordinate& coord) const;
  /** returns a phi in core area for overlap jet elements */
  double jepPhiOverlap(const Coordinate& coord) const;


private: //attribs
	/** eta width of CP modules (CPMs)*/
	double m_cpmEtaWidth;
	/** extreme value of eta, covered by CPMs. This is expected to be 2.8, and it is assumed that the range is symmetric*/
	double m_cpmEtaMax;
	/** phi width of CP FPGAs*/
	double m_cpFPGAPhiWidth;
	/** contains the maximum permissable eta for CP system. Expected to be 2.5 */
	double m_cpEtaMax;
  /** width of JEMs*/
  //double m_jemEtaWidth ;
	/** contains RoI local coordinate mapping (phi,eta)*/
	unsigned int m_roiLocalMap[2][4];
  /** returned value in the case of an invalid Coordinate error */
	static const unsigned int m_error=999;
  bool m_debug;
  
};

#endif
}//end of ns

