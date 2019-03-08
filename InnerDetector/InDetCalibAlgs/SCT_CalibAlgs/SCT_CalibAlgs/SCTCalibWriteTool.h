/** -*- c++ -*- */

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
* @file SCTCalibWriteTool.h
*
* @brief Header file for the SCTCalibWriteTool class
*
* @author Jose E. Garcia
**/

#ifndef SCTCalibWriteTool_H
#define SCTCalibWriteTool_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "EventInfo/EventInfo.h"
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/StoreGate.h"

#include "CoralBase/AttributeListSpecification.h"

// Gaudi includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

//STL
#include <string>
#include <map>
#include <set>
#include <list>
#include <mutex>

//forward declarations
class IdentifierHash;
class SCT_ID;
class IIOVRegistrationSvc;
class IAthenaOutputStreamTool;
class CondAttrListCollection;
class StoreGateSvc;

/**
** Algorithm to test writing conditions data and reading them back.
**/
class SCTCalibWriteTool : public AthAlgTool {

   public:
      // Constructor
      SCTCalibWriteTool(const std::string& type, const std::string& name, const IInterface* parent);
      // Destructor
      virtual ~SCTCalibWriteTool() = default;

      // overloading functions
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      static const InterfaceID & interfaceID();
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

      StatusCode createListLA(const Identifier& wafer_id,
                              const SCT_ID* m_sctId,
                              const int samplesize,
                              const int module,
                              const float lorentz,
                              const float err_lorentz,
                              const float chisq,
                              const float fitParam_a,
                              const float err_a,
                              const float fitParam_b,
                              const float err_b,
                              const float fitParam_sigma,
                              const float err_sigma,
                              const float MCW,
                              const float err_MCW ) const;

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

      coral::AttributeListSpecification* createBasicDbSpec(const bool capsFormat) const;
      void setBasicValues(coral::AttributeList & attrList, const Identifier& wafer_id, const int samplesize,const SCT_ID* m_sctId, const bool capsFormat) const;
      // small helper function
      unsigned int computeIstrip4moncond(const Identifier& elementId) const;

      const CondAttrListCollection* getAttrListCollectionByFolder(const std::string&) const;

      // would it make sense to change the strings to properties?
      // that would be a fairly simple fix
      static const std::string s_separator;
      static const std::string s_defectFolderName;
      static const std::string s_deadStripFolderName;
      static const std::string s_deadChipFolderName;
      static const std::string s_effFolderName;
      static const std::string s_noFolderName;
      static const std::string s_RawOccuFolderName;
      static const std::string s_BSErrFolderName;
      static const std::string s_LAFolderName;

      // cache for the Collections, access by foldername
      mutable std::mutex m_mutex;
      mutable std::map<const std::string, const CondAttrListCollection*>  m_attrListCollectionMap ATLAS_THREAD_SAFE; // Guarded by m_mutex
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
      IntegerProperty              m_beginRun;
      IntegerProperty              m_endRun;
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
      IAthenaOutputStreamTool*     m_streamer;
      //ToolHandle<IAthenaOutputStreamTool> m_streamer;

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
};

inline const InterfaceID & SCTCalibWriteTool::interfaceID() {
   static const InterfaceID IID("SCTCalibWriteTool",1,0);
   return IID;
}

#endif // SCTCalibWriteTool.h
