/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//////////////////////////////////////////////////////////////////////
//  L1DynamicPedestalProviderTxt.cxx 
///////////////////////////////////////////////////////////////////////

#include "TrigT1CaloTools/L1DynamicPedestalProviderTxt.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "TrigBunchCrossingTool/BunchCrossing.h"

#include "PathResolver/PathResolver.h"
#include "CxxUtils/make_unique.h"
#include "CxxUtils/StringUtils.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <iterator>
#include <fstream>
#include <sstream>

using CxxUtils::make_unique;

namespace {
using bcid_t = Trig::IBunchCrossingTool::bcid_type;
static const bcid_t MAX_BCID = 3564;
}

namespace LVL1 
{

class L1DynamicPedestalProviderTxt::ParamFunc {
public:
  virtual double operator()(double mu) = 0;
  virtual ~ParamFunc() {}
};

class ParamFuncPol2 : public L1DynamicPedestalProviderTxt::ParamFunc {
public:
  ParamFuncPol2(double p0, double p1, double p2) : m_p{{p0, p1, p2}} {}
  virtual double operator()(double mu) { return m_p[0] + m_p[1]*mu + m_p[2]*mu*mu; }
private:
  std::array<double, 3> m_p;
};

class ParamFuncExp : public L1DynamicPedestalProviderTxt::ParamFunc {
public:
  ParamFuncExp(double p0, double p1, double p2) : m_p{{p0, p1, p2}} {}
  virtual double operator()(double mu) { return m_p[0]*(1-exp(-m_p[1]*mu)) + m_p[2]*mu; }
private:
  std::array<double, 3> m_p;
};

//================ Constructor ================================================
L1DynamicPedestalProviderTxt::L1DynamicPedestalProviderTxt(const std::string& t,
                                                           const std::string& n,
                                                           const IInterface*  p)
  : AthAlgTool(t, n, p)
  , m_bunchCrossingTool("Trig::MCBunchCrossingTool/BunchCrossingTool")
{
  declareInterface<IL1DynamicPedestalProvider>(this);

  // fill the vectors with default values - didn't find a more clever way due to move-only unique_ptr
  m_emParameterizations[0] = std::vector<std::vector<std::unique_ptr<ParamFunc>>>(s_nElements);
  m_emParameterizations[1] = std::vector<std::vector<std::unique_ptr<ParamFunc>>>(s_nElements);
  m_hadParameterizations[0] = std::vector<std::vector<std::unique_ptr<ParamFunc>>>(s_nElements);
  m_hadParameterizations[1] = std::vector<std::vector<std::unique_ptr<ParamFunc>>>(s_nElements);
  for(std::size_t i = 0; i < s_nElements; ++i) {
    m_emParameterizations[0][i] = std::vector<std::unique_ptr<ParamFunc>>(s_nBCIDPerTrain);
    m_emParameterizations[1][i] = std::vector<std::unique_ptr<ParamFunc>>(s_nBCIDPerTrain);
    m_hadParameterizations[0][i] = std::vector<std::unique_ptr<ParamFunc>>(s_nBCIDPerTrain);
    m_hadParameterizations[1][i] = std::vector<std::unique_ptr<ParamFunc>>(s_nBCIDPerTrain);
  }

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
  // keep destructor in .cxx file since ~unique_ptr needs full type
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
    log << endmsg;

    log << MSG::VERBOSE << "Distance    ";
    for(bcid_t j = bcid; j != std::min(MAX_BCID, bcid+20); ++j) log << std::setw(3) << result[j].second << " ";
    log << endmsg;

    log << MSG::VERBOSE << "LongGap?    ";
    for(bcid_t j = bcid; j != std::min(MAX_BCID, bcid+20); ++j) log <<  std::setw(3) << result[j].first << " ";
    log << endmsg;
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
    if(m_bunchCrossingTool->isFilled(bcid) || m_bunchCrossingTool->bcType(bcid) == Trig::IBunchCrossingTool::MiddleEmpty) {
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

  // The parameterization is done for the dynamic pedestal correction,
  // i.e. correction = (dynamic_pedestal - pedestal).
  // Since the return value is expected to be the dynamic_pedestal itself, the
  // pedestal is added to the value obtained from the parameterization.
  int correction = 0;
  if(layer == 0) {
    correction = std::round((*m_emParameterizations[longGap][iElement][bcid])(mu));
  } else if(layer == 1) {
    correction = std::round((*m_hadParameterizations[longGap][iElement][bcid])(mu));
  } else {
    ATH_MSG_ERROR("Wrong layer index. Give 0 for Em, 1 for Had.");
  }

  // Hardware cannot calculate a dynamic pedestal < 0, but it is possible the parameterisation can
  int dynamic_pedestal = correction + pedestal;
  if (dynamic_pedestal < 0) dynamic_pedestal = 0;

  return dynamic_pedestal;
}

// helpers for parsing in anonymous namespace
namespace {
  struct Context {
    enum ParserState {
      Element,
      Poly,
      Exp,
      BCID
    } P = Element;

    size_t E = 999;
    std::vector<size_t> poly;
    std::vector<size_t> exp;
    size_t N = 0;

    void reset() {
      N = 0;
      E = 999;
      poly.clear();
      exp.clear();
      P = Element;
    }
  };
} // namespace anonymous

//================ Parse input file ============================================
// parses the input file @fileName and fills the parameterizations into @params
// Each file is built out of block with the following format:
//     element [element]
//     poly [BCIDx] [BCID] ...
//     exp [BCIDy] [BCID] ...
//     [BCIDx] [P0(BCIDx)] [P1(BCIDx)] [P2(BCIDx)]
//     .
//     .
//     .
//     [BCIDy] [a0(BCIDy)] [a1(BCIDy)] [a2(BCIDy)] 
//
// where [] is a placeholder for actual numbers.
//
// The fits have the form:
//     poly: F(mu) = P0 + P1 * mu + P2 * mu * mu
//     exp:  F(mu) = -a0 * (1 - exp(-a1 * mu)) + a2 * mu
void L1DynamicPedestalProviderTxt::parseInputFile(const std::string& fileName,
                                                  std::vector<std::vector<std::unique_ptr<ParamFunc>>>& params)
{
  using CxxUtils::StringUtils::trim;
  using std::istream_iterator;

  std::ifstream F(fileName);
  Context ctx;

  // read file line-by-line
  for(std::string L; std::getline(F, L); ) {
    trim(L);
    if(L.empty()) continue; // ignore empty lines
    if(L[0] == '#') continue; // ignore comments

    std::istringstream S(L);

    // parsing
    if(ctx.P == Context::Element) {
      // start parsing a new element block
      std::string C = "";
      S >> C >> ctx.E;
      if(C != "element")
        throw ParseException("got '" + C + "' expected 'element'.");
      if(ctx.E > s_nElements)
        throw ParseException("element number (" +  std::to_string(ctx.E) + ") out-of-range.");
      
      ctx.P = Context::Poly; // advance state
    } else if(ctx.P == Context::Poly) {
      // start parsing a poly line
      std::string C = "";
      S >> C;
      if(C != "poly")  throw ParseException("got '" + C + "' expected 'poly'.");
      std::copy(istream_iterator<size_t>(S), istream_iterator<size_t>(), std::back_inserter(ctx.poly));

      ctx.P = Context::Exp; // advance state
    } else if(ctx.P == Context::Exp) {
      // start parsing a exp line
      std::string C = "";
      S >> C;
      if(C != "exp")  throw ParseException("got '" + C + "' expected 'exp'.");
      std::copy(istream_iterator<size_t>(S), istream_iterator<size_t>(), std::back_inserter(ctx.exp));

      ctx.P = Context::BCID; // advance state
    } else if(ctx.P == Context::BCID) {
      size_t B;
      std::vector<float> P;
      S >> B; // bcid
      std::copy(istream_iterator<float>(S), istream_iterator<float>(), back_inserter(P)); // parameters
      if(P.size() != 3) {
	throw ParseException("BCID=" + std::to_string(B) +
				 ": expected 3 parameters got " + std::to_string(P.size()));
      }
      if(std::binary_search(ctx.poly.begin(), ctx.poly.end(), B)) {
        params[ctx.E][B] = make_unique<ParamFuncPol2>(P[0], P[1], P[2]);
      } else if(std::binary_search(ctx.exp.begin(), ctx.exp.end(), B)) {
        params[ctx.E][B] = make_unique<ParamFuncExp>(P[0], P[1], P[2]);
      } else {
	throw ParseException("BCID '" + std::to_string(B) + "' didn't appear in 'poly' or 'exp' for element '" +
                             std::to_string(ctx.E) + "'.");
      }

      if(++ctx.N == (ctx.poly.size() + ctx.exp.size())) {
	// all bcids exhausted block is finished
        if(ctx.N != s_nBCIDPerTrain) {
          throw ParseException("Not all BCIDs filled");
        }

	ctx.reset();
      }
    }
  }

  for(auto& V : params) {
    if(std::find_if(V.begin(), V.end(), [](std::unique_ptr<ParamFunc>& p) { return p == nullptr; }) != V.end()) {
      throw ParseException("Not all elements and bcids filled!");
    }
  }
}

} // end of namespace LVL1
