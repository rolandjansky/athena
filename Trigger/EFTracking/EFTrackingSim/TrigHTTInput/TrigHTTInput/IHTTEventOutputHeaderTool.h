/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IHTTEventOutputHeaderTool_H
#define IHTTEventOutputHeaderTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TFile.h"
#include "TTree.h"

class HTTLogicalEventOutputHeader;
class HTTLogicalEventInputHeader;

class IHTTEventOutputHeaderTool : virtual public ::IAlgTool 
{      
 public:

  DeclareInterfaceID( IHTTEventOutputHeaderTool, 1, 0);
  virtual ~IHTTEventOutputHeaderTool() = default;
   
  virtual StatusCode readData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader, bool &last) = 0;
  virtual StatusCode writeData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader) = 0;
  
  virtual TTree* getEventTree() {return m_EventTree;};
  virtual  HTTLogicalEventInputHeader*  getLogicalEventInputHeader_1st()  {return m_eventInputHeader_1st; };
  virtual  HTTLogicalEventInputHeader*  getLogicalEventInputHeader_2nd()  {return m_eventInputHeader_2nd; };
  virtual  HTTLogicalEventOutputHeader* getLogicalEventOutputHeader()   {return m_eventOutputHeader;};
 
 protected:
  
  HTTLogicalEventInputHeader  *m_eventInputHeader_1st;
  HTTLogicalEventInputHeader  *m_eventInputHeader_2nd;
  HTTLogicalEventOutputHeader *m_eventOutputHeader;

  

  TFile *m_infile = nullptr;
  TTree *m_EventTree = nullptr;

};


#endif // IHTTEventOutputHeaderTool_H
