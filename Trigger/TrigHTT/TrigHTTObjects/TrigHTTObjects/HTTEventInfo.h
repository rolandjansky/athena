/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTEVENTINFO_H
#define HTTEVENTINFO_H

#include <TObject.h>
#include <vector>
#include <iostream>
#include <sstream>


class HTTEventInfo: public  TObject {

 public:
  
 HTTEventInfo() :  m_run_number(0),
    m_event_number(0),
    m_averageInteractionsPerCrossing(0),
    m_actualInteractionsPerCrossing(0),
    m_LB(0),
    m_BCID(0),
    m_extendedLevel1ID(0),
    m_level1TriggerType(0)
      {};

 HTTEventInfo(unsigned long runNumber, int eventNumber, int bcid) :
  m_run_number(runNumber),
    m_event_number(eventNumber),
    m_averageInteractionsPerCrossing(0),
    m_actualInteractionsPerCrossing(0),
    m_LB(0),
    m_BCID(bcid),
    m_extendedLevel1ID(0),
    m_level1TriggerType(0)
      {};

  virtual ~HTTEventInfo();
  void reset();

  //event info 
  //get
  unsigned long runNumber() const { return m_run_number; }
  unsigned long eventNumber() const { return m_event_number; }
  int averageInteractionsPerCrossing() const {return m_averageInteractionsPerCrossing;}
  int actualInteractionsPerCrossing() const {return m_actualInteractionsPerCrossing;}
  int LB() const {return m_LB;}
  int BCID() const {return m_BCID;}
  unsigned int extendedLevel1ID() const {return m_extendedLevel1ID;}
  unsigned int level1TriggerType() const {return m_level1TriggerType;}
  std::vector<unsigned int> level1TriggerInfo() const {return m_level1TriggerInfo;}

  //set
  void setRunNumber(const unsigned long& val) { m_run_number = val; }
  void setEventNumber(const unsigned long& val) { m_event_number = val; }
  void setaverageInteractionsPerCrossing(const int& val) {m_averageInteractionsPerCrossing = val;}
  void setactualInteractionsPerCrossing(const int& val) {m_actualInteractionsPerCrossing = val;}
  void setLB(const int&val) {m_LB = val;}
  void setBCID(const int&val) {m_BCID = val;}
  void setextendedLevel1ID(const unsigned int &val) {m_extendedLevel1ID = val;}
  void setlevel1TriggerType(const unsigned int &val) {m_level1TriggerType = val;}
  void setlevel1TriggerInfo(const std::vector<unsigned int> &val) {m_level1TriggerInfo = val;}

  
 protected:
  unsigned long m_run_number; // event's run number
  unsigned long m_event_number; // event number
  float m_averageInteractionsPerCrossing;
  float m_actualInteractionsPerCrossing;
  int m_LB;
  int m_BCID;
  unsigned int m_extendedLevel1ID;
  unsigned int m_level1TriggerType;
  std::vector<unsigned int> m_level1TriggerInfo;

  ClassDef(HTTEventInfo, 1)
};

std::ostream& operator<<(std::ostream& , const HTTEventInfo& );
#endif // HTTEVENTINPUTHEADER_H
