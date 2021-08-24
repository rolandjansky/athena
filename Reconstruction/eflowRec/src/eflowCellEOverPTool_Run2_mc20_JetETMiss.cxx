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
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.290000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.257000 ); 

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 2.446577); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.019418); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001250); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.020094); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.057990); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.723511); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.001057); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001007); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001002); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.002619); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 1.702063); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.085074); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 5.000479); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.005173); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.193224); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 7.896759); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.001088); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.482000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.225000 ); 

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 1.324637); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.015594); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 1.876394); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.015597); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.092584); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.026771); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 3.461270); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.005606); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.007052); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.076370); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001014); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001628); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.062922); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.285005); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.007335); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.315000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.617000 ); 

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.129007); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.041575); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001020); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.101844); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.001641); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001420); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001033); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.008482); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.783793); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.744857); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.007799); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.866554); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.001634); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001030); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.107782); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.258793); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.033142); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.001459); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.004636); 

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
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.535000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.220000 ); 

    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 6.587813); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.001017); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.012502); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.235692); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.008155); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.053314); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 1.678407); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001004); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 9.964648); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.028034); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001001); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.031113); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.002679); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.200828); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 1.573345); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.037925); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.001001); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.037923); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.005732); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.013064); 
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.084577); 

    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.253000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.258000 ); 

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 5.499133); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.014471); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.012797); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.010830); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.330626); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.001514); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.933923); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.006547); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.429315); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.003509); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.002774); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.298813); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.008149); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.033713); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.002490); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.491000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.211000 ); 

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 8.835754); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.012692); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 1.260255); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001239); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.205926); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.011456); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.035495); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.059887); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 4.531897); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 1.086954); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.004747); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 2.050890); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.031757); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.006021); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001083); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.140000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.464000 ); 

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.002269); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 4.573904); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.141908); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.015561); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.002436); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.483306); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 9.505370); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001036); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.055825); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.032470); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001559); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.007637); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.041825); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.302490); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.002658); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.045018); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.061722); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.040157); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.061720); 

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
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.525000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.190000 ); 

    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.029212); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.041655); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001005); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.041969); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.003778); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 4.240306); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.032252); 
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.016305); 

    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.232000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.238000 ); 

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 1.038619); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.022489); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.327043); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.012919); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.018382); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.066425); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.296359); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.001623); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001719); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.662741); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 9.967463); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001595); 
 
   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.500000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.186000 ); 

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001350); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 2.239640); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.780719); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.015478); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.100723); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.025176); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.029669); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.011605); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.002280); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 99.997840); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001001); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001680); 
 
   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.193000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.250000 ); 

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 7.843699); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.015209); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.168934); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.003862); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.012936); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.027372); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.051126); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001039); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.018534); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.823946); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.014868); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.012774); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.175652); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.004115); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.311000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0.284000 ); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.426000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0.207000 ); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.369000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.320000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.300000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.303000 ); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.513000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.222000 ); 

    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 3.267479); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.001129); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.002216); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.111912); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.007918); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.477716); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.002208); 
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.004257); 
  
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
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.305000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.216000 ); 

    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 2.820678); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.021690); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.002859); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.007439); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.344034); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 9.931210); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.001023); 
 
   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.520000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.179000 ); 

    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.037505); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 0.063358); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 9.073921); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.002672); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.005679); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 0.925271); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 4.518778); 
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.003261); 
 
   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.377000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.234000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.090000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.413000 ); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::Tile, 0.337000 ); 
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::Tile, 0.444000 ); 

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.217000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.333000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 10.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.013889); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.002953); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 99.999990); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.006784); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.546243); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.007596); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001066); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 99.960684); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001442); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 87.743746); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.003932); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.545799); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001015); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002001); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.621504); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.002329); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.011655); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.540316); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.403000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.208000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 9.964538); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.011879); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 3.851601); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.002240); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.268281); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.013861); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.003947); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 1.072499); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.026885); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.001694); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.010547); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.040194); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.209259); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 10.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.014758); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 9.869793); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.012345); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.166847); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.001099); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.007001); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.054770); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.493000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.255000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.060171); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.044320); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.849846); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.007262); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.754943); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.002087); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.062718); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.015137); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.044678); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.028523); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 8.569525); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001003); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.013244); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.056430); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.002385); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.008060); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.812130); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.031069); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000806); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.004984); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.097549); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.064706); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.006211); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.064811); 

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
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.512000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.240000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 9.999926); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.013427); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.012907); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.044817); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.014925); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.001032); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 99.999977); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 2.482619); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001090); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.017504); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001030); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.042588); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.037926); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 4.992684); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.004123); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.375028); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.032511); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.000564); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.004797); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.004900); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 16.158274); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 4.877537); 
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.012963); 
 
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.170000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.335000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.270047); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.030707); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.003429); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001437); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.001010); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 12.263096); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.006860); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 54.027830); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.779540); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 3.026108); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.003970); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.004499); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.566310); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 1.027607); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001579); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.001766); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.958013); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.003276); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 9.999999); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.013145); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 10.636892); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.393000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.201000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.058578); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.028593); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001003); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.167322); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.015773); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.004058); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 99.999957); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.005920); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.056401); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001119); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001654); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.006264); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.083329); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 3.794558); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001100); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 9.999571); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.010839); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.217891); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.434000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.238000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 1.007222); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.016964); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.008837); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.071477); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.941680); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.006963); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001840); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 99.999964); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.046823); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.025051); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 1.991191); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001060); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.008868); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.144395); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.412337); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.032736); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.284985); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.032735); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 4.236937); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.004916); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.003428); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 99.999425); 

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
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.461000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.232000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 3.630877); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.004580); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.031369); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.058352); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.093061); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.013503); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.003094); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 99.999496); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.790566); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001151); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.001005); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001001); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.026230); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.046633); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 4.993968); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.004058); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.064699); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.037429); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.002222); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001664); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 99.994933); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.001830); 
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 75.961435); 

    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.144000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.281000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 10.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.013484); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.003341); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.007628); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 46.949434); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.591100); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.006830); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001020); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001264); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.002539); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 99.999877); 
 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.004672); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.865442); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 1.274098); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.005331); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002208); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 1.153762); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.001003); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.007711); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 1.238304); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 9.514157); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.002724); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.358000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.189000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 9.999846); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.010152); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.001464); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 99.999762); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.052279); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.037778); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 8.135835); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.002295); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.001698); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.006401); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.003748); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.100878); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.001579); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.650695); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001545); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.081831); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.020832); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.611536); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.318000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.249000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 6.723543); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.013566); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.993188); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001663); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.640366); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.007897); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 7.632974); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001045); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.033118); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.036560); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.001031); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001844); 
 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.499880); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 9.946612); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.010572); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001065); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.210674); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 7.171958); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.007144); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.814454); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.001946); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.003043); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 82.782311); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.259000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.301000 ); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.308000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.266000 ); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.352000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.220000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.439000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.207000 ); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.477000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.226000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 3.892185); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.012692); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.015383); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.065147); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.007263); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.173381); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.219376); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.010167); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.001151); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001537); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001049); 

    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.010116); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.062339); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 5.000464); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.004199); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001001); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.137850); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 9.705167); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.018290); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001109); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.006277); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.011156); 
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.087209); 

    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.104000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.717000 ); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 ); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.257000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.266000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.006921); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 99.284686); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 0.009259); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 99.704471); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.671964); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.011241); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 8.446922); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.001185); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.012518); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 92.563476); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 12.959511); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.027609); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.987936); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 5.579495); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 0.004901); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.001002); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.008488); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 0.038607); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 0.306646); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.012760); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.522656); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 5.000398); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.001155); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.009718); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 0.005473); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 1.067358); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.363000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.192000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 5.078582); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 0.013146); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 0.003084); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 99.077191); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.014354); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 99.998429); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 0.426457); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.014005); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.105292); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 0.011791); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 3.947468); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 0.001001); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.001486); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 0.005795); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 99.999950); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 7.767542); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.016807); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 0.024647); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 8.054774); 
 
   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.325000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.193000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.136430); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 0.032461); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 0.009915); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 3.540163); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.008945); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 99.999670); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 1.107203); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.009004); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.152915); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.033932); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.018067); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.001172); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.003072); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 9.999990); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.014613); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.010509); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 99.704930); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 4.842924); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.002115); 
 
   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.527000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.509000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.043101); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.260208); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 2.392621); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.001004); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.016648); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 99.206114); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 30.731107); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 9.398115); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.002464); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 8.605643); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.004388); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.004540); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 99.999992); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 10.000000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.016562); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.046593); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.064238); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.002031); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.004604); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 99.999919); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 4.930890); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.007629); 
 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 1.188064); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.017790); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.413145); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.001201); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.108564); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 8.549124); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.017810); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.308000 ); 
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.221000 ); 

    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.006428); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.004277); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.041203); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.003187); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.030224); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.004042); 
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.614790); 

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.481000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.241000 ); 

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.003334); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 91.541959); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 9.999544); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.014005); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.123162); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.015984); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.008538); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.708195); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.013727); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.008725); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.004094); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.181196); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.019589); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.100167); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 2.199989); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001108); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.009845); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.105474); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 4.876092); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.003726); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001348); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 23.455307); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 2.253462); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.001039); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.526000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.208000 ); 

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001781); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 1.289954); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000496); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.003029); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.374650); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.013378); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.003171); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 99.999975); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.014054); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.038953); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.019307); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.259108); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.024011); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.004850); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.104271); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 10.000000); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.014068); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.271167); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 4.313391); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.001116); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.004347); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.998163); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.606000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.228000 ); 

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.069069); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.029186); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001225); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.024592); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.020916); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.142942); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.001111); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.040813); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.022144); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 9.838330); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.001545); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 9.195264); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.015786); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.205146); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001001); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.011938); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.004146); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.792007); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.001037); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.036407); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.169826); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.028025); 

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
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.693000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.192000 ); 

    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.052470); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.029270); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.002755); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 99.997610); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.304908); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.007410); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 1.507310); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003905); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 5.060294); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001033); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.517814); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.024120); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.136727); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.015112); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.052662); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.559229); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001067); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 9.554688); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.003067); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.017925); 
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.068461); 

    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.419000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.255000 ); 

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.057176); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.062561); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 4.987522); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.003127); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.009561); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 1.325428); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.118675); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.014708); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 1.061117); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.004113); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001001); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.023429); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.108059); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001024); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.008433); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.126943); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.001001); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.009002); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001180); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.001165); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.478000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.212000 ); 

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001110); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 99.999999); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 10.000000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.009949); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.033967); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.001483); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.634671); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.011962); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.016852); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.006830); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.009813); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.050590); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.009882); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.084030); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 5.000135); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.003705); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.272551); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 1.321067); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.017364); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.516000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.242000 ); 

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.234411); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.020678); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001006); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001542); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 66.908098); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.333047); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.008246); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.080517); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.054732); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.019180); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 9.983733); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.001001); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.028844); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.050454); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.010702); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.288925); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.003759); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 6.582444); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.006423); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.004479); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.865289); 

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
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.674000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.233000 ); 

    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.026334); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.043112); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001014); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 10.000000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.004352); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.002250); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.038411); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.002174); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 9.999997); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.003465); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.437880); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.025119); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.002521); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.097879); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.007977); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.065851); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 2.603611); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.003177); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 7.824800); 
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.003939); 

    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.259000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.328000 ); 

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.015570); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.190012); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 4.967387); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.002017); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.247997); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.009258); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.140776); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.009486); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 1.791458); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001150); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.005330); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.335802); 

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001296); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.004664); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.018505); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.134541); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.014055); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.079053); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 100.000000); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.376000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.234000 ); 

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.003271); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 99.999976); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001899); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.473128); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.011172); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.007849); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.009027); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.016598); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.012690); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.032560); 
  
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.005676); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.234043); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 5.000474); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.004844); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 7.964484); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 10.000000); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.014721); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.415000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.272000 ); 

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.003016); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.240722); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 4.915510); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.005338); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.003041); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 5.460788); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.004875); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.242325); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.044349); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.025958); 
 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.005993); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.096955); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 4.995722); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.001104); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.002298); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.160659); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.024226); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 30.451591); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.281000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.280000 ); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.322000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.337000 ); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.245000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.331000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.414000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.267000 ); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.577000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.444000 ); 

    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.008117); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.097124); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001429); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 6.513350); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.001138); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.009023); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.122521); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 1.337967); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003971); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 9.999262); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.001012); 
 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.028788); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.053422); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.023341); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.002987); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001464); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.177870); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 4.994834); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.015547); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.003606); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 1.342108); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 2.118679); 
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.001001); 
 
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB1, -0.020000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.803000 ); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 ); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.298000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.323000 ); 

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.028973); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.490699); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 5.005885); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.004763); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.066995); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.062428); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.571451); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.006282); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.017687); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 2.164644); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.001356); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.083423); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.127783); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 0.003692); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 99.999999); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.096298); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.094359); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 0.002854); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 5.371498); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.001073); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.022655); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.155589); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.006603); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.201580); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.001904); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.328000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.237000 ); 

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.008807); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.001003); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.017745); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 1.019628); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.011289); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.010892); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 99.999997); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.038457); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.014869); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 7.378083); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.003725); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 8.315309); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 0.004294); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.022227); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.387491); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 0.748352); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.003238); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.018439); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.099911); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.004103); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 1.644639); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.004534); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.350209); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.001062); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.340000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.378000 ); 

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.011705); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 99.997485); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 5.037423); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.005668); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.010227); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 99.999985); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 10.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.005371); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.220165); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.023249); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.004064); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.047333); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.072231); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 0.513098); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.001746); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 6.656953); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 0.003960); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 58.548334); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 6.874627); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.018323); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 7.182159); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.003033); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.002816); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 1.139916); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 9.764915); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.001135); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.630000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.219000 ); 

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.026011); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.465622); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 5.000469); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.002968); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.018232); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 0.473478); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.001114); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 10.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.004127); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 0.002922); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.003034); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 94.336706); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 9.999367); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.017459); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.126953); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.043665); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 8.329061); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.001222); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.002233); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 9.556035); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.001047); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.034922); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 0.005855); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 0.001449); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 99.999994); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 9.999750); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.013665); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.001624); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 99.999958); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.005615); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.018529); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.518959); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.410000 ); 
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.278000 ); 

    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.068111); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.037411); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 6.128595); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.019720); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.002726); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.231641); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.026204); 
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.073142); 
 
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.614000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.157000 ); 

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.253101); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.030080); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 2.604286); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001881); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.005961); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.148329); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.152460); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.019256); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.009771); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.049253); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 4.363376); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001042); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 2.298605); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 1.117365); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.022095); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002626); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.006858); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.001806); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 99.999999); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001011); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 9.244718); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.001564); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 76.767286); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.648000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.131000 ); 

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.020485); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.037657); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000300); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.004137); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.375384); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.014264); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.002795); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.162103); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.035110); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.027166); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001521); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001003); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.002870); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.476272); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.008893); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001745); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.828783); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.013882); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 4.189321); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.016854); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.001645); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 4.871739); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001046); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 97.892275); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.659000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.186000 ); 

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.001006); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.072541); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 4.729800); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.011298); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.092814); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.012024); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.001009); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.145195); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.046734); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.019195); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.002964); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001057); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 99.999997); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.001035); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 99.999992); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.002552); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 99.999957); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.358289); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.028855); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 9.460934); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.001424); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.003413); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 1.451787); 

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
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.773000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.137000 ); 

    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 1.216898); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.013892); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.002471); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001820); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.067412); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.688738); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.005717); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 9.500283); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003084); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 9.983400); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.002967); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.044790); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.035602); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.015368); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.052091); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001009); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.006201); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.026800); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.112365); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001571); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.153400); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002645); 
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 4.867387); 

    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.551000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.179000 ); 

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.134539); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.037987); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.035551); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.054576); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 8.616823); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.001007); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.008145); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.066217); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 9.961613); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001021); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.024539); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.071037); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 1.094840); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.001456); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.102884); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.033749); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 5.000433); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.004125); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.131285); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.036471); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.002531); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 99.999996); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.611000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.143000 ); 

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.007811); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.061359); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.002028); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 1.293390); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.302839); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.014944); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.022826); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.033074); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001141); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.033074); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 9.910777); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.001022); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.005480); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.139341); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.004685); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.084751); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.452321); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001064); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.015552); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.059042); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.002525); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 97.846919); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.585000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.166000 ); 

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.011434); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.039704); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001352); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.043581); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.003510); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.063339); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.244161); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.008942); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.068408); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.019247); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.002451); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.019245); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.002303); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 83.206742); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 1.507426); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.002180); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.003475); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 1.526507); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 9.999979); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.015682); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.002519); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 99.999877); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 6.510326); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.012179); 

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
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.747000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.157000 ); 

    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.035693); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.027098); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.001046); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001450); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.204901); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 2.939545); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.004859); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 2.644177); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.003822); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 8.825468); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.003225); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.003712); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.306463); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 2.610560); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.002571); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.062223); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.035513); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.569376); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001105); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002528); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 99.999995); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.055010); 
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.040738); 

    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.451000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.234000 ); 

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.073735); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.048558); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.550478); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.003779); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.074069); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.013712); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 7.851513); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.005751); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.354685); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.006497); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.269763); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001067); 
 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.001770); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 99.361172); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 6.289951); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.016899); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.135734); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.033777); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.003553); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.526000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.207000 ); 

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.001898); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 99.999984); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 9.999774); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.011355); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.380864); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.013258); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.005704); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.620142); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 1.743201); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.005163); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.111315); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001018); 
 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.606507); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.022759); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001130); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 97.135019); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.028284); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.044094); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 2.915825); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 0.001014); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.582000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.221000 ); 

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.033288); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.032016); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001104); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 2.807756); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.001135); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 9.477274); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.002720); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.042932); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.027099); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 7.323266); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.003456); 
  
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.001866); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 2.467506); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.154805); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.001004); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.400817); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.027816); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.003047); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 3.049018); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.002535); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 99.999973); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.380053); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.017566); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.375000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.268000 ); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.497000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.211000 ); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.117000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.440000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.499000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.196000 ); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.766000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.210000 ); 

    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.042444); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.029911); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 5.000471); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.004696); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.007333); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.067793); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.586916); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001356); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.485554); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.004599); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.009311); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.014158); 
 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.120188); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.033946); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.002996); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.003175); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 1.762781); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.068303); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001037); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.323318); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.002128); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.003544); 
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 1.341659); 
 
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.164000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.359000 ); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.619000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.265000 ); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.437000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.251000 ); 

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.585058); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.031474); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 2.388221); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.016751); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.040531); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.223727); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 9.999968); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.005188); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.044828); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.001067); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.017990); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.288550); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.113908); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.080568); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 0.002384); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.132668); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.067472); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 0.001465); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 99.999982); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.447291); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.027333); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.083361); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.001418); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.114752); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 0.005505); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.114752); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.504000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.212000 ); 

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.006471); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 99.876106); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.001369); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.013645); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 99.999997); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 0.977981); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.012527); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.057665); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 0.028120); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.006687); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 99.999994); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 9.623809); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.016558); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 0.005056); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 99.874015); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.007238); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.442801); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 1.307748); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.023872); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 1.460046); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.010233); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 0.122594); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.116155); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.034339); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 7.522509); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.004133); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.594000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.190000 ); 

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.007639); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 3.388207); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.001419); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.006476); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 99.999998); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 1.601904); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.007647); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.241842); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.021727); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.001891); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 99.999963); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 7.960697); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.019282); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 0.001382); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 90.850188); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.014272); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 0.094969); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 2.484595); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.001005); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.438470); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.438471); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 7.162678); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.003585); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 13.871242); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.663000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.186000 ); 

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.012356); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.356942); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 10.000000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.012720); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.010905); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 0.381756); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 2.647940); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.005710); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.235208); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.009832); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 2.403148); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.001001); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.074744); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.052141); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.001097); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 8.811448); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.004575); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 0.060391); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.051287); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.006836); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.090237); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.008660); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.042198); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 0.036915); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 5.000494); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.004103); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.001791); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 80.377104); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.381683); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.001035); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.003702); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 1.173323); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.001007); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.616000 ); 
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.232000 ); 

    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.037328); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.082406); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.018100); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.022213); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.001646); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 4.660990); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.001031); 
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 17.970202); 

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.662000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.127000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.244111); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.028475); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.019549); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001336); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.002710); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.176415); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.203341); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.018636); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.009967); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.032262); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.006399); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.032262); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.002327); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 99.999986); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.007964); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.001061); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 21.154526); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.001505); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 50.383879); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.006954); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.069407); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.002527); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 66.071828); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.688000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.107000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 1.460267); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.001016); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 0.019356); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.036602); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.019307); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.036813); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 1.207164); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.009910); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.050716); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.023352); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.002030); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001178); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.865122); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.001560); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.024812); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.057378); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 7.947572); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.019394); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002596); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 16.995399); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.002720); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 6.437331); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.297146); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.019359); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.673000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.169000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.003776); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.047577); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 5.000497); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.003729); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.097120); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.158671); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.010573); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 3.715287); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.001029); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 3.648908); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.004925); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.004134); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.667713); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.001221); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001515); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 5.014192); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.001110); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 81.490805); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.118002); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.080136); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.764312); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.006331); 

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
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.804000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.112000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.096593); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 9.999660); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.009287); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.050746); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.009698); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.058636); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 1.247108); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001142); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 1.801868); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.004004); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.008480); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.262310); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 4.995174); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.008890); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.004754); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.601687); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.003139); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 9.915804); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.002354); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002539); 
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 36.520971); 
 
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.606000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.148000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.003230); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.129325); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.557457); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.022346); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.004235); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.148902); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.143790); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.020267); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.012447); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.042504); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001113); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001313); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.002637); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.369713); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.020431); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.040412); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 9.286530); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.002563); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.330219); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.007170); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001680); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.062678); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.002587); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 19.864796); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.659000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.124000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.038676); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.030969); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001200); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.008725); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.057461); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.709063); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.011518); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.034682); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.028024); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001870); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001115); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.083425); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.037322); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.297537); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.003033); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.046642); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.048346); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002536); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 90.171872); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.034190); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.122822); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001069); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.006224); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.605000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.140000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 9.977722); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.009647); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.096716); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.055535); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.017139); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.530675); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.002271); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.984139); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.004713); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 1.493923); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.006276); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.083315); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.042186); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.002588); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 99.999934); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.001002); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.009827); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.002943); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 2.874926); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 9.921821); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.003460); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002954); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 2.751640); 

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
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.774000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.129000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 9.051114); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.009683); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.001003); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.151961); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.001400); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.096492); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.563901); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.005879); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.135920); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001072); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.681575); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.004978); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.631652); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.001001); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.019597); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.144486); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.002719); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 5.492531); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 8.722934); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.002231); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.129991); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.063710); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.052999); 
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.002073); 
 
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.517000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.191000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.103429); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.038509); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 1.255206); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.001358); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.003063); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.412408); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.088522); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.024870); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.001378); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.001395); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.007919); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.070474); 
 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.140234); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.033978); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 3.356644); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.003120); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.001644); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 4.191155); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 7.894382); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.001511); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.060098); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.169270); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 1.434548); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.001376); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.590000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.180000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.019420); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.054885); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000499); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001332); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.320328); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.014912); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.003419); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.538893); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.018165); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.040341); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 6.839946); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.001015); 
 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 9.685652); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.002214); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.001991); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 2.347644); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 1.759216); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.023181); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002834); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 6.174283); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.647000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.187000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.021692); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.032130); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 3.761539); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.002809); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.159455); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.010896); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.002599); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.164485); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.072391); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.067050); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.021891); 
 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 6.793023); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.005515); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.002058); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.003211); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 2.162491); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 4.524975); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.001003); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 9.547322); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.001135); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.002647); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 11.727332); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.477000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.234000 ); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.562000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.168000 ); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.690000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.630000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.492000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.183000 ); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.796000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.155000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.085426); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.022248); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.065298); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.002098); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 9.999999); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.003988); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 0.002964); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.107659); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.027739); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 2.873022); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.003760); 

    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.012197); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.176857); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 1.146187); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.006047); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 9.999747); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.017086); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.002638); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 12.845368); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.002629); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 10.507915); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 1.119325); 
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.002936); 
 
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.264000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.311000 ); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.000000 ); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.516000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.214000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.090004); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.078934); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 0.363700); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.003283); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.031523); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.190332); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.286234); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 0.015835); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.299289); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.001194); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.020807); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.143124); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.001304); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 99.999992); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 0.096742); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 0.070976); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.080032); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.072651); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 0.003041); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.185396); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.038593); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 5.628981); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.012230); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.005220); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 0.102026); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.001003); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.595000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.169000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.005494); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 29.477597); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.001027); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.830783); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 0.013805); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 0.011067); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 100.000000); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.003507); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 99.999999); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.076576); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.031424); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 0.002466); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 83.275522); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 5.481673); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 0.019303); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.013813); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.129156); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 5.143076); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.006806); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.007829); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.006877); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 0.144043); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.007666); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.082949); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 5.000480); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.002723); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.643000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.145000 ); 

    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.007201); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 0.341223); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 4.992306); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 0.004636); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.004233); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 99.999998); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 0.481754); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.010924); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.231549); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.023785); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 7.428223); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.001200); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.695611); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.028567); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 0.004817); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.135510); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.026047); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 0.064859); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 0.740026); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.001151); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.001200); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.027607); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 0.001009); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 0.331128); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.104202); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 0.001010); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.017800); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.704000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.154000 ); 
 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.022044); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.110814); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 2.603611); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 0.001100); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.018145); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 0.072414); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 9.258798); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.001046); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.139750); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.010721); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 1.099554); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.053242); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.051545); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 2.603063); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.002245); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.002187); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.149029); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 0.296415); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.029487); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.008527); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 0.003929); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 0.097225); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.003913); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 99.053705); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 0.135664); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.037699); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.002651); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 25.113634); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.031765); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.063748); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.002836); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 3.523240); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 9.999993); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 0.014392); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.674000 ); 
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.159000 ); 
 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.015528); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.128636); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 2.603608); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.001347); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.002992); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 1.686615); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001018); 

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.720000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.101000 ); 

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 9.621588); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.011595); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.081666); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.031117); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.022242); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.027758); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001152); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001307); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.002317); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 99.999979); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.001650); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.005812); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 1.217878); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.009427); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.002546); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 37.909825); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.001103); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.009690); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.003945); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 0.918385); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.731000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.090000 ); 

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.034634); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.029401); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 1.579379); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.005726); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.005556); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.425851); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 2.539937); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.020611); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 0.002682); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 10.015268); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.017929); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.043807); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002522); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 99.999753); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.068172); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.118196); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 9.874029); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.006873); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.699000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.125000 ); 

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.005432); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.033520); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 9.926631); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.001458); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.003151); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 94.935444); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 7.930215); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.015428); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 2.177232); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.002804); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 6.134930); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.005367); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.001060); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.007578); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.104410); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.064109); 

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
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.834000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.097000 ); 

    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.001332); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.005412); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.082448); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.001933); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 1.698483); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.001023); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 20.413293); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 1.367449); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.037268); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.059967); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 0.001399); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.026572); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.666168); 
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 0.059632); 

    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.670000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.120000 ); 

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.121189); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.032384); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 0.317125); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.014114); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.299956); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 0.015585); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.003605); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.133926); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.018216); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.033532); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.004062); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.108363); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.001854); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.004590); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.189021); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 0.027534); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.002643); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 12.088633); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1, 0.015754); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1, 0.047710); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2, 0.002523); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2, 99.999789); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.706000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.103000 ); 

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.039319); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.029706); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 5.000500); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.002886); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.007731); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.057437); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 1.134807); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.010230); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.044143); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.025737); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001004); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.022272); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.003232); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 1.855644); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 8.947357); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.002321); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 0.001561); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 7.068955); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.001064); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 98.111398); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1, 0.044293); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1, 0.089451); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2, 0.001209); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2, 0.008969); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.644000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.113000 ); 

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.020668); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.027529); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.001109); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.005107); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.315089); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.161476); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.011034); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.021296); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.001554); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.157370); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.015197); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.003022); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 2.319849); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 2.491011); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 0.001339); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.002662); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 7.419835); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 5.000043); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 0.001357); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 0.033603); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 0.130982); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.106938); 

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
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.810000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.117000 ); 

    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.134163); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.002667); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.001279); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.118513); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.008049); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.022683); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 9.971042); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001053); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 9.630627); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001710); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.026586); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.011333); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.016523); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.048136); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.003211); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 1.954361); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 9.999482); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.014686); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.002659); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 8.961668); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 8.168753); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.001301); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002634); 
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 10.825952); 

    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.614000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.156000 ); 

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1, 0.039954); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1, 0.044802); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2, 3.909014); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2, 0.012595); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1, 0.001656); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1, 45.266452); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2, 0.163382); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2, 0.019694); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1, 0.011913); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1, 0.046600); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2, 0.001013); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2, 0.001211); 
  
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1, 0.080848); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1, 0.036271); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2, 0.550793); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2, 0.013699); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1, 0.002757); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1, 6.951871); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2, 0.007636); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2, 0.016210); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.667000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.146000 ); 

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1, 0.079647); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1, 0.028914); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2, 2.603612); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2, 0.001006); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1, 0.004941); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1, 0.108903); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2, 0.451953); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2, 0.013254); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1, 0.021613); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1, 0.033719); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2, 0.001104); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2, 0.033718); 
  
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1, 0.027000); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1, 0.052362); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2, 9.579203); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2, 0.001421); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1, 1.753503); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1, 0.001509); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2, 0.002808); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2, 5.880170); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.712000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.137000 ); 

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1, 0.021663); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1, 0.027857); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2, 0.002809); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2, 0.006942); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1, 0.001278); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1, 0.879657); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2, 0.150857); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2, 0.011873); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1, 0.014171); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1, 0.028872); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2, 0.077614); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2, 0.017948); 
  
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1, 0.003147); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1, 0.075402); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2, 0.002546); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2, 73.906799); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1, 0.143733); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1, 0.004411); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2, 0.003044); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2, 2.605900); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1, 0.002715); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1, 8.289059); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2, 9.991365); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2, 0.001866); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.563000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.149000 ); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.612000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.137000 ); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.530000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.292000 ); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.564000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.157000 ); 
 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.047693); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.009586); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 0.002148); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 70.617457); 
  
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.106526); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 0.034112); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.003254); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 1.775075); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.541560); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.001045); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.002972); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 2.131045); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.838000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.120000 ); 

    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1, 0.015841); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1, 0.027465); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2, 0.001260); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2, 0.005615); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1, 0.004784); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1, 0.044480); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2, 9.890726); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2, 0.001104); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1, 0.001470); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2, 0.019538); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2, 0.012638); 
  
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.156699); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.036326); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 0.002549); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 54.482931); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 8.616195); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.001617); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.003154); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 2.080181); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1, 8.880205); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1, 0.001110); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2, 0.002557); 
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2, 27.003956); 
 
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.397000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.339000 ); 

   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.485000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 1.107000 ); 

   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.000000 ); 

   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.612000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.158000 ); 

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1, 0.810747); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1, 0.026007); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2, 0.010520); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2, 0.196029); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1, 0.402383); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1, 0.010723); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2, 0.008004); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2, 47.263874); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1, 0.037315); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1, 0.047095); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2, 0.002101); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2, 0.462630); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1, 0.045861); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1, 0.077908); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2, 8.838704); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2, 0.003611); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1, 0.008501); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1, 0.130328); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2, 1.006891); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2, 0.023550); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1, 0.103844); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1, 0.032962); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2, 0.329051); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2, 0.021037); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1, 1.364261); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2, 2.295031); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2, 0.002806); 

   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.679000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.122000 ); 

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1, 0.003208); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1, 95.252123); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2, 9.185140); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2, 0.011650); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1, 0.007325); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1, 0.585826); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2, 0.800710); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2, 0.015157); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1, 0.001263); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1, 100.000000); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2, 0.104631); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2, 0.030491); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1, 0.051349); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1, 0.062603); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2, 4.907116); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2, 0.001836); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1, 0.264174); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1, 0.029305); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2, 0.001549); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2, 0.508465); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1, 0.035806); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1, 0.001965); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2, 0.025613); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2, 0.041927); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1, 0.395217); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2, 0.001001); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2, 0.019775); 

   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.700000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.110000 ); 

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1, 0.034552); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1, 0.029913); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2, 0.002092); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2, 8.275290); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1, 0.005996); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1, 0.121052); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2, 0.432114); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2, 0.011250); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1, 0.001248); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1, 0.252649); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2, 0.355443); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2, 0.019459); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1, 0.259467); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1, 0.029212); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2, 0.037755); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2, 0.061485); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1, 0.024440); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1, 0.026888); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2, 0.755296); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2, 0.018722); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1, 0.103133); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1, 0.040182); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2, 0.004327); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2, 99.983547); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1, 0.120719); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2, 0.017174); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2, 0.120706); 

   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.742000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.120000 ); 
 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1, 0.209343); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1, 0.026776); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2, 0.001370); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2, 1.010897); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1, 0.014600); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1, 0.055872); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2, 8.010873); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2, 0.004626); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1, 0.003428); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1, 0.048828); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2, 0.073294); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2, 0.011685); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1, 0.044119); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1, 0.051113); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2, 0.001462); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2, 0.002866); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1, 0.001000); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1, 0.026884); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2, 0.753042); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2, 0.019471); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1, 1.885812); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1, 0.030225); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2, 0.003657); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2, 30.027306); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1, 0.003815); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1, 2.542250); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2, 9.862159); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2, 0.001122); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1, 0.002590); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1, 99.959390); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2, 0.014795); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2, 0.058988); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1, 0.010409); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1, 0.050250); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2, 0.002524); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2, 99.999977); 

   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.729000 ); 
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.150000 ); 

    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1, 0.003690); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1, 0.035739); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2, 0.002495); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2, 94.760899); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1, 0.053859); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1, 0.043472); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2, 0.005978); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2, 0.198402); 
 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1, 0.011764); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1, 0.185154); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2, 5.000490); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2, 0.005158); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1, 0.354097); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1, 0.003720); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2, 0.002601); 
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2, 10.177629); 

  }

  return StatusCode::SUCCESS;

}

StatusCode eflowCellEOverPTool_Run2_mc20_JetETMiss::finalize(){
  return StatusCode::SUCCESS;
}
