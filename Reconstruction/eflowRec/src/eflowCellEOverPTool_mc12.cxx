/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowCellEOverPTool_mc12.h"
#include "eflowRec/eflowCaloRegions.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <vector>
#include <iomanip>
#include <sstream>

eflowCellEOverPTool_mc12::eflowCellEOverPTool_mc12(const std::string& type,const std::string& name,const IInterface* parent) : IEFlowCellEOverPTool( type, name, parent), m_nEBins(4),m_nEtaBins(25),m_nFirstIntLayerBins(eflowFirstIntRegions::nRegions),m_nCaloRegionBins(eflowCalo::nRegions){

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

StatusCode eflowCellEOverPTool_mc12::initialize(){

  return StatusCode::SUCCESS;
}

StatusCode eflowCellEOverPTool_mc12::execute(eflowBinnedParameters *binnedParameters){

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
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.386062);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB1, 0.23867);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.5981);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0150036);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,1.21193);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0275566);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.604207);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0173702);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.235887);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0203174);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0186625);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0229276);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,1.77596e-17);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,1.10406);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00786182);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0267936);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00427717);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0834168);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.000230599);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0211224);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00016531);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0899948);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.480017);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB2, 0.229758);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.274245);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0160872);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.203082);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0310637);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.10189);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0167244);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.87863e-05);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,99.9819);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0604046);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0187969);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0315178);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.022773);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0130336);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0229338);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0105981);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0595922);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.410462);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EMB3, 0.313411);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.330849);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0191277);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.239905);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0304784);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.419856);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0145146);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,7.07945e-08);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,6.13523);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.332318);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0182571);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.177062);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0220777);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.364728);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0334294);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,1.14433e-13);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0541727);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.015263);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.037121);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,1.52838e-12);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0551909);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::HEC, 0.479623);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::HEC, 0.244554);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.479623);
    binnedParameters->setFudgeStdDev( E001bin, eta050bin, eflowFirstIntRegions::Tile, 0.244554);


    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0540086);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0150258);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0404369);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0297108);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0276587);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.00902209);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0214841);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0205812);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.579569);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.033376);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,4.57769e-15);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.05333);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0405758);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0365154);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.025691);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.04672);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00112362);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.056245);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.000760622);
    binnedParameters->setShapeParam( E001bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0738976);
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
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.412769);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB1, 0.19669);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.09183);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0245168);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,2.38639e-12);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0429064);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.408423);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.017741);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.16161);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0203335);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.015458);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0217072);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0082844);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0255928);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00550385);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0357305);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00374958);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0448194);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.466153);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB2, 0.22003);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.196159);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0189305);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.149071);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0271254);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,1.43602);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0153009);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,6.14944e-15);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0237608);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0483247);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0210467);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0235603);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0256538);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0135793);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0331365);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00893133);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0455184);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.366232);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EMB3, 0.320208);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.244463);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0238988);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,6.44169e-14);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.042116);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.386806);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0115348);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.213705);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0141667);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.351634);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0175761);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.196341);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0216838);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.183751);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0286861);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.143138);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0368641);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0077679);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0414626);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,7.81843e-12);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0580155);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::HEC, 0.429896);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::HEC, 0.158201);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.429896);
    binnedParameters->setFudgeStdDev( E001bin, eta100bin, eflowFirstIntRegions::Tile, 0.158201);


    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.375608);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0347576);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,4.38609e-11);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0509055);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0233769);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0291767);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,5.13143e-13);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0536962);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00143096);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0391696);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.000924489);
    binnedParameters->setShapeParam( E001bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0521582);
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
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.36654);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB1, 0.222015);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.703047);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0195);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.514882);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0312708);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.35994);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.01421);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.127539);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0235589);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0174103);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0184906);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0129055);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0219336);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00183276);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0433121);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,7.75673e-07);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,99.9756);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.430656);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB2, 0.199008);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.164336);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.024504);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,1.5099e-14);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0433545);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.26852);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0115615);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,1.34522);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0141189);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0628821);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0186825);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0338401);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0227617);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0034316);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0174253);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0027611);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0376892);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.16193);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EMB3, 0.156036);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.366968);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0164653);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.277524);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0277823);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.271471);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0186509);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,6.31612e-12);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0274634);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.0440978);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0375993);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0292332);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0457761);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00124106);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0494418);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.000768992);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0594236);


   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.552672);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::EME3, 0.169852);




   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.402551);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::HEC, 0.23783);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.388245);
    binnedParameters->setFudgeStdDev( E001bin, eta150bin, eflowFirstIntRegions::Tile, 0.233083);


    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0851647);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0401961);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0528546);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0520077);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.014577);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0365947);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0101586);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0456619);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0028065);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0373802);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00228247);
    binnedParameters->setShapeParam( E001bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0822674);
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
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 0.370439);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB1, 0.185149);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB2, 0);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0.283354);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EMB3, 0.187263);




   // j1st = EME1
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.373412);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME1, 0.187836);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,2.2608);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0216796);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,4.8071e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0288435);


   // j1st = EME2
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.431992);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME2, 0.158464);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.2542);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.114848);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.0053125);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,11.7848);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.129734);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0106542);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.101163);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0143723);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.125212);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.187357);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,3.15444e-06);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,42.6097);


   // j1st = EME3
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.4137);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::EME3, 0.285945);


    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,0.0503245);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.163743);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,0.000743813);
    binnedParameters->setShapeParam( E001bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.176977);


   // j1st = HEC
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.471704);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::HEC, 0.425883);




   // j1st = Tile
    binnedParameters->setFudgeMean( E001bin, eta250bin, eflowFirstIntRegions::Tile, -4.20149);
    binnedParameters->setFudgeStdDev( E001bin, eta250bin, eflowFirstIntRegions::Tile, 2.62476);


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
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.432784);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.227019);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.48999);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0408056);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.49279);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0443954);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.519154);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0193806);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.193142);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0442528);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0248163);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0424682);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,3.70377e-14);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0594586);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0584447);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0605162);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0319606);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.066702);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00662856);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0550524);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00367023);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0594075);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00195215);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0540479);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.0014066);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0629849);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.466548);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.199098);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.24322);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0170699);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.17157);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0342103);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.15539);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0116708);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.931061);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.018951);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0768043);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0187136);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0343988);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0277166);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0429636);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0437562);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0255521);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0520375);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00420824);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0382621);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00273372);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0489767);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.46389);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.230631);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.165762);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0166374);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.111791);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0405521);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.595051);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0135543);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00667315);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0668491);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.307904);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0205709);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.000378545);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0994556);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.428979);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0368748);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.238563);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0617094);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0444321);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0387633);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0267704);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0486553);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0145658);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.044276);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,4.69472e-08);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,99.9996);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.498918);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.250362);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.498918);
    binnedParameters->setFudgeStdDev( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.250362);


    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0892513);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0167746);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.065192);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.036483);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.353773);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.012395);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00233676);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0505093);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0233435);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0076268);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0175192);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0154842);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.558973);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0381376);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,8.33024e-05);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.16795);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0604671);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0400378);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0341575);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0531728);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0063686);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0378211);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0054588);
    binnedParameters->setShapeParam( E003point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0478919);
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
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.438563);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.234904);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.967459);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.013612);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.820401);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.04476);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.420176);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0148326);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.16484);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0463856);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0255423);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0458169);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,1.12203e-10);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0605801);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0454491);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0623816);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0223885);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0690237);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00345157);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0552143);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00159785);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0595215);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.463163);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.198368);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.159865);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0156337);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.116796);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0308073);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,1.46332);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0169861);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.000131577);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.144996);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0657144);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0215249);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0238239);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0342276);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.038893);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0432583);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0189718);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0566657);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00311048);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0248552);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00248277);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0522186);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.445549);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.193439);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.113229);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0193316);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0723195);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0471763);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.55347);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0131594);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00613804);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0524572);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.394257);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0132778);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.263481);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0215606);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.225727);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0344182);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.139914);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0543241);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0176314);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0266841);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0122203);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0666389);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00121665);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0447771);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00101749);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.057104);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.547438);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.312442);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.547438);
    binnedParameters->setFudgeStdDev( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.312442);


    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0749354);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0150419);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0558352);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0311951);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.298284);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0130248);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,4.36422e-05);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.305042);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0341862);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.00883064);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0257254);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0179913);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.440259);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0281385);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.30911);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0424665);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0394235);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0474571);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,6.66662e-06);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,99.9933);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00226563);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0438673);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00174642);
    binnedParameters->setShapeParam( E003point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0653983);
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
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.350861);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.230259);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.653168);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0473297);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.00529565);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0527397);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.453367);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0133676);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.117885);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0491323);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0188599);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0459046);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,1.73408e-12);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.061987);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0175717);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.100123);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,1.56156e-06);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.100127);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00270711);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.200778);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,4.69755e-07);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.200795);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,0.0521846);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.0700555);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,2.20858e-06);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.0700747);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0153676);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0594291);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0102262);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0653345);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00184543);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0774389);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.000573755);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0828419);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.396924);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.197647);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.143248);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0212826);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.092391);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0387421);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.15078);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0150125);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.00398844);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0637863);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0653404);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0294797);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,4.51114e-05);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.100307);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0293973);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0656921);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,1.51257e-06);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0675013);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.00660879);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0987051);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.000247418);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,95.0278);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0133383);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0536713);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.00563206);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.059553);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00349517);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0681567);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,5.0026e-06);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0690816);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.387933);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.209662);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.128265);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0171806);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0922116);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0358492);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.363892);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0141023);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00223661);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0759019);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.411139);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0182349);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.205418);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0243141);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.437998);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0430223);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.000126997);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,95.8447);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.101902);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0584153);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.00125146);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,77.0548);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,318.14);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.00709748);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,0.108413);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.103795);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.0849281);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0362489);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.049733);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.060047);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.00513744);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0848609);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.000870793);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0915497);


   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.428979);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.176213);




   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.381963);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.172633);




   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.375679);
    binnedParameters->setFudgeStdDev( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.173662);


    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0239164);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0174735);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0150284);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0486413);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.159087);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0113015);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00419488);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0763933);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0447261);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0194105);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.000476842);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.128275);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0157916);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0645909);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,7.9214e-07);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.066931);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00864008);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0213351);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.000593778);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,91.1974);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.126327);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0253953);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0729664);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0670561);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0234698);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0382566);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0114666);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.075939);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00445348);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0428742);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00354674);
    binnedParameters->setShapeParam( E003point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.052484);
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
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.272075);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.203596);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.0114196);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0183144);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.00751152);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0236485);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.00513429);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0305528);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.000834485);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,99.9943);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00270503);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.188408);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,1.20441e-06);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,94.1877);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.325);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.111146);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.253511);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.184144);




   // j1st = EME1
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.325954);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.169537);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,2.2348);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0253585);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,0.00018857);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,99.9917);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,1.0548);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.019698);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,5.98327e-11);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.477472);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.0327713);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0218776);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.000244453);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,99.9395);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.00641819);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0239596);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.00460289);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0494493);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,404463);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.00637384);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.668498);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0270671);


   // j1st = EME2
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.359032);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.155752);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,0.428384);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0136495);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,0.347508);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0218773);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,8.90868);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0102778);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,5.40387);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0128386);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.192036);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0158997);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,1.41765e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,100);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.0153774);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.036068);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.0100766);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.049383);


   // j1st = EME3
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.415337);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.264344);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.199876);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0108278);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.158995);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.017647);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,3.95809);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0092208);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.00123217);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.00992147);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.32764);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0156074);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,4.66841e-08);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0211954);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,0.742616);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0172977);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,0.551769);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.032241);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.00932319);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0203745);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.00682646);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0378112);


   // j1st = HEC
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.587736);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.283253);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.0633049);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.00845467);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.0300576);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0126673);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.00299116);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.444485);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,7.4744e-12);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,47.8689);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.0654837);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0149412);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.0371763);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0269554);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,1.46512);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0257852);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.000406445);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,25.9366);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,2.09477);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0194256);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.000126259);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,84.5497);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.0104863);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.18862);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,3.59124e-06);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,4.29975);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.194431);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0257617);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0813192);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0610768);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.00779665);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0475699);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,7.553e-13);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0630669);


   // j1st = Tile
    binnedParameters->setFudgeMean( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.422932);
    binnedParameters->setFudgeStdDev( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.118161);


    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0383771);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0294715);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0264933);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0417716);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00511672);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0556619);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.00194964);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0743095);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.015616);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0367862);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0108256);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0465648);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00180706);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0119724);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.64775e-05);
    binnedParameters->setShapeParam( E003point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.201887);
    ////////////////////////////
    //      0 <= eta <  0.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta050bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.600574);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB1, 0.179304);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.42866);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0270872);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.721449);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0457009);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.47233);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0286463);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.220577);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0425448);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0482882);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0402849);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,1.17451e-13);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0582614);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.298049);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.041532);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.216037);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0526549);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0586039);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0398668);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.038099);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0479191);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00949161);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.039779);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00689925);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.050358);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.594104);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB2, 0.184116);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.391035);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0266005);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.222415);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0422346);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.72493);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0192734);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.36958);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0327681);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.137797);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0237397);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0422033);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0339262);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.175088);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0474337);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,3.72731e-13);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.060999);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0252999);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0461298);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,4.07337e-11);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0609384);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00280671);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0424394);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00199007);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0504542);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.613668);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EMB3, 0.197186);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.12505);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0222434);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0503859);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0460574);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.534612);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0164698);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00460125);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0646932);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.236862);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0107778);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.173248);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0210806);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.48608);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.025879);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.364137);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0393194);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0692399);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0267771);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0517577);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0408961);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.006237);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0369503);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.0045421);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0550897);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::HEC, 0.720781);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::HEC, 0.152216);




   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.720781);
    binnedParameters->setFudgeStdDev( E010bin, eta050bin, eflowFirstIntRegions::Tile, 0.152216);


    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.123771);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0352605);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0619444);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0388756);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.370489);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0115466);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0861636);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.035305);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0285219);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.00969572);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0190131);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0240872);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.731203);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0246292);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.559416);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0401692);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.132726);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0303273);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.098996);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0392619);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0223668);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0316203);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0161152);
    binnedParameters->setShapeParam( E010bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0471309);
    ////////////////////////////
    //      0.5 <= eta <  1.0   //
    ////////////////////////////

    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta100bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.597096);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB1, 0.185309);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.53068);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.026445);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.679023);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0460501);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.905319);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0301185);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.142348);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0472174);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0451239);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0409134);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0190221);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0455557);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.153651);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0502127);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0945895);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.056969);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0206903);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0476656);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0132818);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0552023);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00181672);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0487594);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00139518);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.056702);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.576186);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB2, 0.183032);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.173787);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0235665);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0991471);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.043196);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,1.41502);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0215052);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.00755608);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0537417);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.108402);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0237463);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0344555);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0361572);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0808003);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0493659);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0413609);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0584372);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0123253);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0429214);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00687358);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0514549);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.530239);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EMB3, 0.180694);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0911457);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0261392);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0466792);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0508414);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.4916);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.00945282);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.123102);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.028196);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.37124);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0113951);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.245833);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0249293);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.273721);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0350579);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.178867);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0487656);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0309143);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.036955);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0205428);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0460289);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00335023);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0438638);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00215055);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0542255);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::HEC, 0.71421);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::HEC, 0.143348);




   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.71421);
    binnedParameters->setFudgeStdDev( E010bin, eta100bin, eflowFirstIntRegions::Tile, 0.143348);


    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0967012);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0490271);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.30082e-10);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.062622);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.299696);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0105109);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.105011);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0373534);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.046748);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0127123);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0195925);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0382149);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.711605);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0243261);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.532299);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0444526);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0531289);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0419344);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0305127);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.05415);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00513283);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0495463);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00368707);
    binnedParameters->setShapeParam( E010bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0629527);
    ////////////////////////////
    //      1.0 <= eta <  1.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta150bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.532465);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB1, 0.214413);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.937084);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0453129);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,9.66504e-10);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0600133);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,0.721092);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0412603);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,1.29405e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.058931);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0398086);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.042045);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0238381);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0462064);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.000934668);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.119567);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.000484255);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.150503);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00196382);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0549031);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00155422);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0636455);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,835.517);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.0137994);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,1.80547);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.0213178);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0592815);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0542856);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.0336052);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0617765);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00504682);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0663007);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00297779);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0734021);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.521643);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB2, 0.218652);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.173377);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0129604);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.14659);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0486295);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.5373);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0156605);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.181732);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0408849);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.0823716);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0289597);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0307792);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0398867);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0616394);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0464411);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,3.01958e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0630071);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0955202);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0200879);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.00682577);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0819485);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.031776);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0582351);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0158251);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0657979);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0036938);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0592943);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00207206);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0649848);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.495554);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EMB3, 0.215056);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.115398);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0497148);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.026653);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0522772);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.389119);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.00930231);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.122106);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0439089);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.504697);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0109504);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.348867);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0247028);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.242975);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0139267);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.114257);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0581571);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.131215);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0155055);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.0323679);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0644091);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.18331);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0467009);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0954395);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0575474);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0168255);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0532887);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,5.25861e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0637239);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.000852885);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0731156);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.000506142);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0837585);


   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME1, -5.06251);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME1, 1.69894);




   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME2, 0.327782);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME2, 1.89857);




   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.557418);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::EME3, 0.229677);




   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.549574);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::HEC, 0.280506);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.115511);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0295767);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0278506);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0635465);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.0123831);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.00383663);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,4.24026e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,1.34546);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0741746);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0553275);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0475634);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0662795);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0595384);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0492055);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0437978);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0636105);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,4795.44);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0115563);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,1.61043e-05);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,99.995);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,3164.97);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.00710021);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.460748);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0548857);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.197009);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0204522);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.168865);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0542941);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0165678);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.049793);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0100844);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0616358);


   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.548337);
    binnedParameters->setFudgeStdDev( E010bin, eta150bin, eflowFirstIntRegions::Tile, 0.294486);


    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0626392);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0509761);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,1.93051e-11);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0638132);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.144981);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0119619);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0809809);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.042049);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0508031);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0129531);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0238994);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.041345);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0446243);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0590194);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0270132);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0710243);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.011723);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0671476);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.00689235);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0842775);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,98187.6);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.00485426);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.0519166);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0464737);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.226082);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0313755);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.190993);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0553039);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0414959);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.048189);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0230044);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.056925);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00947267);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0459432);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00640077);
    binnedParameters->setShapeParam( E010bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0598277);
    ////////////////////////////
    //      1.5 <= eta <  2.5   //
    ////////////////////////////

    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::EMB1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::EMB2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::EMB3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::EME1, 0.0491122 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::EME2, 0.0175171 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::EME3, 0.0278496 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::HEC1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::HEC2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::HEC3, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::HEC4, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::Tile1, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::Tile2, 0.0700683 );
    binnedParameters->setRingThickness( E010bin, eta250bin, eflowCalo::Tile3, 0.111398 );


   // j1st = EMB1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.165606);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB1, 0.0869843);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,0.0318579);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0563842);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,7.77322e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,66.7581);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0396909);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.059235);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.014938);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0676382);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0181327);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0893249);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00443861);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.100162);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM1,2.09677e+07);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH1,0.0113374);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, NORM2,1.21777);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC4, WIDTH2,0.0486345);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.00650121);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0561917);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.00463267);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0638951);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.000735779);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0347541);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00056961);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0885949);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.55208);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB2, 0.247468);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.339715);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EMB3, 0.250131);




   // j1st = EME1
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.36359);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME1, 0.217742);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,3.23206);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0409826);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,4.56711e-10);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0589679);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,3.1053);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0455532);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,5.05631e-08);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.050055);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,0.129302);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.045762);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.0296434);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0479405);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,0.214624);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0590544);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,0.0496676);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0628312);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,0.0655995);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0574578);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,0.0346356);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.0681422);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.00502027);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0598537);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,6.58229e-10);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0709451);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0171004);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.188719);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,5.26683e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,40.8468);


   // j1st = EME2
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.393696);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME2, 0.201536);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.01188);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0358155);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,1.24699e-13);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0584514);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,8.67017);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0194514);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.992839);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.034377);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,0.522213);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0295796);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.0279378);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0692185);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,0.210271);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0540364);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,0.0867479);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0608694);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.0758071);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0671977);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.0394057);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0751087);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.00371963);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.257956);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,9.16286e-07);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.257967);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.000135952);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.028314);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.000103346);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0400561);


   // j1st = EME3
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.527091);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::EME3, 0.237256);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.0831528);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0165502);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.0635038);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.031292);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,1.94929);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.00887776);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.648345);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0229475);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.31466);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0190849);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.00156891);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.101439);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,1.06428);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0315038);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,0.849223);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0377082);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,0.234813);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0268451);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.078811);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0891097);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.0081605);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.11407);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.00110352);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.123312);


   // j1st = HEC
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.582603);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::HEC, 0.222585);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0379049);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0547738);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0136199);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.05969);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,1.30542);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0282654);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,1.5931e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,99.991);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.124984);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0559981);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,0.0381666);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0613924);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,0.473027);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.00543462);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,0.0693046);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0684969);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.150397);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.012572);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.0177543);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0665575);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.887396);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0351577);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.54055);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0586046);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.553297);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0389438);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.330613);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0552075);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.259066);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0242892);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.205097);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0432334);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.0588609);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0417743);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0328828);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0540887);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0423655);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0613192);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0205427);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0662142);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0174115);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0501321);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0107605);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0583538);


   // j1st = Tile
    binnedParameters->setFudgeMean( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.451966);
    binnedParameters->setFudgeStdDev( E010bin, eta250bin, eflowFirstIntRegions::Tile, 0.229672);


    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.00341536);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0418534);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,2.54382e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,99.9931);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,4.1755);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0223536);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,1.31178e-06);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,99.9467);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.055726);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0417449);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0325318);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0487636);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.00843027);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0266354);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0062259);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0514632);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0163079);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.045794);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0121207);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0522919);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.000797219);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0568471);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.000433896);
    binnedParameters->setShapeParam( E010bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0632021);
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
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.697879);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB1, 0.13057);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.12437);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0260356);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.355832);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.050421);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.44948);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0234192);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.058673);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0534494);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.138387);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0227853);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0373378);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0377881);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.533066);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0246335);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.3415);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0483415);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.101662);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0284064);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0422723);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0451525);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0126296);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0332683);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00860144);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0464075);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.689183);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB2, 0.131072);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.388108);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.021952);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0639297);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0494259);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.99845);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.016873);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.034982);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0446689);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.229509);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0181826);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0359231);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0330641);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.331547);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0266099);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.201234);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0478992);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0595396);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0242007);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0378191);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.042891);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00690835);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0451443);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,6.784e-14);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.060496);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.678432);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EMB3, 0.166584);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0865285);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0203966);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0503473);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0403092);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.508477);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0155958);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.00324027);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.063992);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.225624);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0103065);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.154778);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0198368);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.494001);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0242816);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.380996);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0377213);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0958386);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0242098);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0746413);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0377151);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0109066);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0348667);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00722282);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0487519);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::HEC, 0.760526);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::HEC, 0.131204);




   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.760526);
    binnedParameters->setFudgeStdDev( E020bin, eta050bin, eflowFirstIntRegions::Tile, 0.131204);


    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0863483);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0204257);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.042449);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0462181);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.207884);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.016476);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00772011);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0506966);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0332129);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0105321);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.015153);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0251259);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.803888);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0218931);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.59923);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0328715);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.156756);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0236469);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.109486);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0339);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0206086);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0343535);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0152588);
    binnedParameters->setShapeParam( E020bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0559234);
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
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.697489);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB1, 0.131404);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.68728);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.028802);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.116775);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0585276);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.43952);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0262085);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0317175);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0566845);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.090664);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0237931);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0461595);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0410375);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.290839);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0249548);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.231652);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0513945);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0389571);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0267031);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0266238);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0486032);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00317069);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0399299);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00212165);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0542309);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.676799);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB2, 0.145329);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.4188);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0260154);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0884869);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0454388);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.69854);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0186236);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0182461);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0490791);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.173917);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0245992);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.00751564);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0485844);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.175016);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0277642);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.137394);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.052672);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0220336);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0304706);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0171345);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0488863);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00221922);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0444313);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00122452);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0538261);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.613545);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EMB3, 0.152498);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0780146);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0410277);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,5.9979e-11);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0557845);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.490494);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0158766);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0199142);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.04286);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.446402);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0161807);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0135979);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.038416);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.382547);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0284576);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.298012);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0439364);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0467373);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0274208);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0357392);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0435169);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00369621);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0409826);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00263276);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.049746);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::HEC, 0.754032);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::HEC, 0.118854);




   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.754032);
    binnedParameters->setFudgeStdDev( E020bin, eta100bin, eflowFirstIntRegions::Tile, 0.118854);


    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0677341);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0150933);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0516746);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0458473);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.153061);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0145561);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0105581);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0485486);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0429477);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0153425);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00725705);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0354076);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.754227);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0334456);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.000135901);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.11316);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.074788);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0387157);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,1.178e-11);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0593218);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00459849);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0406672);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00314327);
    binnedParameters->setShapeParam( E020bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0538657);
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
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.670168);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB1, 0.15632);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.21061);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0272001);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.192465);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0502046);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.0401);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0283322);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0352307);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0548763);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0539759);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0399216);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0159819);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0421795);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0389546);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0612395);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,2.50138e-11);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0640871);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00550531);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0469205);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00363068);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0636575);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM1,284189);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH1,0.000911246);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, NORM2,0.00183022);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC3, WIDTH2,0.307052);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0687518);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0582011);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.36615e-08);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0632912);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00562116);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0545489);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00396052);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0610369);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.678459);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB2, 0.146038);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.368175);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0240117);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.19169);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0428338);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.43466);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.019514);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0392297);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0505527);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.149421);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0245945);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0248445);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.044391);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0183391);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0552311);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.0157193);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0661294);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.00877708);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0567443);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.00553719);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0846502);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM1,22944.3);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH1,0.000953576);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, NORM2,0.000539648);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC3, WIDTH2,0.0996928);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0575781);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0287827);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.0545476);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0650887);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00548169);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0642004);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00212023);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0725839);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.674577);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EMB3, 0.147988);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0720647);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0437992);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0113067);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0461616);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.42853);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0161885);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0278529);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0508944);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.571253);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0127083);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.302891);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0248679);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0340146);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0457349);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,1.69672e-08);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.059846);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0727886);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0289309);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.0421424);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0521314);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.227749);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0297958);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.152454);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0500195);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.014762);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0528374);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.00689659);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0603196);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00166081);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0512462);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00124321);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0591555);


   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.25);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME1, 0.0833594);




   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME2, 1.00534);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME2, 0.249881);




   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.715946);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::EME3, 0.0971401);




   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.768461);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::HEC, 0.136842);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.204759);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0349472);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0206208);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0695801);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0331304);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0677914);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0221045);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0830797);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.040809);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0486628);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0319595);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0634951);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,2631.13);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.00590359);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.0042147);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0832385);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,2.88864e+07);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.00958928);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.961604);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0484038);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.303754);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0261789);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.217775);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0532784);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0298649);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0377049);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0230389);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0531078);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile3, NORM1,0.00619877);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile3, WIDTH1,0.0560193);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile3, NORM2,2.03277e-07);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile3, WIDTH2,25.6122);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.78969);
    binnedParameters->setFudgeStdDev( E020bin, eta150bin, eflowFirstIntRegions::Tile, 0.128734);


    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.078156);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.037444);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0424677);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.055397);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.197277);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.00875644);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0656914);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0447169);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0446521);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0192677);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00835771);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0459606);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.14603);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0487319);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0458366);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0539287);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0439485);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0396716);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0277799);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0500531);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,18.112);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.00761695);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,12.8539);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.022612);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,3.17222);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.00603152);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,1.18932);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0424894);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.340101);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.045206);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,8.75013e-12);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0622013);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0631984);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0321479);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0446935);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0493721);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0055708);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0395518);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00435201);
    binnedParameters->setShapeParam( E020bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0637745);
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
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 0.427651);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB1, 2.19996);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.675001);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB2, 0.0735542);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.437891);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EMB3, 0.229933);




   // j1st = EME1
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.484662);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME1, 0.221139);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,14.1493);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0247021);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,7.98197);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0471069);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,17.704);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0251771);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,7.57465);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0423734);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,1.19976);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0257805);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.797189);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0431661);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,3.75957);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0490253);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,3.19949e-08);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0612675);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,2.24206);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0496484);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,9.00413e-05);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,99.9852);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.314874);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0327558);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.205336);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0562788);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0400212);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0382);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0259112);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.049251);


   // j1st = EME2
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.532854);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME2, 0.224618);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,3.72056);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0343435);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,2.52726);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0417584);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,24.1106);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0290948);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,0.430847);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0564581);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,3.70372);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0318601);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.000258077);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,99.9914);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,1.90633);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0480633);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,1.05833);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0578246);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,0.725308);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0510915);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.423143);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.059053);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.149076);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0736133);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.000150276);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,59.1697);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.054287);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0419352);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.027424);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0527823);


   // j1st = EME3
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.614839);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::EME3, 0.20973);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.937628);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.029299);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.404603);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0507173);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,3.27974);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0294362);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,3.4788e-10);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0522375);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.25602);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0206954);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,1.22067);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0322434);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,2.30661);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.040775);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,0.85814);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0692599);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,1.04791);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0532763);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,0.495287);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0638597);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.105008);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0483423);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.000222953);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,99.9384);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0204876);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0532229);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0119723);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0706938);


   // j1st = HEC
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.637356);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::HEC, 0.200273);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0138373);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0668101);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00572325);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0771842);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,0.664092);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0411823);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,6.81731e-10);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0609851);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,1.19191);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0380947);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,2.82069e-15);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.110338);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.166828);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0389161);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,4.69857e-12);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0575029);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.860939);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0309649);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.636159);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0485085);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.50031);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0350261);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.400717);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0557405);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.252405);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0323032);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,5.50616e-05);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,99.9991);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.0441655);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0220279);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.00174069);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,99.7645);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0661135);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0534516);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0316562);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0621728);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0107794);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0415729);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00800122);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0582878);


   // j1st = Tile
    binnedParameters->setFudgeMean( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.54647);
    binnedParameters->setFudgeStdDev( E020bin, eta250bin, eflowFirstIntRegions::Tile, 0.350948);


    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0184491);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0512898);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0104965);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0574656);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.0853023);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0435453);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.046171);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0575186);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0240274);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0381508);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0142829);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0542248);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.0159069);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0581462);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,1.25857e-06);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.713195);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0343558);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0613357);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.00279395);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.064334);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0011989);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0629437);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.000716933);
    binnedParameters->setShapeParam( E020bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0748103);
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
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.733235);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, 0.114904);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.13032);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0257998);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.114594);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0579209);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,4.04801);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0178553);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0772752);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0473434);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.151012);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.020516);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.022601);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0397924);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.417358);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0264037);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.238399);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0491707);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0868674);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0255223);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0442769);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0445611);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.0145717);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0437725);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,3.69664e-12);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0589631);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.723011);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, 0.121861);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.642541);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0235132);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0907097);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0482728);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,6.06199);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0144663);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0540498);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0424197);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.294438);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0175096);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0217905);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.036282);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.410554);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0228957);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.345274);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0449272);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0695075);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0243254);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0561589);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.041396);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00833336);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0381761);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00542511);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0468151);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.701908);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, 0.15593);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0748617);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0189319);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0488997);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.036661);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.527819);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.01284);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0133188);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0370379);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.262654);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0127677);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0202423);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0299265);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.504967);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0258765);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.397651);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0378789);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.109163);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0332778);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,8.87966e-06);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.277307);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0117554);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0350815);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00828489);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0461829);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.793913);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::HEC, 0.105782);




   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.793913);
    binnedParameters->setFudgeStdDev( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, 0.105782);


    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0802288);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0257981);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0112478);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0581201);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.218107);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0151299);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0123936);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0407157);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0228226);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0154305);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00340897);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.03782);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.875782);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.018597);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.632088);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0340829);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.156627);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0238197);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.121866);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0357935);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0169086);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0363895);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.0121528);
    binnedParameters->setShapeParam( E032point5bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0463105);
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
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.736423);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, 0.114987);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.47101);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0243758);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.142847);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0531441);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.3382);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0213769);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0485625);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0528102);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.107852);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0238079);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0220746);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0458442);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.184806);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0518282);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,1.51863e-09);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0597427);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0353418);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0256569);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0282105);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0483592);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00227402);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0376911);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00164583);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0550837);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.709001);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, 0.130156);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.604866);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0219749);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0352682);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0554709);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,2.74556);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0170123);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0209906);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0519147);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.154575);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0216887);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0153399);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.043177);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.218965);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.022754);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.180412);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0490732);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.027242);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0254288);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0185306);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0478889);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00242448);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0387526);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00174762);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0566512);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.631504);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, 0.143209);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0574979);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0228992);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0317276);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0483278);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.607352);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0145861);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0116509);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0481875);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.491515);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0145203);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0187135);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0350385);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.351162);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0251019);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.250624);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0438232);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0496801);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0260466);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0392457);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0424853);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00331687);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0410198);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00224046);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0533367);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.794826);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::HEC, 0.1063);




   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.794826);
    binnedParameters->setFudgeStdDev( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, 0.1063);


    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0286103);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.019062);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0201424);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0440634);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.0991267);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0129221);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0120403);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0347889);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.043268);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0129611);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00477985);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0350427);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.679244);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.022982);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.548229);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0344652);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.083141);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0249399);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0606611);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0378469);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00489297);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.040922);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00330947);
    binnedParameters->setShapeParam( E032point5bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0547107);
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
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.730938);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, 0.121832);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.42002);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0238862);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.251891);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0467996);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,1.38049);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0237653);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0376213);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0571358);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.0695703);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0222248);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0451717);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0423628);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.0202673);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.0558646);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,0.0132707);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0680128);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.0175178);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0597502);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,4.97123e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0621104);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.053262);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0589253);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,3.79587e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.064348);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00598068);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0651202);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.00101786);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0690174);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.73683);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, 0.116947);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.398184);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0249409);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.120411);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0467341);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,4.26935);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0176871);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0542634);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0472162);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.13072);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0203707);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.061057);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.038799);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0409602);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0534533);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,0.0239524);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.066838);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0051827);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0618737);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,0.00444231);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0806766);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0664675);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.059306);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,5.95341e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.062653);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0059247);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0587975);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00286136);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0654452);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00112263);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0679346);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.000115512);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0709834);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.727181);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, 0.141224);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0721066);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0381146);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0299118);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0466748);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.366491);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0185795);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0473348);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.040687);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.650215);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0173058);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0296233);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0401121);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0484176);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0460214);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.0319566);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0614367);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.0149276);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0502378);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.0111031);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0607526);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM1,1.59996e+06);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH1,0.00823472);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, NORM2,0.136492);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC3, WIDTH2,0.0421098);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM1,2.40193e+12);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH1,0.00692858);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, NORM2,1.00409);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC4, WIDTH2,0.0480119);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.310158);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0270507);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.131854);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0516186);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0254218);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0325152);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0206911);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0566322);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00286511);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0639471);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,4.44769e-14);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0670752);


   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.732788);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::EME3, 0.151806);




   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.802486);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, 0.133792);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0821409);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0262908);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.0491676);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0563357);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.00293731);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0668994);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,7.5928e-13);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.122158);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0494779);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0479097);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.0281899);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0678887);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0335183);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0354375);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.02873);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0416075);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.260169);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0278993);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.138288);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0538276);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.020141);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0425762);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00534125);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0702992);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.825817);
    binnedParameters->setFudgeStdDev( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, 0.120462);


    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0688615);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0457434);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,3.43349e-10);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0600987);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.18162);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0148621);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.046393);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0421547);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0489615);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0174386);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00430755);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0469846);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.128412);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.045795);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0806211);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0587385);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0293174);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0574888);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0157262);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0679661);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.097933);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0382743);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,3.64295e-09);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0575548);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,0.00161831);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.127481);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,9.51426e-08);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.127487);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.365447);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0235192);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.285213);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0417737);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0776766);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0292288);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.056905);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0451469);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00622751);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0352573);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00495057);
    binnedParameters->setShapeParam( E032point5bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0637603);
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
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.755142);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB1, 0.249741);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.755142);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB2, 0.249741);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.497009);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EMB3, 0.230458);




   // j1st = EME1
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.551292);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, 0.217655);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,17.1697);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0401579);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,2.20072e-08);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0597444);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,26.6611);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0291624);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.887267);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0597359);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,1.55657);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0277755);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.560028);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0487706);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,2.99103);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0308055);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,2.77529);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0521319);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,2.66512);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.0254938);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,1.93922);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.053815);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.625481);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0311215);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,0.252386);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0583638);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.314716);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0422712);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,3.56896e-12);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0686969);


   // j1st = EME2
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.602064);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, 0.210963);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,1.83122);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0451101);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,1.31705e-10);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0622343);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,21.4163);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0192491);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,2.81085);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.0519993);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,2.33678);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0200434);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.899966);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0507916);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,3.46396);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0348404);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,1.54784);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0597149);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,2.28154);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.040968);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,0.419366);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0801525);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.234152);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0284238);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.119022);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0582741);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0644405);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0278481);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,0.0346064);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0679875);


   // j1st = EME3
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.684607);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, 0.19063);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,0.690872);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.0538069);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.35745);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0596525);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,6.21309);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0348357);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,0.668907);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0583461);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,5.21384);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.030875);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.108633);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0703515);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,5.23298);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0450718);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,3.18086);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0568357);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,8.44261);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0329608);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,7.54286);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0506427);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.625815);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0443081);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.405756);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0559086);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.0503803);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0830347);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0147422);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0948072);


   // j1st = HEC
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.715574);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, 0.170534);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0325077);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0553121);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.000525651);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0564333);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,1.99432);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0440253);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,8.20461e-12);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0612674);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,2.65762);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0196661);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,1.01551);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.053389);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.607324);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0382615);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,7.60979e-10);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0591576);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,1.00966);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0485827);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,7.53905e-10);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0626533);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,1.6354);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0287289);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,1.30832);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0474237);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.602663);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0183);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.444639);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0424551);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.0592228);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0473027);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.0321768);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0599994);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0604553);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0641008);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0190265);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0706577);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.00871948);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0467114);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00200586);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0758134);


   // j1st = Tile
    binnedParameters->setFudgeMean( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.679973);
    binnedParameters->setFudgeStdDev( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, 0.339024);


    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.213456);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0396117);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.131631);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.049612);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.039825);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0564505);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0143661);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0686034);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.0650619);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.056402);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,0.000114417);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,98.986);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0591522);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0642597);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0117324);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0677778);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0056379);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0551043);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0026207);
    binnedParameters->setShapeParam( E032point5bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0611415);
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
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.736483);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB1, 0.103374);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,2.55656);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0238566);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.047793);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0630488);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.116552);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0190358);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0405042);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0339176);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.360666);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0279537);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.173004);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.0484063);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0620704);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0248103);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.029552);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0433724);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00909664);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0401473);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,8.27723e-07);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,99.9995);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.737127);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB2, 0.121164);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.793325);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0225965);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.356893);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0380562);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.277008);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0178025);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0256508);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0364142);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.517142);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0267396);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.245733);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0473018);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0830081);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0342771);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.0052595);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0591324);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00759877);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0363228);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00525203);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0489339);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.836932);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EMB3, 0.32422);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0494555);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0332619);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,6.60951e-06);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,99.9968);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.531752);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0244468);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.391066);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0368273);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.131848);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.033028);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,7.26426e-05);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0957363);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.0120736);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0353704);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00761955);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0464399);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::HEC, 0.793122);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::HEC, 0.0923701);




   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.793122);
    binnedParameters->setFudgeStdDev( E040bin, eta050bin, eflowFirstIntRegions::Tile, 0.0923701);


    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.046052);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0127134);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0362137);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.025991);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.855719);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0292949);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,4.70102e-05);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.149401);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.129069);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0316726);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,2.81637e-13);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0562229);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.0106514);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0442065);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,2.30774e-11);
    binnedParameters->setShapeParam( E040bin, eta050bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0583083);
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
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.744274);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB1, 0.1074);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.44783);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0265098);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.0865384);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0588334);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.85681);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0184607);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0564029);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0513543);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.114231);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.0209754);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.0199782);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.0469928);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.225685);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0259554);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,0.13702);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.053957);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.0230366);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0286984);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,0.0142804);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.0527195);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM1,0.00193847);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH1,0.0477206);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, NORM2,0.00134211);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile3, WIDTH2,0.0556959);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.720385);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB2, 0.12469);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.353477);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0267809);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0378026);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0527125);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,3.91595);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0139252);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0412311);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0428146);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.149663);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.0201602);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.0227691);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0397078);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.176577);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0257008);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,0.127238);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0522239);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.0216264);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0260765);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.013485);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.051562);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM1,0.00153898);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH1,0.0493344);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, NORM2,0.00107179);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile3, WIDTH2,0.0592676);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.617169);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EMB3, 0.10397);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0749677);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0188799);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,0.0487953);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0410363);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.41676);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0144034);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0167122);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0378064);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.493485);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0134688);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0133895);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0380128);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.336875);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0250384);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.266743);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0427605);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.0459899);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0248874);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0353669);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0411973);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00250539);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0449832);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.00165492);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0544319);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME1, 0);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME1, 0);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME2, 0);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME2, 0);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::EME3, 0);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::EME3, 0);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::HEC, 0.801637);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::HEC, 0.102975);




   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.801637);
    binnedParameters->setFudgeStdDev( E040bin, eta100bin, eflowFirstIntRegions::Tile, 0.102975);


    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0210596);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0221183);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0104663);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0518615);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.109681);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0138659);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.00628212);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.0429558);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0213271);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0130874);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.00612047);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.03129);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.661298);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0217321);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.492101);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0325431);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0887109);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0245044);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0638667);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0370044);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00516027);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0420076);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00349872);
    binnedParameters->setShapeParam( E040bin, eta100bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0548732);
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
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.764688);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB1, 0.116766);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM1,1.08783);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH1,0.0236236);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, NORM2,0.234603);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB1, WIDTH2,0.0482673);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM1,2.16095);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH1,0.0186351);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, NORM2,0.0694117);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB2, WIDTH2,0.0513125);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM1,0.119357);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH1,0.022101);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, NORM2,0.026054);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::EMB3, WIDTH2,0.048706);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM1,0.00854036);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH1,0.040712);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, NORM2,1.07039e-13);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC1, WIDTH2,0.0571563);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM1,0.00226357);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH1,0.0639878);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, NORM2,0.00194021);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::HEC2, WIDTH2,0.0836341);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM1,0.0625387);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH1,0.0610764);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, NORM2,5.73944e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile1, WIDTH2,0.066132);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM1,0.00626636);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH1,0.0718557);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, NORM2,3.39781e-07);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB1, eflowCalo::Tile2, WIDTH2,0.071878);


   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.771461);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB2, 0.117808);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM1,0.587623);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH1,0.0266326);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, NORM2,0.0229086);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB1, WIDTH2,0.0635397);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM1,7.22287);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH1,0.0138396);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, NORM2,0.0583236);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB2, WIDTH2,0.0458422);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM1,0.157128);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH1,0.018889);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, NORM2,0.046221);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::EMB3, WIDTH2,0.0400048);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM1,0.0579017);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH1,0.0557608);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, NORM2,1.35166e-08);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC1, WIDTH2,0.0613299);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM1,0.0386599);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH1,0.0489013);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, NORM2,2.42564e-12);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::HEC2, WIDTH2,0.0617679);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM1,0.0790626);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH1,0.0565141);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, NORM2,3.51948e-11);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile1, WIDTH2,0.0635159);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM1,0.00573497);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH1,0.0596388);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, NORM2,0.00267307);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB2, eflowCalo::Tile2, WIDTH2,0.0664543);


   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.758733);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EMB3, 0.110715);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM1,0.0620451);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH1,0.0464795);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, NORM2,1.75347e-13);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB1, WIDTH2,0.0611962);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM1,0.717783);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH1,0.0139014);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, NORM2,0.0338411);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB2, WIDTH2,0.0462415);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM1,0.619421);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH1,0.0146727);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, NORM2,0.0557322);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::EMB3, WIDTH2,0.0363013);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM1,0.0499271);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH1,0.0422105);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, NORM2,0.039448);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC1, WIDTH2,0.0496339);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM1,0.00979689);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH1,0.0545826);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, NORM2,0.00839733);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::HEC2, WIDTH2,0.0653428);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM1,0.371907);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH1,0.0265471);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, NORM2,0.0951484);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile1, WIDTH2,0.0539596);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM1,0.022852);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH1,0.0263101);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, NORM2,0.0165856);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile2, WIDTH2,0.0598992);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM1,0.00189298);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH1,0.0599194);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, NORM2,0.000819148);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::EMB3, eflowCalo::Tile3, WIDTH2,0.0674331);


   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.525);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME1, 0.0424557);




   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.65);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME2, 0.0833595);




   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.74744);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::EME3, 0.105609);




   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.818399);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::HEC, 0.1128);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0638695);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0443298);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,0.00148048);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.075804);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.24123);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0147351);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,0.0793);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0423738);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,0.00983298);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0460891);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,1.26699e-11);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0610802);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,0.0817091);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.0585763);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,1.76293e-06);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,79.3031);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,0.0314932);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0459884);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,0.0239755);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0568393);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.00627553);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.0721471);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,5.98998e-05);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,67.9071);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.253263);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.0278363);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.100017);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0547109);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0213451);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0378462);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.0149459);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0520612);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.841944);
    binnedParameters->setFudgeStdDev( E040bin, eta150bin, eflowFirstIntRegions::Tile, 0.106823);


    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0587582);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0192387);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.0352654);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0504939);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM1,0.173223);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH1,0.0153738);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, NORM2,0.0511389);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB2, WIDTH2,0.041745);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM1,0.0490708);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH1,0.0166002);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, NORM2,0.0048468);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::EMB3, WIDTH2,0.0433047);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.117943);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.0451658);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0762923);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0542407);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0207441);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0532978);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0153583);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0631381);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,4.01868);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0245466);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,3.29397e-11);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,0.0538779);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.381091);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.0316475);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,0.0434786);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0630169);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.0647908);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0303223);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,0.0212408);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,0.0533034);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM1,0.00566233);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH1,0.0331583);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, NORM2,0.00448386);
    binnedParameters->setShapeParam( E040bin, eta150bin, eflowFirstIntRegions::Tile, eflowCalo::Tile3, WIDTH2,0.0592627);
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
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB1, -4.61097);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB1, 5.45237);




   // j1st = EMB2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.846265);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB2, 0.327316);




   // j1st = EMB3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.602565);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EMB3, 0.228237);




   // j1st = EME1
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.639027);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME1, 0.215541);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM1,16.1626);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH1,0.0227226);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, NORM2,3.31902);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME1, WIDTH2,0.0465012);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM1,37.6345);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH1,0.0224038);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, NORM2,0.86223);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME2, WIDTH2,0.0595088);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM1,2.13407);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH1,0.0241988);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, NORM2,0.404125);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::EME3, WIDTH2,0.0516194);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM1,3.07884);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH1,0.0263253);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, NORM2,1.7487);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC1, WIDTH2,0.0558056);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM1,1.72681);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH1,0.053864);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, NORM2,3.90492e-13);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC2, WIDTH2,0.054072);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM1,0.258341);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH1,0.0507146);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, NORM2,2.12377e-13);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC3, WIDTH2,0.0620963);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM1,0.0844178);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH1,0.0254218);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, NORM2,0.0531113);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME1, eflowCalo::HEC4, WIDTH2,0.0536771);


   // j1st = EME2
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.686045);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME2, 0.200185);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM1,9.42193);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH1,0.0225529);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, NORM2,4.90117);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME1, WIDTH2,0.0410006);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM1,55.8522);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH1,0.0217293);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, NORM2,1.4129);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME2, WIDTH2,0.05486);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM1,4.00946);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH1,0.0245155);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, NORM2,0.487073);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::EME3, WIDTH2,0.0539388);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM1,4.24261);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH1,0.0300368);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, NORM2,1.90479);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC1, WIDTH2,0.0608326);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM1,3.74051);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH1,0.0329827);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, NORM2,1.58807);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC2, WIDTH2,0.0631261);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM1,0.313045);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH1,0.0342136);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, NORM2,0.184506);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC3, WIDTH2,0.0609705);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM1,0.0796352);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH1,0.0478653);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, NORM2,1.89211e-10);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME2, eflowCalo::HEC4, WIDTH2,0.0617326);


   // j1st = EME3
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.739463);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::EME3, 0.150133);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM1,1.59813);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH1,0.049383);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, NORM2,0.351873);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME1, WIDTH2,0.0520928);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM1,4.30123);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH1,0.0195888);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, NORM2,1.45007);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME2, WIDTH2,0.0519723);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM1,3.97975);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH1,0.0202376);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, NORM2,0.531105);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::EME3, WIDTH2,0.0489065);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM1,5.36792);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH1,0.0295755);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, NORM2,2.52517);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC1, WIDTH2,0.0573125);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM1,4.90468);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH1,0.0307104);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, NORM2,2.51923);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC2, WIDTH2,0.0584176);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM1,0.771217);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH1,0.0291316);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, NORM2,0.505057);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC3, WIDTH2,0.0532668);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM1,0.150823);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH1,0.0350834);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, NORM2,0.0869997);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::EME3, eflowCalo::HEC4, WIDTH2,0.0585013);


   // j1st = HEC
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.75962);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::HEC, 0.136345);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM1,0.0397358);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH1,0.0471562);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, NORM2,1.617e-14);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB1, WIDTH2,0.0621259);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM1,0.0748931);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH1,0.0381393);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, NORM2,2.82647e-12);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB2, WIDTH2,0.0597409);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM1,8.33764);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH1,0.0216372);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, NORM2,2.85007e-14);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EMB3, WIDTH2,0.0533737);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM1,2.12525);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH1,0.0461773);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, NORM2,2.24584e-10);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME1, WIDTH2,0.0611674);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM1,6.48813);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH1,0.0163932);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, NORM2,1.51896);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME2, WIDTH2,0.0478621);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM1,0.721813);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH1,0.0179903);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, NORM2,0.469273);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::EME3, WIDTH2,0.0460069);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM1,1.64751);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH1,0.029408);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, NORM2,0.754261);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC1, WIDTH2,0.0540023);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM1,2.31758);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH1,0.0304759);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, NORM2,1.1683);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC2, WIDTH2,0.0512915);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM1,0.946829);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH1,0.030877);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, NORM2,0.171966);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC3, WIDTH2,0.0532912);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM1,0.331721);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH1,0.0337231);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, NORM2,0.213361);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::HEC4, WIDTH2,0.0416921);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM1,0.0763777);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH1,0.059307);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, NORM2,0.0157257);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile1, WIDTH2,0.0637234);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM1,0.0151978);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH1,0.0383974);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, NORM2,0.00716483);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::HEC, eflowCalo::Tile2, WIDTH2,0.0604692);


   // j1st = Tile
    binnedParameters->setFudgeMean( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.733456);
    binnedParameters->setFudgeStdDev( E040bin, eta250bin, eflowFirstIntRegions::Tile, 0.106712);


    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM1,0.0484006);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH1,0.0526203);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, NORM2,0.00444209);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::EMB1, WIDTH2,0.0690316);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM1,0.172758);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH1,0.047481);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, NORM2,0.0879408);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC1, WIDTH2,0.0548595);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM1,0.0292301);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH1,0.0528181);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, NORM2,0.0142219);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC2, WIDTH2,0.0745486);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM1,0.701334);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH1,0.0291106);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, NORM2,2.03086e-06);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC3, WIDTH2,97.6695);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM1,0.0767133);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH1,0.0664772);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, NORM2,1.82064e-05);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::HEC4, WIDTH2,0.0677194);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM1,0.0640297);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH1,0.064925);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, NORM2,1.90208e-11);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile1, WIDTH2,0.0694147);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM1,0.00830602);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH1,0.0608681);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, NORM2,1.90624e-07);
    binnedParameters->setShapeParam( E040bin, eta250bin, eflowFirstIntRegions::Tile, eflowCalo::Tile2, WIDTH2,92.2594);

  }

  return StatusCode::SUCCESS;

}

StatusCode eflowCellEOverPTool_mc12::finalize(){
  return StatusCode::SUCCESS;
}
