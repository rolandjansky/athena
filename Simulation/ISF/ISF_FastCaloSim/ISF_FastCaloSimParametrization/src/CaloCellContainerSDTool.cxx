/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Header include
#include "CaloCellContainerSDTool.h"

// CaloCellContainer sensitive detector
#include "ISF_FastCaloSimParametrization/CaloCellContainerSD.h"

CaloCellContainerSDTool::CaloCellContainerSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent ),
    m_FastHitConvertTool ("FastHitConvertTool/FastHitConvertTool")
{
   declareProperty ("FastHitConvertTool", m_FastHitConvertTool, "FastHitConvertTool");
}

StatusCode CaloCellContainerSDTool::SetupEvent(){
  
  // Get current event context
  const EventContext& ctx = Gaudi::Hive::currentContext();

  if( !getSD() ){
    ATH_MSG_ERROR ("SetupEvent: CaloCellContainerSD never created!");
    return StatusCode::FAILURE;
  }
  else{
    CaloCellContainerSD *localSD = dynamic_cast<CaloCellContainerSD*>(getSD());
    if(!localSD){
      ATH_MSG_ERROR ("SetupEvent: Failed to cast m_SD into CaloCellContainerSD.");
      return StatusCode::FAILURE;
    }
    localSD->StartOfAthenaEvent(ctx);
  }

  return StatusCode::SUCCESS;

}

StatusCode CaloCellContainerSDTool::Gather(){

  // Get current event context
  const EventContext& ctx = Gaudi::Hive::currentContext();

  if( !getSD() ){
    ATH_MSG_ERROR ("Gather: CaloCellContainerSD never created!");
    return StatusCode::FAILURE;
  }
  else{
    CaloCellContainerSD *localSD = dynamic_cast<CaloCellContainerSD*>(getSD());
    if(!localSD){
      ATH_MSG_ERROR ("Gather: Failed to cast m_SD into CaloCellContainerSD.");
      return StatusCode::FAILURE;
    }
    localSD->EndOfAthenaEvent(ctx);
  }

  return StatusCode::SUCCESS;


}

G4VSensitiveDetector* CaloCellContainerSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Initializing SD" );

  // Create a fresh SD
  return new CaloCellContainerSD(name(), m_outputCollectionNames[0], m_FastHitConvertTool);
}
