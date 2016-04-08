/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESIMUTILS_TILECALIBHITNTUPLE_H
#define TILESIMUTILS_TILECALIBHITNTUPLE_H

/********************************************************************
 *
 * NAME:     TileCalibHitNtuple
 * PACKAGE:  offline/TileCalorimeter/TileSimUtils
 *
 * AUTHOR :  A. Solodkov
 * CREATED:  Nov 08, 2012
 *
 * PURPOSE:  Store coordinates of every 
 *           single calibration hit in ntuple
 *
 *  Input: event number, DM cell ID, hit coordiates
 *  Output: ntuple 
 *  Parameters: stream name, ntuple name, max tree size
 *   
 ********************************************************************/

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>

#include <inttypes.h>

/**
@class TileCalibHitNtuple
@brief This class creates ntuple with calibration hits info
*/
class TileCalibHitNtuple: public AthAlgTool {

public:
  TileCalibHitNtuple(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor

  virtual ~TileCalibHitNtuple(); //!< Destructor

  virtual StatusCode initialize();  //!< intialize method
  virtual StatusCode finalize();    //!< finalize method
  
  static const InterfaceID& interfaceID( ) ; //!< AlgTool InterfaceID
  
  void storeHit(int det, int mod, int tow, int samp, int reg,
                int calc, int evt, float x, float y, float z, float e);

private:

  // handle to THistSvc
  ServiceHandle<ITHistSvc>   m_thistSvc;

  // NTuple parameters
  std::string m_streamName;
  std::string m_ntupleID;
  int64_t m_treeSize;

  // The ntuple
  TTree* m_ntuplePtr;

  // ntuple variables
  short m_det ;
  short m_mod ;
  short m_tow ;
  short m_samp;
  short m_reg;
  short m_calc;
  short m_evt;
  float m_xPos;
  float m_yPos;
  float m_zPos;
  float m_ener;

};

#endif
