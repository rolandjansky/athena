/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_TRIGOPMONI_H
#define TRIGSTEERMONITOR_TRIGOPMONI_H

/**
 * @file   TrigOpMoni.h
 * @brief  operation monitoring tool
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 * @author Frank Winklmeier
 */

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "GaudiKernel/IIncidentListener.h"
#include "EventInfo/EventID.h"  /* number_type */
#include "AthenaKernel/IOVRange.h"
#include <map>
#include <string>

class TH1I;
class TH2I;
class TH1F;
namespace MagField {
  class IMagFieldSvc;
}
class IIOVDbSvc;
class StoreGateSvc;
class IJobOptionsSvc;
class EventInfo;

class TrigOpMoni : public TrigMonitorToolBase, public IIncidentListener
{  
public:

  TrigOpMoni(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~TrigOpMoni();
  
  virtual StatusCode initialize();
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();
  
  virtual void handle(const Incident &IncidInfo);

private:
  
  void FillMagFieldHist();
  void FillIOVDbHist();
  void FillIOVDbChangeHist();
  void FillSubDetHist();
  void FillReleaseData();
  void FillLbDiffHist();

  template<class HTYPE>
  bool regHist(HTYPE*& hist, bool verbose=true);
  
  std::string folder2HistName(const std::string& folder);

  
  /// Read 'ReleaseData' metadata file
  StatusCode readReleaseData(const std::string& file, std::map<std::string,std::string>& result);

  /// Histograms for COOL folders
  struct FolderHist {
    FolderHist() : h_time(0), h_bytes(0), total_time(0), total_bytes(0) {}
    TH1F* h_time;
    TH1F* h_bytes;
    float total_time;
    float total_bytes;
  };
    
  bool  m_MagFieldHistFilled;
  bool  m_IOVDbHistFilled;
  bool  m_SubDetHistFilled;

  MsgStream m_log;

  ServiceHandle<StoreGateSvc>         m_StoreGateSvc;
  ServiceHandle<IJobOptionsSvc>       m_JobOptionsSvc;
  MagField::IMagFieldSvc*             m_MagFieldSvc;  
  IIOVDbSvc*                          m_IOVDbSvc;

  TrigMonGroup m_monGroup;
  
  TH2I*       m_MagFieldHist;
  TH2I*       m_iovChangeHist;  
  TH1I*       m_generalHist;
  TH1I*       m_lbDiffHist;

  std::string      m_releaseData;
  bool             m_detailedHists;
  EventID::number_type    m_previousLB;       //!< LB of previous event
  const EventInfo*        m_pEvent;           //!< current event
  std::map<std::string, IOVRange>   m_currentIOVs;      //!< current IOVs managed by IOVDbSvc
  std::map<std::string, FolderHist> m_folderHist;       //!< histograms for COOL folders
};

#endif // TRIGSTEERMONITOR_TRIGOPMONI_H


