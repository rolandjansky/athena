/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelNoisyCellGenerator.cxx
//   Implementation file for class PixelNoisyCellGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelNoisyCellGenerator.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"

#include<fstream>
#include<sstream>
#include<string>
#include<limits>
#include<iomanip>

PixelNoisyCellGenerator::PixelNoisyCellGenerator(const std::string& type, const std::string& name,const IInterface* parent):
  PixelProcessorTool(type,name,parent),
  m_pixelCalibSvc("PixelCalibSvc", name),
  m_timeBCN(1),
  m_mergeCharge(false),
  m_pixelID(0),
  m_rndNoiseProb(5e-8)
{
	declareProperty("TimeBCN",      m_timeBCN,      "Number of BCID");	
  declareProperty("NoiseShape",   m_noiseShape,   "Vector containing noise ToT shape");
  declareProperty("MergeCharge",  m_mergeCharge,  "");
  declareProperty("RndNoiseProb", m_rndNoiseProb, "Random noisy pixels, amplitude from calib. - NOT special pixels!"); 
}

PixelNoisyCellGenerator::~PixelNoisyCellGenerator() {}

StatusCode PixelNoisyCellGenerator::initialize() {
  CHECK(PixelProcessorTool::initialize());
 
  CHECK(m_pixelCalibSvc.retrieve());
  ATH_MSG_DEBUG("Retrieved PixelCalibSvc");

  CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  CHECK(detStore()->retrieve(m_pixMgr,"Pixel"));

  ATH_MSG_DEBUG("PixelNoisyCellGenerator::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelNoisyCellGenerator::finalize() {
  ATH_MSG_DEBUG("PixelNoisyCellGenerator::finalize()");
  return StatusCode::SUCCESS;
}

void PixelNoisyCellGenerator::process(SiChargedDiodeCollection &collection) {

  // Get Module identifier
  Identifier mmod = collection.identify();
  ATH_MSG_DEBUG(" Processing diodes for module " << m_pixelID->show_to_string(mmod));

  double pnoise_rndm(m_rndNoiseProb);
  if (pnoise_rndm>0) addRandomNoise(collection,pnoise_rndm);

  return;
}

void PixelNoisyCellGenerator::addRandomNoise(SiChargedDiodeCollection &collection, double occupancy) const {
 
  // get pixel module design and check it
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign *>(&(collection.design()));

  // compute number of noisy cells
  // multiply the number of pixels with BCN since noise will be evenly distributed
  // over time
  int number_rndm=CLHEP::RandPoisson::shoot(m_rndmEngine,
      p_design->numberOfCircuits()    // =8
      *p_design->columnsPerCircuit()  // =18
      *p_design->rowsPerCircuit()     // =320
      *occupancy
      *static_cast<double>(m_timeBCN));

  for (int i=0; i<number_rndm; i++) {
    int circuit = CLHEP::RandFlat::shootInt(m_rndmEngine,p_design->numberOfCircuits());
    int column  = CLHEP::RandFlat::shootInt(m_rndmEngine,p_design->columnsPerCircuit());
    int row     = CLHEP::RandFlat::shootInt(m_rndmEngine,p_design->rowsPerCircuit());
    addCell(collection,p_design,circuit,column,row);
  }
  return;
}

void PixelNoisyCellGenerator::addCell(SiChargedDiodeCollection &collection,const InDetDD::PixelModuleDesign *design, int circuit, int column, int row) const {
  ATH_MSG_DEBUG("addCell 1 circuit = " << circuit << ", column = " << column << ", row = " << row);
#ifdef __PIXEL_DEBUG__
  ATH_MSG_DEBUG("addCell: circuit,column,row=" << circuit << "," << column << "," << row);
#endif
  ATH_MSG_DEBUG("addCell 2 circuit = " << circuit << ", column = " << column << ", row = " << row);

  if ( row > 159 && design->getReadoutTechnology() == InDetDD::PixelModuleDesign::FEI3 ) row = row+8; // jump over ganged pixels - rowsPerCircuit == 320 above
  ATH_MSG_DEBUG("addCell 3 circuit = " << circuit << ", column = " << column << ", row = " << row);

  InDetDD::SiReadoutCellId roCell(row, design->columnsPerCircuit() * circuit + column);
  ATH_MSG_DEBUG("addCell 4 circuit = " << circuit << ", column = " << column << ", row = " << row);

  Identifier noisyID=collection.element()->identifierFromCellId(roCell);
  ATH_MSG_DEBUG("addCell 5 circuit = " << circuit << ", column = " << column << ", row = " << row);

  // if required, check if the cell is already hit
  if (!m_mergeCharge) {
    if (collection.AlreadyHit(noisyID)) {
      roCell = InDetDD::SiReadoutCellId(); // Set it to an Invalid ID
    }
  }

  // create the diode if the cell is ok
  // p_cell is NOT ok only if:
  // * the newObject() call above failed
  // * no charge merging is allowed and cell already hit
  ATH_MSG_DEBUG("addCell 6 circuit = " << circuit << ", column = " << column << ", row = " << row);
  if (roCell.isValid()) {
    ATH_MSG_DEBUG("addCell 7 circuit = " << circuit << ", column = " << column << ", row = " << row);
    InDetDD::SiCellId diode = roCell;
    ATH_MSG_DEBUG("addCell 7a circuit = " << circuit << ", column = " << column << ", row = " << row);

    // create a random charge following the ToT shape of the noise measured in automn 2006 in EndCapA 
    double ToT = getNoiseToT(); 
    ATH_MSG_DEBUG ( "addCell 7b circuit = " << circuit << ", column = " << column << ", row = " << row);

    ATH_MSG_DEBUG ( "addCell 7c circuit = " << circuit << ", column = " << column << ", row = " << row);
    double chargeShape = m_pixelCalibSvc->getCharge(noisyID,ToT);
    ATH_MSG_DEBUG ( "addCell 7d circuit = " << circuit << ", column = " << column << ", row = " << row);
    //    const double chargeGauss = chargeOfs + chargeVar*CLHEP::RandGaussZiggurat::shoot( m_rndmEngine );

    // add this charge to the collection
    // if (mergeCharge) and p_diode is already hit, the charge is merged
    // the add() will create a SiChargedDiode (if it does not exist).
    // if a new one is created, the flag is not set (==0). 
    //
    // Use chargeShape rather than chargeGauss

    ATH_MSG_DEBUG ( "addCell 8 circuit = " << circuit << ", column = " << column << ", row = " << row);
    collection.add(diode,SiCharge(chargeShape,0,SiCharge::noise));
    ATH_MSG_DEBUG ( "addCell 9 circuit = " << circuit << ", column = " << column << ", row = " << row);
  }
  return;
}

double PixelNoisyCellGenerator::getNoiseToT() const {
//
// Generate a noise ToT according to the ToT noise distribution seen under cosmic tests.
//
  ATH_MSG_DEBUG("Beginning getNoiseToT()");
  double x = CLHEP::RandFlat::shoot(m_rndmEngine,0.,1.);
  ATH_MSG_DEBUG ( " x = " << x );
  int bin=0;
  ATH_MSG_DEBUG("size = " << m_noiseShape.size()); 
  for(int i=1; i<256; i++){
    ATH_MSG_DEBUG ( " i = " << i << " noiseshape = " << m_noiseShape[i] );
    if(x < m_noiseShape[i] && x > m_noiseShape[i-1]) bin = i - 1; 
  }
  double ToTm = bin + 1.5;
  double ToT = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,ToTm,1.);
  return ToT;
  ATH_MSG_DEBUG("Ending getNoiseToT()");
}
