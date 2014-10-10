//////////////////////////////////////////////////////////////////////
//  L1DynamicPedestalProviderTxt.cxx (c) Veit Scjarf
///////////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1DynamicPedestalProviderTxt.h"

#include "PathResolver/PathResolver.h"

#include <cmath>
#include <fstream>

namespace LVL1 
{

//================ Constructor ================================================
L1DynamicPedestalProviderTxt::L1DynamicPedestalProviderTxt(const std::string& t,
                                                           const std::string& n,
                                                           const IInterface*  p)
  : AthAlgTool(t, n, p)
  , m_emParameterizations(s_nEtaSlices, 0)
  , m_hadParameterizations(s_nEtaSlices, 0)
{
  declareInterface<IL1DynamicPedestalProvider>(this);

  // The pattern of filled BCIDs. In future this should be retrieved from
  // the "/Digitization/Parameters" container of the MetaDataStore. For now
  // this is passed as an property.
  declareProperty("BeamIntensityPattern", m_beamIntensityPattern);

  // Input files containing the parameters for the electromagnetic and hadronic
  // layer, respectively.
  declareProperty("InputFileEM", m_inputFileEM);
  declareProperty("InputFileHAD", m_inputFileHAD);
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
  // parse parameterization for the electromagnetic layer
  std::string fileNameEM = PathResolver::find_file(m_inputFileEM, "DATAPATH");
  if(fileNameEM.empty()) {
    ATH_MSG_FATAL("Could not resolve input file: " << m_inputFileEM);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("::initialize: resolved input file: " << fileNameEM);

  try {
    parseInputFile(fileNameEM, m_emParameterizations);
  } catch (const ParseException& e) {
    ATH_MSG_FATAL("Could not parse input file: " << fileNameEM << "; error: " << e.what());
    return StatusCode::FAILURE;
  }

  // parse parameterization for the hadronic layer
  std::string fileNameHAD = PathResolver::find_file(m_inputFileHAD, "DATAPATH");
  if(fileNameHAD.empty()) {
    ATH_MSG_FATAL("Could not resolve input file: " << m_inputFileHAD);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("::initialize: resolved input file: " << fileNameHAD);

  try {
    parseInputFile(fileNameHAD, m_hadParameterizations);
  } catch (const ParseException& e) {
    ATH_MSG_FATAL("Could not parse input file: " << fileNameHAD << "; error: " << e.what());
    return StatusCode::FAILURE;
  }

  fillDistanceFromHeadOfTrain();

  return StatusCode::SUCCESS;
}

// "Parse" the beam intensity pattern to get the bunch train structure.
// Assumptions: -) 25ns bunch spacing
//              -) Relative intensity per filled bunch: > 0.1
// A more clever implementation exists at TrigBunchCrossingTool/MCBunchCrossingTool
// but at the time of writing, the implementation doesn't work anymore.
void L1DynamicPedestalProviderTxt::fillDistanceFromHeadOfTrain()
{
  const int nBCID = m_beamIntensityPattern.size();
  m_distanceFromHeadOfTrain.resize(nBCID, -10); // store -10 as default in each slot

  int trainHeadBCID = -1; // index of the current head of train
  for(int iBCID = 0; iBCID < nBCID; ++iBCID) {
    if(m_beamIntensityPattern[iBCID] < 0.1) {
        // Unfilled bunch. Consider two special cases:
        // i) the next bunch is filled -> store -1
        // ii) the previous bunch was filled -> store train length + 1
        // Otherwise store -10 as no dynamic pedestal value is available
        if(iBCID + 1 < nBCID && m_beamIntensityPattern[iBCID+1] > 0.1) {
          ATH_MSG_VERBOSE("Found unfilled bunch before train BCID=" << iBCID);
          // case i)
          m_distanceFromHeadOfTrain[iBCID] = -1;
        } else if(iBCID > 0 && m_beamIntensityPattern[iBCID-1] > 0.1) {
          ATH_MSG_VERBOSE("Found unfilled bunch after train BCID=" << iBCID);
          m_distanceFromHeadOfTrain[iBCID] = iBCID - trainHeadBCID;

          // consistency check: the number of bunches per train should be the same as s_nBCIDPerTrain - 2
          if(iBCID - trainHeadBCID != s_nBCIDPerTrain - 2) {
            ATH_MSG_WARNING("Train length is not matching compile-time value"
                            "of L1DynamicPedestalProviderTxt::s_nBCIDperTrain!");
          }

          // after last bunch of train, train ended
          trainHeadBCID = -1;
        }
        // nothing to do here since -10 is the default
    } else {
      // Filled bunch.
      if(trainHeadBCID < 0) {
        // found new train
        trainHeadBCID = iBCID; 
        ATH_MSG_VERBOSE("Found new train BCID=" << iBCID);
      }
      m_distanceFromHeadOfTrain[iBCID] = iBCID - trainHeadBCID;
    }
  }
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
  int bcid = m_distanceFromHeadOfTrain[iBCID] + 1;
  
  if(bcid < 0) return pedestal;

  // iEta is given (despite its name) as element index
  int iEta = 33 - iElement - 1;
  ATH_MSG_VERBOSE("iEta transformed to " << iEta << " from " << iElement);

  // The parameterization is done for the dynamic pedestal correction,
  // i.e. correction = (dynamic_pedestal - pedestal).
  // Since the return value is expected to be the dynamic_pedestal itself, the
  // pedestal is added to the value obtained from the parameterization.
  int correction = 0;
  if(layer == 0) {
    correction = std::round((m_emParameterizations[iEta])->get(iEta, bcid, mu));
  } else if(layer == 1) {
    correction = std::round((m_hadParameterizations[iEta])->get(iEta, bcid, mu));
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
        if(etaSlices[iSlice] < 0 || (unsigned)etaSlices[iSlice] >= s_nEtaSlices) {
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

      if(etaSlice < 0 || (unsigned)etaSlice >= s_nEtaSlices) {
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
  for(unsigned etaSlice = 0; etaSlice < s_nEtaSlices; ++etaSlice) {
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
  if(iEta < 0 || (unsigned)iEta >= s_nEtaSlices) return 0.0f;

  // p is parameterized as a 2nd order polynomial of eta
  float p = m_q0[iBCID] + m_q1[iBCID] * iEta + m_q2[iBCID] * iEta * iEta;
  return p * mu;
}

} // end of namespace
