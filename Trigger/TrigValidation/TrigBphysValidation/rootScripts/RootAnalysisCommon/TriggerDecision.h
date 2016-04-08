/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************
// TriggerDecision.h
// Simple representation of a single 
// trigger decision 
// Can be created from the n-tuple tree
// made by GetTrigger in BPhysAnalysisTools
//*****************************************

class TriggerDecision {
  
private:
  int m_event;
  int m_lumiBlock;
  int m_run;
  int m_prescale;
  std::string m_name;

public:
  // Constructor
  TriggerDecision(void) {
    m_event = 0;
    m_lumiBlock = 0;
    m_run = 0;
    m_prescale = 0;
    m_name = "";
  }
  // Set methods
  void setEvent(int input) {m_event = input;}
  void setLumiBlock(int input) {m_lumiBlock = input;}
  void setRun(int input) {m_run = input;}
  void setPrescale(int input) {m_prescale = input;}
  void setName(std::string input) {m_name = input;}

  // Get methods
  int eventNumber(void) {return m_event;}
  int lumiBlock(void) {return m_lumiBlock;}
  int runNumber(void) {return m_run;}
  int prescale(void) {return m_prescale;}
  std::string name(void) {return m_name;}

}; // End of class definition
