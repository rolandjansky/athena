/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONDQAHISTTOOL_H
#define MUON_MUONDQAHISTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

static const InterfaceID IID_MuonDQAHistTool("Muon::MuonDQAHistTool",1,0);


class MsgStream;
class TH1;
class TH2;

namespace Muon {


  /**
     @brief Tool to fill track/segment histograms
     The tool can be used to:
     - extract information from the track into a data structure where it can be used for further analysis.
     The following information can be extracted
     - track/segment parameters
     - hit local positions, residuals and pulls 
     - hit counts e.g. number of mdt hits, number of mdt chambers, etc.
     - fill the extracted information into a set of histograms

  */
  
  class MuonDQAHistTool : public AthAlgTool {
  public:
 
    /** @brief a pair of MeasurementBase + TrackParameters is the minimum input to calculate a residual */
 
    /** @brief constructor */
    MuonDQAHistTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonDQAHistTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonDQAHistTool; }
    TH1F* makeAndRegisterTH1F(ManagedMonitorToolBase::MonGroup& mon, const char* name, std::string title, int nBins, float minX, float maxX);
    TH2F* makeAndRegisterTH2F(ManagedMonitorToolBase::MonGroup& mon, const char* hName, std::string hTitle, int nBinsX, float minX, float maxX, int nBinsY, float minY, float maxY);
  
  private:
    MsgStream* m_log;
  };

}

#endif
