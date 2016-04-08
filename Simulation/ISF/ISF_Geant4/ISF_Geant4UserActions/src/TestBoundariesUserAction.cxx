/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestBoundariesUserAction.h"

#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include <iostream>

#include "TFile.h"
#include "TTree.h"

using namespace iGeant4;

void TestBoundariesUserAction::BeginOfRun(const G4Run* /*aRun*/)
{
  file = TFile::Open("points.root","RECREATE");
  tree = new TTree("points","points");
  //  tree->Branch("data",&data.x,"x:y:z/F:volume/I:enter/O:exit:leave");
  tree->Branch("x",&data.x,"x/F");
  tree->Branch("y",&data.y,"y/F");
  tree->Branch("z",&data.z,"z/F");
  tree->Branch("volume",&data.volume,"volume/I");
  tree->Branch("enter",&data.enter,"enter/O");
  tree->Branch("exit",&data.exit,"exit/O");
  tree->Branch("leave",&data.leave,"leave/O");
  return;
}

void TestBoundariesUserAction::EndOfRun(const G4Run* /*aRun*/)
{
  tree->Write();
  file->Close();
  delete file;
  file=0;
  tree=0;
}

TestBoundariesUserAction::TestBoundariesUserAction(const std::string& type, const std::string& name, const IInterface* parent):
  UserActionBase(type,name,parent),
  file(0), tree(0)
{
  sel["Atlas::Atlas"]=0;
  sel["IDET::IDET"]=1;
  sel["CALO::CALO"]=2;
  sel["MUONQ02::MUONQ02"]=3;
  sel["BeamPipe::BeamPipe"]=4;

}


void TestBoundariesUserAction::Step(const G4Step* aStep)
{
  G4StepPoint * preStep = aStep->GetPreStepPoint();

  G4StepPoint *postStep = aStep->GetPostStepPoint();

  if (!preStep || !postStep) {
    ATH_MSG_ERROR("TestBoundariesUserAction Error something missing");
    return;
  }

  G4TouchableHistory* preTH = (G4TouchableHistory*)(preStep->GetTouchable());
  G4TouchableHistory* postTH = (G4TouchableHistory*)(postStep->GetTouchable());


  /*
  if (preTH){
    int preN=preTH->GetHistoryDepth();
    for (int i=0; i<preN; ++i) {
      std::cout<<" "<<preTH->GetVolume(i)->GetLogicalVolume()->GetName();
    }
    std::cout<<"\n";
  }
  else std::cout<<" no preTH\n";
  if (postTH) {
    int postN=postTH->GetHistoryDepth();
    for (int i=0; i<postN; ++i) {
      std::cout<<" "<<postTH->GetVolume(i)->GetLogicalVolume()->GetName();
    }
    std::cout<<"\n";
  }
  else std::cout<<" no postTH\n";
*/
  G4LogicalVolume*    preLV=preStep->GetPhysicalVolume()->GetLogicalVolume();
  if (preTH) {
    int preN=preTH->GetHistoryDepth();
    if (preN>0) preLV= preTH->GetVolume(preN-1)->GetLogicalVolume();
  }


  G4LogicalVolume*    postLV=0;
  if (postStep->GetPhysicalVolume())
    postLV=postStep->GetPhysicalVolume()->GetLogicalVolume();
  if (postTH) {
    int postN=postTH->GetHistoryDepth();
    if (postN>0) postLV=  postTH->GetVolume(postN-1)->GetLogicalVolume();
  }

  if (postLV!=preLV) {
    // bool startedInVolume=false;
    //    bool exitedVolume=false;
    //bool leftWorld=false;
    G4ThreeVector pos=aStep->GetPostStepPoint()->GetPosition();
    //    std::cout<<"pos    :"<<pos<<"\n";
    data.Set(pos.x(),pos.y(),pos.z(),0);
    if (preLV) {
      std::string preStepPointName=preLV->GetName();
      //      std::cout<<"preLV  :"<<preStepPointName<<"\n";
      SMap::const_iterator it=sel.find(preStepPointName);
      if (it!=sel.end()) {
        //startedInVolume=true;
        data.volume=it->second;
        data.Reset();
        data.exit=true;
        if (tree) tree->Fill();
      }
    }
    if (postLV) {
      std::string postStepPointName=postLV->GetName();
      //      std::cout<<"postLV :"<<postStepPointName<<"\n";
      SMap::const_iterator it=sel.find(postStepPointName);
      if (it!=sel.end()) {
        //startedInVolume=true;
        data.volume=it->second;
        data.Reset();
        data.enter=true;
        if (tree) tree->Fill();
      }
    }
    else {
      //      std::cout<<"postLV :   ---\n";
      //leftWorld=true;
        data.volume=0;
        data.Reset();
        data.leave=true;
        if (tree) tree->Fill();
    }

  }

}

StatusCode TestBoundariesUserAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
