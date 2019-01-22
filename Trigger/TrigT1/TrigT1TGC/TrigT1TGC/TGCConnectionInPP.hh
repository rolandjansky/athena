/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionInPP.hh,v 1.1.1.10 2003-11-03 10:18:17 mishino Exp $
//  Table of connection between input channel and output channel
// of PatchPanel.
#ifndef TGCConnectionInPP_hh
#define TGCConnectionInPP_hh

#include "TrigT1TGC/TGCPatchPanel.hh"
#include <iostream>
#include <vector>

namespace LVL1TGCTrigger {

class TGCConnectionInPP {
public:
  int getChannelIn(int connectorOut, int chOut);
  int getConnectorIn(int connectorOut, int chOut);
  TGCPatchPanel* getPPIn(int connectorOut, int chOut);

  int getOredChannelIn(int connectorOut, int chOut);
  int getOredConnectorIn(int connectorOut, int chOut);
  TGCPatchPanel* getOredPPIn(int connectorOut, int chOut);
  int getNumberOfChannel(int i)const{return m_nCh[i];};

  void setConnection(int connectorOut, int chOut,
  int connectorIn, int chIn, TGCPatchPanel* PP);
  //  void setNumberOfPPOutputConnectorChannel(int nConnector, int nPPOutCh);
  // test whether there are ored channels.
  bool existOredSignal() const;
  void readConnectionTable(TGCPatchPanel* PP);
  // print out connection table.
  void dumpConnection(int PPId=0) const;
  bool replacePatchPanelPointers(TGCPatchPanel* newPatchPanel, 
				 const std::vector<const TGCPatchPanel*> oldPatchPanels);

  TGCConnectionInPP();
  ~TGCConnectionInPP();
  TGCConnectionInPP(const TGCConnectionInPP& right);
  TGCConnectionInPP& operator=(const TGCConnectionInPP& right);

private:
  int m_nCh[NumberOfPPOutputConnector];
  TGCPatchPanel** m_patchPanelIn[NumberOfPPOutputConnector];
  int* m_channelIn[NumberOfPPOutputConnector];
  int* m_connectorIn[NumberOfPPOutputConnector];

  TGCPatchPanel** m_oredPatchPanelIn[NumberOfPPOutputConnector];
  int* m_oredChannelIn[NumberOfPPOutputConnector];
  int* m_oredConnectorIn[NumberOfPPOutputConnector];
}; 


} //end of namespace bracket

#endif // TGCConnectionInPP_hh
