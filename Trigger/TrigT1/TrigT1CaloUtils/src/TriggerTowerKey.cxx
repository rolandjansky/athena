/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          TriggerTowerKey.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/TriggerTowerKey.h"

#else
// running in TRIGGERSPACE
#include "TriggerTowerKey.h"

#endif

#include <cmath>

namespace LVL1 {

/** takes phi and eta and constructs a TriggerTowerKey object
which can return the key for this coord
\*todo This method must be phased out - it will not work when this class simulates the hardware
completely
*/
TriggerTowerKey::TriggerTowerKey(double phi, double eta) {
	m_phi=phi;
	m_eta=eta;
	m_debug=false;
	if (m_debug) { std::cout << "TriggerTowerKey: m_debug output turned on...."<<std::endl; }
	setupThisKeyValues();
	//std::cout << "m_ieta : "<<m_ieta<<std::endl;
}

/** constructs a TriggerTowerKey object
*/
TriggerTowerKey::TriggerTowerKey() {
	m_phi=0.0;
	m_eta=0.0;
	m_debug=false;
	if (m_debug) { std::cout << "TriggerTowerKey: m_debug output turned on...."<<std::endl; }
	setupThisKeyValues();
	//std::cout << "m_ieta : "<<m_ieta<<std::endl;
}

TriggerTowerKey::~TriggerTowerKey(){
}

/** converts integer phi, eta
coordinates to phi, eta trigger bins. */
BinAndCoord* TriggerTowerKey::calculateTriggerBin(ICoordinate* iCoord){

  double phiBinWidth=((2*M_PI)/64.0);
  int iphi=iCoord->phi();
  int ieta=iCoord->eta();
  double centralPhi=0.0;
  double centralEta=0.0;
  int phiBin=0; int etaBin=0;
  int abs_ieta=abs(ieta);
  int sign=ieta/abs_ieta;

  if (m_debug){
    std::cout << "TriggerTowerKey: start calcTrigBin"<<std::endl;
    std::cout << "phi, eta   : ("<<iCoord->phi()<<", "<<iCoord->eta()<<")"<<std::endl;
    std::cout << "iphi, ieta : ("<<iphi<<", "<<ieta<<")"<<std::endl;
    std::cout << "abs_ieta : ("<<abs_ieta<<" and sign :  "<<sign<<std::endl;
  }

  if ( abs_ieta<=25 ){
    /* in 0.1x0.1 barrel region
    This extends from -2.4 to +2.4 in eta, and has divisions of 0.1 in eta.
    ieta extends from -24 to +24, and the centres of ieta=+/-1,2,3 are at
    centralEta=+/-0.05, 0.15, 0.25 etc.*/
    phiBin=iphi;
    etaBin=ieta;
    centralEta=( (static_cast<double>(abs_ieta)*0.1)-0.05)*sign;
    centralPhi=(iphi*phiBinWidth)+(phiBinWidth/2);
    if (m_debug) std::cout << "central : ("<<centralPhi<<", "<<centralEta<<")"<<std::endl;
    //    std::cout<<etaBin<<std::endl;
  }else{
    if (abs_ieta>25 && abs_ieta<=32){
      //std::cout <<"0.2x0.2 :";
      /* in 0.2 x 0.2 region.
      Here we have three 0.2 cells from eta=+-2.5-3.2 (etaBin=26,27,28),
      then a final 0.1 cell from +-3.1 to 3.2 (etaBin=-32).*/
      etaBin=( ( (abs_ieta-24) /2 )+25)*sign;
      // this ^ perhaps seems slightly counter-intuitive ...
      // The point is we have cells like |'|''|''|''|
      // (where the edge of the first cell is ieta=-32) - i.e. the first
      // cell is 0.1x0.2 instead of 0.2x0.2.
      // so if we did (ieta-33)/2+29 as you might expect
      // our algorithm will (wrongly) behave as if the cells are
      // |''|''|''|'| instead.
      phiBin=iphi/2;
      centralPhi=(static_cast<double>(phiBin)*2.0+1.0)*phiBinWidth;
      // it's a bit harder for the centralEta case though so:
      if (abs_ieta<32){
        centralEta=((static_cast<double>(static_cast<int>((abs_ieta-26)/2))*0.2)+2.6)*sign;
      }else{
        //in weird 0.1x0.2 region
        centralEta=3.15*sign;
      }
    }else{
      // in FCAL
      // there's a complication here, namely that the eta bins aren't multiples
      // of 0.1.We therefore must work in doubles ...
      
      double fcalEtaSize=0.425; //=(4.9-3.2)/4

      double abs_eta = iCoord->dEta()*sign;
      double temp=static_cast<double>(abs_eta-3.2);
      temp = (temp/ fcalEtaSize );
      int iTemp=static_cast<int>(temp);
      etaBin=(iTemp+32)*sign;
     
      phiBin=iphi/4;
     
      centralEta=((static_cast<double>(iTemp)*fcalEtaSize)+3.2+(fcalEtaSize/2))*sign;
//std::cout<<"eb: "<<etaBin<<" iTemp"<<iTemp<<std::endl;
      centralPhi=(static_cast<double>(phiBin)*phiBinWidth*4)+(phiBinWidth*2);
    }// end else: in .4x.4
  }// end of not in 0.1x0.1
  if (m_debug) std::cout << "central     : /t("<<centralPhi<<", "<<centralEta<<")"<<std::endl;
  Coordinate* centralCoords = new Coordinate(centralPhi, centralEta);
  BinAndCoord* bandc = new BinAndCoord(phiBin,etaBin,centralCoords);
  return bandc;
}

#ifndef  TRIGGERSPACE
// these methods are only applicable if running in Athena

/** returns the key of the passed tower */
unsigned int TriggerTowerKey::ttKey( const TriggerTower & tower) {
  return key(tower.phi(), tower.eta());
}

/** returns the key of the passed calo cell */
unsigned int TriggerTowerKey::ttKey( const CaloCell & calocell){
  return key(calocell.phi(), calocell.eta());
}
#endif

/** returns trigger tower key of passed coords */
unsigned int TriggerTowerKey::ttKey(const double phi, const double eta){
  if (m_debug) std::cout << "TTK: returning key for coords ("<<phi<<","<<eta<<")"<<std::endl;
  return key(phi, eta);
}

} // end LVL1 namespace

/** returns the TT region that the coordinate corresponds to. */
LVL1::TriggerTowerKey::TriggerTowerRegion
    LVL1::TriggerTowerKey::region(const Coordinate& coord) const{
  double absEta=fabs(coord.eta());
  if (absEta<2.5) return TTBarrel;
  if ((absEta>=2.5)&&(absEta<3.1)) return TTEndCap1;
  if ((absEta>=3.1)&&(absEta<3.2)) return TTEndCap2;
  if ((absEta>=3.2)&&(absEta<=4.9)) return TTFCAL;
  std::cerr <<"TriggerTowerKey::region UNKNOWN REGION"
            << "Coordinate is ("<<coord.phi()<<", "<<coord.eta()<<")"
            <<std::endl;
  return TTRegionError;
}

/** returns eta width of trigger tower at coordinate coord */
double LVL1::TriggerTowerKey::dEta(const Coordinate& coord) const{
  TriggerTowerRegion reg = region(coord);
  if (reg==TTBarrel) return 0.1;
  if (reg==TTEndCap1) return 0.2;
  if (reg==TTEndCap2) return 0.1;
  if (reg==TTFCAL) return 0.425;
  return 0;
}

/** returns phi height of trigger tower at coordinate coord */
double LVL1::TriggerTowerKey::dPhi(const Coordinate& coord) const{
  TriggerTowerRegion reg = region(coord);
  if (reg==TTBarrel) return M_PI/32;//0.0981748;//PI/32
  if (reg==TTEndCap1) return M_PI/16;//0.19635;//PI/16
  if (reg==TTEndCap2) return M_PI/16;//0.19635;
  if (reg==TTFCAL) return M_PI/8;//0.392699;//PI/8
  return TTRegionError;
}
