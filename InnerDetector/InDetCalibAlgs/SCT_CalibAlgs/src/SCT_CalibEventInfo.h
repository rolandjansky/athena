/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibEventInfo.h
 * Header file for the SCT_CalibEventInfo class
 * @author Shaun Roe
 **/

#ifndef SCT_CalibEventInfo_h
#define SCT_CalibEventInfo_h

#include "AthenaBaseComps/AthAlgTool.h"

#include "SCT_CalibAlgs/ISCT_CalibEvtInfo.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

class StatusCode;
class ISvcLocator;
class InterfaceID;

class SCT_CalibEventInfo: public extends<AthAlgTool, ISCT_CalibEvtInfo>
{
 public:
  //@name Service methods, reimplemented
  //@{
  SCT_CalibEventInfo(const std::string&, const std::string&, const IInterface*);
  virtual ~SCT_CalibEventInfo() = default;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //@}
  //@name ISCT_CalibEvtinfo interface methods, implemented
  //@{
  virtual void setTimeStamp(const int begin, const int end);
  virtual void setTimeStamp(const std::string& begin, const std::string& end);
  virtual void setTimeStamp(const int ts);
  virtual void getTimeStamps(int& begin, int& end) const;
  virtual void getTimeStamps(std::string& begin, std::string& end) const;
  virtual int timeStamp() const;
  virtual int duration() const;

  virtual void setSource(const std::string source);

  virtual void setLumiBlock(const int begin, const int end);
  virtual void setLumiBlock(const int lb);
  virtual void getLumiBlock(int& begin, int& end) const;
  virtual int lumiBlock() const;
  virtual int numLumiBlocks() const;

  virtual void setRunNumber(const int rn);
  virtual int runNumber() const;

  virtual void setCounter(const int counterVal);
  virtual void incrementCounter();
  virtual int counter() const;

  virtual void setBunchCrossing(const int bc);
  //@}

 private:

  int  m_timeStampBegin;
  std::string m_tsBeginString;
  std::string m_tsEndString;
  int  m_timeStampEnd;
  int  m_duration;
  int  m_LBBegin;
  int  m_LBEnd;
  int  m_numLB;
  std::string m_source;

  //
  int  m_runNumber;
  int  m_lumiBlock;
  int  m_timeStamp;
  int  m_bunchCrossing;
  int  m_counter;
  std::string toUtc(const int timestamp) const;
};

#endif
