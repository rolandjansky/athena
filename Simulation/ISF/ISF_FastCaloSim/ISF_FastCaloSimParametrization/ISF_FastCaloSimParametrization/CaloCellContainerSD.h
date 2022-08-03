/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMPARAMETRIZATION_CALOCELLCONTAINERSD_H
#define ISF_FASTCALOSIMPARAMETRIZATION_CALOCELLCONTAINERSD_H

/* Base class header */
#include "G4VSensitiveDetector.hh"
/* Athena WriteHandle include */
#include "StoreGate/WriteHandle.h"
/* Empty cell builder interface include */
#include "CaloInterface/ICaloCellMakerTool.h"
/* Fast hit converter include to take into account sampling fractions */
#include "FastCaloSimHit/FastHitConvertTool.h"
/* CaloCellContainer include */
#include "CaloEvent/CaloCellContainer.h"


class G4TouchableHistory;
class TFCSSimulationState;

class CaloCellContainerSD : public G4VSensitiveDetector
{
public:
  CaloCellContainerSD(const std::string& name, const std::string& CaloCellContainerName, PublicToolHandle<FastHitConvertTool> FastHitConvertTool);
  ~CaloCellContainerSD() {}

  // Start of *ATHENA* event
  void StartOfAthenaEvent(const EventContext& ctx);

  // End of *ATHENA* event
  void EndOfAthenaEvent(const EventContext& ctx);

  // Needs to be implemented, but is not used for this SD
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  
  // Method to record the cells from a TFCSSimulationState
  void recordCells(TFCSSimulationState&);

protected:
  // The empty cell builder tool to initialize an empty calo cell container
  PublicToolHandle<ICaloCellMakerTool> m_EmptyCellBuilderTool;
  // The write handle to the CaloCellContainer
  SG::WriteHandle<CaloCellContainer> m_caloCellContainer;
  // The empty cell builder tool to initialize an empty calo cell container
  PublicToolHandle<FastHitConvertTool> m_FastHitConvertTool;


};

#endif // ISF_FASTCALOSIMPARAMETRIZATION_CALOCELLCONTAINERSD_H



