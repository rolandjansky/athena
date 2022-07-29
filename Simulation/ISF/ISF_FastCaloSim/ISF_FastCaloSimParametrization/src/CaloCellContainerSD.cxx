/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "ISF_FastCaloSimParametrization/CaloCellContainerSD.h"

// Athena headers
#include "MCTruth/TrackHelper.h"

// FastCaloSim simulation include
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"


CaloCellContainerSD::CaloCellContainerSD(const std::string& name, const std::string& CaloCellContainerName, PublicToolHandle<FastHitConvertTool> FastHitConvertTool)
  : G4VSensitiveDetector( name ),
    m_EmptyCellBuilderTool("EmptyCellBuilderTool/EmptyCellBuilderTool"),
    m_caloCellContainer (CaloCellContainerName),
    m_FastHitConvertTool (FastHitConvertTool)
{
  if(m_EmptyCellBuilderTool.retrieve().isFailure()) {
    G4Exception("CaloCellContainerSD", "FailedEmptyCellBuilderToolRetrieval", FatalException, "CaloCellContainerSD: Failed to retrieve the empty cell builder tool.");
    abort();
  }
}


void CaloCellContainerSD::StartOfAthenaEvent(const EventContext& ctx){

  if (!m_caloCellContainer.isValid()) m_caloCellContainer = std::make_unique<CaloCellContainer>(SG::VIEW_ELEMENTS);

  //Initialize cell container with empty cells
  if(m_EmptyCellBuilderTool->process(&*m_caloCellContainer, ctx).isFailure()){
    G4Exception("CaloCellContainerSD", "FailedEmptyCellBuilderToolProcess", FatalException, "CaloCellContainerSD: Failed to process calo cell container with the empty cell builder tool.");
    abort();
  }

  return;
}

void CaloCellContainerSD::EndOfAthenaEvent(const EventContext& ctx){
  
  // Update the calo iterators of the calo cell container
  m_caloCellContainer->updateCaloIterators();

  // Convert FastCaloSim hits into HitCollections, taking into account sampling fractions
  if(m_FastHitConvertTool->process(&*m_caloCellContainer, ctx).isFailure()){
    G4Exception("CaloCellContainerSD", "FailedFastHitConvertToolProcess", FatalException, "CaloCellContainerSD: Failed to process calo cell container with the fast hit convert tool.");
    abort();
  }

  return;
}

G4bool CaloCellContainerSD::ProcessHits(G4Step*, G4TouchableHistory* ){
  // This method needs to be implemented when deriving from G4VSensitiveDetector has no use in this case
  G4Exception("CaloCellContainerSD", "UndefinedProcessHitsCall", FatalException, "CaloCellContainerSD: Call to undefined ProcessHits.");
  abort();
  return true;
}


void CaloCellContainerSD::recordCells(TFCSSimulationState& simState)
{
  // Add the energies from the simulation state to the CaloCellContainer
  for(const auto& icell : simState.cells()) {
    CaloCell* caloCell = (CaloCell*)m_caloCellContainer->findCell(icell.first->calo_hash());
    caloCell->addEnergy(icell.second); 
  }

}
