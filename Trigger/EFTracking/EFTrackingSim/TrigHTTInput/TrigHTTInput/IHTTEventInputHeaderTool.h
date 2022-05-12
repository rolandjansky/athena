/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file IHTTEventInputHeaderTool.h
 *
 * This declares a basic interface for input tools which provide the HTTEventInputHeader data
 * to all HTT processing.
 */

#ifndef IHTTEventInputHeaderTool_H
#define IHTTEventInputHeaderTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TFile.h"
#include "TTree.h"


// to do: merge this with TrigHTTSGInput/ITrigHTTInputTool
// since they are both abstract interfaces

class HTTEventInputHeader;
class IHTTEventInputHeaderTool : virtual public ::IAlgTool 
{      
 public:
  virtual ~IHTTEventInputHeaderTool() = default;
  DeclareInterfaceID( IHTTEventInputHeaderTool, 1, 0);

   
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last) = 0;
  virtual StatusCode writeData(HTTEventInputHeader* header) = 0;
  virtual HTTEventInputHeader*  getEventInputHeader()   {return m_eventHeader; };
  virtual TTree* getEventTree() {return m_EventTree;};

 protected:

  HTTEventInputHeader *       m_eventHeader;
  TFile *                     m_infile = nullptr;
  TTree *                     m_EventTree = nullptr;
  
};


#endif // IHTTEventInputHeaderTool_H
