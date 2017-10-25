/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKMETATOOL_H
#define LUMIBLOCKMETATOOL_H

/** Modified from @file ReadMeta.h
 *  @brief This file contains the class definition for the ReadMeta class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: LumiBlockMetaDataTool.h,v 1.4 2009-05-19 07:51:28 radbal Exp $
 **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPoolKernel/IMetaDataTool.h"
#include "LumiBlockComps/ILumiBlockMetaDataTool.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "TTree.h"
#include "TString.h"

#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"

#include <vector>
#include <string>

class StoreGateSvc;
class IGoodRunsListSelectorTool;
//class LumiBlockRangeContainerConverter;


namespace Root {
  class TGRLCollection;
}

//class LumiBlockMetaDataTool : public AthAlgTool, virtual public IMetaDataTool, virtual public ILumiBlockMetaDataTool, virtual public IIncidentListener {
class LumiBlockMetaDataTool : public AthAlgTool, virtual public IMetaDataTool, virtual public ILumiBlockMetaDataTool {
public: // Constructor and Destructor
   /// Standard Service Constructor
   LumiBlockMetaDataTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~LumiBlockMetaDataTool();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();
   // StatusCode stop();

   /// Function collecting the metadata from a new input file
   virtual StatusCode beginInputFile();

   /// Function collecting the metadata from a new input file
   virtual StatusCode endInputFile();

   /// Function writing the collected metadata to the output
   virtual StatusCode metaDataStop();

   /// Incident service handle listening for BeginFile and EndFile.
   //void handle(const Incident& incident);

   /// functions from ILumiBlockMetaDataTool
   inline const Root::TGRLCollection* getGRLCollection() const { return m_grlcollection; }
   inline const TString& getUniqueGRLString() const { return m_grlxmlstring; }
   const TString getGRLString( const TString& grlname ) const;

private:

   /// Fill metaDataStore and ntuples
   StatusCode finishUp();

   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   StoreGateSvc_t m_pMetaDataStore;
   StoreGateSvc_t m_pInputStore;
   StoreGateSvc_t m_tagDataStore;


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
   std::string m_version;
   TString m_DQLBColl_name;
   TString m_unfinishedDQLBColl_name;
   TString m_grlxmlstring;

   int m_nfiles;
   bool m_fileCurrentlyOpened;
   std::string m_CurrentFileName;
   bool m_calcLumi;
   bool m_storexmlfiles;
   bool m_applydqcuts;
   /*LumiBlockRangeContainerConverter* m_converter;*/
   Root::TGRLCollection* m_grlcollection;

   ToolHandle< IGoodRunsListSelectorTool > m_GoodRunsListSelectorTool;

};


#endif
