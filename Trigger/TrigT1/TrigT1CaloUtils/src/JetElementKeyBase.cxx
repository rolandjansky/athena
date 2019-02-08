/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                           JetElementKeyBase.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
    
  Converted to base class JetElementKeyBase by Alan Watson, 20/01/06
 ***************************************************************************/

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/JetElementKeyBase.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/ICoordinate.h"

#else
// running in TRIGGERSPACE
#include "JetElementKeyBase.h"
#include "TrigT1CaloDefs.h"
#include "ICoordinate.h"

#endif

#include <math.h>

namespace LVL1 {

/** takes phi and eta and constructs a JetElementKeyBase object
which can return the key for this coord
*/
JetElementKeyBase::JetElementKeyBase(double phi, double eta) :
	KeyUtilities(phi,eta),
	m_debug(false)
	{
	if (m_debug) { std::cout << "JetElementKeyBase: m_debug output turned on...."<<std::endl; }
	//setupRegionData();
	//setupThisKeyValues();
}

/** constructs a JetElementKeyBase object*/
JetElementKeyBase::JetElementKeyBase() :
	KeyUtilities(),
	m_debug(false)
	{
	if (m_debug) { std::cout << "JetElementKeyBase: m_debug output turned on...."<<std::endl; }
	//setupRegionData();
	//setupThisKeyValues();
}

/** constructs a JetElementKeyBase object*/
JetElementKeyBase::JetElementKeyBase(const Coordinate coord) :
	KeyUtilities(coord.phi(),coord.eta()),
	m_debug(false)
	{
	if (m_debug) { std::cout << "JetElementKeyBase: m_debug output turned on...."<<std::endl; }
	//setupRegionData();
	//setupThisKeyValues();
}

JetElementKeyBase::~JetElementKeyBase(){
}


/** converts integer phi, eta
coordinates to phi, eta trigger bins.
Returns coordinates of centre of JEs*/
BinAndCoord* JetElementKeyBase::calculateTriggerBin(ICoordinate* iCoord){

  int iphi=iCoord->phi();
  int ieta=iCoord->eta();
  double centralPhi=0.0;
  double centralEta=0.0;
  int phiBin=0; int etaBin=0;
  int abs_ieta=abs(ieta);
  int sign=ieta/abs_ieta;

  if (m_debug){
    std::cout << "JetElementKeyBase: start calculateTriggerBin"<<std::endl;
    //std::cout << "phi, eta   : ("<<m_phi<<", "<<m_eta<<")"<<std::endl;
    std::cout << "iphi, ieta : ("<<iphi<<", "<<ieta<<")"<<std::endl;
    std::cout << "abs_ieta : ("<<abs_ieta<<") and sign :  "<<sign<<std::endl;
  }
  
  if (abs_ieta==TrigT1CaloDefs::RegionERROREtaBin){
  
    // okay this JE has been deliberately labelled as a wrong 'un.
    // so basically ignore it.
    phiBin=0;
    centralPhi=0.0;
    etaBin=20*sign;
    centralEta=TrigT1CaloDefs::RegionERROREtaCentre;
    
  }else{
  
    int region;
    if ( abs_ieta<23 ) {
      region = 0;
      etaBin=(ieta+1)/2; 
    }
    else if ( abs_ieta<25 ) { // can't see why this region != region 0??
      region = 1;
      etaBin=(ieta+1)/2; 
    }
    else if ( abs_ieta<28 ) { // remaining regions only have 1 eta bin, varying sizes
      region = 2;
      etaBin=13*sign;
    }
    else if ( abs_ieta<30 ) {
      region = 3;
      etaBin=14*sign;
    }
    else if ( abs_ieta<=32 ) {
      region = 4;
      etaBin=15*sign;
    }
    else {
      region = 5;
      etaBin=16*sign;
    }
    
    // Get centre of JE in eta and phi
    if (region == 0) {
      centralEta=(static_cast<double>(etaBin)*0.2)-0.1; 
    }
    else {
      centralEta=m_regionEtaCentre[region]*sign;
    }
    
    // Phi bin size depends on region - this should take care of that
    int nTTperJE = static_cast<int>( 0.5 + (m_regionHeight[region]/(M_PI/32)) );
    phiBin=iphi/nTTperJE;
    centralPhi=(static_cast<double>(phiBin)+0.5)*m_regionHeight[region];
    
  }
  
  if (m_debug) std::cout << "central : ("<<centralPhi<<", "<<centralEta<<")"
                      << " bin : ("<<phiBin<<","<<etaBin<<")"<<std::endl;
  Coordinate* centralCoords = new Coordinate(centralPhi, centralEta);
  BinAndCoord* bandc = new BinAndCoord(phiBin,etaBin,centralCoords);
  return bandc;
}

#ifndef  TRIGGERSPACE
// these methods are only applicable if running in Athena

/** returns key of passed tower */
unsigned int JetElementKeyBase::jeKey( const xAOD::TriggerTower& tower) {
  if (m_debug) std::cout << "JetElementKeyBase: returning key for coords ("<<tower.phi()<<","<<tower.eta()<<")"<<std::endl;
  return key(tower.phi(), tower.eta());
}

/** returns the key of the passed tower */
unsigned int JetElementKeyBase::jeKey( const xAOD::JetElement& jetElement){
  if (m_debug) std::cout << "JetElementKeyBase: returning key for coords ("<<jetElement.phi()<<","<<jetElement.eta()<<")"<<std::endl;
  return key(jetElement.phi(), jetElement.eta());
}

#endif


/** returns trigger tower key of passed coords */
unsigned int JetElementKeyBase::jeKey(const double phi, const double eta){
  if (m_debug) std::cout << "JetElementKeyBase: returning key for coords ("<<phi<<","<<eta<<")"<<std::endl;
  return key(phi, eta);
}

/** returns trigger tower key of passed Coordinate */
unsigned int JetElementKeyBase::jeKey(const Coordinate coord){
  if (m_debug) std::cout << "JetElementKeyBase: returning key for Coord: ("<<coord.phi()<<","<<coord.eta()<<")"<<std::endl;
  return key(coord.phi(), coord.eta());
}



/**returns key of JE in -ve eta dir.*/
Coordinate  JetElementKeyBase::leftEta(const double phi, const double eta){
  Coordinate centre=getCentre(phi,eta);
  double temp_eta=centre.eta();

  // look in method for definition of regions
  unsigned int reg=region(temp_eta);
  // suppress leftEta in -ve region 5
  unsigned int maxRegion=5+((temp_eta>0.0)? 0:-1);
  if ( reg==0 ){
    temp_eta-=m_regionWidth[0];
  }else{
    if ((reg==TrigT1CaloDefs::RegionError)||(reg>maxRegion)) {
      temp_eta=TrigT1CaloDefs::RegionERROREtaCentre;
   }else{
      int sign = ((temp_eta>0.0)? 1:-1);
      temp_eta= sign*m_regionEtaCentre[reg-sign];    }
  }
  // there's a complication here ... moving left can move us
  // into a JE with a different phi height, and so phi can change.
  // However we want to keep eta at 8.0, so reset eta afterwards.
  Coordinate coord( getCentre(phi,temp_eta) );
  coord.setCoords(coord.phi(), temp_eta) ;
//  analCoordChecking(coord);
	return coord;
}

/**returns key of JE in -ve eta dir.*/
Coordinate  JetElementKeyBase::leftEta(const Coordinate coord){
	return leftEta(coord.phi(), coord.eta());
}

/**returns key of JE in -ve eta dir.*/
unsigned int JetElementKeyBase::leftEtaKey(const double phi, const double eta){
  Coordinate coord=leftEta(phi,eta);
  return jeKey(coord.phi(),coord.eta());
}

/**returns key of JE in +ve eta dir.*/
unsigned int JetElementKeyBase::rightEtaKey(const double phi, const double eta){
  Coordinate coord=rightEta(phi,eta);
  return jeKey(coord.phi(),coord.eta());
}

/**returns eta coord of JE in +ve eta dir.*/
Coordinate JetElementKeyBase::rightEta(const double phi,const double eta){
  Coordinate centre=getCentre(phi,eta);
  double temp_eta=centre.eta();

  unsigned int reg=region(temp_eta);
  // supress rightEta in +ve region 5
  unsigned int maxRegion=5+((temp_eta>0.0)? -1:0);
  if ( reg==0 ){
    temp_eta+=m_regionWidth[0];
  }else{
    if ((reg==TrigT1CaloDefs::RegionError)||(reg>maxRegion)) {
      temp_eta=TrigT1CaloDefs::RegionERROREtaCentre;
    }else{
      int sign = ((temp_eta>0.0)? 1:-1);
      temp_eta= sign*m_regionEtaCentre[reg+sign];
    }
  }
  // there's a complication here ... moving right can move us
  // into a JE with a different phi height, and so phi can change.
  // However we want to keep eta at 8.0, so reset eta afterwards.
  Coordinate coord( getCentre(phi,temp_eta) );
  coord.setCoords(coord.phi(), temp_eta) ;
//  analCoordChecking(coord);
  return coord;
}

/**returns eta coord of JE in +ve eta dir.*/
Coordinate JetElementKeyBase::rightEta(const Coordinate coord){
	return rightEta(coord.phi(), coord.eta());
}


/**returns coord of next JE in +ve phi dir.*/
Coordinate JetElementKeyBase::upPhi(const double phi, const double eta){
  Coordinate centre=getCentre(phi,eta);
  double temp_phi=centre.phi();
  unsigned int reg=region(centre.eta());
  if (reg==TrigT1CaloDefs::RegionError) {
//    std::cout << "Out of bounds error in JetElementKeyBase::upPhi"<<std::endl;
    temp_phi=0.0;
  }else{
    temp_phi+= m_regionHeight[reg];
  }
  centre.setCoords(temp_phi,centre.eta());
  analCoordChecking(centre);
  return centre;
}

/**returns coord of next JE in +ve phi dir.*/
Coordinate JetElementKeyBase::upPhi(const Coordinate coord){
	return upPhi(coord.phi(), coord.eta());
}

/**returns key of next JE in +ve phi dir.*/
unsigned int JetElementKeyBase::upPhiKey(const double phi, const double eta){
	Coordinate coord=upPhi(phi,eta);
	return jeKey(coord.phi(),coord.eta());
}

/**returns coord of next JE in -ve phi dir.*/
Coordinate JetElementKeyBase::downPhi(const double phi, const double eta){
  Coordinate centre=getCentre(phi,eta);
  double temp_phi=centre.phi();
  unsigned int reg=region(centre.eta());
  if (reg==TrigT1CaloDefs::RegionError) {
//    std::cout << "Out of bounds error in JetElementKeyBase::downPhi"<<std::endl;
    temp_phi=0.0;
  }else{
    temp_phi-= m_regionHeight[reg];
  }
  centre.setCoords(temp_phi,centre.eta());
  analCoordChecking(centre);  
  return centre;
}

/**returns coord of next JE in -ve phi dir.*/
Coordinate JetElementKeyBase::downPhi(const Coordinate coord){
	return downPhi(coord.phi(), coord.eta());
}


/**returns key of next JE in -ve phi dir.*/
unsigned int JetElementKeyBase::downPhiKey(const double phi, const double eta){
	Coordinate coord=downPhi(phi,eta);
	return jeKey(coord.phi(),coord.eta());
}

Coordinate JetElementKeyBase::upperLeft(const double phi,const double eta){
	Coordinate coord=upPhi(phi,eta);
	coord=leftEta(coord.phi(),coord.eta());
	return coord;
}

Coordinate JetElementKeyBase::upperRight(const double phi,const double eta){
	Coordinate coord=upPhi(phi,eta);
	coord=rightEta(coord.phi(),coord.eta());
	return coord;
}

Coordinate JetElementKeyBase::lowerLeft(const double phi,const double eta){
	Coordinate coord=downPhi(phi,eta);
	coord=leftEta(coord.phi(),coord.eta());
	return coord;
}

Coordinate JetElementKeyBase::lowerRight(const double phi,const double eta){
	Coordinate coord=downPhi(phi,eta);
	coord=rightEta(coord.phi(),coord.eta());
	return coord;
}

Coordinate JetElementKeyBase::upperLeft(const Coordinate coord){
	Coordinate newCoord=upperLeft(coord.phi(),coord.eta());
	return newCoord;
}

Coordinate JetElementKeyBase::upperRight(const Coordinate coord){
	Coordinate newCoord=upperRight(coord.phi(),coord.eta());
	return newCoord;
}

Coordinate JetElementKeyBase::lowerLeft(const Coordinate coord){
	Coordinate newCoord=lowerLeft(coord.phi(),coord.eta());
	return newCoord;
}

Coordinate JetElementKeyBase::lowerRight(const Coordinate coord){
	Coordinate newCoord=lowerRight(coord.phi(),coord.eta());
	return newCoord;
}

/**return height of JE at (phi,eta)*/
double JetElementKeyBase::dPhi(const double /*phi*/, const double eta) const {
	unsigned int reg=region(eta);

	if (reg==TrigT1CaloDefs::RegionError) return 0.0;
	return m_regionHeight[reg];
}

/**return height of this JE*/
double JetElementKeyBase::dPhi() const{
	return dPhi(m_phi,m_eta);
}

/**return height of JE*/
double JetElementKeyBase::dPhi(const Coordinate& coord) const{
  return dPhi(coord.phi(), coord.eta() );
}

/**return width of JE at (phi,eta)*/
double JetElementKeyBase::dEta(const double /*phi*/, const double eta) const{
  unsigned int reg=region(eta);
  if (reg==TrigT1CaloDefs::RegionError) return 0.0;

  return m_regionWidth[reg];
}

/**return width of this JE*/
double JetElementKeyBase::dEta() const{
  return dEta(m_phi,m_eta);
}

/**return width of JE*/
double JetElementKeyBase::dEta(const Coordinate& coord) const{
  return dEta(coord.phi(), coord.eta() );
}

unsigned int JetElementKeyBase::region(double eta) const{
  unsigned int reg=TrigT1CaloDefs::RegionError;
  int sign=((eta>0.0) ? 1:-1);
  if (eta==0.0) sign=1;
  double abs_eta=eta*sign;
  if (abs_eta<2.2){
    // barrel
    reg=0;
  }else{
    if((abs_eta>2.2)&&(abs_eta<=2.4)) reg=1;
    if((abs_eta>2.4)&&(abs_eta<=2.7)) reg=2;
    if((abs_eta>2.7)&&(abs_eta<=2.9)) reg=3;
    if((abs_eta>2.9)&&(abs_eta<=3.2)) reg=4;
    if((abs_eta>3.2)&&(abs_eta<=4.9)) reg=5;
  }//end if
  return reg;
}

JetElementKeyBase::JERegion JetElementKeyBase::jeRegion(const Coordinate& coord) const{
  // really unhappy about this. It'll go horribly wrong if anyone alters the enum.
  // Ideally I'd like region(eta) to return an enum, and have a
  // separate numRegion(eta) which returns ints.
  // That will require a lot of re-writing though....
  unsigned int reg=region(coord.eta());
  if (reg==5) return FCAL;
  if (reg==4) return EndCap3;
  if (reg==3) return EndCap2;
  if (reg==2) return EndCap1;
  if (reg==1) return EndBarrel;
  return Barrel;
  
}

bool JetElementKeyBase::isFCAL(double eta) const{
  return (region(eta)==5);
}

/** get region data from TrigT1CaloDefs and put in more useful format -
vectors*/
void JetElementKeyBase::setupRegionData(){
	// the first case is a bit weird - it's actually the centre
	// of the outermost cell of region 0
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region0EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region1EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region2EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region3EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region4EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region5EtaCentre);

	m_regionWidth.push_back(TrigT1CaloDefs::Region0Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region1Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region2Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region3Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region4Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region5Width);

	m_regionHeight.push_back(TrigT1CaloDefs::Region0Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region1Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region2Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region3Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region4Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region5Height);
}

/** Does same as setupRegionData, except in FCAL where uses phi granularity
    appropriate for the jet trigger. */
void JetElementKeyBase::setupJetRegionData(){
	// the first case is a bit weird - it's actually the centre
	// of the outermost cell of region 0
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region0EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region1EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region2EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region3EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region4EtaCentre);
	m_regionEtaCentre.push_back(TrigT1CaloDefs::Region5EtaCentre);

	m_regionWidth.push_back(TrigT1CaloDefs::Region0Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region1Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region2Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region3Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region4Width);
	m_regionWidth.push_back(TrigT1CaloDefs::Region5Width);

	m_regionHeight.push_back(TrigT1CaloDefs::Region0Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region1Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region2Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region3Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region4Height);
	m_regionHeight.push_back(TrigT1CaloDefs::Region5JetHeight);
}

/** returns the central coordinate of the JE which contains the passed coord*/
Coordinate JetElementKeyBase::getCentre(const double phi, const double eta){
  ICoordinate* iCoord = convertCoordsToIntegers(phi, eta);
  BinAndCoord* bandc  = calculateTriggerBin(iCoord);
  Coordinate yuck=*(bandc->coords() ); // this is horrible, but easier than changing other stuff
  delete iCoord; delete bandc ;
  return yuck;
}

/** returns the central coordinate of the JE which contains the passed coord*/
Coordinate JetElementKeyBase::getCentre(const Coordinate coord){
  Coordinate centre = getCentre( coord.phi(), coord.eta() );
  return centre;
}

} // end LVL1 namespace

/** this is a bit over the top, but it looks to see if the
calculated coordinate matches a coordinate centre that
JetElementKeyBase would return. It doesn't return any errors or
anything as it's purely being used in tests external to Athena */
void LVL1::JetElementKeyBase::analCoordChecking(const Coordinate& coord){
  Coordinate centre( getCentre( coord.phi(), coord.eta() ) );
  double allowedSpread=0.0002;// I don't mind a bit of a rounding error....
  if ( (coord.phi()<(centre.phi()-allowedSpread))||(coord.phi()>(centre.phi()+allowedSpread) ) )
    std::cout << "PHI ERROR: "<< coord.phi() <<"!="<<centre.phi()<<std::endl;
  if ( (coord.eta()<(centre.eta()-allowedSpread))||(coord.eta()>(centre.eta()+allowedSpread) ) )
    std::cout << "ETA ERROR: "<< coord.eta() <<"!="<<centre.eta()<<std::endl;
  return;
}



