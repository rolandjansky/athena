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

#include "TMath.h"


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
     m_version(1), // db object version number
     m_useBB(false)
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
  declareProperty("UseBB",m_useBB=true,"Use BB"); // for v2 database use IDW or BB
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

  switch (m_inputSource) {
  case 0: // no bow correction
    m_distortions = 0;
    return false;
  case 1: // constant bow
    m_distortions = generateConstantBow();
    return true;
  case 2: // read from file
    m_distortions = readFromTextFile();
    return m_distortions; // converts to bool
  case 3: // random generation
    m_distortions = generateRandomBow();
    return m_distortions; // converts to bool
  case 4: // read from database here 
    m_ownDistortions = false;
    m_distortions  = 0;
    return !((StatusCode) registerCallBack().isFailure());
  default: // any other case: do not apply bow correction
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Input source option " << m_inputSource 
	  << " is not implemented: no bow correction is applied" << endreq;
    return false;
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
  if (msgLvl(MSG::INFO))
    msg(MSG::INFO) << "Reading pixel distortions from file: " << m_textFileName << endreq;

  int distosize; // dimension of distortion vector

  // set dimension of distortion vector
  if (m_version < 2) distosize = 3;
  else distosize = 441;

  int idmod;
  std::vector<float> data(distosize);
  DetCondCFloat *disto = 0;

  std::string file_name = PathResolver::find_file(m_textFileName, "DATAPATH");
  if (file_name.size() == 0) {
    if (msgLvl(MSG::WARNING))
      msg(MSG::WARNING) << "Distortion file " << m_textFileName << " not found! No pixel distortion will be applied." << endreq;

    return 0;
  }

  std::ifstream input(file_name.c_str());
  if (!input.good()) {
    if (msgLvl(MSG::WARNING))
      msg(MSG::WARNING) << "Cannot open " << file_name << " not found! No pixel distortion will be applied." << endreq;

    return 0;
  }

  disto = new DetCondCFloat(distosize, getVersionName()); // create distortion container
  while (input >> std::hex >> idmod >> std::dec) {
    for (int i = 0; i < distosize; ++i) input >> data[i];
    if (msgLvl(MSG::VERBOSE)) {
      for (int i = 0; i < distosize; ++i) msg(MSG::VERBOSE) << "DATA" << i << " = " << data[i];
      msg(MSG::VERBOSE) << endreq;
    }

    disto->add(Identifier(idmod), data.data());
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
PixelDistortionsTool::correction(Identifier id, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const
{
  double localphi = locpos[0];
  double localeta = locpos[1];
  static Amg::Vector2D nullCorrection(0., 0.);

  // This is needed to avoid rounding errors if the z component of the
  // direction vector is too small.
  if (std::fabs(direction.z()) < 1. * CLHEP::micrometer) return nullCorrection;

  // If a particle has a too shallow trajectory with respect to the
  // module direction, it is likely to be a secondary particle and no
  // shift should be applied.
  const double invtanphi = direction.x() / direction.z();
  const double invtaneta = direction.y() / direction.z();
  if (sqrt(invtanphi*invtanphi + invtaneta*invtaneta) > 100.)
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

  if (!disto) {
    if (msgLvl(MSG::VERBOSE))
      msg(MSG::VERBOSE) << "Distortions array empty for id = "
                        << m_pixelid->show_to_string(id) << "(" << id.getString() << ")" << endreq;

    return nullCorrection;
  }

  double localphiCor, localetaCor, localZ;
  const unsigned long long ull_id = id.get_compact();

  // If old database versions, the corrections are from the pixel survey, otherwise corrections are from
  // insitu shape measurements.
  if (m_version < 2) {
    localZ = getSurveyZ(localeta, localphi, disto);
  } else if (isOldParam(ull_id)) {
    localZ = getSurveyZ(localeta, localphi, disto);
  } else {
    float modEtaSize, modPhiSize;

    // set module sizes in millimetre
    if (ull_id >= 0x200000000000000 && ull_id <= 0x20d980000000000) {
      if (isIBL3D(ull_id)) {
        modEtaSize = 19.75 * CLHEP::millimeter;
        modPhiSize = 16.75 * CLHEP::millimeter;
      } else {
        modEtaSize = 40.4 * CLHEP::millimeter;
        modPhiSize = 16.8 * CLHEP::millimeter;
      }
    } else {
      modEtaSize = 60.2 * CLHEP::millimeter;
      modPhiSize = 16.44 * CLHEP::millimeter;
    }

    localZ = getInSituZ(localeta, modEtaSize, localphi, modPhiSize, disto);
  }

  localetaCor = -localZ * invtaneta;
  localphiCor = -localZ * invtanphi;

  // In earlies code version there was a bug in the sign of the correction.
  // In MC this was not seen as reco just corrects back what was done in digitization.
  // In order to maintain backward compatibilty in older MC we need to reproduce this wrong behaviour.
  if (m_version == 0) localphiCor = -localphiCor;

  if (msgLvl(MSG::VERBOSE)) {
    std::stringstream mstr; // So we can use operator<< of HepGeom::Vector3D<double>.
    mstr << "Distortion correction for id = "
         << m_pixelid->show_to_string(id) << " (" << id.getString() << ") : "
         << "Local pos = " <<  "(" << locpos[0] << ", " <<  locpos[1] << ")"
         << ", Direction = " << direction << ", localZ = " << localZ
         << ", Correction = " << "(" << localphiCor << "," << localetaCor << ")";
    msg(MSG::VERBOSE) << mstr.str() << endreq;
  }

  return Amg::Vector2D(localphiCor, localetaCor);
}

double PixelDistortionsTool::getInSituZ(const double localeta, const double eta_size, const double localphi, const double phi_size, const float *disto) const
{
  if (m_useBB) {
    double etaHalfRangeBB = eta_size * 10. / 21.;
    double phiHalfRangeBB = phi_size * 10. / 21.;
    double etaRangeBB = eta_size * 20. / 21.;
    double phiRangeBB = phi_size * 20. / 21.;
    double eta, phi;

    // map positions on interval [- edge, 1 + edge]
    // edge is the part outside of the Bezier-Bernstein range
    if (TMath::Abs(localeta) - etaHalfRangeBB > 0) {
      if (localeta < 0)
        eta = (localeta + etaHalfRangeBB) / etaRangeBB;
      else
        eta = 1. + (localeta - etaHalfRangeBB) / etaRangeBB;
    } else {
      eta = localeta / etaRangeBB + 0.5;
    }
    if (TMath::Abs(localphi) - phiHalfRangeBB > 0) {
      if (localphi < 0)
        phi = (localphi + phiHalfRangeBB) / phiRangeBB;
      else
        phi = 1. + (localphi - phiHalfRangeBB) / phiRangeBB;
    } else {
      phi = localphi / phiRangeBB + 0.5;
    }
    return bernstein_bezier(&eta, &phi, disto);
  } else {
  return IDW(&localeta, &eta_size, &localphi, &phi_size, disto);
}
}

double PixelDistortionsTool::getSurveyZ(const double localeta, const double localphi, const float *disto) const
{
  const double xFE = 22.0 * CLHEP::millimeter; // Distance between the 2 Front-End line, where bows have been measured
  const double yFE = 60.8 * CLHEP::millimeter; // Length of the active surface of each module

  double data0 = disto[0] / CLHEP::meter;           // curvature is in m-1
  double data1 = disto[1] / CLHEP::meter;           // curvature is in m-1
  double data2 = tan(0.5 * disto[2] * CLHEP::degree);   // twist angle in degree

  double twist1 = -data2;
  double twist2 = data2;
  double b1 = sqrt((1. + twist1*twist1) * (1. + twist1*twist1) * (1. + twist1*twist1));
  double b2 = sqrt((1. + twist2*twist2) * (1. + twist2*twist2) * (1. + twist2*twist2));
  double z1 = localeta * twist1 - 0.5 * b1 * localeta*localeta * data1;
  double z2 = localeta * twist2 - 0.5 * b2 * localeta*localeta * data0;
  double zoff1 = (b1 * yFE*yFE * data1) / 24.;
  double zoff2 = (b2 * yFE*yFE * data0) / 24.;
  z1 = z1 + zoff1;
  z2 = z2 + zoff2;

  return z1 + ((z2 - z1) / xFE) * (localphi + xFE / 2.);
}

double PixelDistortionsTool::bernstein_grundpolynom(const double *t, const int n, const int i) const
{
  return TMath::Binomial(n, i) * TMath::Power(*t, i) * TMath::Power(1. - *t, n - i);
}

double PixelDistortionsTool::bernstein_bezier(const double *u, const double *v, const float *P) const
{
  int n = 20;
  int m = 20;
  double r = 0.;

  for (int i = 0; i <= n; ++i) {
    for (int j = 0; j <= m; ++j) {
      int k = (i * (m + 1)) + j;
      if (P[k] < -10.) continue;
      r += bernstein_grundpolynom(u, n, i) * bernstein_grundpolynom(v, m, j) * P[k];
    }
  }

  return r;
}

double PixelDistortionsTool::IDW(const double *x, const double *u_length, const double *y, const double *v_length, const float *P) const
{
  double nom = 0.;
  double denom = 0.;
  // shift range from [-module_length / 2, module_length / 2] to [0, module length]
  double u = *x + *u_length / 2.;
  double v = *y + *v_length / 2.;
  short n = 21;
  short m = 21;

  for (short i = 0; i < n; ++i) {
    for (short j = 0; j < m; ++j) {
      short k = i * m + j;

      if (P[k] < -10.) continue;

      // d(r, r_i) = 0
      double u_i = *u_length * ((double)i) / ((double)n) + (*u_length / ((double)n * 2.));
      double v_j = *v_length * ((double)j) / ((double)m) + (*v_length / ((double)m * 2.));
      if ((TMath::Abs(u - u_i) < 1e-6) && (TMath::Abs(v - v_j) < 1e-6))
        return P[k];

      double d = TMath::Sqrt(TMath::Power(u - u_i, 2) + TMath::Power(v - v_j, 2));
      double w = 1. / TMath::Power(d, 4);
      nom += w * P[k];
      denom += w;
    }
  }
  return nom / denom;
}

bool PixelDistortionsTool::isOldParam(const unsigned long long ull_id) const
{
  // Only pixel modules can have the old parametrisation
  if (ull_id < 0x240000000000000) return false;
  // For now: no new parametrisation for Pixel
  else return true;

  // No new parametrisation for non-barrel
  if (!(ull_id < 0x300000000000000)) return true;

  // Check for pixel modules with old parametrisation
  if (ull_id == 0x240200000000000) return true;
  if (ull_id == 0x240500000000000) return true;
  if (ull_id == 0x240800000000000) return true;
  if (ull_id == 0x241200000000000) return true;
  if (ull_id == 0x241680000000000) return true;
  if (ull_id == 0x241700000000000) return true;
  if (ull_id == 0x241800000000000) return true;
  if (ull_id == 0x242200000000000) return true;
  if (ull_id == 0x242780000000000) return true;
  if (ull_id == 0x242800000000000) return true;
  if (ull_id == 0x243200000000000) return true;
  if (ull_id == 0x243800000000000) return true;
  if (ull_id == 0x244200000000000) return true;
  if (ull_id == 0x244800000000000) return true;
  if (ull_id == 0x245200000000000) return true;
  if (ull_id == 0x245300000000000) return true;
  if (ull_id == 0x245800000000000) return true;
  if (ull_id == 0x246200000000000) return true;
  if (ull_id == 0x246480000000000) return true;
  if (ull_id == 0x246800000000000) return true;
  if (ull_id == 0x247200000000000) return true;
  if (ull_id == 0x247780000000000) return true;
  if (ull_id == 0x247800000000000) return true;
  if (ull_id == 0x248200000000000) return true;
  if (ull_id == 0x248500000000000) return true;
  if (ull_id == 0x248800000000000) return true;
  if (ull_id == 0x249200000000000) return true;
  if (ull_id == 0x249800000000000) return true;
  if (ull_id == 0x24a200000000000) return true;
  if (ull_id == 0x24a480000000000) return true;
  if (ull_id == 0x24a800000000000) return true;
  if (ull_id == 0x24b200000000000) return true;
  if (ull_id == 0x24b800000000000) return true;
  if (ull_id == 0x24c200000000000) return true;
  if (ull_id == 0x24c800000000000) return true;
  if (ull_id == 0x24d200000000000) return true;
  if (ull_id == 0x24d800000000000) return true;
  if (ull_id == 0x24e200000000000) return true;
  if (ull_id == 0x24e480000000000) return true;
  if (ull_id == 0x24e800000000000) return true;
  if (ull_id == 0x24f200000000000) return true;
  if (ull_id == 0x24f480000000000) return true;
  if (ull_id == 0x24f800000000000) return true;
  if (ull_id == 0x250200000000000) return true;
  if (ull_id == 0x250380000000000) return true;
  if (ull_id == 0x250800000000000) return true;
  if (ull_id == 0x251200000000000) return true;
  if (ull_id == 0x251500000000000) return true;
  if (ull_id == 0x251800000000000) return true;
  if (ull_id == 0x252200000000000) return true;
  if (ull_id == 0x252800000000000) return true;
  if (ull_id == 0x253200000000000) return true;
  if (ull_id == 0x253800000000000) return true;
  if (ull_id == 0x254200000000000) return true;
  if (ull_id == 0x254800000000000) return true;
  if (ull_id == 0x255200000000000) return true;
  if (ull_id == 0x255800000000000) return true;
  if (ull_id == 0x280200000000000) return true;
  if (ull_id == 0x280500000000000) return true;
  if (ull_id == 0x284200000000000) return true;
  if (ull_id == 0x284680000000000) return true;
  if (ull_id == 0x284800000000000) return true;
  if (ull_id == 0x285400000000000) return true;
  if (ull_id == 0x285480000000000) return true;
  if (ull_id == 0x286500000000000) return true;
  if (ull_id == 0x287500000000000) return true;
  if (ull_id == 0x287800000000000) return true;
  if (ull_id == 0x288200000000000) return true;
  if (ull_id == 0x288400000000000) return true;
  if (ull_id == 0x28c200000000000) return true;
  if (ull_id == 0x290680000000000) return true;
  if (ull_id == 0x290700000000000) return true;
  if (ull_id == 0x293480000000000) return true;
  if (ull_id == 0x293800000000000) return true;
  if (ull_id == 0x294300000000000) return true;
  if (ull_id == 0x294680000000000) return true;
  if (ull_id == 0x295580000000000) return true;
  if (ull_id == 0x295680000000000) return true;
  if (ull_id == 0x297200000000000) return true;
  if (ull_id == 0x298700000000000) return true;
  if (ull_id == 0x298800000000000) return true;
  if (ull_id == 0x299300000000000) return true;
  if (ull_id == 0x299380000000000) return true;
  if (ull_id == 0x299480000000000) return true;
  if (ull_id == 0x299500000000000) return true;
  if (ull_id == 0x299800000000000) return true;
  if (ull_id == 0x29a780000000000) return true;
  if (ull_id == 0x29c200000000000) return true;
  if (ull_id == 0x29c600000000000) return true;
  if (ull_id == 0x29e600000000000) return true;
  if (ull_id == 0x29e800000000000) return true;
  if (ull_id == 0x29f500000000000) return true;
  if (ull_id == 0x2a0800000000000) return true;
  if (ull_id == 0x2a2200000000000) return true;
  if (ull_id == 0x2a2280000000000) return true;
  if (ull_id == 0x2a2380000000000) return true;
  if (ull_id == 0x2a2580000000000) return true;
  if (ull_id == 0x2a3580000000000) return true;
  if (ull_id == 0x2a3800000000000) return true;
  if (ull_id == 0x2a4800000000000) return true;
  if (ull_id == 0x2c0680000000000) return true;
  if (ull_id == 0x2c9380000000000) return true;
  if (ull_id == 0x2ca480000000000) return true;
  if (ull_id == 0x2ca700000000000) return true;
  if (ull_id == 0x2cb480000000000) return true;
  if (ull_id == 0x2ce580000000000) return true;
  if (ull_id == 0x2ce680000000000) return true;
  if (ull_id == 0x2cf300000000000) return true;
  if (ull_id == 0x2cf380000000000) return true;
  if (ull_id == 0x2cf500000000000) return true;
  if (ull_id == 0x2d0300000000000) return true;
  if (ull_id == 0x2d2280000000000) return true;
  if (ull_id == 0x2d2500000000000) return true;
  if (ull_id == 0x2d3800000000000) return true;
  if (ull_id == 0x2d4380000000000) return true;
  if (ull_id == 0x2d7800000000000) return true;
  if (ull_id == 0x2d8200000000000) return true;
  if (ull_id == 0x2da500000000000) return true;
  if (ull_id == 0x2da600000000000) return true;
  if (ull_id == 0x2da800000000000) return true;
  if (ull_id == 0x2db800000000000) return true;
  if (ull_id == 0x2dc680000000000) return true;
  if (ull_id == 0x2dd600000000000) return true;
  if (ull_id == 0x2dd680000000000) return true;
  if (ull_id == 0x2de300000000000) return true;
  if (ull_id == 0x2de380000000000) return true;
  if (ull_id == 0x2de600000000000) return true;
  if (ull_id == 0x2de800000000000) return true;
  if (ull_id == 0x2e0800000000000) return true;
  if (ull_id == 0x2e3580000000000) return true;
  if (ull_id == 0x2e3600000000000) return true;
  if (ull_id == 0x2e3780000000000) return true;
  if (ull_id == 0x2e4500000000000) return true;
  if (ull_id == 0x2e5380000000000) return true;
  if (ull_id == 0x2e6400000000000) return true;
  if (ull_id == 0x2e6680000000000) return true;
  if (ull_id == 0x2e7500000000000) return true;
  if (ull_id == 0x2e7680000000000) return true;
  if (ull_id == 0x2e7800000000000) return true;
  if (ull_id == 0x2e8500000000000) return true;
  if (ull_id == 0x2e8700000000000) return true;
  if (ull_id == 0x2e8780000000000) return true;
  if (ull_id == 0x2e9700000000000) return true;
  if (ull_id == 0x2ea600000000000) return true;
  if (ull_id == 0x2eb300000000000) return true;
  if (ull_id == 0x2ec500000000000) return true;
  if (ull_id == 0x2ec700000000000) return true;
  if (ull_id == 0x2ed300000000000) return true;
  if (ull_id == 0x2ee600000000000) return true;
  if (ull_id == 0x2f0600000000000) return true;
  if (ull_id == 0x2f2500000000000) return true;
  if (ull_id == 0x2f2800000000000) return true;
  if (ull_id == 0x2f3700000000000) return true;

  return false;
}

bool PixelDistortionsTool::isIBL3D(const unsigned long long ull_id) const
{
  // Stave 1
  if (ull_id >= 0x200000000000000 && ull_id <= 0x200180000000000) return true;
  if (ull_id >= 0x200800000000000 && ull_id <= 0x200980000000000) return true;

  // Stave 2
  if (ull_id >= 0x201000000000000 && ull_id <= 0x201180000000000) return true;
  if (ull_id >= 0x201800000000000 && ull_id <= 0x201980000000000) return true;

  // Stave 3
  if (ull_id >= 0x202000000000000 && ull_id <= 0x202180000000000) return true;
  if (ull_id >= 0x202800000000000 && ull_id <= 0x202980000000000) return true;

  // Stave 4
  if (ull_id >= 0x203000000000000 && ull_id <= 0x203180000000000) return true;
  if (ull_id >= 0x203800000000000 && ull_id <= 0x203980000000000) return true;

  // Stave 5
  if (ull_id >= 0x204000000000000 && ull_id <= 0x204180000000000) return true;
  if (ull_id >= 0x204800000000000 && ull_id <= 0x204980000000000) return true;

  // Stave 6
  if (ull_id >= 0x205000000000000 && ull_id <= 0x205180000000000) return true;
  if (ull_id >= 0x205800000000000 && ull_id <= 0x205980000000000) return true;

  // Stave 7
  if (ull_id >= 0x206000000000000 && ull_id <= 0x206180000000000) return true;
  if (ull_id >= 0x206800000000000 && ull_id <= 0x206980000000000) return true;

  // Stave 8
  if (ull_id >= 0x207000000000000 && ull_id <= 0x207180000000000) return true;
  if (ull_id >= 0x207800000000000 && ull_id <= 0x207980000000000) return true;

  // Stave 9
  if (ull_id >= 0x208000000000000 && ull_id <= 0x208180000000000) return true;
  if (ull_id >= 0x208800000000000 && ull_id <= 0x208980000000000) return true;

  // Stave 10
  if (ull_id >= 0x209000000000000 && ull_id <= 0x209180000000000) return true;
  if (ull_id >= 0x209800000000000 && ull_id <= 0x209980000000000) return true;

  // Stave 11
  if (ull_id >= 0x20a000000000000 && ull_id <= 0x20a180000000000) return true;
  if (ull_id >= 0x20a800000000000 && ull_id <= 0x20a980000000000) return true;

  // Stave 12
  if (ull_id >= 0x20b000000000000 && ull_id <= 0x20b180000000000) return true;
  if (ull_id >= 0x20b800000000000 && ull_id <= 0x20b980000000000) return true;

  // Stave 13
  if (ull_id >= 0x20c000000000000 && ull_id <= 0x20c180000000000) return true;
  if (ull_id >= 0x20c800000000000 && ull_id <= 0x20c980000000000) return true;

  // Stave 14
  if (ull_id >= 0x20d000000000000 && ull_id <= 0x20d180000000000) return true;
  if (ull_id >= 0x20d800000000000 && ull_id <= 0x20d980000000000) return true;

  return false;
}

std::string PixelDistortionsTool::getVersionName() const
{
  std::ostringstream ostr;
  ostr << m_folderName << "_v" << m_version;
  return ostr.str();
}

StatusCode PixelDistortionsTool::finalize()
{
  return StatusCode::SUCCESS;
}
