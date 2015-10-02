/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpecialPixelGenerator.cxx
//   Implementation file for class SpecialPixelGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelDigitizationTool.h"
#include "SpecialPixelGenerator.h"
#include "TimeSvc.h"

#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiHelper.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimEvent/SiCharge.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"

using namespace InDetDD;

static const InterfaceID IID_ISpecialPixelGenerator("SpecialPixelGenerator", 1, 0);
const InterfaceID& SpecialPixelGenerator::interfaceID( ){ return IID_ISpecialPixelGenerator; }

SpecialPixelGenerator::SpecialPixelGenerator(const std::string& type, const std::string& name,const IInterface* parent ):
  AthAlgTool(type,name,parent),
  m_npixTot(0),
  m_nmodTot(0),
  m_doUpdate(true),
  m_pixelID(0),
  m_usePixCondSum(true),
  m_pixelConditionsSummarySvc("PixelConditionsSummarySvc",name),
  m_spmNoiseProb(1e-5),
  m_spmNoBumpProb(5e-4),
  m_spmDisableProb(2e-5),
  m_spmBadTOTProb(5e-4),
  m_specialPixelMapSvc("SpecialPixelMapSvc",name),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"), 
  m_CalibSvc("CalibSvc",name),
  m_rndmEngine(0)
{
  declareInterface< SpecialPixelGenerator >( this );
  
  declareProperty("SpmNoiseProb",       m_spmNoiseProb,         "Special Pixels map gen: probability for a noisy pixel");
  declareProperty("SpmNoBumpProb",      m_spmNoBumpProb,        "Special Pixels map gen: probability for pixel with out bump");
  declareProperty("SpmDisableProb",     m_spmDisableProb,       "Special Pixels map gen: probability for a disabled pixel");
  declareProperty("SpmBadTOTProb",      m_spmBadTOTProb,        "Special Pixels map gen: probability for corrupted TOT");
  declareProperty("SpecialPixelMapSvc",m_specialPixelMapSvc);
  declareProperty("UsePixCondSum",      m_usePixCondSum,        "Use PixelConditionsSummarySvc");
  declareProperty("PixelConditionsSummarySvc",m_pixelConditionsSummarySvc);
  declareProperty("RndmSvc",         m_rndmSvc,          "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("RndmEngine",      m_rndmEngineName,   "Random engine name");
  declareProperty("CalibSvc",m_CalibSvc,"Calib Svc");
}

// Destructor:
SpecialPixelGenerator::~SpecialPixelGenerator()
{}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode SpecialPixelGenerator::initialize() {
  StatusCode sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "SpecialPixelGenerator::initialize() failed" );
    return sc ;
  }
  ATH_MSG_DEBUG ( "SpecialPixelGenerator::initialize()" );
  //  
  // get PixelID
  //
  if ( detStore()->retrieve(m_pixelID,"PixelID").isFailure() ) {
    // if this fails, it's probably a bug -> FATAL!
    ATH_MSG_FATAL ( "Could not get Pixel ID helper" );
    return StatusCode::FAILURE;
  }
  m_npixTot = m_pixelID->pixel_hash_max();
  m_nmodTot = m_pixelID->wafer_hash_max();

  if (m_CalibSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get CalibSvc");
	return StatusCode::FAILURE;
  }
  //
  // get SpecialPixelMapSvc
  //
  if (m_CalibSvc->usePixMapCDB()) {
    if ( ! m_specialPixelMapSvc.retrieve().isSuccess() ) {
      ATH_MSG_FATAL ( "Unable to retrieve SpecialPixelMapSvc" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ( "Retrieved SpecialPixelMapSvc" );
  } 

  //
  // get PixelConditionsSummarySvc
  //
  if (m_usePixCondSum) {
    if ( m_pixelConditionsSummarySvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL ( "Unable to retrieve PixelConditionsSummarySvc" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ( "Retrieved PixelConditionsSummarySvc" );
  }

  //
  // get the Rndmsvc
  //
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( " Can't get RndmSvc " );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( "Retrieved RndmSvc" );
 
  }
  //
  // get the PixelDigitizationTool random stream
  //
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  }
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode SpecialPixelGenerator::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "SpecialPixelGenerator::finalize() failed" );
    return sc ;
  }
  ATH_MSG_DEBUG ( "SpecialPixelGenerator::finalize()" );
  return sc ;
}

void SpecialPixelGenerator::updatePixelMap(){
  // update pixel map
  if (m_CalibSvc->usePixMapCDB()) {
    ATH_MSG_DEBUG ( "retrieving special pixel map from cond db" );
    
    m_detectorMap =  m_specialPixelMapSvc->getPixelMap(0);
    if (m_detectorMap == 0) {
      ATH_MSG_ERROR ( "BUG TRAP - special pixelmap ptr == 0!!!" );
    }

  } else {
    if (m_doUpdate) {
      ATH_MSG_VERBOSE ( "generating a random special pixel map" );
      generatePixelMap();
      m_doUpdate = false;
    }
  }
}

void SpecialPixelGenerator::generatePixelMap() const {
  //
  // Generates a random pixel map
  //
  // first clear the current map
  ATH_MSG_VERBOSE ( "generating a new SpecialPixelMap" );
  ATH_MSG_VERBOSE ( "map size = " << m_detectorMapGen.size() );
  if (m_detectorMapGen.size()>0)
    m_detectorMapGen.clear();
  
  m_detectorMapGen.resize(m_pixelID->wafer_hash_max());

  for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++){
    m_detectorMapGen[i] = new ModuleSpecialPixelMap();
  }

  //
  // Get number of pixels
  //
//   const int nLayer  = 3;
//   int nchips        = 16;
//   int nphibarrel[nLayer] = {22,38,52};
//   int nphidisks     = 48;
//   int netabarrel    = 13;
//   //
//   int nphipixel     = 160;
//   int nphipixgang   = 8;
//   int netapixel     = 18;
//   //
//   int ndisks        = nLayer*2; // 3 disks per side
//   int nmodules      = (nphibarrel[0]+nphibarrel[1]+nphibarrel[2])*netabarrel + nphidisks*ndisks;
//   int npixpermod    = nchips*nphipixel*netapixel;

  //
  // Pull the number of special pixels
  //
  unsigned int status;
  int npix;
  //
  // Fill special pixels - TODO: match this with the offline specifications!
  // Several new probabilities are needed.
  //
  ATH_MSG_VERBOSE ( "filling special pixels" );
  status = (1 << 0) + (1<<15);  // unusable and noisy
  npix = fillSpecialPixels(m_spmNoiseProb,status, true);
  ATH_MSG_VERBOSE ( "generated " << npix << " noisy pixels" );

  status = (1 << 0) + (1<<9);   // unusable and disconnected bump
  npix = fillSpecialPixels(m_spmNoBumpProb,status, true);
  ATH_MSG_VERBOSE ( "generated " << npix << " pixels with no bump" );

  status = (1 << 0) + (1<<16);  // unusable and unknown dead
  npix = fillSpecialPixels(m_spmDisableProb,status, true);
  ATH_MSG_VERBOSE ( "generated " << npix << " disabled pixels" );

  status = (1 << 0) + (1<<14);  // unusable and bad TOT
  npix = fillSpecialPixels(m_spmBadTOTProb,status, true);
  ATH_MSG_VERBOSE ( "generated " << npix << " pixels with corrupt TOT" );
  // Special pixel map is now filled!
  
}

int SpecialPixelGenerator::fillSpecialPixels( double prob, unsigned int status, bool merge ) const {
  // total number of pixels
  //m_npixTot = m_pixelID->pixel_hash_max();

  // total number of modules
  //m_nmodTot = m_pixelID->wafer_hash_max();



  // fills randomly special pixels according to given probability
  // returns the number of special pixels created (or merged)
  if (!((prob<1.0) || (prob>0.0))) return 0;

  // first get the number of special pixel (Poisson)
  unsigned int npix = CLHEP::RandPoisson::shoot( m_rndmEngine, m_npixTot*prob);
  if (npix>m_npixTot) npix=m_npixTot;
  // keep a vector with used hashes, we want to find npix unique pixels

  // loop over all pixels
  for (unsigned int i=0; i<npix; i++) {

    // first select module hash
    int hash = CLHEP::RandFlat::shootInt(m_rndmEngine,m_nmodTot);
    IdentifierHash idHash = hash;
    if (!idHash.is_valid()) {
      ATH_MSG_ERROR ( "module IdHash NOT OK" );
    }
    Identifier idModuleFull;
//     IdContext modContext = m_pixelID->wafer_context();
//     if (0 != m_pixelID->get_id(idHash, idModuleFull,&modContext)) {
//       ATH_MSG_ERROR ( "failed making full Id of IdHash!" );
//     }

    // TODO: these constants should NOT BE HARDCODED!!! FIX THIS!
    const int nChips = 16;
    const int nCols  = 18;
    const int nRows  = 164;

    // with a given module hash, select FE, row and col
    int pixChip = CLHEP::RandFlat::shootInt(m_rndmEngine,nChips);
    int pixCol  = CLHEP::RandFlat::shootInt(m_rndmEngine,nCols);
    int pixRow  = CLHEP::RandFlat::shootInt(m_rndmEngine,nRows);
    //
    unsigned int pixelID;
    pixelID  = ModuleSpecialPixelMap::encodePixelID( pixChip, pixCol, pixRow );

    ATH_MSG_VERBOSE ( "setting status" );
    // set status
    if (merge) {
      unsigned int oldstat = m_detectorMapGen[idHash]->pixelStatus(pixelID);
      status |= oldstat;
    }
    m_detectorMapGen[idHash]->setPixelStatus(pixelID,status);
    ATH_MSG_VERBOSE ( "done!" );
  }
  //
  return npix;
}

void SpecialPixelGenerator::process(SiChargedDiodeCollection &collection) const {
  // 
  // Call maskDead method, which checks the module status from PixelConditionsSummarySvc
  // and disable diodes if the module is not active. This sould be re-coded elsewhere...
  // F.Djama April 30 2008.
  //
  if (m_usePixCondSum) maskDead(collection);
  //
  // process diode collection and mark those being special
  // if needed, modify their charges

  // first loop over all diodes in collection and disable those that are
  // masked bad in the special pixel map

  Identifier     pixid=collection.identify();    // pixel Identifier from the charge collection
  IdentifierHash moduleHash = m_pixelID->wafer_hash(pixid); // wafer hash
  SiChargedDiode *diode;    // second is a SiChargedDiode
  
  unsigned int   pixelID;  // numeric pixel id used in SpecialPixelMap
  //
  ATH_MSG_VERBOSE ( "Processing diodes for module " 
	<< m_pixelID->show_to_string(pixid) );
  // retrieve the special pixel map for this module, 
  // either from condition DB or from the internally generated one
  getID(pixid, pixelID);
  const ModuleSpecialPixelMap *modmap=0;
  if (m_CalibSvc->usePixMapCDB()) modmap = m_detectorMap->module(moduleHash);
  else modmap=m_detectorMapGen.module(moduleHash);
  if ( !modmap ) return; // no special pixels for this module: nothing to do!
  // process hits
  for(SiChargedDiodeIterator i_chargedDiode=collection.begin();
      i_chargedDiode!=collection.end(); ++i_chargedDiode) {
    // first is an Identifier
    pixid = collection.getId((*i_chargedDiode).first);
    diode = &((*i_chargedDiode).second);
    // find diode with id pixid in special pixel map
    getID( pixid, pixelID );
    unsigned int status = modmap->pixelStatus(pixelID);
    bool pixuseful = modmap->pixelUseful(pixelID);
    // DO NOT mask out pixels that are not useful (3rd argument of Sihelper method call = false
    // JUST set the correspondin word (2nd argument = true)
    if (!pixuseful) {
      bool setbad=false;
      if (status & (1<<15)) { //noisy
        SiHelper::noise(*diode,true,false);
        setbad=true;
      }
      if (status & (1<<9)) { // disconn
        SiHelper::disconnected(*diode,true,false);
        setbad=true;
      }
      if (status & (1<<16)) { // unknown dead
        SiHelper::disabled(*diode,true,false);
        setbad=true;
      }
      if (status & (1<<14)) { // bad TOT
        SiHelper::badToT(*diode,true,false);
        setbad=true;
      }
      if ((status>0) && (!setbad)) { // if none of the above, disable the pixel
        SiHelper::disabled(*diode,true,false);
        setbad=true;
      }
    }

//     ATH_MSG_DEBUG ( "ModulePixel: "
//           << moduleID << "   "
//           << pixelID << " -> "
//           << status );
  }
  ATH_MSG_VERBOSE ( "Processing done!" );
}

void SpecialPixelGenerator::getID( const Identifier & id, unsigned int & pixID ) const {
  // get pixel id from a pixel identifier
  // pixID is as follows:
  //   |rrrr|rrrc|cccc|ffff|
  // r = row [0,163]
  // c = col [0,17]
  // f = FE [0,15]
  int          endcap    = m_pixelID->barrel_ec(id );  // barrel or endcap
  unsigned int phimod    = m_pixelID->phi_module(id ); // module phi
  unsigned int phipix    = m_pixelID->phi_index(id );  // pixel phi
  unsigned int etapix    = m_pixelID->eta_index(id );  // pixel eta
  //
  pixID = ModuleSpecialPixelMap::encodePixelID( endcap, phimod, etapix, phipix );
}

const DetectorSpecialPixelMap* SpecialPixelGenerator::getDetectorMap() const{
  if(m_CalibSvc->usePixMapCDB()) return m_detectorMap;
  else return  &m_detectorMapGen; 
}

void SpecialPixelGenerator::maskDead(SiChargedDiodeCollection &collection) const {
  // 
  // Checks the module status from PixelConditionsSummarySvc
  // and disable diodes if the module is not active. This sould be re-coded elsewhere...
  // F.Djama April 30 2008.

  const Identifier     pixid=collection.identify();    // pixel Identifier from the charge collection
  const IdentifierHash moduleHash = m_pixelID->wafer_hash(pixid); // wafer hash
  //
  ATH_MSG_VERBOSE ( "Processing diodes for module " 
	<< m_pixelID->show_to_string(pixid) );
  //
  // process hits
  for(SiChargedDiodeIterator i_chargedDiode=collection.begin();
      i_chargedDiode!=collection.end(); ++i_chargedDiode) {
    bool active = m_pixelConditionsSummarySvc->isActive(moduleHash,collection.getId((*i_chargedDiode).first));
    SiChargedDiode* diode = &((*i_chargedDiode).second);
    if (!active) {
      SiHelper::disabled(*diode,true,true);
    }
  }
}  
