/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/PAlgorithm.h"
#include "DataQualityInterfaces/PMetadata.h"
#include "DataQualityInterfaces/PParameter.h"
#include "DataQualityInterfaces/PRegion.h"
#include "DataQualityInterfaces/PResult.h"
#include "DataQualityInterfaces/PFunctions.h"
#include "boost/lexical_cast.hpp"
#include "TDirectory.h"
#include "TProcessID.h"
#include <string>
#include <iostream>

//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqm_persistency::PAlgorithm)
///\endcond
//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqm_persistency::PMetadata)
///\endcond
//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqm_persistency::PParameter)
///\endcond
//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqm_persistency::PRegion)
///\endcond
//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqm_persistency::PResult)
///\endcond

namespace dqm_persistency {

  PParameter::PParameter() :
    name(""),
    input(0),
    result(0),
    algorithm(0),
    reference(0),
    weight(1),
    disableChildren(false),
    metadata(0),
    shape(0),
    parentRegion(-2) {}

  PRegion::PRegion() :
    PParameter(),
    layout(0) {}

  PResult::PResult() :
    resultobj(0),
    status(),
    timestamp() {}
  
  void PParameter::Print(Option_t*) const {
    std::cout << "Parameter " << name << "\n"
	      << " Input " << input << "\n"
	      << " Results " << result
	      << std::endl;
    if (result) result->Print();
    std::cout << " Algorithm " << algorithm << "\n";
    if (algorithm) algorithm->Print();
    std::cout << " Reference " << reference << "\n"
	      << " Weight " << weight << "\n"
	      << " disableChildren " << disableChildren << "\n"
	      << " metadata " << metadata << "\n";
    if (metadata) metadata->Print();
    std::cout << " shape " << shape << "\n"
	      << " parent region " << parentRegion
	      << std::endl;
  }

  PRegion* PParameter::GetParentRegion(TDirectory& topdir) {
    if (parentRegion == -2) { return 0; }
    if (parentRegion == -1) { 
      return dynamic_cast<PRegion*>(GetNode(topdir, -1));
    }
    return dynamic_cast<PRegion*>(GetNode(topdir, parentRegion));
  }

  void PRegion::Print(Option_t* opt) const {
    std::cout << "Region " << name << std::endl;
    PParameter::Print(opt);
    std::cout << "# daughter parameters: " << subparameters.size()
	      << std::endl;
  }

  void PRegion::AddSubparameter(PParameter& daughter) {
    int uid = TProcessID::AssignID(&daughter);
    if (!(TProcessID::GetPID()->GetObjectWithID(uid))) {
      TProcessID::GetPID()->PutObjectWithID(&daughter);
    }
    subparameters.push_back(uid);
    if (name != "top_level") {
      uid = TProcessID::AssignID(this);
      if (!(TProcessID::GetPID()->GetObjectWithID(uid))) {
	TProcessID::GetPID()->PutObjectWithID(this);
      }
      daughter.parentRegion = uid;
    } else {
      daughter.parentRegion = -1;
    }
  }

  PParameter* PRegion::GetSubparameter(TDirectory& topdir, unsigned int i) const {
    if (i >= subparameters.size()) {
      std::cerr << "ERROR: attempt to access invalid subparameter " << i
		<< std::endl
		<< "       valid values are in [0," << subparameters.size()
		<< ")" << std::endl;
    }
    return GetNode(topdir, subparameters[i]);
  }

  void PResult::Print(Option_t*) const {
    std::cout << "  Status: " << status << std::endl;
    for (std::vector<std::pair<std::string, float> >::const_iterator ri = results.begin();
	 ri != results.end(); ++ri) {
      std::cout << "  " << ri->first << " " << ri->second << std::endl;
    }
  }

  void PAlgorithm::Print(Option_t*) const {
    std::cout << "  Algorithm name " << name << "\n"
    	      << "  library " << library << "\n"
    	      << "  parameters: " << "\n";
    for (std::map<std::string, std::vector<float> >::const_iterator pit = parameters.begin();
    	   pit != parameters.end(); ++pit) {
      //std::cout << "   " << pit->first << " " << pit->second << "\n";
      std::cout << "   " << pit->first << " ";
      for (std::vector<float>::const_iterator p2it = pit->second.begin();
      	   p2it != pit->second.end(); ++p2it) {
      	std::cout << *p2it << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "  Green thresholds: " << "\n";
    for (std::map<std::string, float>::const_iterator thit = greenThresholds.begin();
	 thit != greenThresholds.end(); ++thit) {
      std::cout << "   " << thit->first << " " << thit->second << std::endl;
    }
    std::cout << "  Red thresholds: " << "\n";
    for (std::map<std::string, float>::const_iterator thit = redThresholds.begin();
    	 thit != redThresholds.end(); ++thit) {
      std::cout << "   " << thit->first << " " << thit->second << std::endl;
    }
  }
	   
  void PMetadata::Print(Option_t*) const {
    for (std::map<std::string, std::string >::const_iterator pit = data.begin();
	 pit != data.end(); ++pit) {
      std::cout << "   " << pit->first << " " << pit->second << "\n";
    }
  } 

  void Print(const PParameter* param, TDirectory* topdir, 
	     Option_t* opt) {
    param->Print();
    const PRegion* region = dynamic_cast<const PRegion*>(param);
    if (region) {
      TString option = opt; option.ToLower();
      if (option.Contains("r")) {
	for (unsigned int i = 0; i < region->subparameters.size(); i++) {
	  std::cout << "Subparameter " << region->subparameters[i] << std::endl;
	  const PParameter* subparam = region->GetSubparameter(*topdir, i);
	  if (subparam) { 
	    Print(subparam, topdir, opt); 
	  } else {
	    std::cerr << "ERROR: can't find subparameter " 
		      << region->subparameters[i]
		      << " from top-level directory "
		      << topdir->GetPath()
		      << std::endl;
	  }
	}
      }
    }
  }

  void WriteTreeRecurse(PRegion& region, TDirectory& paramdir, 
			PMetadata& topmap, const std::string& path) {
    TProcessID* pid = TProcessID::GetPID();
    for (unsigned int i = 0; i < region.subparameters.size(); i++) {
      int subparam = region.subparameters[i];
      TObject* subobj = pid->GetObjectWithID(subparam);
      PParameter* pparam = dynamic_cast<PParameter*>(subobj);
      if (!pparam) {
	std::cerr << "ERROR: can't retrieve parameter " << subparam
		  << std::endl;
	continue;
      }
      std::string uid(boost::lexical_cast<std::string>(subparam));
      if (path == "") {
	topmap.data[pparam->name] = uid;
      } else {
	topmap.data[path + "/" + pparam->name] = uid;
      }
      paramdir.WriteTObject(subobj, uid.c_str());
      PRegion* preg = dynamic_cast<PRegion*>(subobj);
      if (preg) {
	std::string daughterpath = (path != "") ? (path+"/"+preg->name) : (preg->name) ;
	WriteTreeRecurse(*preg, paramdir, topmap, daughterpath);
      }
    }
  }

  // The tree has to be fully loaded with TProcessID available for all elements!
  void WriteTree(PRegion& top_level, TDirectory& topdir, PMetadata& topmap) {
    TDirectory* paramdir = topdir.GetDirectory("Parameters");
    if (!paramdir) {
      paramdir = topdir.mkdir("Parameters");
      if (!paramdir) {
	std::cerr << "ERROR: can't make Parameters subdirectory for storage"
		  << std::endl;
	return;
      }
    }
    WriteTreeRecurse(top_level, *paramdir, topmap, "");
    topmap.data["top_level"] = boost::lexical_cast<std::string>(top_level.GetUniqueID());
    topdir.WriteTObject(&top_level, "top_level");
    topdir.WriteTObject(&topmap, "object_map");
  }

  PParameter* GetNode(TDirectory& topdir, const std::string& nodename) {
    PParameter* rv(0);
    if (nodename == "top_level") {
      rv = dynamic_cast<PParameter*>(topdir.Get("top_level"));
      return rv;
    }
    PMetadata* pmd = dynamic_cast<PMetadata*>(topdir.Get("object_map"));
    if (pmd) {
      std::string key = pmd->data[nodename];
      if (key == "") {
	std::cerr << "ERROR: can't find node " << nodename << " in object_map"
		  << std::endl;
	return 0;
      } else {
	rv = dynamic_cast<PParameter*>(topdir.Get(("Parameters/" + key).c_str()));
	if (rv) { rv->SetUniqueID(rv->GetUniqueID() & 0xffffff); }
	return rv;
      }
    } else {
      std::cerr << "ERROR: can't retrieve object_map from file!" << std::endl;
      return 0;
    }
  }

  PParameter* GetNode(TDirectory& topdir, int key) {
    if (key == -1) {
      return dynamic_cast<PParameter*>(topdir.Get("top_level"));
    }
    PParameter* rv = dynamic_cast<PParameter*>(topdir.Get(("Parameters/" + boost::lexical_cast<std::string>(key)).c_str()));
    if (rv) { rv->SetUniqueID(rv->GetUniqueID() & 0xffffff); }
    return rv;
  }
}
