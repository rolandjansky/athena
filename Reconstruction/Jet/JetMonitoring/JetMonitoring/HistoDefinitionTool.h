//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_HISTODEFINITIONTOOL_H
#define JETMONITORING_HISTODEFINITIONTOOL_H
///////////////////////////////////////////
///
/// \class HistoDefinitionTool
/// This tool does no particular tasks. It's intent is only to be able to communicate
///  histo information (title, labels, binning) from jobOption to C++
/// 
/// Mainly used to allow to define histo binning in jobOption for jet attributes plotting.
///  See usage in JetAttributeHistos
////////////////////////////////////////////

#include <vector>
#include <string>
#include "AsgTools/AsgTool.h"

class TH1F;
class TH2F;
class TProfile;
class TProfile2D;

// temporary, we should define a proper interface
static const InterfaceID IID_HistoDefinitionTool("HistoDefinitionTool", 1, 0);

class HistoDefinitionTool :  public asg::AsgTool {  

  ASG_TOOL_CLASS(HistoDefinitionTool, asg::IAsgTool)

  public:


  HistoDefinitionTool(const std::string & t);
  ~HistoDefinitionTool(){}


  virtual StatusCode initialize() ;

  std::string attributeName(){return m_attname;}


  /// Build histos according to properties
  TH1F* buildTH1F();
  TH2F* buildTH2F();
  TProfile* buildTProfile(bool useYLimits=false);

  // for convenience a templated method doing the same as above:
  template<typename T> 
  T* build(){return NULL;}


  static const InterfaceID& interfaceID( ){return IID_HistoDefinitionTool;}
  
protected:

  std::string m_attname;
  std::string m_htitle;

  int m_nbinsx = 10;
  float m_xlow = 0.0;
  float m_xup = 1.0;

  int m_nbinsy = 10;
  float m_ylow = 0.0;
  float m_yup = 1.0;
  
};

template<> TH1F* HistoDefinitionTool::build<TH1F>();
template<> TH2F* HistoDefinitionTool::build<TH2F>();
template<> TProfile* HistoDefinitionTool::build<TProfile>();


#endif
