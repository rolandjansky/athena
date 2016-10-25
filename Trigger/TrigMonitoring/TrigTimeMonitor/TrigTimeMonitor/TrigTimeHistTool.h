// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGTIMEHISTTOOL_H
#define TRIGTIMEHISTTOOL_H

#include <vector>
#include <map>
#include <string>

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include "boost/regex.hpp"
#include "TProfile.h"

class TH1F;
class TH2F;
class TrigTimer; 


/**
 * \brief Tool to be used to save time intervals defined in algorithms 
 *        in the histograms form.
 *
 * Actually 2 histograms can be created per timer.
 * One which histograms timer->elapsed() and another 
 * with timer->elapsed()/timer->propValue() if propName != "" and value != 0
 *
 * One may use it to have 2 histograms with selection.
 * Namely if property would be 1 or 0 then only entries with property 1 will 
 * be found in the histogram. (Thing like this: 1 for accepted 0 for rejected.
 * Then in the timer histogram you have all events and in the perObjTimer 
 * histogram rejected. Do you need more logic? Book the histogram from i.e. 
 * -100 to 100. Give -1 propValue for accepted 1 rejected and 0 some other.)
 *
 *
 * To use it one has to attach this tool to HLT algorithm as one of AthenaMonTools
 * 
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>
 **/


class TrigTimeHistTool: public TrigMonitorToolBase {

public:
  TrigTimeHistTool(const std::string & type, const std::string & name,
		   const IInterface* parent);

  virtual ~TrigTimeHistTool();

  StatusCode initialize();
  StatusCode bookHists();
  StatusCode fillHists();
  StatusCode finalHists();
    
private:
  bool selected(const std::string& name, const boost::regex& regex);    //!< true if key entry selected by the timer (all is empty)
  struct TimerHist {                           //! internal structure to keep associations
    TH1F *m_timerHist{0};                         //!< between the timing histogram
    TH1F *m_timerPerObjHist{0};                   //!< the timing per object histogram
    TrigTimer *m_timer{0};                        //!< and the timer 
  };
  std::vector< struct TimerHist* > m_entries;  //!< collection of associations
  struct TimerGroupHist {
    TH2F *m_timerHist{0};
    TrigLBNHist<TProfile> *m_timerProf{0};        //!< per-LB profile histogram if requested
    std::vector<TrigTimer *> m_timers;
  };
  std::map<const std::string, TimerGroupHist *> m_groupHists; //!< mapping for group histograms

  // configuration
  std::string m_key;                           //!< list of requested timers 
  std::string m_path;                          //!< booking path of histograms
  int m_bins;                                  //!< number of bins (for all histograms)
  std::vector<double> m_timerHistLimits;       //!< timing histogram limits
  bool m_doPerObjHist;                         //!< flag - if true do per obj histogram
  bool m_doPerLBGroups;                        //!< enable per LB group histograms
  std::vector<double> m_timerPerObjHistLimits; //!< limits for per object timing histogram
  std::string m_parentName;                    //!< name of parent algorithm (used to find which timers to record)
  std::map<std::string, std::string> m_regexGroups; //!< map of regular expressions to grouped histograms
  std::string m_includeName;                   //!< to store regex for positive filtering of the timers
  std::string m_excludeName;                   //!< to store regex for negative filtering of the timers
};


#endif 
