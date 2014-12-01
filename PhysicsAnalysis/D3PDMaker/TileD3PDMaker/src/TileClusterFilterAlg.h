/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileClusterFilterAlg.h
 * Author: Carlos.Solans@cern.ch
 *
 * July 2014
 */

#ifndef TILECLUSTERFILTERALG_H
#define TILECLUSTERFILTERALG_H


// TRACK INCLUDES
#include "TrackTools.h"

//================================================
class TileClusterFilterAlg : public AthAlgorithm{
//================================================

public:
  TileClusterFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  ~TileClusterFilterAlg(){};
  
  virtual StatusCode initialize();
  virtual StatusCode execute(); 
  
private:
  
  StoreGateSvc* m_storeGate;
  std::string m_inputContainerName; 
  std::string m_outputContainerName;
  std::string m_trackContainerName;
  ToolHandle<ITrackTools> m_trackInCalo;
  float m_deltaR;

}; 

#endif // TILECLUSTERFILTERALG_H
