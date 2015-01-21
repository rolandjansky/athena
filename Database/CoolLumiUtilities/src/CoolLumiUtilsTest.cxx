/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/CoolLumiUtilsTest.h"


MyTestClass::MyTestClass() {
}

void
MyTestClass::setValue(int value) {
  m_value = value;
}

int
MyTestClass::getValue() const {
  return m_value;
}

CoolLumiUtilsTestObj::CoolLumiUtilsTestObj() {
  m_value = 0;
}

void 
CoolLumiUtilsTestObj::setValue(float value) {
  m_value = value;
}

float
CoolLumiUtilsTestObj::getValue() const {
  return m_value;
}

void 
CoolLumiUtilsTestObj::append(float value) {
  m_list.push_back(value);
}

const std::vector<float>&
CoolLumiUtilsTestObj::getList() {
  return m_list;
}

CoolLumiUtilsVector::CoolLumiUtilsVector()
  : std::vector<double>(1000, 0.) {
}

void
CoolLumiUtilsVector::append(double value) {
  this->push_back(value);
}

// boost::python::list
// CoolLumiUtilsTestObj::getList() {
//   return m_list;
// }
