//////////////////////////////////////////////////////////////////////
//  L1DynamicPedestalProviderTxt.cxx (c) Veit Scjarf
///////////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1DynamicPedestalProviderTxt.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "TrigBunchCrossingTool/BunchCrossing.h"

#include "PathResolver/PathResolver.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <algorithm>
#include <cmath>
#include <fstream>

using CxxUtils::make_unique;

namespace {
using bcid_t = Trig::IBunchCrossingTool::bcid_type;
static const bcid_t MAX_BCID = 3564;
}

namespace LVL1 
{

//================ Constructor ================================================
L1DynamicPedestalProviderTxt::L1DynamicPedestalProviderTxt(const std::string& t,
                                                           const std::string& n,
                                                           const IInterface*  p)
  : AthAlgTool(t, n, p)
  , m_bunchCrossingTool("Trig::MCBunchCrossingTool/BunchCrossingTool")
{
  declareInterface<IL1DynamicPedestalProvider>(this);

  m_emParameterizations[0].assign(s_nElements, nullptr);
  m_emParameterizations[1].assign(s_nElements, nullptr);
  m_hadParameterizations[0].assign(s_nElements, nullptr);
  m_hadParameterizations[1].assign(s_nElements, nullptr);

  declareProperty("BunchCrossingTool", m_bunchCrossingTool);

  // Input files containing the parameters for the electromagnetic and hadronic
  // layer, respectively.
  declareProperty("InputFileEM_ShortGap", m_inputFileEMShort);
  declareProperty("InputFileHAD_ShortGap", m_inputFileHADShort);
  declareProperty("InputFileEM_LongGap", m_inputFileEMLong);
  declareProperty("InputFileHAD_LongGap", m_inputFileHADLong);
}

//================ Destructor =================================================
L1DynamicPedestalProviderTxt::~L1DynamicPedestalProviderTxt()
{
  // free all genuinely allocated parameterizations
  for(auto p : m_allocatedParameterizations) {
    delete p;
  }
}

//================ Initialisation =============================================
StatusCode L1DynamicPedestalProviderTxt::initialize()
{
  CHECK( m_bunchCrossingTool.retrieve() );

  // parse parameterization for the electromagnetic layer
  std::string fileNameEMShort = PathResolver::find_file(m_inputFileEMShort, "DATAPATH");
  if(fileNameEMShort.empty()) {
    ATH_MSG_FATAL("Could not resolve input file: " << m_inputFileEMShort);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("::initialize: resolved input file: " << fileNameEMShort);

  try {
    parseInputFile(fileNameEMShort, m_emParameterizations[0]);
  } catch (const ParseException& e) {
    ATH_MSG_FATAL("Could not parse input file: " << fileNameEMShort << "; error: " << e.what());
    return StatusCode::FAILURE;
  }

  std::string fileNameEMLong = PathResolver::find_file(m_inputFileEMLong, "DATAPATH");
  if(fileNameEMLong.empty()) {
    ATH_MSG_FATAL("Could not resolve input file: " << m_inputFileEMLong);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("::initialize: resolved input file: " << fileNameEMLong);

  try {
    parseInputFile(fileNameEMLong, m_emParameterizations[1]);
  } catch (const ParseException& e) {
    ATH_MSG_FATAL("Could not parse input file: " << fileNameEMLong << "; error: " << e.what());
    return StatusCode::FAILURE;
  }

  // parse parameterization for the hadronic layer
  std::string fileNameHADShort = PathResolver::find_file(m_inputFileHADShort, "DATAPATH");
  if(fileNameHADShort.empty()) {
    ATH_MSG_FATAL("Could not resolve input file: " << m_inputFileHADShort);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("::initialize: resolved input file: " << fileNameHADShort);

  try {
    parseInputFile(fileNameHADShort, m_hadParameterizations[0]);
  } catch (const ParseException& e) {
    ATH_MSG_FATAL("Could not parse input file: " << fileNameHADShort << "; error: " << e.what());
    return StatusCode::FAILURE;
  }

  std::string fileNameHADLong = PathResolver::find_file(m_inputFileHADLong, "DATAPATH");
  if(fileNameHADLong.empty()) {
    ATH_MSG_FATAL("Could not resolve input file: " << m_inputFileHADLong);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("::initialize: resolved input file: " << fileNameHADLong);

  try {
    parseInputFile(fileNameHADLong, m_hadParameterizations[1]);
  } catch (const ParseException& e) {
    ATH_MSG_FATAL("Could not parse input file: " << fileNameHADLong << "; error: " << e.what());
    return StatusCode::FAILURE;
  }

  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  CHECK(incSvc.retrieve());
  incSvc->addListener(this, "BunchConfig");

  return StatusCode::SUCCESS;
}

void L1DynamicPedestalProviderTxt::handle(const Incident& inc)
{
  if(inc.type() != "BunchConfig") return;

  parseBeamIntensityPattern();
}

namespace {

// Display results of the parsing for debugging purposes
template<typename Log, typename Tool, typename ResultVector>
void printPatternParsingInfo(Log& log, const Tool& tool, const ResultVector& result) {
  for(bcid_t bcid = 0; bcid < MAX_BCID; bcid += 20) {
    // print 20 items at once

    log << MSG::VERBOSE << "Filled      ";
    for(bcid_t j = bcid; j != std::min(MAX_BCID, bcid+20); ++j) log << std::setw(3) << tool->isFilled(j) << " ";
    log << endreq;

    log << MSG::VERBOSE << "Distance    ";
    for(bcid_t j = bcid; j != std::min(MAX_BCID, bcid+20); ++j) log << std::setw(3) << result[j].second << " ";
    log << endreq;

    log << MSG::VERBOSE << "LongGap?    ";
    for(bcid_t j = bcid; j != std::min(MAX_BCID, bcid+20); ++j) log <<  std::setw(3) << result[j].first << " ";
    log << endreq;
  }
}

} // namespace [anonymous]

// "Parse" the beam intensity pattern to get the bunch train structure.
void L1DynamicPedestalProviderTxt::parseBeamIntensityPattern()
{
  //  using bcid_t = Trig::IBunchCrossingTool::bcid_type;
  auto BC = Trig::IBunchCrossingTool::BunchCrossings;

  m_distanceFromHeadOfTrain.assign(MAX_BCID, std::make_pair(false, -10));

  for(bcid_t bcid = 0; bcid != MAX_BCID; ++bcid) {
    if(m_bunchCrossingTool->isFilled(bcid)) {
      m_distanceFromHeadOfTrain[bcid] = std::make_pair(m_bunchCrossingTool->gapBeforeTrain(bcid) > 250,
                                                       m_bunchCrossingTool->distanceFromFront(bcid, BC));
    } else {
      if(m_bunchCrossingTool->gapAfterBunch(bcid, BC) == 1) {
        bcid_t head = ((bcid + 1) == MAX_BCID ? 0 : bcid + 1); // wrap around
        m_distanceFromHeadOfTrain[bcid] = std::make_pair(m_bunchCrossingTool->gapBeforeTrain(head) > 250,
                                                         -1);
      } else if(m_bunchCrossingTool->gapBeforeBunch(bcid, BC) == 1) {
        bcid_t tail = bcid ? bcid - 1 : MAX_BCID - 1; // wrap around
        m_distanceFromHeadOfTrain[bcid] = std::make_pair(m_bunchCrossingTool->gapBeforeTrain(tail) > 250,
                                                         m_bunchCrossingTool->distanceFromFront(tail, BC) + 1);
      } else {
        m_distanceFromHeadOfTrain[bcid] = std::make_pair(false, -10);
      }
    }
  }
  if(msgLvl(MSG::VERBOSE)) printPatternParsingInfo(msg(), m_bunchCrossingTool, m_distanceFromHeadOfTrain);
}

//================ dynamic pedestal ==============================================
// Return the dynamic pedestal.
// In case no correction is available or applicable this function
// returns the uncorrected pedestal.
int L1DynamicPedestalProviderTxt::dynamicPedestal(int iElement, int layer, int pedestal, int iBCID, float mu)
{
  if(iBCID < 0 || (unsigned)iBCID >= m_distanceFromHeadOfTrain.size()) return pedestal;

  // Only one bunch train is parameterized. Thus the BCID needs to be mapped
  // to the first train. The train starts at bcid = 1, thus the '+ 1'.
  // Bunches without available parameterization will have a value of -9 and a value of 0 is returned.
  auto bcidInfo = m_distanceFromHeadOfTrain[iBCID];
  bool longGap = bcidInfo.first;
  int bcid = bcidInfo.second + 1;
  
  if(bcid < 0) return pedestal;

  // iEta is given (despite its name) as element index
  int iEta = 33 - iElement - 1;

  // The parameterization is done for the dynamic pedestal correction,
  // i.e. correction = (dynamic_pedestal - pedestal).
  // Since the return value is expected to be the dynamic_pedestal itself, the
  // pedestal is added to the value obtained from the parameterization.
  int correction = 0;
  if(layer == 0) {
    correction = std::round(m_emParameterizations[longGap][iElement]->get(iEta, bcid, mu));
  } else if(layer == 1) {
    correction = std::round(m_hadParameterizations[longGap][iElement]->get(iEta, bcid, mu));
  } else {
    ATH_MSG_ERROR("Wrong layer index. Give 0 for Em, 1 for Had.");
  }
  return correction + pedestal;
}

//================ Parse input file ============================================
// parses the input file @fileName and fills the parameterizations into @params
// Each line is formatted as a space separated list of tokens. Two different input blocks exist,
// corresponding to the different parameterizations:
// i) Parameters used for the first parameterization (one line per eta-slice starting with a integer number)
// ii) Parameters used for the second parameterization (multiple lines starting with the same upper-case character)
//
// A block of type i) exists of a single linke, which looks like: "eta p_1 p_2 ... p_74".
// Here, eta is an integer denoting the eta-slice ([0, 66[) and p_[BCID] is the parameter of the
// correspoding BCID (floating point value)
//
// A block of type ii) consists of 4 lines each starting with the same character ([A-Z]):
// SECTION eta1 eta2 ... etaN
// SECTION q0_1 q0_2 ... q0_74
// SECTION q1_1 q1_2 ... q1_74
// SECTION q2_1 q2_2 ... q2_74
//
// where SECTION is the upper case character, etaN are the eta-slices this parameterization
// can be applied to and q0_[BCID], q1_[BCID], q2_[BCID] are the parameters for the parabola which parameterizes 
// p as a function of eta
//
// Currently the train-structure simulated in MC consists of 4 Trains (BCID in [1, 350]). Since currently all four trains
// are treated identically, only 74 BCID values are stored corresponding to one full train plus the unfilled bcid
// immediately before and after the train.
void L1DynamicPedestalProviderTxt::parseInputFile(const std::string& fileName, std::vector<IParameterization*>& params)
{
  std::ifstream ifs(fileName);
  
  int firstChar = ifs.peek();
  while(ifs.good()) {
    std::string line;

    if(firstChar == '#') {
        // skip comments - no comments are allowed inside a type ii) block
        std::getline(ifs, line);
    } else if (firstChar >= 'A' && firstChar <= 'Z') {
      ATH_MSG_VERBOSE("::parsing: found type ii) block.");
      // type ii) section - start parsing
      std::getline(ifs, line);

      // first line of new section contains the eta-slices this parameterization is applied to
      std::vector<int> etaSlices;
      std::string section = parseLine<std::string, int>(line, etaSlices);

      // next lines contain q0, q1 and q2
      std::getline(ifs, line);
      std::vector<float> q0;
      if(section != parseLine<std::string, float>(line, q0)) {
        // premature end of section
        throw ParseException("Premature end of section, expected q0.");
      }

      std::getline(ifs, line);
      std::vector<float> q1;
      if(section != parseLine<std::string, float>(line, q1)) {
        // premature end of section
        throw ParseException("Premature end of section, expected q1.");
      }

      std::getline(ifs, line);
      std::vector<float> q2;
      if(section != parseLine<std::string, float>(line, q2)) {
        // premature end of section
        throw ParseException("Premature end of section, expected q2.");
      }

      // check if section really ended by peeking next character
      if(ifs.good() && ifs.peek() == firstChar) {
        throw ParseException("Section to long.");
      }

      if(q0.size() != s_nBCIDPerTrain || q1.size() != s_nBCIDPerTrain || q2.size() != s_nBCIDPerTrain) {
        throw ParseException("Less entries than BCIDs.");
      }

      // create an object of type SecondParameterization and store a pointer to it for every eta-slice
      SecondParameterization *param = new SecondParameterization(q0, q1, q2);
      m_allocatedParameterizations.push_back(param);
      for(unsigned iSlice = 0; iSlice < etaSlices.size(); ++iSlice) {
        if(etaSlices[iSlice] < 0 || (unsigned)etaSlices[iSlice] >= s_nElements) {
          throw ParseException("Eta out of range");
        }
        if(params[etaSlices[iSlice]] != 0) {
          throw ParseException("Two parameterization for one eta slice.");
        }
        params[etaSlices[iSlice]] = param;
      }
    } else if (firstChar >= '0' && firstChar <= '9') {
      ATH_MSG_VERBOSE("::parsing: Found type i) block!");
      // type i) line - start tokenizing
      std::getline(ifs, line);

      std::vector<float> p; // holds the parameter values per BCID
      int etaSlice = parseLine<int, float>(line, p);

      if(etaSlice < 0 || (unsigned)etaSlice >= s_nElements) {
        throw ParseException("Eta out of range.");
      }

      // check parsed values
      if(p.size() != s_nBCIDPerTrain) {
        throw ParseException("Less entries than BCIDs.");
      }

      if(params[etaSlice] != 0) {
        throw ParseException("Two parameterization for one eta slice.");
      }
      FirstParameterization *param = new FirstParameterization(p);
      m_allocatedParameterizations.push_back(param);
      params[etaSlice] = param;
    } else {
      throw ParseException("Unknown first character.");
    }

    firstChar = ifs.peek(); // peek next char, will set eof-bit and terminate loop
  }

  // one last consistency check: see if every slice has a parameterization
  for(unsigned etaSlice = 0; etaSlice < s_nElements; ++etaSlice) {
    if(!params[etaSlice]) {
      throw ParseException("Not all slices are configured.");
    }
  }
}

float L1DynamicPedestalProviderTxt::FirstParameterization::get(int /* iEta */,
                                                               int iBCID, float mu) const
{
  if(iBCID < 0 || (unsigned)iBCID >= s_nBCIDPerTrain) return 0.0f;
  return m_p[iBCID] * mu;
}

float L1DynamicPedestalProviderTxt::SecondParameterization::get(int iEta,
                                                                int iBCID, float mu) const
{
  if(iBCID < 0 || (unsigned)iBCID >= s_nBCIDPerTrain) return 0.0f;
  if(iEta < 0 || (unsigned)iEta >= s_nElements) return 0.0f;

  // p is parameterized as a 2nd order polynomial of eta
  float p = m_q0[iBCID] + m_q1[iBCID] * iEta + m_q2[iBCID] * iEta * iEta;
  return p * mu;
}

} // end of namespace
