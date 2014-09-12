/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALGO_H
#define ALGO_H

#include "TPython.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "MsgStream.h"

#ifndef ALGO_CXX
extern const float mm;
extern const float GeV;
extern const float TeV;
#endif

namespace HLT {
  enum ErrorCode {
    OK = 0,
    BAD_JOB_SETUP,
    ERROR
  };

  template <typename T>
  void declareProperty(const std::string &m_name, const std::string &pname, T &prop);
  template <>
  void declareProperty<bool>(const std::string &m_name, const std::string &pname, bool &prop);
  template <>
  void declareProperty<std::string>(const std::string &m_name, const std::string &pname, std::string &prop);
  template <typename T>
  void declareProperty(const std::string &m_name, const std::string &pname, std::vector<T> &prop);
  template <>
  void declareProperty<std::string>(const std::string &m_name, const std::string &pname, std::vector<std::string> &prop);


  class FexAlgo {
    public:


    FexAlgo(const std::string &name);
    virtual ~FexAlgo();

    template <typename T>
    void declareProperty(const std::string &pname, T &prop) {
      HLT::declareProperty(m_name, pname, prop);
    }

    template <typename T>
    void declareProperty(const std::string &pname, std::vector<T> &prop) {
      HLT::declareProperty(m_name, pname, prop);
    }

    const std::string &name() const;

    protected:
    MsgStream &msg();
    std::ostream &msg(MSG::Level lvl);
    MSG::Level msgLvl();

    private:

    std::string m_name;
    MsgStream m_msg;

  };

}

std::ostream &operator <<(std::ostream &, const std::vector<std::string> &);
std::ostream &operator <<(std::ostream &, const std::vector<float> &);

#endif

