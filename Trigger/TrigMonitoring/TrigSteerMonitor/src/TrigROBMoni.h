/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERMONITOR_TRIGROBMONI_H
#define TRIGSTEERMONITOR_TRIGROBMONI_H

/**
 * @file   TrigROBMoni.h
 * @brief  ROB data access monitoring
 * @author Frank Winklmeier
 *
 * $Id: TrigROBMoni.h 761301 2016-07-12 11:51:07Z fwinkl $
 */

// STL includes
#include <string>
#include <vector>
#include <map>

// Trigger includes
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

// Framework includes
#include "GaudiKernel/ServiceHandle.h"

// Forward declararations
namespace HLT {
  class TrigSteer;
}

class TH1;
class TH2I;
class TH2F;
class TProfile;
class TProfile2D;
class TrigMonGroup;
class Algorithm;
class TrigROBMoni;

/// Typedefs
namespace {
  typedef std::map<const uint16_t, std::string> SubDetNamesMap;
  typedef std::map<const uint16_t, std::string> SubDetGroupNamesMap;
}


/**
 * Set of histograms
 *
 * Helper class to create different views of the same variable. For example,
 * plot a give variable per algorithm, subdetector, etc.
 */
template <class HTYPE>
struct HistSet {
  /**
   * Constructor
   *
   * @param montool    Tool instantiating this class
   * @param name       Histogram name   
   * @param axisLabel  Title used for axis label
   */
  HistSet(TrigROBMoni* montool, const std::string& name, const std::string& axisLabel = "") :
    montool(montool), name(name), axisLabel(axisLabel) {
    for (int n=0; n<NHISTS; ++n) hist[n] = 0;
  }


  /**
   * Create and book histograms of type HTYPE
   *
   * @param monGroup   monitoring group
   * @param yLabels    option labels for y-axis
   * @param regHist    true if histogram should be registered with THistSvc
   */
  StatusCode book(TrigMonitorToolBase::TrigMonGroup& monGroup,
                  const std::vector<std::string>* yLabels = 0,
                  bool regHist = true);

  /**
   * Fill histograms
   *
   * @param value      value to fill into histograms
   * @param rob        ROBDataStruct to extract subdetectors
   * @param requester  algorithm name
   */
  StatusCode fill(double value, const robmonitor::ROBDataStruct& rob,
                  const std::string& requester);

  /**
   * Call Reset on all histograms
   */
  void reset() {
    for (int n=0; n<NHISTS; ++n) if (hist[n]) hist[n]->Reset();
  }

  /**
   * Add all histograms of HistSet
   */
  void add(const HistSet& hs) {
    for (int n=0; n<NHISTS; ++n) if (hist[n]) hist[n]->Add(hs.hist[n]);
  }
  
  /**
   * Histogram types
   */
  enum Type {
    ALGO = 0,     ///< histogram of variable vs. algorithm
    SUBDET,       ///< histogram of variable vs. sub-detector
    SDGROUP,      ///< histogram of variable vs. sub-detector group
    NHISTS        ///< number of histograms
  };
    
  TH1* hist[NHISTS];        ///< List of all histograms

  TrigROBMoni* montool;     ///< the tool I am working for
  std::string  name;        ///< name used for histograms
  std::string  axisLabel;   ///< axis title for variable
};



/**
 * Monitoring tool for ROB data access
 *
 * It uses the information published by the Lvl2ROBDataProviderSvc in
 * StoreGate for each call to addROBData (see robmonitor::ROBDataMonitorStruct).
 */
class TrigROBMoni : public TrigMonitorToolBase {
  
  template <class HTYPE> friend struct HistSet;
  
public:
  TrigROBMoni(const std::string& type, const std::string& name,
              const IInterface* parent);
  virtual ~TrigROBMoni();

  virtual StatusCode initialize();
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();
  //  virtual StatusCode finalHists();

  
private:
  
  /// Pointer to steering (partent algo of this tool)
  const HLT::TrigSteer* m_steering;

  /// Steering FEX algos (the ones that can request data)
  std::vector<Algorithm*> m_fexAlgos;

  /// Map to assign ROB history codes to bins
  std::map<robmonitor::ROBHistory, short> m_ROBHistoryToBin;
  
  /// List of ROB history codes as strings
  std::vector<std::string> m_ROBHistoryLabels;

  /* Histogram sets */
  HistSet<TProfile> m_hs_request_time;
  HistSet<TProfile> m_hs_request_size;
  HistSet<TProfile> m_hs_cached_fraction;
  HistSet<TH2I> m_hs_history_total;
  HistSet<TH2I> m_hs_history_event;

  TH2F* m_h_shared_requests;
  
  /* Properties */
  bool m_detailedMonitoring;
  StringProperty m_ROBDataMonitorCollection_SG_Name;

private:
  
  /// All subdetectors (PIXEL_BARREL, ...)
  static SubDetNamesMap      m_subDetNames;
  
  /// All subdetectors groups (PIXEL, ...)
  static SubDetGroupNamesMap m_subDetGroupNames;
};




#endif


