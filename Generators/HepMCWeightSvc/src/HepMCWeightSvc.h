/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVGENPRODTOOLS_HEPMCWEIGHTSVC_H
#define EVGENPRODTOOLS_HEPMCWEIGHTSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GenInterfaces/IHepMCWeightSvc.h"
#include <string>

#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/ToolHandle.h"

/// @brief Service to read/write HepMC's WeightContainer key names from/to IOVMetaDataContainers
///  author: will buttinger , NLAA
/// 
///
class HepMCWeightSvc : public AthService , public IHepMCWeightSvc, virtual public IIncidentListener  {
public:



  // Constructor and destructor
  HepMCWeightSvc(const std::string& name, ISvcLocator* pSvcLocator);

  ~HepMCWeightSvc() {;}

  ///checks for any changes to weightnames ... none are allowed. Then records to metadata
  virtual StatusCode setWeightNames(const std::map<std::string, std::size_t>& weightNames);

   ///returns the current weight names ... will only change at file boudaries, it is assumed all events in a given file will have same weights in SAME ORDER
   virtual const std::map<std::string, std::size_t>& weightNames();


   virtual StatusCode initialize();

   virtual void handle(const Incident& inc); //listen for beginInputFile and load the weights at that point 

  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) {
      if(IID_IHepMCWeightSvc == riid) {
         *ppvInterface = dynamic_cast<IHepMCWeightSvc*>(this);
         return StatusCode::SUCCESS;
      }
      return AthService::queryInterface(riid,ppvInterface);
   }

private:
   bool weightNamesChanged(const std::map<std::string, std::size_t>& weightNames);

   StatusCode loadWeights(); //reads from the IOVMetaData

   std::map<std::string, std::size_t> m_currentWeightNames;

   ToolHandle<IIOVDbMetaDataTool> m_metaDataTool;

   bool m_weightNamesLoaded;

};

#endif
