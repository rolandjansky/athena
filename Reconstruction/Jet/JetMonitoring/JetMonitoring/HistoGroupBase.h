//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_HISTOGROUPBASE_H
#define JETMONITORING_HISTOGROUPBASE_H
///////////////////////////////////////////
///
/// \class HistoGroupBase
/// A base class for dual-use tools managing and filling a list of histograms
/// 
/// Inpired by TrkValHistUtils/PlotBase.h
/// 
////////////////////////////////////////////

#include <vector>
#include <string>
#include "AsgTools/AsgTool.h"

class TH1;

// temporary, we should define a proper interface
static const InterfaceID IID_HistoGroupBase("HistoGroupBase", 1, 0);

class HistoGroupBase :  public asg::AsgTool {  

  ASG_TOOL_CLASS(HistoGroupBase, asg::IAsgTool)

  public:

  //--------------------------------------------
  /// Redefinition of fill intervals as in ManagedMonitorToolBase
  // (TO DO : typedef to ManagedMonitorToolBase::Interval_t
  // when compiled in Athena, redefine when compiling standalone ? )
  enum Interval_t { file = 0, eventsBlock, lumiBlock,
                    lowStat, medStat, higStat,
                    run, fill, all,
                    
                    useToolInterval // used to use the value from the tool's m_interval property  
  };


  /// Simply a histo and its path in the hiearchy 
  struct HistData {
    TH1* hist;
    std::string path; 
    Interval_t interval;
  };

  HistoGroupBase(const std::string & t);
  ~HistoGroupBase(){}

  virtual StatusCode initialize() ;
  
  virtual int buildHistos();
  
  virtual int fillHistos();

  virtual int finalizeHistos();


  /// allows a higher level tool to put the histos in this group as a sub-dir of 'preDir' 
  virtual void prefixHistoDir(const std::string & preDir);
  
  /// register the histo h in this group (if h!=NULL). The histo name is changed if m_prefixedHistoName==true. Returns h.
  template<class T>
  T* bookHisto(T* h, Interval_t ityp= useToolInterval) { renameAndRegister(h,"",ityp); return h;}
  
  /// Same as bookHistom but put the histo under subdirectory subD.
  template<class T>
  T* bookHistoInSubDir(const std::string& subD, T* h, Interval_t ityp= useToolInterval) { renameAndRegister(h, subD,ityp); return h;}

  /// return all the histos in this group
  const std::vector<HistData> & bookedHistograms();


  virtual void setInterval(Interval_t ityp, bool force = false){ if(force || (m_interval==-1) ) m_interval = ityp;}

  static const InterfaceID& interfaceID( ){return IID_HistoGroupBase;}

protected:
  
  /// Rename an histo according to the tool's path
  virtual void renameAndRegister(TH1* h, const std::string & subD="", Interval_t ityp= useToolInterval);


  /// The list of histos in this group.
  std::vector<HistData> m_vBookedHistograms;

  /// The path where histos in this group leave
  std::string m_histoDir;

  /// when used in Athena monitoring context, the histos of this group will have this type of interval
  int m_monIntervalType; 

  /// true if the histo name should contain the full hierarchy path
  bool m_prefixedHistoName; 

  int m_interval;

};
#endif
