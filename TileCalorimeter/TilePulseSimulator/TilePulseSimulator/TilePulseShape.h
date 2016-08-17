/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEPULSESIMULATOR_TILEPULSESHAPE_H
#define TILEPULSESIMULATOR_TILEPULSESHAPE_H

#include "TObject.h"
#include "TString.h"
#include <vector>

class TF1;
class TGraph;
class TSpline;
class MsgStream;
class IMessageSvc;

class TilePulseShape : public TObject{
 public:
  TilePulseShape(IMessageSvc* msgSvc, const std::string& name);
  TilePulseShape(IMessageSvc* msgSvc, const std::string& name, const TString& fileName);
  TilePulseShape(IMessageSvc* msgSvc, const std::string& name, const std::vector<double>& shapevec);
  virtual ~TilePulseShape();
  void loadPulseShape(const TString& fileName);
  void setPulseShape(const std::vector<double>& shapevec);
  
  //=== access to the underlying graph
  TGraph* getGraph(double t0=0., double ped=0., double amp=1.);
  double  eval(double x, bool useSpline=true);

  //=== modify the pulseshape
  void resetDeformation();
  int scalePulse(double leftSF=1., double rightSF=1.);
  
 private:
  TGraph* m_pulseShape;
  TGraph* m_deformedShape;
  TSpline* m_deformedSpline;
  
  MsgStream* m_log;
};

#endif // TILEPULSESIMULATOR_TILEPULSESHAPE_H
