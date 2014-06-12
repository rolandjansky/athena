/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************************
// Randomize calo cell energy according to CaloNoiseTool
//
// D. Varouchas 
//
// ****************************************************************************************

#include "CaloCellCorrection/CaloCellRandomizer.h"

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"


#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
//#include "CaloUtils/ICaloNoiseTool.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include <CLHEP/Random/Randomize.h>

#include "GaudiKernel/MsgStream.h"


using CLHEP::RandGauss;


// ======================================================
// Constructor

CaloCellRandomizer::CaloCellRandomizer(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_noiseTool("undefined"),  m_corrSampleMin(0),m_corrSampleMax(0),m_fractionSigma(1),m_GaussRand(false),m_GaussRandGroupedSamples(false),m_GaussRandShifted(false),m_GaussRandShifted_Custom(false), m_shift_EMB(1),m_shift_EMEC(1),m_shift_HEC(1),m_shift_TileBar(1),m_shift_TileGap(1),m_shift_TileExt(1),m_shift_FCAL(1)

{
 declareInterface<CaloCellCorrection>(this);
 declareProperty("noiseTool",m_noiseTool,"Tool Handle for noise tool");
 // declareProperty("SampleNums",m_sampleNums);
 declareProperty("DoRandomSampleMin",m_corrSampleMin);
 declareProperty("DoRandomSampleMax",m_corrSampleMax);
 declareProperty("RandomiseGroupedSampleCells",m_GaussRandGroupedSamples);
 declareProperty("DoGaussRandomization",m_GaussRand);
 declareProperty("DoShiftedGaussRandomization",m_GaussRandShifted);
 declareProperty("FractionOfSigmaNoiseInShifting",m_fractionSigma);

 declareProperty("DoShiftedGaussRandomizationCustomisedInCalo",m_GaussRandShifted_Custom);
 declareProperty("CustomShiftEMB",m_shift_EMB);
 declareProperty("CustomShiftEMEC",m_shift_EMEC);
 declareProperty("CustomShiftHEC",m_shift_HEC);
 declareProperty("CustomShiftTileBar",m_shift_TileBar);
 declareProperty("CustomShiftTileGap",m_shift_TileGap);
 declareProperty("CustomShiftTileExt",m_shift_TileExt);
 declareProperty("CustomShiftFCAL",m_shift_FCAL);

}

//========================================================
// Initialize

StatusCode CaloCellRandomizer::initialize()
{
 MsgStream  log(msgSvc(),name());
 log << MSG::INFO << " in CaloCellRandomizer::initialize() " << endreq;

  

  if(m_noiseTool.retrieve().isFailure()){
    log << MSG::INFO
	<< "Unable to find tool for CaloNoiseTool"
	<< endreq;
    return StatusCode::FAILURE;
  }  
  else {
    log << MSG::INFO << "Noise Tool retrieved" << endreq;
  }
  
  log << MSG::INFO << "CaloCellRandomizer initialize() end" << endreq;

  
  return StatusCode::SUCCESS;

}


// ============================================================================

void CaloCellRandomizer::MakeCorrection(CaloCell* theCell)
{
  
  int sampl = 0;
  float Gauss = 0;
  float GaussShifted = 0;

  const CaloDetDescrElement* caloDDE = theCell->caloDDE();
  sampl=caloDDE->getSampling();    
   
  //CaloNoiseTool
  double SigmaNoise = m_noiseTool->getNoise(theCell,ICalorimeterNoiseTool::ELECTRONICNOISE);
  

  Gauss = RandGauss::shoot(0.,1.);

  GaussShifted = Gauss+m_fractionSigma;


  MsgStream  log(msgSvc(),name());
  
    
    
  if( m_GaussRand  ){
    setenergy(theCell,(Gauss*SigmaNoise));
  }
  
    
  if(m_GaussRandShifted){
    //    setenergy(theCell,(GaussShifted)); 
    setenergy(theCell,(GaussShifted*SigmaNoise)); 
  }
  
  
  if(m_GaussRandShifted_Custom){//var 
    if (sampl<4){ 
      GaussShifted = RandGauss::shoot(m_shift_EMB*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    }
    if (sampl>3 && sampl<8){ 
      GaussShifted = RandGauss::shoot(m_shift_EMEC*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    }
    if (sampl>7 && sampl<12){ 
      GaussShifted = RandGauss::shoot(m_shift_HEC*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    } 
    if (sampl>11 && sampl<15){ 
      GaussShifted = RandGauss::shoot(m_shift_TileBar*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    } 
    if (sampl>14 && sampl<18){ 
      GaussShifted = RandGauss::shoot(m_shift_TileGap*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    }  
    if (sampl>17 && sampl<21){
      GaussShifted = RandGauss::shoot(m_shift_TileExt*SigmaNoise,SigmaNoise);
      setenergy(theCell,(GaussShifted));
    }
    if (sampl>20 && sampl<24){
      GaussShifted = RandGauss::shoot(m_shift_FCAL*SigmaNoise,SigmaNoise);
      setenergy(theCell,(GaussShifted));
    } 
    
  }
  
  
  if(m_GaussRandGroupedSamples){
    if(sampl>m_corrSampleMin && sampl<m_corrSampleMax){
      setenergy(theCell,(Gauss*SigmaNoise));
    }
  }
  
  
  //check if cells were masked before from tile reco alg, so to remove them, Tile Case only
  //  const TileCell* aTileCell = dynamic_cast<const TileCell*> (theCell);
  //if CaloNoiseTool big values means we have bad cells (for tile when badcell is not applicalble). get ride of these cells
  if ( (sampl>11 && sampl<21) && SigmaNoise>999){
    setenergy(theCell,0);
  }
  
  
}
