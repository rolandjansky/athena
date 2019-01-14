/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONMDT_CABLING_MUONMDT_CABLINGSVC_H
#define MUONMDT_CABLING_MUONMDT_CABLINGSVC_H

#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
//#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

class MuonMDT_CablingMap;
class MdtMezzanineType;
class StoreGateSvc;
//class IMDTcablingSvc;
class IMDTCablingDbTool;
class ITagInfoMgr;

class IdentifierHash;

#include <list>
#include <string>

static const InterfaceID IID_IMuonMDT_CablingSvc("MuonMDT_CablingSvc", 1, 0);



class MuonMDT_CablingSvc : public AthService 
			   //			   virtual public IInterface, 
			   // virtual public IIncidentListener 

{

 public:

  MuonMDT_CablingSvc(const std::string& name,ISvcLocator* sl);
  virtual ~MuonMDT_CablingSvc();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** methods called at trigger stop/start */
  //  virtual StatusCode start();
  //  virtual StatusCode stop();

  /** method for begin run transition */
  //  virtual void handle(const Incident&);

  static const InterfaceID& interfaceID() { return IID_IMuonMDT_CablingSvc; }

  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );

  // IOV service callback
  StatusCode initMappingModel(IOVSVC_CALLBACK_ARGS_P(I,keys));

  // IOV service callback
  StatusCode initMappingModelNoCallback();

  //  MuonMDT_CablingMap* getCablingMap() {return m_cablingMap;}
  const MuonMDT_CablingMap* getCablingMap();

  /*
   *  get the id of the ROD to which a chamber is connected;
   *  this function is needed by the bytestream converters 
   */
  uint32_t getROBId(int stationName, int stationEta, int stationPhi);

  /*
   * Get the ROB id from the hashId
   */
  uint32_t getROBId(const IdentifierHash mdtHashId);

  /*
   *  get the robs corresponding to a vector of hashIds
   */
  std::vector<uint32_t> getROBId(const std::vector<IdentifierHash>& mdtHashVector);

  /*
   *  get all rob Ids
   */
  std::vector<uint32_t> getAllROBId();

 /** return a vector of HashId lists for a  given list of ROD's */
  const std::vector<IdentifierHash> getChamberHashVec(const std::vector< uint32_t> &ROBId_list);

 /** return a HashId list for a  given ROD */
  const std::vector<IdentifierHash>& getChamberHashVec(const uint32_t ROBId);

  /*
   *  get the offline id for a given online id
   *
   */  
  bool getOfflineId(uint8_t subdetectorId,
		    uint8_t rodId,
		    uint8_t csmId,
		    uint8_t tdcId,
		    uint8_t channelId,
		    int& stationName, int& stationEta, int& stationPhi,
		    int& multiLayer, int& layer, int& tube);
    
  /** return the online id given the offline id */
  bool getOnlineId(int stationName, int stationEta, int stationPhi,
		   int multiLayer, int layer, int tube,
		   uint8_t& subdetectorId, uint8_t& rodId, uint8_t& csmId,
		   uint8_t& tdcId, uint8_t& channelId);


  virtual StatusCode compareTags(IOVSVC_CALLBACK_ARGS);

  /** Returns true if we're using the old (non-DB) cabling*/
  bool usingOldCabling() const;

 private:

  /** pointer to the class holding the cabling map */
  //  MuonMDT_CablingMap* m_cablingMap;
  const MuonMDT_CablingMap* m_cablingMap;

  /** flag to use the old cabling service.
      To be activated only when reading old simulated data */
  bool m_useOldCabling;

  /** pointer to the db access tool */
  ToolHandle<IMDTCablingDbTool> m_dbTool;

  /** flag to indicate the first access to the map (to be fixed) */
  bool m_firstAccess;

  // this flag is by default false, to be set to true only to read MDT calibration stream
  bool m_doCalStreamInit;


  /** pointer to the old cabling service: added for backwards compatibility to read
      old simulated data */
  //  ServiceHandle<IMDTcablingSvc> m_oldCablingSvc;
  
  ServiceHandle<ITagInfoMgr>  m_tagInfoMgr;                   // Tag Info Manager
  mutable bool  m_tagsCompared;
  
  StoreGateSvc* m_detStore;

  /**Set to true, to override callback and take original setting of m_useOldCabling*/
  bool m_forceUse;
};

inline bool MuonMDT_CablingSvc::usingOldCabling() const {
    return m_useOldCabling;
}

#endif    //  MUONMDT_CABLING_MUONMDT_CABLINGSVC_H 
