/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/RegionCreator.h"

// Geant4 includes used in functions

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

RegionCreator::RegionCreator(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type,name,parent)
{
}

// Athena method, called at initialization time
StatusCode RegionCreator::initialize()
{
  ATH_MSG_INFO(" initializing RegionCreator "<<name() );
  if (m_regionName.empty()) {
    m_regionName = this->name();
    // re-initialize m_regionName in order to take the real tool name rather than the path to it
    size_t ipos=m_regionName.value().find_last_of(".");
    size_t length=m_regionName.value().size();
    if (ipos<length) {
      ATH_MSG_VERBOSE( "m_regionName: " << m_regionName.value() << " needs to be reset.");
      m_regionName=m_regionName.value().substr(ipos+1,length-ipos-1);
    }
    ATH_MSG_INFO( "m_regionName default value reset to "<<m_regionName.value());
  }

  //create a new G4Region
  G4Region* theRegion=new G4Region(m_regionName.value());

  // loop over volumes and fish for those in the list
  int nVolumes = 0;
  G4LogicalVolumeStore* logStore=G4LogicalVolumeStore::GetInstance();
  for (const auto& vol: m_logicalVolumes)
    for (auto* it: *logStore)
      {

        if (vol == static_cast<const std::string&>(it->GetName()))
          {
            nVolumes++;
            it->SetRegion(theRegion);
            theRegion->AddRootLogicalVolume(it);
          }
      }

  ATH_MSG_INFO(" a total of "<<nVolumes<<" volumes was assigned to region "<<m_regionName.value() );

  // create a G4ProductionCuts object and set appropriate values
  G4ProductionCuts* cuts=new G4ProductionCuts();
  cuts->SetProductionCut(m_gammaCut,"gamma");
  cuts->SetProductionCut(m_electronCut,"e-");
  cuts->SetProductionCut(m_positronCut,"e+");
  cuts->SetProductionCut(m_protonCut,"proton");

  // assign cuts to the region and return succesfully
  theRegion->SetProductionCuts(cuts);

  Dump();

  return StatusCode::SUCCESS;
}

void RegionCreator::Dump()
{
  ATH_MSG_INFO("+----------------------------------------------------+");
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("    Dump of region  "<<m_regionName.value());
  ATH_MSG_INFO("    Volumes being assigned to this region:");
  for (const auto& vol: m_logicalVolumes)
    {
      ATH_MSG_INFO("      "<<vol);
    }
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("    Cuts selected for this region:");
  ATH_MSG_INFO("      Gamma    ="<<m_gammaCut);
  ATH_MSG_INFO("      Electron ="<<m_electronCut);
  ATH_MSG_INFO("      Positron ="<<m_positronCut);
  ATH_MSG_INFO("      Proton   ="<<m_protonCut);
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("+----------------------------------------------------+");
}
