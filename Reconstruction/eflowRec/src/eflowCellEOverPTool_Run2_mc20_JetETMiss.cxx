/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowCellEOverPTool_Run2_mc20_JetETMiss.h"
#include "eflowRec/eflowCaloRegions.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <vector>
#include <iomanip>
#include <sstream>

eflowCellEOverPTool_Run2_mc20_JetETMiss::eflowCellEOverPTool_Run2_mc20_JetETMiss(const std::string& type,const std::string& name,const IInterface* parent) : IEFlowCellEOverPTool( type, name, parent) {

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

StatusCode eflowCellEOverPTool_Run2_mc20_JetETMiss::initialize(){

  return StatusCode::SUCCESS;
}

StatusCode eflowCellEOverPTool_Run2_mc20_JetETMiss::fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const {

  if (binnedParameters) {

    binnedParameters->initialise(m_eBinValues, m_etaBinBounds);

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.273000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.273000 );

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 2.430767);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.019476);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001233);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.013504);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.085965);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 4.496594);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.004566);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.020524);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.009038);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001967);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 2.236991);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 1.745519);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.001027);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.001037);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.078000);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.023336);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.033761);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 2.603612);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.003359);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.469000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.237000 );

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 9.999970);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.012575);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.146945);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.003791);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.003235);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 30.184499);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.091194);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.027833);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.007377);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.073173);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 2.736605);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001334);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.063979);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001277);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.523000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.287000 );

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 6.288200);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.015590);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001100);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 66.665725);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001041);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.001299);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001252);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001683);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.003140);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 98.936314);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 3.653319);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.005901);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.018458);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.033520);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 2.603238);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.004177);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 1.394501);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.031353);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000407);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.004443);
 
   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.549000 );
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.225000 );

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 9.999591);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.014740);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.005853);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.006676);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.003984);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.084472);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.001001);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.002696);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.001014);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001074);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 1.779059);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001004);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.015391);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.214663);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 9.991552);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.013909);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.003593);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 99.999578);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.107361);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.030559);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 2.603612);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.002389);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.235000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.279000 );

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 2.438569);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.019357);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001064);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.016446);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.043251);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.002664);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 99.997585);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 6.881502);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.003468);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 7.856221);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001031);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.084840);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.013118);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.005543);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.471000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.229000 );

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.025662);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.016092);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.223173);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.021308);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.023856);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.070684);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.078610);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.019270);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.003501);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.153995);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.005280);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.003416);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.556670);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.030973);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 5.000563);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.004736);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.129000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.463000 );

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.413140);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.023288);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.022379);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.064586);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 5.776477);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.001259);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.910106);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001237);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.060470);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.031593);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.001087);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001561);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.005526);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.043681);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.216729);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.015920);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.033320);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.036799);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.063710);

   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.545000 );
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.181000 );

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.023085);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.045927);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001011);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.025824);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.004092);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 99.387972);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 3.307507);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.015921);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.189000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.271000 );

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 9.999999);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.014380);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.002691);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 99.999997);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.600230);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.008613);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 4.022202);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.001118);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.107938);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 99.999930);
 
   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.471000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.198000 );

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.004041);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.003215);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 99.999709);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.024936);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.076473);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.069879);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.018465);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.188444);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.001001);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.002274);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 99.653707);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.124000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.293000 );

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 9.999994);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.014667);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.003046);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.014668);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.030000);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.048679);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 1.539492);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001015);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001004);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.026782);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.751332);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.014434);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.004719);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 3.541952);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 3.161268);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.001002);

   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.295000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.282000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.411000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.208000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.337000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.376000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.281000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.339000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.511000 );
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.243000 );

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.006739);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.058749);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 1.404471);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.001210);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 3.179413);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.001003);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.011091);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.262955);
 
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 0.000000 );

   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );

   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );

   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.283000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.220000 );

    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 9.999999);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.017010);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.001229);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 2.277783);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.004544);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 9.935270);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.001005);
 
   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.487000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.184000 );

    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.266017);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 0.029149);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.001014);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.002852);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 0.001217);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.009364);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.086317);
 
   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.378000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.244000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.133000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.511000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::Tile, 0.329000 );
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::Tile, 0.145000 );

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.184000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.343000 );

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 10.000000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.014045);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.002462);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 99.999954);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.006137);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 99.999997);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.189938);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.009373);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 9.107127);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.002950);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.002121);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.047085);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.057753);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 4.587154);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.002548);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002273);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.508011);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 2.588133);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.001288);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 3.485011);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.002931);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001407);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.314292);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.403000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.214000 );

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.051601);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.028611);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001541);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.100386);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.008164);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.096351);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.024115);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.636941);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.001012);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.330672);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.006755);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 9.999994);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.014669);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.216053);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.034541);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.024481);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.167812);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.002809);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.004557);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.054928);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.508000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.252000 );

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.367087);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.023933);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 9.999941);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.002082);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 10.000000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.004042);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.337693);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 4.432589);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.005121);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 8.386390);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001015);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.012599);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.057725);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.321556);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.003823);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 3.447576);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.026967);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000865);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.005085);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.152515);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.002521);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.325109);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.023622);

   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.540000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.232000 );

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 1.679513);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.016070);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.035670);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001133);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.204930);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.083019);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.008377);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.134367);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001002);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.166812);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001213);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.042912);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.037772);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 5.000395);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.003646);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.052966);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.036976);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.615565);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001050);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.003758);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 3.281063);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.806139);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.005890);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.134000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.347000 );

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.778557);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.021376);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.031487);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.042614);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 6.752565);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.004614);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.006383);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 14.110808);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.244558);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001086);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001984);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.005268);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.455350);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.889920);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.005388);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002383);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.491262);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 5.000500);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.002621);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 3.525535);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.008214);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.392000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.205000 );

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.038329);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.028497);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001357);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 8.344034);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.001004);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.078728);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.026757);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.001001);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.295676);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.004592);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.042584);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 9.999998);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.014316);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.258501);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001064);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.204117);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.008513);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001941);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.439000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.238000 );

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 9.999999);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.012773);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 1.341338);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001027);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 9.999999);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.004307);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001265);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 99.999955);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.309146);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.009914);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 9.935278);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001023);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.023754);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.045729);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 4.968506);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.004140);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.619038);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.033210);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 9.150053);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.002012);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 7.696045);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.001012);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.010458);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.094024);

   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.491000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.230000 );

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 4.066602);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.012120);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.007427);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.062634);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001378);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 21.496609);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 9.648884);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.004370);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.789864);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001038);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.001001);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001202);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.030306);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.044529);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 5.000182);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.003725);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.597338);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.032050);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.000959);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.004800);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.004976);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.309125);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 3.118516);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.008515);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.102000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.296000 );

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 10.000000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.013581);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.002784);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 99.999994);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 10.000000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.004258);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.006905);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 9.998708);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 2.958438);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001001);
 
 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.007652);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.487961);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 3.374657);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.003222);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.003030);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.698779);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.240164);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.006493);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.612765);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.024283);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 11.529260);

  // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.345000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.195000 );

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.054471);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.028693);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001217);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.215511);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.013448);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.008742);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.199373);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 7.784329);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.003524);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 9.999565);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001003);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001001);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.008059);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001224);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 99.999996);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001277);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.502771);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 2.014834);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001093);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.326000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.240000 );

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 5.232537);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.013112);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001403);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 99.999471);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.017640);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.020761);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.136830);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001001);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.128326);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.015140);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.036918);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.002040);
 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.422306);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.200975);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.029410);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.001291);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.001004);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.007807);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.010836);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.129898);

   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.237000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.308000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.285000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.269000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.334000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.272000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.458000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.206000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.513000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.203000 );
 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.004744);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 99.998117);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 9.999995);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.004403);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.707617);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001060);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.001010);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001001);
 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.012901);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.056064);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 1.665236);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.007408);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 9.999997);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.018484);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.142895);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002948);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 99.987661);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.782839);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.001000);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.162000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.164000 );

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );

   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.159000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.309000 );

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.010003);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 93.499854);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 9.996510);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.015051);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 10.000000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.004612);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.018126);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.008160);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 23.677518);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.001915);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 12.789462);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.037342);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.597370);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 0.001148);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.107902);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 0.041452);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 0.289829);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.012909);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.492888);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 5.000423);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.002520);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.004485);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 2.005897);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 5.000500);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.005194);

   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.346000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.192000 );

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.020011);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 0.096813);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 1.208516);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.001469);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.128495);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 0.035119);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 4.284932);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.001051);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 9.966936);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 4.766700);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.004428);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 4.899162);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.001004);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 0.004698);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 99.997261);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.021537);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 99.999929);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 0.683539);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.003573);
 
   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.317000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.208000 );

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 1.077422);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 0.014069);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 0.027958);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.110288);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.007815);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 1.267187);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.008720);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.001093);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.161361);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.032848);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.024578);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.043311);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 0.006986);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.149233);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.006124);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 99.999984);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 4.989282);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.005546);
 
   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.581000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.484000 );

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.020697);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.244566);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.001005);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.012788);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 99.999971);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 10.000000);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.004869);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 9.999969);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.004100);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 0.002462);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 94.274135);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.003438);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 99.999966);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 9.999997);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.016688);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.004602);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.637805);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 3.733852);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.004565);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.003005);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 99.999841);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 3.792563);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.003525);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.039527);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 0.033976);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 2.603612);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.002065);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.005209);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.086553);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 4.616107);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.001001);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.001014);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.111353);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 4.689173);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.019168);

   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.302000 );
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.212000 );

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.487000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.234000 );

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.095601);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.001651);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.091025);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.048199);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.098731);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.018114);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.007244);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 1.264995);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 8.234197);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.002970);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.003642);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.208605);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.022423);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.092977);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001013);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.009572);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.107290);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 9.762570);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.001001);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001355);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001343);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.004751);

  // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.530000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.210000 );

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.002796);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.158724);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001034);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.121944);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.022803);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.626360);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.122358);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.001010);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.015248);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.036568);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 1.144208);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.018899);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.005279);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.101375);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.008261);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.069660);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 2.603561);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001012);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.201784);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.002864);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.007751);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.494348);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.601000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.223000 );

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.066472);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.029317);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001231);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.223890);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.008709);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.408166);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.045154);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.021667);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.005590);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001011);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.402621);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.022837);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.007194);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.068905);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.071711);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.068068);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.002237);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 5.134756);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.060080);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.001386);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.054663);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.040063);

   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.708000 );
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.179000 );

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 1.183326);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 9.999958);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.010685);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.059175);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.011569);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 7.239940);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001189);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 9.692930);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.002953);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 9.999272);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.002896);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 8.072285);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.006613);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.115770);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.034380);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.016672);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.051619);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 2.541218);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.007074);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001382);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 64.818540);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.014927);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.142700);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.424000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.254000 );

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.077428);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.052443);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 4.991635);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.003172);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.008614);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 1.868719);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.080811);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.017227);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.285238);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.003343);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.285236);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.020486);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.117951);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001047);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.001001);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.005432);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.008933);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.121595);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001023);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.007138);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.483000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.218000 );

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001206);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 99.999964);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 4.978308);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.003990);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.417068);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.007910);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.542227);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.008180);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.010284);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.047891);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 2.764843);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.002907);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.003089);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.134976);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.022178);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.041673);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.261227);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 8.949743);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.013741);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.506000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.232000 );

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.023406);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.039224);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001778);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001436);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 99.994632);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 2.766367);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.005494);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.057447);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.021325);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.001749);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001145);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.325590);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.025282);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001004);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.212573);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 9.987774);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.001001);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.005144);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.868127);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.004660);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 1.136619);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.505806);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.001379);

   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.689000 );
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.206000 );

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.007790);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.073198);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001235);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.006413);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.049333);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 7.562952);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001099);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 9.711848);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003185);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 9.926295);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.003281);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.217300);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.026523);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.027209);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.045142);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.010199);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.061820);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.122202);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001603);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.105240);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 6.123376);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.004145);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.266000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.325000 );

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.018339);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.144472);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 2.580102);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.002483);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.010106);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.072495);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.015253);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 1.991892);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.004473);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001001);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001041);
 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.019041);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.132646);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.001001);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.007307);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.011248);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.123903);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 4.782139);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.001329);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.381000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.238000 );

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.003404);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.556652);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001347);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.395789);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.011856);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.006982);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 99.999998);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.007000);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.031690);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.002566);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.372040);
  
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.005170);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.248250);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 4.998321);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001533);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.003994);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.159817);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 4.985794);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001285);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.408000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.271000 );

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.002704);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.216438);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000444);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.003832);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 9.999997);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.004489);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.002672);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 99.999996);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.341033);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.052606);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.024013);
 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001658);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.197710);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.490846);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.021901);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.002785);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.119344);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.008433);

   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.273000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.287000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.312000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.350000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.199000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.328000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.425000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.265000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.617000 );
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.363000 );

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 5.024274);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.013206);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.963984);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001102);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.154217);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.010697);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.010796);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.008217);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.064102);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.007469);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.272299);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001013);
 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.033694);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.051011);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.570811);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.003935);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.006317);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.081636);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.000348);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.003316);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.022500);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.028216);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.003983);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.811587);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.197000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.143000 );

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.255000 );

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );

   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.287000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.324000 );

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 10.000000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.015185);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 0.015717);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 99.999999);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.017762);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 1.727980);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.014169);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 3.151321);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.015472);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 5.314128);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 9.148194);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.081391);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.129254);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 0.003587);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 99.999954);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.095426);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.094054);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 0.002715);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 7.780155);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.006762);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.021637);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.155240);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.006545);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.192284);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.002254);

   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.313000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.246000 );

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.001087);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 99.999995);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 0.006408);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 99.999998);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.147364);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 0.035073);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 1.038396);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.001034);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.006078);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 34.394326);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.006068);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.956438);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 0.099000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.060719);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 9.975438);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 0.004857);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.034751);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.069172);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 0.012922);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.726216);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.013739);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.168390);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.002025);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 99.999971);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.004122);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.354217);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.002891);

   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.357000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.378000 );

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.010673);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 99.999997);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.005741);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.009058);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 99.999997);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 10.000000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.005407);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.209161);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.023721);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.003501);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.002392);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 99.999998);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 10.000000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.017914);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 9.999995);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 0.011732);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 0.003975);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 99.999999);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.003264);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.001003);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.002850);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.583716);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 0.001003);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.010266);

   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.627000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.222000 );

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.023256);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.541900);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.001041);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.015662);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 99.999924);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 9.999846);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.004090);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.634301);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.007337);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 0.478980);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.001010);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.026082);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.091236);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 0.062477);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.001029);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.031704);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.073898);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 5.000482);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.003347);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.005036);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 0.001819);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 85.971905);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.001447);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 99.860324);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 0.797461);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.001038);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 1.745272);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.001103);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.002015);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 1.173868);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 1.025881);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.008209);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.579799);

   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.442000 );
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.232000 );
 
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.615000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.158000 );

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.959418);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.949398);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.020955);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.006044);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.138117);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.145157);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.019672);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.008349);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.050090);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.050091);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001013);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.520656);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.089397);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.037769);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002058);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 4.587958);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.005416);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001394);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 5.977657);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001262);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 41.478134);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.651000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.130000 );

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.017245);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.038559);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000323);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.004091);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.002984);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.133545);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.367415);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.014343);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.036485);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.026157);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.193698);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001001);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001017);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.004860);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.002148);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.165669);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.001001);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001108);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 14.794230);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.001614);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 7.330255);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001017);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 99.879442);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.645000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.188000 );

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.003531);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.074334);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.007690);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.002898);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.054000);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.015633);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.072888);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001021);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.063081);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.017725);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.044311);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001041);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.034515);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.049823);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.004567);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.645706);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.008304);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.299082);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000500);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.003560);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 1.820337);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.001438);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002655);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 8.046850);

   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.777000 );
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.134000 );

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 9.993636);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.010021);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 7.533029);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.009857);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.015575);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.017909);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.260165);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001098);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 2.611143);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003883);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001099);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.002074);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 2.618723);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 2.512348);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.001385);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.065237);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.070844);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.067245);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.070852);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002447);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.091175);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002506);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 99.910563);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.554000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.178000 );

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.143589);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.036820);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 4.998317);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.004586);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.101578);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.022325);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.005060);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.232099);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 2.011899);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.003979);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.136656);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.002443);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.018052);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.079904);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 5.000484);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.002483);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002168);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 2.925608);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 5.000500);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.002973);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 7.151748);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.003735);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.002843);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 4.694892);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.615000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.142000 );

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.030210);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.032689);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001001);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.002067);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.688708);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.306191);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.014877);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.003055);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.022922);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.033073);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 1.910251);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.020345);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001344);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.032671);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 7.980012);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.001373);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.014587);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.059536);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.002545);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 42.720686);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.579000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.167000 );

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.030886);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.028636);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001022);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.003249);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.069956);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.348382);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.008236);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.075634);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.019729);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 9.999912);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.002993);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.792563);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.013488);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.002534);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.945416);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.022437);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.069499);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 9.534138);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.002381);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.231944);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.019071);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002545);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 36.718512);

   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.754000 );
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.154000 );

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 8.703204);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.009255);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 9.925437);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.009519);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.145258);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 10.000000);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.004138);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.776787);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.005134);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 4.791044);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001007);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.003854);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.306154);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 1.109053);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.003636);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.002726);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 6.015761);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.186943);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.031080);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002767);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 3.729373);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.282979);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.023348);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.451000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.233000 );

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.406121);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.025305);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.002144);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.541422);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.006880);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.306851);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.082776);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.021476);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.005225);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.153201);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001910);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001430);
 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001678);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 3.800845);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.086883);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.035505);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.134354);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.033894);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 2.603609);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.003899);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.530000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.204000 );

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001698);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 99.999967);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 9.999999);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.011307);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.005061);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.609868);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.320288);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.014067);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.013241);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.052054);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.074040);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001000);
 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 3.180345);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.001151);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.008847);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.104692);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.076675);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.033763);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001401);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.009749);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.578000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.214000 );

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.016244);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.039600);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.002525);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 2.234628);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.178248);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.010807);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.044356);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.027041);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.105492);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.008341);
 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.047952);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.040021);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.236134);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.022798);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.126260);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.043070);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.002622);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 99.992166);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.004889);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.070761);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002570);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 99.992116);

   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.373000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.254000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.501000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.211000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.223000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.208000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.493000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.245000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.768000 );
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.181000 );

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.038902);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.027882);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001005);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001736);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 99.999967);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 9.999997);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.004066);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.808107);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.004759);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.428994);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.003201);

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.015987);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.067742);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.012146);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.001030);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 2.300635);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.002141);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.004260);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.754382);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002790);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 4.560108);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 5.000500);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.002755);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.199000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.156000 );

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.138000 );

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );

   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.440000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.246000 );

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.054323);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.122736);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 4.612326);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.001335);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.027094);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.407374);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.277901);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.015136);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.016194);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.276076);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.001007);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.276088);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.106292);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.082891);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 0.002210);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.114826);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.072391);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 0.001076);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 99.999908);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.019699);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.107451);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.798783);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.002133);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.006755);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.113984);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 0.134437);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.002275);

   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.519000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.204000 );

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.005897);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.002739);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.012821);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 1.009297);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.012633);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.041072);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 0.059396);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.534015);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.001293);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 6.509264);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.004510);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 0.006285);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 1.339435);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.011769);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.242875);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 8.616767);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.001049);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.007404);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.193004);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 4.999678);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 0.003942);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.004607);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.145131);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 3.103577);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.001214);

   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.601000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.186000 );

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.005469);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 99.999997);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 2.603612);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.001262);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.076998);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 0.025337);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 0.341389);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.001056);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.001641);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.253012);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.021706);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.001278);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 50.438961);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 1.908726);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.024092);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.001144);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 99.479796);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 6.656292);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.017379);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.147695);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.001564);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 0.001415);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.639515);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.001816);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.253513);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 0.123292);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.001003);

   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.670000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.184000 );
 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.018818);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.192877);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 4.999770);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.003117);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.008917);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 0.555832);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 9.999999);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.004553);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.001376);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 4.647306);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 1.285813);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.005951);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.073197);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.051993);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 5.000110);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.004051);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.436686);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.028461);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.446283);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.009662);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.068060);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 5.000455);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.003748);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.002555);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 0.123018);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 5.000500);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.002978);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.001743);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 99.999995);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 1.380677);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.006552);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.013262);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.057710);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.002584);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 40.894479);

   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.768000 );
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.374000 );

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.663000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.125000 );

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.269303);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.586517);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.022032);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.111745);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.026135);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 7.803818);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.001237);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.014335);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.032928);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.032928);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.002286);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 99.999997);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.038282);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.004304);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.422028);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.030513);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.002543);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 92.928348);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 4.672934);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.001975);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.002588);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 17.795902);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.689000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.108000 );

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.019597);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.035677);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001647);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.014648);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.041013);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 1.111566);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.010243);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.047644);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.023774);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.002267);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.003695);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.003688);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.002170);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 26.949702);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.002591);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 18.485218);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 1.892166);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.021760);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.002859);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 2.784650);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 9.852343);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.001013);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.654000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.180000 );

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.008535);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.014105);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.066201);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.020604);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001289);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.064557);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.125320);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.010779);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.229285);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.008485);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.021709);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.023850);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.003584);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.113260);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.003124);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 1.562327);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001563);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 9.803591);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.001419);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.838233);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.018799);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.066216);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.001025);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.006902);

   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.808000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.112000 );

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.189258);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.016772);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.096207);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.002584);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.004487);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.022827);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 1.855126);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.004442);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 9.428742);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003229);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 5.894943);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001053);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 9.242651);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.001458);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.015248);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.175864);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 9.600356);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.014073);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.003081);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 1.676657);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.004184);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.732942);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 9.455019);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.001092);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.607000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.148000 );

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.150836);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.033419);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 4.998857);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.003083);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.004207);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.128867);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.140448);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.020499);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.010532);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.042886);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001486);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.042886);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.003813);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.310686);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 9.795510);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001008);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.315571);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.032842);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.002712);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 6.711595);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 4.687345);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.006131);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.354634);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.095895);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.661000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.123000 );

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.034351);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.031062);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001098);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.715748);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.011540);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.008253);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.058472);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.032391);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.028678);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.002550);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001011);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.002007);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 99.999992);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 3.478562);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.005990);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001001);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 2.092171);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001847);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 5.352856);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.521257);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.041138);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001027);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.040976);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.598000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.143000 );

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 6.189764);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.009903);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.101774);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.196724);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.001914);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.045428);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.018477);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.118711);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.007572);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.097073);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.018099);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.062396);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.043010);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.002611);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 25.422810);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.002698);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 6.344252);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.010262);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.002521);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 99.999958);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002666);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.065415);

   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.783000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.129000 );

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.001010);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.011203);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.009199);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.034102);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.097099);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 8.868777);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.003971);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.002987);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.016887);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 5.493593);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.003485);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 9.999990);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.016320);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.005822);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.511316);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.042331);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.047703);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.002529);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 99.996715);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002719);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 5.350882);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 5.000500);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.001020);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.519000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.186000 );

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.620263);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.274729);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.027149);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.084661);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.025747);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.002615);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.486694);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.007680);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.070830);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001023);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001230);
 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.102893);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.036588);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.296037);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.003640);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.014266);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.006633);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.001642);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.002651);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 14.814823);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.496891);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.001157);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.593000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.178000 );

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.018669);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.053426);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 1.551799);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001124);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.009469);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.027796);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 2.481208);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.007659);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.016974);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.041657);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 9.716181);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001023);

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 1.761857);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.001319);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001860);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 73.675478);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 1.621444);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.021651);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002862);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 5.504599);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.644000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.180000 );

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.009460);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.041837);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 4.974932);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.005195);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.183749);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.010360);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.002248);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.200407);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001001);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.098324);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.094615);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.019456);

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001235);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 37.328767);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001363);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 17.213711);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.069202);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.046437);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.002521);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 99.999996);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 7.151882);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.011710);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002595);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 25.330115);

   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.479000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.223000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.567000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.168000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.156000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.776000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.464000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.275000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.808000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.138000 );

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.007671);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.038971);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 4.998823);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.004911);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 9.999995);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.003921);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.001202);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.051883);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 6.835044);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001246);
 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.139381);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.034008);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.007330);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.002288);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.002920);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 2.953121);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.487573);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001122);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001671);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 8.220018);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.001001);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 5.846253);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.152000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.196000 );

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 );

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );

   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.520000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.208000 );

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.084902);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.080190);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 8.646328);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.002909);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.237822);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.017814);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.028008);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.202180);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.917601);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.004016);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.019603);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.142499);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.001214);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 99.999956);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 0.092069);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 0.071923);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.078110);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.072750);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 5.000500);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 0.001002);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.062558);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.029047);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.014982);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.097182);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.484499);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 8.718815);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.014550);

   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.605000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.163000 );

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.004912);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 30.386770);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.001653);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.009944);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 99.999711);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 0.746891);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.014480);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.003079);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 100.000000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.077853);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.031522);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 0.019778);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.112026);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 0.500860);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 0.001001);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.013039);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.121652);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 1.180371);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.002691);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.004939);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.166796);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.001105);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 0.007824);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.009993);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.074535);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 5.000465);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.003672);

   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.648000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.141000 );

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.003637);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 99.999999);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 9.999978);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.010428);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.650791);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 0.009916);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 0.004116);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 100.000000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.001264);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 32.781116);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.298861);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.020843);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.762495);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.027957);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 0.005481);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.126781);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.009804);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 0.089427);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 0.224461);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.025311);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.003111);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.111979);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 5.000500);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.001108);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.136257);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 2.603612);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.001675);

   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.708000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.150000 );

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.017519);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.121638);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 4.995824);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.002555);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.108575);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 0.015822);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 8.337360);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.067050);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.015684);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 0.241780);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.001093);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.396562);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 10.000000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.017455);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.226077);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.033162);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.208990);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.018929);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.030808);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 0.001917);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.133770);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.001000);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 0.229065);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 0.289959);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.008953);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.646217);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.033163);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.003089);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 2.834923);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.001033);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 3.320640);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.001636);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 13.815686);

   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.689000 );
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.170000 );

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.033994);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.036875);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 9.644620);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.018503);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001756);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.482457);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.002786);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.479715);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.720000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.100000 );

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.301532);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.024591);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.023731);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.015116);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.019861);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.029226);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001427);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.034140);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.071207);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.002538);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 99.999706);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.041077);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.039638);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.002548);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 35.460579);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.269076);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.073942);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 9.905240);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.004515);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.730000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.091000 );

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.051310);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.026206);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000509);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.003431);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.064083);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.014725);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.017642);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.034863);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 2.014469);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.021478);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.002518);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 97.238945);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.002568);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 17.534745);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 6.034970);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001511);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.001007);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.003962);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.172651);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.082769);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.686000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.127000 );

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.001001);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.062341);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 9.984000);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.003082);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.006710);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.302407);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 4.990905);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.006909);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.146984);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.067505);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 9.372147);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.001579);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.029578);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.085566);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.081459);

   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.837000 );
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.095000 );

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.001001);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.067335);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.642782);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.006397);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.002701);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 11.162148);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.589260);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.001371);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001062);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 4.694980);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.001561);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 97.032829);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.022889);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.009159);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.238365);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.672000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.117000 );

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.177316);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.027814);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.678425);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.014947);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.001937);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.421012);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.273890);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.016497);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.017734);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.033945);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001298);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001587);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.002221);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 5.437005);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 2.779751);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.005544);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002826);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 5.250932);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 7.944746);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.004052);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.002519);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 99.999996);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.055551);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.038186);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.708000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.102000 );

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.003597);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.105871);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.075228);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.005311);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.010130);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.048620);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 1.216017);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.009991);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.041755);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.026472);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.006759);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001003);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.002731);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 7.769395);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.087556);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.002521);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.013363);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.053023);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002534);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 86.178251);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.029159);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.162048);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001182);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.006866);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.638000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.110000 );

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.008788);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.027344);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 1.402353);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.005146);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.016938);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.024535);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 9.990707);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.004391);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.403481);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.009735);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.005259);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.146446);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.002850);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 3.749896);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 5.818119);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.017346);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.012977);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.002668);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 7.437279);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.001730);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.035225);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002562);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 24.323185);

   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME1, 0.000000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME2, 0.000000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME3, 0.000000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::HEC, 0.000000 );

   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.814000 );
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.117000 );

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.004325);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.035127);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 9.925321);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001568);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.019094);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.013930);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 8.703927);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001133);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.001027);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.041066);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.067419);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.007395);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.038992);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.050449);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.002767);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 5.292287);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.037949);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.038115);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.002528);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 85.616916);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 9.149870);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.001208);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002516);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 99.985066);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.615000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.152000 );

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.098399);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.035269);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.088973);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.003715);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.002987);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.207596);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.172817);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.018818);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.011472);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.047247);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.004570);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001258);
 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 1.965546);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.005878);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.001758);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 99.999950);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.093104);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.035509);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.002813);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 5.357077);

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.669000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.143000 );

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.076228);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.029026);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.002410);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.005015);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.104162);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.478915);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.013015);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.022779);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.033704);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.009310);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001040);
 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.006453);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001955);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 99.999998);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 9.999998);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.015491);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002727);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 8.486332);

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.712000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.137000 );

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.019396);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.027512);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001275);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.005617);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.151050);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.011754);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001085);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 99.999735);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001043);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.076970);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.099705);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.018901);
 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.002695);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 9.281623);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001009);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.032382);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001320);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 5.847504);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.001321);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 62.293865);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.002625);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 12.368721);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 5.135934);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.001004);

   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.563000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.147000 );

   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.611000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.136000 );

   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.515000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.184000 );

   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.567000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.164000 );
 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.002137);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 9.487579);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 0.007012);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.003737);

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.005135);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.439020);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 4.998936);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.007617);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.007938);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.002728);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 4.313281);

   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.844000 );
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.113000 );

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.009854);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.027365);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.001225);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.005615);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 10.000000);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.003801);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.210361);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.349375);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003829);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 1.340910);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.004152);
 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.003447);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 1.228406);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 9.935302);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.001402);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.002626);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 10.911167);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 1.329153);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001039);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.068138);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002529);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 36.343235);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.500000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 1.399000 );

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.453000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.900000 );

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 );

   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.616000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.153000 );

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.559397);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.029609);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 0.006886);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.255880);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.080006);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.057972);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 4.206131);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.005655);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.001001);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 7.878205);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.036152);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.050509);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.045270);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.077617);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 8.153364);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 0.001534);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.048651);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.063716);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 9.964085);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 0.001498);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.001957);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 2.025674);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.180947);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.001604);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.001232);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.436038);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 4.464788);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.002093);

   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.686000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.121000 );

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.002765);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 96.916459);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 5.388649);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.012435);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.958385);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 0.014474);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 0.007656);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.403792);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 12.675464);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.102760);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.031298);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 0.012725);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.105259);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 9.005891);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 0.016722);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 7.554586);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.016778);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 0.001602);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.659725);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.006889);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.035984);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.212439);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 0.024585);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.024553);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.038399);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 7.252921);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.001077);

   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.704000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.107000 );

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.002457);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 0.620740);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 4.262954);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.011403);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.464084);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 0.010832);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 0.005526);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.116619);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.002134);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.130555);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.390957);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.018844);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.023817);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.102510);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 7.855708);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.001544);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.007818);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 0.026886);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 0.749985);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.019840);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.001000);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.328307);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 0.001001);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.021503);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.010430);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.181473);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 0.988467);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.002295);

   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.745000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.118000 );

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.029541);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.056737);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.002742);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.011031);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 0.061865);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 10.000000);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.004392);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.006232);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.037522);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 9.470616);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.003724);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.059278);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.026898);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 0.746029);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.020737);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 1.441068);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.011407);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 0.003415);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 1.281537);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 9.999985);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.014349);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 0.003445);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 99.699145);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.066529);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 0.037743);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 0.003602);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 5.584295);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.002946);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 2.843216);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 9.221966);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.001019);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.002588);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 17.088930);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 7.440280);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.002661);

   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.747000 );
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.146000 );
 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1, 0.007465);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1, 0.232192);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2, 0.152786);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2, 0.011250);
 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.028154);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.098673);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 5.000500);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.002265);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001462);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.586208);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.001774);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 8.541391);

  }

  return StatusCode::SUCCESS;

}

StatusCode eflowCellEOverPTool_Run2_mc20_JetETMiss::finalize(){
  return StatusCode::SUCCESS;
}
