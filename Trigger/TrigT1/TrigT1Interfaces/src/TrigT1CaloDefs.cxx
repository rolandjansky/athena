/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          TrigT1CaloDefs.cxx  -  description
                             -------------------
    begin                : Mon Apr 22 2002
    email                : e.moyse@qmul.ac.uk
 ***************************************************************************/

#ifndef TRIGGERSPACE
//running in Athena
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#else
//TRIGGERSPACE
#include "TrigT1CaloDefs.h"
#endif

#include <math.h>

namespace LVL1 {


#ifndef TRIGGERSPACE
//General Default locations:
const std::string TrigT1CaloDefs::CaloCellContainerLocation="AllCalo";

const std::string TrigT1CaloDefs::AtlfastCellContainerLocation="/Event/AtlfastCells";

const std::string TrigT1CaloDefs::TriggerTowerLocation="TriggerTowers";
const std::string TrigT1CaloDefs::xAODTriggerTowerLocation="xAODTriggerTowers";
const std::string TrigT1CaloDefs::xAODTriggerTowerRerunLocation="xAODTriggerTowers_rerun";
const std::string TrigT1CaloDefs::JetElementLocation="JetElements";
const std::string TrigT1CaloDefs::CPMTowerLocation="CPMTowers";

const std::string TrigT1CaloDefs::CPMHitsLocation="CPMHits";
const std::string TrigT1CaloDefs::JEMHitsLocation="JEMHits";
const std::string TrigT1CaloDefs::JEMEtSumsLocation="JEMEtSums";

const std::string TrigT1CaloDefs::CPMCMXDataLocation="CPCMXData";
const std::string TrigT1CaloDefs::JetCMXDataLocation="JetCMXData";
const std::string TrigT1CaloDefs::EnergyCMXDataLocation="EnergyCMXData";

const std::string TrigT1CaloDefs::CMMJetHitsLocation="CMMJetHits";
const std::string TrigT1CaloDefs::CMMEtSumsLocation="CMMEtSums";
const std::string TrigT1CaloDefs::CMMCPHitsLocation="CMMCPHits";
const std::string TrigT1CaloDefs::CMMRoILocation="CMMRoIs";
const std::string TrigT1CaloDefs::CMXJetTobLocation="CMXJetTobs";
const std::string TrigT1CaloDefs::CMXJetHitsLocation="CMXJetHits";
const std::string TrigT1CaloDefs::CMXEtSumsLocation="CMXEtSums";
const std::string TrigT1CaloDefs::CMXCPTobLocation="CMXCPTobs";
const std::string TrigT1CaloDefs::CMXCPHitsLocation="CMXCPHits";
const std::string TrigT1CaloDefs::CMXRoILocation="CMXRoIs";
const std::string TrigT1CaloDefs::CPMRoILocation="CPMRoIs";
const std::string TrigT1CaloDefs::CPMTobRoILocation="CPMTobRoIs";
const std::string TrigT1CaloDefs::JEMRoILocation="JEMRoIs";
const std::string TrigT1CaloDefs::JEMTobRoILocation="JEMTobRoIs";
const std::string TrigT1CaloDefs::CPBSCollectionLocation="CPBSCollection";
const std::string TrigT1CaloDefs::JEPBSCollectionLocation="JEPBSCollection";
const std::string TrigT1CaloDefs::JEPRoIBSCollectionLocation="JEPRoIBSCollection";
const std::string TrigT1CaloDefs::RODHeaderLocation="RODHeaders";

const std::string TrigT1CaloDefs::JetROILocation="JetROIs";
const std::string TrigT1CaloDefs::EnergyROILocation="EnergyROIs";
const std::string TrigT1CaloDefs::EmTauROILocation="EmTauROIs";
const std::string TrigT1CaloDefs::JetEtROILocation="JetEtROIs";

const std::string TrigT1CaloDefs::EmTauCTPLocation="CaloTriggerDataLocation/EmTauCTP";
const std::string TrigT1CaloDefs::JetCTPLocation="CaloTriggerDataLocation/JetCTP";
const std::string TrigT1CaloDefs::EnergyCTPLocation="CaloTriggerDataLocation/EnergyCTP";

const std::string TrigT1CaloDefs::EmTauSlinkLocation="CaloTriggerDataLocation_EmTauSlink";
const std::string TrigT1CaloDefs::jepSlinkLocation="CaloTriggerDataLocation_JEPSlink";
const std::string TrigT1CaloDefs::energySlinkLocation="CaloTriggerDataLocation_JEPEnergySlink";

const std::string TrigT1CaloDefs::EmTauTopoTobLocation="EmTauTopoTOBs";
const std::string TrigT1CaloDefs::JetTopoTobLocation="JetTopoTOBs";
const std::string TrigT1CaloDefs::EnergyTopoDataLocation="EnergyTopoData";

const std::string TrigT1CaloDefs::CTPCaloConfigLocation="/Run/CaloTrigConfig";
const std::string TrigT1CaloDefs::CTPJetEnergyConfigLocation="/Run/JetEnergyTrigConfig";

#endif



//JetElementKey specific
const double TrigT1CaloDefs::Region0EtaCentre=2.1;// this is an exception
const double TrigT1CaloDefs::Region1EtaCentre=2.3;
const double TrigT1CaloDefs::Region2EtaCentre=2.55;
const double TrigT1CaloDefs::Region3EtaCentre=2.8;
const double TrigT1CaloDefs::Region4EtaCentre=3.05;
const double TrigT1CaloDefs::Region5EtaCentre=4.05;

// TrigT1CaloDefs::RegionERROREtaBin defined in header.
const double TrigT1CaloDefs::RegionERROREtaCentre=8.0;
	  
const double TrigT1CaloDefs::Region0Width=0.2;
const double TrigT1CaloDefs::Region1Width=0.2;
const double TrigT1CaloDefs::Region2Width=0.3;
const double TrigT1CaloDefs::Region3Width=0.2;
const double TrigT1CaloDefs::Region4Width=0.3;
const double TrigT1CaloDefs::Region5Width=1.7;

const double TrigT1CaloDefs::Region0Height=M_PI/16;
const double TrigT1CaloDefs::Region1Height=M_PI/16;
const double TrigT1CaloDefs::Region2Height=M_PI/16;
const double TrigT1CaloDefs::Region3Height=M_PI/16;
const double TrigT1CaloDefs::Region4Height=M_PI/16;
const double TrigT1CaloDefs::Region5Height=M_PI/8;

// FCAL (region 5) has different phi granularity for jet and ET triggers
const double TrigT1CaloDefs::Region5JetHeight=M_PI/16;

// geometrical info
const double TrigT1CaloDefs::cpmPhiSize=M_PI/2;
const double TrigT1CaloDefs::cpmEtaSize=0.4;  
const double TrigT1CaloDefs::jemPhiSize=M_PI/2;
const double TrigT1CaloDefs::jemEtaSize=0.8;  //note, end JEMs are larger.


}//end ns;


