/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDistortionAlg.h"
#include "GaudiKernel/EventIDRange.h"

// Random Number Generation
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "PathResolver/PathResolver.h"

#include <map>
#include <stdint.h>
#include <string>
#include <istream>

PixelDistortionAlg::PixelDistortionAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_pixelID(nullptr) { }

StatusCode PixelDistortionAlg::initialize() {
  ATH_MSG_DEBUG("PixelDistortionAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_rndmSvc.retrieve());

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDistortionAlg::execute() {
  ATH_MSG_DEBUG("PixelDistortionAlg::execute()");

  SG::WriteCondHandle<PixelDistortionData> writeHandle(m_writeKey);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  SG::ReadCondHandle<DetCondCFloat> readHandle(m_readKey);
  const DetCondCFloat* readCdo = *readHandle; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range
  ATH_MSG_DEBUG("Size of DetCondCFloat " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelDistortionData> writeCdo(std::make_unique<PixelDistortionData>());

  constexpr int nmodule_max = 2048;
  std::unordered_map<uint32_t,std::vector<float>> distortionMap;
  if (m_inputSource==0) { // no bow correction
    ATH_MSG_DEBUG("No bow correction");
    writeCdo -> setVersion(m_version);
    for (int i=0; i<nmodule_max; i++) {
      distortionMap[i].push_back(0.0);
      distortionMap[i].push_back(0.0);
      distortionMap[i].push_back(0.0);
    }
  }
  else if (m_inputSource==1) { // constant bow
    ATH_MSG_DEBUG("Using constant pixel distortions ");
    writeCdo -> setVersion(m_version);
    for (int i=0; i<nmodule_max; i++) {
      distortionMap[i].push_back(m_R1*CLHEP::meter); // convert to 1/mm
      distortionMap[i].push_back(m_R2*CLHEP::meter); // convert to 1/mm
      distortionMap[i].push_back(2.0*atan(m_twist)/CLHEP::degree); // convert to degree
    }
  }
  else if (m_inputSource==2) { // read from file
    ATH_MSG_DEBUG("Reading pixel distortions from file: " << m_textFileName);
    writeCdo -> setVersion(m_version);

    std::string file_name = PathResolver::find_file(m_textFileName, "DATAPATH");
    if (file_name.size()==0) {
      ATH_MSG_ERROR("Distortion file " << m_textFileName << " not found! No pixel distortion will be applied.");
      return StatusCode::FAILURE;
    }
    std::ifstream input(file_name.c_str());
    if (!input.good()) {
      ATH_MSG_ERROR("Cannot open " << file_name   << " not found! No pixel distortion will be applied.");
      return StatusCode::FAILURE;
    }
    while (!input.eof()) {
      unsigned int idmod;
      float r1,r2,twist;
      unsigned int hashID = 0;
      if (m_version==0) { 
        input >> std::hex >> idmod >> std::dec >> r1 >> r2 >> twist;
        hashID = m_pixelID->wafer_hash((Identifier)idmod); 
      }
      else if (m_version>0) { 
        input >> idmod >> r1 >> r2 >> twist;
        hashID = idmod;
      }
      ATH_MSG_VERBOSE("Identifier = " << idmod << " HashID = " << hashID << " R1 = " << r1 << " R2 = " << r2 << " Twist = " << twist);
      distortionMap[hashID].push_back(r1); // convert to 1/mm
      distortionMap[hashID].push_back(r2); // convert to 1/mm
      distortionMap[hashID].push_back(twist); // convert to degree
    }
    input.close();
  }
  else if (m_inputSource==3) { // random generation
    ATH_MSG_DEBUG("Using random pixel distortions");
    writeCdo -> setVersion(m_version);

    ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
    rngWrapper->setSeed(name(),Gaudi::Hive::currentContext());
    CLHEP::HepRandomEngine *rndmEngine = *rngWrapper;

    for (int i=0; i<nmodule_max; i++) {
      float r1    = CLHEP::RandGaussZiggurat::shoot(rndmEngine,m_mean_R,m_RMS_R);
      float r2    = CLHEP::RandGaussZiggurat::shoot(rndmEngine,r1,m_RMS_R/10.);//to implement a correlation between distortions on 2 sides of the module
      float twist = CLHEP::RandGaussZiggurat::shoot(rndmEngine,m_mean_twist,m_RMS_twist);
      distortionMap[i].push_back(r1*CLHEP::meter); // convert to 1/mm
      distortionMap[i].push_back(r2*CLHEP::meter); // convert to 1/mm
      distortionMap[i].push_back(2.0*atan(twist)/CLHEP::degree); // convert to degree
    }
  }
  else if (m_inputSource==4) { // read from database here 
    int version = 0;
    if (readCdo->tag()=="/Indet/PixelDist") {
      version = 0; // For reproducing bug in earlier versions for backward compatibility 
      ATH_MSG_INFO("Detected old version of pixel distortions data.");
    } 
    else {
      bool gotVersion = false;
      // Get version number, expecting string to have the form /Indet/PixelDist_v#
      // If not recongnized will default to latest version.
      std::string baseStr = "/Indet/PixelDist_v";
      if (readCdo->tag().compare(0,baseStr.size(),baseStr)==0) {
        std::istringstream istr(readCdo->tag().substr(baseStr.size()));
        int version_tmp = 0;
        istr >> version_tmp;
        if (istr.eof()) { // Should be have read whole stream if its a number
          version = version_tmp;
          gotVersion = true;
        }
      }
      if (!gotVersion) {
        ATH_MSG_WARNING("Unable to determine version number of pixel distortions data. Version string:  " << readCdo->tag());
      }
    }
    writeCdo -> setVersion(version);

    for (int i=0; i<nmodule_max; i++) {
      if (readCdo->find(m_pixelID->wafer_id(IdentifierHash(i)))) {
        const float *disto = readCdo->find(m_pixelID->wafer_id(IdentifierHash(i)));
        distortionMap[i].push_back(disto[0]);
        distortionMap[i].push_back(disto[1]);
        distortionMap[i].push_back(disto[2]);
      }
    }
  }
  writeCdo -> setDistortionMap(distortionMap);

  if (m_write) {
    std::ofstream* outfile = new std::ofstream("output_distortion.txt"); 
    for (int i=0; i<nmodule_max; i++) {
      if (distortionMap[i].size()) {
        if (m_version==0) {
          *outfile << m_pixelID->wafer_id(IdentifierHash(i)) << " " << distortionMap[i].at(0) << " " << distortionMap[i].at(1) << " " << distortionMap[i].at(2) << std::endl;
        }
        else if (m_version>0) {
          *outfile << i << " " << distortionMap[i].at(0) << " " << distortionMap[i].at(1) << " " << distortionMap[i].at(2) << std::endl;
        }
      }
    }
    outfile->close(); 
    delete outfile; 
  }

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDRange rangeW{start, stop};

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDistortionData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

