/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "LArTools/LArCablingService.h"
#include "LArTools/LArSuperCellCablingTool.h"

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
      pParent=myParent; 
      if ( m_isSC ) {
           ToolHandle<LArSuperCellCablingTool> lscct;
           CHECK( lscct.retrieve() );
           LArSuperCellCablingTool* plscct = &(*lscct);
           m_larCablingSvc = (LArCablingBase*) plscct;
      } else {
           ToolHandle<LArCablingService> lcS;
           CHECK( lcS.retrieve() );
           LArCablingService* plcS = &(*lcS);
           m_larCablingSvc = (LArCablingBase*)plcS;
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
  
  LArRawChannelBuilderParams *pParent;
  
  LArRawChannelBuilderStatistics *helper;  
  
  StoreGateSvc*  m_detStore;
  
  LArCablingBase* m_larCablingSvc;

  bool m_isSC;
};

#endif

