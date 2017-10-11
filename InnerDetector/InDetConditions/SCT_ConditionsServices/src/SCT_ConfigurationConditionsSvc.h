/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConfigurationConditionsSvc.h
 * header file for service which reads SCT configuration from database
 * @author shaun.roe@cern.ch, gwilliam@mail.cern.ch
**/

#ifndef SCT_ConfigurationConditionsSvc_h
#define SCT_ConfigurationConditionsSvc_h

// STL includes
#include <string>

// These are in the base class header
//#include <set>
//#include <list>
//#include <map>
//this is in map
//#include <utility>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"

#include "AthenaKernel/IIOVSvc.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"

#include "Identifier/Identifier.h"

#include "SCT_ConditionsServices/ISCT_ReadoutTool.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"

#include "SCT_ConditionsData/SCT_ConfigurationCondData.h"

#include "SCT_Cabling/ISCT_CablingSvc.h" 

#include "AthenaKernel/IIOVDbSvc.h"  // Why can't forward declare 

// Forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;
//class IIOVDbSvc;
namespace InDetDD {class SCT_DetectorManager; }

/**
 * @class SCT_ConfigurationConditionsSvc
 * Service which reads SCT_Configuration from the database
 * Deals with bad modules, bad links, strips out of the readout and bad strips
**/

class SCT_ConfigurationConditionsSvc: virtual public ISCT_ConfigurationConditionsSvc, virtual public AthService{
  friend class SvcFactory<SCT_ConfigurationConditionsSvc>;
public:

  //@name Service methods
  //@{
  SCT_ConfigurationConditionsSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_ConfigurationConditionsSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
  //@}
  
  /**Can the service report about the given component? (chip, module...)*/
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h);
  
  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  
  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash & hashId);

  /**Manually get the data in the structure before proceding*/
  virtual StatusCode                    fillData(){return StatusCode::FAILURE;}
  
  /**Fill data from an IOVDbSvc callback*/
  virtual StatusCode                    fillData(int& i , std::list<std::string>& l);
  
  /**Are the data available?*/
  virtual bool                          filled() const;
  
  /**Can the data be filled during the initialize phase?*/
  virtual bool                          canFillDuringInitialize(){ return false; }

  /**List of bad modules*/
  virtual const std::set<Identifier>*   badModules() { return m_data.getBadModuleIds(); }
  /**List of bad strips*/
  virtual void                          badStrips(std::set<Identifier>& strips, bool ignoreBadModules = false, bool ignoreBadChips = false);
  /**List of bad strips for a given module*/
  virtual void                          badStrips(const Identifier & moduleId , std::set<Identifier>& strips, bool ignoreBadModules = false, bool ignoreBadChips = false);
  /**List of bad links*/
  virtual std::pair<bool, bool>         badLinks(const Identifier & moduleId);
  /**Bad links for a given module*/
  virtual const std::map<Identifier, std::pair<bool, bool> >* badLinks() {return m_data.getBadLinks();}
  /**List of bad chips*/
  virtual const std::map<Identifier, unsigned int>*           badChips() {return m_data.getBadChips();}
  /**Bad chips for a given module*/
  virtual unsigned int                  badChips(const Identifier & moduleId) const;
  /** Get the chip number containing a particular strip*/
  int                                   getChip(const Identifier & stripId) const;

private:
  SCT_ConfigurationCondData             m_data;                          //!< Contain bad module/wafer/link/chip/strip information
  bool                                  m_filled;                        //!< Had the data been filled?
  ServiceHandle<StoreGateSvc>           m_detStore;                      //!< Handle on the detector store
  ServiceHandle<IIOVSvc>                m_IOVSvc;                        //!< Handle on the IOV service
  ServiceHandle<IIOVDbSvc>              m_IOVDbSvc;                      //!< Handle on the IOVDb service
  const DataHandle<CondAttrListVec>     m_dataChannel;                   //!< Handle for strip info from DB
  const DataHandle<CondAttrListVec>     m_dataModule;                    //!< Handle for module info from DB
  const DataHandle<CondAttrListVec>     m_dataMur;                       //!< Handle for link info from DB
  const SCT_ID*                         m_pHelper;                       //!< ID helper for SCT
  ServiceHandle<ISCT_CablingSvc>        m_cablingSvc;                    //!< Handle on SCT cabling service
  ToolHandle<ISCT_ReadoutTool> m_readoutTool{this, "SCT_ReadoutTool", "SCT_ReadoutTool", "Handle on readout tool"}; //!< Handle on readout tool
  const InDetDD::SCT_DetectorManager*   m_pManager;                      //!< SCT detector manager
  bool                                  m_checkStripsInsideModules;      //!< Do we want to check if a strip is bad because it is inside a bad module

  /** Calculate the strip ID based on module SN, chip number and strip number*/
  Identifier                            getStripId(const unsigned int truncatedSerialNumber, const unsigned int chipNumber, const unsigned int stripNumber) const;
  //void                                  setAllStripsBad(const Identifier& waferId);
  /** Retreive a given folder from the DB*/
  StatusCode                            retrieveFolder(const DataHandle<CondAttrListVec> &pDataVec, const std::string & folderName);
  /** Fill the data on bad strips*/
  StatusCode                            fillChannelData();
  /** Fill the data on bad modules and chips not read out*/
  StatusCode                            fillModuleData();
  /** Fill the data on bad links*/
  StatusCode                            fillLinkStatus();
  /** Is a strip within a bad module*/
  bool                                  isStripInBadModule(const Identifier& stripId);
  /** Is a wafer in a bad module*/
  bool                                  isWaferInBadModule(const Identifier& waferId);
  
  /**Is a chip with this Identifier good?*/
  bool isGoodChip(const Identifier& stripId) const;

  /** enum for constants*/
  enum {badLink=255, stripsPerChip=128, lastStrip=767};
  //determine which folder to use; COMP200 style or CONDBR2 style
  std::string determineFolder(const std::string &option1, const std::string &option2) const;

};

#endif
