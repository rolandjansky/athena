/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKCOMPS_LUMIBLOCKMETATOOL_H
#define LUMIBLOCKCOMPS_LUMIBLOCKMETATOOL_H

/**
 * @class LumiBlockMetaDataTool
 *
 * @brief Declaration of the LumiBlockMetaDataTool class
 *
 * @author Peter van Gemmeren <gemmeren@anl.gov>
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IMetaDataTool.h"

#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"

#include <vector>
#include <string>

class StoreGateSvc;

class LumiBlockMetaDataTool 
     : public AthAlgTool
     , virtual public IMetaDataTool {

 public: 
   LumiBlockMetaDataTool(const std::string& type
			 , const std::string& name
			 , const IInterface* parent);

   virtual ~LumiBlockMetaDataTool();

 public:
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Function collecting the metadata from a new input file
   virtual StatusCode beginInputFile(const SG::SourceID&) override;

   /// Function collecting the metadata from a new input file
   virtual StatusCode endInputFile(const SG::SourceID&) override;

   /// Function writing the collected metadata to the output
   virtual StatusCode metaDataStop() override;

 private:
   /// Fill metaDataStore and ntuples
   StatusCode finishUp();

   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   StoreGateSvc_t m_pMetaDataStore;
   StoreGateSvc_t m_pInputStore;

   // The m_cacheInputRangeContainer stores the LumiBlockRange info for files that are open
   //   We need to keep suspect lumiblocks separated from complete and incomplete ones
   //   since something that comes in as suspect always stays suspect
   xAOD::LumiBlockRangeContainer* m_cacheInputRangeContainer;
   xAOD::LumiBlockRangeAuxContainer* m_cacheInputRangeAuxContainer;
   xAOD::LumiBlockRangeContainer* m_cacheSuspectInputRangeContainer;
   xAOD::LumiBlockRangeAuxContainer* m_cacheSuspectInputRangeAuxContainer;

   // The m_cacheOutputRangeContainer stores the LumiBlockRange info for files that have been read and closed
   xAOD::LumiBlockRangeContainer* m_cacheOutputRangeContainer;
   xAOD::LumiBlockRangeAuxContainer* m_cacheOutputRangeAuxContainer;
   xAOD::LumiBlockRangeContainer* m_cacheSuspectOutputRangeContainer;
   xAOD::LumiBlockRangeAuxContainer* m_cacheSuspectOutputRangeAuxContainer;

   std::string  m_LBColl_name;
   std::string  m_unfinishedLBColl_name;
   std::string  m_suspectLBColl_name;

   bool m_fileCurrentlyOpened;
   std::string m_CurrentFileName;
};


#endif
