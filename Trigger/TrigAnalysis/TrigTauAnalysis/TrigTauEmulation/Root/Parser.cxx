/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/Parser.h"
#include "TrigTauEmulation/MsgStream.h"

// Default constructor
Parser::Parser(const std::string& name) : asg::AsgTool(name)
{
  m_is_DR = false;
  m_is_BOX = false;
  m_is_OLR = false;
}

// Copy constructor
Parser::Parser(const Parser& other) : asg::AsgTool(other.name() + "_copy")
{
  m_is_DR = other.m_is_DR;
  m_is_BOX = other.m_is_BOX;
  m_is_OLR = other.m_is_OLR;
}

// Tool initialize
StatusCode Parser::initialize()
{
  return StatusCode::SUCCESS;
}


// Tool execute 
StatusCode Parser::execute(const std::string & chain_name)
{
  m_nonTOPO_items.clear();
  m_TOPO_items.clear();
  m_all_items.clear();
  m_is_DR = false;
  m_is_BOX = false;
  m_is_OLR = false;

  bool is_parsed = parse_chain_name(chain_name);
  if (is_parsed)
    return StatusCode::SUCCESS;
  else
    return StatusCode::FAILURE;
}


bool Parser::parse_chain_name(const std::string & chain_name)
{

  bool item_uses_TOPO = (chain_name.find("-") != std::string::npos);
  std::string L1chain_nonTOPO;
  std::string L1chain_TOPO;
  if(item_uses_TOPO){
    L1chain_nonTOPO = chain_name.substr(0,(chain_name.substr(0,chain_name.find("-"))).rfind("_")); //  finds the last dash before the TOPO part
    L1chain_TOPO = chain_name.substr((chain_name.substr(0,chain_name.find("-"))).rfind("_")+1);
  }
  else{
    L1chain_nonTOPO = chain_name;
  }
  std::vector<std::string> list_RoI;
  list_RoI.clear();
  if (L1chain_nonTOPO.find("_")!=std::string::npos) {
    while (L1chain_nonTOPO.find("_")!=std::string::npos) {
      std::string RoI_name = L1chain_nonTOPO.substr(0,L1chain_nonTOPO.find("_")).c_str();
      if(RoI_name != "L1") list_RoI.push_back(RoI_name);
      L1chain_nonTOPO = L1chain_nonTOPO.substr(L1chain_nonTOPO.find("_")+1);
    }
    if(L1chain_nonTOPO!=""){
      list_RoI.push_back(L1chain_nonTOPO);
    }
  }

  for(unsigned int ii = 0; ii < list_RoI.size(); ii++){
    std::string L1_trig_object = list_RoI.at(ii);
    int num_objects = 1; //default
    if(std::atoi(L1_trig_object.substr(0,1).c_str()) != 0){
      // just take the first character of the item (cannot do items with >9 items - but no triggers like this
      num_objects = std::atoi(L1_trig_object.substr(0,1).c_str());
      L1_trig_object = L1_trig_object.substr(1);
    }
    m_nonTOPO_items[L1_trig_object] = num_objects;
    m_all_items.push_back(L1_trig_object);
  }

  // NOW get the TOPO part of the name
  std::string TOPO_object_1 = "NULL"; // set the order of vector items
  std::string TOPO_object_2 = "NULL";
  std::string TOPO_object_3 = "NULL";
  if(item_uses_TOPO){
    if (L1chain_TOPO.find("DR")!=std::string::npos) {
      L1chain_TOPO = L1chain_TOPO.substr(3).c_str();
      m_is_DR = true; // for DR items
    }
    else if (L1chain_TOPO.find("BOX")!=std::string::npos) {
      L1chain_TOPO = L1chain_TOPO.substr(4).c_str();
      m_is_BOX = true; // for BOX items
    }
    if(L1chain_TOPO.find("-")!=std::string::npos){
      m_is_OLR = true; // now for overlap items
      std::string OLR_object = L1chain_TOPO.substr(L1chain_TOPO.rfind("-")+1).c_str();
      if(OLR_object == "") OLR_object = "NULL";
      TOPO_object_3 = OLR_object;
      if(L1chain_TOPO.find("_")!=std::string::npos){
        L1chain_TOPO = L1chain_TOPO.substr(0,L1chain_TOPO.rfind("_")).c_str(); // takes care of L1_DR-MU10TAU12I_TAU12I-J25 style names
      }
      else{
        L1chain_TOPO = L1chain_TOPO.substr(0,L1chain_TOPO.rfind("-")).c_str();
      }
    }
    // now break up the topo part of the name into constituents 
    if(L1chain_TOPO.rfind("TAU")!=std::string::npos){
      m_TOPO_object_name = L1chain_TOPO;
      std::string second_TOPO_object = L1chain_TOPO.substr(L1chain_TOPO.rfind("TAU")).c_str();
      if(second_TOPO_object == "") second_TOPO_object = "NULL";
      TOPO_object_2 = second_TOPO_object;
      L1chain_TOPO = L1chain_TOPO.substr(0,L1chain_TOPO.rfind("TAU")).c_str();
      if(L1chain_TOPO == "") L1chain_TOPO = "NULL";
      TOPO_object_1 = L1chain_TOPO;
    }
    else{
      // only items with one tau as OLR here and no topo DR/BOX cut
      if(L1chain_TOPO == "") L1chain_TOPO = "NULL";
      TOPO_object_2 = L1chain_TOPO;
    }

    m_TOPO_items.push_back(TOPO_object_1);
    m_TOPO_items.push_back(TOPO_object_2);
    m_TOPO_items.push_back(TOPO_object_3);

    // add the full name items to the all_items vector
    for (unsigned int i = 0; i < m_TOPO_items.size(); i++) {
      std::string item_name = m_TOPO_items[i];
      if(item_name.find("TAU")!=std::string::npos && item_name.find("I")!=std::string::npos)
        item_name = item_name + "M";
      if(item_name.find("EM")!=std::string::npos)
        item_name = item_name + "HI";
      m_TOPO_items[i] = item_name;
      if(item_name!="NULL")m_all_items.push_back(item_name);
    }

  }//end topo

  // Protection should be added in this method 
  // to check if the parsing can go wrong
  return true;
}


int Parser::get_pass_number(const std::string & item_name)
{
  auto search = m_nonTOPO_items.find(item_name);
  // Check that the find iterator does not reach the end of the map 
  // (will happens if the item does not exist.)
  if (search != m_nonTOPO_items.end())
    return m_nonTOPO_items[item_name];
  else
    MY_MSG_WARNING(item_name << " is not in the map of items"); 
  return -1;
}


std::map<std::string, int> Parser::get_items(const std::string & type)
{
  if (type == "") {
    return m_nonTOPO_items;
  } 

  std::map<std::string, int> selected_items;
  for (auto const &it : m_nonTOPO_items) {
    std::size_t found = (it.first).find(type);
    if (found != std::string::npos) {
      selected_items[it.first] = it.second;
    }
  }
  return selected_items;

}

std::vector<std::string> Parser::get_tool_names(const std::string & type)
{
  if (type == "") {
    return m_all_items;
  }

  std::vector<std::string> selected_items;
  for (auto const &it : m_all_items) {
    std::size_t found = it.find(type);
    if (found != std::string::npos)
      selected_items.push_back(it);
  }
  return selected_items;
}

std::vector<std::string> Parser::get_vec_items(const std::string & type)
{
  if (type == "TOPO") {
    // selects only vector of the topo items
    return m_TOPO_items;
  }

  return m_all_items;

}

bool Parser::check_TOPO_type(const std::string & TOPO_type)
{
  if(TOPO_type == "DR")
    return m_is_DR;
  else if(TOPO_type == "BOX")
    return m_is_BOX;
  else if(TOPO_type == "OLR")
    return m_is_OLR;
  else{
    MY_MSG_WARNING("Undefined TOPO requirement" << TOPO_type); 
    return 0;
  }
}

std::string Parser::return_TOPO_object_string()
{
  if(m_is_DR) {
    if (m_is_OLR) {
      return "DR-" + m_TOPO_items[0] + m_TOPO_items[1] + "-" + m_TOPO_items[2];
    } else {
      return "DR-" + m_TOPO_object_name;
    }
  } else if(m_is_BOX) {
    if (m_is_OLR) {
      return "BOX-" + m_TOPO_items[0] + m_TOPO_items[1] + "-" + m_TOPO_items[2];
    } else {
      return "BOX-" + m_TOPO_object_name;
    }
  } else if(m_is_OLR) {
    if (m_TOPO_items[0] == "NULL") {
      return m_TOPO_items[1] + "-" + m_TOPO_items[2];
    } else {
      return m_TOPO_items[0] + m_TOPO_items[1] + "-" + m_TOPO_items[2];
    }
  } else {
    return "";
  }
}
