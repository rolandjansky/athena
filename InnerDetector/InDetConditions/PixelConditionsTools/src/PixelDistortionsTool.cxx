/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelDistortionsTool.h"
#include "InDetIdentifier/PixelID.h"
#include "Identifier/Identifier.h"


#include "CLHEP/Units/SystemOfUnits.h"
#include "DetDescrConditions/DetCondCFloat.h"
#include "PathResolver/PathResolver.h"

#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>


// DB object version numbers:
//  0: Old code which had wrong sign in phi correction.
//  1: Fix sign of phi correction.



PixelDistortionsTool::PixelDistortionsTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
  :  AthAlgTool(type,name,parent), 
     m_pixelid(0),
     m_rndmEngine(0),
     m_rndmSvc(0),
     m_distortions(0),
     m_doCorrections(false),  
     m_lastDisto(0),
     m_lastID(),                // Invalid Id.
     m_ownDistortions(true),
     m_version(1) // db object version number 
{

  declareInterface<IModuleDistortionsTool>(this);
  
  declareProperty("InputSource",m_inputSource=4,"Source of module distortions: 0 (none), 1 (constant), 2 (text file), 3 (random), 4 (database)");
  declareProperty("TextFileName",m_textFileName="PixelSurveyDistortions.txt","Read distortions from this file"); 
  declareProperty("FolderName",m_folderName="/Indet/PixelDist", "Folder name. Should not be changed."); 
  declareProperty("WriteData",m_write=false, "Record data in storegate"); 
  declareProperty("R1",m_R1=0.1/CLHEP::meter,"Fixed distortion parameters: radius of curvature");//corresponding to a sagitta of 50 um
  declareProperty("R2",m_R2=0.1/CLHEP::meter,"Fixed distortion parameters: radius of curvature");//corresponding to a sagitta of 50 um
  declareProperty("tantwist",m_twist=0.0005,"Fixed distortion parameters: twist angle"); 
  declareProperty("RndmEngine",m_rndmEngineName,"Random distortion parameters: engine used");
  declareProperty("Mean_R",m_mean_R=0.12/CLHEP::meter,"Random distortion parameters: Mean of radius of curvature"); //Mean value from Survey
  declareProperty("RMS_R",m_RMS_R=0.08/CLHEP::meter,"Random distortion parameters: RMS of curvature radius"); //RMS value from Survey
  declareProperty("Mean_twist",m_mean_twist=-0.0005,"Random distortion parameters: Mean twist angle");//Mean value from Survey
  declareProperty("RMS_twist",m_RMS_twist=0.0008,"Random distortion parameters: RMS of twist angle");//RMS value from Survey
  declareProperty("OverrideVersion",m_overrideVersion=-1,"Override database object version number"); // No override if < 0

}


StatusCode
PixelDistortionsTool::initialize() 
{
    
  // Get the Pixel helper
  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
    if(msgLvl(MSG::FATAL))msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_overrideVersion >= 0) {
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Distortion data version number overriden to " << m_overrideVersion << endreq;
    m_version = m_overrideVersion; 
  }

  m_doCorrections = initializeDistortions();

  if (!m_doCorrections) {
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Distortion corrections are disabled" << endreq;
  }

  //if (m_log.level() <=  MSG::VERBOSE) {
  if(msgLvl(MSG::VERBOSE)){
    msg(MSG::VERBOSE) << "Dump of DetCondCFloat contents (if using DB, dump will appear during callback): " << endreq;
    dumpData();
  }

  if (m_write && m_distortions) writeData();

  return StatusCode::SUCCESS;
}


PixelDistortionsTool::~PixelDistortionsTool() 
{
    if (m_ownDistortions) {
      delete m_distortions;
    } 
}

bool 
PixelDistortionsTool::initializeDistortions() 
{
  m_ownDistortions = true;
  StatusCode sc;

  switch (m_inputSource) {
  case 0: // no bow correction
    m_distortions = 0;
    return false;
    break;
  case 1: // constant bow
    m_distortions = generateConstantBow();
    return true;
    break;
  case 2: // read from file
    m_distortions = readFromTextFile();
    return m_distortions; // converts to bool
    break;
  case 3: // random generation
    m_distortions = generateRandomBow();
    return m_distortions; // converts to bool
    break;
  case 4: // read from database here 
    m_ownDistortions = false;
    m_distortions  = 0;
    sc = registerCallBack();
    return !sc.isFailure();
    break;
  default: // any other case: do not apply bow correction
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Input source option " << m_inputSource 
	  << " is not implemented: no bow correction is applied" << endreq;
    return false;
    break;
  }
  // return a random value. The program never gets here anyway.
  // but it's needed to fix a compilation warning.
  return true; 
}



const DetCondCFloat *
PixelDistortionsTool::generateConstantBow()
{
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Using constant pixel distortions " << endreq;
  const int distosize=3; // dimension of distortion vector
  std::vector<float> data(distosize);
  DetCondCFloat *disto=0;
  PixelID::const_id_iterator begin=m_pixelid->wafer_begin();
  PixelID::const_id_iterator end=m_pixelid->wafer_end();
  data[0]=m_R1 * CLHEP::meter; // convert to 1/mm
  data[1]=m_R2 * CLHEP::meter; // convert to 1/mm
  data[2]=2.0*atan(m_twist) / CLHEP::degree; // convert to degree
  disto=new DetCondCFloat(distosize,getVersionName()); // create distortion container
  int index = disto->add(*begin,data.data());
  for ( PixelID::const_id_iterator id=(begin+1); id!=end; ++id) {
    disto->share(*id,index);
  }
  return disto;
}

const DetCondCFloat * 
PixelDistortionsTool::generateRandomBow()
{
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Using random pixel distortions" << endreq;
  StatusCode sc = service("AtRndmGenSvc", m_rndmSvc);
  if (!sc.isSuccess() || 0 == m_rndmSvc) {
    if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not find AtRndmGenSvc" << endreq;
    return 0;
  }
  // get the PixelDigitization random stream
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Getting random number engine : <" << m_rndmEngineName << ">" << endreq;
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  //msg(MSG::ERROR) << "STEP 2 " << endreq;
  if (m_rndmEngine==0) {
    if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not find RndmEngine : " << m_rndmEngineName << endreq;
    return 0;
  }

  const int distosize=3; // dimension of distortion vector
  std::vector<float> data(distosize);
  DetCondCFloat *disto=0;
  PixelID::const_id_iterator begin=m_pixelid->wafer_begin();
  PixelID::const_id_iterator end=m_pixelid->wafer_end();

  // get random number generator
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "RndmEngine" << m_rndmEngine << endreq;
  disto=new DetCondCFloat(distosize,getVersionName());
  for ( PixelID::const_id_iterator id=(begin+1); id!=end; id++) {
    data[0]=CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,m_mean_R,m_RMS_R);
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "DATA[0] " << data[0] << " " << m_mean_R << " " <<
      m_RMS_R <<endreq;
    data[1]=CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,data[0],m_RMS_R/10.);//to implement a correlation between distortions on 2 sides of the module
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "DATA[1] " << data[1] << " " << data[0]
	  <<  " " << m_RMS_R/10. <<endreq;
      data[2]=CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,m_mean_twist,m_RMS_twist);
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "DATA[2] " << data[2] << " " << m_mean_twist 
	  << " " << m_RMS_twist <<endreq;
      data[0] *= CLHEP::meter; // convert to 1/mm
      data[1] *= CLHEP::meter; // convert to 1/mm
      data[2] = 2.0*atan(data[2]) / CLHEP::degree; // convert to degree
      if(msgLvl(MSG::VERBOSE))msg(MSG::VERBOSE) << "DATA0 = " << data[0] << " DATA1 = " << data[1] << " DATA2 = " << data[2] << endreq;	
      disto->add(*id,data.data());
 }
  return disto;
}


const DetCondCFloat * 
PixelDistortionsTool::readFromTextFile() 
{
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Reading pixel distortions from file: " << m_textFileName << endreq;
  const int distosize=3; // dimension of distortion vector
  std::vector<float> data(distosize);
  DetCondCFloat *disto=0;
//  PixelID::const_id_iterator begin=m_pixelid->wafer_begin();
//  PixelID::const_id_iterator end=m_pixelid->wafer_end();
  std::string file_name = 
    PathResolver::find_file (m_textFileName, "DATAPATH");
  if (file_name.size()==0) {
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Distortion file " << m_textFileName   << " not found! No pixel distortion will be applied." << endreq;
    return 0;
  }
  std::ifstream input(file_name.c_str());
  if ( !input.good() ) {
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Cannot open " << file_name   << " not found! No pixel distortion will be applied." << endreq;
    return 0;
  }
  disto=new DetCondCFloat(distosize,getVersionName()); // create distortion container
  unsigned int idmod;
  while ( 1 ) {
    input >> std::hex >> idmod >> std::dec;
    for ( int i=0; i<distosize; i++) input >> data[i];
    //data[0]/=CLHEP::meter;                    // curvature is in m-1
    //data[1]/=CLHEP::meter;                    // curvature is in m-1
    //data[2]=tan(0.5*data[2]*CLHEP::degree);   // twist angle in degree
    if(msgLvl(MSG::VERBOSE))msg(MSG::VERBOSE) << "DATA0 = " << data[0] << " DATA1 = " << data[1] << " DATA2 = " << data[2] << endreq;	
    if ( input.bad() || input.eof() ) break;
    disto->add(Identifier(idmod),data.data());
  }
  input.close();

  return disto;

}


StatusCode PixelDistortionsTool::registerCallBack()
{
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Reading pixel distortions from database" << endreq;
  if (detStore()->contains<DetCondCFloat>(m_folderName)) {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Registering callback on DetCondCFloat with folder " << m_folderName << endreq;
    const DataHandle<DetCondCFloat> data;
    StatusCode sc = detStore()->regFcn(&IModuleDistortionsTool::fillDataCallBack, 
				       dynamic_cast<IModuleDistortionsTool *>(this), 
				       data, m_folderName);
    return sc;
  } else {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Unable to register callback on DetCondCFloat with folder " << m_folderName <<endreq;
    return StatusCode::FAILURE;
  }
}

StatusCode PixelDistortionsTool::fillDataCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{ 
  StatusCode sc = StatusCode::SUCCESS;
  (void) I; // avoid warning about unused parameter 
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Callback for PixelDistortions" << endreq;
  // Do not expect any other keys but check anyway.
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {    
    if (*itr == m_folderName) {
      sc = fillData();
    }
  }
  // reset cached items
  m_lastDisto = 0;
  m_lastID = Identifier(); // Invalid ID
  return sc;
}
  

StatusCode PixelDistortionsTool::fillData() {
  StatusCode sc = detStore()->retrieve(m_distortions, m_folderName);      
  if (sc.isFailure()) {
    m_distortions = 0;
    if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Unable to retrieve DetCondCFloat from " << m_folderName << endreq;
  } else {
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Retrieved DetCondCFloat from " << m_folderName << endreq;

    // Determine version number from tag
    if (m_distortions->tag() == "/Indet/PixelDist") {
      m_version = 0; // For reproducing bug in earlier versions for backward compatibility 
      if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Detected old version of pixel distortions data." << endreq;
    } else {
      bool gotVersion = false;
      // Get version number, expecting string to have the form /Indet/PixelDist_v#
      // If not recongnized will default to latest version.
      std::string baseStr = m_folderName+"_v";
      if (m_distortions->tag().compare(0, baseStr.size(), baseStr) == 0) {
	std::istringstream istr(m_distortions->tag().substr(baseStr.size()));
	int version_tmp = 0;
	istr >> version_tmp;
	if (istr.eof()) { // Should be have read whole stream if its a number
	  m_version = version_tmp;
	  gotVersion = true;
	}
      }
      if (!gotVersion) {
	if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to determine version number of pixel distortions data. Version string:  " 
						   << m_distortions->tag() << endreq;
      }
    }
  }
  
  if (m_overrideVersion >= 0) {
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Overriding version number. Version number from db was : " << m_version << endreq;
    m_version = m_overrideVersion; 
  }
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Version number of pixel distortions data: " << m_version << endreq;
  

  //if (m_log.level() <=  MSG::VERBOSE) {
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Dump of DetCondCFloat contents: " << endreq;
    dumpData();
  }
  return sc;
}

// Write distortions to storegate. Useful if you want to write constants to DB.
StatusCode PixelDistortionsTool::writeData() {
  StatusCode sc = StatusCode::SUCCESS;
  if (m_distortions) {
    sc = detStore()->record(m_distortions,m_folderName);
    if (sc.isFailure()) {
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Unable to record DetCondCFloat with name " << m_folderName << endreq;
    } else {
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Successfully recorded DetCondCFloat with name " << m_folderName << endreq;  
      m_ownDistortions = false;
    }
  }
  return sc;
}

// For Testing
void PixelDistortionsTool::dumpData() const {
  if (m_distortions) {
    m_distortions->print2();
  } else {
    if(msgLvl(MSG::INFO))msg(MSG::INFO) << "No Distortions " << endreq;
  }
}

const float * PixelDistortionsTool::getDistortionsArray(Identifier id) const
{
  if (m_distortions) {
    return m_distortions->find(id);
  } else {
    return 0;
  }
}

Amg::Vector2D
PixelDistortionsTool::correctReconstruction(Identifier id, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const 
{
  Amg::Vector2D newlocpos(locpos);
  if (m_doCorrections) {
    newlocpos += correction(id, locpos, direction);
  }
  return newlocpos;
}

Amg::Vector2D
PixelDistortionsTool::correctSimulation(Identifier id, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const 
{
  Amg::Vector2D newlocpos(locpos);
  if (m_doCorrections) {
    newlocpos -= correction(id, locpos, direction);
  }
  return newlocpos;
}

Amg::Vector2D
PixelDistortionsTool::correction(Identifier id, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
    
  double localphi = locpos[0];
  double localeta = locpos[1];
  static Amg::Vector2D nullCorrection(0., 0.);
  
  // This is needed to avoid rounding errors if the z component of the
  // direction vector is too small.
  if ( std::fabs(direction.z())<1.*CLHEP::micrometer) return nullCorrection; 

  // If a particle has a too shallow trajectory with respect to the 
  // module direction, it is likely to be a secondary particle and no 
  // shift should be applied.
  double invtanphi = direction.x()/direction.z(); 
  double invtaneta = direction.y()/direction.z();
  if ( sqrt(invtanphi*invtanphi+invtaneta*invtaneta) > 100. ) 
    return nullCorrection;

  const float *disto = 0;
  if (m_lastID.is_valid() && id == m_lastID) {
    // Used cached pointer.
    disto = m_lastDisto;
  } else {
    disto = getDistortionsArray(id);
    m_lastDisto = disto;
    m_lastID = id;
  } 

  const double xFE = 22.0*CLHEP::millimeter; //Distance between the 2 Front-End line, where bows have been measured
  const double yFE = 60.8*CLHEP::millimeter;  //Length of the active surface of each module
 
  

  if ( !disto ) {
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Distortions array empty for id = " 
	    << m_pixelid->show_to_string(id) << "(" << id.getString() << ")" << endreq;
    }
    return nullCorrection;
  }

  double data0 = disto[0] /CLHEP::meter;           // curvature is in m-1
  double data1 = disto[1] /CLHEP::meter;           // curvature is in m-1
  double data2 = tan(0.5*disto[2]*CLHEP::degree);   // twist angle in degree
 
  double twist1 = -data2;
  double twist2 =  data2;
  double b1 = sqrt((1.+twist1*twist1)*(1.+twist1*twist1)*(1.+twist1*twist1));
  double b2 = sqrt((1.+twist2*twist2)*(1.+twist2*twist2)*(1.+twist2*twist2));
  double z1 = localeta*twist1-0.5*b1*localeta*localeta*data1;
  double z2 = localeta*twist2-0.5*b2*localeta*localeta*data0;
  double zoff1 = (b1*yFE*yFE*data1)/24.;
  double zoff2 = (b2*yFE*yFE*data0)/24.;
  z1 = z1+zoff1;
  z2 = z2+zoff2;
  double zbow = z1+((z2-z1)/xFE)*(localphi+xFE/2.);
  
  double localphiCor = -zbow * invtanphi;
  double localetaCor = -zbow * invtaneta;

  // In earlies code version there was a bug in the sign of the correction. 
  // In MC this was not seen as reco just corrects back what was done in digitization. 
  // In order to maintain backward compatibilty in older MC we need to reproduce this wrong behaviour.
  if (m_version == 0) localphiCor = -localphiCor;
   
  if(msgLvl(MSG::VERBOSE)) {
    std::stringstream mstr; // So we can use operator<< of HepGeom::Vector3D<double>. 
    mstr << "Distortion correction for id = " 
	 << m_pixelid->show_to_string(id) << " (" << id.getString() << ") : " 
	 << "Local pos = " <<  "(" << locpos[0] << ", " <<  locpos[1] << ")"
	 << ", Direction = " << direction
	 << ", Correction = " << "(" << localphiCor << "," << localetaCor << ")";
      msg(MSG::VERBOSE) << mstr.str() << endreq;
  }
  return Amg::Vector2D(localphiCor, localetaCor); 

}
  
std::string PixelDistortionsTool::getVersionName() const 
{
  std::ostringstream ostr;
  ostr << m_folderName << "_v" << m_version;
  return ostr.str();
}


StatusCode
PixelDistortionsTool::finalize()
{
  return StatusCode::SUCCESS;
}
