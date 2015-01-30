/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RoadMarketTool_H
#define FTK_RoadMarketTool_H

#include "TrigFTKSim/FTK_RoadMarketToolI.h"
#include "TrigFTKSim/FTKRoadOutput.h"
#include "TrigFTKSim/FTKRoadInput.h"
#include "TrigFTKSim/FTK_RoadMarketIO.h"
#include "TrigFTKSim/FTKRoadStream.h"


#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"


#include <vector>
#include <string>
#include <TTree.h>

/** The market tool is a tool that keep an array of road streams
    in memory, allowing to interface this array with an output and
    an input interface. This tool can be used to move roads from the
    road finder to the track fitter without the use of the SG */
class FTK_RoadMarketTool : virtual public FTK_RoadMarketToolI, public AthAlgTool {
private:
  int m_nbanks;

  FTKRoadStream **m_data;

  FTK_RoadMarketOutput m_roadoutput;
  FTK_RoadMarketInput m_roadinput;

  RoadFinder*  m_roadFinder;
  TrackFitter* m_trackFitter;

  mutable MsgStream m_log;

  bool m_SaveRoads; // if True the roads are saved in the SG
  TTree *m_tree; // externally controlled TTree where the roads can be stored
  int m_bufsize; // standard buffer size for the roads
  std::string m_bnamefmt; // Format string for the branches: def "FTKRoadsStream%d."
  std::vector<int> m_goodreg; // integer representing the regions where output is expected

public:
  FTK_RoadMarketTool(const std::string&, const std::string&, const IInterface*);
  virtual ~FTK_RoadMarketTool();

  StatusCode initialize();
  StatusCode finalize();

  bool SaveRoads() const { return m_SaveRoads; }
  void SaveRoads(bool flag) { m_SaveRoads = flag; }

  virtual void ConnectRoads(TTree*,const char*,const std::vector<int>&, int bufsize=0);

  virtual FTKRoadOutput* outputReference() { return &m_roadoutput;}
  virtual FTKRoadInput* inputReference() { return &m_roadinput;}

  virtual RoadFinder* roadFinderReference() { return m_roadFinder; }
  virtual void setRoadFinderReference(RoadFinder* roadfinder) { m_roadFinder = roadfinder; }

  virtual TrackFitter* trackFitterReference() { return m_trackFitter; }
  virtual void setTrackFitterReference(TrackFitter* trackFitter) { m_trackFitter = trackFitter; }

  void initRoads();

  void setNBanks(int nbanks);
  int getNBanks() const { return m_nbanks; }

  FTKRoadStream* getStream(int id) { return m_data[id]; }

  unsigned long runNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->runNumber() : 0 ); }
  unsigned long eventNumber(const unsigned int& ibank) const { return( (m_data && (m_data[ibank])) ? (m_data[ibank])->eventNumber() : 0 ); }
  void setRunNumber(const unsigned int& ibank,const unsigned long& val) { if( m_data && (m_data[ibank]) ) { (m_data[ibank])->setRunNumber(val); } }
  void setEventNumber(const unsigned int& ibank,const unsigned long& val) { if( m_data && (m_data[ibank]) ) { (m_data[ibank])->setEventNumber(val); } }



  void clearRoads();
  void prepareFits();
};
#endif // FTK_RoadMarketTool_H
