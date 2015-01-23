/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTools/UserActionBase.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "G4VUserPhysicsList.hh"
#include "G4RunManager.hh"
#include "G4EmProcessOptions.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

UserActionBase::UserActionBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
{
//  declareInterface<IUserAction>(this);
//  declareProperty("PhysicsList"	    , m_physicsListName = "FTFP_BERT", "Name for physics list");
//  declareProperty("NeutronTimeCut"  , m_neutronTimeCut = 0, "Time cut for neutron killer");
//  declareProperty("NeutronEnergyCut", m_neutronEnergyCut = 0, "Energy cut for neutron killer");
//  declareProperty("GeneralCut"	    , m_generalCut = 0, "General cut");
//  declareProperty("EMMaxEnergy"	    , m_emMaxEnergy = 7*CLHEP::TeV, "Maximum energy for EM tables");
//  declareProperty("EMMinEnergy"	    , m_emMinEnergy = 100*CLHEP::eV, "Minimum energy for EM tables");
//  declareProperty("EMDEDXBinning"   , m_emDEDXBinning = 77, "Binning for EM dE/dX tables");
//  declareProperty("EMLambdaBinning" , m_emLambdaBinning = 77, "Binning for EM Lambda tables");
//  declareProperty("ApplyEMCuts"	    , m_applyEMCuts = true, "Apply cuts EM flag in Geant4");
}

//G4AtlasUA::Priority UserActionBase::Priority(G4AtlasUA::Role theRole){
//
//  auto role=m_priorities.find(theRole);
//
//  if(role==m_priorities.end()){
//    ATH_MSG_ERROR("Request for priority for role "<<theRole<<" which is not defined for this action");
//    return G4AtlasUA::Priority::None;
//  }
//
//  return (*role).second;
//
//}
//
//void  UserActionBase::SetPriority(G4AtlasUA::Priority pri, G4AtlasUA::Role theRole){
//
//  auto role=m_priorities.find(theRole);
//  
//  if(role==m_priorities.end()){
//    ATH_MSG_ERROR("Attempt to set priority for role "<<theRole<<" which is not defined for this action");
//  }
//
//  (*role).second=pri;
//
//
//}
//
//
void UserActionBase::AddRole(G4AtlasUA::Role theRole){


  //  m_priorities[theRole]=priority;
  m_roles.push_back(theRole);

}
