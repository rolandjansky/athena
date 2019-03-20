/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//==============================================================================
// $Id: TrigT1CaloDefs.h 632038 2014-11-28 19:27:49Z amazurov $
//==============================================================================

#ifndef TRIGT1CALODEFS_H
#define TRIGT1CALODEFS_H
#include <string>

#ifndef  TRIGGERSPACE
// running in Athena
//#include "StoreGate/DataVector.h"
//#include "TrigT1Calo/EmTauROI.h"

#else
// running in TRIGGERSPACE

#endif


namespace LVL1 {
class TrigT1CaloDefs {

public:

#ifndef  TRIGGERSPACE
//typedefs
//	typedef DataVector<EmTauROI> t_EmTauROICollection ;

  // locations
  static const std::string CaloCellContainerLocation;
  
  static const std::string TriggerTowerLocation;
  static const std::string xAODTriggerTowerLocation;
  static const std::string xAODTriggerTowerRerunLocation;
  static const std::string CPMTowerLocation;
  static const std::string JetElementLocation;
  
  static const std::string CPMCMXDataLocation;
  static const std::string JetCMXDataLocation;
  static const std::string EnergyCMXDataLocation;
  
  static const std::string CPMHitsLocation;
  static const std::string JEMHitsLocation;
  static const std::string JEMEtSumsLocation;
  
  static const std::string CMMJetHitsLocation;
  static const std::string CMMEtSumsLocation;
  static const std::string CMMCPHitsLocation;
  static const std::string CMMRoILocation;
  static const std::string CMXJetTobLocation;
  static const std::string CMXJetHitsLocation;
  static const std::string CMXEtSumsLocation;
  static const std::string CMXCPTobLocation;
  static const std::string CMXCPHitsLocation;
  static const std::string CMXRoILocation;
  static const std::string CPMRoILocation;
  static const std::string CPMTobRoILocation;
  static const std::string JEMRoILocation;
  static const std::string JEMTobRoILocation;
  static const std::string CPBSCollectionLocation;
  static const std::string JEPBSCollectionLocation;
  static const std::string JEPRoIBSCollectionLocation;
  static const std::string RODHeaderLocation;
  
  static const std::string AtlfastCellContainerLocation;

  static const std::string JetROILocation;
  static const std::string EnergyROILocation;
  static const std::string EmTauROILocation;
  static const std::string JetEtROILocation;
  
  static const std::string EmTauCTPLocation;
  static const std::string JetCTPLocation;
  static const std::string EnergyCTPLocation;

  static const std::string EmTauSlinkLocation;
  static const std::string jepSlinkLocation;
  static const std::string energySlinkLocation;

  static const std::string EmTauTopoTobLocation;
  static const std::string JetTopoTobLocation;
  static const std::string EnergyTopoDataLocation;
  
  static const std::string CTPCaloConfigLocation;
  static const std::string CTPJetEnergyConfigLocation;
#endif	
	//JetElementKey
	static const unsigned int RegionError=999;
	
	static const double Region0EtaCentre;
	static const double Region1EtaCentre;
	static const double Region2EtaCentre;
	static const double Region3EtaCentre;
	static const double Region4EtaCentre;
	static const double Region5EtaCentre;
        static const int RegionERROREtaBin=81;
        static const double RegionERROREtaCentre;
	
	static const double Region0Width;
	static const double Region1Width;
	static const double Region2Width;
	static const double Region3Width;
	static const double Region4Width;
	static const double Region5Width;
	
	static const double Region0Height;
	static const double Region1Height;
	static const double Region2Height;
	static const double Region3Height;
	static const double Region4Height;
	static const double Region5Height;
	
	static const double Region5JetHeight;


static const unsigned int Error=999;

//General hardware
static const unsigned int numOfCPThresholds=16;
static const unsigned int numOfJetThresholds=8;
static const unsigned int numOfFwdJetThresholds=4;
static const unsigned int numOfMissingEtThresholds=8;
static const unsigned int numOfSumEtThresholds=8;
static const unsigned int numOfMEtSigThresholds=8;
static const unsigned int numOfJetEtSumThresholds=4;
static const unsigned int numOfEMCMXThresholds=16;
static const unsigned int numOfTauCMXThresholds=16;
static const unsigned int numOfJetCMXThresholds=25;
static const unsigned int numOfIsolationBits=5;

static const double cpmPhiSize;
static const double cpmEtaSize;
static const double jemPhiSize;
static const double jemEtaSize;

//ROI word data
 
 
enum Run2RoIWordTypes {
  run2JetRoIType=0,
  run2CpRoIType=1,
  run2METRoIType=2,
  run2SETRoIType=3
}  ;

enum JEPRoITypes {
  Run2JetType = 0,
  Energy0Type = 2,
  Energy1Type = 3,
  Run1JetType = 4,
  JetEtType = 5,
  JEPRoITypeError = 999
} ;

enum EmTauTobTypes {
  emTobType = 0,
  tauTobType = 1
} ;
 
enum RoIType {
  CpRoIWordType=0,
  JetRoIWordType=1,
  JetEtRoIWordType=2,
  EnergyRoIWordType0=3,
  EnergyRoIWordType1=4,
  EnergyRoIWordType2=5,
  EMRoIWordType=6,
  TauRoIWordType=7,
  RoIWordTypeError=999
}  ;
 
enum RoIWordTypes {
  cpRoIType=0,
  jetRoIType=2,
  energyRoIType=1
}  ;

enum CPRoIWordTypes {
  EMTauRoIWord = 0,
  EMRoIWord = 10,
  TauRoIWord = 11
} ;

enum JetRoIWordTypes {
  jetRoI=0,
  jetEtRoI=1
}  ;

enum EnergyRoIWordTypes {
  energyRoI0=0,
  energyRoI1=2,
  energyRoI2=1
}  ;

enum FullEnergyRoIWordTypes {
  energyRoIWord0=4,
  energyRoIWord1=6,
  energyRoIWord2=5,
  energyRoIWordError=999
}  ;

enum ClusterAlgorithm {
    EMAlg = 0,
    TauAlg,
    Num,
    ClustAlgError=999
  };

enum JetWindowSize {
    W4 = 4,
    W6 = 6,
    W8 = 8,
    JetWinError=999
};



//ROD
static const unsigned int numOfCPRoIRODs=4;
static const unsigned int numOfJEPRoIRODs=2;

//NB the only other parts of TrigT1Calo that should have explicit understanding of hardware layout
// are the key classes and the CoordToHardware conversion classes.

};

}//end ns
#endif  // TRIGT1CALODEFS_H


