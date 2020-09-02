/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONITORTOOLBASE_H
#define MONITORTOOLBASE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "IMonitorToolBase.h"

#include <string>
#include <vector>
#include <map>

class MonitorToolBase : public AthAlgTool,
			virtual public IMonitorToolBase
                         
{

 public:
  typedef std::map<std::string,std::string>   map_type;
  
 public:

  MonitorToolBase(const std::string & type, const std::string & name,
		   const IInterface* parent); 
  
  // setup output streams
  StatusCode setupOutputStreams(std::vector<std::string> Mapping
				= std::vector<std::string>()     );
  
  //default destructor
  virtual ~MonitorToolBase();
  
  StatusCode initialize()
    { m_counter=m_preScale; return StatusCode::SUCCESS; };

  //default finalHists() method
  virtual StatusCode finalHists();

  //default checkHists() method, called periodically (argument false)
  //  or after finalHists (argument true)
  virtual StatusCode checkHists(bool fromFinalize);

  //default runStat() method - empty
  virtual StatusCode runStat();
  
  virtual bool histsNotBooked();
  
  virtual bool preSelector();

  // Simple method for accessing histo service for booking
  IHistogramSvc* ToolHistoSvc();
  
  // Simple method for accessing root histo service for booking
  ITHistSvc*     ToolRootHistSvc();
  
  // Functions for use in dynamic histogram booking
  void SetBookStatus(bool status);

  virtual StatusCode convertLWHists() { return StatusCode::SUCCESS; } //Reimplement if using LWHists
  
 protected:
  
  std::string getStreamName(unsigned int number, bool useDefault=true);
  
  std::string getStreamName(std::string stream, bool useDefault=true);
  
 protected:
  
  IHistogramSvc* m_histsvc;
  ITHistSvc*     m_rootsvc;
  bool m_isBooked;
  std::string m_path;
  
  // default stream name
  std::string m_THistSvc_streamname;
  
  // multiple StreamNames
  std::vector<std::string>            m_THistSvc_streamnameMapping;
  std::map<std::string,std::string>   m_map_THistSvc_streamnames;
  bool m_setupStreamMap;
  
  std::string m_THistSvc_refStream;
  
  std::string m_LogFileName;
  std::string m_FormatString;

  unsigned long m_preScale;
  unsigned long m_counter;
  
};

#endif
