/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ReadCalibDataCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

#include "GaudiKernel/EventIDRange.h"

// Include boost stuff
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"

// Include STL stuff
#include <limits>
#include <memory>

using namespace SCT_ConditionsData;

// Utility functions
namespace {
  template <typename C> 
  bool fillFromString(const std::string& source, C& userContainer) {
    typedef typename C::value_type V_t;
    V_t errVal{0};
    boost::char_separator<char> sep{" "};
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    Tokenizer tok{source, sep};
    bool noNan{true};
    for (Tokenizer::iterator i{tok.begin()}; i!=tok.end(); ++i) {
      try {
	userContainer.push_back(boost::lexical_cast<V_t>(*i));
      } catch (const boost::bad_lexical_cast&) {
	userContainer.push_back(errVal);
	noNan=false;
      }
    }
    return noNan;
  }
  float coerceToFloatRange(const double value) {
    const float maxfloat{std::numeric_limits<float>::max()};
    if (value>maxfloat) return maxfloat;
    else return static_cast<float>(value);
  } 
}

SCT_ReadCalibDataCondAlg::SCT_ReadCalibDataCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_defectMapIntToString{}
  , m_condSvc{"CondSvc", name}
  , m_id_sct{nullptr}
{
  m_ignoreDefects.value().push_back("NOISE_SLOPE");
  m_ignoreDefects.value().push_back("OFFSET_SLOPE");
  m_ignoreDefects.value().push_back("GAIN_SLOPE");
  m_ignoreDefects.value().push_back("BAD_OPE");
  m_ignoreDefectParameters.value().push_back(-1000.);
  m_ignoreDefectParameters.value().push_back(-1000.);
  m_ignoreDefectParameters.value().push_back(-1000.);
  m_ignoreDefectParameters.value().push_back(-1000.);
}

StatusCode SCT_ReadCalibDataCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  
  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "SCT_ID"));

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Read Cond Handle
  ATH_CHECK(m_readKeyGain.initialize());
  ATH_CHECK(m_readKeyNoise.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  // Write Cond Handles
  for (unsigned int i{GAIN}; i<NFEATURES; i++) {
    SG::WriteCondHandleKey<SCT_CalibDefectData>* writeKeyData{nullptr};
    if (     i==GAIN)  writeKeyData = &m_writeKeyGain;
    else if (i==NOISE) writeKeyData = &m_writeKeyNoise;
    if (writeKeyData==nullptr) continue;
    ATH_CHECK(writeKeyData->initialize());
    if (m_condSvc->regHandle(this, *writeKeyData).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << writeKeyData->fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }
  ATH_CHECK(m_writeKeyInfo.initialize());
  if (m_condSvc->regHandle(this, m_writeKeyInfo).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyInfo.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  // Fit Defects
  m_defectMapIntToString[0]  = "UNKNOWN";       //<! Defect type not in this map, add!  
  m_defectMapIntToString[1]  = "DEAD";          //<! Output always < 1%
  m_defectMapIntToString[2]  = "STUCKON";       //<! Output always > 98%
  m_defectMapIntToString[3]  = "UNDER";         //<! Occupancy never reaches max, always less than 95%
  m_defectMapIntToString[4]  = "OVER";          //<! Occcupancy greater than 100%
  m_defectMapIntToString[5]  = "BADFIT";        //<! The fit was not good for some reason - parameter is a chi2 cut
  // NPt Gain Defects
  m_defectMapIntToString[32] = "VLO_GAIN";      //<! Gain < 0.3 * chip average
  m_defectMapIntToString[9]  = "LO_GAIN";       //<! Gain < 0.75 * chip average
  m_defectMapIntToString[10] = "HI_GAIN";       //<! Gain > 1.25 * chip average
  m_defectMapIntToString[11] = "LO_OFFSET";     //<! Offset < -100
  m_defectMapIntToString[12] = "HI_OFFSET";     //<! Offset > 200
  m_defectMapIntToString[13] = "UNBONDED";      //<! Noise <= 750
  m_defectMapIntToString[14] = "PARTBONDED";    //<! Noise <= 1100
  m_defectMapIntToString[15] = "NOISY";         //<! Noise > 1.15* av chip noise
  m_defectMapIntToString[33] = "V_NOISY";       //<! Noise > 1.25* av chip noise
  m_defectMapIntToString[34] = "NOISE_SLOPE";   //<! Slope in noise across module, slope/chan > 1.
  m_defectMapIntToString[35] = "OFFSET_SLOPE";  //<! Slope in offset across module, slope/chan > 0.07
  m_defectMapIntToString[36] = "GAIN_SLOPE";    //<! Slope in gain across module, slope/chan > 0.04
  // Noise Occupancy Defects
  m_defectMapIntToString[19] = "NO_HI";         //<! High noise occupancy, 0.0005
  m_defectMapIntToString[37] = "BAD_OPE";       //<! Bad occupancy per event variance/binomial variance > 2.0)
  m_defectMapIntToString[38] = "DOUBTR_HI";     //<! High double trigger noise occupancy, > 5

  //Check ignoreDefects vectors are the same size
  if (m_ignoreDefects.value().size() != m_ignoreDefectParameters.value().size()) {
    ATH_MSG_FATAL("IgnoreDefect != IgnoreDefectsParameters, check job options!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_ReadCalibDataCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  bool validWriteCondHandle{true};
  // Do we have a valid Write Cond Handle for current time?
  SG::WriteCondHandle<SCT_CalibDefectData> writeHandleData[NFEATURES]{m_writeKeyGain, m_writeKeyNoise};
  for (unsigned int i{GAIN}; i<NFEATURES; i++) {
    if (writeHandleData[i].isValid()) {
      ATH_MSG_DEBUG("CondHandle " << writeHandleData[i].fullKey() << " is already valid."
                    << ". In theory this should not be called, but may happen"
                    << " if multiple concurrent events are being processed out of order.");
    } else {
      validWriteCondHandle = false;
    }
  }
  SG::WriteCondHandle<SCT_AllGoodStripInfo> writeHandleInfo{m_writeKeyInfo};
  if (writeHandleInfo.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleInfo.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
  } else {
    validWriteCondHandle = false;
  }
  if (validWriteCondHandle) return StatusCode::SUCCESS;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle[NFEATURES]{m_readKeyGain, m_readKeyNoise};
  const CondAttrListCollection* readCdo[NFEATURES]{*readHandle[GAIN], *readHandle[NOISE]};
  EventIDRange rangeR[NFEATURES];
  for (unsigned int i{GAIN}; i<NFEATURES; i++) {
    if (readCdo[i]==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object " << readHandle[i].key());
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    if (not readHandle[i].range(rangeR[i])) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle[i].key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle[i].fullKey() << " readCdo->size()= " << readCdo[i]->size());
    ATH_MSG_INFO("Range of input is " << rangeR[i]);
  }

  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }
  EventIDRange rangeDetEle;
  if (not sctDetEle.range(rangeDetEle)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << m_SCTDetEleCollKey.key());
    return StatusCode::FAILURE;
  }

  // Set range of output
  EventIDRange rangeW{EventIDRange::intersect(rangeR[GAIN], rangeR[NOISE], rangeDetEle)};
  if (rangeW.stop().isValid() and rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection range: " << rangeW);
    return StatusCode::FAILURE;
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<SCT_CalibDefectData> writeCdoData[NFEATURES]{nullptr, nullptr};
  for (unsigned int i{GAIN}; i<NFEATURES; i++) {
    writeCdoData[i] = std::make_unique<SCT_CalibDefectData>();
  }
  std::unique_ptr<SCT_AllGoodStripInfo> writeCdoInfo{std::make_unique<SCT_AllGoodStripInfo>()};
  // Initialize arrays and all strips to True
  for (int w{0}; w!=NUMBER_OF_WAFERS; ++w) {
    for (int s{0}; s!=STRIPS_PER_WAFER; ++s) {
      (*writeCdoInfo)[w][s]=true;
    }
  }

  
  // Create pointer to CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects theseDefects;
  
  // loop over collection
  for (unsigned int i{GAIN}; i<NFEATURES; i++) {
    CondAttrListCollection::const_iterator itLoop{readCdo[i]->begin()};
    CondAttrListCollection::const_iterator itLoop_end{readCdo[i]->end()};
    for (; itLoop!=itLoop_end; ++itLoop) {
      CondAttrListCollection::ChanNum chanNum{(*itLoop).first};
      const coral::AttributeList &anAttrList{(*itLoop).second};
  
      // Convert chanNum=offlineID into identifier
      Identifier moduleId{chanNum};
      IdentifierHash hashId0{m_id_sct->wafer_hash(moduleId)};
      IdentifierHash hashId1;
      m_id_sct->get_other_side(hashId0, hashId1);

      // Check for PhiSwap readout
      const InDetDD::SiDetectorElement* p_element{elements->getDetectorElement(hashId0)};
      bool phiSwap0Present{p_element->swapPhiReadoutDirection()};
      p_element = (elements->getDetectorElement(hashId1));
      bool phiSwap1Present{p_element->swapPhiReadoutDirection()};
       
      // Clear theseDefects
      theseDefects.begDefects.clear();
      theseDefects.endDefects.clear();
      theseDefects.typeOfDefect.clear();
      theseDefects.parValue.clear();
  
      // Get all defect parameters from COOL attrib list
      const std::string &gaindefectb{(anAttrList["defectBeginChannel"]).data<std::string>()};
      const std::string &gaindefecte{(anAttrList["defectEndChannel"]).data<std::string>()};
      const std::string &defectType{(anAttrList["defectType"]).data<std::string>()};
      const std::string &parValue{(anAttrList["defectParameter"]).data<std::string>()};

      // Convert the defect strings to vectors
      std::vector<unsigned int> gaindefectbvec;
      fillFromString(gaindefectb, gaindefectbvec);
      std::vector<unsigned int> gaindefectevec;
      fillFromString(gaindefecte, gaindefectevec);
      std::vector<unsigned int> defectTypevec;
      fillFromString(defectType, defectTypevec);
      std::vector<double> parValuevec;
      fillFromString(parValue, parValuevec);

      // Fill the Calib defect objects
      long unsigned int gainvec_size{gaindefectbvec.size()};
      for (long unsigned int i{0}; i<gainvec_size; ++i) {
        theseDefects.begDefects.push_back(gaindefectbvec[i]);
        theseDefects.endDefects.push_back(gaindefectevec[i]);
        theseDefects.typeOfDefect.push_back(m_defectMapIntToString[defectTypevec[i]]);
        theseDefects.parValue.push_back(coerceToFloatRange(parValuevec[i]));
      }
      // Fill the isGoodWaferArray
      if (not theseDefects.begDefects.empty()) {
        for (unsigned int i{0}; i<theseDefects.begDefects.size(); ++i) { // loop over all defects
          // Check for defects and their limits not to take into account in isGood
          bool ignoreDefect{false};
          unsigned int ig{0};
          while (ig<m_ignoreDefects.value().size()) { //loop until found defect or end of ignoredefects
            if (m_ignoreDefects.value()[ig] == theseDefects.typeOfDefect[i]) {
              if (                                                   m_ignoreDefectParameters.value()[ig]<-999.                   ) ignoreDefect = true; //no check on parameter value, defect ignored
              else if (theseDefects.typeOfDefect[i]=="NO_HI"     and m_ignoreDefectParameters.value()[ig]>theseDefects.parValue[i]) ignoreDefect = true; //noise below threshold, > 0.0005 (in DB, so default values printed here)
              else if (theseDefects.typeOfDefect[i]=="NOISY"     and m_ignoreDefectParameters.value()[ig]>theseDefects.parValue[i]) ignoreDefect = true; //noise below threshold, > 1.15* av chip average
              else if (theseDefects.typeOfDefect[i]=="V_NOISY"   and m_ignoreDefectParameters.value()[ig]>theseDefects.parValue[i]) ignoreDefect = true; //noise below threshold, > 1.25* av chip average
              else if (theseDefects.typeOfDefect[i]=="VLO_GAIN"  and m_ignoreDefectParameters.value()[ig]<theseDefects.parValue[i]) ignoreDefect = true; // gain to low, < 0.3 * chip average
              else if (theseDefects.typeOfDefect[i]=="LO_GAIN"   and m_ignoreDefectParameters.value()[ig]<theseDefects.parValue[i]) ignoreDefect = true; // gain to low < 0.75 * chip average
              else if (theseDefects.typeOfDefect[i]=="HI_GAIN"   and m_ignoreDefectParameters.value()[ig]>theseDefects.parValue[i]) ignoreDefect = true; // gain to high > 1.25 * chip average
              else if (theseDefects.typeOfDefect[i]=="LO_OFFSET" and m_ignoreDefectParameters.value()[ig]>theseDefects.parValue[i]) ignoreDefect = true; // offset to low < -100
              else if (theseDefects.typeOfDefect[i]=="HI_OFFSET" and m_ignoreDefectParameters.value()[ig]<theseDefects.parValue[i]) ignoreDefect = true; // offset to high > 200
            }
            ig++;
          }
          if (not ignoreDefect) {
            //set the isGoodBool value for all strips for this defect
            for (unsigned int strip{theseDefects.begDefects[i]}; strip <= theseDefects.endDefects[i]; ++strip) { 
              // Check for phiSwap and which wafer side before filling isGood vector
              if (strip < STRIPS_PER_WAFER) { //side 0 0->767
                const unsigned int waferId0{hashId0};
                SCT_WaferGoodStripInfo& thisWaferIsGoodData0{(*writeCdoInfo)[waferId0]};
                const unsigned int side0StripNumber{phiSwap0Present ? (  STRIPS_PER_WAFER-1-strip) : strip};
                thisWaferIsGoodData0[side0StripNumber] = false;
              } else { // side 1 768->1535 => 0->767
                const unsigned int waferId1{hashId1};
                SCT_WaferGoodStripInfo& thisWaferIsGoodData1{(*writeCdoInfo)[waferId1]};
                const unsigned int side1StripNumber{phiSwap1Present ? (2*STRIPS_PER_WAFER-1-strip) : (strip-STRIPS_PER_WAFER)};
                thisWaferIsGoodData1[side1StripNumber] = false;
              }
            }
          }
        }
      }
      
      // Fill the CalibDefectData maps with the Calib defect objects
      if (i==GAIN) {
        if (theseDefects.begDefects.empty()) {
          ATH_MSG_DEBUG("No NPtGain defects for module " << moduleId);
          continue;
        }
        if (not (writeCdoData[i]->addModule(moduleId, theseDefects))) {
          ATH_MSG_ERROR("Unable to add module " << moduleId << " to NPtGain defect map");
          return StatusCode::RECOVERABLE;
        } else {
          ATH_MSG_DEBUG("Defects for module " << moduleId << " added to NPG defect map");
        }
      } else if (i==NOISE) {
        if (theseDefects.begDefects.empty()) {
          ATH_MSG_DEBUG("No NoiseOccupancy defects for module " << moduleId);
          continue;
        }
        if (not (writeCdoData[i]->addModule(moduleId, theseDefects))) {
          ATH_MSG_ERROR("Unable to add module " << moduleId << " to NoiseOccupancy defect map");
          return StatusCode::RECOVERABLE;
        } else {
          ATH_MSG_DEBUG("Defects for module " << moduleId << " added to NoiseOccupancy defect map");
        }
      }
    }
  }

  //  Record the output cond objects
  ATH_MSG_DEBUG("There are " << writeCdoInfo->size() << " elements in " << writeHandleInfo.key());
  if (writeHandleInfo.record(rangeW, std::move(writeCdoInfo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_AllGoodStripInfo " << writeHandleInfo.key() 
                  << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandleInfo.key() << " with range " << rangeW << " into Conditions Store");
  for (unsigned int i{GAIN}; i<NFEATURES; i++) {
    ATH_MSG_DEBUG("There are " << writeCdoData[i]->size() << " elements in " << writeHandleData[i].key());
    if (writeHandleData[i].record(rangeW, std::move(writeCdoData[i])).isFailure()) {
      ATH_MSG_FATAL("Could not record SCT_CalibDefectData " << writeHandleData[i].key()
                    << " with EventRange " << rangeW << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << writeHandleData[i].key() << " with range " << rangeW << " into Conditions Store");
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_ReadCalibDataCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
