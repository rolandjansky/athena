/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileClusterFilterAlg.h
 * Author: Carlos.Solans@cern.ch, Archil.Durglishvili@cern.ch
 *
 * Nov 2014
 */

#ifndef TILEEOPFILTERALG_H
#define TILEEOPFILTERALG_H

//Local track tools
#include "TrackTools.h"

typedef std::vector<std::vector<int> > ASSOCCONTAINER;
#define ASSOCCONTAINER_CONSTRUCTOR(size) ASSOCCONTAINER( size ,std::vector<int>(0))


//================================================
class TileEopFilterAlg : public AthAlgorithm{
//================================================

public:
  
  TileEopFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  ~TileEopFilterAlg(){};

  virtual StatusCode initialize();
  virtual StatusCode execute(); 

private:

  std::string m_inputTracks;
  std::string m_outputTracks;
  std::string m_inputClusters;
  std::string m_outputClusters;
  std::string m_inputCells;
  std::string m_outputCells;
  std::string m_trackClusters;
  std::string m_trackCells;
  std::string m_clusterCells;
  float       m_trackMom;
  float       m_trackEta;
  float       m_trackIso;
  float       m_trackPtrel;
  float       m_trackClusterR;
  float       m_trackCellR;
  bool        m_dumpLarCells;
  
  StoreGateSvc* m_storeGate;
  ToolHandle<ITrackTools> m_trackInCalo;
  
}; 

#endif
