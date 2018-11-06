/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDDCS_H
#define TILECONDITIONS_TILECONDDCS_H

// Tile includes
#include "TileConditions/ITileDCSTool.h"
#include "TileConditions/TileCablingSvc.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class TileCablingService;


template <class TYPE> class SvcFactory;


//Class to provide DCS information about modules from the COOL database
class TileDCSSvc: virtual public AthService {
  friend class SvcFactory<TileDCSSvc>;

 public:
  static const InterfaceID& interfaceID(); /*as in TileCablingSvc.h*/
  TileDCSSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TileDCSSvc(){ /*do nothing*/}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  enum TileDCSStatus {NON_EXISTING=0, UNKNOWN=1,
                      OK_DRAWER=10, OK_KNOWNBADPMT, OK,
                      WARNING_DRAWER=20, WARNING_KNOWNBADPMT, WARNING,
                      ALERT_DRAWER=30, ALERT_KNOWNBADPMT, ALERT};

  // these 3 methods return values found in COOL
  // and they use ros=1,2,3,4, module and pmt as parameter, all three starts from "1", like in DCS
  float getDCSHV     (int ros,  int module,  int pmt) const;
  float getDCSHVSET  (int ros,  int module,  int pmt) const;
  int   getDCSSTATES (int ros,  int module) const;

  // these 3 methods return status values which can be used to detect bad PMTs
  // ros=1,2,3,4 as usual, while drawer and channel starts from "0"
  TileDCSStatus getDCSHVSTATUS (int ros, int drawer, int channel) const; // status for HV deviation
  TileDCSStatus getDCSSTATUS   (int ros, int drawer) const;              // status of drawer voltages
  TileDCSStatus getDCSSTATUS   (int ros, int drawer, int channel) const;

  bool statusHVIsBad (int ros, int drawer, int channel) const;
  bool statusIsBad (int ros, int drawer) const;
  bool statusIsBad (int ros, int drawer, int channel) const;

  int  getNcalls() const { return m_ncalls; } // how many times callBack was triggered

  std::string partitionName(int ros) {return m_partName[ros];}

 private:

  ToolHandle<ITileDCSTool> m_dcsTool{this, "TileDCSTool", "TileDCSTool", "Tile DCS tool"};

  int m_ncalls;

  const TileCablingService* m_cabling;

  int m_pmt2channel[2][68];

  std::vector<std::string> m_partName;

  int m_version;
};

#endif
