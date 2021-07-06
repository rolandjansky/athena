//  -*- c++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_HISTOFORJETSELECTION_H
#define JETMONITORING_HISTOFORJETSELECTION_H
///////////////////////////////////////////
///
/// \class HistosForJetSelection
///  Fills a list of histograms on a selected subset of a JetContainer.
///  
///  The tool has 2 sort of properties
///   - a JetSelector Tool (actually, to be implemented) or an option to use a built-in selection (none, or (sub)leading jet)
///   - a list of JetHistoBase (ToolHandleArray)
/// The tool uses the selector on the JetContainer it is passed. It then calls each of its JetHistoBase on the selected jets.
////////////////////////////////////////////


#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"
#include "JetInterface/IJetSelector.h"

class HistosForJetSelection : public JetHistoBase {
  ASG_TOOL_CLASS0(HistosForJetSelection);
  
public:
  
  HistosForJetSelection(const std::string &t);
  virtual ~HistosForJetSelection();
  
  virtual StatusCode initialize() ;
  virtual int buildHistos();

  virtual int fillHistosFromContainer(const xAOD::JetContainer & cont, float weight);

  virtual int finalizeHistos();

  /// redefine to forward to sub histos
  virtual void prefixHistoDir(const std::string & preDir);

  /// redefine to forward to sub histos
  virtual void setInterval(Interval_t ityp, bool force = false);

  bool m_inverseToolSelection = false;

  /// Describes the selection type : built-in or use of an external type.
  enum SelectionType {
    AllJets,
    LeadingJet,
    SubLeadingJet,
    FromTool
  };

protected:

  virtual void modifyNameAndTitle(TH1* h);
  

  /// List of histo tools to call on selected jets
  ToolHandleArray<JetHistoBase> m_histoTools;

  ToolHandle<IJetSelector> m_selTool; /// used only if m_selType == FromTool

  /// Flag for the selection 
  int m_selType;

  /// Suffix of the histos titles. If the property is not set and using a built-in type, it will be automatically chosen.
  std::string m_titleSuffix;
  /// Suffix of the histos names 
  std::string m_nameSuffix;
};
#endif
