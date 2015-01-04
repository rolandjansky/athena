/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COOLLUMIUTILITIES_COOLLUMIUTILSTEST_H
#define COOLLUMIUTILITIES_COOLLUMIUTILSTEST_H

class MyTestClass {

 public:
  MyTestClass();

  void setValue(int);
  int getValue() const;

 private:
  int m_value;

};

//#include "Python.h"
//#include <boost/python.hpp>
#include <vector>

class CoolLumiUtilsTestObj {

 public:
  CoolLumiUtilsTestObj();

  void setValue(float);
  float getValue() const;

  void append(float);
  //boost::python::list getList();
  const std::vector<float>& getList();

 private:
  float m_value;
  //boost::python::list m_list;
  std::vector<float> m_list;
};

class CoolLumiUtilsVector : public std::vector<double> {

 public:
  CoolLumiUtilsVector();
  void append(double);

};

#endif
