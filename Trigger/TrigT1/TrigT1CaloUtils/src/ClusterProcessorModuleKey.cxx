/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ClusterProcessorModuleKey.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/ClusterProcessorModuleKey.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/ICoordinate.h"

#include "TrigT1CaloUtils/TriggerTowerKey.h"

#else
//// running in TRIGGERSPACE
#include "ClusterProcessorModuleKey.h"
#include "TrigT1CaloDefs.h"
#include "ICoordinate.h"
#include "TriggerTowerKey.h"

//
#endif

#include <math.h>

namespace LVL1 {

/** constructs a ClusterProcessorModuleKey object*/
ClusterProcessorModuleKey::ClusterProcessorModuleKey() : KeyUtilities(), m_debug(false){
  if (m_debug) std::cout << "ClusterProcessorModuleKey: m_debug output turned on...."<<std::endl;
}

ClusterProcessorModuleKey::~ClusterProcessorModuleKey(){
}

/** returns trigger tower key of passed crate+module */
unsigned int ClusterProcessorModuleKey::cpmKey(unsigned int crate, unsigned int module) {
  double eta = module*TrigT1CaloDefs::cpmEtaSize - 3.0; // so module 1 spans -2.8<eta<-2.4
  double phi = (crate+0.5)*TrigT1CaloDefs::cpmPhiSize;
  return key(phi, eta);
}
/**return height of CPM*/
double ClusterProcessorModuleKey::dPhi(const Coordinate& /*coord*/) const{
  return TrigT1CaloDefs::cpmPhiSize;// one quadrant
}

/**return width of JE*/
double ClusterProcessorModuleKey::dEta(const Coordinate& /*coord*/) const{
  return TrigT1CaloDefs::cpmEtaSize;
}

} //end of ns

/** sets the eta and phi bins*/
void LVL1::ClusterProcessorModuleKey::setBins(ICoordinate* iCoord, unsigned int& phiBin, int& etaBin){
  unsigned int iPhiSize=16 ; // 64 bins in total, so 16/module
  phiBin=( iCoord->phi() )/(iPhiSize);
  int iEtaSize=4;// CPMs are 0.4 in eta, so ietaSize=4
  etaBin=sharpRound2(iCoord->eta(),iEtaSize);

  return;
}

/** Returns CPM number for TT coordinate */
unsigned int LVL1::ClusterProcessorModuleKey::cpm(const Coordinate & coord) const {
  /**  TT and RoI coordinates are offset by 0.5*tower width.
       Shift by 0.25*tower width to allow common code without rounding errors */
  double eta = coord.eta() - 0.025; 
  if (fabs(eta)<=2.5) { //  check within range
    double offset=2.8;  // Offset chosen to get RoI->module without rounding errors 
    double temp = (eta+offset)/TrigT1CaloDefs::cpmEtaSize + 1.;
    return static_cast<unsigned int>(temp);
  }
  else {  
    return 999;
  } 
}

/** converts integer phi, eta
coordinates to phi, eta trigger bins.
Returns coordinates of centre of JEs*/
LVL1::BinAndCoord* LVL1::ClusterProcessorModuleKey::calculateTriggerBin(ICoordinate* iCoord){
  int abs_ieta=abs(iCoord->eta() );
  int sign=( iCoord->eta() )/abs_ieta;

  int etaBin=0; unsigned int phiBin=0;
  setBins(iCoord,phiBin,etaBin);

  double centralPhi=( static_cast<double>(phiBin)*TrigT1CaloDefs::cpmPhiSize )
    + TrigT1CaloDefs::cpmPhiSize*0.5;
  double centralEta=( static_cast<double>(etaBin)*TrigT1CaloDefs::cpmEtaSize )
     - (TrigT1CaloDefs::cpmEtaSize)*sign*0.5;
  if (m_debug){
    std::cout << "ClusterProcessorModuleKey: start calcTrigBin"<<std::endl;
    std::cout << "phi, eta   : ("<<m_phi<<", "<<m_eta<<")"<<std::endl;
    std::cout << "iphi, ieta : ("<<( iCoord->phi() )<<", "<<( iCoord->eta() )<<")"<<std::endl;
    std::cout << "abs_ieta : ("<<abs_ieta<<" and sign :  "<<sign<<std::endl;
    std::cout << "central : ("<<centralPhi<<", "<<centralEta<<")"
        << "bin : ("<<phiBin<<","<<etaBin<<")"<<std::endl;
  }

  Coordinate* centralCoords = new Coordinate(centralPhi, centralEta);
  if (m_debug) std::cout <<" ClusterProcessorModuleKey : created coord "<<(*centralCoords)<<std::endl;
	return new BinAndCoord(phiBin,etaBin,centralCoords);
}



/** returns the quadrant number associated with the phi coordinate,
0   - 90   = 0
90 - 180 = 1
180-270 = 2
270-360 = 3 */
unsigned int LVL1::ClusterProcessorModuleKey::phiQuadrant(const double phi) const {
  double temp = phi/TrigT1CaloDefs::cpmPhiSize; // Good for TT coord, not RoI
  unsigned int quad =  static_cast<unsigned int>(temp);
  quad=(quad > 3 ? 0 : quad);

  return static_cast<unsigned int>(quad);
}
