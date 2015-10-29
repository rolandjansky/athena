/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDDCS_H
#define TILECONDITIONS_TILECONDDCS_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/DataHistory.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/IOVSvcDefs.h" 
#include "AthenaKernel/IIOVDbSvc.h" 

// Tile includes
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileCondToolEmscale.h"

#include <utility>
#include <vector>
#include <string>
#include <list>
#include <map>

// Forward declaration
class StoreGateSvc;
class IIOVSvc;
class ISvcLocator;
class StatusCode;
class IAthenaOutputStreamTool;
class TileCondDCS_Data;
class TileCablingService;


template <class TYPE> class SvcFactory;

//conversion from ROS and drawer number to index (used in array)
#define RosDrawer(ros,drawer) (((ros-1)<<6) | drawer)

//conversion from RosDrawer and pmt-1 to index (used in map)
#define PmtIndex(RosDrawer,pmt0) ((RosDrawer<<8) | pmt0)


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

  virtual StatusCode fillData(int& i, std::list<std::string>& keys );
   //Manually get the data in the structure before proceding
  virtual StatusCode fillData(){return StatusCode::FAILURE;}

  enum TileDCSStatus {NON_EXISTING=0, UNKNOWN=1, 
                      OK_DRAWER=10, OK_KNOWNBADPMT, OK, 
                      WARNING_DRAWER=20, WARNING_KNOWNBADPMT, WARNING, 
                      ALERT_DRAWER=30, ALERT_KNOWNBADPMT, ALERT};

  // these 3 methods return values found in COOL
  // and they use ros=1,2,3,4, module and pmt as parameter, all three starts from "1", like in DCS
  float getDCSHV     (int ros,  int module,  int pmt) const { 
      return m_HV[RosDrawer(ros,(module-1))][pmt-1]; } //return the HV
  float getDCSHVSET  (int ros,  int module,  int pmt) const { 
      return m_HVSET[RosDrawer(ros,(module-1))][pmt-1]; } //return the HVSET
  int   getDCSSTATES (int ros,  int module) const { 
      return m_STATES[RosDrawer(ros,(module-1))]; } // return drawer state

  // these 3 methods return status values which can be used to detect bad PMTs
  // ros=1,2,3,4 as usual, while drawer and channel starts from "0"
  TileDCSStatus getDCSHVSTATUS (int ros, int drawer, int channel) const; // status for HV deviation
  TileDCSStatus getDCSSTATUS   (int ros, int drawer) const;              // status of drawer voltages
  TileDCSStatus getDCSSTATUS   (int ros, int drawer, int channel) const {
      return  m_STATUS[RosDrawer(ros,drawer)][channel]; } // final status - the worst of 2 statuses above

  bool statusHVIsBad (int ros, int drawer, int channel) const { 
      return getDCSHVSTATUS(ros,drawer,channel) > WARNING; }
  bool statusIsBad (int ros, int drawer) const { 
      return getDCSSTATUS(ros,drawer) > WARNING; }
  bool statusIsBad (int ros, int drawer, int channel) const { 
      return m_STATUS[RosDrawer(ros,drawer)][channel] > WARNING; }

  int  getNcalls() const { return m_ncalls; } // how many times callBack was triggered

  std::string partitionName(int ros) {return m_partName[ros];}

 private:
  //Key for DataHandle
  BooleanProperty m_readAnyFolder; 
  BooleanProperty m_readHV; 
  BooleanProperty m_readHVSET;
  BooleanProperty m_readSTATES;
  std::string m_folderPrefix;
  float m_goodHVlimit;
  float m_warningHVlimit;
  int m_goodDrawer;
  std::vector<int> m_warningDrawer;
  int m_unknownValue;

  int m_ncalls;

  //Declare Storegate container
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<IIOVDbSvc> m_IOVDbSvc; //!< Handle on the IOVDb service
  ServiceHandle<TileCablingSvc> m_tileCablingSvc;
  const TileCablingService* m_cabling;
  ToolHandle<TileCondToolEmscale>  m_tileToolEmscale; //!< main Tile Calibration tool

  //DataHandle for callback
  const DataHandle<CondAttrListCollection> m_DCSData_HV;             
  const DataHandle<CondAttrListCollection> m_DCSData_HVSET;
  const DataHandle<CondAttrListCollection> m_DCSData_STATES;

  std::vector<int> m_pModulesHV; 
  std::vector<int> m_pModulesHVSET;
  std::vector<int> m_pModulesSTATES;
  std::map<int, std::pair<float,float> > m_knownBadHV;

  int m_STATES[256];
  float m_HV[256][55];
  float m_HVSET[256][68];
  TileDCSStatus m_STATUS[256][48];

  int m_channel2pmt[2][68];
  int m_pmt2channel[2][68];
  bool m_channel_used[4][48]; // 0=LB, 1=EB, 2=EB without D4, 3=EB with special C10

  int read_config(std::string filename, std::string substr,
                  std::vector<int> & m_pModules);
    
  int read_badhv(std::string filename);

  void fillHVref();

  std::vector<std::string> m_partName;

  int m_version;
};

#endif
