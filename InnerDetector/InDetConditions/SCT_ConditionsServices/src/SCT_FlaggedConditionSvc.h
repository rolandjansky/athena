/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file SCT_FlaggedConditionSvc.h
 * header file for service allowing one to flag modules as 'bad' with a reason
 * @author gwilliam@mail.cern.ch
 */

#ifndef SCT_FlaggedConditionSvc_h
#define SCT_FlaggedConditionSvc_h
 
// STL
#include <map>
#include <string>

// Gaudi
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Local
#include "SCT_ConditionsServices/ISCT_FlaggedConditionSvc.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"

// Forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;

class Identifier;
class SCT_ID;
class StoreGateSvc;
class StatusCode;

/*
 * @class SCT_FlaggedConditionSvc
 * Service allowing one to flag detector elements as 'bad' with a reason
 */

class SCT_FlaggedConditionSvc: virtual public ISCT_FlaggedConditionSvc, virtual public IIncidentListener, virtual public AthService{
  friend class SvcFactory<SCT_FlaggedConditionSvc>;

public:
  //@name Service methods
  //@{
  SCT_FlaggedConditionSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_FlaggedConditionSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
  virtual void handle(const Incident& inc);
  //@}

  /**Can the service report about the given component? (chip, module...)*/
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  
  /**Is the detector element good?*/
  virtual bool isGood(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  virtual bool isGood(const IdentifierHash & hashId);

  /**Manually get the data*/
  virtual StatusCode fillData() {
    m_filled = true;
    return StatusCode::SUCCESS;
  }

  /** Get the data via callback*/
  virtual StatusCode fillData(int& /*i*/ , std::list<std::string>& /*l*/) {
    return StatusCode::FAILURE;
  }
  
  /**Are the data available?*/
  virtual bool filled() const {return m_filled;}

  /**Can the data be filled during the initialize phase?*/
  virtual bool canFillDuringInitialize(){ return false; }

  /**Flag a wafer as bad with a reason (by Identifier)*/
  bool flagAsBad(const Identifier& id, const std::string& source);
  /**Flag a wafer ID as bad with a reason (by IdentifierHash)*/
  bool flagAsBad(const IdentifierHash& hashId, const std::string& source);
  
  /**Get the reason why the wafer is bad (by Identifier)*/ 
  const std::string& details(const Identifier& id) const;
  /**Get the reason why the wafer is bad (by IdentifierHash)*/ 
  const std::string& details(const IdentifierHash& id) const;

  /**Get number flagged as bad (per event)*/
  inline int numBadIds() const {return m_badIds.size();}

  /**Get IdentifierHashs ofwafers flagged as bad + reason (per event)*/
  inline const std::map<IdentifierHash, std::string>* getBadIds() const {return &m_badIds;}

  /**Reset between events*/
  virtual void resetBadIds();

 private:
  bool                                                m_filled;      //!< Has this been filles
  std::map<IdentifierHash, std::string> m_badIds;  //!< Map of bad IdentiferHash and reason

  ServiceHandle<StoreGateSvc>                         m_detStore;    //!< Handle on detector store
  const SCT_ID*                                       m_sctID;       //!< ID helper for SCT
};

#endif
