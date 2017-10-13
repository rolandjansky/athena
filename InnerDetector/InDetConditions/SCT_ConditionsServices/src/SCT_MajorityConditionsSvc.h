/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityConditionsSvc.h
 * header file for service 
 * @author gwilliam@mail.cern.ch
 **/

#ifndef SCT_MajorityConditionsSvc_h
#define SCT_MajorityConditionsSvc_h

// STL includes
#include <string>
#include <list>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"

#include "AthenaKernel/IIOVSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "SCT_ConditionsServices/ISCT_DetectorLevelConditionsSvc.h"

#include "SCT_ConditionsData/SCT_MajorityCondData.h"

// Forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class StatusCode;

/**
 * @class SCT_MajorityConditionsSvc
 * Service which reports on whether the majority of the SCT (or component) is in LV/HV on state with 
 * at least a fraction (default 0.9) of the HV in that state
 * 
 **/

class SCT_MajorityConditionsSvc: virtual public ISCT_DetectorLevelConditionsSvc, virtual public AthService {
  friend class SvcFactory<SCT_MajorityConditionsSvc>;

 public:
  //@name Service methods
  //@{
  SCT_MajorityConditionsSvc(const std::string& name, ISvcLocator* svc);
  virtual ~SCT_MajorityConditionsSvc() {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  static const InterfaceID& interfaceID();
  //@}
  
  /**Is the detector good?*/
  virtual bool                             isGood();

  /**Is a barrel/endcap good?*/
  virtual bool                             isGood(int bec);
  
  /**Manually get the data in the structure before proceding*/
  virtual StatusCode                       fillData() { return StatusCode::FAILURE; }
  
  /**Fill data from an IOVDbSvc callback*/
  virtual StatusCode                       fillData(int& i, std::list<std::string>& l);
  
  /**Are the data available?*/
  virtual bool                             filled() const;
  
  /**Can the data be filled during the initialize phase?*/
  virtual bool                             canFillDuringInitialize() { return false; }

 private:
  bool                                     m_filled;                        //!< Had the data been filled?
  ServiceHandle<StoreGateSvc>              m_detStore;                      //!< Handle on the detector store
  ServiceHandle<IIOVSvc>                   m_IOVSvc;                        //!< Handle on the IOV service
  const DataHandle<CondAttrListCollection> m_dataMajority;                  //!< Handle for majority data
  SCT_MajorityCondData                     m_data;                          //!< Store majority state and HV fraction information
  bool                                     m_overall;                       //!< Use overall vvalue or ECA/B/ECC
  float                                    m_majorityFraction;              //!< Required fraction in majority state
  static const std::string                 s_coolMajorityFolderName;        //!< Majority folder name in COOL

  /** Retreive a given folder from the DB*/
  StatusCode                               retrieveFolder(const DataHandle<CondAttrListCollection>& pDataVec, const std::string& folderName);
};

#endif // SCT_MajorityConditionsSvc_h
