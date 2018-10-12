/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILEBEAMINFOPROVIDER_H
#define TILERECUTILS_TILEBEAMINFOPROVIDER_H

/********************************************************************
 *
 * NAME:     TileBeamInfoProvider
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  A. Solodkov
 * CREATED:  June 06, 2004
 *
 * PURPOSE:  Re
 *
 *  Input: TileBeamElemContainer
 *  Output: contents of cispar fragment, header, status and trigType
 *  Parameters:
 *    TileBeamElemContainer - Name of input container
 *
 ********************************************************************/

// Tile includes
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileTriggerContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileConditions/ITileDCSTool.h"
#include "TileEvent/TileDQstatus.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>
#include <inttypes.h>


class TileHWID;
class TileRawChannelCollection;
class StoreGateSvc;
class TileBeamInfoProvider;
class ITileBadChanTool;
class IAtRndmGenSvc;
namespace CLHEP {
  class HepRandomEngine;
}



/**
@class coincBoard
@brief Small class holding the information for a cosmic trigger coincidence board
*/
class coincBoard {
public:
  bool trig[96];  //!< each component of this array correspond to a single tower of a module (12 modules x 8 towers = 96 components)
  uint32_t amp;
};

/**
@class TileBeamInfoProvider
@brief This class provides the contents of cispar fragment, header, status and trigType from the objects in a TileBeamElemContainer
*/
class TileBeamInfoProvider: public AthAlgTool
                          , virtual public IIncidentListener {
  //friend class TileDQstatus;
 public:
  TileBeamInfoProvider(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor

  virtual ~TileBeamInfoProvider(); //!< Destructor

  virtual StatusCode initialize();  //!< intialize method
  virtual StatusCode finalize();    //!< finalize method

  virtual void handle(const Incident&) ;   //!< Callback for Incident service

  static const InterfaceID& interfaceID( ) ; //!< AlgTool InterfaceID

  const TileDQstatus* getDQstatus(); //<! Creates TileDQstatus object and fills arrays from DQ fragment

  bool          isChanDCSgood   (int partition, int drawer, int channel) const;

  inline uint32_t   eventCounter(void)  const { return m_evt; }
  inline uint32_t      calibMode(void)  const { return m_calibMode; }
  inline int            trigType(void)  const { return m_trigType; }
  inline uint32_t      laserFlag(void)  const { return m_laserFlag; }
  inline uint32_t       digiSize(void)  const { return m_digiSize; }
  inline uint32_t           BCID(void)  const { return m_BCID; }
  inline const uint32_t * cispar(void)  const { return m_cispar; }
  inline const uint32_t * laspar(void)  const { return m_laspar; }
  uint32_t        checkCalibMode(void);
  inline const std::vector<coincBoard> & getCoincBoard(void) const { return m_coincTrig; }

  void setContainers(const TileDigitsContainer * digitsCnt = NULL,
                     const TileRawChannelContainer * rcCnt = NULL,
                     const TileBeamElemContainer * beamElemCnt = NULL);

  const TileRawChannelContainer * dspContainer() const { return m_rcCnt;}
  bool noiseFilterApplied() const { return m_noiseFilterApplied;}
  void setNoiseFilterApplied(bool val=true) {m_noiseFilterApplied=val;}
  bool incompleteDigits() const { return m_incompleteDigits;}

 protected:

  bool m_checkDQ;   //!< if false, skip DQ checks (set to false if container is not found in first event)
  bool m_checkDigi; //!< if false, skip reading of TileDigitsContainer ( /-/-/-/ )
  bool m_checkBeam; //!< if false, skip reading of TileBeamContainer   ( /-/-/-/ )
  bool m_checkDCS;  //!< if false, do not use Tile DCS at all
  bool m_simulateTrips;  //! if true simulate drawer trips

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfo",
                                                    "EventInfo", "Input Event info key"};

  SG::ReadHandleKey<TileBeamElemContainer> m_beamElemContainerKey{this, "TileBeamElemContainer",
                                                                  "", "Input Tile beam elements container key"};

  SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this, "TileDigitsContainer",
                                                              "", "Input Tile digits container key"};

  SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this, "TileRawChannelContainer",
                                                                      "", "Input Tile raw channel container key"};

  SG::WriteHandleKey<TileTriggerContainer> m_triggerContainerKey{this, "TileTriggerContainer",
                                                                 "", "Output Tile trigger container key"};

  SG::WriteHandleKey<TileLaserObject> m_laserObjectKey{this, "TileLaserObject",
                                                       "", "Output Tile laser object key"};


  ToolHandle<ITileDCSTool> m_tileDCS{this, "TileDCSTool", "TileDCSTool", "Tile DCS tool"};
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;  //!< Random number service to use
  ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tool which provides trips probabilities also

  const TileHWID* m_tileHWID; //!< Pointer to TileHWID

  int      m_trigType;     //!< Trigger type
  uint32_t m_laserFlag;    //!< Laser Flag
  uint32_t m_cispar[110];  //!< Parameter for CIS
  uint32_t m_laspar[32];   //!< Parameter for LASER

  uint32_t m_calibMode;    //!< Calibration mode

  uint32_t m_digiSize;     //!< number of samples in digits

  uint32_t m_BCID;         //!< BCID in LASTROD

  uint32_t m_evt;          //!< number of event

  std::vector<coincBoard> m_coincTrig; // vector with instances of the coincBoard class

  TileDQstatus m_DQstatus; //<! DQStatus

  const TileDigitsContainer *     m_digitsCnt;
  const TileRawChannelContainer * m_rcCnt;
  const TileBeamElemContainer *   m_beamElemCnt;

  CLHEP::HepRandomEngine* m_pHRengine;    //!< Random number generator engine to use
  double* m_rndmVec;

  bool m_noiseFilterApplied;    //!< True if noise filter is applied to DSP container

  bool m_incompleteDigits;    //!< True if not all digits are available (taken from frag 1)

};

#endif
