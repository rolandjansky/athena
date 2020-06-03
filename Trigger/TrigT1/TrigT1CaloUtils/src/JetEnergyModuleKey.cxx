/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetEnergyModuleKey.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"
// #include "TrigT1CaloUtils/ICoordinate.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

#include "TrigT1CaloUtils/JetElementKey.h"

#else
//// running in TRIGGERSPACE
#include "JetEnergyModuleKey.h"
#include "TrigT1CaloDefs.h"
#include "ICoordinate.h"
#include "JetElementKey.h"
#include "CoordToHardware.h"

//
#endif

#include <math.h>

namespace LVL1 {

/** constructs a JetEnergyModuleKey object*/
JetEnergyModuleKey::JetEnergyModuleKey() : KeyUtilities(), m_debug(false){
	if (m_debug) std::cout << "JetEnergyModuleKey: m_debug output turned on...."<<std::endl;
}

JetEnergyModuleKey::~JetEnergyModuleKey(){
}

/** returns trigger tower key of passed Coordinate */
unsigned int JetEnergyModuleKey::jemKey(const xAOD::JetElement* jetElement) {
  return key(jetElement->phi(), jetElement->eta());
}



/** returns trigger tower key of passed crate+module */
unsigned int JetEnergyModuleKey::jemKey(unsigned int crate, unsigned int module) {
  double eta = ((module%8)+0.5)*TrigT1CaloDefs::jemEtaSize - 3.2;
  int quadrant = crate + ((module>7) ? 2 : 0);
  double phi = (quadrant+0.5)*TrigT1CaloDefs::jemPhiSize;
  return key(phi, eta);
}

/** returns JetElementKeys of all JetElements in passed crate+module */
std::vector<unsigned int> JetEnergyModuleKey::jeKeys(unsigned int crate, unsigned int module) {
  std::vector<unsigned int> keys;

  // Protection
  if (crate > 1 || module > 15) return keys;

  // First get coordinates
  std::vector<Coordinate> coords = jeCoords(crate, module);

  // Then calculate keys
  std::vector<Coordinate>::const_iterator it = coords.begin();
  JetElementKey get(0.,0.);
  for (; it != coords.end(); it++) {
    keys.push_back(get.jeKey(*it));
  }
  
  return keys;
}


/** returns coordinates of all JetElements in passed crate+module */
std::vector<Coordinate> JetEnergyModuleKey::jeCoords(unsigned int crate, unsigned int module) {
  std::vector<Coordinate> coords;

  // Protection
  if (crate > 1 || module > 15) return coords;

  /** Get keys for core JetElement locations in this module<br>
       Algorithm: start from bottom-left JetElement coordinate in JEM<br>
       Then step up by 8 elements in phi (4 in FCAL)<br>
       Increment eta and repeat<br>
       Do so for all 4 eta bins in JEM */
       
  int top = ( module < 8 ? 0 : 1 );
  double phiStart = crate*TrigT1CaloDefs::jemPhiSize
                  + 2.*top*TrigT1CaloDefs::jemPhiSize
                  + 0.5*TrigT1CaloDefs::Region0Height;
  double etaStart;
  // Central JEMs are easy
  if ( (module%8) > 0) {
    etaStart = (module%8)*TrigT1CaloDefs::jemEtaSize - 3.1;
  }
  // -ive eta end
  else {
    etaStart = -TrigT1CaloDefs::Region5EtaCentre;
  }

  // First coordinate for this JEM
  Coordinate startCol(phiStart, etaStart);
  // Nested incrementation of eta and phi
  CoordToHardware cth;
  JetElementKey get(0.,0.);
  while (cth.jepModule( startCol ) == module) {
    Coordinate next = get.getCentre(startCol);
    while (cth.jepCrate( next ) == crate) {
      coords.push_back(next);
      next = get.upPhi(next);
    }
    startCol = get.rightEta(startCol);
    if (startCol.eta() == TrigT1CaloDefs::RegionERROREtaCentre) break;
  }
  
  return coords;
}

/** returns the phi row of a coord within the JEM that contains it. Used in energy trigger (JEM) */
unsigned int LVL1::JetEnergyModuleKey::row(const Coordinate& coord) const{
  double phi=coord.phi();
  double rowHeight=TrigT1CaloDefs::jemPhiSize/8.0;

  unsigned int rowsPerJEM = 8u;
  int rowNum=static_cast<int>(phi/rowHeight);
  rowNum=rowNum%rowsPerJEM;
  return rowNum;
}

/** returns the phi row of a coord within the JEM that contains it.
Used in energy trigger (JEM) */
unsigned int LVL1::JetEnergyModuleKey::row(const xAOD::JetElement* JE) const{
  Coordinate coord(JE->phi(), JE->eta());
  return row( coord );
}


/** returns the phi coord of the row of the JEM at the coordinates passed*/
double LVL1::JetEnergyModuleKey::rowPhiCoord(unsigned int rowNum, const Coordinate& jemCoord){
  //not sure how to do this.
  //should really reuse code, but above is tied to KU stuff. Probably way to go though.

  ICoordinate* iCoord = convertCoordsToIntegers(jemCoord.phi(), jemCoord.eta());
  int etaBin=0; unsigned int phiBin=0;
  setBins(iCoord,phiBin,etaBin);
  // now work out row coord.

  if (rowNum>7){
    std::cerr << "ERROR!!! JetEnergyModuleKey::rowPhiCoord exceeds allowed range. Row, "<<rowNum
         <<" will be set to zero"<<std::endl;
    rowNum=0;
  }
  double phi =( (static_cast<double>(phiBin))*TrigT1CaloDefs::jemPhiSize ); //bot of JEM
  phi+= static_cast<double>( rowNum )*0.2+0.1; //mid of row
  delete iCoord;
  if (m_debug){
    if ( (phi<(jemCoord.phi()-0.8))||(phi>(jemCoord.phi()+0.8) ) ){
    std::cerr << "ERROR!!! JetEnergyModuleKey::rowPhiCoord phi of row is "<<phi
         <<" in a JEM at ("<<jemCoord.phi()<<", "<<jemCoord.eta()<<")"<<std::endl;
    }//endif phi sensible
  }//endif debug
  return phi;
}

/**return height of JEM*/
double JetEnergyModuleKey::dPhi(const Coordinate& /*coord*/) const{
  return M_PI/2;// one quadrant
}

/**return width of JE*/
double JetEnergyModuleKey::dEta(const Coordinate& coord) const{
  if (region(coord) == MidJEM) return 0.8;
  if ((region(coord) == LeftEndJEM)||(region(coord) == RightEndJEM))
      return 2.5;//end JEMs cover 2.4-4.9
  return 0.0;
}

} //end of ns


/** r */
LVL1::JetEnergyModuleKey::JEMRegion
    LVL1::JetEnergyModuleKey::region(const Coordinate& coord) const{
  double absEta=fabs(coord.eta());
  if (absEta<=2.4) return MidJEM;
  if ((coord.eta()>2.4)&&(coord.eta()<=4.9))return RightEndJEM;
  if ((coord.eta()>=-4.9)&&(coord.eta()<-2.4))return LeftEndJEM;
  std::cerr << "JetEnergyModuleKey::region ... Unknown region!"<<std::endl
            << "Coordinate is ("<<coord.phi()<<", "<<coord.eta()<<")"
            <<std::endl;
  return JEMRegionERROR;
}



/** sets the eta and phi bins*/
void LVL1::JetEnergyModuleKey::setBins(ICoordinate* iCoord, unsigned int& phiBin, int& etaBin){
  unsigned int iPhiSize=16 ; // =PI/2 * 10
  phiBin=( iCoord->phi() )/(iPhiSize);
  int iEtaSize=8;// JEMs are 0.8 in eta, i.e ietaSize=8
  etaBin=sharpRound2(iCoord->eta(),iEtaSize);
  // the above calculation fails since end JEMs are larger, so ....
  etaBin=(etaBin < -4 ? -4 : etaBin); // etaBin=-5
  etaBin=(etaBin > 4 ? 4 : etaBin); //   belongs to the JEM in etaBin=-4

  return;
}

/** return row of passed coordinate */
unsigned int LVL1::JetEnergyModuleKey::col(const Coordinate& coord) {
  switch (region(coord)){
    case MidJEM :
      return midJEMEtaCol(coord);
    case LeftEndJEM :
      return leftEndJEMEtaCol(coord);
    case RightEndJEM :
      return rightEndJEMEtaCol(coord);
    default:
      std::cerr << "JetEnergyModuleKey::etaColumn UNKNOWN REGION"<<std::endl;
  }
  return static_cast<unsigned int>(JEMRegionERROR);
}

/** Returns JEM number for coordinate */
unsigned int LVL1::JetEnergyModuleKey::jem(const Coordinate & coord) const {
  double min=3.2; double jemEtaWidth=0.8;
  unsigned int crateModifier=(phiQuadrant(coord.phi())/2);
  //=0 for quad 0&1, =1 for quad 2&3

  double temp = (coord.eta()+min)/jemEtaWidth;
  int jem = static_cast<int>(temp);
  jem=(jem < 0 ? 0 : jem);
  jem=(jem > 7 ? 7 : jem);

  jem+=(8*crateModifier);
  return static_cast<unsigned int>(jem);
}

/** converts integer phi, eta
coordinates to phi, eta trigger bins.
Returns coordinates of centre of JEs*/
LVL1::BinAndCoord* LVL1::JetEnergyModuleKey::calculateTriggerBin(ICoordinate* iCoord){
//  double phiBinWidth=((2*M_PI)/64.0);
  int abs_ieta=abs(iCoord->eta() );
  int sign=( iCoord->eta() )/abs_ieta;

  int etaBin=0; unsigned int phiBin=0;
  setBins(iCoord,phiBin,etaBin);

  double centralPhi=( (static_cast<double>(phiBin))*TrigT1CaloDefs::jemPhiSize )
    + TrigT1CaloDefs::jemPhiSize*0.5;
  double centralEta=0.0;
  if ((etaBin>-4)&&(etaBin<4)){
    centralEta=(static_cast<double>(etaBin)*(TrigT1CaloDefs::jemEtaSize) )
      -(TrigT1CaloDefs::jemEtaSize)*sign*0.5;
  }else{
     centralEta=3.65*sign;//centre of end JEMs
  }
  if (m_debug){
    std::cout << "JetEnergyModuleKey: start calcTrigBin"<<std::endl;
    std::cout << "phi, eta   : ("<<m_phi<<", "<<m_eta<<")"<<std::endl;
    std::cout << "iphi, ieta : ("<<( iCoord->phi() )<<", "<<( iCoord->eta() )<<")"<<std::endl;
    std::cout << "abs_ieta : ("<<abs_ieta<<" and sign :  "<<sign<<std::endl;
    std::cout << "central : ("<<centralPhi<<", "<<centralEta<<")"
        << "bin : ("<<phiBin<<","<<etaBin<<")"<<std::endl;
  }

  Coordinate* centralCoords = new Coordinate(centralPhi, centralEta);
  if (m_debug) std::cout <<" JetEnergyModuleKey : created coord "<<(*centralCoords)<<std::endl;
	return new BinAndCoord(phiBin,etaBin,centralCoords);
}

/** No descriptions */
unsigned int LVL1::JetEnergyModuleKey::midJEMEtaCol(const Coordinate& coord) const{
  double etaWidth=0.2;//only true for middle JEMS
  double JEMEtaSize=0.8;
//  unsigned int columnsPerJEM= 4;
  int etaBin=( jem(coord)%8)-4;//bins range from -4 to +3
  double etaMin=(etaBin*JEMEtaSize);

  double temp =(coord.eta()-etaMin)/etaWidth;
  int colNum=sharpRound(temp);

  colNum=(colNum < 0 ? 0 : colNum);//sorts out any rounding errors
  colNum=(colNum > 3 ? 3 : colNum);

  return colNum;
}
/** returns eta row of JEMs 0,7,8 or 15 */
unsigned int LVL1::JetEnergyModuleKey::leftEndJEMEtaCol(const Coordinate& coord) const{
  // left hand end first
  JetElementKey get;
  switch (get.jeRegion( coord )){
    case JetElementKey::EndCap1 :
      return 3;
      break;
    case JetElementKey::EndCap2 :
      return 2;
      break;
    case JetElementKey::EndCap3 :
      return 1;
      break;
    case JetElementKey::FCAL :
      return 0;
      break;
    case JetElementKey::Barrel :
      // here to stop compiler warnings
      break;
    case JetElementKey::EndBarrel :
      // here to stop compiler warnings
      break;
  }          
  return static_cast<unsigned int>(JEMRegionERROR);
}

/** returns eta row of JEMs 0,7,8 or 15 */
unsigned int LVL1::JetEnergyModuleKey::rightEndJEMEtaCol(const Coordinate& coord) const{
  // left hand end first
  JetElementKey get;
  switch (get.jeRegion( coord )){
    case JetElementKey::EndCap1 :
      return 0;
      break;
    case JetElementKey::EndCap2 :
      return 1;
      break;
    case JetElementKey::EndCap3 :
      return 2;
      break;
    case JetElementKey::FCAL :
      return 3;
      break;
    case JetElementKey::Barrel :
      // here to stop compiler warnings
      break;
    case JetElementKey::EndBarrel :
      // here to stop compiler warnings
      break;
  }
  return static_cast<unsigned int>(JEMRegionERROR);
}



/** returns the quadrant number associated with the phi coordinate,
0   - 90   = 0
90 - 180 = 1
180-270 = 2
270-360 = 3 */
unsigned int LVL1::JetEnergyModuleKey::phiQuadrant(const double phi) const {
  double temp = phi/(M_PI/2);
  unsigned int quad =  static_cast<unsigned int>(temp);
  quad=(quad > 3 ? 0 : quad);

  return static_cast<unsigned int>(quad);
}
