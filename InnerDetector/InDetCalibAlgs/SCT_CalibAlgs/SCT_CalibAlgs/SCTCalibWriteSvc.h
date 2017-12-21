/** -*- c++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
* @file SCTCalibWriteSvc.h
*
* @brief Header file for the SCTCalibWriteSvc class 
*
* @author Jose E. Garcia
**/

#ifndef SCTCalibWriteSvc_H
#define SCTCalibWriteSvc_H

//STL
#include <string>
#include <map>
#include <set>
#include <list>

// Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/ReadHandleKey.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"


// Athena includes
#include "Identifier/Identifier.h"

// local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"

//forward declarations
class ISvcLocator;
class IdentifierHash;
class SCT_ID;
class EventInfo;
class IIOVRegistrationSvc;
class IAthenaOutputStreamTool;
class CondAttrListCollection;
class StoreGateSvc;

/**
** Algorithm to test writing conditions data and reading them back.
  **/
class SCTCalibWriteSvc: public AthService {
  friend class SvcFactory<SCTCalibWriteSvc>;
 public:
  // Constructor
  SCTCalibWriteSvc(const std::string &name, ISvcLocator *pSvcLocator) ;
  // Destructor
  ~SCTCalibWriteSvc();
  
  // overloading functions
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_SCTCalibWriteSvc("SCTCalibWriteSvc", 1, 0); 
    return IID_SCTCalibWriteSvc;
  }
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF); 
  
  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData();
  
  //I'm going to fill this from job options, so the callback version of fillData is not needed.
  virtual StatusCode fillData(int& /*i */, std::list<std::string>& /*l*/) { //comment out unused parameters to prevent compiler warning
    return StatusCode::FAILURE;
  }
  
  std::string 
  addDefect(const std::string& defectlist,const int defectBeginChannel,const int defectEndChannel) const;
  
  std::string 
  addNumber(const std::string numStr,const unsigned long long number) const;

  std::string 
  createDefectString(const int defectBeginChannel,const int defectEndChannel) const;
  
  StatusCode 
  createCondObjects( const Identifier& wafer_id,const SCT_ID* m_sctId,const int samplesize,const std::string& defectType,const float threshold,const std::string& defectList ) const;
  
  StatusCode createListStrip( const Identifier& wafer_id,
            const SCT_ID* m_sctId,
            const int samplesize,
            const std::string& defectType,
            const float threshold,
            const std::string& defectList ) const;
  
  StatusCode createListChip( const Identifier& wafer_id,
           const SCT_ID* m_sctId,
           const int samplesize,
           const std::string& defectType,
           const float threshold,
           const std::string& defectList ) const;

  StatusCode createListEff( const Identifier& wafer_id,
          const SCT_ID* m_sctId,
          const int samplesize,
          const float eff ) const;
  
  StatusCode createListNO( const Identifier& wafer_id,
         const SCT_ID* m_sctId,
         const int samplesize,
         const float noise_occ ) const;

  StatusCode createListRawOccu( const Identifier& wafer_id,
        const SCT_ID* m_sctId,
        const int samplesize,
        const float raw_occu ) const;

  StatusCode createListBSErr(const Identifier& wafer_id,
	const SCT_ID* m_sctId,
	const int samplesize,
        const std::string & errorList,
        const std::string & probList) const ;
    
    //StatusCode createListBSErr( const Identifier& wafer_id,
    //        const unsigned long long n_events,
    //        const std::string& BSErrorType,
    //        const std::string& Ratio ) const;
    
  //  StatusCode createListLA(const Identifier& wafer_id,const SCT_ID* m_sctId,const int samplesize, const int module, const float lorentz, const float MCW ) const;
  StatusCode createListLA(const Identifier& wafer_id,const SCT_ID* m_sctId,const int samplesize, const int module, const float lorentz, const float err_lorentz, const float chisq, const float fitParam_a, const float err_a, const float fitParam_b, const float err_b, const float fitParam_sigma, const float err_sigma, const float MCW, const float err_MCW ) const;
/*  StatusCode createListLA( const Identifier& wafer_id,
         const SCT_ID* m_sctId,
         const int samplesize,
         const float lorentz,
         const float MCW ) const;
*/
  StatusCode wrapUpNoisyChannel();
  StatusCode wrapUpDeadStrips();
  StatusCode wrapUpDeadChips();
  StatusCode wrapUpEfficiency();
  StatusCode wrapUpNoiseOccupancy();
  StatusCode wrapUpRawOccupancy();
  StatusCode wrapUpBSErrors();
  StatusCode wrapUpLorentzAngle();

private:
  StoreGateSvc* m_detStore;
  SG::ReadHandleKey<EventInfo> m_eventInfoKey;

  StatusCode registerIOV(const CLID& clid);
  int stringToInt(const std::string& s) const;
  
  StatusCode streamOutCondObjects(const std::string & foldername);
  StatusCode streamOutCondObjectsWithErrMsg(const std::string & foldername);

  StatusCode registerCondObjects(const std::string & foldername,const std::string & tagname);
  StatusCode recordAndStream(const CondAttrListCollection* pCollection,const std::string & foldername, bool & flag);
  StatusCode registerCondObjectsWithErrMsg(const std::string& foldername,const std::string& tagname);
  //
  coral::AttributeListSpecification* createBasicDbSpec(const bool capsFormat) const;
  void setBasicValues(coral::AttributeList & attrList, const Identifier& wafer_id, const int samplesize,const SCT_ID* m_sctId, const bool capsFormat) const;
  // small helper function
  unsigned int computeIstrip4moncond(const Identifier& elementId) const;
  
  const CondAttrListCollection* getAttrListCollectionByFolder(const std::string&) const;
  
  // would it make sense to change the strings to properties?
  // that would be a fairly simple fix
  static std::string s_separator;
  static std::string s_defectFolderName;
  static std::string s_deadStripFolderName;
  static std::string s_deadChipFolderName;
  static std::string s_effFolderName;
  static std::string s_noFolderName;
  static std::string s_RawOccuFolderName;
  static std::string s_BSErrFolderName;
  static std::string s_LAFolderName;
  // cache for the Collections, access by foldername
  mutable std::map<const std::string, const CondAttrListCollection*>  m_attrListCollectionMap;
  CondAttrListCollection*      m_attrListColl;
  CondAttrListCollection*      m_attrListColl_deadStrip;
  CondAttrListCollection*      m_attrListColl_deadChip;
  CondAttrListCollection*      m_attrListColl_eff;
  CondAttrListCollection*      m_attrListColl_no;
  CondAttrListCollection*      m_attrListColl_RawOccu;
  CondAttrListCollection*      m_attrListColl_BSErr;
  CondAttrListCollection*      m_attrListColl_LA;
  BooleanProperty              m_writeCondObjs;
  BooleanProperty              m_regIOV;
  BooleanProperty              m_readWriteCool;
  BooleanProperty              m_twoStepWriteReg;
  BooleanProperty              m_manualiov;
  /*IntegerProperty              m_regTime;*/
  IntegerProperty              m_version;
  /*IntegerProperty              m_nhits_noisychip;*/
  /*IntegerProperty              m_nhits_noisywafer;*/
  /*IntegerProperty              m_nhits_noisymodule;*/
  IntegerProperty      m_beginRun;
  IntegerProperty      m_endRun;
  StringProperty               m_streamName;
  std::string                  m_tagID4NoisyStrips;
  std::string                  m_tagID4DeadStrips;
  std::string                  m_tagID4DeadChips;
  std::string                  m_tagID4Efficiency;
  std::string                  m_tagID4NoiseOccupancy;
  std::string                  m_tagID4RawOccupancy;
  std::string                  m_tagID4BSErrors;
  std::string                  m_tagID4LorentzAngle;


  IIOVRegistrationSvc*         m_regSvc;
  //IAthenaOutputStreamTool*     m_streamer;
  ToolHandle<IAthenaOutputStreamTool> m_streamer;

  //StringArrayProperty          m_badElements; unused list of bad detector elements (= module sides)
  std::set<Identifier>         m_badIds;
  bool                         m_defectRecorded;
  bool                         m_deadStripRecorded;
  bool                         m_deadChipRecorded;
  bool                         m_effRecorded;
  bool                         m_noRecorded;
  bool                         m_RawOccuRecorded;
  bool                         m_BSErrRecorded;
  bool                         m_LARecorded;
  const SCT_ID*                m_pHelper;
  // FIXME: this caches only the last call of getList.
  // creating a hash of _all_ calls may be faster, but wastes a lot of memory
  mutable std::string          m_currentDefectList;
};

#endif // SCTCalibWriteSvc.h
