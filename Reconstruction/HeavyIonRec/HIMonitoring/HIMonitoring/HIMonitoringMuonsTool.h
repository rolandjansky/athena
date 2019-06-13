/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HIMONITORINGMUONSTOOL_H
#define HIMONITORINGMUONSTOOL_H


#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1D;

class HIMonitoringMuonsTool: public ManagedMonitorToolBase
{
public:
  HIMonitoringMuonsTool(const std::string& type, const std::string& name,
                        const IInterface* parent);

  virtual ~HIMonitoringMuonsTool();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


  void book_hist();
private:
  /// histograms

  TH1D* m_h_Auth_fcal {};
  TH1D* m_h_Type_fcal {};
  TH1D* m_h_Chi2ndf_fcal {};

  TH1D* m_h_Chi2ndfID_fcal {};
  TH1D* m_h_Chi2ndfME_fcal {};
  TH1D* m_h_Chi2ndfMS_fcal {};
};

#endif
