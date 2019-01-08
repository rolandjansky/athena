/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          CoordToHardware.cxx  -  description
                             -------------------
    begin                : Fri Apr 19 2002
    email                : moyse@ph.qmw.ac.uk
 ***************************************************************************/

#ifndef TRIGGERSPACE
#include "TrigT1CaloUtils/CoordToHardware.h"
#else
#include "CoordToHardware.h"
#endif

#include <cmath>
#include <iostream>

LVL1::CoordToHardware::CoordToHardware():
	m_cpmEtaWidth(0.4), m_cpmEtaMax(2.8),
	m_cpFPGAPhiWidth(M_PI/16.0), m_cpEtaMax(2.5), m_debug(false)
{
	fillRoILocalMap();
}

LVL1::CoordToHardware::~CoordToHardware(){
}

/** returns ID of JEP Crate that covers this coordinate.
(quadrants 0+2, 1+3)  */
unsigned int LVL1::CoordToHardware::jepCrate(const Coordinate& Coord){
  unsigned int quad=phiQuadrant( Coord.phi() );
  return (quad%2);
}

/** returns crate ID covering that coord.
 */
unsigned int LVL1::CoordToHardware::cpCrate(const Coordinate& coord){

  return ( phiQuadrant(coord.phi()) );
}


/** returns Module ID of CP RoI ROD
\todo check that this is still correct - it seems sensible but hasn't be formally defined */
unsigned int LVL1::CoordToHardware::cpRoIROD(const Coordinate& coord){
	return ( phiQuadrant(coord.phi()) );
}

/** return ID of CP module */
unsigned int LVL1::CoordToHardware::cpModule(const Coordinate& coord){
	if ( cpCoordIsValid(coord) ) {
		// CPM1 is at -2.6 to -2.4  (here set it to -2.8)
    // CPM8 is at 0.0 to 0.4
 		double eta=coord.eta();
		unsigned int  cpm = static_cast<unsigned int>( (eta+m_cpmEtaMax)/m_cpmEtaWidth )+1;
		return cpm;
	}
	return m_error;
}
/** returns ID of JEP module (i.e. JEM) There are 16 JEMS per crate, and 8 JEMS per quadrant.
The central 6 JEMS cover the barrel, JEMs 0,7,8 & 15 extend over the FCAL as well.
(each crate contains JEMS for opposite quadrants, so one crate might have JEMs 0-7 in quadrant 0
and JEMs 8-15 in quadrant 2)
*/
unsigned int LVL1::CoordToHardware::jepModule(const Coordinate& coord){
  JetEnergyModuleKey jemKey;
  return static_cast<unsigned int>(jemKey.jem(coord));
}

/** returns ID [0-7] of the CP FPGA. The FPGAs cover a region of 0.2 in phi and 0.4 in eta.
FPGA 0 starts at phi=0 within its CPM. */
unsigned int LVL1::CoordToHardware::cpModuleFPGA(const Coordinate& coord){
	if ( cpCoordIsValid(coord) ) {
		double phi=coord.phi();
		// should be 32 FPGAs in 2*PI
		double temp = phi/m_cpFPGAPhiWidth;
		// so temp mod 8 returns FPGA #
  		return ( (static_cast<unsigned int>(temp))%8);
	}
	// not a valid coord
	return m_error;
}
/** fill m_roiLocalMap so that it looks like this:
<pre>
 2 3 6 7
 0 1 4 5
</pre>
 (as usual, phi increases up and eta increases right) */
void LVL1::CoordToHardware::fillRoILocalMap(){

	m_roiLocalMap[0][0]=0;
	m_roiLocalMap[0][1]=1;
	m_roiLocalMap[0][2]=4;
	m_roiLocalMap[0][3]=5;
	m_roiLocalMap[1][0]=2;
	m_roiLocalMap[1][1]=3;
	m_roiLocalMap[1][2]=6;
	m_roiLocalMap[1][3]=7;
}

/** returns local RoI coordinate within FPGA */
unsigned int LVL1::CoordToHardware::cpModuleLocalRoI(const Coordinate& coord){
  if ( cpCoordIsValid(coord) ) {
    double phi=coord.phi();
    double eta=coord.eta();
    // should be 32 FPGAs in 2*PI, and there are 2 local RoI locations in phi per FPGA
    // so "temp" ranges from 0-63
    unsigned int temp = static_cast<unsigned int>( (phi/m_cpFPGAPhiWidth)*2);
    // so this (below) gives us the integer phi coordinate WITHIN the FPGA
    unsigned int iLocalPhi= temp%2;
    // There are 14 CPMs, each with 8 FPGAs that each span it completely in eta
    // (i.e. one eta position for FPGAs in CPM) and there are 4 local RoI
    // locations in eta per FPGA

    temp=static_cast<unsigned int>( (eta+2.8)/0.1 );

    //temp = static_cast<unsigned int>(  (eta+(m_cpmEtaMax+m_cpmEtaWidth)/m_cpmEtaWidth ) * 4);
    unsigned int iLocalEta =   temp%4;
    return m_roiLocalMap[iLocalPhi][iLocalEta];
  }
  // invalid coord
  return m_error;
}

/** returns false if Coord is outside permitted region */
bool LVL1::CoordToHardware::cpCoordIsValid(const Coordinate& coord) const {
  double eta= coord.eta();
	if ( (eta>=-m_cpEtaMax) && (eta <=m_cpEtaMax) ) return true;
  std::cerr << "CoordToHardware::cpCoordIsValid: Coordinate outside permitted range of eta = "
       <<-m_cpEtaMax<<" to "<<m_cpEtaMax<<std::endl;
	return false;
}

/** return JEP (Jet Energy Processing) hardware coordinate word for this coordinate. This word is formed as follows:
-5 bits: RoI local coordinates within FPGA [0-31]
-4 bits: JEM number [0-15]
-1 bits: JEP crate number [0-1]*/
unsigned int LVL1::CoordToHardware::jepCoordinateWord(const Coordinate& coord){
  unsigned int crate =jepCrate(coord);
  unsigned int jem=jepModule(coord);
  unsigned int PJ=jepLocalCoordinate(coord);

  unsigned int word=0;
  if ((crate==m_error)||(jem==m_error)||(PJ==m_error)){
    std::cerr << "CPCoordinateWord ERROR: "<< std::endl;
  }else{
    word = crate; // put 1 bit crate number
    word = (word<<4) + jem; // followed by 4 bit JEM number
  	word = (word<<5) + PJ; // JP
  }
  if (m_debug) std::cout << " Coords set to ("<<coord.phi()<<", "<<coord.eta()<<") giving : "<<word
       << " from crate "<<crate
       << " JEM "<<jem<<" PJ : "<<PJ<< std::endl;
	return word;
}

/** returns the quadrant number associated with the phi coordinate,
0   - 90   = 0
90 - 180 = 1
180-270 = 2
270-360 = 3 */
unsigned int LVL1::CoordToHardware::phiQuadrant(const double phi) const {
  double temp = phi/(M_PI/2);
  return static_cast<unsigned int>(temp);
}

/** return CP (Cluster Processing) hardware coordinate word for this coordinate. This word is formed as follows:
-3 bits: RoI local coordinates within FPGA [0-7]
-3 bits: CP FPGA position within CPM (Cluster Processing module) [0-7]
-4 bits: CPM position within crate [1-14]
-2 bits: CP crate number [0-3]*/
unsigned int LVL1::CoordToHardware::cpCoordinateWord(const Coordinate& coord){
  unsigned int crate =cpCrate(coord);
  unsigned int cpm=cpModule(coord);
  unsigned int fpga=cpModuleFPGA(coord);
  unsigned int lc=cpModuleLocalRoI(coord);

  unsigned int word=0;
  if ((crate==m_error)||(cpm==m_error)||(fpga==m_error)||(lc==m_error)){
    std::cerr << "CPCoordinateWord ERROR: "<< std::endl;
  }else{
    word= crate; // put 2 bit crate number
    word = (word<<4) + cpm; // followed by 4 bit CPM number
    word = (word<<3) + fpga; // 3 bit FPGA number
    word = (word<<3) + lc; //2 bit local coord
  }
  if (m_debug) std::cout << " Coords set to ("<<coord.phi()<<", "<<coord.eta()<<") giving : "<<word
       << " from crate "<<crate
       << " CPM "<<cpm<<" fpga : "<<fpga<<" LC : "<<lc<< std::endl;
  return word;
}
/** This is based on fig 16 of JEM spec */
unsigned int LVL1::CoordToHardware::jepLocalCoordinate(const Coordinate& coord){
  JetEnergyModuleKey jemKey;
  unsigned int row=jemKey.row(coord);
  unsigned int col=jemKey.col(coord);
  unsigned int frame = 4*(col>>1) + (row>>1);
  unsigned int rl = 2*(row%2) + (col%2);
  return (frame<<2)+rl; 
}

/** returns crate ID covering that overlap coord. */
unsigned int LVL1::CoordToHardware::cpCrateOverlap(const Coordinate& coord){

  const double phi = cpPhiOverlap(coord);
  return (phi == coord.phi()) ? m_error
                              : cpCrate(Coordinate(phi, coord.eta()));
}

/** return ID of CP module for overlap coord */
unsigned int LVL1::CoordToHardware::cpModuleOverlap(const Coordinate& coord){

  const double phi = cpPhiOverlap(coord);
  return (phi == coord.phi()) ? m_error
                              : cpModule(Coordinate(phi, coord.eta()));
}

/** returns ID of JEP Crate that covers this overlap coordinate */
unsigned int LVL1::CoordToHardware::jepCrateOverlap(const Coordinate& coord){

  const double phi = jepPhiOverlap(coord);
  return (phi == coord.phi()) ? m_error
                              : jepCrate(Coordinate(phi, coord.eta()));
}

/** returns ID of JEP module (i.e. JEM) for overlap coord */
unsigned int LVL1::CoordToHardware::jepModuleOverlap(const Coordinate& coord){

  const double phi = jepPhiOverlap(coord);
  return (phi == coord.phi()) ? m_error
                              : jepModule(Coordinate(phi, coord.eta()));
}

/** returns a phi in core area for overlap CPM towers */
double LVL1::CoordToHardware::cpPhiOverlap(const Coordinate& coord) const {

  double       phi        = coord.phi();
  const double twoPi      = 2.*M_PI;
  const double piByTwo    = M_PI/2.;
  const double twoPhiBins = M_PI/16.;
  const double phiLoc     = std::fmod(phi, piByTwo);
  if (phiLoc < twoPhiBins) {
    phi -= twoPhiBins;
    if (phi < 0.) phi += twoPi;
  } else if (phiLoc > piByTwo - twoPhiBins) {
    phi += twoPhiBins;
    if (phi >= twoPi) phi -= twoPi;
  }
  return phi;
}

/** returns a phi in core area for overlap jet elements */
double LVL1::CoordToHardware::jepPhiOverlap(const Coordinate& coord) const {

  double       phi        = coord.phi();
  const double eta        = coord.eta();
  const bool   fcal       = eta < -3.2 || eta > 3.2;
  const double twoPi      = 2.*M_PI;
  const double piByTwo    = M_PI/2.;
  const double twoPhiBins = M_PI/8.;
  const double onePhiBin  = (fcal) ? twoPhiBins : twoPhiBins/2.;
  const double phiLoc     = std::fmod(phi, piByTwo);
  if (phiLoc < twoPhiBins) {
    phi -= twoPhiBins;
    if (phi < 0.) phi += twoPi;
  } else if (phiLoc > piByTwo - onePhiBin) {
    phi += onePhiBin;
    if (phi >= twoPi) phi -= twoPi;
  }
  return phi;
}
