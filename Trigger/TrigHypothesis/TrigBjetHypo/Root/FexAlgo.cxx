/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define FEXALGO_CXX

#include "FexAlgo.h"
#include <string>
#include "TPython.h"
#include <sstream>
#include <iostream>
#include "MsgStream.h"

const float mm = 1;
const float GeV = 1e3;
const float TeV = 1e6;

  template <typename T>
  void HLT::declareProperty(const std::string &m_name, const std::string &pname, T &prop) {
    std::stringstream item_exp;
    item_exp << "try:" << std::endl;
    item_exp << "  a = " << m_name << "." << pname << std::endl;
    item_exp << "except:" << std::endl;
    item_exp << "  a = " << prop << std::endl;
    TPython::Exec(item_exp.str().c_str());
    item_exp.str("");
    item_exp << "a";
    prop = (T) TPython::Eval(item_exp.str().c_str());
  }

  template <>
  void HLT::declareProperty<bool>(const std::string &m_name, const std::string &pname, bool &prop) {
    std::stringstream item_exp;
    item_exp << "try:" << std::endl;
    item_exp << "  a = " << m_name << "." << pname << std::endl;
    item_exp << "except:" << std::endl;
    if (prop == true)
      item_exp << "  a = True" << std::endl;
    else
      item_exp << "  a = False" << std::endl;
    TPython::Exec(item_exp.str().c_str());
    item_exp.str("");
    item_exp << "a";
    int i = -1;
    i = (int) TPython::Eval(item_exp.str().c_str());
    if (i == -1) return;
    prop = (i == 0)?false:true;
  }

  template <>
  void HLT::declareProperty<std::string>(const std::string &m_name, const std::string &pname, std::string &prop) {
    std::stringstream item_exp;
    item_exp << "try:" << std::endl;
    item_exp << "  a = " << m_name << "." << pname << std::endl;
    item_exp << "except:" << std::endl;
    item_exp << "  a = \"" << prop << "\"" << std::endl;
    TPython::Exec(item_exp.str().c_str());
    item_exp.str("");
    item_exp << "a";
    prop = (const char *) TPython::Eval(item_exp.str().c_str());
  }

  template <typename T>
  void HLT::declareProperty(const std::string &m_name, const std::string &pname, std::vector<T> &prop) {
    prop.clear();
    std::stringstream len_exp;
    len_exp << "try:" << std::endl;
    len_exp << "  a = len(" << m_name << "." << pname << ")" << std::endl;
    len_exp << "except:" << std::endl;
    len_exp << "  a = -1" << std::endl;
    TPython::Exec(len_exp.str().c_str());
    len_exp.str("");
    len_exp << "a";
    int len = TPython::Eval(len_exp.str().c_str());
    if (len < 0) return;
    for (int i = 0; i < len; ++i) {
      std::stringstream item_exp;
      item_exp << m_name << "." << pname << "[" << i << "]";
      T item = TPython::Eval(item_exp.str().c_str());
      prop.push_back(item);
    }
  }

  template <>
  void HLT::declareProperty<std::string>(const std::string &m_name, const std::string &pname, std::vector<std::string> &prop) {
    prop.clear();
    std::stringstream len_exp;
    len_exp << "try:" << std::endl;
    len_exp << "  a = len(" << m_name << "." << pname << ")" << std::endl;
    len_exp << "except:" << std::endl;
    len_exp << "  a = -1" << std::endl;
    TPython::Exec(len_exp.str().c_str());
    len_exp.str("");
    len_exp << "a";
    int len = TPython::Eval(len_exp.str().c_str());
    if (len < 0) return;
    for (int i = 0; i < len; ++i) {
      std::stringstream item_exp;
      item_exp << m_name << "." << pname << "[" << i << "]";
      std::string item = (const char *) TPython::Eval(item_exp.str().c_str());
      prop.push_back(item);
    }
  }

HLT::FexAlgo::FexAlgo(const std::string &n)
  : m_name(n), m_msg(n) {
  int lvl = (int) MSG::INFO;
  declareProperty("OutputLevel", lvl);
  m_msg.setLevel( (MSG::Level) lvl);
  std::cout << "Output level of " << n << " is: " << (int) lvl << std::endl;
}

HLT::FexAlgo::~FexAlgo() {
}

MsgStream &HLT::FexAlgo::msg() {
  return m_msg;
}

std::ostream &HLT::FexAlgo::msg(MSG::Level lvl) {
  return m_msg << lvl;
}

MSG::Level HLT::FexAlgo::msgLvl() {
  return m_msg.getLevel();
}

void useless() {
  HLT::FexAlgo c("myAlgo");
  float f;
  int i;
  unsigned int ui;
  std::vector<float> vf;
  std::vector<std::string> vs;
  std::vector<int> vi;
  std::string s;
  bool b;
  c.declareProperty("a", f);
  c.declareProperty("a", i);
  c.declareProperty("a", ui);
  c.declareProperty("a", vf);
  c.declareProperty("a", vi);
  c.declareProperty("a", s);
  c.declareProperty("a", b);

  HLT::declareProperty("name", "a", f);
  HLT::declareProperty("name", "a", i);
  HLT::declareProperty("name", "a", ui);
  HLT::declareProperty("name", "a", vf);
  HLT::declareProperty( "name", "a", vi);
  HLT::declareProperty("name", "a", s);
  HLT::declareProperty("name", "a", b);
  HLT::declareProperty("name", "a", vs);

}


std::ostream &operator <<(std::ostream &os, const std::vector<std::string> &v) {
  os << "[";
  for (unsigned int i = 0; i < v.size(); ++i) {
    os << "\"" << v[i] << "\"";
    if (i != v.size() - 1)
      os << ", ";
  }
  os << "]";
  return os;
}

std::ostream &operator <<(std::ostream &os, const std::vector<float> &v) {
  os << "[";
  for (unsigned int i = 0; i < v.size(); ++i) {
    os << "\"" << v[i] << "\"";
    if (i != v.size() - 1)
      os << ", ";
  }
  os << "]";
  return os;
}

