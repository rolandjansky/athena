/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowCellEOverPTool_mc12_LC.h"
#include "eflowRec/eflowCaloRegions.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <vector>
#include <iomanip>
#include <sstream>

//This file was generated using mhodgkin/EOverP-00-00-19 with the following datasets
//user.mhodgkin.mc12_8TeV.159038.e1189_s1620_s1622_r3586.17289_Tag98_LC.D3PD/
//user.mhodgkin.mc12_8TeV.159039.e1189_s1620_s1622_r3586.17289_Tag98_LC.D3PD/

eflowCellEOverPTool_mc12_LC::eflowCellEOverPTool_mc12_LC(const std::string& type,const std::string& name,const IInterface* parent) : IEFlowCellEOverPTool( type, name, parent){

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

StatusCode eflowCellEOverPTool_mc12_LC::fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const {

  if (binnedParameters) {

    binnedParameters->initialise(m_eBinValues, m_etaBinBounds);

    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.582043);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.343399);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.39465);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0244867);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,4.26601e-11);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0435449);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.33795);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0152147);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,2.87558e-13);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0338559);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0386784);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.019673);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0186772);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0246639);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00399361);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0319538);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.000547714);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0854776);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.981199);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.393636);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.535344);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0198332);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.409683);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0268075);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.18695);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0114342);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,3.16983);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0136975);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.190183);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0182798);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,7.29358e-15);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.627597);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0135579);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0268979);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0106032);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0442046);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.817743);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.675904);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.611042);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0207795);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.420639);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0313947);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.457047);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0123802);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.248573);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0153758);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.619562);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0151591);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.459442);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0219468);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,1.03143);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0234085);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.884079);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0307887);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0460047);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0299152);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,5.09368e-15);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0530811);


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
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::Tile, 1.19143);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.486002);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00704939);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.01856);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00525966);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0279302);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0329743);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.00770702);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0247265);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0154886);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.31889);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.030532);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,1.12028e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0493486);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.117289);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0331059);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,6.3341e-11);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.053531);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0027281);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0502694);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00179492);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0786374);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.577237);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.344763);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.5503);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0221223);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.94154e-15);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0428795);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.00011);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.016568);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,1.85896e-05);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,100);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.024218);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0206603);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.011038);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0264528);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00278217);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0315106);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,7.81598e-12);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0479642);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 1.06769);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.402081);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.420683);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0186422);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.349366);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.024362);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.11277);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0115236);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,2.4956);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0137487);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.133849);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0157849);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0800792);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0202823);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0101868);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0262833);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,2.19367e-07);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,99.8902);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.721499);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.785911);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.508065);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0213691);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.370324);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0287579);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.542555);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0120308);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.308643);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.014825);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.816818);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0205613);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.000314484);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.176203);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.531766);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0248214);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.38182);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0476576);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0180512);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0336179);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0112151);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0553665);


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
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.999775);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.201304);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.784643);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0250578);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.607177);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.034151);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0864048);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0396251);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,7.92514e-10);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0552568);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0028131);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0361671);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00216698);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0509037);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.573727);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.410339);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.13857);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0176965);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.64983);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0249139);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.559509);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0151693);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.255966);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0178699);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0188589);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0164434);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0114558);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0212072);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.000121021);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.449998);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,5.11776e-06);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,97.0088);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 1.14084);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.421118);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.408743);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0188354);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.314616);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0248986);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.123973);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0190914);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0623296);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0238021);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.00364356);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.031454);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00256731);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0412727);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.319381);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.260891);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,1.05282);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0172656);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.787772);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0249179);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.103851);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0280221);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,5.73418e-11);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0483709);


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
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::HEC, 1.26667);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.664841);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::Tile, 1.16279);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.47727);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00389074);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0434899);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0031338);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0532939);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 1.33643);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 1.3973);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.932003);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.484441);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.00244076);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.45);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.000283359);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,99.8091);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.00187093);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.19007);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,8.84934e-07);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,20.8385);


   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME2, 1.54446);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.427444);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.0826533);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0253918);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.0572609);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0547601);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,248.047);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.00716482);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.0894722);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0202004);


   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME3, 1.42011);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.712736);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::HEC, 1.711);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.941968);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::Tile, 1.3875);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::Tile, 0.219018);


    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.473697);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.444885);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.9039);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0164804);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,2.04401);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0406294);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.23773);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.005643);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.742213);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0384487);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0529172);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0425556);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,7.05567e-05);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0454742);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0573265);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0713687);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0244785);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0775106);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00677838);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0670298);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.002596);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0718018);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00251746);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0593521);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00207422);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0697783);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.816984);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.290098);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.434511);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.025068);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.255998);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0386595);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.60335);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0156971);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.000505354);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.105093);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.155683);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0240233);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,6.02275e-07);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,99.9899);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0478347);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0471223);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,6.6436e-11);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.061796);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00308987);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0495831);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00193891);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0544383);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.998848);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.416059);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.198715);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0405523);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.74257e-13);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0608753);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.746797);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0120003);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0196908);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0484828);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.635254);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0101092);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.417525);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0255781);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,1.12089);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0252909);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.836164);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0443519);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0782613);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.020925);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.06147);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0474379);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00615317);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0456599);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00493373);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0540083);


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
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.906475);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.309916);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0633824);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0580257);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0101206);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0607185);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.29323);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0103733);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0109421);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0614261);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0323851);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0136789);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00300811);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0459225);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.964007);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.02808);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.684995);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0499039);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.117487);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0404998);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0666702);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0511737);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0126482);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0494653);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00962883);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0637608);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.412662);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.522697);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.04367);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0165567);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.21428);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0432811);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.979149);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0148061);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.223335);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.049115);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0375185);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0105161);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0255891);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0542522);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0376214);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0844919);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00902791);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0902767);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00310907);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0608095);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00130383);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0652033);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.000637909);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.074839);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.000365957);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0849055);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.876044);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.327918);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.335608);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0159786);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.250512);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0288603);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.46946);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.014842);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,7.31108e-12);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0232359);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.136821);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0216187);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,5.27316e-16);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.038817);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.036505);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0436167);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,4.86345e-17);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.058813);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00258027);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0467528);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00102624);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0533154);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.971651);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.386165);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.172456);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0209052);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.116047);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0373247);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.701397);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0133376);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00195099);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0813467);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.745237);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0206889);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.000666896);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0825283);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.706427);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0312002);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.494115);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0463245);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0390009);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.037663);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0239005);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0505766);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00194514);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0727399);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.000982216);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0862439);


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
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.902152);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.277857);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0406259);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0701053);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,2.77285e-06);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.070117);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.193773);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0109015);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00746428);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0623762);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0357917);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0129786);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00454346);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0493134);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.696786);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0335672);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.451527);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0505828);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0699624);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0390015);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0390423);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0592916);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00675756);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0416848);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0048751);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.081046);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.261618);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.536245);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.58958);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0240894);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.941985);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0426793);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.8096);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0234851);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0174947);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0607153);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.035487);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0132398);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0193551);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0462482);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0146164);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.062382);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00517766);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0815012);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.000865089);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0804215);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000280809);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0882708);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.891535);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.348838);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.314845);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.017066);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.238462);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0271714);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.95585);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0110355);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,3.15309);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0134016);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.153586);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0196871);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0713805);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0246891);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0136192);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0415849);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00905608);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0478241);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.000900096);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0527432);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.000533564);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0583759);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.957746);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.412045);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.323433);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0228881);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.20729);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0399561);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.778153);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0140275);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00125307);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0917548);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,1.09039);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.021146);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.00262044);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0751219);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.114199);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0562641);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,2.65691e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,15.5387);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.261254);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0421067);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,2.19512e-11);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0597289);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00870524);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0521786);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00448513);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0594476);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.000566614);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0651862);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.000342382);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0764578);


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
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 1.21813);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.497226);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 1.02084);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.387078);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0262238);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.044278);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00333113);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0459984);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.11826);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.011835);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00183206);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0589);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0390212);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0138678);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0128221);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0358343);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.00437085);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.140849);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,3.42293e-08);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,10.4638);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00257072);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.45);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.000361771);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,100);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.28995);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0397918);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.165926);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0524469);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0796114);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.040339);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0503216);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0480352);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0110723);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0485698);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00856193);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0615126);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.834378);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.470151);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,5157.97);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0112755);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.282581);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0210128);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.00534322);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0192622);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.00393363);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0272145);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,292.74);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.00716482);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.105595);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0202004);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.000776022);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0359493);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.000545594);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0453526);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.856896);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.428855);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,7.50505);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0170197);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,5.31903);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0347678);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,4.97486);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0193234);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.148254);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.063161);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.134426);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0443905);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,5.2575e-10);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0623441);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.074125);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0815321);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.0242763);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0892333);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.029934);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0815776);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.0114244);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0895638);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.00313629);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.175271);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.000839608);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.224153);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.00328524);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.107696);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,1.02639e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.108833);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM1,0.000648026);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH1,0.0940022);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM2,0.000224479);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH2,0.102139);


   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 1.18607);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.389879);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.14887);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0167855);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.821292);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0342192);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,23.1775);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0172558);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.0532631);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.103701);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.91985);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0245396);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.0148954);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.123041);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.174089);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0754541);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.041277);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0813936);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.0393301);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.112549);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.0123739);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.123306);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.00485162);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.118121);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.000922636);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.132307);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.000627505);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0718199);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.000266101);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0855114);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM1,0.00133838);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH1,0.0512138);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM2,0.00088116);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH2,0.0610775);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, NORM1,0.026069);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, WIDTH1,0.0310321);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, NORM2,0.000361453);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, WIDTH2,0.0406066);


   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 1.24931);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.551226);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.478404);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0223808);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.0742311);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.084464);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,6.32331);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0120169);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,3.11308);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0161489);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,11.5645);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0213586);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.00113823);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.18096);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.44279);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0239456);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.70145);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0520041);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.180189);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0778288);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.0489296);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0823544);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.00848228);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0926187);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.00216838);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.10023);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0022454);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.200503);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.000340638);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.228923);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM1,0.0104961);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH1,0.0194324);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM2,0.000268056);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH2,0.188761);


   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 1.53128);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.675774);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.215552);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0640863);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.0840226);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0683223);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.673106);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0807689);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.000210064);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0807772);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.272917);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0717894);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,4.09066e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.071807);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,4.59355);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0202263);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,3.75686);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0687338);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.34258);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0789779);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.795433);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0824721);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.473774);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0540965);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.219602);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.06047);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.102123);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0468863);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.00267664);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.681627);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.126135);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0289773);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.077758);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0685592);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0662765);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0371076);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0224223);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0664901);


   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 1.09654);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.359323);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.112803);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0469187);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0621787);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0599094);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.274362);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0481588);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.165013);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0573304);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,2.68257);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.188921);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.00119313);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.192312);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0412566);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0358286);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0306762);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0433906);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0030337);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0614175);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00219682);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0764381);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.805954);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.210869);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.70209);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0217939);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,2.10079);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0404436);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.92406);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0320002);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.371207);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0333827);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0969665);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0399421);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,2.69304e-11);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0589181);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.538848);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.045679);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.303903);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.053266);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0758553);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0410411);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0525126);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0518764);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0153692);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0448682);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0112312);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0540302);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.869822);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.206983);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.420165);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0349496);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,3.6686e-10);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.056545);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.25838);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0188342);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.00558005);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0575453);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.185072);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0210207);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0656937);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0318749);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.202194);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0418203);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.12005);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0511787);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0213059);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0480023);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,1.0821e-11);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0614445);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00537737);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0445942);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00350742);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.055247);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 1.04273);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.281411);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.191548);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0152232);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.14742);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0425554);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.884153);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.00846456);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.424773);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.019411);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.540121);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0123825);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.236469);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0266407);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.963282);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0286817);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.695253);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0409625);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.100315);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0308703);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0700654);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0416767);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00833857);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0435532);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00643871);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0531126);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.999084);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.200221);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.213974);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0485393);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.24738e-09);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0623149);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.425892);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.00964805);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.199241);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0395941);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0554471);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0202484);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0233564);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0398015);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.20854);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0204444);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.660922);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0441904);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.190752);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.031636);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.135742);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0433735);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0193435);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0475527);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,1.72742e-12);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0619836);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.822914);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.234562);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.37684);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0249697);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.13487);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0427598);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.2495);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0331686);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.000115249);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.2407);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0632238);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0434985);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,9.92964e-14);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0610892);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.190976);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0517477);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.104873);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.058832);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.029564);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0463191);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0195549);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0536321);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0023864);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.051614);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00170831);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.060103);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.894441);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.215906);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.281706);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0256815);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.169455);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0395225);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.12983);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.011633);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.11767);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0237984);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.177855);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0279078);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.000126892);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0956931);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.131247);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0451581);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0774198);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0530854);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0122929);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0491713);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,7.83157e-12);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0626437);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00146729);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0477129);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.0009179);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0568046);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.995669);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.243755);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.136938);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.034037);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0586866);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0463767);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.743681);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0110902);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0901877);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0335883);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.702725);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0106945);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.482503);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.022901);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.616271);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0334536);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.405956);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0498945);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0466107);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0408615);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0268226);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0508994);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00327247);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0460956);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00224465);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0603896);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::HEC, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::HEC, 0);




   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::Tile, 1.00214);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.371571);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.100874);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0488858);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,7.93951e-10);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0624738);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.377134);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0115299);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0337754);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0479642);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0711687);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0158628);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0030764);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0546156);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.913888);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0264254);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.699215);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0401853);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0946054);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0444752);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.81318e-11);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0620192);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00646555);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0385917);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00528842);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0475629);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.806748);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.294017);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.5588);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0205688);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.01894);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0437091);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.14797);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0267681);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.165224);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0437899);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0509432);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.043527);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0082672);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0512934);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0478254);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0524597);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,4.01672e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,92.1229);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0817438);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0398385);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,3.20456e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0595489);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,48.1512);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.0218793);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,4.7851e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,99.9242);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0322065);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0618252);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0196181);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0698282);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00424844);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0669953);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00303174);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0760802);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0012099);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0802702);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,1.17759e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0807182);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.882676);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.265746);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.316372);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0235877);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.176544);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0403177);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.41764);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.015496);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0749601);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0419425);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.168106);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0230894);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0536919);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0356068);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0246894);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.06323);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,1.37364e-06);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0682467);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0025709);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.14159);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,3.59725e-07);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.141631);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0367445);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0561717);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.01886);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0625237);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0020691);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0674708);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.000956794);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0749272);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.888685);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.305093);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.163461);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.038549);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,5.09112e-12);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0574977);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.769471);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0121546);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0824324);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0457475);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.923977);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0106642);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.651814);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0228015);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0741465);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0689584);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,3.52935e-06);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0691452);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.00306864);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.015);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.00206745);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,99.9999);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,10.8179);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0314511);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,0.00663032);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0623397);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.375055);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.034466);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.255563);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0487951);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0221825);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0552713);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0114799);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0617721);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00274974);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.060006);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00204851);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0700409);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.977332);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.342661);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.27864);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0491567);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0708295);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0539091);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.204544);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0572661);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0973845);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0658143);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.214462);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0409333);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.149778);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.05384);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.16137);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0508505);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,9.78298e-06);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0532361);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.381757);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0284886);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.226836);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0717011);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.026353);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0528393);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0136113);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0585091);


   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.980601);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.337341);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.104055);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0506865);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00167808);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0516077);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.20847);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0128353);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0860417);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0436007);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0464672);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0430111);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.000686377);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0703348);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.112479);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0556193);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0498855);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.063536);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0183155);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0424258);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0127403);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0528068);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.398303);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0313794);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.258599);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0601284);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0691787);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0356193);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0421131);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.055306);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00922677);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0449384);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00707565);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.068328);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.185898);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.256079);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0151839);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0282128);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.000970005);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,99.9592);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00349825);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.018141);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00300664);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0760595);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,2.23388);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.0250813);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,0.00138715);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.0612348);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00478584);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0658807);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00238825);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0780128);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.82398);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.327281);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,7.38164);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0200861);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,4.46767);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0413378);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,7.93219);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0288747);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.958269);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0490113);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.326398);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0479655);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.0384348);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0496766);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.556781);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0611745);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.188605);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0676115);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.381277);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0645263);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.144341);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0730719);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.0512992);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.055051);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.027333);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0683915);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0176231);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0436025);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0113963);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0547487);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM1,0.0885245);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH1,0.0617427);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM2,0.0454335);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH2,0.0686088);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, NORM1,0.0315463);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, WIDTH1,0.061096);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, NORM2,4.33553e-09);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, WIDTH2,0.0618978);


   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.930968);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.287571);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.0227);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.021288);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.47516);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0462928);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,24.2508);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0182788);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.664668);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0530064);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.34531);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0270451);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.180576);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0589439);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.630848);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0463658);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.149979);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0941878);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.29922);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0663241);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.142206);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0740592);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0295745);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0659374);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.0129717);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0889299);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.00606463);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0858954);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,2.37948e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,91.1379);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM1,0.00256659);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH1,0.0713335);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM2,0.00160837);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH2,0.0825776);


   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME3, 1.07235);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.363204);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.409967);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0532112);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.0307665);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0551652);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,6.28725);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.015383);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.212723);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.070893);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,9.66215);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0209367);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.0900337);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0699711);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,3.64139);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0283978);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.539);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0535344);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.930518);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0482575);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.35022);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0830261);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.0829703);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0738107);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.000263321);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0750283);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0155345);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0524228);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.00834217);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0649126);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM1,0.0198396);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH1,0.0524221);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM2,0.011974);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH2,0.0577392);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM1,0.0138055);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH1,0.0479254);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM2,7.72093e-12);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH2,0.0633643);


   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::HEC, 1.14045);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.408307);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0152971);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0837083);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00902208);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.115351);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.450549);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0480612);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,1.66851e-12);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0627679);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.7544);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0600729);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.00252655);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0929951);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.364575);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0176944);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.205154);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0589144);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,4.31443);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0247965);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.90298);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0593982);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,4.20665);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0334495);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.09765);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0694505);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.397585);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0392318);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.158188);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0798646);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.0918992);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0670526);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,4.45962e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,39.3538);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0755632);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0565619);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0298018);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0619028);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.01254);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.06642);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,6.9837e-07);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.06768);


   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.893841);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.259204);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00800006);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0700712);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00554453);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0850833);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.303974);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0483229);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.158849);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0570757);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.628689);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0361188);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.422374);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0482567);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.770481);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0303593);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.585041);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0382782);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,0.00458129);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.402703);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,2.04936e-14);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,73.1042);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0495783);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0502458);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0266416);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0619064);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00110561);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0704183);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,4.65025e-05);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0723744);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.885113);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.135815);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,4.12934);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0243356);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.628339);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0469245);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,3.23253);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0222954);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.134503);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.043924);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.191489);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0206038);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0979237);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0372563);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.847106);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0248729);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.580887);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0491886);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.126561);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.024815);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.105089);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0446412);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0206683);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0402022);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0141358);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0505638);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.954878);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.131922);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.671191);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0239881);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.219387);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0409012);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.7977);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0164884);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0761466);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0393137);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.292439);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0200352);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0645101);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0348283);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.516809);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.035607);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.333316);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0455506);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0617572);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.026659);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0435979);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0451819);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00800433);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0417837);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00560788);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0511102);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 1.02992);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.180607);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.246924);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0151626);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.200737);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0368541);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.924773);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.015763);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0178247);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.045478);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.45477);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0130805);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.132822);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0280082);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.872081);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0242051);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.677868);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0394322);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.135132);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0239083);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.10744);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0385101);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0141029);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0358015);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0103603);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.049013);


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
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::Tile, 1.02389);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.246234);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.156128);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0464337);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.33532e-09);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0608905);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.401315);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.012389);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0479314);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0452289);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0786711);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0162675);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0231591);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0390967);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,1.03184);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0216291);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.79634);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0417648);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.182913);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0296085);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.137643);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0417042);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0282645);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0377521);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0204258);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0467715);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.898259);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.145855);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.81881);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0265419);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.371873);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0506281);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.28582);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.024817);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0370052);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0545895);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.170189);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0270462);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0411166);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0451255);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.42721);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0273177);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.278172);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0543604);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0454846);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0278472);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0318657);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0518405);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00338886);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.050492);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.002285);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0628482);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.949559);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.136046);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.335613);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0230088);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.145619);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0431661);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.24477);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0156985);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0580658);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0404366);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.228776);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.021243);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0575949);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0359689);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.221679);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.043757);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.122273);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0519778);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0200937);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.038974);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0136494);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0500258);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00182285);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0480145);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00118282);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0558185);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 1.00229);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.305011);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.194713);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0178563);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.139304);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0447963);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.759918);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.016251);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0259097);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0458043);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.635981);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0186374);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0169996);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0460144);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.563851);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0334752);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.377337);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0450811);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0678627);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0330147);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0454912);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0431898);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00290717);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0474831);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00202673);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0616033);


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
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::Tile, 1.02805);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.281257);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.119938);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.040877);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,3.51233e-11);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.051873);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.183676);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0139318);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0413125);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0416021);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0720255);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.01679);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0131776);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0419708);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.872777);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0247272);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.661201);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0400088);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.105028);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0259652);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0798182);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0412308);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00702177);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0452507);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00433942);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0556503);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.861481);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.198084);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.80794);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0227327);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.815699);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0444328);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.25124);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0280591);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0238015);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0633964);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0936038);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0308091);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0341319);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0468274);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0775696);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0454836);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,2.62945e-10);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0606343);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0100199);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0612329);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.000911558);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0644624);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0959122);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0542867);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0388171);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0603921);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00736856);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0636694);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00395374);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0715445);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.929429);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.175418);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.378173);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.019554);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.269985);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0419765);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.59627);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0150054);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0794081);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0454366);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.214034);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0249281);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.057444);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0390755);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,1.90748);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0141681);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.0998726);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.063348);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.182487);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.00942106);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.00669857);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0873853);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0983667);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0515067);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0448551);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.058208);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00760105);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0560825);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0050611);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0638575);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.992268);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.239893);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.145952);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0508573);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0156549);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0531799);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.548279);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0139766);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0701022);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0458849);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.746524);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0131586);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.312016);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0269801);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0156226);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0807305);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0119379);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0988942);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.420868);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0428792);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,1.04597e-08);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0588145);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,3.30076e+09);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.0076678);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,1.86266);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0400668);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.430275);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0331504);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.306114);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0483849);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0277482);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0452528);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0186533);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0634383);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00158655);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0621844);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00117389);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0707475);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::HEC, 1.03564);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.267892);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.14472);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0601833);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.000213199);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,99.6837);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.234695);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0506047);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.141947);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0647084);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0898673);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0529305);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0653914);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0681854);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.195841);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0519053);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,2.088e-05);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0523322);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.33476);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0415727);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.235654);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0533854);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.034967);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.041125);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.024008);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0540776);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::Tile, 1.08197);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.296413);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.144909);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0447053);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,5.8983e-12);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0608071);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.19375);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0177673);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0789042);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0415166);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0616427);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0176171);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0175653);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0478069);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.121793);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0392531);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0617075);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0690199);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0250957);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0592565);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0135849);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0726398);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.00782795);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0722678);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,7.02984e-10);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0818944);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.441384);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0252848);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.335199);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0458927);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0784529);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0290422);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0435402);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0519075);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0103184);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0367753);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00800372);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0593336);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.514981);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 2.47178);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.742185);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 1.06039);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.835291);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.235856);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,6.30925);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0211922);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,3.02166);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0448112);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,9.41262);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0227142);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,2.24138);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0459301);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.606255);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0415533);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,8.50847e-12);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0589103);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.901918);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.029151);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.748454);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0618868);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.776148);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0479837);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.355044);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.060867);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.0822194);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.055295);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0256341);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0617267);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0220989);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0417092);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.00803887);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.074811);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM1,0.0451476);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH1,0.0609966);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM2,0.0303056);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH2,0.0687488);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, NORM1,0.0695234);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, WIDTH1,0.0306515);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, NORM2,6.17752e-13);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, WIDTH2,0.0611561);


   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.934087);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.229527);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.08611);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0161867);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,1.02894);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0449757);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,17.8714);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.020432);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.964862);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.051365);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,1.89011);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0253709);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.274891);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.05365);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.58398);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0384143);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.698494);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0704371);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.757204);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0526637);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.360516);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0727719);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0708097);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.06141);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.0269209);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0679465);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0125695);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0625823);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.00567782);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0746372);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM1,0.00906263);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH1,0.0577332);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM2,0.00738816);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH2,0.0675406);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, NORM1,0.173344);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, WIDTH1,0.0197312);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, NORM2,1.7972e-09);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, WIDTH2,0.0586947);


   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME3, 1.03882);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.32079);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.541793);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0482201);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,2.76838e-10);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0630333);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,3.87048);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0161531);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,1.31677);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0446077);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,7.57114);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0206863);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.125587);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0620786);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,3.86106);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0338054);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.57231);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.050691);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.73721);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0378722);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.964773);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0601872);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.130103);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0518518);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0618453);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0596387);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.022309);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.045651);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0153848);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.052375);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM1,0.0522403);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH1,0.0642986);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM2,0.0089372);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH2,0.0682574);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM1,0.0105055);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH1,0.0663123);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM2,7.6671e-07);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH2,0.0679749);


   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::HEC, 1.04235);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.333185);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.217426);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0415366);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,2.61523e-10);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.059068);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.401905);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0509893);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.142097);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0556459);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.943049);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0116461);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.692113);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0523511);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.453348);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0178908);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.136789);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0549464);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,3.33785);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0269919);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.4088);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0515301);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.78287);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0298166);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.6699);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0527667);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.464052);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0273462);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.336409);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0562411);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.10949);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0419655);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0580419);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0606498);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.10294);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0491515);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0565888);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0551085);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0303983);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0304106);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0284788);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0541929);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.937842);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.320194);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,1.07476);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0474129);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.000125406);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,43.5139);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,1.40978);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.024202);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,1.01014);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0443742);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.802376);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0256193);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.616624);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0388294);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,0.00128333);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.449987);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,1.45426e-06);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,99.2849);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0734569);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0444581);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0485952);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0511243);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00358896);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0582674);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0023147);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0716785);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.926065);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.101858);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,4.05961);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0243872);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.412865);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0506153);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,6.3606);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0189404);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.101874);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0489336);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.228768);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0197329);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.102177);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0385016);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.974871);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0245282);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.594409);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0489896);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.112919);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0261393);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0630629);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0476456);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0224825);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0362228);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0145921);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0491314);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.967183);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.105601);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.830576);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0257869);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0932898);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0500355);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.82374);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.014841);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0374097);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0475673);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.284704);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0188279);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0827268);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0342445);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.722243);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0255516);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.564768);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0447385);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0935695);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0248756);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0729518);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0420824);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.0110666);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0473195);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,5.04773e-10);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0564817);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.999794);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.116265);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.203661);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0200035);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.112901);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0405083);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.945794);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0150613);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0130441);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0522888);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.483723);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.014075);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0538325);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0328193);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.798351);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0262642);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.6133);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0410987);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.127632);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.02677);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.097471);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0396871);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0157579);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0355321);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0112908);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0489305);


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
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 1.02415);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.253598);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.128234);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0414379);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.25639e-10);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0614621);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.272108);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0140436);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0245613);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0461193);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0741635);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0143764);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0194308);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0353461);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.97949);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0215332);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.739252);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0386693);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.2099);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0371637);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,5.65367e-05);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.183056);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0301296);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0372447);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0194455);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0468961);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.925316);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.117108);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.1177);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0262181);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.251688);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0552344);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.58084);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.023233);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0478064);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0550534);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.203805);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0246011);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.055928);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0448334);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.591095);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0269901);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.356111);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0523957);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0602603);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0299393);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0283889);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0524539);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0037345);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0461591);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00255313);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0582749);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.958375);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.111995);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.484742);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0400721);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,1.58378e-11);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0596778);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.7581);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0159074);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.135039);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0395257);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.262717);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0204326);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0717523);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0378672);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.335841);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0418401);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.18592);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0504918);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0372796);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0274653);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0295106);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.049108);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00277723);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0474133);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00191566);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0594786);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.989326);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.143377);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.178537);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0159637);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.134834);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0418984);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.762618);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0141849);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0352856);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0416103);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.643657);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0150768);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0409732);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0378575);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.558574);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0324444);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.381281);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0479539);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0677231);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0285798);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.050743);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0442769);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00441514);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0447958);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00305176);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0548486);


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
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 1.02137);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.27534);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0812952);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0201077);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0548615);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0441991);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.162184);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0168563);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0108457);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0433227);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0879739);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0143508);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0112604);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0367993);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.918079);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0315822);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.00294367);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0847146);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.104083);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0267015);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0763897);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0398345);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00765889);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0403331);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0048966);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0541945);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.889943);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.152788);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.03448);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0245319);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.493573);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0471407);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.64601);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0232558);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.111104);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0478823);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.131918);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0280742);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0388445);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0466599);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0578114);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0562909);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,9.22504e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0637777);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0130993);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0525171);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,4.38341e-07);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.055025);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.128228);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0566505);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.02263e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0622457);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00943742);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.061385);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00402178);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0676794);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.942472);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.143373);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.566416);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0215264);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.331076);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0416692);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.73227);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0162433);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0743954);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0462019);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.250795);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0218744);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0593354);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0402706);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0674172);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0531973);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.0420254);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0663792);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0265346);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.048214);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.0211869);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0580027);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM1,0.482112);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH1,0.0382085);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM2,1.10642e-06);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH2,0.0427638);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC4, NORM1,2956.82);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC4, WIDTH1,0.0103817);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC4, NORM2,1614.31);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC4, WIDTH2,0.0231107);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.11506);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0501249);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0555249);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0586628);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00914444);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0620889);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00381695);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0702026);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 1.03264);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.327589);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.138515);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0486992);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0222168);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0510877);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.650881);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0168618);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.105135);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0438395);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.710377);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0159433);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0662357);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0400309);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.974146);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0356495);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,1.56004e-08);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0362193);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.16584);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0451877);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,6.47531e-11);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0591933);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM1,3321.37);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH1,0.00781981);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM2,1.01827);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH2,0.0585443);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.47525);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0295561);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.302225);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0506243);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0366997);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0522725);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0158546);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0587031);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00207194);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0612308);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00161081);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0698141);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 1.05985);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.208996);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.304893);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.045871);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,7.42787e-12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0611431);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.02381);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0487227);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,7.38144e-12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0623858);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.203029);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0470606);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.137524);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0567331);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0856763);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0467378);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0566247);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0639338);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.0142147);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.08152);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,1.59038e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,2.18921);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.44645);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0301466);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.285505);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0575284);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0401369);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0501072);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0201599);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0572657);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 1.11585);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.295452);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.131186);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.026355);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0696995);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0567568);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.337605);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0226427);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0549908);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.045863);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0587979);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0130132);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0362471);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0379759);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.242582);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0455979);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.119742);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0559611);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0704903);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0593416);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,5.7158e-05);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.444799);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.498695);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0254326);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.348331);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0440184);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0910478);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0318482);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0285904);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0538156);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00957643);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0365179);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00760538);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0619696);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 1.125);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.15);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.025);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0);




   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.881536);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.168727);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,6.32066);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.019292);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,3.37753);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0436688);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,10.7318);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0212859);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,2.44005);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0451026);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.705026);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0225308);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.444618);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0467676);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.13971);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0466259);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.602033);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0580348);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.933671);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0285241);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.671823);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0587368);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.111034);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0494035);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0491685);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0569466);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0341523);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0485116);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0169651);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0554712);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM1,0.0843463);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH1,0.0627343);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM2,0.0218307);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH2,0.0663175);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, NORM1,0.0226602);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, WIDTH1,0.0620977);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, NORM2,3.27362e-12);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile2, WIDTH2,0.0630092);


   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.962689);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.175806);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.37653);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0204559);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.929365);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0457221);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,22.409);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0183174);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.752061);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0518717);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,2.01471);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0259281);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.196435);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0540946);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.44903);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0405093);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.791106);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0609762);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.885279);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0313813);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.751074);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0597013);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0780242);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0507751);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.0398584);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0605394);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0213387);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0456081);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0116819);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0541427);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM1,0.0300027);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH1,0.0743944);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM2,1.79491e-06);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH2,0.0744249);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, NORM1,0.00687054);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, WIDTH1,0.0653982);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, NORM2,5.67907e-07);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile2, WIDTH2,0.0677572);


   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 1.05647);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.304901);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.607857);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0485157);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,8.95549e-10);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0632979);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,3.69899);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0176332);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.947198);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0486002);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,6.65349);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0181085);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.492345);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.04517);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,3.76666);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0321272);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.66427);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0504087);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.81469);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0444884);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.961104);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0536079);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.124785);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0494121);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0616596);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0582522);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0233869);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0503545);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0110392);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0610646);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM1,0.062277);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH1,0.0491656);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM2,0.041257);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH2,0.0550062);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM1,0.0271246);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH1,0.0521497);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM2,1.05509e-12);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH2,0.0650386);


   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 1.0488);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.270275);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0538788);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0440423);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0436874);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0503556);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.436384);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0516325);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.0637588);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0539581);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,1.15493);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0121883);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.549824);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0508093);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.414677);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0170874);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.143762);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0485251);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.96034);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0260013);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,2.22882);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0480049);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.99655);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0463768);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.00108924);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.202913);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.498816);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0302826);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.365031);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.051327);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.121374);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0343659);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0825824);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0508873);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.117283);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0573974);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,1.8075e-09);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0644375);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0238808);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0480314);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0141952);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0549846);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 1.00026);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.326293);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,1.41588);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0222612);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,1.16037);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0447061);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,1.49571);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.021093);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,1.18613);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0417704);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.53713);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0362582);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,2.09569e-15);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0563501);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.164118);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0584118);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0481361);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.063387);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0101056);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0651615);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00445192);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.074121);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.699743);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.243212);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,3.33794);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0249769);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,2.17987e-05);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,61.5529);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.293385);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0193123);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0695016);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0336105);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,1.04604);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0247466);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.324879);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0462339);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.124341);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.018752);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.100625);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0397647);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0110518);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0454151);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,7.32099e-12);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0566393);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.870755);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.222884);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.659103);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.024292);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.00315393);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0615891);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.269449);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0149457);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.10448);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0283027);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.569563);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0238051);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.443375);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0388293);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0510441);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0267954);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0379497);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0384144);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00418288);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0456969);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,2.49969e-07);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,100);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.806697);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.357716);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0285626);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0304259);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,5.03143e-14);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0469472);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.290723);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0126164);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0899348);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0285334);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.761912);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0294152);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.539389);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0397704);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.119558);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0269696);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0876294);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0370417);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00856338);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.03668);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00599923);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0491911);


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
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.990163);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.132034);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00986746);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.016718);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00642991);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0449959);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.934764);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0239693);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.731827);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0407733);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.156311);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0384134);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,5.24542e-10);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.057709);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0141463);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0351764);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0100409);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0472007);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.694476);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.300924);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.59815);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.02588);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,3.52134e-05);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,99.9805);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,3.9775);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0155973);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,1.2971);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0244068);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.274311);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.024951);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.00129316);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0547654);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.457353);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.023967);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.258331);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0417015);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0293311);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.037048);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000159348);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0950567);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00163869);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0350312);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00115828);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0484374);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.791495);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.290102);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.558218);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0273387);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,1.86704e-11);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0522972);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.00637);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0132695);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.337649);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.024334);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.290626);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0158396);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0981075);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0274663);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.275769);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.025851);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.211424);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0409187);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0198606);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0294245);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.014835);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0420332);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00118149);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0363898);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.000883621);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0522977);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.775106);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.765286);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0309201);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0158757);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0190029);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0402934);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.227625);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0116154);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0212467);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0351443);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.460932);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0153547);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0297277);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0390181);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.465257);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.034164);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.305952);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0466857);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0430603);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0347057);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.029431);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0453217);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00371723);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0387393);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00242589);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0512111);


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
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.952958);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.189152);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00749324);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0170421);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00322372);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0499791);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.0341901);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0102103);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00181534);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0480021);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.00899787);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0114398);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00302086);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0447849);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.749148);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0231545);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.558631);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0368102);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.114337);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0262773);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0776313);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0388674);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00884725);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0387689);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00613419);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0523187);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.847849);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 1.46915);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.51515);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0177747);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.15501);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0275389);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.69155);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0154189);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.722959);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0244326);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.189758);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0238384);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,8.85065e-07);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,99.9671);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0142397);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0215521);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.00904365);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.028417);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00140733);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0266274);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,1.71778e-15);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,85.1675);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0582951);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.022655);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0438134);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0385187);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00128391);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0538989);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000742419);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0624135);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.60659);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.412966);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.613463);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0280495);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,3.16202e-05);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.117039);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,5.98639);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0122227);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.3991);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0206083);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.269349);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.015425);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.148023);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0235561);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.000257828);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0734811);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.00017697);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.102441);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,7.97908e-06);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.349606);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,2.33503e-06);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.34908);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0616717);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0275318);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0379172);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.049178);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00182209);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.053069);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0010544);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0626974);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.575912);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.461394);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0190289);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0380027);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,8.52804e-08);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,97.8007);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.123834);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0107941);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0387894);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0319583);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.279858);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0152449);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0895393);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0337268);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0208154);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0424803);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0169162);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0511849);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0602583);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0348962);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,1.30273e-13);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0582996);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.414378);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0270802);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.297892);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0464354);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0296631);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0386304);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0189305);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0581508);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00358592);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0307384);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00274807);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0692776);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.025);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.692114);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.539909);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.00425275);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0178338);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00169695);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0631118);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.0211538);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.00908676);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,0.00333732);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0510954);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.00316635);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.00420149);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,0.000647171);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0536954);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.119416);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0359981);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0721333);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0455127);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0458044);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0406068);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0376558);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0504483);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,3049.29);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.010935);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.000127319);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0552035);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.268337);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0303532);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.185166);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0519144);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0249553);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0423923);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0167222);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0570713);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.841568);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::Tile, 1.099);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00249684);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0449579);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,2.53786e-05);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0557353);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.0100821);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.00943476);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0031614);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0405946);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.00372798);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0452869);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,1.39393e-16);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0602284);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0782898);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0332643);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0484567);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0407073);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00663412);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0532529);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.00466629);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0657456);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.356678);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0250662);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.248429);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0440131);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0640485);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.044518);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.86598e-05);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.269147);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00709515);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0377149);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00528367);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0601927);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

   // j1st = EMB1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.287762);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 0.833945);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0315964);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0302318);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,4.86253e-09);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0491808);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00586861);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0472255);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,2.64212e-11);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0632443);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 1.925);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 2.39869);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.305531);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.250025);




   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.613662);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.352488);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,5.74283);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0261721);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,9.04727e-08);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,82.9641);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,19.6563);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0204938);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.000187841);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,99.9978);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,1.19895);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0257558);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,2.09776e-06);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,100);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,1.27048);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0249313);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.989474);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0404347);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.773238);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0222247);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.593474);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0417476);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.0702615);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0200011);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.0572184);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.042272);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0203795);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0247283);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0155572);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0397388);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM1,0.010861);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH1,0.0259254);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, NORM2,0.00689176);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::Tile1, WIDTH2,0.0564375);


   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.773444);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.342717);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.55806);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0282136);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,5.67644e-13);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0495357);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,26.6702);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.01203);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,10.2902);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0207714);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,2.41947);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0165431);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,1.12877);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0272768);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.45533);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0277217);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,1.09685);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0449635);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.851056);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0275745);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.660943);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0460528);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.0641574);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0231821);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.0497872);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0488755);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0179085);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0291161);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0128231);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0424021);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM1,0.00779037);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH1,0.0433296);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, NORM2,0.00320198);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::Tile1, WIDTH2,0.0500146);


   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.725155);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.561848);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.0771584);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.02978);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,6.03411e-14);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0543024);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,2.20242);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0107729);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.468342);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0272605);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,4.20251);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0132844);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,1.56582);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0281901);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.98229);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.034894);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,1.983);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0484766);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.66514);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0375795);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,1.06567);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0515066);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.124353);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0412522);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.0713733);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.051392);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0283927);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0394779);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0157901);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0510148);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM1,0.0322723);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH1,0.0250517);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, NORM2,0.0196764);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile1, WIDTH2,0.0538088);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM1,0.00186495);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH1,0.0507632);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, NORM2,0.00154306);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::Tile2, WIDTH2,0.0584806);


   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.847609);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.352559);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,2.62668);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0226877);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,6.18862e-08);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,86.4312);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.107721);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0288422);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.000231118);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0699451);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.281719);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0138667);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.0900599);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0308736);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.0740474);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0374402);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,2.521e-15);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0573776);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,2.37853);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0243777);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.85167);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0442231);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,3.80773);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0268239);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,2.93519);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.046623);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.504626);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0267966);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.398259);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0477866);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.152159);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0295291);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.111686);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0454859);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.100588);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0274822);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0590407);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0516229);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.017889);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0367929);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0136779);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0506228);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.610674);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.638089);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,6.29804);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0198432);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,2.13463e-07);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.799622);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME1, NORM1,0.988026);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME1, WIDTH1,0.0271928);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME1, NORM2,1.71729e-10);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME1, WIDTH2,0.0527927);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME3, NORM1,0.371253);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME3, WIDTH1,0.0267237);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME3, NORM2,1.86623e-10);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EME3, WIDTH2,0.0493423);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.764001);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0268229);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.569494);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.040421);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,1.73754);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0215982);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,1.31907);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0375133);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.307987);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.041751);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,1.77745e-09);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0571278);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.108431);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0266971);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.101926);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0526138);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00668219);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0479408);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.00433094);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0601025);

    }

return StatusCode::SUCCESS;

}

StatusCode eflowCellEOverPTool_mc12_LC::finalize(){
  return StatusCode::SUCCESS;
}
