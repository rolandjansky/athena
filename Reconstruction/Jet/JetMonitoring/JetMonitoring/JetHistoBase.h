//  -*- c++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETHISTOGROUP_H
#define JETMONITORING_JETHISTOGROUP_H
///////////////////////////////////////////
///
/// \class JetHistoBase
/// A base class for tool building a group of histogram. Specialization 
/// 
/// This class defines 2 filling call dedicated to jets : 
///   - fillHistosFromContainer(JetContainer&) 
///   - fillHistosFromJet(Jet&)
/// The default implementation of the first is  loops over the container and calls fillHistosFromJet()
/// on each jet.
////////////////////////////////////////////

#include "xAODJet/JetContainer.h"
#include "JetMonitoring/HistoGroupBase.h"

// temporary, we should define a proper interface
static const InterfaceID IID_JetHistoBase("JetHistoBase", 1, 0);

class JetHistoBase :  public HistoGroupBase {

  ASG_TOOL_CLASS0(JetHistoBase)

  public:
  
  JetHistoBase(const std::string & t);
  ~JetHistoBase(){}

  virtual StatusCode initialize() ;

  
  virtual int fillHistosFromContainer(const xAOD::JetContainer & cont, float weight);
  virtual int fillHistosFromJet(const xAOD::Jet &j, float /*weight*/);

  virtual int finalizeHistos();


  static const InterfaceID& interfaceID( ){return IID_JetHistoBase;}

protected:
 

};

#endif
