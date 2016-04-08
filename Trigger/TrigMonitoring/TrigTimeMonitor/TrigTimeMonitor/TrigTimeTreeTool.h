// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTIMETREETOOL_H
#define TRIGTIMETREETOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/MonitorToolBase.h"
class TrigTimeTreeEntry;
class TTree;
class TBranch;
class ITrigTimerSvc;

/**
 * \brief Tool to be used to save timers content as a ROOT Tree.
 *
 * To use it one has to plug it in the list of AthenMonTools of HLT algorithms. 
 * If The TrigHLTBaseTreeTool from package TrigMonitorBase is attached before then timers 
 * values are attached to the basic HLT informations defined there.
 *
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>
 **/
class TrigTimeTreeTool: public MonitorToolBase {

public:
  TrigTimeTreeTool(const std::string & type, const std::string & name,
		   const IInterface* parent);

  virtual ~TrigTimeTreeTool();

  StatusCode initialize();
  StatusCode bookHists();
  StatusCode fillHists();
  StatusCode finalHists();
    
private:
  bool selected(const std::string& name);     //!< method to decide if the timer was selected, @return true if yes

  TTree *m_tree;                              //!< tree where we want to fill
  std::vector <TBranch*> m_branches;          //!< all our branches
  std::string m_path;                         //!< booking path
  bool m_localTree;                           //!< flag saying if global or local tree are to be filled
  bool m_doStamps;                            //!< YES, this flag decides abut saving absolute time marks
  std::vector<std::string> m_key;             //!< list of requested keys (all if empty)
  std::vector<TrigTimeTreeEntry *> m_entries; //!< timers of interest
  std::string m_parentName;


};

#endif 
