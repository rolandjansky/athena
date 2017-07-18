/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataInterfaceTool.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "JetEvent/Jet.h"

#include "TF1.h"
#include "TMath.h"

#include <map>

using Analysis::CalibrationDataVariables;
using Analysis::CalibrationDataContainer;
using Analysis::UncertaintyResult;

using std::cerr;
using std::endl;
using std::string;

//================ Constructor =================================================

Analysis::CalibrationDataInterfaceTool::CalibrationDataInterfaceTool(const string& t,
								     const string& n,
								     const IInterface*  p ) :
  AthAlgTool(t,n,p),
  m_broker("PerformanceBroker")
{
  declareInterface<Analysis::ICalibrationDataInterfaceTool>(this);

  declareProperty("taggerName", m_taggerName = "undefined",
		  "tagging algorithm name");
  declareProperty("operatingPoints", m_operatingPoints,
		  "operating points for this tagging algorithm");
  declareProperty("efficiencyCalibrationBName", m_EffcalibrationBName = "default",
		  "efficiency calibration curve for b jets");
  declareProperty("efficiencyCalibrationCName", m_EffcalibrationCName = "default",
		  "efficiency calibration curve for c jets");
  declareProperty("efficiencyCalibrationTName", m_EffcalibrationTName = "default",
		  "efficiency calibration curve for tau jets");
  declareProperty("efficiencyCalibrationLightName", m_EffcalibrationLightName = "default",
		  "efficiency calibration curve for light-flavour jets");
  declareProperty("scaleFactorCalibrationBName", m_SFcalibrationBName = "default",
		  "scale factor calibration curve for b jets");
  declareProperty("scaleFactorCalibrationCName", m_SFcalibrationCName = "default",
		  "scale factor calibration curve for c jets");
  declareProperty("scaleFactorCalibrationTName", m_SFcalibrationTName = "default",
		  "scale factor calibration curve for tau jets");
  declareProperty("scaleFactorCalibrationLightName", m_SFcalibrationLightName = "default",
		  "scale factor calibration curve for light-flavour jets");
  declareProperty("PerformanceBroker", m_broker,
		  "tool interfacing with COOL Database");
}

//================ Destructor =================================================

Analysis::CalibrationDataInterfaceTool::~CalibrationDataInterfaceTool()
{}

//================ Interface stuff ===============================================

StatusCode Analysis::CalibrationDataInterfaceTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
   if ( riid == ICalibrationDataInterfaceTool::interfaceID() )  {
      *ppvIf = (ICalibrationDataInterfaceTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }

   return AthAlgTool::queryInterface( riid, ppvIf );
}

//================ Initialisation =================================================

StatusCode Analysis::CalibrationDataInterfaceTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  sc = m_broker.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("initialize() in " << name() << ": unable to retrieve CalibrationBroker tool!");
    return sc;
  }

  string::size_type end;

  std::vector<string> calibrationBNames;
  if (m_EffcalibrationBName.size() > 0) {
    do {
      end = m_EffcalibrationBName.find(";");
      calibrationBNames.push_back(m_EffcalibrationBName.substr(0,end));
      if (end != string::npos) m_EffcalibrationBName = m_EffcalibrationBName.substr(end+1);
    } while (end != string::npos);
  }

  std::vector<string> calibrationCNames;
  if (m_EffcalibrationCName.size() > 0) {
    do {
      end = m_EffcalibrationCName.find(";");
      calibrationCNames.push_back(m_EffcalibrationCName.substr(0,end));
      if (end != string::npos) m_EffcalibrationCName = m_EffcalibrationCName.substr(end+1);
    } while (end != string::npos);
  }

  std::vector<string> calibrationTNames;
  if (m_EffcalibrationTName.size() > 0) {
    do {
      end = m_EffcalibrationTName.find(";");
      calibrationTNames.push_back(m_EffcalibrationTName.substr(0,end));
      if (end != string::npos) m_EffcalibrationTName = m_EffcalibrationTName.substr(end+1);
    } while (end != string::npos);
  }

  std::vector<string> calibrationLightNames;
  if (m_EffcalibrationLightName.size() > 0) {
    do {
      end = m_EffcalibrationLightName.find(";");
      calibrationLightNames.push_back(m_EffcalibrationLightName.substr(0,end));
      if (end != string::npos) m_EffcalibrationLightName = m_EffcalibrationLightName.substr(end+1);
    } while (end != string::npos);
  }


  // insert the calibration names into a common object
  std::map<string, std::vector<string> > effNames;
  effNames["B"] = calibrationBNames;
  effNames["C"] = calibrationCNames;
  effNames["T"] = calibrationTNames;
  effNames["Light"] = calibrationLightNames;
  setEffCalibrationNames(effNames);

  // insert the calibration names into a common object
  std::map<string, string> calibrationNames;
  calibrationNames["B"] = m_SFcalibrationBName;
  calibrationNames["C"] = m_SFcalibrationCName;
  calibrationNames["T"] = m_SFcalibrationTName;
  calibrationNames["Light"] = m_SFcalibrationLightName;
  setSFCalibrationNames(calibrationNames);

  // register all objects
  for (std::map<string, string>::const_iterator it = calibrationNames.begin();
       it != calibrationNames.end(); ++it) {
    for (std::vector<string>::const_iterator op = m_operatingPoints.begin();
	 op != m_operatingPoints.end(); ++op) {
      registerObjects(it->first, *op);
    }
  }

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Analysis::CalibrationDataInterfaceTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//============================================================================================

//====================== efficiency scale factor retrieval =============================

Analysis::CalibResult
Analysis::CalibrationDataInterfaceTool::getScaleFactor (const Jet& jet, const string& label,
							const string& OP, Uncertainty unc) const
{
  // // For now, a calibration for the charm efficiency scale factor is assumed not to exist
  // if (label == "C") return getScaleFactor(jet, "B", OP, unc);

  // for light-flavour jets, rename from "N/A"
  string flavour(label);
  if (flavour == "N/A") flavour = "Light";

  string author = jet.jetAuthor();

  string sfName(getBasename(OP, flavour, "_SF", true));

  // Return a dummy result if the object is not found
  std::pair<CalibrationDataContainer*,bool> ret =
    m_broker->retrieveTObject<CalibrationDataContainer>(m_taggerName, author, sfName);
  CalibrationDataContainer* container = ret.first;
  if (! container) {
    ATH_MSG_WARNING("in " << name() << ": unable to find SF calibration for object with "
		    << "tagger/jetCollection/flavour/operating point = "
		    << m_taggerName << "/" << author << "/" << flavour << "/" << OP);
    return Analysis::dummyResult;
  }

  /* Here, it isn't obvious what to do with the second element of the pair returned:
     This indicates whether a new object has been loaded (presumably due to IOV changes),
     but the CalibrationDataContainer should take of any necessary computations itself.
   */

  // fill the CalibrationDataVariables object
  CalibrationDataVariables variables;
  makeVariables (jet, variables);

  // always retrieve the result itself
  double value;
  if (container->getResult(variables, value) == CalibrationDataContainer::kError)
    return Analysis::dummyResult;

  // retrieve the statistical uncertainty if desired
  double stat(0);
  if (unc == Total || unc == Statistical) {
    if (container->getStatUncertainty(variables, stat) == CalibrationDataContainer::kError)
      cerr << "getScaleFactor: error retrieving Scale factor statistical uncertainty!"
	   << endl;
  }
  UncertaintyResult resSyst(0,0);
  if (unc == Total || unc == Systematic) {
    if (container->getSystUncertainty(variables, resSyst) == CalibrationDataContainer::kError)
      cerr << "getScaleFactor: error retrieving Scale factor parameter systematic uncertainty!"
	   << endl;
  }

  double uncertainty = combinedUncertainty(stat, resSyst);
  Analysis::CalibResult result = std::make_pair(value, uncertainty);

  result.first = std::max(0., result.first);
  if (TMath::Abs(result.first) < Analysis::CalibZERO)
    result.first = 1.;
  return result;
}

//====================== "MC" efficiency retrieval ======================================

Analysis::CalibResult
Analysis::CalibrationDataInterfaceTool::getMCEfficiency (const Jet& jet, const string& label,
							 const string& OP, Uncertainty unc) const
{
  // extract the relevant jet quantities: kinematic variables and jet author

  // for light-flavour jets, rename from "N/A"
  string flavour(label);
  if (flavour == "N/A") flavour = "Light";

  string author = jet.jetAuthor();

  string effName(getBasename(OP, flavour, "_Eff", false));

  // Return a dummy result if the object is not found
  std::pair<CalibrationDataContainer*,bool> ret =
    m_broker->retrieveTObject<CalibrationDataContainer>(m_taggerName, author, effName);
  CalibrationDataContainer* container = ret.first;
  if (! container) {
    ATH_MSG_WARNING("in " << name() << ": unable to find Eff calibration for object with "
		    << "tagger/jetCollection/flavour/operating point = "
		    << m_taggerName << "/" << author << "/" << flavour << "/" << OP);
    return Analysis::dummyResult;
  }

  /* Here, it isn't obvious what to do with the second element of the pair returned:
     This indicates whether a new object has been loaded (presumably due to IOV changes),
     but the CalibrationDataContainer should take of any necessary computations itself.
   */

  // fill the CalibrationDataVariables object
  CalibrationDataVariables variables;
  makeVariables (jet, variables);

  // always retrieve the result itself
  double value;
  if (container->getResult(variables, value) == CalibrationDataContainer::kError)
    return Analysis::dummyResult;

  // retrieve the statistical uncertainty if desired
  double stat(0);
  if (unc == Total || unc == Statistical) {
    if (container->getStatUncertainty(variables, stat) == CalibrationDataContainer::kError)
      cerr << "getMCEfficiency: error retrieving MC efficiency statistical uncertainty!"
	   << endl;
  }
  UncertaintyResult resSyst(0,0);
  if (unc == Total || unc == Systematic) {
    if (container->getSystUncertainty(variables, resSyst) == CalibrationDataContainer::kError)
      cerr << "getMCEfficiency: error retrieving MC efficiency parameter systematic uncertainty!"
	   << endl;
  }

  double uncertainty = combinedUncertainty(stat, resSyst);
  Analysis::CalibResult result = std::make_pair(value, uncertainty);

  result.first = std::max(0., std::min(1., result.first));
  return result;
}

//====================== efficiency retrieval ==========================================

Analysis::CalibResult
Analysis::CalibrationDataInterfaceTool::getEfficiency (const Jet& jet, const string& label,
						       const string& OP, Uncertainty unc) const
{
  Analysis::CalibResult sfResult = getScaleFactor(jet, label, OP, unc);
  Analysis::CalibResult effResult = getMCEfficiency(jet, label, OP, unc);

  double relative = 0;
  double value = effResult.first*sfResult.first;
  if (value != 0) {
    relative = effResult.second/effResult.first;
    double sfRelative = sfResult.second/sfResult.first;
    relative = TMath::Sqrt(sfRelative*sfRelative + relative*relative);
  } else {
    ATH_MSG_WARNING("in " << name() << ": null result, SF=" << sfResult.first
		    << " MC eff=" << effResult.first);
    relative = Analysis::dummyValue;
  }

  return std::make_pair(value,value*relative);
}

//============================================================================================

void Analysis::CalibrationDataInterfaceTool::registerObjects(const string& flavour,
							     const string& OP) const
{
  static const string slash("/");

  string common = OP; common += slash;
  common += flavour; common += slash;
  string nameEff(common); nameEff += EffCalibrationName(flavour); nameEff += "_Eff";
  string nameSF(common); nameSF += SFCalibrationName(flavour); nameSF += "_SF";

  m_broker->registerHistogram(m_taggerName, nameSF);
  m_broker->registerHistogram(m_taggerName, nameEff);
}

void
Analysis::CalibrationDataInterfaceTool::makeVariables (const Jet& jet,
						       CalibrationDataVariables& x) const
{
  // msg() << MSG::DEBUG << "jet variables:";
  x.jetAuthor = jet.jetAuthor();
  x.jetPt = jet.pt() * 0.001;  // NB convert from MeV to GeV!
  x.jetEta = jet.eta();
}
