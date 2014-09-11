/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterCharmNNTool.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
/// Created : January 2008
///
/// Adopted for Gaia by Dan Guest in 2013
///
/// DESCRIPTION:
///
/// This class computes the Neural Network probability for being b-jet, 
/// c-jet or uds-jet for 
/// a single event. The Undergraduate Tagger (TM) is used for training, 
/// C++11 code that avoids ROOT wherever possible.
///
/////////////////////////////////////////////////////////////////////////////

#include <string> 
#include <map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <boost/format.hpp>

#include "TObjString.h"

#include "JetTagTools/GaiaNNTool.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "JetTagInfo/IJetFitterTagInfo.h"
#include "JetTagInfo/JetFitterGenericTagInfo.h"
#include "JetTagTools/JetTaggerCB.h"


namespace Analysis {

  const std::string GaiaNNTool::SPECS = "specs"; 
  const std::string GaiaNNTool::TRAINING = "training"; 

  // variable names used internally to pass jet pt / eta around 
  const std::string GaiaNNTool::JET_PT = "pt"; 
  const std::string GaiaNNTool::JET_ETA = "eta"; 


  GaiaNNTool::GaiaNNTool(const std::string& name,
					     const std::string& n, 
					     const IInterface* p):
    AthAlgTool(name, n,p),
    m_calibrationDirectory("JetTaggerCb"),
    m_calibrationSubDirectory("NeuralNetwork"),
    m_calibrationTool("BTagCalibrationBroker"), 
    m_min_pt(20*1e3), 
    m_max_abs_eta(2.5)
  {
    declareProperty("calibration_directory",m_calibrationDirectory);
    declareProperty("calibration_subdirectory",m_calibrationSubDirectory);
    declareProperty("calibration_tool",	m_calibrationTool);

    m_variable_remapping["IP3DNeg"] = "IP3D"; 
    m_variable_remapping["SV1Flip"] = "SV1"; 
    m_variable_remapping["SV0Flip"] = "SV0"; 
    m_variable_remapping["IP3D_pu"] = "ip3d_pu"; 
    m_variable_remapping["IP3D_pc"] = "ip3d_pc"; 
    m_variable_remapping["IP3D_pb"] = "ip3d_pb"; 
    m_variable_remapping["IP3DNeg_pu"] = "ip3d_pu"; 
    m_variable_remapping["IP3DNeg_pc"] = "ip3d_pc"; 
    m_variable_remapping["IP3DNeg_pb"] = "ip3d_pb"; 
    m_variable_remapping["SVInfoPlus_mass"] = "svp_mass"; 
    m_variable_remapping["SVInfoPlus_energyfrac"] = "svp_efrc"; 
    declareProperty("variable_remapping", m_variable_remapping); 

    declareProperty("min_pt", m_min_pt); 
    declareProperty("max_abs_eta", m_max_abs_eta); 

    declareInterface<IJetFitterClassifierTool>(this);
  } 


  GaiaNNTool::~GaiaNNTool() {

    for (std::map<std::string,JetTagger::NeuralNet*>::iterator itr = 
	   m_networks.begin();
    	 itr != m_networks.end();
    	 itr++){ 
      delete itr->second; 
      itr->second = 0; 
    } 
  
  }

  StatusCode GaiaNNTool::initialize() {

    StatusCode sc = m_calibrationTool.retrieve();
    if (sc.isFailure())
      {
	ATH_MSG_FATAL( "Could not retrieve " << m_calibrationTool  
		      << ". Aborting...");
	return sc;
      } else ATH_MSG_INFO(" Retrieved: " << m_calibrationTool);

    initializeCalibrationFile();

    ATH_MSG_INFO(" Initialization of JetFitterNNTool succesfull");
    return StatusCode::SUCCESS;
  }

  void GaiaNNTool::initializeCalibrationFile()
  {

    registerHist(SPECS); 
    registerHist(TRAINING); 

    ATH_MSG_DEBUG(" Registered NN histograms with directory: " << 
		  m_calibrationDirectory << " and subdirectory " << 
		  m_calibrationSubDirectory);

    m_calibrationTool->printStatus();
    
  }
  
  void GaiaNNTool::registerHist(std::string name) 
  { 
    std::string full_name = m_calibrationSubDirectory + "/" + name; 
    m_calibrationTool->registerHistogram(m_calibrationDirectory, full_name); 
  }

  std::string GaiaNNTool
  ::getCalString(std::string jetauthor, std::string name)
  {
    std::string full_name = m_calibrationSubDirectory + "/" + name; 
    
    std::pair<TObject*, bool> stringpair = m_calibrationTool->
      retrieveTObject<TObject>(m_calibrationDirectory, 
			       jetauthor, full_name); 

    if (stringpair.second == true) {
      m_calibrationTool->updateHistogramStatus
	(m_calibrationDirectory, jetauthor, name, false);
    }
            
    TObjString* cal_string = dynamic_cast<TObjString*>(stringpair.first);
      
    if (cal_string == 0){
      std::string fuller_name = m_calibrationDirectory + '/' + 
        jetauthor + '/' + m_calibrationSubDirectory + '/' + name; 
      if (stringpair.first) { 
	fuller_name.append(" [but an object was found]"); 
      }
      throw std::runtime_error("Cannot retrieve string: " + fuller_name);
    } 
    std::string calibration(cal_string->GetString().Data()); 
    return calibration; 
  }
  
  StatusCode GaiaNNTool::finalize() {
    ATH_MSG_INFO(" Finalization of GaiaNNTool succesfull");
    return StatusCode::SUCCESS;
  }


  void 
  GaiaNNTool::fillLikelihoodValues(IJetFitterTagInfo& in_tag_info,
					     const std::string & jetauthor,
					     double jetpT,
					     double jeteta,
					     double /*IP3dlike*/) {
    if (jetauthor=="") {
      ATH_MSG_WARNING(" Hypothesis or jetauthor is empty."
		      " No likelihood value given back. ");
    }
    std::vector<double> weights = in_tag_info.tagLikelihood();

    if ( (jetpT < m_min_pt) || (std::abs(jeteta) > m_max_abs_eta )) { 
      weights.clear();
      weights.assign(3,0.0); 
      in_tag_info.setTagLikelihood(weights);
      return; 
    }

    //at each fillLikelihood call check if the calibration is updated
    //(or load it for the first time)
    loadCalibration(jetauthor);

    try { 
      dynamic_cast<JetFitterGenericTagInfo&>(in_tag_info); 
    }
    catch (const std::bad_cast& c) { 
      std::string error = "JetFitter is configured wrong: " __FILE__ 
	" requires JetFitterGenericTagInfo"
	" make sure you're using JetFitterCharmVariablesFactory";
      throw std::runtime_error(error); 
    }
  
    JetFitterGenericTagInfo& myTagInfo = 
      dynamic_cast<JetFitterGenericTagInfo&>(in_tag_info); 

    myTagInfo.setTemporaryDouble(JET_PT, jetpT / 1000); 
    myTagInfo.setTemporaryDouble(JET_ETA, jeteta); 

  
    if (weights.size()!=0) {
      weights.clear();
      ATH_MSG_WARNING(" The probability values for the tagging were"
		      " already filled. Inconsistency found...");
    }

    typedef std::map<std::string, int>::const_iterator ImapItr; 
    typedef std::map<std::string, double>::const_iterator DmapItr; 
    std::map<std::string, double> inputs; 
    for (ImapItr itr = myTagInfo.getIntsBegin(); 
    	 itr != myTagInfo.getIntsEnd(); itr++) { 
      ATH_MSG_DEBUG(itr->first.c_str() << " " << itr->second); 
      inputs[getRemapping(itr->first)] = itr->second; 
    }
    for (DmapItr itr = myTagInfo.getDoublesBegin(); 
    	 itr != myTagInfo.getDoublesEnd(); itr++) { 
      ATH_MSG_DEBUG(itr->first.c_str() << " " << itr->second); 
      inputs[getRemapping(itr->first)] = itr->second; 
    }
    
    // may throw a runtime error if the inputs aren't complete
    std::map<std::string, double> outputs =
      m_networks.find(jetauthor)->second->predict(inputs);

    ATH_MSG_DEBUG("NN output b: " << outputs["bottom"] << 
		  " c: " << outputs["charm"] << " u: " <<  outputs["light"]);

    // inversion with respect to NN output
    weights.push_back(outputs["bottom"]);//b 
    weights.push_back(outputs["light"]);//l
    weights.push_back(outputs["charm"]);//c
    if (outputs.count("tau")) { 
      weights.push_back(outputs["tau"]); 
    }

    myTagInfo.setTagLikelihood(weights);
    myTagInfo.makeValid(); 

  }

  void GaiaNNTool::loadCalibration(const std::string & jetauthor) {
    
    bool update = m_calibrationTool->retrieveHistogram
      (m_calibrationDirectory, jetauthor, SPECS).second;
    
    
    if ( (update == false) && (m_networks.count(jetauthor) == 1) ) {
      return;
    } 

    std::string specs = getCalString(jetauthor, SPECS); 
    std::string training = getCalString(jetauthor, TRAINING); 

    std::auto_ptr<JetTagger::NeuralNet> new_net(new JetTagger::NeuralNet); 
    std::stringstream spec_stream(specs);
    bool specs_loaded = new_net->load_specifications(spec_stream); 
    std::stringstream net_stream(training); 
    bool net_loaded = new_net->load_net(net_stream); 
    if (!specs_loaded) { 
      throw std::runtime_error("problem loading specs for " + jetauthor); 
    }
    if (!net_loaded) { 
      throw std::runtime_error("problem loading net for " + jetauthor); 
    }
    if (specs_loaded && net_loaded) { 
      if (m_networks.count(jetauthor)) { 
	delete m_networks[jetauthor]; 
	m_networks.erase(jetauthor); 
      }
      m_networks.insert(std::make_pair(jetauthor, new_net.get())); 
      new_net.release(); 
    }
  }

  std::string GaiaNNTool::getRemapping(const std::string& in) const { 
    std::map<std::string, std::string>::const_iterator remap = 
      m_variable_remapping.find(in); 
    if (remap == m_variable_remapping.end() ) { 
      return in; 
    }
    return remap->second; 
  }



}//end Analysis namespace
