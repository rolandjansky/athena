/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear eamcs, this is -*-c++-*-as

/** 
 * @class LArRawChannelBuilderToolBaseClass
 * @author Rolf Seuster
 * @brief Base tool called by LArRawChannelBuilderDriver
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLBASECLASS_H
#define LARROD_LARRAWCHANNELBUILDERTOOLBASECLASS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/ToolHandle.h"

#include "LArROD/ILArRawChannelBuilderToolBaseClass.h"

#include "LArROD/LArRawChannelBuilderParams.h"
#include "LArROD/LArRawChannelBuilderStatistics.h"

#include "StoreGate/StoreGateSvc.h"

#include "LArCabling/LArOnOffIdMapping.h"

class LArRawChannelBuilderToolBaseClass : public AthAlgTool,
					  public virtual ILArRawChannelBuilderToolBaseClass
{
 public:
  
  LArRawChannelBuilderToolBaseClass(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);


  ~LArRawChannelBuilderToolBaseClass();
  

  
  StatusCode initToolHidden(LArRawChannelBuilderParams *myParent)
    {
      m_parent=myParent; 
      if ( m_isSC ) {
         ATH_CHECK( m_cablingKeySC.initialize() );
      } else {
         ATH_CHECK( m_cablingKey.initialize() );
      }
      return initTool();
    };
  
  virtual StatusCode initTool()
    { return StatusCode::SUCCESS; };
  
  virtual void initEventHidden() = 0;
  
  virtual void initEvent()            { };
  
  virtual void finalEventHidden();
  
  virtual void finalEvent()            { };
  
  virtual void printSummary();
  
  //template<typename T>
  //  StatusCode regHandleDetectorStore(const T& object, const std::string& key);
  
  //template<typename T>
  //  StatusCode retrieveDetectorStore(const T*& object, const std::string& key);
  
  Identifier currentID();
  
 protected:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"SCCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};
  
  LArRawChannelBuilderParams *m_parent;
  
  LArRawChannelBuilderStatistics *m_helper;  
  
  StoreGateSvc*  m_detStore;
  
  bool m_isSC;
};

#endif

