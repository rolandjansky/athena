/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AraToolUsingTool_H
#define AraToolUsingTool_H

#include "GaudiKernel/ToolHandle.h"

#include "AraTool/AraToolBase.h"
#include "AraToolExample/AnExampleAraToolWrapper.h"

#include "AraToolExample/AnExampleAraTool.h"

class AraToolUsingTool : virtual public AraToolBase
{
 public:
  AraToolUsingTool(PropertyMgr *pmgr=0);

  virtual ~AraToolUsingTool() {}
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  StatusCode alsoPerformTask(float &inp, float &out);
  
 public: //Setters and getters
  float getAlsoProperty1() {return m_alsoProperty1;}
  void setAlsoProperty1(float p1) {m_alsoProperty1=p1;}

 protected:
  float m_alsoProperty1;
  //Athena
  ToolHandle< AnExampleAraToolWrapper > m_alsoTool;
  //ARA
  AnExampleAraTool *m_alsoToolAra;
};

#endif
