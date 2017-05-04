/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_MONITORCONDITIONSSVC_SCT_MONITORCONDITIONSSVC_H
#define SCT_MONITORCONDITIONSSVC_SCT_MONITORCONDITIONSSVC_H

//STL
#include <string>
#include <map>
#include <set>
#include <list>

// Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

//COOL includes

// Athena includes
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"

// local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_MonitorConditionsSvc.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class EventInfo;
class IIOVRegistrationSvc;
class IAthenaOutputStreamTool;
class CondAttrListCollection;
namespace coral{
  class AttributeList;
}
//class IIOVDbSvc;

/**
 ** Class for keeping track of errors caught by the monitoring
 **/

class SCT_MonitorConditionsSvc: virtual public ISCT_MonitorConditionsSvc, virtual public AthService{
  friend class SvcFactory<SCT_MonitorConditionsSvc>;

public:
  // Constructor
  SCT_MonitorConditionsSvc ( const std::string& name, ISvcLocator* svc );
  // Destructor
  virtual ~SCT_MonitorConditionsSvc();

  // overloading functions
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);

  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT);

  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId);


  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData();

  //I'm going to fill this from job options, so the callback version of fillData is not needed.
  virtual StatusCode fillData(int& /*i */, std::list<std::string>& /*l*/) { //comment out unused parameters to prevent compiler warning
    return StatusCode::FAILURE;
  }

  ///Are the data available?
  virtual bool filled() const;

  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize() { return true; }

  /// List of bad strip Identifiers
  virtual void badStrips(std::set<Identifier>& strips);
  
  /// List of bad strip Identifiers for a given module
  virtual void badStrips(const Identifier & moduleId, std::set<Identifier>& strips);

  /// String of bad strip numbers for a given module
  virtual std::string badStripsAsString(const Identifier & moduleId);

  /// String of dead strip/chip numbers for a given module
  virtual std::string deadStripsAsString(const Identifier & moduleId);
  virtual std::string deadChipsAsString(const Identifier & moduleId);

  // ------------------------------------------------------------------------------------
  // local stuff 
  // ------------------------------------------------------------------------------------

private:
  std::string deadStrip_module(const Identifier& id) const;
  std::string deadChip_module(const Identifier& id) const;
  float eff_module(const Identifier& id) const;
  float no_module(const Identifier& id) const;

  std::string addDefect(const std::string& defectlist,
    const int defectBeginChannel,
    const int defectEndChannel) const;

  std::string createDefectString(const int defectBeginChannel,
    const int defectEndChannel) const;

  StatusCode createCondObjects( const Identifier& wafer_id,
    const SCT_ID* sctId,
    const int samplesize,
    const std::string& defectType,
    const float threshold,
    const std::string& defectList ) const;
    
  StatusCode setBasicListValues(coral::AttributeList & attrList,
    const Identifier & module_id,
    const SCT_ID* sctId,
    const int samplesize,
    const bool camelCasedBec) const;

  StatusCode createListEff( const Identifier& wafer_id,
    const SCT_ID* sctId,
    const int samplesize,
    const float eff ) const;

  StatusCode createListNO( const Identifier& wafer_id,
    const SCT_ID* sctId,
    const int samplesize,
    const float noise_occ ) const;

    //std::string getList(const unsigned int imodule,
    //const std::string& foldername) const;
    
  std::string getList(const Identifier & imodule) const;
  std::string getDeadThingList(const Identifier & imodule, const bool thingIsStrip) const;
  std::string getDeadStripList(const Identifier & imodule) const;
  std::string getDeadChipList(const Identifier & imodule) const;

  float getEff(const Identifier& imodule,
    const std::string& foldername) const;

  float getNO(const Identifier& imodule,
    const std::string& foldername) const;

  bool stripIsNoisy(const int strip, const std::string& defectList) const;

  bool chipIsNoisy(int strip, const std::string& defectList) const;

  bool waferIsNoisy(const int strip, const std::string& defectList) const;

  bool moduleIsNoisy(const std::string& defectList) const ;

  bool inRange(const int theNumber, const std::string& stringRange) const;

  bool inList(const int theNumber, const std::string& theList) const;

  int  nBlock(const int theNumber, const std::string& stringRange) const;

  int  nDefect(const int theNumber, const std::string& theList) const;

  int  nBlock(const std::string& stringRange) const;

  int  nDefect(const std::string& theList) const;

  bool inRange(const int x, const int min, const int max) const;

  void expandRange(const std::string& rangeStr, std::set<int>& rangeList);
  
  void expandList(const std::string& defectStr, std::set<int>& defectList);

  StatusCode printCondObjects(const std::string & foldername) const;

  StatusCode streamOutCondObjects(const std::string & foldername) const;

  StatusCode registerCondObjects(const std::string & foldername,const std::string & tagname);

  StatusCode genericWrapUp(const CondAttrListCollection* theCollection, const std::string & theFolderName, const std::string & theTag);
  
  StatusCode wrapUpNoisyChannel();

  StatusCode wrapUpEfficiency();

  StatusCode wrapUpNoiseOccupancy();

  StatusCode registerIOV(const CLID& clid);

  // small helper function
  unsigned int computeIstrip4moncond(const Identifier& elementId) const;

  virtual StatusCode getAttrListCollection(int& i, std::list<std::string>& l);

  const CondAttrListCollection* getAttrListCollectionByFolder(const std::string&) const;

  // would it make sense to change the strings to properties?
  // that would be a fairly simple fix
  std::vector<std::string> m_keys;
  static std::string s_separator;
  static std::string s_defectFolderName;
  static std::string s_deadStripFolderName;
  static std::string s_deadChipFolderName;
  static std::string s_effFolderName;
  static std::string s_noFolderName;
  // cache for the Collections, access by foldername
  mutable std::map<const int, const std::string>  m_defectListMap;
  mutable std::map<const std::string, const CondAttrListCollection*>  m_attrListCollectionMap;
  const DataHandle<CondAttrListCollection> m_DefectData;
  const CondAttrListCollection*      m_attrListCollection;
  CondAttrListCollection*      m_attrListColl;
  CondAttrListCollection*      m_attrListColl_eff;
  CondAttrListCollection*      m_attrListColl_no;
  ServiceHandle<StoreGateSvc>  m_detStore;
  ServiceHandle<IIOVDbSvc>     m_IOVDbSvc;
  BooleanProperty              m_writeCondObjs;
  BooleanProperty              m_regIOV;
  BooleanProperty              m_readWriteCool;
  BooleanProperty              m_twoStepWriteReg;
  BooleanProperty              m_manualiov;
  IntegerProperty              m_regTime;
  IntegerProperty              m_version;
  IntegerProperty              m_nhits_noisychip;
  IntegerProperty              m_nhits_noisywafer;
  IntegerProperty              m_nhits_noisymodule;
  UnsignedIntegerProperty      m_beginRun;
  UnsignedIntegerProperty      m_endRun;
  StringProperty               m_streamName;
  std::string                  m_tagID4List;
  std::string                  m_tagID4Eff;
  std::string                  m_tagID4NO;

  const EventInfo*             m_evt;
  IIOVRegistrationSvc*         m_regSvc;
  IAthenaOutputStreamTool*     m_streamer;

  StringArrayProperty          m_badElements; //list of bad detector elements (= module sides)
  std::set<Identifier>         m_badIds;
  bool                         m_filled;
  bool                         m_defectRecorded;
  bool                         m_effRecorded;
  bool                         m_noRecorded;
  const SCT_ID*                m_pHelper;
  // FIXME: this caches only the last call of getList.
  // creating a hash of _all_ calls may be faster, but wastes a lot of memory
  mutable std::string          m_currentDefectList;

};

#endif // SCT_MonitorConditinosSvc.h
