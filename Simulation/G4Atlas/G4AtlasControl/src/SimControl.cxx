/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/SimControl.h"

// STL includes
#include <iostream>

// Geant4 includes
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisManager.hh"

// Framework includes
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "PathResolver/PathResolver.h"

// FADS includes
#include "FadsPackageLoader/PackageLoader.h"
#include "FadsXMLParser/XMLReader.h"

SimControl::SimControl()
{
  // std::cout<<" Creating the SimControl "<<std::endl;
}

SimControl::~SimControl()
{
}

void SimControl::G4Command(const std::string& comm) const
{
  G4UImanager *ui=G4UImanager::GetUIpointer();
  ui->ApplyCommand(comm);
}

void SimControl::UIsession() const
{
  std::cout<<" Starting a G4 terminal "<<std::endl;
  G4UIsession * ses = new G4UIterminal(new G4UItcsh);
  ses->SessionStart();

  delete ses;
}

void SimControl::load(const std::string& lib) const
{
  FADS::PackageLoader a(lib.c_str());
}

void SimControl::ReadXML(const std::string& fileName) const
{

  const std::string filename=fileName;
  std::string file=PathResolver::find_file(filename,"DATAPATH");
  if (!file.empty())
    {
      FADS::XMLReader *temp=FADS::XMLReader::GetXMLReader();
      temp->Process(file);
    }
  else std::cout<<"file "<<filename<<" not found"<<std::endl;

}

const MCTruthMenu& SimControl::mcMenu() const
{
  static const MCTruthMenu& temp=mctruthMenu;
  return temp;
}

void SimControl::initializeG4(bool isMT) const
{
  auto rm = G4RunManager::GetRunManager();
  if (rm) {
    rm->Initialize();
    // Initialization differs slightly in multi-threading.
    // TODO: add more details about why this is here.
    if(!isMT && rm->ConfirmBeamOnCondition()) {
      rm->RunInitialization();
    }
  }
  else throw std::runtime_error("Run manager retrieval has failed");
}

void SimControl::initializeGraphics() const
{
  // G4VisManager* visManager=new G4SvcVisManager();
  // visManager->Initialize();
}
