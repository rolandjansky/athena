/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUtils/JetDistances.h"

#include "JetMomentTools/JetLArHVTool.h"
#ifdef ASGTOOL_ATHENA
#include "JetUtils/JetCellAccessor.h"
#endif

JetLArHVTool::JetLArHVTool(const std::string& name)
  : JetModifierBase(name),
    m_useCells(true),    
#ifdef ASGTOOL_ATHENA  
    m_hvCorrTool("LArHVCorrTool")
#endif
{
  declareProperty("UseCells", m_useCells=true);
  declareProperty("HVCorrTool",m_hvCorrTool);
#ifdef ASGTOOL_ATHENA  
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr="LArHVScaleCorr");
#endif
}


StatusCode JetLArHVTool::initialize()
{
// Retrieve HVCorrTool
  StatusCode sc = m_hvCorrTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Unable to find tool for LArHVCorrTool");
    return StatusCode::FAILURE;
  }
  
  
  return StatusCode::SUCCESS;

}


int JetLArHVTool::modifyJet( xAOD::Jet& jet ) const 
{
  double energy=0;
  double energyHVaff=0;
  int numCells=0;
  int numCellsHVaff=0;
  
  double eFrac = 0;
  double nFrac = 0;
  
  

  if( m_useCells){
#ifdef ASGTOOL_ATHENA
    jet::JetCellAccessor::const_iterator it =  jet::JetCellAccessor::begin(&jet);
    jet::JetCellAccessor::const_iterator itE = jet::JetCellAccessor::end(&jet);
    for( ;it!=itE; it++) {
      //e += it->e()*it.weight();
      const CaloCell* thisCell = *it;
      Identifier cellID=thisCell->ID();
      numCells++;
      energy+=fabs(thisCell->e());
      
      //keep only the LAr cells in the jet:
      if(thisCell->caloDDE()->is_tile())continue;
      
      //float hvdev = 0;
      
      //retrieve offline correction from DB:
      float hvcorr = m_hvCorrTool->Scale(cellID);
      
      //           //retrieve online correction from MetaData (available only in raw data)
      //           float hvonline = m_dd_HVScaleCorr->HVScaleCorr(cellID);
      
      //Correction should be between (0 and 2)
      if (!(hvcorr>0. && hvcorr<100.)) continue;
         
      
      //check if the difference between correction and 1 is greater than 2 per thousand, then we are in a LAr HV affected area.
      if(fabs(hvcorr)-1.>0.002){
        ATH_MSG_DEBUG ( "HV affected" << hvcorr );
        energyHVaff+=fabs(thisCell->e());
        numCellsHVaff++;
      }//end of non nominal HV area
      
#endif
    }//cells
    
    if(energy>0) eFrac = energyHVaff/energy;
    if(numCells>0) nFrac = double(numCellsHVaff)/double(numCells);

  } // use cells
  

  
  // set the attributes
  jet.setAttribute<float>("LArBadHVEnergyFrac", eFrac);
  jet.setAttribute<float>("LArBadHVNCellFrac", nFrac);
  
  
  return 0;
}


