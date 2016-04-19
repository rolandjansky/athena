/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/SimControl.h"

// Geant4 includes
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisManager.hh"

#include <iostream>

#include "FadsPackageLoader/PackageLoader.h"
#include "FadsXMLParser/XMLReader.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "G4AtlasControl/DataCardSvc.h"


SimControl::SimControl()
{
  // std::cout<<" Creating the SimControl "<<std::endl;
  dCard=DataCardSvc::GetDataCardSvc();
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

const PhysicsMenu& SimControl::physMenu() const
{
  static const PhysicsMenu& temp=physicsMenu;
  return temp;
}

const MCTruthMenu& SimControl::mcMenu() const
{
  static const MCTruthMenu& temp=mctruthMenu;
  return temp;
}

const FieldMenu& SimControl::fieldMenu() const
{
  static const FieldMenu& temp=fldMenu;
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

void SimControl::DefineCard(const std::string name, const std::string type, int defValue)
{
  std::cout<<" calling the int version of DefineCard "<<std::endl;
  dCard->DefineCard(name,type,defValue);
}
void SimControl::DefineCard(const std::string name, const std::string type, double defValue)
{
  std::cout<<" calling the double version of DefineCard "<<std::endl;
  dCard->DefineCard(name,type,defValue);
}
void SimControl::DefineCard(const std::string name, const std::string type, const std::string defValue)
{
  std::cout<<" calling the string version of DefineCard "<<std::endl;
  dCard->DefineCard(name,type,defValue);
}
void SimControl::SetCard(const std::string name, int Value)
{
  std::cout<<" calling the int version of SetCard "<<std::endl;
  dCard->SetCard(name,Value);
}
void SimControl::SetCard(const std::string name, double Value)
{
  std::cout<<" calling the double version of SetCard "<<std::endl;
  dCard->SetCard(name,Value);
}
void SimControl::SetCard(const std::string name, const std::string Value)
{
  std::cout<<" calling the string version of SetCard "<<std::endl;
  dCard->SetCard(name,Value);
}

const FieldIntegrationMenu& SimControl::fieldIntegrationMenu() const
{
  static const FieldIntegrationMenu& temp=fldIntMenu;
  return temp;
}
