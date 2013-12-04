/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////
//
// author: gia@mail.cern.ch
// 
// December, 2005
//
/////////////////////////////////////////////

#ifndef TILESIMINFOLOADER_H
#define TILESIMINFOLOADER_H


#include "GaudiKernel/Service.h"

#include <string>
#include <vector>


class TileG4SimOptions;
class StoreGateSvc;

static const InterfaceID IID_TileSimInfoLoader("TileSimInfoLoader", 1 , 0);

/** 
* @class TileSimInfoLoader
* @brief Special options used in G4 simulation
* @author Gia Khoriauli
*
*      This class provides methods to pass some options 
*       to G4 simulation of TileCal via jobOptions 
*/

class TileSimInfoLoader : public Service  
{
 public:

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  static const InterfaceID& interfaceID() { return IID_TileSimInfoLoader; }

  TileSimInfoLoader(const std::string& name, ISvcLocator* pSvcLocator);
  ~TileSimInfoLoader();


 private:

  StoreGateSvc*     m_detStore   ;
  TileG4SimOptions* m_tileSimInfo;

  std::vector<double> m_deltaTHit   ;
  double m_timeCut     ;
  bool   m_tileTB      ;
  bool   m_platesToCell;
  int    m_Ushape      ;
  bool   m_doBirk      ;
  bool   m_doTileRow   ;
  bool   m_doTOFCorrection;

};


#endif
