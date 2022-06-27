/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TestBoundariesUserAction.h"

#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"

#include "TFile.h"
#include "TTree.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

  TestBoundariesUserAction::TestBoundariesUserAction()
    : AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),
                   "TestBoundariesUserAction")
  {
  }

  void TestBoundariesUserAction::BeginOfRunAction(const G4Run*)
  {
    m_file = TFile::Open("points.root","RECREATE");
    ATH_MSG_INFO("Open file points.root, create tree");
    m_file->cd();
    m_tree = new TTree("points","points");
    m_tree->Branch("x",&m_data.x,"x/F");
    m_tree->Branch("y",&m_data.y,"y/F");
    m_tree->Branch("z",&m_data.z,"z/F");
    m_tree->Branch("volume",&m_data.volume,"volume/I");
    m_tree->Branch("enter",&m_data.enter,"enter/O");
    m_tree->Branch("exit",&m_data.exit,"exit/O");
    m_tree->Branch("leave",&m_data.leave,"leave/O");
  }

  void TestBoundariesUserAction::EndOfRunAction(const G4Run*)
  {
    ATH_MSG_INFO("Writing file points.root");
    m_file->cd();
    m_tree->Write();
    m_file->Close();
    delete m_file;
    m_file=nullptr;
    m_tree=nullptr;
  }

  void TestBoundariesUserAction::UserSteppingAction(const G4Step* aStep)
  {
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    G4StepPoint* postStep = aStep->GetPostStepPoint();

    if (!preStep || !postStep) {
      ATH_MSG_ERROR("TestBoundariesUserAction Error something missing");
      return;
    }

    const G4TouchableHistory* preTH = dynamic_cast<const G4TouchableHistory*>(preStep->GetTouchable());
    const G4TouchableHistory* postTH = dynamic_cast<const G4TouchableHistory*>(postStep->GetTouchable());
    G4LogicalVolume* preLV = preStep->GetPhysicalVolume()->GetLogicalVolume();

    if (preTH) {
      int preN=preTH->GetHistoryDepth();
      if (preN>0) preLV = preTH->GetVolume(preN-1)->GetLogicalVolume();
    }

    G4LogicalVolume* postLV{};
    if (postStep->GetPhysicalVolume())
      postLV = postStep->GetPhysicalVolume()->GetLogicalVolume();
    if (postTH) {
      int postN = postTH->GetHistoryDepth();
      if (postN>0) postLV = postTH->GetVolume(postN-1)->GetLogicalVolume();
    }

    if (postLV!=preLV) {
      const G4ThreeVector& pos = aStep->GetPostStepPoint()->GetPosition();
      m_data.Set(pos.x(),pos.y(),pos.z(),0);
      if (preLV) {
        const std::string& preStepPointName = preLV->GetName();
        SMap::const_iterator it = m_sel.find(preStepPointName);
        if (it!=m_sel.end()) {
          m_data.volume = it->second;
          m_data.Reset();
          m_data.exit = true;
          if (m_tree) m_tree->Fill();
        }
      }
      if (postLV) {
        SMap::const_iterator it = m_sel.find( postLV->GetName() );
        if (it!=m_sel.end()) {
          m_data.volume = it->second;
          m_data.Reset();
          m_data.enter = true;
          if (m_tree) m_tree->Fill();
        }
      }
      else {
        m_data.volume = 0;
        m_data.Reset();
        m_data.leave = true;
        if (m_tree) m_tree->Fill();
      }
    }
  }

} // namespace G4UA
