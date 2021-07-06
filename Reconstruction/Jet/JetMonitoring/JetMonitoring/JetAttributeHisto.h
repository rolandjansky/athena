//  -*- c++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETATTRIBUTEHISTOS_H
#define JETMONITORING_JETATTRIBUTEHISTOS_H
/////////////////////////////////////////////////
/// \class JetAttributeHisto
/// 
/// \brief A histo building tool (JetHistoBase) using attributes to fill histograms.
///
/// This tool fills one  histogram with using  jet attribute(s).
/// The histogram definition (type, binning, titles) is passed through a 
/// HistoDefinitionTool. 
/// TH1F, TH2F and TProfile are supported.
/// Properties :
///  - AttributeNames (vector<string>) : names of attributes to plot (1 entry->TH1F, 2 entries->TH2F or TProfile)
///  - AttributeTypes (vector<string>) : types of attributes to plot (same lenght as AttributeNames)
///  - AttributeNames (vector<bool>)

#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

namespace jet {
  class HistoFiller;
}
class JetAttributeHisto : public JetHistoBase {
  ASG_TOOL_CLASS0(JetAttributeHisto);
  
public:
  //typedef SG::AuxElement::Accessor< T > accessor_t;
  
  JetAttributeHisto(const std::string &t);
  virtual ~JetAttributeHisto();

  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromJet(const xAOD::Jet &j, float weight);
  virtual int fillHistosFromContainer(const xAOD::JetContainer & cont, float weight);
  
protected:
  /// Rename an histo according to the tool's path. redefine here to take m_selectedIndex into account.
  virtual void renameAndRegister(TH1* h, const std::string & subD="", Interval_t i=all);

  ToolHandle<HistoDefinitionTool> m_histoDef;
  std::vector<std::string> m_attTypes;
  std::vector<std::string> m_attNames;
  std::vector<bool> m_attGeV;

  int m_selectedIndex;
  bool m_doTProfile;
  jet::HistoFiller* m_histoFiller;
};



#endif
