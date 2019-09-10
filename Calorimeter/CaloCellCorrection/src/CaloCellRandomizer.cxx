/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************************
// Randomize calo cell energy according to CaloNoise
//
// D. Varouchas 
//
// ****************************************************************************************

#include "CaloCellRandomizer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaKernel/RNGWrapper.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "CLHEP/Random/Randomize.h"

using CLHEP::RandGauss;


// ======================================================
// Constructor

CaloCellRandomizer::CaloCellRandomizer(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_corrSampleMin(0),m_corrSampleMax(0),m_fractionSigma(1),m_GaussRand(false),m_GaussRandGroupedSamples(false),m_GaussRandShifted(false),m_GaussRandShifted_Custom(false), m_shift_EMB(1),m_shift_EMEC(1),m_shift_HEC(1),m_shift_TileBar(1),m_shift_TileGap(1),m_shift_TileExt(1),m_shift_FCAL(1),
    m_athRNGSvc ("AthRNGSvc", name),
    m_randomEngine (nullptr)
{
 declareInterface<CaloCellCorrection>(this);
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
 declareProperty("AthRNGSvc", m_athRNGSvc);
 declareProperty("RandomStream", m_randomStream = "CaloCellRandomizer");
}

//========================================================
// Initialize

StatusCode CaloCellRandomizer::initialize()
{
  ATH_MSG_INFO( " in CaloCellRandomizer::initialize() "  );
  ATH_CHECK( m_caloNoiseKey.initialize() );
  ATH_MSG_INFO( "Noise Tool retrieved"  );
  ATH_MSG_INFO( "CaloCellRandomizer initialize() end"  );
  ATH_CHECK( m_athRNGSvc.retrieve() );
  m_randomEngine = m_athRNGSvc->getEngine (this, m_randomStream);
  if (!m_randomEngine) {
    ATH_MSG_ERROR("Could not get random number engine from AthRNGSvc. Abort.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


// ============================================================================

void CaloCellRandomizer::MakeCorrection (CaloCell* theCell,
                                         const EventContext& ctx) const
{
  CLHEP::HepRandomEngine* engine = m_randomEngine->getEngine (ctx);

  int sampl = 0;
  float Gauss = 0;
  float GaussShifted = 0;

  const CaloDetDescrElement* caloDDE = theCell->caloDDE();
  sampl=caloDDE->getSampling();    
   
  //CaloNoise
  SG::ReadCondHandle<CaloNoise> caloNoise (m_caloNoiseKey, ctx);
  double SigmaNoise = caloNoise->getNoise(theCell->ID(), theCell->gain());
  

  Gauss = RandGauss::shoot(engine, 0.,1.);

  GaussShifted = Gauss+m_fractionSigma;


  if( m_GaussRand  ){
    setenergy(theCell,(Gauss*SigmaNoise));
  }
  
    
  if(m_GaussRandShifted){
    //    setenergy(theCell,(GaussShifted)); 
    setenergy(theCell,(GaussShifted*SigmaNoise)); 
  }
  
  
  if(m_GaussRandShifted_Custom){//var 
    if (sampl<4){ 
      GaussShifted = RandGauss::shoot(engine, m_shift_EMB*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    }
    if (sampl>3 && sampl<8){ 
      GaussShifted = RandGauss::shoot(engine, m_shift_EMEC*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    }
    if (sampl>7 && sampl<12){ 
      GaussShifted = RandGauss::shoot(engine, m_shift_HEC*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    } 
    if (sampl>11 && sampl<15){ 
      GaussShifted = RandGauss::shoot(engine, m_shift_TileBar*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    } 
    if (sampl>14 && sampl<18){ 
      GaussShifted = RandGauss::shoot(engine, m_shift_TileGap*SigmaNoise,SigmaNoise); 
      setenergy(theCell,(GaussShifted)); 
    }  
    if (sampl>17 && sampl<21){
      GaussShifted = RandGauss::shoot(engine, m_shift_TileExt*SigmaNoise,SigmaNoise);
      setenergy(theCell,(GaussShifted));
    }
    if (sampl>20 && sampl<24){
      GaussShifted = RandGauss::shoot(engine, m_shift_FCAL*SigmaNoise,SigmaNoise);
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
  //if CaloNoise big values means we have bad cells (for tile when badcell is not applicalble). get ride of these cells
  if ( (sampl>11 && sampl<21) && SigmaNoise>999){
    setenergy(theCell,0);
  }
  
  
}
