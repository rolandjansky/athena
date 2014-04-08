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

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPoolKernel/IMetaDataTool.h"
#include "LumiBlockComps/ILumiBlockMetaDataTool.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockRange_p1.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p1.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "TTree.h"
#include "TString.h"

#include <vector>
#include <string>

class StoreGateSvc;
class ILumiCalcSvc;
class IGoodRunsListSelectorTool;
class LumiBlockCollectionConverter;
class ITriggerRegistryTool;

namespace Root {
  class TGRLCollection;
}

class LumiBlockMetaDataTool : public AlgTool, virtual public IMetaDataTool, virtual public ILumiBlockMetaDataTool, virtual public IIncidentListener {
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

   /// Incident service handle listening for BeginFile and EndFile.
   void handle(const Incident& incident);

   /// functions from ILumiBlockMetaDataTool
   inline const Root::TGRLCollection* getGRLCollection() const { return m_grlcollection; }
   inline const TString& getUniqueGRLString() const { return m_grlxmlstring; }
   const TString getGRLString( const TString& grlname ) const;

private:

   /// Fill metaDataStore and ntuples
   void finishUp();

   /// Functions to filter out LBs with bad DQ flags. Passes new lbc that you own.
   LumiBlockCollection* FilterOnDQFlags( const LumiBlockCollection& lbc,
                                         const std::vector<std::string>& grlnameVec=std::vector<std::string>(),
                                         const std::vector<std::string>& brlnameVec=std::vector<std::string>() ) ;
   /// record lbc in metadatastore
   StatusCode RecordInMetaDataStore( LumiBlockCollection* plbc, const TString& lbcname ) ;
   /// loop over grlselector registrations, and add dq-approved lbc to grlcollection
   StatusCode AddDQCollections( const LumiBlockCollection& lbc, const TString& lbcprefix, const TString& grlprefix="" ) ;
   /// make copy of lb collection
   LumiBlockCollection* GetCopyOfCollection( const LumiBlockCollection& lbc ); 

   StatusCode fillFromXML( LumiBlockCollection* lbc_target, const std::string& xmlString );

   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   StoreGateSvc_t m_pMetaDataStore;
   StoreGateSvc_t m_pInputStore;
   StoreGateSvc_t m_tagDataStore;
   LumiBlockCollection*  m_cacheLBColl;
   LumiBlockCollection*  m_unfinishedLBColl;
   LumiBlockCollection*  m_tempLBColl;
   StringProperty  m_LBColl_name;
   StringProperty  m_unfinishedLBColl_name;
   std::string m_version;
   TString m_DQLBColl_name;
   TString m_unfinishedDQLBColl_name;
   TString m_grlxmlstring;

   int m_nfiles;
   bool m_fileCurrentlyOpened;
   bool m_createAANT;
   bool m_calcLumi;
   bool m_storexmlfiles;
   bool m_applydqcuts;

   TTree *AANtree;
   ITHistSvc * tHistSvc;

   LumiBlockCollectionCnv_p1 m_lbc_conv;
   LumiBlockRangeCnv_p1 m_lbr_conv;

   LumiBlockCollection_p1 *m_lbc;
   LumiBlockRange_p1 *m_lbr;

   LumiBlockCollectionConverter* m_converter;
   Root::TGRLCollection* m_grlcollection;

   ServiceHandle<ILumiCalcSvc> m_lcSvc;
   ToolHandle< IGoodRunsListSelectorTool > m_GoodRunsListSelectorTool;
   ToolHandle< ITriggerRegistryTool > m_TriggerRegistryTool;

};


#endif
