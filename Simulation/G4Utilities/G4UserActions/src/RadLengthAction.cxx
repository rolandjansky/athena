/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RadLengthAction.h"
#include "SimHelpers/ServiceAccessor.h"
#include "MCTruth/TrackHelper.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <TTree.h>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"

namespace G4UA
{

  RadLengthAction::RadLengthAction(const Config& config)
    : m_config(config)
    , m_SDMDT(nullptr)
    , m_SDTGC(nullptr)
    , m_SDCSC(nullptr)
    , m_SDRPC(nullptr)
  {
  }


  // BeginOfRunAction used for initialization of the most crucial maps,
  // registering trees and reading depth level from jobOptions
  void RadLengthAction::BeginOfRunAction(const G4Run*)
  {
    // getting level of depth from jobOptions
    // depth = atof( theProperties["VolumeDepthLevel"].c_str() );

    // creating reference to sensitive detectors of muonsystem
    // later on used for comparison in Stepping
    G4SDManager *SDM=G4SDManager::GetSDMpointer();
    m_SDMDT = (G4VSensitiveDetector*) SDM->FindSensitiveDetector("AMS");
    m_SDTGC = (G4VSensitiveDetector*) SDM->FindSensitiveDetector("TMS");
    m_SDCSC = (G4VSensitiveDetector*) SDM->FindSensitiveDetector("CMS");
    m_SDRPC = (G4VSensitiveDetector*) SDM->FindSensitiveDetector("RMS");


    // clearing the maps with comparison volumes, trees and variables
    topvolmap.clear();
    treeMap.clear();
    variables.clear();

    // creating volume store
    G4LogicalVolumeStore *store = G4LogicalVolumeStore::GetInstance();
    // getting most top volume "Atlas::Atlas" by hand
    G4LogicalVolume* atlas = store->GetVolume("Atlas::Atlas");

    // logical volume vector, initialized with atlas only
    // later on filled with all logicals of depth level
    std::vector<G4LogicalVolume*> logvec;
    logvec.push_back(atlas);

    // physical volume vector later on used for topvolmap and names
    std::vector<G4VPhysicalVolume*> physvec;

    // counter for loop up to depth level
    int counter = 0;

    // loop to fill physvec with all physical volumes of depth level
    while(counter != m_config.VolumeDepthLevel){
      // counter increase up to depth
      counter++;

      // create tmplogvec to be filled with (logical) daughters of counter depth level
      std::vector<G4LogicalVolume*> tmplogvec;

      // iterator for logical
      std::vector<G4LogicalVolume*>::iterator logit;

      // looping on logical vector of previous depth level
      for(logit=logvec.begin() ; logit < logvec.end(); logit++ ){
        for(int k=0; k<(*logit)->GetNoDaughters();k++){
          // dumping all logical daughters for next level
          tmplogvec.push_back((*logit)->GetDaughter(k)->GetLogicalVolume());
          // dumping the corresponding physical if not made of gas or last depth level where all are dumped
          if(((*logit)->GetDaughter(k)->GetLogicalVolume()->GetMaterial()->GetRadlen()<100*CLHEP::cm) || (counter == m_config.VolumeDepthLevel)){
            physvec.push_back((*logit)->GetDaughter(k));
          }
        }
      }

      // logical vector is put to the tmplogvec (with daughters) for next stage
      // to physvec all not gas daughters of this level where already added
      logvec=tmplogvec;

    }

    // loop on physvec to put the volumes in the topvolmap using their name
    // for comparison in Stepping and initializing all other maps
    std::vector<G4VPhysicalVolume*>::iterator physit;
    for( physit=physvec.begin() ; physit < physvec.end(); physit++ ){
      std::string fulldaughtername = (*physit)->GetName();
      std::string daughtername;
      std::string::size_type npos;
      npos=fulldaughtername.find("::");
      // nicer naming for depth level =1
      if(npos!=std::string::npos && m_config.VolumeDepthLevel==1) daughtername = fulldaughtername.substr(0,npos);
      else daughtername = fulldaughtername;
      topvolmap[daughtername]= (*physit);
    }

    // adding by hand two additional names to topvolmap (without volume)
    // due to initialization purpose of topvolmap
    topvolmap["ToMuChamber"] = 0;
    topvolmap["ToMuTrigger"] = 0;

    // using topvolmap to initialize treemap (trees with volumenames)
    // and variables map of vectors with FIXED SIZE TO NUMBER OF VARIABLES TO BE DUMPED
    std::map<std::string,G4VPhysicalVolume*>::iterator volit;
    for(volit=topvolmap.begin(); volit!=topvolmap.end(); volit++){
      treeMap[(*volit).first]=new TTree(TString((*volit).first), TString((*volit).first));
      variables[(*volit).first].resize(12);
    }

    // procedure to get the THistoSvc
    StatusCode status;

    static ITHistSvc* hSvc=0;

    if(!hSvc){
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      status = svcLocator->service("THistSvc", hSvc, true);
      if( status.isFailure() ){
        return;
      }
    }

    // using already initialized treeMap to register the trees with volname
    // and branches which are REFERENCED to the components of the corresponding
    // entry in variables map
    std::map<std::string,TTree*>::iterator it;
    for(it=treeMap.begin(); it!=treeMap.end(); it++){
      std::string filename= "/RadLengthAction/";
      std::string treepath= filename+(*it).first;
      if (hSvc) hSvc->regTree(treepath.c_str(), treeMap[(*it).first]).ignore();
      //if (!hSvc) log()<< MSG::ERROR << "Cannot register Tree!" << (*it).first << endreq;
      treeMap[(*it).first]->Branch("EnergyLoss",  &variables[(*it).first].at(0), "EnergyLoss/D");
      treeMap[(*it).first]->Branch("RadLength",   &variables[(*it).first].at(1),  "RadLength/D");
      treeMap[(*it).first]->Branch("Intlength",   &variables[(*it).first].at(2),  "Intlength/D");
      treeMap[(*it).first]->Branch("InitialEta",  &variables[(*it).first].at(3), "InitialEta/D");
      treeMap[(*it).first]->Branch("InitialPhi",  &variables[(*it).first].at(4), "InitialPhi/D");
      treeMap[(*it).first]->Branch("PointEta",    &variables[(*it).first].at(5),   "PointEta/D");
      treeMap[(*it).first]->Branch("PointPhi",    &variables[(*it).first].at(6),   "PointPhi/D");
      treeMap[(*it).first]->Branch("PointX",      &variables[(*it).first].at(7),     "PointX/D");
      treeMap[(*it).first]->Branch("PointY",      &variables[(*it).first].at(8),     "PointY/D");
      treeMap[(*it).first]->Branch("PointZ",      &variables[(*it).first].at(9),     "PointZ/D");
      treeMap[(*it).first]->Branch("PointR",     &variables[(*it).first].at(10),     "PointR/D");
      treeMap[(*it).first]->Branch("Charge",     &variables[(*it).first].at(11),     "Charge/D");
    }
  }

  // empty EndOfRunAction
  void RadLengthAction::EndOfRunAction(const G4Run*)
  {

  }

  // BeginOfEventAction used to get variables of primary particle and reinitialization
  void RadLengthAction::BeginOfEventAction(const G4Event* anEvent)
  {

    // getting variables of the primary particle
    G4PrimaryVertex *vert=anEvent->GetPrimaryVertex(0);
    //     vert->SetPosition(0.0,0.0,0.0);
    G4PrimaryParticle *part=vert->GetPrimary();
    G4ThreeVector mom=part->GetMomentum();
    etaPrimary=mom.eta();
    phiPrimary=mom.phi();
    chargePrimary=part->GetCharge();
    G4cout<<"Begin Of Event"<<" "<<(double)vert->GetX0()<<" "<<(double)vert->GetY0()<<" "<<(double)vert->GetZ0()<< G4endl;
    G4cout<<"Begin Of Event"<<" "<<(double)mom[0]<<" "<<(double)mom[1]<<" "<<(double)mom[2]<< G4endl;

    // putting passflags for muon chambers/trigger (back) to false
    MuChamberPassed = false;
    MuTriggerPassed = false;

    // reinitialize the variables vector for this event
    std::map<std::string,std::vector<double> >::iterator it;
    for(it=variables.begin(); it!=variables.end(); it++){
      for(unsigned int i=0; i< variables[(*it).first].size(); i++) variables[(*it).first].at(i)=0;
    }
  }

  // EndOfEvent used for filling all trees (branches are already refering to variables)
  void RadLengthAction::EndOfEventAction(const G4Event*)
  {

    //for the case there was never a chamber passed set eloss to 1 radlength/intlength to -1
    if(!MuChamberPassed){
      variables["ToMuChamber"].at(0) =  1.;
      variables["ToMuChamber"].at(1) = -1.;
      variables["ToMuChamber"].at(2) = -1.;
    }

    //for the case there was never a trigger passed set eloss to 1 radlength/intlength to -1
    if(!MuChamberPassed){
      variables["ToMuTrigger"].at(0) =  1.;
      variables["ToMuTrigger"].at(1) = -1.;
      variables["ToMuTrigger"].at(2) = -1.;
    }


    std::map<std::string,TTree*>::iterator it;
    for(it=treeMap.begin(); it!=treeMap.end(); it++){
      treeMap[(*it).first]->Fill();
    }
  }

  // Stepping to get the variables and make volume association
  void RadLengthAction::UserSteppingAction(const G4Step* aStep)
  {

    // killing secondaries to save computing time, perhaps
    if(TrackHelper(aStep->GetTrack()).IsSecondary()) aStep->GetTrack()->SetTrackStatus(fStopAndKill);

    // entering primary particle
    if(TrackHelper(aStep->GetTrack()).IsPrimary()) {

      // get touchable history since used often
      G4TouchableHistory* touchHist =(G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();

      // get point before Stepping was started
      G4ThreeVector xyz = (G4ThreeVector) aStep->GetPreStepPoint()->GetPosition();

      // get radlength and intlength of volumes passed with this Stepping
      // double radl=touchHist->GetVolume()->GetLogicalVolume()->GetMaterial()->GetRadlen();
      // double intl=touchHist->GetVolume()->GetLogicalVolume()->GetMaterial()->GetNuclearInterLength();
      double radl=aStep->GetPreStepPoint()->GetMaterial()->GetRadlen();
      double intl=aStep->GetPreStepPoint()->GetMaterial()->GetNuclearInterLength();

      // get the steplength for calculation X0 and intlength
      double stepl=aStep->GetStepLength();
      G4cout<<aStep->GetPreStepPoint()->GetMaterial()->GetName()<<" "<<radl<<" "<<stepl<<" "<<stepl/radl<<" "<<(double) xyz[0]<<" "<<(double) xyz[1]<<" "<< (double) xyz[2]<<G4endl;
      // define and fill vector for the FIXED NUMBER of variables which should be dumped later on
      const unsigned int nVariablesToSave(12);
      std::vector<double> varvec(nVariablesToSave);
      varvec.at(0)  = (double) aStep->GetDeltaEnergy();
      varvec.at(1)  = (double) stepl/radl;
      varvec.at(2)  = (double) stepl/intl;
      varvec.at(3)  = (double) etaPrimary;
      varvec.at(4)  = (double) phiPrimary;
      varvec.at(5)  = (double) xyz.pseudoRapidity();
      varvec.at(6)  = (double) xyz.phi();
      varvec.at(7)  = (double) xyz[0];
      varvec.at(8)  = (double) xyz[1];
      varvec.at(9)  = (double) xyz[2];
      varvec.at(10) = (double) sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]);
      varvec.at(11) = (double) chargePrimary;

      // loop on topvolmap to search for mother of current volume on volume depth level
      // dump the variables in corresponding component of variables map
      std::map<std::string,G4VPhysicalVolume*>::iterator it;
      for(it=topvolmap.begin(); it!=topvolmap.end(); it++){
        if((*it).second == touchHist->GetVolume(touchHist->GetHistoryDepth()-m_config.VolumeDepthLevel) ){
          this->fillVariables(varvec, (*it).first);
        }
      }

      // get sensitive detector of current volume
      G4VSensitiveDetector* SD = (G4VSensitiveDetector*) touchHist->GetVolume()->GetLogicalVolume()->GetSensitiveDetector();

      // set muon chamber pass flag true up to end of event and dump
      // variables collected up to this point in variables map
      if(SD==m_SDCSC || SD==m_SDMDT) MuChamberPassed = true;
      if(!MuChamberPassed) this->fillVariables(varvec, "ToMuChamber");

      // set muon trigger pass flag true up to end of event and dump
      // variables collected up to this point in variables map
      if(SD==m_SDRPC || SD==m_SDTGC) MuTriggerPassed = true;
      if(!MuTriggerPassed) this->fillVariables(varvec, "ToMuTrigger");

    }//end primary particle
  }

  // how variables should be dumped in variables map
  void RadLengthAction::fillVariables(std::vector<double> varvec, const std::string& name){
    // first three components should be added (deltaenergy, radlength, intlength)
    for(unsigned int i = 0; i<3; i++) variables[name].at(i) += varvec.at(i);
    // other components should be overwritten
    // (last value inside the volume will be kept by change and dumped in EndOfEvent)
    for(unsigned int j = 3; j<12; j++) variables[name].at(j) = varvec.at(j);
  }
} //namespace G4UA
