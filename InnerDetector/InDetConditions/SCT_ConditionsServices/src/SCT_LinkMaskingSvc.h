/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingSvc.h
 * 
 * @author gwilliam@mail.cern.ch
**/

#ifndef SCT_LinkMaskingSvc_h
#define SCT_LinkMaskingSvc_h

// STL includes
#include <string>
#include <set>
#include <list>
#include <utility>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "StoreGate/DataHandle.h"

// Forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;

/**
 * @class SCT_LinkMaskingSvc
 * 
 * 
**/

class SCT_LinkMaskingSvc: virtual public ISCT_ConditionsSvc, public AthService {
  friend class SvcFactory<SCT_LinkMaskingSvc>;
public:

  //@name Service methods
  //@{
  SCT_LinkMaskingSvc(const std::string& name, ISvcLocator* svc);
  virtual ~SCT_LinkMaskingSvc() {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  static const InterfaceID& interfaceID();
  //@}
  
  /**Can the service report about the given component? (chip, module...)*/
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h);
  
  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  
  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash& hashId);

  /**Manually get the data in the structure before proceding*/
  virtual StatusCode                    fillData() { return StatusCode::FAILURE; }
  
  /**Fill data from an IOVDbSvc callback*/
  virtual StatusCode                    fillData(int& i, std::list<std::string>& l);
  
  /**Are the data available?*/
  virtual bool                          filled() const;
  
  /**Can the data be filled during the initialize phase?*/
  virtual bool                          canFillDuringInitialize() { return false; }
  
private:
  std::set<Identifier>                     m_maskedLinkIds;                 //!< Set of masked link identifiers 
  bool                                     m_filled;                        //!< Had the data been filled?
  ServiceHandle<StoreGateSvc>              m_detStore;                      //!< Handle on the detector store
  const DataHandle<CondAttrListCollection> m_dataLink;                      //!< Handle for link info from DB
  const SCT_ID*                            m_sctHelper;                     //!< ID helper for SCT

  /** Retreive a given folder from the DB*/
  StatusCode                            retrieveFolder(const DataHandle<CondAttrListCollection> &pDataVec, const std::string & folderName);
};

inline const InterfaceID& SCT_LinkMaskingSvc::interfaceID(){
  return ISCT_ConditionsSvc::interfaceID(); 
}

#endif // SCT_LinkMaskingSvc_h
