/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <sstream>

// ROOT
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"

// Boost
#include <boost/functional/hash.hpp>

// Local
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostExec/Util.h"

using namespace std;

//-----------------------------------------------------------------------------------------      
bool Anp::ReadROSXML(const std::string &xml_path, std::vector<ROS> &ros_vec, bool debug)
{ 
  //
  // Read registry at input path and configure RunModule
  //   
  Registry ros_reg = Anp::ParseXml(xml_path);

  //
  // Hash function from ROS name
  //
  boost::hash<string> string_hash;

  while(ros_reg.KeyExists("ros")) {
    //
    // Read ROB ids for single ROS
    //
    Registry ros;
    ros_reg.Get("ros", ros);
    ros_reg.RemoveKey("ros");

    ROS data;    
    ros.Get("id", data.ros);

    if(debug) {
      cout << "ROS =" << data.ros << endl;
    }

    //
    // Read ROBINs
    //
    if(ros.KeyExists("robin")){
      std::set<uint32_t> ros_robs;
      while(ros.KeyExists("robin")) {
	
	Registry robin;
	ros.Get("robin", robin);
	ros.RemoveKey("robin");
	
	string robin_name;
	robin.Get("id", robin_name);
	
	// Create new ROBIN
	ROBIN data_robin;
	data_robin.robin    = robin_name;
	data_robin.robin_id = string_hash(robin_name);

	if(debug) { 
	  cout << "   ROBIN =" << data_robin.robin 
	       << "   Hash  =" << data_robin.robin_id   << endl;
	}

	std::set<uint32_t> robin_robs = ReadROB(robin, debug);
	
	data_robin.robs.insert(robin_robs.begin(),robin_robs.end());

	ros_robs.insert(robin_robs.begin(),robin_robs.end());

	data.robins.push_back(data_robin);
      }
      data.robs.insert(ros_robs.begin(),ros_robs.end());
    }
    else{
      std::set<uint32_t> ros_robs = ReadROB(ros, debug);
      data.robs.insert(ros_robs.begin(),ros_robs.end());
    }

    //
    // Save ROS 
    //
    data.ros_id = string_hash(data.ros);
    ros_vec.push_back(data);
    
    if(debug) {
      cout << "ROS: " << data.ros << " hash=" << data.ros_id << endl;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------------------   
std::set<uint32_t> Anp::ReadROB(Registry &ros, bool debug)
{
  //
  // Read ROB ids
  //
  std::set<uint32_t> robs;
  while(ros.KeyExists("rob")) {
    Registry rob;
    ros.Get("rob", rob);
    ros.RemoveKey("rob");
    
    string rob_id;
    rob.Get("id", rob_id);
    
    stringstream rob_str;
    rob_str << rob_id;
    
    uint32_t rob_int = 0;
    rob_str >> hex >> rob_int;
    
    if(debug) { 
      cout << "   ROB hex=" << rob_id << endl
	   << "   ROB int=" << rob_int << endl;
    }
    
    robs.insert(rob_int);
  }

  return robs;
}

//-----------------------------------------------------------------------------------------   
std::string Anp::AsStringRoIType(TrigMonRoi::Type type)
{
  
  if     (type == TrigMonRoi::kMuon)   return "muon";
  else if(type == TrigMonRoi::kJet)    return "jet";
  else if(type == TrigMonRoi::kJetEt)  return "jetet";
  else if(type == TrigMonRoi::kEmTau)  return "emtau";
  else if(type == TrigMonRoi::kEnergy) return "te";

  return "none";
}

//-----------------------------------------------------------------------------------------   
std::string Anp::AsStringDetType(unsigned type)
{
  
  if     (type == 0x1) return "pixel";
  else if(type == 0x2) return "sct";
  else if(type == 0x3) return "trt";
  else if(type == 0x4) return "lar";
  else if(type == 0x5) return "tile";
  else if(type == 0x6) return "muon";
  else if(type == 0x7) return "tdaq";
  else if(type == 0x8) return "fwd";
  else if(type == 0xf) return "other";

  return "none";
}

//--------------------------------------------------------------------------------------      
TH1* Anp::MakeLBHist(const std::string &hname, 
		     const std::string &yaxis, 
		     unsigned min_lb, 
		     unsigned max_lb,
		     TDirectory *dir)
{
  //
  // Create histogram
  //
  if(!(min_lb > 0 && min_lb < max_lb)) {
    return 0;
  }

  TH1 *h = new TH1F(hname.c_str(), "", max_lb-min_lb+1, min_lb, max_lb+1);
  h->SetDirectory(dir);
  h->SetStats(false);
  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetTitle("luminosity block");
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(yaxis.c_str());  
  
  if(!h) cout << "ERROR - MakeLBHist failed for histogram: " << hname.c_str() << endl;
  
  return h;
}

//--------------------------------------------------------------------------------------      
std::pair<unsigned,unsigned> Anp::GetMaxLB(const Exec::Summary &sum)
{
  //
  // Find LB range
  //
  unsigned min_lb = 0, max_lb = 0;

  for(unsigned i = 0; i < sum.sum_lbs.size(); ++i) {
    const Exec::SummaryLB &s = sum.sum_lbs.at(i);

    if(i == 0) {
      min_lb = s.lb;
      max_lb = s.lb;
    }
    else {
      min_lb = std::min<unsigned>(min_lb, s.lb);
      max_lb = std::max<unsigned>(max_lb, s.lb);
    }
  }
  
  pair<unsigned,unsigned> lbs(min_lb,max_lb);

  return lbs;
}
