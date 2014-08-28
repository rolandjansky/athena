/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AnExampleAraTool_H
#define AnExampleAraTool_H

#include "AraTool/AraToolBase.h"

class AnExampleAraTool : virtual public AraToolBase
{
 public:
  AnExampleAraTool(PropertyMgr *pmgr=0);

  virtual ~AnExampleAraTool() {}
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  StatusCode performTask(float &inp, float &out);
  
 public: //Setters and getters
  float getProperty1() {return m_property1;}
  void setProperty1(float p1) {m_property1=p1;}

  float getProperty2() {return m_property2;}
  void setProperty2(float p2) {m_property2=p2;}

 protected:
  float m_property1;
  float m_property2;
};

#endif
