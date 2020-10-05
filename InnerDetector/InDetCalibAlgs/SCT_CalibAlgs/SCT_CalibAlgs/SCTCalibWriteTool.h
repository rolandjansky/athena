/** -*- c++ -*- */

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/IOVTime.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "EventInfo/EventInfo.h"
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "StoreGate/ReadHandleKey.h"

#include "CoralBase/AttributeListSpecification.h"

// Gaudi includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

//STL
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

//forward declarations
class IdentifierHash;
class SCT_ID;
class IIOVRegistrationSvc;
class IAthenaOutputStreamTool;

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
      static const InterfaceID& interfaceID();
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

      std::string
      addDefect(const std::string& defectlist,const int defectBeginChannel,const int defectEndChannel) const;

      std::string
      addNumber(const std::string numStr,const unsigned long long number) const;

      std::string
      createDefectString(const int defectBeginChannel,const int defectEndChannel) const;

     StatusCode createCondObjects ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                                 (const Identifier& wafer_id,
                                  const SCT_ID* m_sctId,
                                  const int samplesize,
                                  const std::string& defectType,
                                  const float threshold,
                                  const std::string& defectList) const;

      StatusCode createListStrip ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                                (const Identifier& wafer_id,
                                 const SCT_ID* m_sctId,
                                 const int samplesize,
                                 const std::string& defectType,
                                 const float threshold,
                                 const std::string& defectList) const;

      StatusCode createListChip ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                               (const Identifier& wafer_id,
                                const SCT_ID* m_sctId,
                                const int samplesize,
                                const std::string& defectType,
                                const float threshold,
                                const std::string& defectList) const;

      StatusCode createListEff ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                              (const Identifier& wafer_id,
                               const SCT_ID* m_sctId,
                               const int samplesize,
                               const float eff) const;

      StatusCode createListNO ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                             (const Identifier& wafer_id,
                              const SCT_ID* m_sctId,
                              const int samplesize,
                              const float noise_occ) const;

      StatusCode createListRawOccu ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                                  (const Identifier& wafer_id,
                                   const SCT_ID* m_sctId,
                                   const int samplesize,
                                   const float raw_occu) const;

      StatusCode createListBSErr ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                                (const Identifier& wafer_id,
                                 const SCT_ID* m_sctId,
                                 const int samplesize,
                                 const std::string& errorList,
                                 const std::string& probList) const ;

      StatusCode createListLA ATLAS_NOT_THREAD_SAFE // Thread unsafe CondAttrListCollection::add is used.
                             (const Identifier& wafer_id,
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
                              const float err_MCW) const;

      StatusCode wrapUpNoisyChannel();
      StatusCode wrapUpDeadStrips();
      StatusCode wrapUpDeadChips();
      StatusCode wrapUpEfficiency();
      StatusCode wrapUpNoiseOccupancy();
      StatusCode wrapUpRawOccupancy();
      StatusCode wrapUpBSErrors();
      StatusCode wrapUpLorentzAngle();

   private:
      SG::ReadHandleKey<EventInfo> m_eventInfoKey{this, "EventInfo", "ByteStreamEventInfo"};

      int stringToInt(const std::string& s) const;

      StatusCode streamOutCondObjects(const std::string& foldername);
      StatusCode streamOutCondObjectsWithErrMsg(const std::string& foldername);

      StatusCode registerCondObjects(const std::string& foldername,const std::string& tagname) const;
      StatusCode recordAndStream(const CondAttrListCollection* pCollection,const std::string& foldername, bool& flag);
      StatusCode registerCondObjectsWithErrMsg(const std::string& foldername,const std::string& tagname) const;

      coral::AttributeListSpecification* createBasicDbSpec(const bool capsFormat) const;
      void setBasicValues(coral::AttributeList& attrList, const Identifier& wafer_id, const int samplesize,const SCT_ID* m_sctId, const bool capsFormat) const;
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
      mutable std::mutex m_mutex{};
      mutable std::map<const std::string, const CondAttrListCollection*>  m_attrListCollectionMap ATLAS_THREAD_SAFE; // Guarded by m_mutex
      std::unique_ptr<CondAttrListCollection> m_attrListColl;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_deadStrip;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_deadChip;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_eff;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_no;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_RawOccu;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_BSErr;
      std::unique_ptr<CondAttrListCollection> m_attrListColl_LA;
      BooleanProperty              m_writeCondObjs{this, "WriteCondObjs", true};
      BooleanProperty              m_regIOV{this, "RegisterIOV", true};
      BooleanProperty              m_readWriteCool{this, "ReadWriteCool", true};
      BooleanProperty              m_twoStepWriteReg{this, "TwoStepWriteReg", false};
      BooleanProperty              m_manualiov{this, "ManualIOV", true};
      IntegerProperty              m_version{this, "Version", 0};
      IntegerProperty              m_beginRun{this, "BeginRun", IOVTime::MINRUN};
      IntegerProperty              m_endRun{this, "EndRun", IOVTime::MAXRUN};
      StringProperty               m_streamName{this, "StreamName", "CondStreamTest"};
      StringProperty               m_tagID4NoisyStrips{this, "TagID4NoisyStrips", ""};
      StringProperty               m_tagID4DeadStrips{this, "TagID4DeadStrips", ""};
      StringProperty               m_tagID4DeadChips{this, "TagID4DeadChips", ""};
      StringProperty               m_tagID4Efficiency{this, "TagID4Efficiency", ""};
      StringProperty               m_tagID4NoiseOccupancy{this, "TagID4NoiseOccupancy", ""};
      StringProperty               m_tagID4RawOccupancy{this, "TagID4RawOccupancy", ""};
      StringProperty               m_tagID4BSErrors{this, "TagID4BSErrors", ""};
      StringProperty               m_tagID4LorentzAngle{this, "TagID4LorentzAngle", ""};

      IIOVRegistrationSvc*         m_regSvc{nullptr};
      IAthenaOutputStreamTool*     m_streamer{nullptr};

      bool                         m_defectRecorded{false};
      bool                         m_deadStripRecorded{false};
      bool                         m_deadChipRecorded{false};
      bool                         m_effRecorded{false};
      bool                         m_noRecorded{false};
      bool                         m_RawOccuRecorded{false};
      bool                         m_BSErrRecorded{false};
      bool                         m_LARecorded{false};
      const SCT_ID*                m_pHelper{nullptr};
};

inline const InterfaceID& SCTCalibWriteTool::interfaceID() {
   static const InterfaceID IID{"SCTCalibWriteTool", 1, 0};
   return IID;
}

#endif // SCTCalibWriteTool.h
