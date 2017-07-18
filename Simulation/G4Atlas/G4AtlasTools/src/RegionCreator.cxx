/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/RegionCreator.h"

#include "GaudiKernel/SystemOfUnits.h"
// Geant4 includes used in functions

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

RegionCreator::RegionCreator(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),m_regionName(name),m_gammaCut(1.*Gaudi::Units::mm),m_electronCut(1.*Gaudi::Units::mm),m_positronCut(1.*Gaudi::Units::mm),
    m_protonCut(1.*Gaudi::Units::mm)
{
  ATH_MSG_INFO( "Region Creator being built: " << name );

  // re-initialize m_regionName in order to take the real tool name rather than the path to it
  size_t ipos=m_regionName.find_last_of(".");
  size_t length=m_regionName.size();
  if (ipos<length) m_regionName=m_regionName.substr(ipos+1,length-ipos-1);
  ATH_MSG_INFO( "m_regionName default value reset to "<<m_regionName);

  declareProperty( "VolumeList" , m_logicalVolumes , "List of volumes to be included in this region" );
  declareProperty( "ElectronCut" , m_electronCut, "Cut to be applied for electrons");
  declareProperty( "PositronCut" , m_positronCut, "Cut to be applied for positrons");
  declareProperty( "GammaCut" , m_gammaCut, "Cut to be applied for gammas");
  declareProperty( "ProtonCut" , m_protonCut, "Cut to be applied for gammas");

  declareProperty( "RegionName" , m_regionName , "Region name (same as the Tool name if not set");


}

// Athena method, called at initialization time
StatusCode RegionCreator::initialize()
{
  ATH_MSG_INFO(" initializing RegionCreator "<<name() );

  //create a new G4Region
  G4Region* theRegion=new G4Region(m_regionName);

  // loop over volumes and fish for those in the list
  int nVolumes = 0;
  G4LogicalVolumeStore* logStore=G4LogicalVolumeStore::GetInstance();
  for (const auto& vol: m_logicalVolumes)
    for (auto* it: *logStore)
      {

        if (vol==it->GetName())
          {
            nVolumes++;
            it->SetRegion(theRegion);
            theRegion->AddRootLogicalVolume(it);
          }
      }

  ATH_MSG_INFO(" a total of "<<nVolumes<<" volumes was assigned to region "<<m_regionName );

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
  ATH_MSG_INFO("    Dump of region  "<<m_regionName);
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

StatusCode
RegionCreator::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if ( riid == IRegionCreator::interfaceID() )
    {
      *ppvIf = (IRegionCreator*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  return AlgTool::queryInterface( riid, ppvIf );
}
