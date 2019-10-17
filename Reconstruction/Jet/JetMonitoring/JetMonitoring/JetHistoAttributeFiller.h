// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETATTHISTOFILLER_H
#define JETATTHISTOFILLER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/IJetHistoFiller.h"
#include "JetMonitoring/JetHistoVarTool.h"

////////////////////////////////////////////////////
/// \class JetHistoAttributeFiller
///
/// A IJetHistoFiller implementation in charge of filling 1 1D or 2D histogram.
/// The histogram is filled according to 1 (or 2 if 2D histo, or 3 if TProfile2D)
// "jet variables" for each jet in the JetContainer passed to the processJetContainer()
/// function.
/// 
/// The tool does not directly fill histograms but uses Monitored::Scalar and Monitored::Group
/// as is requested by the monitoring framewok.
///
/// The variables are specified by means of IJetHistoVarTool which are just configurable version
/// of the JetVariable class.
/// The IJetHistoVarTool are set via the properterties VarX, VarY and VarZ 
/// The Monitoring Group is specified by the Group property
///
/// The tool support plotting variables of type float,int ,vector<float> and vector<int>.
///
class JetHistoAttributeFiller : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoAttributeFiller( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoAttributeFiller(){}

  virtual StatusCode initialize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  

  
private:


  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  ToolHandle<IJetHistoVarTool> m_varX;
  ToolHandle<IJetHistoVarTool> m_varY;

  ToolHandle<IJetHistoVarTool> m_varZ;
  
  int m_nVar=1;
  
};
#endif
