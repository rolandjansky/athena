/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowCellEOverPTool_mc12_LC.h"
#include "eflowRec/eflowCaloRegions.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <vector>
#include <iomanip>
#include <sstream>

//This file was generated using mhodgkin/EOverP-00-00-19 with the following datasets
//user.mhodgkin.mc12_8TeV.159038.e1189_s1620_s1622_r3586.17289_Tag98_LC.D3PD/
//user.mhodgkin.mc12_8TeV.159039.e1189_s1620_s1622_r3586.17289_Tag98_LC.D3PD/

eflowCellEOverPTool_mc12_LC::eflowCellEOverPTool_mc12_LC(const std::string& type,const std::string& name,const IInterface* parent) : IEFlowCellEOverPTool( type, name, parent), m_nEBins(4),m_nEtaBins(25),m_nFirstIntLayerBins(eflowFirstIntRegions::nRegions),m_nCaloRegionBins(eflowCalo::nRegions){

  declareInterface<IEFlowCellEOverPTool>(this);

  m_eBinValues.push_back(  1.0*Gaudi::Units::GeV );
  m_eBinValues.push_back( 3.5*Gaudi::Units::GeV );
  m_eBinValues.push_back( 10.0*Gaudi::Units::GeV );
  m_eBinValues.push_back( 25.0*Gaudi::Units::GeV );
  m_eBinValues.push_back( 32.5*Gaudi::Units::GeV );
  m_eBinValues.push_back( 40.0*Gaudi::Units::GeV );

  for (double x = 0.0; x < 2.0; x+=0.5) {
    m_etaBinBounds.push_back(x);
  }
  m_etaBinBounds.push_back(2.5);

}

StatusCode eflowCellEOverPTool_mc12_LC::initialize(){

  return StatusCode::SUCCESS;
}

StatusCode eflowCellEOverPTool_mc12_LC::execute(eflowBinnedParameters *binnedParameters){

  if (binnedParameters) {
    
    binnedParameters->initialise(m_eBinValues, m_etaBinBounds);

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta050bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.615716);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.353373);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.06098);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0160033);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,2.37744);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0277764);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.09892);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0162711);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.480753);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0190265);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0303416);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0222188);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0145306);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0263455);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0082407);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0788267);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,4.29621e-07);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0788478);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.000568103);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0242752);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000360458);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0785401);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.930356);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.412114);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.518865);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0175863);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.394321);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0283331);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.31923);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0156922);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.28178e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0308555);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.10266);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0218247);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,5.72739e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0323018);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0169384);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0310144);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.012157);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0417798);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.861591);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.623821);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.592738);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0199045);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.417764);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.032433);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.759565);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0124581);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.376494);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0150642);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.773879);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0180875);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.373907);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0218418);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.90205);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0300931);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,5.78842e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0535708);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0331749);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0309894);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0217198);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0467137);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00124795);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0347692);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00104166);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.092843);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.965726);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.537367);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.13277);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0235987);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,3.40806e-05);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.371597);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.32066);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0100243);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.134461);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0178085);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0362696);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.00815468);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0283787);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0180896);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.17126);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0258802);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.870472);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0402784);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0748235);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0318949);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0530323);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0408757);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00388826);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0315574);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00310977);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0980356);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta100bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.599754);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.353416);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.12692);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0156147);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.61407);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0272394);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.748514);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.015971);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.381453);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0182717);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0303512);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0266895);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,2.39444e-13);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0419825);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0100981);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0376172);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00707566);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0543873);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.945996);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.420136);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.386319);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0268861);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,3.89687e-15);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0503452);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.92271);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0200306);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,5.70714e-11);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0353448);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0836839);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0235623);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0301817);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0289696);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0161155);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0342174);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,2.83942e-15);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0549841);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.000777714);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0451741);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00046545);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0526428);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.824209);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.565509);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.428814);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0184615);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.30754);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0329277);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.605411);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0132348);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.312279);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0159914);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.870953);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0166484);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.50023);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0215274);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.385254);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.031761);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.254505);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0482197);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0113664);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0327394);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00752431);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0562692);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.935545);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.286262);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.785907);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0334704);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,1.96971e-11);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0538733);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0605629);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0365116);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.60091e-13);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0560682);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00156105);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0566915);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.000960145);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0827565);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta150bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.537455);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.450428);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.60952);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0248244);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.02776e-10);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0434094);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.722107);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0155303);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.314623);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0180757);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0355323);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0257211);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,2.94794e-13);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.041865);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00780866);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0343958);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00557636);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0418507);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.93779);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.412382);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.375887);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.018839);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.277986);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0278324);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.84884);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0116488);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,2.89822);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0140322);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.112562);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0196018);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0619814);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0236366);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.00827729);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0356326);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00581237);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0482156);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.864898);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.652592);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.66843);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.017526);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.491119);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0318648);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.781977);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0175633);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,1.13722e-07);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.022925);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.108454);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0528403);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0517505);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0618855);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00285045);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0615602);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00149364);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0687786);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::HEC, 1.3625);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.408312);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::Tile, 1.08858);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.506322);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.170747);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0336119);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.117718);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0442539);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.038985);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0351149);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0334157);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.041078);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0068967);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0401161);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0052383);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.070047);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E001bin, eta250bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 1.2584);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 1.11091);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0181609);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0568642);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.00599179);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.088092);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM1,5741.51);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH1,0.0013288);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM2,1.67519);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH2,0.0605338);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00244919);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.190122);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.01589e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,93.8917);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.892224);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.410305);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,5.10696);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0159825);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,3.88204);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0234544);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.128675);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0146156);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.0827511);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0188825);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.051324);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0313851);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.0340343);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0468985);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.00931997);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.188127);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,5.40525e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,95.5939);


   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME2, 1.28467);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.489742);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.962559);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0163174);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.777578);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0227625);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.427959);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0133559);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.321394);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0164303);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.136895);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0290839);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.000734793);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,99.9984);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.0024155);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.188478);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,1.00463e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,6.22102);


   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME3, 1.31806);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.72772);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,1.70234);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.024669);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.30017);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0348849);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.0207885);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.226);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,4.80574e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.226016);


   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::HEC, 1.69233);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.922583);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::Tile, 1.1121);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::Tile, 0.420247);


    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta050bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.68732);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.402982);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.41181);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0175419);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.74315);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0444421);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.796147);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0137496);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.434573);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0439349);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0361338);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0448266);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.000592347);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0455492);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0965683);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0581426);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0686391);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0668367);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.023435);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0524642);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0175425);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0633346);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00419318);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0482768);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00313633);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0566034);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.865524);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.318413);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.417777);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0178001);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.276959);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0387199);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.69929);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0105815);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,2.37287);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0165817);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.117836);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0186326);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.049642);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0305891);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0531311);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0544699);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0193211);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.067161);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00353593);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0415529);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00214056);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0747061);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 1.17084);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.567853);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.257086);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0163519);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.18398);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0345495);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,1.0957);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0124493);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00745725);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0489592);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.672059);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0108684);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.446481);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0254128);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.981668);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0359071);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.624829);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0485217);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0651555);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0392743);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0390598);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0486185);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0049321);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0453993);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00408705);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0578801);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.997665);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.362305);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.261164);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0437505);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.54447e-10);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0621133);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.693211);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0118702);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0260327);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0644385);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0390629);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0132885);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0218947);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0274178);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.26388);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0394257);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0248696);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0888735);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.200657);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0480069);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0138182);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0495815);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0281184);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0427927);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0193545);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0519114);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta100bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.713529);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.416729);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.64826);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0164648);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.12656);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0423131);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.798368);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0165006);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.180192);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0493674);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0386499);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0102912);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0252433);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0523389);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0702374);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0700184);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0200844);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.074908);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0049364);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0586709);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00190893);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0629295);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.000552782);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0646943);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000358419);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0734786);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.892478);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.321648);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.31551);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.016081);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.227195);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0325327);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.06261);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0157414);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0030763);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0717473);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.102857);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.024861);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0298638);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0365236);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.043177);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0477085);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.022131);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.054736);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00321038);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.054976);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00126121);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0620513);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 1.15191);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.541361);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.234856);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0207373);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.151028);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0437463);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.958527);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0158725);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00802957);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0550534);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.851545);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0110415);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.599824);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0235519);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.584814);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0446022);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,1.61391e-15);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0614738);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0321556);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0415968);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0184903);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0491136);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00201274);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0450907);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00153075);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.064228);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 1.08626);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.512128);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.131283);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0231584);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0667264);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.052562);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.490758);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0141136);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0035687);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.106464);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0530975);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0117652);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0236376);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.034269);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.982975);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0438253);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.00136008);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.171942);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0474075);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0406763);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.027724);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0489927);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00924028);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.035661);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00756934);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0945333);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta150bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.637671);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.501279);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.25461);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0212165);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.745383);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0488329);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.791731);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0152727);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.153723);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0557119);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0458471);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.00718078);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0334607);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0480669);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.102922);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0563321);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,4.70026e-06);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0571791);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00173074);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.435843);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,5.95172e-10);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,56.023);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0301576);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0651874);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0107149);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0689127);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0035309);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0732908);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00111753);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0781215);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.000574758);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0797746);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000403562);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.091759);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.877709);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.372428);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.309301);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0172418);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.21863);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0349809);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.3049);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.00877621);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,2.68589);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0165278);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.119356);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0300593);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,3.09477e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.145872);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,22.8681);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.00647568);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,3.82213e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0272007);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0186286);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0485946);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0102181);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0686892);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00220963);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0471077);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00117022);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0526638);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.959615);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.357716);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.306578);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0159097);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.224594);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.034082);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.857294);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0133881);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00451925);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0808446);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,1.0929);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.021808);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.000543355);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.105614);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.173639);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0417409);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,5.17735e-08);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0549237);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,193899);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.00386099);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.00416617);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,2.8889);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,3.04396e+09);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0079152);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,4.73912);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0401498);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.171548);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0372019);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0826779);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0754935);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00746863);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0443385);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00436437);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.076532);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.000535977);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.125529);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.000126705);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.138744);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 1.18405);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.594718);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 1.04953);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.435854);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0580595);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0166496);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0412807);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0390872);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.362377);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0122358);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.01774);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0389829);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0865131);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0211858);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00169424);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0777716);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.027166);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.050061);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,7.78836e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,99.9964);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00123962);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.295312);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,6.38001e-07);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,11.8147);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.343853);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.043455);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.000576426);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.112559);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0453351);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0423855);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,9.3217e-13);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0612553);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00736451);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0393637);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00631243);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0479082);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E003point5bin, eta250bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.665497);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.360557);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.000733302);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.103481);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,9.33788e-08);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.103491);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.00949112);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0130347);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,1.0644e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,99.9974);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00427405);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.193062);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,1.7061e-06);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,93.6275);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,898404);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.00665414);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,1.74738);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.0267006);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,2.90444e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.292714);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.51303e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.300032);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.778051);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.369936);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,7.30318);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0164116);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,5.24189);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0342835);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,4.24281);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0135065);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,1.30587);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0421319);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.100968);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.011564);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.0737417);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0429887);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.131568);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0666931);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.0496803);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0724352);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.0650002);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0646601);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.0365214);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0730681);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.00490291);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0821524);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.00210012);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0949172);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.000478667);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.179857);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,5.23762e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.200282);


   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 1.10536);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.43858);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.951995);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0151204);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.705684);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0294564);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,25.5269);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0149014);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.0591496);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.080954);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.779604);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0209819);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.210466);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0493923);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.259806);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0439082);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.111203);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0794785);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.10891);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0654741);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.0560807);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0712718);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.00884953);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0696698);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.00441428);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0790138);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0046292);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.088839);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.000633379);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.145668);


   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 1.20702);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.583604);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.481296);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0397617);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,1.71138e-10);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0575587);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,8.07766);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0135607);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.121171);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0573422);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,10.5244);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0214897);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.0199308);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0792495);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.41901);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0293016);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.65399);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0534982);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.209182);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0392577);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.128248);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0503647);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.00918094);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0378537);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.00361169);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0946717);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.000503454);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.45);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,6.46975e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,96.3893);


   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 1.45006);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.710327);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.29944);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0465104);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,2.07216e-08);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0497705);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.864748);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0537375);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,5.40156e-10);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0642408);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.455782);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0114313);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.258542);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0497544);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,3.66089);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0299253);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.14854);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0710726);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.09351);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.042533);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,1.95493);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0744277);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.364266);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0686662);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.125367);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0730041);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.122196);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0498022);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0466202);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.082625);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0729235);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0408852);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.042243);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0518128);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.021429);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0421946);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0118251);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0545725);


   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.989782);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.369308);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0434887);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0126562);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,5.1567e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,99.9815);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0025555);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.295879);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,1.28576e-06);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,22.8548);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0274952);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0310179);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0212151);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.039353);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00201076);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0556793);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00154995);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.068765);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta050bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.930423);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.130633);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.62495);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0263886);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.205416);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0586753);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.72479);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0248349);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0532678);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0557939);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.162166);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0241992);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.056903);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0427756);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.786433);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0246952);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.509642);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.049746);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.125389);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0246228);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.103832);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0433177);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0169625);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.03892);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0109261);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.049687);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.969544);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.132519);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.45132);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0232751);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.123661);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0466066);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.73037);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0172032);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0360911);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0457756);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.258421);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.01883);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0753037);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0358744);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.506331);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0455072);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,4.411e-12);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0552564);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0694436);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0420793);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,1.12739e-14);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0593406);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.0121662);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0437027);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,1.33322e-13);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0607318);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 1.10446);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.556274);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.20975);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0246922);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0576055);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0531572);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.855146);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0150791);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0138205);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0502538);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.569261);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0154928);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0457635);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0369066);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.853338);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0277298);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.624822);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0412904);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.123832);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0279856);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0910064);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0395674);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0156781);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0377885);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0106717);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0469043);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::Tile, 1.12026);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.613446);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.160497);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0202363);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0958907);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0431087);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.458385);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0173342);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0135667);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0480373);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0944035);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0130118);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0463414);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0289671);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.07051);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0226456);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.832796);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0352046);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.1988);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0242888);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.145711);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0358917);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0244804);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0366388);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0159533);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0475587);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta100bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.934619);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.148942);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.77388);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0219379);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.939372);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0438739);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.54435);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0227748);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.12975);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0476959);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.09803);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0237055);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.055301);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0453277);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.370027);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0504644);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,4.82714e-10);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0581398);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0422428);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0291847);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0276144);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0514475);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0026833);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0468808);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00191147);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0550557);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.97842);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.138585);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.296061);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0218817);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.184439);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0426207);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.30343);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0168642);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0502393);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0434024);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.193026);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0213932);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0547782);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0409179);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.292794);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0453206);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.131174);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.052215);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0337421);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0412219);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0164447);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0478727);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00346321);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0445373);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00238543);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0542267);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 1.07966);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.481844);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.133997);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0245054);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0655496);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.053485);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.601648);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0163025);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0312018);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0447857);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.55637);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0172078);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0513873);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0391571);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.568498);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0320685);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.415397);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0460174);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.061037);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.027763);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0483649);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0448148);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00440331);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0412196);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00303855);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0563787);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::Tile, 1.10536);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.408897);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0592485);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.047381);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,9.02188e-10);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0625622);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.288852);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.011605);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0447119);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0345694);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.099443);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0138468);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00932899);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0409216);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.900243);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0237766);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.684992);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0387691);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.101887);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0390697);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,8.5496e-12);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0578566);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00584635);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0436819);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00427098);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.056323);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta150bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.898774);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.174793);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.65235);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0220426);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.805124);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.044009);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.03603);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0254821);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.131568);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0487704);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0734003);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.041235);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0194375);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.044507);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0728773);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0520295);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.0261941);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0580768);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0103398);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0692194);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00114361);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0736946);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0867705);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0581217);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0177956);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0620882);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00874083);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0587233);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0046137);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.066049);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00114362);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0752661);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000196083);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0802817);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.94032);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.16779);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.435367);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.024183);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.213114);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0441587);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.33181);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.019223);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.048778);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.04833);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.234917);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0233131);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0611716);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0413773);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0184824);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0585225);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.015842);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0678855);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0347597);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0618206);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,7.6491e-07);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.065682);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.126388);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0553907);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0247995);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0592844);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00684492);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0660981);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00282198);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0748245);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.932316);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.205282);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.13676);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0218283);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.101372);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0533412);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.713761);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0227052);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0163985);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0610856);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.832989);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0131055);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.43367);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0248105);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.207566);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0446581);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,3.30671e-09);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0606906);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0317669);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0159727);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.0124773);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0833399);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,0.112429);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0454844);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,8.66653e-07);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,98.243);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM1,69736.9);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH1,0.0109994);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM2,44863.1);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH2,0.0199089);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.451646);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0307233);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.297301);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0501346);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0302414);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0519675);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0137801);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0578993);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00314778);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0599849);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00121713);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0659498);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME1, 1.075);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::HEC, 1.15133);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.933706);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.270459);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.044632);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,1.24882e-08);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0598703);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.0143703);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0403338);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,8.69704e-13);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0544178);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0960298);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0557324);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0605895);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0647557);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0570647);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0567347);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0301033);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0674958);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,5.55314);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0258961);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,1.88145e-05);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,52.7254);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.343223);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0548579);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,3.62938e-06);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,96.4685);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0260697);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.047526);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0160487);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0606379);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::Tile, 1.13214);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.621935);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.173324);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0438344);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,6.42256e-11);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0577477);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.289315);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0205897);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0757552);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0438993);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0830517);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0161186);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0322077);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0353418);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.241245);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0474251);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.126591);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0551835);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0750281);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0538812);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0448987);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0652472);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.662726);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.03371);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,8.10855e-08);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0528289);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,340.428);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0134574);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,296.192);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0270075);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.514538);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.028123);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.282512);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0514669);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0871306);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0306554);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0513121);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0530328);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00942197);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0333749);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0065211);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0643752);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E020bin, eta250bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.735634);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.328122);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.604173);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 1.50581);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.888813);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.107809);




   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.913003);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.216053);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,6.73264);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0231453);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.95106);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0503034);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,9.74881);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0244988);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,1.90919);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0480927);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.577915);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.044901);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,9.47666e-13);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0602421);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.19332);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0221989);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.966597);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0584936);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.853229);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0249959);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.808322);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0568222);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.107943);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0519);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0270059);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0608595);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0365004);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0468403);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,1.16361e-16);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0623386);


   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.99207);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.210223);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.28344);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.017668);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.942382);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0455568);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,23.6356);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0183256);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.87615);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0531996);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.95902);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0245233);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.297112);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0527628);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.35221);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0376648);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.750059);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0653213);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,1.03268);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0240497);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.776477);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0631683);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0911888);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0619057);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.00026649);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.167851);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0234927);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0614079);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,9.72335e-07);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,99.9713);


   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME3, 1.24373);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.842039);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.475903);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0521161);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.140876);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0565239);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,4.07519);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0170586);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,1.18199);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0456251);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,7.63044);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0196418);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.322543);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0522083);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,3.71514);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0352512);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.33667);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.055049);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.41296);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0489695);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.667708);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0639668);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.126941);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.053374);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0565155);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0619754);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0258204);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0487148);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.015332);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0581898);


   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::HEC, 1.20496);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.827331);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0285945);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.05323);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00767624);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0573275);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.380806);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0520623);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.140573);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0566786);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.990777);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0457673);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.00172194);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0634802);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.417237);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0183545);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.15846);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0536516);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,3.58075);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0226498);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.73153);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0481705);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,4.07081);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0244369);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,3.07057);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0513152);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.400341);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0345244);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.23956);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0581194);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.152035);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0476558);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0595869);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0527536);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.127262);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0560018);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0616688);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0662686);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0135239);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0575559);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00708512);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0696615);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.887311);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.281742);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0640233);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0426383);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0533927);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0491652);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.220189);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0441709);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.132757);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0533945);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0302997);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0443065);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.000272942);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,99.9509);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,513277);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.00666919);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,1.02471);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0265412);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0766073);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0609284);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0415029);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0668571);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00698454);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0654053);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00319206);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0730924);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta050bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.95362);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.110796);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.71367);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0248688);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.123271);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0627097);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,3.845);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0205863);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0472318);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0580239);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.176902);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0226392);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0413314);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0447275);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.670338);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0263638);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.368442);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0509952);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.122478);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0241563);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0914903);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.039266);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0172195);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0360709);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0113328);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0470675);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.987656);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.101326);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.933672);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.02496);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0444951);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0584645);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.16733);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0165569);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0366048);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0472284);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.262084);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0193657);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0411036);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0411808);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.717363);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0244566);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.547701);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0450962);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0748163);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0412551);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,7.29912e-10);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0417593);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.011818);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0376842);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00751386);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.048445);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 1.09444);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.584625);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.161793);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0217781);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0711954);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0469267);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,1.08084);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0127673);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0266297);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0433924);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.596231);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0136354);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0256716);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0402526);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.822239);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0258487);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.612742);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0423836);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.134355);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0251287);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.101257);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0415115);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0176007);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0372026);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0114453);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0475821);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 1.1024);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.488395);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.109365);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0394134);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,2.98867e-11);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0593824);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.363499);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0130947);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0176935);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0415918);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0675689);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0149547);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0176004);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0329631);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.06543);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0202391);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.807101);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0344263);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.198697);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0335818);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,4.24909e-05);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.164996);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0301512);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0343352);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0206117);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0466454);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta100bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.955667);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.116735);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.82479);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0234537);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.526086);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0463529);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.55466);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0214113);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0726799);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0523888);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.158039);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0227321);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0493259);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.046149);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.364569);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0254632);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.257807);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.054802);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.043738);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0245198);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0370072);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0473731);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00299643);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0435811);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00210119);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.059324);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.991469);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.111096);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.560465);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0244123);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0642271);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0525817);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.15738);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0159057);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0330378);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0485479);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.218886);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0224733);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0291419);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.046729);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.355429);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0279022);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.260021);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0526236);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0366047);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.027317);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0248665);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0504834);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00266298);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0457743);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00176377);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0593183);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 1.10873);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.695662);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.14834);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0451252);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.04828e-10);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0625584);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.793691);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0156603);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0279601);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.046528);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.547848);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0167001);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0264565);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0440011);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.57006);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0321901);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.382888);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0485887);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0631081);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0275341);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0462735);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0439236);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00355124);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0420765);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00252499);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0585857);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 1.1117);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.519818);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0683837);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0222778);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.039357);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0482726);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.211248);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0162897);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0109885);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0457281);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0602212);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0176177);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0100356);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0384654);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.920927);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0224874);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.671275);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.034375);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.135193);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0250098);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0983164);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0384517);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00800971);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0419485);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00518093);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.051268);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta150bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.92826);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.137803);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.14321);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0247777);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.398216);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0486291);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.48959);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0224917);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0844543);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0513914);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0805758);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0221198);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0509862);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0452464);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0799972);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0412217);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,1.45021e-12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0599691);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00326859);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.079865);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00171334);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.100561);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0839738);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0607645);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,3.97002e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0649561);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00892423);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0615569);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00289531);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0674943);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00141008);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0667343);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00064919);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0736506);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.961811);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.129027);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.444826);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0223732);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.240837);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0438059);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.96061);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0158154);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0807328);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0464527);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.205241);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0206373);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0678665);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0401179);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.00119531);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0813387);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.00102455);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0981249);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0162071);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0560894);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.00996831);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0739795);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.10866);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0580441);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0150626);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0613677);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00759389);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0620787);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00417323);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0705012);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.0011861);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0762287);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,5.31414e-06);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0783212);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.958725);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.164076);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.14251);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0466394);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.67934e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0584832);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.615114);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0190775);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0500291);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0455495);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.785844);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0171199);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0440813);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0406939);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.136715);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0374818);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0990231);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0599077);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0941238);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0533779);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,1.11599e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0603413);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.437043);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0261504);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.262433);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0499515);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0283895);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0512538);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0167274);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0585594);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00258596);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0581881);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00130861);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.066106);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.875);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 1.2973);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 1.20989);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.189302);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0470393);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,6.82895e-11);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0582253);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.59223);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0167997);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,0.134061);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0436102);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.00875951);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0583162);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,3.97845e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0647077);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0946559);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0549322);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0561654);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0633895);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0479789);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0492297);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0411248);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0599917);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.0198762);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0674893);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,2.14754e-05);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,67.1168);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.402783);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0277182);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.220193);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0567459);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0358908);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0494448);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,1.45194e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0629282);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 1.1858);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.791972);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.107334);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0499646);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,2.69242e-08);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0516357);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.407565);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0158571);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0819849);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0459033);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0710297);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0177092);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0101151);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0466501);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.251366);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0501161);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.118455);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.057871);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0453869);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0533813);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0292789);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0635681);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.0363758);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0685775);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,2.13593e-07);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0689862);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,1.14852);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0455132);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,2.50639e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0694619);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.559902);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.032935);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0799574);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.062729);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0873149);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0323866);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0248792);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0563956);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00859924);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0349115);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00645271);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0613398);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E032point5bin, eta250bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.906244);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.245435);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.825104);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.147243);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.986983);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.149034);




   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.942902);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.167844);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,7.23271);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0216713);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,2.46286);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0458662);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,12.5367);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.024251);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.852424);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0558123);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.620286);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0251942);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.34988);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0523133);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.46825);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.022563);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,1.2585);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.056252);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,1.29823);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0225622);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,1.09239);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0545071);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.121158);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0467039);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0610624);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0573152);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0529089);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0290586);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0501242);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0534828);


   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 1.14241);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.816685);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.27462);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0220627);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.893748);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0501072);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,20.568);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.019701);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.631783);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0559535);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.90636);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.021303);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.680354);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0440952);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.72263);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0351538);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.958884);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0610176);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,1.30375);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0327846);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.821089);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0605291);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.116655);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0315471);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.102882);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0618928);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0350388);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0449778);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0185409);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0640637);


   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 1.32149);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 1.04384);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.613502);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0507833);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,6.01059e-10);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0636481);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,4.33507);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0180511);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,1.17189);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0470196);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,7.25677);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0200424);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.171484);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0595919);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,3.86601);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0309318);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.69239);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0522582);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.85737);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0416495);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,1.0831);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0580366);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.152262);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0334273);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.128116);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0608491);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0486088);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.031429);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.045534);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0550496);


   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 1.24354);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.938591);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0397662);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0509349);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0180299);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0551752);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.33085);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0364248);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,4.43897e-13);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0556976);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.415061);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0471662);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,1.48137e-13);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0630371);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,1.28732);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0111572);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.570569);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0474333);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.421935);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0184181);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.139307);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0481254);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,3.04313);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0242147);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.3046);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.045368);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.90913);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0263028);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.95201);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0471762);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.436542);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0276084);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.354908);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.051069);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.109037);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0423976);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0632409);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.053522);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0993356);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0510074);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0595645);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0578297);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0185491);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0361472);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0104691);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0617878);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.980266);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.146353);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0575846);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0473686);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0405233);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0526275);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.236412);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0448006);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.116798);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0549612);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0415034);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0361016);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0276849);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0470363);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.144728);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0353892);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,2.14635e-05);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0465042);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.114994);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0587841);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0257048);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0676063);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0202462);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0378439);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00612726);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0678138);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta050bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.964722);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.0874931);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.23913);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0249958);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.0589057);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0726548);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.198436);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.020154);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0534993);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0377154);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.55427);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0304502);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.153816);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0567016);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0982394);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0333634);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00381175);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0671839);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00669085);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0409343);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0044074);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0490394);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.988988);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.083377);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.666498);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0191296);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.032386);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.053668);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.330296);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0187934);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0258689);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0421122);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.569735);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0258211);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.327702);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0479483);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0740614);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0398339);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,2.02551e-13);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0592405);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.0108399);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0455391);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,5.07324e-13);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0605866);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 1.07445);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.43357);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.191952);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0174499);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.103852);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0384264);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.933303);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0213916);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.69694);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0413069);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.129403);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0241172);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0986252);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0388811);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0180663);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0361053);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0130961);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0461839);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::Tile, 1.08529);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.283013);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.127559);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0332216);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,4.42338e-07);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,99.9988);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.18104);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0213424);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.920381);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0363301);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.18165);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0256053);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.138468);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0384938);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0171098);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0358148);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0119643);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0449382);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta100bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.967024);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.0977904);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.81787);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0250573);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.161434);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0584214);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.80282);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.018758);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0728242);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0511621);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.17634);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0220408);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.03138);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0506855);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.31031);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0523658);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.68217e-13);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0526232);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0386294);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0474123);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,3.0365e-10);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0605376);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00363558);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0394446);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00260706);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.057602);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.993042);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.0939035);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.36175);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0251887);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0980402);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0468842);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.31772);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0136034);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0569418);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0414164);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.206816);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0217343);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0240508);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0472625);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.335819);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0470554);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,1.09376e-11);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0579961);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0342421);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0266647);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0221383);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0508299);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00313163);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0474226);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00193225);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.055219);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 1.07041);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.567318);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.137582);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.042647);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.22994e-12);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0603123);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.876558);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0135482);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0329028);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0424319);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.780405);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0142691);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0277611);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0420634);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.610715);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0273054);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.367918);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0469862);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0476567);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0251442);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0307375);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0476953);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00367662);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0414772);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00248088);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0591978);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::Tile, 1.09335);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.515721);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0682661);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0427932);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.68959e-09);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0576955);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.212185);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0124508);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0251962);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0368494);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.102078);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0157068);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00447335);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0472153);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.870715);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0240686);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.684739);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0374183);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.13402);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0237224);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.100488);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0394671);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00838521);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0427848);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00536909);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0552126);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta150bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.941411);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.108213);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.2299);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.02468);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.235558);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0524064);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.12547);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0203653);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0722203);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0541435);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.120191);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0217806);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0413828);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0484108);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0661072);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0322039);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.0598461);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0641355);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0315843);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0517686);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00807741);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0578997);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM1,33.4997);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH1,0.0314037);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM2,0.0108745);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH2,0.0601384);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.1229);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0571654);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,4.8569e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0644418);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0125708);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0653474);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,1.48403e-07);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0665038);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00140642);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0729484);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000229064);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0775201);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.961799);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.110371);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.476125);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.024546);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.10534);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0533831);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,6.90819);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0148691);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0747179);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0475138);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.175325);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0195747);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0538071);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0432842);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.152613);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0523822);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,3.83325e-11);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0614666);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0213664);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0597482);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.0107405);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0715062);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM1,0.290565);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH1,0.0319345);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM2,1.52247e-10);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH2,0.0557866);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.116175);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0589042);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,7.86034e-10);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0642577);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00869825);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0677963);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00162689);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0724354);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00083685);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0798065);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,1.23037e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0803378);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.966128);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.149547);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.111853);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0184034);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0796285);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0511115);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,1.01989);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0143803);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0542872);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0447593);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.751075);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0152511);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0326622);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0425347);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.134925);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0457379);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0848385);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0546824);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0132674);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0612271);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.011372);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0787868);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,0.830645);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0335433);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,1.00066e-06);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0355883);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.511108);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0293477);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.121433);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0570882);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0267709);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0328277);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0240652);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0598163);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00327601);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0592451);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0017696);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.067342);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.925);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.0555729);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.875);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::HEC, 1.3151);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::HEC, 1.25031);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.155448);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0426334);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,5.8578e-11);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0611974);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.563553);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0176012);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,0.132259);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0424358);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.0207278);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0462791);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,1.95958e-11);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0592526);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.165936);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.035575);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0745936);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0631903);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0373038);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0533996);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0210799);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0660642);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.574909);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0322541);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.00398549);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0546799);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.395707);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0302006);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.121267);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0610359);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0396113);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0384432);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0278306);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0524112);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::Tile, 1.28139);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::Tile, 1.0865);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.096247);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0200766);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0730177);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0532891);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.528817);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0123498);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0881307);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0430971);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0876982);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.013554);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.009963);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0472481);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.204814);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0377986);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.17447);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0620926);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0335962);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0512487);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0280248);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.062042);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.213555);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0466305);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,3.41119e-05);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,71.9106);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,4.205);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0288788);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,0.0499375);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0358835);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.50815);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0299953);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0831335);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0636586);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0631181);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.035437);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0175601);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0607065);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00832451);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0352745);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00551041);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.064099);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E040bin, eta250bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.938537);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.171119);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.924129);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.143179);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.917185);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.201074);




   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.954266);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.111107);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,6.93862);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0222812);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.05958);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0517544);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,17.3766);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0206915);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.611547);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0580775);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,1.04035);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0231831);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.238767);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0547898);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.38187);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0547847);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,9.85757e-12);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0550315);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,1.2392);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0537575);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,3.21432e-09);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0540912);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.183177);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.022424);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.133571);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0568059);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0584261);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0197977);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0500795);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0497239);


   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.997715);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.10864);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.97559);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.023941);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.367692);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0558768);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,25.6327);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.017111);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.429426);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0567281);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,2.04114);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0239498);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.195294);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0556291);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,2.12009);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0302637);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,1.1022);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0583135);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,1.67527);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0316239);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.934956);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0581086);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.143224);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0259773);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.132363);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0581515);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0413278);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0265645);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0391527);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0553292);


   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME3, 1.32172);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME3, 1.08126);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.434314);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0499976);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.00315646);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0511649);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,3.84757);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0180008);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.488114);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0489815);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,6.15647);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0181461);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.148583);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0545803);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,3.84018);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0253965);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.69809);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0477776);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,2.21125);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0301085);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,1.57104);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0526204);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.17911);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0359555);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.13777);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0589333);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0486644);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0377779);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.037075);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0550965);


   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::HEC, 1.22809);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.965488);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0390141);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0640783);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00267693);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0664228);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.0707838);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0468597);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,2.10558e-10);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0526946);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.360085);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0365859);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,7.80206e-11);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0568998);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.30559);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0509923);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,7.75102e-09);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0606213);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,1.43436);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0187197);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.199697);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0546252);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.419325);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0215523);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.06614);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0546107);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.77969);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0229598);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.1181);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0422326);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,4.46757);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0239232);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,3.47935);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0434758);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.59779);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0267249);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.411918);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0495412);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.201751);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0263779);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.14207);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.048453);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.105861);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0541321);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.048028);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0598984);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0244478);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0380468);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0119366);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0597578);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.962206);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.125841);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0663686);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0487306);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0407854);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0538701);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.27636);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0451348);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.155144);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0552165);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0483879);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0441264);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0308212);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0518807);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.040808);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0432832);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,3.21299e-09);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0449541);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,301.039);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0174358);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,0.0802715);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0539511);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.134351);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0632078);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0170909);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0659339);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0266647);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.04811);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00323806);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0821658);

    }

  return StatusCode::SUCCESS;

}

StatusCode eflowCellEOverPTool_mc12_LC::finalize(){
  return StatusCode::SUCCESS;
}
