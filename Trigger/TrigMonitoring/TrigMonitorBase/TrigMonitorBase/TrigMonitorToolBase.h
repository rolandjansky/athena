// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONITORTOOLBASE_TRIGMONITORTOOLBASE_H
#define TRIGMONITORTOOLBASE_TRIGMONITORTOOLBASE_H

#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

/**
 * @brief The base class for HLT Monitoring tools.
 *
 * The implementation is reviewed and we skip the step related to ManagedMonitrToolBase
 * The MonGroup functionality is replaced by TrigMonGroup 
 *
 * It inherits from ManagedMonitorToolBase where booking base functions are defined.
 * user inheriting from this tool should always call initialize() of this class like: TrigMonitorToolBase::initialize().
 * 
 */


class ITrigLBNHist;
class TH1;



class TrigMonitorToolBase : public AthAlgTool, virtual public IMonitorToolBase {
 public:

  TrigMonitorToolBase(const std::string & type, 
		      const std::string & name,
		      const IInterface* parent); 
  
  virtual ~TrigMonitorToolBase();

  virtual StatusCode initialize();

  virtual StatusCode setupOutputStreams(std::vector<std::string> Mapping);
  
  
  // book & fill (pure virtual)
  virtual StatusCode bookHists() = 0;
  virtual StatusCode fillHists() = 0;
  virtual StatusCode finalHists()     { return StatusCode::SUCCESS; }
  virtual StatusCode runStat()        { return StatusCode::SUCCESS; }
  virtual StatusCode checkHists(bool) { return StatusCode::SUCCESS; }

  /**
   * @brief prescale execution of the tool
   * Intention is that if this method returns true it should be executed. Otherwise not.
   * @warning It depends on the tool user to make propper use of this method.
   *
   * The default implementation (in this base class) is using Scaler property to control presaclling.
   */
  virtual bool       preSelector();
  

  enum Level { debug, expert, shift, express, runsum, runstat = runsum };
  class TrigMonGroup {
  public:
    TrigMonGroup(TrigMonitorToolBase* tool, const std::string& algo, Level l );
    StatusCode regHist(TH1* h);
    StatusCode regHist(ITrigLBNHist* h);
    StatusCode deregHist(TH1* h);
  private:
    TrigMonitorToolBase* m_tool;
    std::string m_algo;
    Level m_level;
  };

  friend class TrigMonGroup;

private:
  std::string level2string( Level l );

  ServiceHandle<ITHistSvc> m_histSvc;

  unsigned int m_prescale; //!< presacling factor
  unsigned int m_execs;    //!< counter for prescaling
  
  unsigned int m_lbnHistoryDepth; //!< Defines after how many LBNs we should drop histogram
  unsigned int m_lbnHistoryGroup; //!< Defines  how many LBNs should be grouped together
  
public:
  virtual StatusCode convertLWHists() { return StatusCode::SUCCESS; } //Reimplement if using LWHists

};


#endif
