/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoSvc.h
 * header file for service allowing one to declare modules as bad
 * @author daiki.hayakawa@cern.ch
**/

#ifndef SCT_RODVetoSvc_h
#define SCT_RODVetoSvc_h
//STL includes
#include <string>
#include <set>
#include <vector>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaKernel/CLASS_DEF.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"


//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;
class ISCT_CablingSvc;
//class Identifier; < not fwd declared, used in a vector so need storage size
// class Identifier;
class SCT_RODVetoSvc_test;

/**
 * @class SCT_RODVetoSvc
 * Service allowing one to manually declare detector elements as 'bad' in the joboptions file
**/
class SCT_RODVetoSvc: virtual public ISCT_ConditionsSvc, public AthService{
  friend class SvcFactory<SCT_RODVetoSvc>;
  /** Allow the test class access to all methods */
  friend class SCT_RODVetoSvc_test;

public:
  //@name Service methods
  //@{
  SCT_RODVetoSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_RODVetoSvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
  //@}
  
  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier & elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash & hashId);

  
  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData();
  
  //I'm going to fill this from job options, but these may pecify the database is to be used
  virtual StatusCode fillData(int& /*i*/ , std::list<std::string>& /*l*/);
  
  ///Are the data available?
  virtual bool filled() const;
  
  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize();
  
private:

  ServiceHandle<ISCT_CablingSvc> m_cabling;

  


  //UnsignedIntegerArrayProperty m_badRODElements;
  

  // The vector of bad rods should be kept in a threadsafe way so it can 
  // be called and read safely.
  SG::ReadHandleKey< std::vector<unsigned int> > m_badRODElements;

  std::set<Identifier> m_badIds;

  // These are the read- and writehandle keys for the boolean that checks
  // if the service has been filled. 
  SG::WriteHandleKey<bool> m_wFilled;
  SG::ReadHandleKey<bool>  m_rFilled;
  
  const SCT_ID * m_pHelper;
  ServiceHandle<StoreGateSvc> m_detStore;
};

inline const InterfaceID & SCT_RODVetoSvc::interfaceID(){
  return ISCT_ConditionsSvc::interfaceID(); 
}

#endif
