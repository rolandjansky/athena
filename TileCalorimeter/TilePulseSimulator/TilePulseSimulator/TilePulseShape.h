/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TILEPULSESHAPE__HPP
#define __TILEPULSESHAPE__HPP

#include "TObject.h"
#include "TString.h"
#include "TF1.h"
#include <vector>


class TGraph;
class TSpline;
class MsgStream;
class IMessageSvc;

class TilePulseShape : public TObject{
 public:
  TilePulseShape(IMessageSvc* msgSvc, std::string name);
  TilePulseShape(IMessageSvc* msgSvc, std::string name, TString fileName);
  TilePulseShape(IMessageSvc* msgSvc, std::string name, std::vector<double> shapevec);
  virtual ~TilePulseShape();
  void loadPulseShape(TString fileName);
  void setPulseShape(std::vector<double> shapevec);
  
  //=== access to the underlying graph
  TGraph* getGraph(double t0=0., double ped=0., double amp=1.);
  double  eval(double x, bool useSpline=true);

  //=== modify the pulseshape
  void resetDeformation();
  int scalePulse(double leftSF=1., double rightSF=1.);
  
 private:
  TGraph* _pulseShape;
  TGraph* _deformedShape;
  TSpline* _deformedSpline;
  
  MsgStream* m_log;
};

#endif
