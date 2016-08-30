/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARSER_H
#define PARSER_H

// Framework includes
#include "AsgTools/AsgTool.h"
// Local includes
#include "TrigTauEmulation/IParser.h"

//
#include <map>
#include <vector>


class Parser : public virtual IParser, public asg::AsgTool

{

  ASG_TOOL_CLASS(Parser, IParser)


 public:
  Parser(const std::string& name);
  Parser(const Parser& other);
  Parser& operator=(const Parser& other) = delete;
  virtual ~Parser() {};

  virtual StatusCode initialize();

  // Tool execute
  virtual StatusCode execute(const std::string & chain_name);



  int get_pass_number(const std::string & item_name);
  bool check_TOPO_type(const std::string & TOPO_type);

  std::map<std::string, int> get_items(const std::string & type="");
  std::vector<std::string> get_vec_items(const std::string & type="");
  std::vector<std::string> get_tool_names(const std::string & type="");
  std::string return_TOPO_object_string();

 private:

  std::map<std::string, int> m_nonTOPO_items;

  std::vector<std::string> m_TOPO_items;

  std::vector<std::string> m_all_items;

  bool parse_chain_name(const std::string & chain_name);

  bool m_is_DR;

  bool m_is_BOX;

  bool m_is_OLR;

  std::string m_TOPO_object_name;

};
#endif
