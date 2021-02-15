/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include <limits>
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
      virtual void setEventNumber(const int en);
      virtual int runNumber() const;
      virtual int eventNumber() const;

      virtual void setCounter(const int counterVal);
      virtual void incrementCounter();
      virtual int counter() const;

      virtual void setBunchCrossing(const int bc);
      //@}

   private:

      std::string m_tsBeginString{""};
      std::string m_tsEndString{""};
      int  m_timeStampBegin{std::numeric_limits<int>::max()};
      int  m_timeStampEnd{std::numeric_limits<int>::min()};
      int  m_duration{0};
      int  m_LBBegin{std::numeric_limits<int>::max()};
      int  m_LBEnd{std::numeric_limits<int>::min()};
      int  m_numLB{0};
      std::string m_source{"UNKNOWN"};

      //
      int  m_runNumber{0};
      int  m_eventNumber{0};
      int  m_lumiBlock{0};
      int  m_timeStamp{0};
      int  m_bunchCrossing{0};
      int  m_counter{0};
      std::string toUtc(const int timestamp) const;
};

#endif
