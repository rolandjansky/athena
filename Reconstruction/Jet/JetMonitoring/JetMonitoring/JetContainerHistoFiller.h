//  -*- c++ -*- 

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETCONTAINERHISTOFILLER_H
#define JETMONITORING_JETCONTAINERHISTOFILLER_H
///////////////////////////////////////////
///
/// \class JetContainerHistoFiller
///  Fill a list of histos on a JetContainer.
///
/// 
////////////////////////////////////////////


#include "GaudiKernel/ToolHandle.h"
#include "AsgTools/AsgTool.h"

#include "JetMonitoring/JetHistoBase.h"
#include "xAODEventInfo/EventInfo.h"


// temporary, we should define a proper interface
static const InterfaceID IID_JetContainerHistoFiller("JetContainerHistoFiller", 1, 0);

class JetContainerHistoFiller : public HistoGroupBase {
  ASG_TOOL_CLASS0(JetContainerHistoFiller)
  

  
public:
  
  JetContainerHistoFiller(const std::string &t);
  virtual ~JetContainerHistoFiller(){}

  virtual StatusCode initialize() ;

  virtual int buildHistos();

  virtual int fillHistos();

  virtual int finalizeHistos();

  /// redefine from base class to forward prefix path to sub histos
  virtual void prefixHistoDir(const std::string & preDir);


  static const InterfaceID& interfaceID( ){return IID_JetContainerHistoFiller;}

  virtual void setInterval(Interval_t ityp, bool force = false);


protected:


  /// The list of histogramming tools
  ToolHandleArray<JetHistoBase> m_histoTools;
  /// the jet container to build histos from
  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerName{this, "JetContainer", ""};
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};

};
#endif
