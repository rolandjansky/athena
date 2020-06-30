/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowCellEOverPTool_mc12_HLLHC.h"
#include "eflowRec/eflowCaloRegions.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <vector>
#include <iomanip>
#include <sstream>

eflowCellEOverPTool_mc12_HLLHC::eflowCellEOverPTool_mc12_HLLHC(const std::string& type,const std::string& name,const IInterface* parent) : IEFlowCellEOverPTool( type, name, parent) {

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
  m_etaBinBounds.push_back(3.5);
  m_etaBinBounds.push_back(4.0);
}

StatusCode eflowCellEOverPTool_mc12_HLLHC::initialize(){

  return StatusCode::SUCCESS;
}

StatusCode eflowCellEOverPTool_mc12_HLLHC::fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const {

  if (binnedParameters) {

    binnedParameters->initialise(m_eBinValues, m_etaBinBounds);

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.321000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.299000 );


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.58218);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0168204);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.25334);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0271471);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.663936);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0189791);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,5.53498e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0316743);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0203202);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0196479);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,6.94375e-14);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.036906);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00835219);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0264806);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0044585);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0791346);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.000400347);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0212892);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000303684);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0730744);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.465000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.304000 );


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.292708);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0258821);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,1.45001e-11);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.043646);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.21009);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0134251);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.10769);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0164651);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0624758);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.020866);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,6.16482e-13);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0317775);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0115304);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0311416);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00862162);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0391306);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.300000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.288000 );


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.332119);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0195858);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.235161);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.032395);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.380046);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0126418);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.188588);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0154497);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.361352);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.021527);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,1.89128e-16);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0425202);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.404778);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0320893);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,4.62233e-16);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0534411);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0184951);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0309242);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0120615);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0479672);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.000868798);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0519249);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.000587644);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0849451);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.308000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.542000 );


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0698781);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0138596);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0534621);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0256467);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.161996);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0148295);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,7.89294e-05);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0915887);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0245581);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.00865135);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0191107);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0173007);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.583746);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0269363);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.433412);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0413036);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0398166);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0381357);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.14169e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0554185);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00278402);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0322164);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00215081);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0973158);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.304000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.312000 );


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.0613);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0156099);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.808449);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0267883);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.435437);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0160278);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.211776);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0183567);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0185577);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0247299);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,3.72713e-15);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0481121);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00570054);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0426672);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00351536);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0554037);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.457000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.285000 );


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.201064);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0171953);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.151554);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0286717);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,1.43369);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0215994);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.67493e-09);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0352575);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0497882);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.022884);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0185839);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0288421);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0123494);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0451839);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00635336);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0564201);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.300000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.509000 );


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.233052);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0303991);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.000362163);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,100);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.30696);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0131204);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.166569);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0158926);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.399002);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0169557);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.2238);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0218754);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.164223);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0320828);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.107628);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0490205);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00587084);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0309042);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00389205);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0593085);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.515000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::Tile, 1.039000 );


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.393702);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0343823);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,2.41034e-12);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0509397);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0292083);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0309945);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0229514);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0378671);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.316000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.229000 );


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.711413);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.016814);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.540101);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0266796);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.359921);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0181035);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,1.77937e-06);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,98.1747);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0187861);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0267296);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,2.17679e-13);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0418478);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0012833);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0407267);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.000841242);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0497419);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.444000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.255000 );


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.168614);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0183531);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.124575);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0279988);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.0004);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0119676);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.17914);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0143852);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0582675);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0190325);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0317971);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0235254);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.00582294);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0288492);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00437983);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0452182);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.300000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.583000 );


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.297088);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0172759);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.217393);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0333402);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.306666);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0160616);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.216629);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0190399);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.0461627);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0562768);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0207018);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0657336);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00149515);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0641968);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.000783497);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0715853);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.591000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.343000 );




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.651000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.811000 );




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.332000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.472000 );


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0810416);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0335637);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0558209);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0443118);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0174255);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0416614);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,1.5816e-12);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0580952);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0034654);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0433706);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,1.91116e-10);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0594901);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.00724761);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0548047);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.00255358);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0860387);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00124882);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.190155);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,5.35791e-07);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,1.71129);


   // j1st = EMB2




   // j1st = EMB3




   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.000000 );


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,1.74282);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0159687);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.34623);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0229601);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.0589921);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0158155);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,3.87065e-09);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0212464);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.0358702);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0297682);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.0239638);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0455734);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.0114182);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.188175);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,6.75831e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,37.6017);


   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.000000 );


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.364715);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0150168);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.276339);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0271537);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.182221);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0133989);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.131511);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0166971);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.0728284);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0171654);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.0524122);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0308567);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.00135954);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.188377);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,6.77379e-07);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,21.542);


   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.000000 );


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,0.667073);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0273051);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,0.502539);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0376906);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.00853938);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.45);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.00165487);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,100);


   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.000000 );




   // j1st = Tile


    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.406000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.312000 );


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.43156);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0163051);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.12042);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0431609);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.586798);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0147808);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.230807);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0457591);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.025757);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0396094);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,1.20506e-11);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0597906);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0636316);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0563992);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0433544);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0646537);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0189566);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0506081);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0144211);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0611632);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00318928);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0496917);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00230189);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0580063);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.499000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.198000 );


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.248212);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0151245);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.182582);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0294892);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.13156);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0128634);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.876613);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0173541);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0751431);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0168211);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.03546);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0292961);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0536873);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0560957);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,2.62402e-09);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0642689);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0109966);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0447182);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.000407257);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0886383);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.0026831);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0422008);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,1.01313e-07);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,100);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.334000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.316000 );


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.148983);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0153559);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.109587);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0305993);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.597489);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0122659);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00397483);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0462338);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.288587);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0209442);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,6.22038e-05);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0947991);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.443061);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0367057);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.276648);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0489165);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0430324);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0387216);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0263116);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0479872);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00356521);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0466604);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00288499);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0596275);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.740000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.230000 );


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.11613);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.046663);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,9.00573e-10);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0615625);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.365771);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0116307);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0196174);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0526727);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0241309);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0132287);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,2.93404e-05);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.24575);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.548824);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0315008);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.355506);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0542414);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0666754);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0476401);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0336508);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0540582);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00879835);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0434908);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00695046);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0577408);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.357000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.285000 );


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.951874);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0404413);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,2.09284e-10);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0592775);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.446213);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0159506);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.127674);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0474695);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0247545);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.010705);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0150163);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0516894);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0422736);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0698411);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0120804);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0746947);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00298504);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0627866);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0010503);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0668908);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.000401819);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0674418);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000253859);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0762671);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.482000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.214000 );


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.162365);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0176177);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.1142);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0353907);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,1.50849);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0168855);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.000911174);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0757216);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0653476);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0223868);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0231897);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0316196);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0381943);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0424628);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0219708);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0497537);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00251685);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0476216);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00115537);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0597969);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.367000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.232000 );


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.124101);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0211553);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0789882);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0456678);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.544969);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0173078);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00418027);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0591932);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.386584);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0127227);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.255357);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0229685);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.243719);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0458036);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,7.2292e-14);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.061688);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0201373);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0423654);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0115447);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.049243);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00145989);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0482276);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00111997);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0701061);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.755000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.293000 );


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0778484);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0169458);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0592545);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.044326);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.265112);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0136897);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00660444);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0544958);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0301779);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.012885);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00686981);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.046152);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.529264);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.044202);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,5.82567e-05);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.20014);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0282497);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0688898);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00711749);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0810541);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0055954);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0717456);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,1.10729e-06);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,3.78592);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.300000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.275000 );


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.618127);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0211489);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.373319);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.053205);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.416507);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0135327);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.113642);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0530699);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0217858);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0483019);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,8.02195e-11);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0578879);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0587616);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.058959);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,9.03463e-10);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0775054);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00153442);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.361609);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,3.31998e-10);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,32.0691);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0140964);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0686344);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00482143);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.072399);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00179167);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0792087);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000558903);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0844355);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.000468176);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0747588);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000320548);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0856648);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.414000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.250000 );


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.135475);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0176591);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0939267);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0364501);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.19448);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.00933317);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.15722);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0171829);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.062551);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0287651);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.000128159);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.090063);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0131508);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0447349);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00696293);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.067602);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00172008);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.047677);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.000868349);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0536292);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.373000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.218000 );


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.125619);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.016321);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0905572);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0340535);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.40404);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0140158);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,2.24734e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,100);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.474099);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0228371);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.000262616);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.126311);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.366807);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0348501);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,8.57729e-10);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0536777);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,1.09793);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0128531);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.00535363);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.35329);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,299.656);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0224636);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,0.108199);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0570538);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.0860347);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.036449);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0517141);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.053564);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00476957);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0188928);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00451606);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0730821);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.000350296);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.118436);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,8.87913e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.131158);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.300000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.262000 );




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.339000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.223000 );




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.300000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.672000 );


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0276654);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0222098);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00534601);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0780049);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.201252);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0136194);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00100958);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0848644);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0463383);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0186511);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00355049);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0636113);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0124098);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0697969);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,2.02768e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,43.0469);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.185515);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0295411);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.117274);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0468);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0257123);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.042705);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,1.12688e-12);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0613129);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00465345);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0452127);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,6.02903e-11);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0614419);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////


   // j1st = EMB1


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.00458102);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0145426);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,1.88805e-15);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.5599);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0019465);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.218184);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,8.23078e-07);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,7.60627);


   // j1st = EMB2




   // j1st = EMB3




   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.300000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.279000 );


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,2.80389);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0174439);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.92293);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0420891);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,1.88636);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0119581);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.784414);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0412051);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.086613);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0108126);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.0445219);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0431278);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.099347);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0645505);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.0338671);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.07);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.0464464);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.066873);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.0202846);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0750935);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.00370922);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0855148);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.00148092);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.098304);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.000395498);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.45);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.000137124);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,99.9999);


   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.415000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.244000 );


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.411762);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0155112);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.296018);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0317198);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,8.5939);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0161733);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.128641);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0554278);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.324516);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0197057);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.106326);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0495868);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.143272);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0566275);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.0568543);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.062271);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.0814092);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0673488);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.0406843);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.073446);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.00720591);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.071149);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.00348804);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0807755);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.00397349);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0948414);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.000734123);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.102828);


   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.360000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.264000 );


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.221942);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0311642);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.1124);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0387052);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,3.24687);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0132041);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.0280368);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0559469);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.20326);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0158991);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,1.72949);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.02414);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,1.02871);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0346604);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,0.630881);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0566312);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.114741);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0368067);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.0771602);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0443537);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.00622442);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.042518);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,5.37162e-12);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0595713);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.000489715);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.381595);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,8.33367e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.384994);


   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.582000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.334000 );


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.284158);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0456957);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,1.60367e-11);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0620157);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.5221);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0542033);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,2.33832e-13);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0637572);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.238457);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0462337);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,1.8411e-10);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0622381);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,1.81159);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0696539);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,8.75491e-12);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0695807);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,2.30227);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0620773);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.712492);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0795494);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.280207);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0704436);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.0959456);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0748832);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.10217);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0509335);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0389563);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0820412);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0356129);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0412401);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0206591);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0518396);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0109973);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.039154);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00669246);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0543702);


   // j1st = Tile


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.014411);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0147906);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.00950171);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0243858);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00286624);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.201153);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,1.22956e-06);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,1.74394);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0117055);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0305104);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,1.10007e-10);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0489894);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00110908);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0560136);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.000829553);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0693709);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.576000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.180000 );


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.43176);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0264147);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.791667);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0447015);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.74367);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0312494);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0155367);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0583005);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0506118);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0387003);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,1.30443e-12);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0591039);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.257556);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0457905);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.147931);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.053561);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0497407);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0457296);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,2.10153e-11);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0619428);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00987725);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0413989);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00707126);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0504321);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.608000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.154000 );


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.406566);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0262505);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.163512);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0428703);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.60076);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0189702);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0305073);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0464687);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.158106);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0203088);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0598946);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.031921);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.220037);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0454751);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,7.20622e-14);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.062655);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0359768);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0424384);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,9.57033e-12);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.060766);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00521616);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0455598);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,3.83091e-14);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0616888);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.524000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.154000 );


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.14787);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0170635);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.103371);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0405007);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.561538);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0100085);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.134143);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.030998);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.234464);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0101076);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.155732);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0226077);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.481828);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0285387);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.37224);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0416412);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0760033);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0392878);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,3.52227e-12);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0586419);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00729375);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0390991);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0053566);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0489968);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.722000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.154000 );


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.128433);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0155222);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0940428);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0493488);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.37723);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0121414);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0404692);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0438888);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0306165);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0123617);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0147644);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0277508);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.695735);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0275237);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.531432);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0438924);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.129321);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.027756);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0971365);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0387448);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0136531);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0410244);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0112152);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0511111);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////


   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.543000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.190000 );


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.52832);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0220805);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.972506);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0423516);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.86905);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0333982);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.141036);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0430651);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0510567);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0430306);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,2.4439e-15);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0608037);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.134676);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.051376);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0814807);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0587628);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0190709);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0499105);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0116366);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0576878);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00164961);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0516156);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00123942);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0606703);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.590000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.157000 );


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.186236);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.021036);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.12303);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0419191);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,1.87103);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0176736);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.00732167);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0560577);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.108894);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0213373);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0366756);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0346895);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0869118);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0476519);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0512302);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0544952);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0108252);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0447457);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00654401);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0522346);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.510000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.155000 );


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.109028);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0465488);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.31824e-10);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0613212);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.519179);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0116016);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.128661);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0287316);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.379677);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0111537);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.256543);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0245121);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.287154);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0366949);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.179849);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0510882);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0349159);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0393902);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0214494);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0497507);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00189587);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0445635);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00141012);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0551907);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.721000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.144000 );


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.107734);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0442725);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.04335e-10);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0618364);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.304626);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.00946568);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0772496);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0391326);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0464627);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0323949);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,4.60511e-06);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,1.18441);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.648665);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0302429);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.476018);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0445259);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0570575);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0406443);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0352308);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0475855);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00782868);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0458018);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0052633);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0548087);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.489000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.226000 );


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.79114);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0232748);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.562818);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0482167);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.552515);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0410349);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0280215);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0475267);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0270376);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0468892);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.00601263);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0489855);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0278002);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.063943);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,1.73426e-09);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.067019);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00422421);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0538067);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00295251);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0646187);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM1,0.0305991);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH1,0.143847);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM2,1.62703e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH2,0.191941);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0305965);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0550051);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0205343);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0622774);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00490065);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.058384);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00340977);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0649425);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.550000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.191000 );


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.175507);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0404948);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,9.76601e-12);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0611764);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.7971);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0147703);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.255061);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0392313);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0928868);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0294416);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0204564);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.041184);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0590927);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0120044);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.0266221);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0575838);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.00950486);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0766625);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,1.00831e-06);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,89.8731);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM1,0.00125205);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH1,0.340479);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM2,2.91253e-14);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH2,37.3823);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.035015);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0523895);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0217389);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0590855);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00434122);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0568257);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00273717);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0641086);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.535000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.198000 );


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0852493);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0154937);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0609738);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0530026);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.442581);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0111968);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0862074);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0426281);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.524073);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0124265);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.326695);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0259184);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0387601);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0569221);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,2.65288e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,29.8963);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0372308);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0378605);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,9.26293e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0523843);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,278.088);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0205206);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,0.108545);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0569066);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.200939);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0434414);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.111024);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0558184);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.013543);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0568308);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00640424);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0637356);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.402000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.233000 );




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.459000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.168000 );




   // j1st = EME3




   // j1st = HEC


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.136382);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0448769);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,5.47603e-06);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.047752);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.0018476);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0854441);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,2.72158e-12);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,99.8487);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0762528);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0447318);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.32456e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0603275);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.100018);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0419884);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0786916);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0557095);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.0266601);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0693337);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,1.1751e-06);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0694083);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.181088);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0514933);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0899712);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0568709);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0187196);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0556232);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00933904);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0606709);


   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.795000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.169000 );


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.10351);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0485295);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,9.07744e-12);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0623686);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.125739);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0424995);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,8.81662e-13);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0617634);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0573944);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0345456);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,4.28562e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0532603);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0866207);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0317206);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0726558);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0600944);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00484889);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0784019);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.00337335);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.100586);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,445969);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.00635418);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.0392434);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0541595);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.267861);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0314505);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.172691);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0543714);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0563838);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0445705);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0329435);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.053013);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00919329);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0502742);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00624208);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0658234);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.0511672);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0173667);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.0365299);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0426034);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0111146);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0596594);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.0049117);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0743571);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.000381564);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0342217);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00025611);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0525073);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM1,3353.53);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH1,0.000895537);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM2,0.0803427);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH2,0.0710168);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00121142);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.124225);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,2.21386e-05);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.126535);


   // j1st = EMB2




   // j1st = EMB3




   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.483000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.239000 );


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,4.15008);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0239174);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.70041);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0502026);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,4.9675);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0383672);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.560446);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0489854);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.325845);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.046213);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,2.75898e-11);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0613053);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.533422);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0530403);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.266806);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0692005);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.609148);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0517168);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.255356);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0678099);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.0687165);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0612782);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0299398);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0696185);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.015527);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0638857);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0071023);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0744424);


   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.547000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.192000 );


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.587762);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0256319);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.246536);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0531167);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,10.8857);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0178785);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.89118);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0457925);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.667654);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0263233);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.11678);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0548898);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.421174);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0480662);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.167724);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0768477);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.225175);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0641018);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.101274);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0713283);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0258046);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0574324);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.0140833);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0672338);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.00832946);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0498456);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.00498009);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0667664);


   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.572000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.220000 );


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.300834);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0470954);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,3.25232e-12);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0623969);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,2.78532);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0131695);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.452008);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0515226);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.33656);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0130931);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,1.73185);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0302168);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,1.75021);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0327991);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.14403);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0558077);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.617317);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0557263);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.257868);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0627204);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.047606);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0591886);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0213238);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.065177);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0130116);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0539308);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.00646192);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0619736);


   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.679000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.230000 );


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0796651);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0479837);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,1.67211e-10);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0596882);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.737579);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0258178);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,0.420932);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0343544);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.351888);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0510842);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,3.05314e-12);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0633916);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.65514);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0488786);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.0340784);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0556956);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.265112);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0145909);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.1093);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0533027);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.17383);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.030074);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.40347);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.059193);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,2.39075);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0388658);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.899707);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0741364);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.277399);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0578693);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.000338516);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.216801);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.0829032);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0438172);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0440031);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0662169);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.104072);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0544505);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.044512);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0596786);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0144483);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0526697);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,6.25827e-10);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0637125);


   // j1st = Tile


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00235472);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.05536);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.001529);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0648168);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.184838);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0336996);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,2.40929e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,99.9976);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0560997);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0356523);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,2.64137e-10);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.055552);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0114804);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.062333);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.00435962);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0998012);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.0411583);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0579674);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,1.31741e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0637596);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0149687);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0673642);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.00491883);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0729786);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00190878);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0168903);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.000383659);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.111162);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.658000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.127000 );


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.26847);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0257643);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.141225);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0568183);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.36902);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0246428);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0499834);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0521249);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.125038);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0233302);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0367646);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0385952);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.520795);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0251202);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.328618);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0480523);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.105142);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.026393);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0753722);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0419659);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0133497);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0381432);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00867097);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.048894);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.669000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.126000 );


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.397084);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0260478);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.043321);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0528443);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.1401);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0169856);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0284962);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0445515);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.206694);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0163943);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0708912);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0290886);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.322759);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0260355);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.26906);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0450194);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0563909);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0402059);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,1.85716e-13);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0559031);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00889098);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0449852);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,2.27276e-13);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0608962);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.583000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.123000 );


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.107883);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0255483);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0322997);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0493452);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.365157);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0152538);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00657056);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0557869);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.219015);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0105978);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.15959);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.020256);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.526796);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0263296);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.378852);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0391036);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.107037);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.026992);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.078879);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0371195);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0116382);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0356413);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00810173);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0473555);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.760000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.134000 );


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.112534);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0209078);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0612746);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0431291);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.29426);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0161611);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0147467);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.043944);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0347578);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0125305);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0138492);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0281717);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.789035);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0221812);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.612032);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.034079);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.154621);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0334293);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.93648e-05);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.108695);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0182256);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0447715);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,2.17268e-11);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0595785);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.620000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.139000 );


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.45897);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0244269);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.485298);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0452412);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.38804);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0234058);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0633661);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0506653);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0675873);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0234324);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0365939);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0419819);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.227198);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0497176);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,7.2288e-10);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0571454);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0314932);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0298404);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0198815);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0500504);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00187819);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0464706);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00133628);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.055265);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.654000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.126000 );


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.23826);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0287242);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0296536);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0561712);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.55626);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0173003);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0420874);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0422297);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.170413);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0205383);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0296703);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0382141);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.176682);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0285588);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.159897);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0503605);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0273297);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0455678);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,1.83236e-14);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0605593);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00277535);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0426334);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.0019246);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.052521);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.556000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.107000 );


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.080015);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0238996);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0373374);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0507116);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.392235);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0157038);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.020834);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0442625);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.290069);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0177745);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0118027);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0406984);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.348993);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0284273);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.270348);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0436774);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0482153);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0289524);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0368072);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0420967);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00306544);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.040368);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00212887);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0539898);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.738000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.128000 );


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0456492);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0159767);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0310284);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0465349);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.190177);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0107129);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0196107);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0335917);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0463462);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0129062);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0069137);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0329134);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.63315);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0238872);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.480849);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0371451);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0691731);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0382234);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,1.01373e-11);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.059777);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00365001);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0443089);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00277234);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0560647);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.581000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.166000 );


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.30745);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0290323);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.127655);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0555132);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.891194);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0263361);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0681112);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0509749);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.055814);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0336575);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0307466);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0444479);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0344114);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0581535);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.00836807);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.063316);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00520136);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0675133);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00216503);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.080296);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0723214);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0566884);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00392935);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0585221);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00676273);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0558888);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0038172);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0629374);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.629000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.159000 );


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.338733);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0236299);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.177718);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0417135);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.19544);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0192795);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0363422);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0475842);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.167427);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0253788);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.02569);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0438348);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.01127);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0587553);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.00965997);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0689978);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0407365);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0445821);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,5.15947e-11);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0606615);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0901288);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0540389);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00952278);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0565413);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00523441);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0628054);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00201054);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.070353);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.618000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.155000 );


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.09757);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0209532);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0705625);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0501152);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.495793);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0243236);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0105066);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0593188);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.56928);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.013114);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.308539);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.024683);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0853562);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0479908);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,6.98167e-09);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0617534);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0322536);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0148471);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.00525991);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0977182);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,0.0951486);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.045434);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,1.37726e-06);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,64.4744);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM1,50.4708);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH1,0.000518764);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM2,0.00557717);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH2,0.0586984);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.302789);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0288472);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.238756);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0462398);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0216502);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0505848);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00970464);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0566738);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00226788);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0688328);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,1.02221e-05);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0705482);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.474000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.167000 );




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.538000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.140000 );




   // j1st = EME3




   // j1st = HEC


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.193592);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0241119);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0505344);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0575344);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.0118399);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0308121);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,5.28977e-13);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.053755);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0564312);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0634212);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0275778);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0780485);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0345557);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0535107);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0175554);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0633892);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,5.0867);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0254908);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,1.47043e-05);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,72.6057);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.220489);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0529551);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,2.00189e-06);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,88.8299);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0142571);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0479393);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00834526);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0600595);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.816000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.129000 );


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0877375);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0331704);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0169171);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0628235);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.179143);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0207188);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0536145);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0429526);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0575629);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0188798);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0145695);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0383498);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.115862);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0465705);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0734799);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0545995);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0365587);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0545131);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0238504);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0667381);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,1.28941);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0292393);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,3.27216e-09);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0518437);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,293.669);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0130995);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,198.393);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0272136);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.360333);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0276764);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.17285);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0501804);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0618056);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.030395);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.036782);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0510028);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00717991);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0327531);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00400793);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0646005);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////


   // j1st = EMB1




   // j1st = EMB2




   // j1st = EMB3




   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.580000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.175000 );


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,5.29384);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0223761);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.99781);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0457662);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,7.8631);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0234169);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,2.28111);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.043148);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.408412);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0450111);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,1.13702e-10);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0599763);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.909703);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0243617);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.69451);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.057482);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.870895);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0281186);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.606206);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0571675);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.105631);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0270518);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0879581);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.056267);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0313929);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0470701);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,2.65879e-16);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0621862);


   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.638000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.145000 );


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.929508);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.018059);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.794451);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0438236);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,15.627);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.01926);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.682889);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0526405);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.18005);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.024933);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.149787);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0540057);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.895263);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0358552);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.453459);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0664068);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.721926);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0264127);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.52165);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0641985);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0613263);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0617475);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.00013177);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.18982);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.018728);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0591224);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,5.81739e-14);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0612284);


   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.661000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.157000 );


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.359184);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0485797);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.124247);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0532233);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,2.54092);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0182001);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.774736);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0448014);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.75711);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0217872);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.141581);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.054786);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.20746);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0360479);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.36022);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0550949);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.04435);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.048398);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.499602);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0624226);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.0909056);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.049404);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0475978);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.057347);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0203063);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0431742);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0143546);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0507402);


   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.722000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.175000 );


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0129383);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0504252);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00945794);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0579527);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.288101);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0508515);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.0991999);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0551192);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.650296);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0442932);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.0109936);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0674595);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.220481);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0221806);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.0710614);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0560189);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.35534);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0225483);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.80025);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.047595);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,2.92962);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0242139);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.10523);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0517304);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.306678);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0525893);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,3.66634e-05);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.622399);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.117962);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0471868);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0459269);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0526173);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.104755);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.053291);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0502035);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0627469);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0118436);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0507354);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00713461);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0632954);


   // j1st = Tile


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0375708);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0474946);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,2.52216e-10);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0616794);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.128119);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.044039);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.078585);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0551801);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0200631);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0434832);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.000176544);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,99.9993);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,481464);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.00677049);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.902576);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0264165);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0524479);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0614769);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.024961);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0670193);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00473647);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0670134);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00107146);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0716983);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.691000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.099000 );


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.22286);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0241584);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.172378);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0533242);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,4.05152);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0196149);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0642167);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0513073);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.160269);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0209104);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0285107);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0392588);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.550724);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0258677);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.247482);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0492322);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.10623);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.024479);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0769743);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0384226);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0142365);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0341461);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00939109);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.045253);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.694000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.103000 );


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.793048);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0251113);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.027439);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.057932);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.57373);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0165723);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0311546);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0455675);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.269796);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0179657);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0174461);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0373641);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.444891);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0261207);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.270118);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0449894);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0536461);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0404201);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,3.28968e-11);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0409252);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00923773);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0450636);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,3.37923e-12);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0596799);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.600000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.110000 );


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.104622);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.022586);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0469233);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0465624);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.400436);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.015268);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0177332);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0398485);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.243751);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0138888);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0168723);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0305657);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.555191);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0245941);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.416403);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0399252);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.126083);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0247628);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0964075);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0400248);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0150651);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0357455);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0100389);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0463317);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.783000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.102000 );


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0682025);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0155913);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0472144);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0417573);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.240625);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0129248);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0139777);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0402148);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0317658);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0139131);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00529227);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0322097);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.801009);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0186565);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.591533);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0330683);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.163532);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0324993);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.09362e-05);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.173905);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0222134);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0325613);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0153872);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0462309);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.663000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.114000 );


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.51378);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0273804);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.136236);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.054484);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.34341);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0210483);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0763926);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0479934);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.108172);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0229198);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0342008);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0426187);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.25801);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0269534);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.163013);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.053386);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0355739);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0255167);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0287438);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0462245);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00234796);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.044317);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00166527);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0583839);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.681000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.108000 );


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.490985);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0248469);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0393611);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0519812);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.32719);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0162868);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0231018);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0491577);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.16297);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0225159);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.00982868);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0471155);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.218408);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0279049);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.135629);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0518661);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0287619);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0284021);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0171934);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0491029);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00218528);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0445654);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00145678);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.057174);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.584000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.089000 );


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0759035);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0231883);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0383519);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0478522);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.525387);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0152318);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0195471);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.04203);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.43908);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.014938);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0120291);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0391316);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.353532);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0287434);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.256231);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0426947);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0532986);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0264294);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0398616);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0414249);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00291059);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0422746);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00207431);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0550312);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.761000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.110000 );


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0554494);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0182134);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0365286);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0425278);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.148982);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0176383);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00920694);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.044749);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0402713);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0172119);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00410208);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0385493);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.70349);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0219506);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.514606);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0339096);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.100317);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0247921);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0740704);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0380426);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00594556);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0409529);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0036856);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0515688);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.632000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.138000 );


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.923022);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0236095);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.345001);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0446888);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.24677);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0229866);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0437655);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0537223);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0658013);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0236996);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0350941);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0441018);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0718088);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0418597);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,3.10418e-12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0619809);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00811941);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0622673);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,1.30591e-05);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,88.9661);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,926166);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.00574081);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,0.171801);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.0428157);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0548374);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0602103);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.60391e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.064627);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00839087);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0582517);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00238365);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.063023);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00149664);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0666316);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000294337);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0709869);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.674000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.141000 );


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.481914);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0279371);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0319074);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0591499);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.45163);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0162708);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0521545);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0464636);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.172051);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0233377);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0260823);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0430338);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.000709271);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0821057);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.000607947);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0989921);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0125993);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0489716);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,1.06907e-13);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0613261);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0799019);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.056891);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00199701);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0584965);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00568685);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0614987);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00268959);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0696051);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.670000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.124000 );


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0896502);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0430149);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,8.61018e-12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0605165);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.405883);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0175624);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0462341);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0429915);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.604687);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0166912);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0368151);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0381554);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0952323);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0424092);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0630234);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0563537);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0581073);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.055654);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,2.79056e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0613598);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.316898);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0254784);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.176037);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0485687);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0233718);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0492296);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0137241);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0565655);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0022339);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0539419);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00129777);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0616361);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.519000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.149000 );




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.576000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.103000 );




   // j1st = EME3




   // j1st = HEC


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.113967);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0424065);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0207825);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0444903);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.00588077);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0598422);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,6.58715e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0650588);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0657888);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0549409);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0379762);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0635992);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0316191);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0507851);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0271021);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0634956);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.0351864);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0391264);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.0187348);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0600992);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.288794);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0270478);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.180582);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0530235);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0250609);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0482719);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,3.95042e-11);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0624533);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.805000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.120000 );


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0728621);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.046984);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,9.89981e-12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0584146);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.251993);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0146801);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0676421);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0435449);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.050834);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0140769);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0193356);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0338331);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.1128);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0532951);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0508425);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0616501);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.028327);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0574232);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0157782);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0693185);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.0484037);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0620854);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,4.00496e-07);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0657159);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,0.514656);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0448175);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,8.49537e-06);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0487596);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.367981);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0247012);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.265064);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0435837);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0697904);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0348195);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00807895);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0635174);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00689002);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0361319);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00525004);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0577743);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1




   // j1st = EMB2




   // j1st = EMB3




   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.635000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.141000 );


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,6.19746);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0233926);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.64489);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0471837);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,11.3375);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.024017);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.784126);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0521671);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.50557);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0254571);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.24039);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0505531);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.14357);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0230458);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.892218);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0558674);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,1.12562);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0299834);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.624859);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.058697);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.119477);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0428752);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0740935);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0537863);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0427884);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0438706);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0265513);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0538541);


   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.690000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.127000 );


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.20666);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.021164);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.804359);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0461634);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,16.2928);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0192049);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.530989);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0512967);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.21617);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0241392);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.25691);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0482477);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.11354);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0349349);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.563962);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0608327);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.846323);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0346222);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.504759);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0613693);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0755724);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0358791);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.0598718);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0632458);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0261397);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.040521);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0150148);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0599371);


   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.692000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.119000 );


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.446582);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0487708);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,5.17897e-13);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0632326);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,2.98957);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0180235);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.813269);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0453);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,4.69935);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0192552);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.153444);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0514732);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.57509);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0290207);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.76901);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0510153);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.48191);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0389556);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.880331);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0566413);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.136242);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0349211);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0946762);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0610287);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.044527);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0403116);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.033562);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0540277);


   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.739000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.136000 );


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0153596);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0503328);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0089752);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0553728);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.290073);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0339623);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,2.95932e-11);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0557426);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.332149);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0515326);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.0235258);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.053458);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.989708);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0128883);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.39949);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0472934);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.243986);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0192832);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.0922185);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0473386);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.1387);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.023901);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.63294);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0447968);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.02984);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0263946);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.08898);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0472308);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.358412);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0290191);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.243905);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0515002);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.085789);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0370426);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0646299);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0523088);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0601389);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0526165);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0311897);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0600758);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.00977417);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0384518);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00522021);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0632307);


   // j1st = Tile


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0201071);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0503644);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0156756);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0570086);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.123583);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0421338);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0686842);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0537062);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.028633);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0355167);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.019514);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.046151);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.163842);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0344131);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.000124144);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,99.4109);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0885137);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0236117);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0634066);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0630784);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0127241);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0446466);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00070743);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0885421);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.730000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.083000 );


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.17614);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.024964);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.12352);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0522556);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.127318);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0163003);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0633877);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0311579);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.384456);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0397603);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00795027);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0893198);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0813436);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0267562);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0438048);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0432945);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00486804);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0403581);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0030982);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0495172);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.729000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.084000 );


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.67587);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0185049);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.034705);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0478348);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.279231);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0168225);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0390385);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0299849);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.312085);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0422105);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,8.01646e-11);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0427923);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0564904);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0241307);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0383508);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0415124);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.0079108);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0281026);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00575035);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0465838);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.646000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.080000 );


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.120833);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0108529);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0455207);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0409214);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.62799);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.035329);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.00231607);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0835762);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.114452);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0249358);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.084622);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0360926);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0143931);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0355374);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0103169);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0454802);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.801000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.089000 );


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0661352);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0181298);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0308186);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0336525);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.903132);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0280265);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.00173836);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0791247);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.144279);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0228577);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.107244);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0334375);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.011505);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0314205);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0076875);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.042833);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.708000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.091000 );


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.56119);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.024423);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.114499);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0551192);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.62351);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0184546);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0635337);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0487529);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.121118);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0226581);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0182105);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0479331);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.241527);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0279677);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.10282);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0577611);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0302174);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0456934);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,1.56032e-12);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0605244);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00284517);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.03803);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00204332);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0561112);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.717000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.085000 );


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.392916);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0251562);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0664101);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0468055);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.73093);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0133327);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.052746);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0406351);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.146319);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0214577);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.010963);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0451028);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.212695);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0223284);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.181934);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0475403);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0262181);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0265859);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0163539);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0497598);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00226322);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0472648);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00129847);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0552459);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.617000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.089000 );


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0779037);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0211502);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0320657);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0464143);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.459304);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0144688);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0306492);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0353507);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.47749);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.013729);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0115097);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.039844);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.391893);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0264173);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.206052);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0453452);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0399504);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0248274);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0228291);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0459155);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00306053);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0386042);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00205591);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0546711);


   // j1st = EME1




   // j1st = EME2




   // j1st = EME3




   // j1st = HEC




   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.787000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.106000 );


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0522344);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0173089);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0367975);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0427471);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.157775);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0133301);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0144742);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.040853);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0455022);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0151746);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00203424);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0464898);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.66148);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0238178);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.52503);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0363029);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0972988);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0247395);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0717163);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0390044);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00614898);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0428568);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00384012);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0535588);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.693000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.112000 );


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.04058);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0245497);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.158374);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.049984);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.88969);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0200308);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.068463);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0500765);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.106999);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0221355);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0321205);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0459137);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0357597);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0389003);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.0294906);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0657968);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0223763);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0195435);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.0191797);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0577746);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM1,1.85019e+12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH1,0.00693099);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM2,0.77333);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH2,0.0480124);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0865918);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0560916);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,4.69104e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0646545);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0103771);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0618199);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,1.42061e-10);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0640779);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.721000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.126000 );


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.417358);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0266421);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0430749);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0581093);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,6.06788);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0146726);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0656028);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0453245);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.142136);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0199057);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0404992);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0415489);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0821334);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0526648);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.00347676);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0544563);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0117072);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0602247);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.00677871);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0755989);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM1,0.228276);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH1,0.0323147);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM2,3.69142e-09);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH2,0.0357092);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0787826);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0574006);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,1.34438e-09);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0637603);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00665256);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0653984);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.001018);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0692364);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.720000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.108000 );


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0769331);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0435715);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.06973e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0606016);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.686974);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0146987);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0386745);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0425162);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.627117);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0149972);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0310439);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0387532);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0655697);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0484833);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0423458);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0569884);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0190078);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0555275);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.0126341);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0693699);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,9217.37);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.00210147);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,0.610271);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0333958);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.354687);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0292366);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.079949);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0559165);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0218668);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0339349);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0139948);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0609941);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00264671);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0586258);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00120061);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0663476);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.563000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.140000 );




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.627000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.127000 );




   // j1st = EME3




   // j1st = HEC


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0816557);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0420198);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,3.87155e-13);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0606918);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.414992);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0134515);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,0.106619);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0391022);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.0151375);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0417935);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,4.84735e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0560474);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.112463);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0383512);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0261453);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.068982);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0306669);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0485925);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0188906);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0581838);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.960074);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.027809);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.000281361);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0891797);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.279366);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0314745);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0389111);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0673132);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0265388);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0361786);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0184285);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0494776);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.822000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.110000 );


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0641832);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0201076);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0457372);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0508921);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.403482);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0125178);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0573864);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0428808);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0418837);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0141969);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00883953);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0428138);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.106501);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0532723);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0546096);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0621851);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0199346);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.052676);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0164619);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0640172);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.233811);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.043414);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,4.11018e-05);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,99.9104);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.391746);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0298154);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0508687);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.063098);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0491785);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0350276);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0120624);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0593722);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00671071);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0334307);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00463861);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.060557);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.203497);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.053081);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,6.68094e-08);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.061973);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0320718);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0646491);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.0113092);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0726401);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,0.40913);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.0231817);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,0.00618228);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.0339834);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0482104);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0617956);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0181561);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0688583);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00341247);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0549235);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00214413);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0635135);


   // j1st = EMB2




   // j1st = EMB3




   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.697000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.123000 );


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,6.5128);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0221447);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,1.09971);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0474854);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,16.3319);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0211335);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.338712);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0611115);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.869078);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0232659);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.164597);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0529256);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.14881);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0287525);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.655292);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0591881);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.968572);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0533593);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,7.14436e-06);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,98.1521);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.152309);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0232571);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.110022);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0558493);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0482718);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0253548);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0338111);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0518962);


   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.736000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.100000 );


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.73191);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0240263);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.348783);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0512831);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,25.8323);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0161874);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.376784);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0529721);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.56218);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0232427);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.134018);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0528657);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.4877);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0307377);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.688257);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0580025);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,1.29175);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0317916);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.674943);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0577605);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.123532);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0276264);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.092466);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0582832);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0339402);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0257484);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0315913);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0529622);


   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.729000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.095000 );


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.323704);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0462113);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,8.50017e-11);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.060623);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,2.86696);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0188349);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.283557);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0503395);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,4.39009);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0179396);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.120544);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0488857);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.79331);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0249441);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.93564);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0463383);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.76108);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0293338);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,1.19237);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0520034);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.147439);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0349693);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.105286);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0573916);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0391658);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0441786);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.018891);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0522748);


   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.763000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.111000 );


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0236786);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0607171);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00237314);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0634735);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.0375176);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0487391);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,2.54488e-10);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0519017);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.833552);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0296668);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,1.83443e-11);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0562922);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.252614);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0480815);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,4.66107e-13);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0618544);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,1.1218);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0190025);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.144791);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0522178);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.264636);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0207886);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.0501236);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.049792);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.11687);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0225741);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.60814);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0409602);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.56914);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0234799);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.77481);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0425815);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.472696);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0261771);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.330235);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.048248);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.155045);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0251619);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.119562);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0463912);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0730592);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0542234);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0284529);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.059963);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0158788);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0369271);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00973623);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0551154);


   // j1st = Tile


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0341412);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0488482);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0239211);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0544879);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.169896);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0444054);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0952583);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0552796);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0332019);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0453793);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0205827);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.05349);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.0234349);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0464545);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,1.09201e-11);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.061161);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,3.76257e+08);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.00782468);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,0.408109);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0401014);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0975048);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0595211);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0183247);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.06259);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0197959);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0459932);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00138243);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0876664);

    binnedParameters->setFudgeMean( E001bin, eta350bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta350bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta350bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta350bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta350bin, eflowFirstIntRegions::EME1, 0.619000 );
    binnedParameters->setFudgeStdDev( E010bin, eta350bin, eflowFirstIntRegions::EME1, 0.613000 );
    binnedParameters->setFudgeMean( E020bin, eta350bin, eflowFirstIntRegions::EME1, 0.532000 );
    binnedParameters->setFudgeStdDev( E020bin, eta350bin, eflowFirstIntRegions::EME1, 0.233000 );
    binnedParameters->setFudgeMean( E032point5bin, eta350bin, eflowFirstIntRegions::EME1, 0.572000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta350bin, eflowFirstIntRegions::EME1, 0.130000 );
    binnedParameters->setFudgeMean( E040bin, eta350bin, eflowFirstIntRegions::EME1, 0.639000 );
    binnedParameters->setFudgeStdDev( E040bin, eta350bin, eflowFirstIntRegions::EME1, 0.195000 );

    binnedParameters->setFudgeMean( E001bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E020bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E032point5bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeMean( E040bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta450bin, eflowFirstIntRegions::EME1, 0.000000 );

    binnedParameters->setFudgeMean( E001bin, eta350bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta350bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta350bin, eflowFirstIntRegions::EME2, 0.300000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta350bin, eflowFirstIntRegions::EME2, 1.837000 );
    binnedParameters->setFudgeMean( E010bin, eta350bin, eflowFirstIntRegions::EME2, 0.515000 );
    binnedParameters->setFudgeStdDev( E010bin, eta350bin, eflowFirstIntRegions::EME2, 0.203000 );
    binnedParameters->setFudgeMean( E020bin, eta350bin, eflowFirstIntRegions::EME2, 0.594000 );
    binnedParameters->setFudgeStdDev( E020bin, eta350bin, eflowFirstIntRegions::EME2, 0.186000 );
    binnedParameters->setFudgeMean( E032point5bin, eta350bin, eflowFirstIntRegions::EME2, 0.636000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta350bin, eflowFirstIntRegions::EME2, 0.148000 );
    binnedParameters->setFudgeMean( E040bin, eta350bin, eflowFirstIntRegions::EME2, 0.693000 );
    binnedParameters->setFudgeStdDev( E040bin, eta350bin, eflowFirstIntRegions::EME2, 0.131000 );

    binnedParameters->setFudgeMean( E001bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeMean( E020bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeMean( E032point5bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeMean( E040bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta450bin, eflowFirstIntRegions::EME2, 0.000000 );

    binnedParameters->setFudgeMean( E001bin, eta350bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta350bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta350bin, eflowFirstIntRegions::EME3, 0.424000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta350bin, eflowFirstIntRegions::EME3, 0.253000 );
    binnedParameters->setFudgeMean( E010bin, eta350bin, eflowFirstIntRegions::EME3, 0.549000 );
    binnedParameters->setFudgeStdDev( E010bin, eta350bin, eflowFirstIntRegions::EME3, 0.230000 );
    binnedParameters->setFudgeMean( E020bin, eta350bin, eflowFirstIntRegions::EME3, 0.612000 );
    binnedParameters->setFudgeStdDev( E020bin, eta350bin, eflowFirstIntRegions::EME3, 0.166000 );
    binnedParameters->setFudgeMean( E032point5bin, eta350bin, eflowFirstIntRegions::EME3, 0.642000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta350bin, eflowFirstIntRegions::EME3, 0.139000 );
    binnedParameters->setFudgeMean( E040bin, eta350bin, eflowFirstIntRegions::EME3, 0.687000 );
    binnedParameters->setFudgeStdDev( E040bin, eta350bin, eflowFirstIntRegions::EME3, 0.120000 );

    binnedParameters->setFudgeMean( E001bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeMean( E020bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeMean( E032point5bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeMean( E040bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta450bin, eflowFirstIntRegions::EME3, 0.000000 );

    binnedParameters->setFudgeMean( E001bin, eta350bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta350bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta350bin, eflowFirstIntRegions::HEC, 0.971000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta350bin, eflowFirstIntRegions::HEC, 0.500000 );
    binnedParameters->setFudgeMean( E010bin, eta350bin, eflowFirstIntRegions::HEC, 0.636000 );
    binnedParameters->setFudgeStdDev( E010bin, eta350bin, eflowFirstIntRegions::HEC, 0.220000 );
    binnedParameters->setFudgeMean( E020bin, eta350bin, eflowFirstIntRegions::HEC, 0.682000 );
    binnedParameters->setFudgeStdDev( E020bin, eta350bin, eflowFirstIntRegions::HEC, 0.180000 );
    binnedParameters->setFudgeMean( E032point5bin, eta350bin, eflowFirstIntRegions::HEC, 0.705000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta350bin, eflowFirstIntRegions::HEC, 0.145000 );
    binnedParameters->setFudgeMean( E040bin, eta350bin, eflowFirstIntRegions::HEC, 0.732000 );
    binnedParameters->setFudgeStdDev( E040bin, eta350bin, eflowFirstIntRegions::HEC, 0.130000 );

    binnedParameters->setFudgeMean( E001bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeMean( E020bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeMean( E032point5bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeMean( E040bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta450bin, eflowFirstIntRegions::HEC, 0.000000 );

    binnedParameters->setFudgeMean( E001bin, eta350bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta350bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta350bin, eflowFirstIntRegions::FCAL, 0.506000 );
    binnedParameters->setFudgeStdDev( E010bin, eta350bin, eflowFirstIntRegions::FCAL, 0.299000 );
    binnedParameters->setFudgeMean( E020bin, eta350bin, eflowFirstIntRegions::FCAL, 0.478000 );
    binnedParameters->setFudgeStdDev( E020bin, eta350bin, eflowFirstIntRegions::FCAL, 0.278000 );
    binnedParameters->setFudgeMean( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL, 0.473000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL, 0.261000 );
    binnedParameters->setFudgeMean( E040bin, eta350bin, eflowFirstIntRegions::FCAL, 0.502000 );
    binnedParameters->setFudgeStdDev( E040bin, eta350bin, eflowFirstIntRegions::FCAL, 0.268000 );
    binnedParameters->setFudgeMean( E001bin, eta450bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta450bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeMean( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL, 0.000000 );
    binnedParameters->setFudgeMean( E010bin, eta450bin, eflowFirstIntRegions::FCAL, 0.534000 );
    binnedParameters->setFudgeStdDev( E010bin, eta450bin, eflowFirstIntRegions::FCAL, 0.376000 );
    binnedParameters->setFudgeMean( E020bin, eta450bin, eflowFirstIntRegions::FCAL, 0.540000 );
    binnedParameters->setFudgeStdDev( E020bin, eta450bin, eflowFirstIntRegions::FCAL, 0.308000 );
    binnedParameters->setFudgeMean( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL, 0.569000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL, 0.273000 );
    binnedParameters->setFudgeMean( E040bin, eta450bin, eflowFirstIntRegions::FCAL, 0.626000 );
    binnedParameters->setFudgeStdDev( E040bin, eta450bin, eflowFirstIntRegions::FCAL, 0.268000 );


    ////////////////////////////
    //      2.5 <= eta <  4.0  //
    ////////////////////////////

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM1, 1.3813793);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH1, 0.0232664);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM2, 0.0039834937);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH2, 0.34451613);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM1, 0.17716285);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH1, 0.054805612);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM2, 0.026187203);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH2, 0.2464913);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM1, 0.01234669);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH1, 0.047877717);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM2, 0.014973409);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH2, 0.50541161);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM1, 0.31656023);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH1, 0.059290981);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM2, 0.14174174);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH2, 0.40185226);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM1, 0.1443976);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH1, 0.057142009);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM2, 0.007423484);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH2, 26111.848);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM1, 0.10888774);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH1, 0.17098204);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM2, 0.057311313);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH2, 0.82554351);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM2, 0.0036144337);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH2, 0.21087275);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM1, 0.059867211);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH1, 0.036830397);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM2, 0.006232976);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH2, 2.2431972e+08);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM1, 0.16109627);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH1, 0.046519754);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM2, 0.0082772051);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH2, 2542.2248);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM2, 0.003517246);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH2, 0.23591711);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM1, 0.022720075);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH1, 0.036575156);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM2, 0.0064717183);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH2, 2.6575438e+09);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM1, 0.0068899654);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH1, 0.048576774);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM2, 0.0083968234);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH2, 1568910.6);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E001bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E001bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 1.5019511);


    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM1, 1.3813793);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH1, 0.0232664);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM2, 0.0039834937);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH2, 0.34451613);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM1, 0.17716285);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH1, 0.054805612);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM2, 0.026187203);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH2, 0.2464913);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM1, 0.01234669);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH1, 0.047877717);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM2, 0.014973409);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH2, 0.50541161);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM1, 0.082213185);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH1, 0.031848575);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM2, 0.0021992948);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH2, 0.50648018);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM1, 0.1443976);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH1, 0.057142009);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM2, 0.007423484);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH2, 26111.848);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM1, 0.013193767);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH1, 0.12809421);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM2, 0.0094367992);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH2, 799.22293);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM1, 0.0099201798);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH1, 0.17534287);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM2, 0.0059733624);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH2, 51689.012);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM2, 0.0036144337);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH2, 0.21087275);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM1, 0.059867211);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH1, 0.036830397);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM2, 0.006232976);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH2, 2.2431972e+08);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM1, 0.16109627);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH1, 0.046519754);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM2, 0.0082772051);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH2, 2542.2248);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM2, 0.003517246);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH2, 0.23591711);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM1, 0.022720075);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH1, 0.036575156);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM2, 0.0064717183);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH2, 2.6575438e+09);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM1, 0.0068899654);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH1, 0.048576774);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM2, 0.0083968234);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH2, 1568910.6);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM1, 0.064274159);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH1, 0.062056168);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM2, 0.004851177);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH2, 407207.46);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E003point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E003point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 1.5019511);











    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM1, 1.3813793);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH1, 0.0232664);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM2, 0.0039834937);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH2, 0.34451613);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM1, 0.23609988);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH1, 0.05719458);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM2, 0.0081589893);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH2, 0.32690281);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM1, 0.011784363);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH1, 0.11826397);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM2, 0.0028071707);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH2, 27279.884);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM1, 0.082213185);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH1, 0.031848575);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM2, 0.0021992948);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH2, 0.50648018);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM1, 0.20183155);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH1, 0.059766992);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM2, 0.0064836516);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH2, 0.77168335);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM1, 0.018280145);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH1, 0.11454765);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM2, 0.0044973389);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH2, 4277.3138);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM1, 0.010507261);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH1, 0.16367948);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM2, 0.0026607468);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH2, 12028.25);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM1, 0.010507261);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH1, 0.16367948);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM2, 0.0026607468);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH2, 12028.25);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM1, 0.010507261);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH1, 0.16367948);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM2, 0.0026607468);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH2, 12028.25);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM1, 0.010507261);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH1, 0.16367948);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM2, 0.0026607468);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH2, 12028.25);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM2, 0.0036144337);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH2, 0.21087275);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM1, 0.055648604);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH1, 0.039226644);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM2, 0.0030136557);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH2, 50950.582);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM1, 0.18071435);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH1, 0.042852113);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM2, 0.0046857163);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH2, 3.6842169);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM2, 0.003517246);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH2, 0.23591711);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM1, 0.016981017);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH1, 0.034934684);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM2, 0.0027894721);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH2, 77064.908);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM1, 0.0075209883);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH1, 0.045748948);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM2, 0.0038305995);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH2, 2.5581616e+09);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM1, 0.051846429);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH1, 0.062647078);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM2, 0.002422447);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH2, 550038.9);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM1, 0.051846429);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH1, 0.062647078);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM2, 0.002422447);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH2, 550038.9);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM1, 0.051846429);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH1, 0.062647078);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM2, 0.002422447);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH2, 550038.9);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM1, 0.051846429);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH1, 0.062647078);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM2, 0.002422447);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH2, 550038.9);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E010bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E010bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 1.5019511);













    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM1, 1.3813793);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH1, 0.0232664);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM2, 0.0039834937);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH2, 0.34451613);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM1, 0.23449859);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH1, 0.052786845);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM2, 0.0047146775);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH2, 0.45730436);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM1, 0.031271648);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH1, 0.060151358);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM2, 0.0026248073);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH2, 444040.85);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM1, 0.018843124);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH1, 0.08280612);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM2, 0.0056078819);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH2, 0.26995778);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM1, 0.082213185);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH1, 0.031848575);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM2, 0.0021992948);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH2, 0.50648018);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM1, 0.26470439);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH1, 0.059848676);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM2, 0.0056677487);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH2, 0.49846045);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM1, 0.02357232);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH1, 0.099399152);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM2, 0.0028146556);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH2, 8.5208336);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM1, 0.012387016);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH1, 0.13900915);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM2, 0.0015286771);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH2, 4637.2694);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM1, 0.012387016);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH1, 0.13900915);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM2, 0.0015286771);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH2, 4637.2694);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM1, 0.012387016);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH1, 0.13900915);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM2, 0.0015286771);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH2, 4637.2694);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM1, 0.012387016);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH1, 0.13900915);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM2, 0.0015286771);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH2, 4637.2694);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM2, 0.0036144337);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH2, 0.21087275);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM1, 0.05252265);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH1, 0.039222033);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM2, 0.0018315621);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH2, 1342641.6);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM1, 0.21120431);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH1, 0.040515028);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM2, 0.0035771621);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH2, 1.3319126);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM1, 0.047839315);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH1, 0.074603286);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM2, 0.0014737539);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH2, 19264.942);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM2, 0.003517246);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH2, 0.23591711);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM1, 0.011382257);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH1, 0.034009729);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM2, 0.0019165648);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH2, 37985896);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM1, 0.0048428696);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH1, 0.043778151);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM2, 0.0024266003);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH2, 10.891788);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM1, 0.042437972);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH1, 0.060029289);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM2, 0.001511592);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH2, 67637.77);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM1, 0.042437972);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH1, 0.060029289);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM2, 0.001511592);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH2, 67637.77);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM1, 0.042437972);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH1, 0.060029289);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM2, 0.001511592);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH2, 67637.77);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM1, 0.042437972);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH1, 0.060029289);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM2, 0.001511592);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH2, 67637.77);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E020bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E020bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 1.5019511);













    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM1, 1.3813793);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH1, 0.0232664);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, NORM2, 0.0039834937);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME1, WIDTH2, 0.34451613);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM1, 0.91133313);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH1, 0.031747292);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, NORM2, 0.0095742452);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME2, WIDTH2, 0.14308432);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM1, 0.043243474);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH1, 0.042702698);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, NORM2, 0.0012876631);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::EME3, WIDTH2, 0.61753758);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM1, 0.017872019);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH1, 0.084197789);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, NORM2, 0.00074130888);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC1, WIDTH2, 0.50061279);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM1, 0.017872019);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH1, 0.084197789);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, NORM2, 0.00074130888);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC2, WIDTH2, 0.50061279);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM1, 0.017872019);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH1, 0.084197789);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, NORM2, 0.00074130888);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC3, WIDTH2, 0.50061279);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM1, 0.017872019);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH1, 0.084197789);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, NORM2, 0.00074130888);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME1,  eflowCalo::HEC4, WIDTH2, 0.50061279);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM1, 0.082213185);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH1, 0.031848575);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, NORM2, 0.0021992948);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME1, WIDTH2, 0.50648018);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM1, 0.34232222);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH1, 0.061562698);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, NORM2, 0.005208505);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME2, WIDTH2, 0.18303114);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM1, 0.047252116);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH1, 0.068264213);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, NORM2, 0.001276509);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::EME3, WIDTH2, 0.49466179);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM1, 0.021432233);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH1, 0.083042841);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, NORM2, 0.00028390353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC1, WIDTH2, 2233.5658);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM1, 0.021432233);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH1, 0.083042841);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, NORM2, 0.00028390353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC2, WIDTH2, 2233.5658);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM1, 0.021432233);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH1, 0.083042841);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, NORM2, 0.00028390353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC3, WIDTH2, 2233.5658);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM1, 0.021432233);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH1, 0.083042841);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, NORM2, 0.00028390353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::HEC4, WIDTH2, 2233.5658);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL0, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL1, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, NORM2, 0.0089706729);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME2,  eflowCalo::FCAL2, WIDTH2, 0.43388569);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, NORM2, 0.0036144337);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME1, WIDTH2, 0.21087275);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM1, 0.039133495);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH1, 0.041354959);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, NORM2, 0.00070313738);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME2, WIDTH2, 0.43677138);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM1, 0.22406408);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH1, 0.039434804);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, NORM2, 0.0012524351);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::EME3, WIDTH2, 0.4050655);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM1, 0.078739208);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH1, 0.063620685);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, NORM2, 0.00033318953);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC1, WIDTH2, 2.1746991);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM1, 0.078739208);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH1, 0.063620685);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, NORM2, 0.00033318953);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC2, WIDTH2, 2.1746991);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM1, 0.078739208);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH1, 0.063620685);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, NORM2, 0.00033318953);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC3, WIDTH2, 2.1746991);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM1, 0.078739208);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH1, 0.063620685);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, NORM2, 0.00033318953);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::HEC4, WIDTH2, 2.1746991);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL0, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL1, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, NORM2, 0.0073663499);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::EME3,  eflowCalo::FCAL2, WIDTH2, 0.58100713);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, NORM2, 0.003517246);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME1, WIDTH2, 0.23591711);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM1, 0.0031119248);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH1, 0.034001721);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, NORM2, 0.00026767493);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME2, WIDTH2, 9159295);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM1, 0.0016333563);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH1, 0.04466355);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, NORM2, 0.00048380202);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::EME3, WIDTH2, 1.2059434);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM1, 0.023887099);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH1, 0.058887659);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, NORM2, 0.00024090821);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC1, WIDTH2, 5.7812127);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM1, 0.023887099);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH1, 0.058887659);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, NORM2, 0.00024090821);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC2, WIDTH2, 5.7812127);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM1, 0.023887099);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH1, 0.058887659);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, NORM2, 0.00024090821);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC3, WIDTH2, 5.7812127);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM1, 0.023887099);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH1, 0.058887659);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, NORM2, 0.00024090821);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::HEC4, WIDTH2, 5.7812127);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL0, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL1, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH1, 0);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, NORM2, 0.0069057353);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::HEC,  eflowCalo::FCAL2, WIDTH2, 0.43762312);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.03326799);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.082254);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.0047988219);
    binnedParameters->setShapeParam( E032point5bin, eta350bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 0.81971037);

    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL0, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL1, WIDTH2, 1.5019511);

    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM1, 0.0898856);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH1, 0.0743953);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, NORM2, 0.00602437);
    binnedParameters->setShapeParam( E032point5bin, eta450bin, eflowFirstIntRegions::FCAL,  eflowCalo::FCAL2, WIDTH2, 1.5019511);



  }

  return StatusCode::SUCCESS;

}

StatusCode eflowCellEOverPTool_mc12_HLLHC::finalize(){
  return StatusCode::SUCCESS;
}
