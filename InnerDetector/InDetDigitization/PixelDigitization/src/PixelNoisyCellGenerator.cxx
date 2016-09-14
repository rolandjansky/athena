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
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TimeSvc.h"
#include "CalibSvc.h"

#include<fstream>
#include<sstream>
#include<string>
#include<limits>
#include<iomanip>

//#define __PIXEL_DEBUG__

using namespace InDetDD;

static const InterfaceID IID_IPixelNoisyCellGenerator("PixelNoisyCellGenerator", 1, 0);
const InterfaceID& PixelNoisyCellGenerator::interfaceID( ){ return IID_IPixelNoisyCellGenerator; }

PixelNoisyCellGenerator::PixelNoisyCellGenerator(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_TimeSvc("TimeSvc",name),
  m_CalibSvc("CalibSvc",name),
  m_mergeCharge(false),
  m_pixelID(0),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0),
  m_spmNoiseOccu(1e-5),
  m_rndNoiseProb(5e-8)
{
  declareInterface< PixelNoisyCellGenerator >( this );
  declareProperty("NoiseShape",m_noiseShape,"Vector containing noise ToT shape");
  declareProperty("RndmSvc",         m_rndmSvc,          "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("RndmEngine",      m_rndmEngineName,   "Random engine name");	
  declareProperty("MergeCharge",     m_mergeCharge,      "");
  declareProperty("SpmNoiseOccu",    m_spmNoiseOccu,         "Special Pixels map gen: probability for a noisy pixel in SPM");
  declareProperty("RndNoiseProb",       m_rndNoiseProb,         "Random noisy pixels, amplitude from calib. - NOT special pixels!"); 
}

// Destructor:
PixelNoisyCellGenerator::~PixelNoisyCellGenerator()
{}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelNoisyCellGenerator::initialize() {
  StatusCode sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelNoisyCellGenerator::initialize() failed");
    return sc ;
  }
  
  if (m_TimeSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get TimeSvc");
	return StatusCode::FAILURE;
  }
  if (m_CalibSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get CalibSvc");
	return StatusCode::FAILURE;
  }
  if (m_rndmSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get RndmSvc");
	return StatusCode::FAILURE;
  }
  else {
	ATH_MSG_DEBUG("Retrieved RndmSvc");
  }
 
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
	ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
	return StatusCode::FAILURE;
  } else { 
	ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  }
  

  //  
  // get PixelID
  //
  if ( detStore()->retrieve(m_pixelID,"PixelID").isFailure() ) {
    // if this fails, it's probably a bug -> FATAL!
    ATH_MSG_FATAL ( "Could not get Pixel ID helper" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "PixelNoisyCellGenerator::initialize()");
  return StatusCode::SUCCESS;

}

  


//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelNoisyCellGenerator::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelNoisyCellGenerator::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelNoisyCellGenerator::finalize()");
  return sc ;
}

// process the collection of diode collection
void PixelNoisyCellGenerator::process(SiChargedDiodeCollection &collection) const
{
  //
  // Get Module identifier
  //
  Identifier mmod = collection.identify();
  ATH_MSG_DEBUG ( " Processing diodes for module "
	<< m_pixelID->show_to_string(mmod));
  //
  // if probability is 0, do nothing
  //

  double pnoise_rndm(m_rndNoiseProb);
  if (pnoise_rndm>0) addRandomNoise(collection,pnoise_rndm);
  
  double pnoise_spec(m_spmNoiseOccu);
  
  if (pnoise_spec>0) addNoisyPixels(collection,pnoise_spec);
  
  return;
}
 
void PixelNoisyCellGenerator::addNoisyPixels(SiChargedDiodeCollection &collection, double occupancy) const {
  //
  // Get Module identifier
  //
  Identifier mmod = collection.identify();
  IdentifierHash moduleHash = m_pixelID->wafer_hash(mmod);
  
  int ecb = m_pixelID->barrel_ec(mmod);
  int phi = m_pixelID->phi_module(mmod);

  // number of bunch crossings
  int bcn = m_TimeSvc->getTimeBCN();

  //
  // get pixel module design and check it
  //
  const PixelModuleDesign *p_design = dynamic_cast<const PixelModuleDesign*>(&(collection.design()));
  if (!p_design) return;

  //
  // compute number of noisy cells
  // multiply the number of pixels with BCN since noise will be evenly distributed
  // over time
  //
  int number_spec=CLHEP::RandPoisson::shoot(m_rndmEngine,
				      p_design->numberOfCircuits()    // =8
				      *p_design->columnsPerCircuit()  // =18
				      *p_design->rowsPerCircuit()     // =320
				      *occupancy
                                      *static_cast<double>(bcn));

  unsigned int imod = moduleHash;
  ATH_MSG_DEBUG ( " Generating noisy pixels for module " << imod << "using map " << m_noisyPixel);
  std::map<unsigned int,std::vector<unsigned int> >::const_iterator mapit = m_noisyPixel->find(imod);
  std::vector<unsigned int> noisypixel;
  
  if (mapit != m_noisyPixel->end()) {
    noisypixel = mapit->second;
  } 
  int isize = noisypixel.size(); 
  bool allNoisy = false;
  ATH_MSG_DEBUG ( " Size of noisy SPM = " << isize << ", number of needed pixels = " << number_spec);
  if(number_spec > isize){
    number_spec=isize;
    allNoisy = true;
  }  

  //
  // create the noisy pixels
  //
  ATH_MSG_DEBUG ( " Size of noisy SPM = " << isize << ", number of available pixels = " << number_spec);
  for(int i=0 ; i<number_spec; i++) {
    //
    // find a random readout cell
    //
    int irank;
    if(allNoisy){
      irank = i;
    }
    else {	
      irank = CLHEP::RandFlat::shootInt(m_rndmEngine,isize);
    }
    unsigned int noise_pixelID = noisypixel[irank];
    ATH_MSG_DEBUG ( " Noisy pixel = " << noise_pixelID);
    ATH_MSG_DEBUG ( "********************** 0 ");
#ifdef __PIXEL_DEBUG__
    ATH_MSG_DEBUG ( "********************** 1 ");
    ATH_MSG_DEBUG ( "Adding noisy pixel hit on: " 
	  << std::hex << noise_pixelID << std::dec);
    ATH_MSG_DEBUG ( "********************** 1b ");
#endif
    ATH_MSG_DEBUG ( "********************** 2 ");
    int chip = (noise_pixelID & 0xF);
    ATH_MSG_DEBUG ( "chip = " << chip);
    noise_pixelID >>=4;
    int column = (noise_pixelID & 0x1F);
    ATH_MSG_DEBUG ( "column = " << column);
    noise_pixelID >>=5;
    int row = noise_pixelID;
    ATH_MSG_DEBUG ( "row = " << row);
    int circuit;
    if(chip>=p_design->numberOfCircuits()) {
      circuit=chip-p_design->numberOfCircuits();
    } else {
      circuit= p_design->numberOfCircuits()-chip-1;
      column = p_design->columnsPerCircuit()-column-1;
      row    = p_design->rowsPerCircuit()-row-1;
    } 

    ATH_MSG_DEBUG ( "number of circuits = " << p_design->numberOfCircuits() );
    ATH_MSG_DEBUG ( "columns per circuits = " << p_design->columnsPerCircuit() );
    ATH_MSG_DEBUG ( "rows per circuits = " << p_design->rowsPerCircuit() );
    ATH_MSG_DEBUG ( "circuit = " << circuit << ", column = " << column << ", row = " << row);
    if ( (ecb!=0) && (phi%2==0) ) row=p_design->rowsPerCircuit()-row-1;
    ATH_MSG_DEBUG ( "circuit = " << circuit << ", column = " << column << ", row = " << row);
    addCell(collection,p_design,circuit,column,row);

    ATH_MSG_DEBUG ( "PixelID = "<<noise_pixelID<<" circuit= "<<circuit<<" column= "<<column<<" row= "<<row);
  }
}

void PixelNoisyCellGenerator::addRandomNoise(SiChargedDiodeCollection &collection, double occupancy) const {
  //
  // Get Module identifier
  //
  //Identifier mmod = collection.identify(); //not used
  
  // number of bunch crossings
  int bcn = m_TimeSvc->getTimeBCN();

  //
  // get pixel module design and check it
  //
  const PixelModuleDesign *p_design = dynamic_cast<const PixelModuleDesign *>(&(collection.design()));
  if (!p_design) return;

  //
  // compute number of noisy cells
  // multiply the number of pixels with BCN since noise will be evenly distributed
  // over time
  //
  int number_rndm=CLHEP::RandPoisson::shoot(m_rndmEngine,
                                      p_design->numberOfCircuits()    // =8
				      *p_design->columnsPerCircuit()  // =18
				      *p_design->rowsPerCircuit()     // =320
				      *occupancy
                                      *static_cast<double>(bcn));
  for (int i=0; i<number_rndm; i++) {
    int circuit = CLHEP::RandFlat::shootInt(m_rndmEngine,p_design->numberOfCircuits());
    int column  = CLHEP::RandFlat::shootInt(m_rndmEngine,p_design->columnsPerCircuit());
    int row     = CLHEP::RandFlat::shootInt(m_rndmEngine,p_design->rowsPerCircuit());
    addCell(collection,p_design,circuit,column,row);
  }
  return;
}

void PixelNoisyCellGenerator::addCell(SiChargedDiodeCollection &collection,const PixelModuleDesign *design, int circuit, int column, int row) const {
    ATH_MSG_DEBUG ( "addCell 1 circuit = " << circuit << ", column = " << column << ", row = " << row);
#ifdef __PIXEL_DEBUG__
  ATH_MSG_DEBUG ( "addCell: circuit,column,row="
	<< circuit << "," << column << "," << row);
#endif
    ATH_MSG_DEBUG ( "addCell 2 circuit = " << circuit << ", column = " << column << ", row = " << row);
  if (row>159 && design->getReadoutTechnology() == PixelModuleDesign::FEI3) row = row+8; // jump over ganged pixels - rowsPerCircuit == 320 above
    ATH_MSG_DEBUG ( "addCell 3 circuit = " << circuit << ", column = " << column << ", row = " << row);

  SiReadoutCellId roCell(row, design->columnsPerCircuit() * circuit + column);
     ATH_MSG_DEBUG ( "addCell 4 circuit = " << circuit << ", column = " << column << ", row = " << row);
  Identifier noisyID=collection.element()->identifierFromCellId(roCell);
    ATH_MSG_DEBUG ( "addCell 5 circuit = " << circuit << ", column = " << column << ", row = " << row);
  //
  // if required, check if the cell is already hit
  //
  if (!m_mergeCharge) {
    if (collection.AlreadyHit(noisyID)) {
      roCell = SiReadoutCellId(); // Set it to an Invalid ID
    }
  }
  //
  // create the diode if the cell is ok
  // p_cell is NOT ok only if:
  // * the newObject() call above failed
  // * no charge merging is allowed and cell already hit
  //
    ATH_MSG_DEBUG ( "addCell 6 circuit = " << circuit << ", column = " << column << ", row = " << row);
  if (roCell.isValid()) {
    ATH_MSG_DEBUG ( "addCell 7 circuit = " << circuit << ", column = " << column << ", row = " << row);
    SiCellId diode = roCell;
    ATH_MSG_DEBUG ( "addCell 7a circuit = " << circuit << ", column = " << column << ", row = " << row);
    //
    // create a random charge following the ToT shape of the noise measured in automn 2006 in EndCapA 
    //
    double ToT = getNoiseToT(); 
    ATH_MSG_DEBUG ( "addCell 7b circuit = " << circuit << ", column = " << column << ", row = " << row);
    //
    // now, transform the noise ToT to charge. Kind of "inverted calibration"...:
    double totA = m_CalibSvc->getCalQ2TotA( noisyID );
    double totE = m_CalibSvc->getCalQ2TotE( noisyID );
    double totC = m_CalibSvc->getCalQ2TotC( noisyID );
    ATH_MSG_DEBUG ( "addCell 7c circuit = " << circuit << ", column = " << column << ", row = " << row);
    const double chargeShape = (totA*totE - ToT*totC)/(ToT-totA); 
    ATH_MSG_DEBUG ( "addCell 7d circuit = " << circuit << ", column = " << column << ", row = " << row);
//    const double chargeGauss = chargeOfs + chargeVar*CLHEP::RandGaussZiggurat::shoot( m_rndmEngine );
    //
    // add this charge to the collection
    // if (mergeCharge) and p_diode is already hit, the charge is merged
    // the add() will create a SiChargedDiode (if it does not exist).
    // if a new one is created, the flag is not set (==0). 
    //
    // Use chargeShape rather than chargeGauss
    //
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
