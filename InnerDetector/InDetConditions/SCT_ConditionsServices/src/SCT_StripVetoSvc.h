/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_StripVetoSvc.h
 * header file for service allowing one to declare strips as bad
 * @author shaun.roe@cern.ch
**/

#ifndef SCT_StripVetoSvc_h
#define SCT_StripVetoSvc_h
//STL includes
#include <string>
#include <set>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"

//Athena includes
#include "Identifier/Identifier.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class StatusCode;
//class Identifier; < not fwd declared, used in a vector so need storage size


/**
 * @class SCT_StripVetoSvc
 * Service allowing one to manually declare detector strips as 'bad' in the joboptions file
**/
class SCT_StripVetoSvc: virtual public ISCT_ConditionsSvc, public AthService{
  friend class SvcFactory<SCT_StripVetoSvc>;
public:
  //@name Service methods
  //@{
  SCT_StripVetoSvc( const std::string & name, ISvcLocator* svc);
  virtual ~SCT_StripVetoSvc(){}
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
  
  //I'm going to fill this from job options, so the callback version of fillData is not needed.
  virtual StatusCode fillData(int& /*i */, std::list<std::string>& /*l*/) {//comment out unused parameter to prevent compiler warning
    return StatusCode::FAILURE;
  }
  
  ///Are the data available?
  virtual bool filled() const;
  
  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize(){ return true; }
  
private:
  StringArrayProperty m_badElements; //list of bad detector elements (= strips)
  std::set<Identifier> m_badIds;
  bool m_filled;
  const SCT_ID * m_pHelper;
  
};

inline const InterfaceID & SCT_StripVetoSvc::interfaceID(){
  return ISCT_ConditionsSvc::interfaceID(); 
}

#endif
