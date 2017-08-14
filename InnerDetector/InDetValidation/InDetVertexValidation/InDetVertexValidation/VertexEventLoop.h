/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VertexAnalysis_VertexEventLoop_H
#define VertexAnalysis_VertexEventLoop_H


#ifdef ROOTCORE
#include <EventLoop/Algorithm.h>

//xAOD
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

//#endif

//ROOT
#include <TH1.h>

//vertex truth matching tool
class InDetVertexTruthMatchTool;

//helper to handle histo creation and filling in interface agnostic way
class VertexAnalysis;

class VertexEventLoop : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
 public:
  // float cutValue;



  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
 public:
  // Tree *myTree; //!
  // TH1 *myHist; //!

  xAOD::TEvent *m_event; //!
  int m_entryNumber; //!

#ifndef __CINT__
  InDetVertexTruthMatchTool * m_matchTool; //!
  VertexAnalysis * m_analyzer; //!
#endif
  
  // this is a standard constructor
  VertexEventLoop ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(VertexEventLoop, 1);


};

#endif
#endif
