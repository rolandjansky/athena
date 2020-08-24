/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DSConfigSvc.cxx 742408 2016-04-23 18:55:57Z stelzer $

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaKernel/errorcheck.h"

#include "CoolKernel/Record.h"

#include "StoreGate/DataHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Trigger configuration include(s):
#include "TrigConfBase/TrigDBConnectionConfig.h"

#include "L1TopoConfig/L1TopoMenu.h"
#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/BunchGroupSet.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTStreamTag.h"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/TrigConfCoolFolderSpec.h"
#include "TrigConfStorage/TrigConfCoolL1PayloadConverters.h"
#include "TrigConfStorage/TrigConfCoolHLTPayloadConverters.h"
#include "TrigConfStorage/DBLoader.h"

// Local include(s):
#include "./DSConfigSvc.h"

#include "boost/lexical_cast.hpp"


using namespace std;
using namespace TrigConf;
using namespace TrigConfCoolL1PayloadConverters;
using namespace TrigConfCoolHLTPayloadConverters;

using boost::lexical_cast;

#define TRIGGER_CONF_HLTMENU  "/TRIGGER/HLT/Menu"
#define TRIGGER_CONF_HLTKEYS  "/TRIGGER/HLT/HltConfigKeys"
#define TRIGGER_CONF_HLTPS    "/TRIGGER/HLT/Prescales"
#define TRIGGER_CONF_HLTPSK   "/TRIGGER/HLT/PrescaleKey"
#define TRIGGER_CONF_LVL1KEY  "/TRIGGER/LVL1/Lvl1ConfigKey"
#define TRIGGER_CONF_LVL1MENU "/TRIGGER/LVL1/Menu"
#define TRIGGER_CONF_LVL1PS   "/TRIGGER/LVL1/Prescales"
#define TRIGGER_CONF_LVL1PIT  "/TRIGGER/LVL1/CTPCoreInputMapping"

// new additions not sure yet if I should make them available in ESD/AOD
#define TRIGGER_CONF_HLTGRP   "/TRIGGER/HLT/Groups"
#define TRIGGER_CONF_LVL1ITD  "/TRIGGER/LVL1/ItemDef"
#define TRIGGER_CONF_LVL1THR  "/TRIGGER/LVL1/Thresholds"

// new additions BG
// bunchgroup set key
#define TRIGGER_CONF_LVL1BGKEY   "/TRIGGER/LVL1/BunchGroupKey"
// description of the BG
#define TRIGGER_CONF_LVL1BGDESC  "/TRIGGER/LVL1/BunchGroupDescription"
// definition of the bunchgroup
#define TRIGGER_CONF_LVL1BGDEF   "/TRIGGER/LVL1/BunchGroupContent"

TrigConf::DSConfigSvc::DSConfigSvc( const std::string& name,
                                    ISvcLocator* pSvcLocator ) : 
   base_class(name, pSvcLocator),
   m_topoMenu(new TXC::L1TopoMenu),
   m_masterKey( 0 ),
   m_lvl1PsKey( 0 ),
   m_hltPsKey( 0 ),
   m_lvl1BgKey( 0 ),
   m_configSrc( "" ),
   m_detstore( "StoreGateSvc/DetectorStore", name ),
   m_folders( "" )
{
   m_configSourceString="dblookup";
   m_dbServer="TRIGGERDB";
   base_class::declareCommonProperties();
}


TrigConf::DSConfigSvc::~DSConfigSvc()
{}


StatusCode
TrigConf::DSConfigSvc::initialize() {

   CHECK(ConfigSvcBase::initialize());

   // get detector store
   CHECK( m_detstore.retrieve() );

   ServiceHandle< IJobOptionsSvc > jobOptionsSvc( "JobOptionsSvc", name() );
   if( jobOptionsSvc.retrieve().isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve JobOptionsSvc" );
   } else {
      const Gaudi::Details::PropertyBase* p =
         Gaudi::Utils::getProperty( jobOptionsSvc->getProperties( "IOVDbSvc" ), "Folders" );   
      if( p ) m_folders = p->toString();
      ATH_MSG_DEBUG( "The string 'folders' is: " << m_folders );
   }

   const bool multichannel  = true;
   const bool singlechannel = false;

   CHECK( registerCallbackForFolder( TRIGGER_CONF_HLTMENU,    multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_HLTKEYS,    singlechannel ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_HLTPS,      multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_HLTPSK,     singlechannel ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1KEY,    singlechannel ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1MENU,   multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1PS,     multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_HLTGRP,     multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1ITD,    multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1THR,    multichannel  ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1BGKEY,  singlechannel ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1BGDESC, singlechannel ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1BGDEF,  singlechannel ) );
   CHECK( registerCallbackForFolder( TRIGGER_CONF_LVL1PIT,    multichannel  ) );

   ATH_MSG_INFO( "Successfully initalized DSConfigSvc" );

   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::DSConfigSvc::queryInterface( const InterfaceID& riid,
                                       void** ppvIF ) {

   if( riid == ITrigConfigSvc::interfaceID() ) {
      *ppvIF = static_cast< ITrigConfigSvc* >( this );
   } else if( riid == IProperty::interfaceID() ) {
      *ppvIF = static_cast< IProperty* >( this );
   } else if ( riid == IL1TopoConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< IL1TopoConfigSvc* >(this);
   } else if( riid == ILVL1ConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< ILVL1ConfigSvc* >( this );
   } else if( riid == IHLTConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< IHLTConfigSvc* >( this );
   } else {
      return AthService::queryInterface( riid, ppvIF );
   }

   addRef();
   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::DSConfigSvc::registerCallbackForFolder( const std::string& foldername,
                                                  bool multichannel ) {

   if( ! hasFolder( foldername ) ) return StatusCode::SUCCESS;

   const bool triggerUpdateOnRegistration = true;

   if( multichannel ) {
      const DataHandle< CondAttrListCollection > aptr;
      CHECK( m_detstore->regFcn( &DSConfigSvc::update, this, aptr,
                                 foldername, triggerUpdateOnRegistration ) );
   } else {
      const DataHandle< AthenaAttributeList > aptr;
      CHECK( m_detstore->regFcn( &DSConfigSvc::update, this, aptr,
                                 foldername, triggerUpdateOnRegistration ) );
   }
   ATH_MSG_DEBUG( "Registered callback for folder " << foldername );

   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::DSConfigSvc::update( IOVSVC_CALLBACK_ARGS_K( keys ) ) {

   // callback function to update beamspot cache when condDB data changes
   //I = 0; // not used, suppress compiler warnings

   bool update_HLTMENU  = false;  // /TRIGGER/HLT/Menu
   bool update_HLTKEYS  = false;  // /TRIGGER/HLT/HltConfigKeys
   bool update_HLTPS    = false;  // /TRIGGER/HLT/Prescales
   bool update_HLTPSK   = false;  // /TRIGGER/HLT/PrescaleKey
   bool update_LVL1KEY  = false;  // /TRIGGER/LVL1/Lvl1ConfigKey
   bool update_LVL1MENU = false;  // /TRIGGER/LVL1/Menu
   bool update_LVL1PS   = false;  // /TRIGGER/LVL1/Prescales
   bool update_LVL1PIT  = false;  // /TRIGGER/LVL1/CTPCoreInputMapping
   bool update_HLTGRP   = false;  // /TRIGGER/HLT/Groups
   bool update_LVL1ITD  = false;  // /TRIGGER/LVL1/ItemDef
   bool update_LVL1THR  = false;  // /TRIGGER/LVL1/Thresholds
   bool update_LVL1BGK  = false;  // /TRIGGER/LVL1/BunchGroupKey
   bool update_LVL1BGD  = false;  // /TRIGGER/LVL1/BunchGroupDescription
   bool update_LVL1BGC  = false;  // /TRIGGER/LVL1/BunchGroupContent

   ATH_MSG_INFO( "update callback invoked for " << keys.size() << " keys: " << keys );

   // Decide which folders were updated:
   for( string & key : keys ) {
      if     ( key == TRIGGER_CONF_HLTMENU    ) update_HLTMENU  = true;
      else if( key == TRIGGER_CONF_HLTKEYS    ) update_HLTKEYS  = true;
      else if( key == TRIGGER_CONF_HLTPS      ) update_HLTPS    = true;
      else if( key == TRIGGER_CONF_HLTPSK     ) update_HLTPSK   = true;
      else if( key == TRIGGER_CONF_LVL1KEY    ) update_LVL1KEY  = true;
      else if( key == TRIGGER_CONF_LVL1MENU   ) update_LVL1MENU = true;
      else if( key == TRIGGER_CONF_LVL1PS     ) update_LVL1PS   = true;
      else if( key == TRIGGER_CONF_LVL1PIT    ) update_LVL1PIT  = true;
      else if( key == TRIGGER_CONF_HLTGRP     ) update_HLTGRP   = true;
      else if( key == TRIGGER_CONF_LVL1ITD    ) update_LVL1ITD  = true;
      else if( key == TRIGGER_CONF_LVL1THR    ) update_LVL1THR  = true;
      else if( key == TRIGGER_CONF_LVL1BGKEY  ) update_LVL1BGK  = true;
      else if( key == TRIGGER_CONF_LVL1BGDESC ) update_LVL1BGD  = true;
      else if( key == TRIGGER_CONF_LVL1BGDEF  ) update_LVL1BGC  = true;
   }

   bool updated_SMK      = false;
   bool updated_LVL1PSK  = false;
   bool updated_HLTPSK   = false;
   bool updated_LVL1BG   = false;

   // read the runwise configuration keys (master + HLT prescales)
   if( update_HLTKEYS ) {
      ATH_MSG_INFO( "Updating trigger configuration: HLT keys" );
      const AthenaAttributeList* keysAttrList = nullptr;
      CHECK( m_detstore->retrieve( keysAttrList, TRIGGER_CONF_HLTKEYS ) );
      uint32_t old_masterkey = m_masterKey;
      readHltConfigKeys( *keysAttrList, m_masterKey, m_hltPsKey, m_configSrc );
      ATH_MSG_INFO( "  Configuration key : " << m_masterKey );
      ATH_MSG_INFO( "  Run-bound HLT prescale key (old style): " << m_hltPsKey );
      ATH_MSG_INFO( "  Origin            : " << m_configSrc );
      updated_SMK = ( old_masterkey != m_masterKey );
      if( updated_SMK ) {
         ATH_MSG_INFO( "  Changed from " << old_masterkey << " to " << m_masterKey );
      }
      if( (m_configSrc.find("TRIGGERDBREPR") == 0) || 
          (m_configSrc.find("TRIGGERDBMC") == 0) || 
          (m_configSrc.find("TRIGGERDBATN") == 0) ) {
          /* this is for reprocessing or MC runs
           * in this case we connect to the specified TriggerDB
           * 
           * For data we use what is specified through job options (TRIGGERDB or TRIGGERDB_RUN1)
           */
          m_dbconfig->m_server = m_configSrc;
      }
   }


   // read the lb-wise configuration keys (LVL1 prescale key)
   if( update_LVL1KEY ) {
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 keys" );
      const AthenaAttributeList* keysAttrList = nullptr;
      CHECK( m_detstore->retrieve( keysAttrList, TRIGGER_CONF_LVL1KEY ) );
      unsigned int old_l1psk = m_lvl1PsKey;
      readLvl1ConfigKey( *keysAttrList, m_lvl1PsKey );
      ATH_MSG_INFO( "  LVL1 prescale key: " << m_lvl1PsKey );
      updated_LVL1PSK = (old_l1psk != m_lvl1PsKey);
   }


   // Bunchgroup key
   if( update_LVL1BGK ) {
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 bunchgroup keys" );
      const AthenaAttributeList* bgkcoll = nullptr;
      CHECK( m_detstore->retrieve( bgkcoll, TRIGGER_CONF_LVL1BGKEY ) );
      uint32_t old_lvl1BgKey = m_lvl1BgKey;
      m_lvl1BgKey = readLvl1BGKey( *bgkcoll );
      ATH_MSG_INFO( "  LVL1 bunchgroup key : " << m_lvl1BgKey );
      updated_LVL1BG = ( old_lvl1BgKey != m_lvl1BgKey );
   }


   // HLT PSK
   if( update_HLTPSK ) {
      ATH_MSG_INFO( "Updating trigger configuration: HLT prescale key and prescales" );
      const AthenaAttributeList* keysAL = nullptr;
      CHECK( m_detstore->retrieve( keysAL, TRIGGER_CONF_HLTPSK ) );
      unsigned int oldPsKey = m_hltPsKey;
      m_hltPsKey = readHltPrescaleKey( *keysAL );
      ATH_MSG_INFO( "  HLT prescale key : " << m_hltPsKey );
      updated_HLTPSK = (oldPsKey != m_hltPsKey);
   }


   /**
    * if we don't found a SMK in COOL then it is likely to be a MC job
    * configured from an XML file
    */

   if ( m_masterKey == 0 ) {
      // MC job from xml file

      // read the LVL1 Menu object
      if( update_LVL1MENU ) {
         const CondAttrListCollection* lvl1MenuAttrColl = nullptr;
         CHECK( m_detstore->retrieve(lvl1MenuAttrColl, TRIGGER_CONF_LVL1MENU ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 menu" );
         m_ctpConfig.menu().clear();
         CondAttrListCollection::size_type nCh = lvl1MenuAttrColl->size();
         CondAttrListCollection::const_iterator chIt = lvl1MenuAttrColl->begin();
         for( ; chIt != lvl1MenuAttrColl->end(); ++chIt ) {
            const CondAttrListCollection::ChanAttrListPair& idListPair = *chIt;
            // the attributeList for this channel (a channel corresponds to a triggerItem)
            CondAttrListCollection::ChanNum       ctpid   = idListPair.first;
            const CondAttrListCollection::AttributeList& atrList = idListPair.second;
            TrigConf::TriggerItem* ti = createLvl1TriggerItem( atrList );
            ti->setCtpId( ( int ) ctpid ); // ChanNum is a typedef of uint32
            m_ctpConfig.menu().addTriggerItem( ti ); // the menu takes the ownership of the TI
         }
         ATH_MSG_INFO( "  Number of items: " << nCh );
         if( msgLvl( MSG::DEBUG ) ) {
            m_ctpConfig.print();
         }
      }


      // read the LVL1 PITs
      if( update_LVL1PIT ) {
         const CondAttrListCollection* lvl1PitAttrColl = nullptr;
         CHECK( m_detstore->retrieve( lvl1PitAttrColl, TRIGGER_CONF_LVL1PIT ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 PITs" );
         CondAttrListCollection::size_type nCh = lvl1PitAttrColl->size();
         CondAttrListCollection::const_iterator chIt = lvl1PitAttrColl->begin();
         for( ; chIt != lvl1PitAttrColl->end(); ++chIt ) {
            const CondAttrListCollection::ChanAttrListPair& idListPair = *chIt;
            // the attributeList for this channel (a channel corresponds to a PIT)
            CondAttrListCollection::ChanNum       tipNumber = idListPair.first;
            const CondAttrListCollection::AttributeList& attrList   = idListPair.second;
            TrigConf::PIT* pit = readLvl1InputMap( attrList );
            TrigConf::TIP* tip = readLvl1TIPMap( attrList );
            pit->setPitNumber( ( int ) tipNumber );   // ChanNum is a typedef of uint32
            tip->setTipNumber( ( int ) tipNumber );
            tip->setClock(tipNumber%2);
            m_ctpConfig.menu().addPit( pit );  // the menu takes the ownership of the PIT and TIP
            m_ctpConfig.menu().addTip( tip );
         }
         ATH_MSG_INFO( "  Number of PITs: " << nCh );
         if( msgLvl( MSG::DEBUG ) ) {
            m_ctpConfig.print();
         }
      }


      // read the HLT Menu object
      if( update_HLTMENU ) {
         const CondAttrListCollection* hltMenuAttrColl = nullptr;
         CHECK( m_detstore->retrieve( hltMenuAttrColl, TRIGGER_CONF_HLTMENU ) );
         ATH_MSG_INFO( "Updating trigger configuration: HLT menu" );
         m_hltFrame.theHLTChainList().clear();
         m_hltFrame.theHLTSequenceList().clear();
         size_t nChains(0);
         for(CondAttrListCollection::const_iterator attrListIt = hltMenuAttrColl->begin(); attrListIt != hltMenuAttrColl->end(); attrListIt++ ) {
            const CondAttrListCollection::AttributeList& attrList = attrListIt->second;
            m_hltFrame.theHLTChainList().addHLTChain( createHLTChain( attrList, &m_hltFrame.theHLTSequenceList() ) );
            nChains++;
         }
         ATH_MSG_INFO( "  Number of chains: " << nChains );
         ATH_MSG_DEBUG( m_hltFrame.chains() );
      }


      if( update_HLTPS ) {
         const CondAttrListCollection* prescaleAL = nullptr;
         const CondAttrListCollection* hltMenuAttrColl = nullptr;
         CHECK( m_detstore->retrieve( prescaleAL, TRIGGER_CONF_HLTPS ) );
         CHECK( m_detstore->retrieve( hltMenuAttrColl, TRIGGER_CONF_HLTMENU ) );
         ATH_MSG_INFO( "Updating trigger configuration: HLT prescale key and prescales" );


         HLTPrescaleSet* pss = new HLTPrescaleSet();
         // set the HLT merged/not merged chains
         bool mergedHLT = false;
         CondAttrListCollection::const_iterator attrListIt = hltMenuAttrColl->begin();
         const CondAttrListCollection::AttributeList& attrList = attrListIt->second;
         if (attrList["ChainName"].data<cool::String255>().find("HLT") != std::string::npos) mergedHLT=true;
         ATH_MSG_INFO( "  HLT prescale key : " << m_hltPsKey );
         // set the prescale, pass-through and rerun-prescale values
         CondAttrListCollection::size_type nCh = prescaleAL->size();
         uint npositivepsL2( 0 );
         uint npositivepsEF( 0 );
         uint npositivepsHLT( 0 );
         CondAttrListCollection::const_iterator chIt = prescaleAL->begin();
         for( ; chIt != prescaleAL->end(); ++chIt ) {
            const CondAttrListCollection::ChanAttrListPair& idListPair = *chIt;
            // the attributeList for this channel (a channel corresponds to a triggerItem)
            CondAttrListCollection::ChanNum       ch      = idListPair.first;
            const CondAttrListCollection::AttributeList& attrList = idListPair.second;
            float ps, pt, rrps;
            readHltPrescale( attrList, ps, pt, rrps);
            TrigConf::HLTLevel level = (mergedHLT ? HLT :( ( ch % 2 ==0 ) ? L2 : EF));
            unsigned int cc = ch;
            if (! mergedHLT ) cc /= 2;
            else cc -= 20000;
            if( ( level == L2 ) && ( ps > 0.0 ) ) ++npositivepsL2;
            if( ( level == EF ) && ( ps > 0.0 ) ) ++npositivepsEF;
            if( ( level == HLT ) && ( ps > 0.0 ) ) ++npositivepsHLT;
            pss->thePrescale(cc,level).
               setPrescale(ps).
               setPassThrough(pt).
               setRerunPrescale("",rrps);
         }
         m_hltFrame.theHLTChainList().applyPrescaleSet(pss);
         ATH_MSG_INFO( "  Number of HLT prescales: " << nCh );
         ATH_MSG_INFO( "  Number of positive prescales at HLT: " << npositivepsHLT );
      }


      // read the lb-wise prescale set object
      if( update_LVL1PS ) {
         const CondAttrListCollection* lvl1PsAtrColl = nullptr;
         CHECK( m_detstore->retrieve( lvl1PsAtrColl, TRIGGER_CONF_LVL1PS ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 prescales" );
         CondAttrListCollection::size_type nCh = lvl1PsAtrColl->size();
         bool isRun2 = ( nCh == 512 );
         ATH_MSG_INFO( "  Number of prescales: " << nCh );
         if( nCh != 256 &&  nCh != 512 ) {
            //REPORT_MESSAGE( MSG::FATAL )
            ATH_MSG_FATAL( "Number of prescales is not 256 or 512, but " << nCh );
            return StatusCode::FAILURE;
         }
         TrigConf::PrescaleSet pss;
         pss.reset();
         pss.resize(nCh);
         int ps( 0 ); // the prescale value to be read
         for( CondAttrListCollection::ChanNum ch = 0; ch != nCh; ++ch ) {
            // the attributeList for this channel (a channel corresponds to a
            // single prescale, the channel id is the ctpid)
            const CondAttrListCollection::AttributeList& attrList =
               lvl1PsAtrColl->chanAttrListPair( ch )->second;
            readLvl1Prescale( attrList, ps );
            if( isRun2 ) {
               pss.setCut( ch, ps );
            } else {
               pss.setPrescale( ch, float(ps) );
            }
         }
         if( msgLvl( MSG::DEBUG ) ) {
            pss.print("",5);
         }
         m_ctpConfig.setPrescaleSet( pss ); // overwrites the old one
      }


      if( update_HLTGRP ) {
         const CondAttrListCollection* hltgrpAtrColl = nullptr;
         CHECK( m_detstore->retrieve( hltgrpAtrColl, TRIGGER_CONF_HLTGRP ) );
         ATH_MSG_INFO( "Updating trigger configuration: HLT chain groups" );
         size_t nGroups(0);
         for(CondAttrListCollection::const_iterator attrListIt = hltgrpAtrColl->begin(); attrListIt != hltgrpAtrColl->end(); attrListIt++ ) {
            const CondAttrListCollection::AttributeList& attrList = attrListIt->second;
            addGroupsToHltChain( attrList, m_hltFrame.theHLTChainList() );
            nGroups++;
         }
         ATH_MSG_INFO( "  Number of groups: " << nGroups );
      }


      if( update_LVL1THR ) {
         const CondAttrListCollection* l1thrAtrColl = nullptr;
         CHECK( m_detstore->retrieve( l1thrAtrColl, TRIGGER_CONF_LVL1THR ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 thresholds" );
         CondAttrListCollection::size_type nCh = l1thrAtrColl->size();
         ATH_MSG_INFO( "  Number of thresholds: " << nCh );
         vector<TriggerThreshold*> tmpThrVector;
         for( CondAttrListCollection::ChanNum ch = 0; ch != nCh; ++ch ) {
            const CondAttrListCollection::AttributeList& attrList = l1thrAtrColl->chanAttrListPair( ch )->second;
            tmpThrVector.push_back(createLvl1Threshold( attrList ) );
         }
         /**
          * a unpleasant hack so we can figure out if this Run1 (CTPVersion 3) or Run2 (CTPVersion 4)
          * we use the fact that in Run2 the cable names were different
          */
         bool isRun2 = false;
         for(TriggerThreshold * thr : tmpThrVector) {
            if( thr->cableName()=="EM1" || thr->cableName()=="EM2" || 
                thr->cableName()=="JET1" || thr->cableName()=="JET2" || 
                thr->cableName()=="TOPO1" || thr->cableName()=="TOPO2" || 
                thr->cableName()=="TAU1" || thr->cableName()=="TAU2") {
               isRun2 = true;
               break;
            }
         }
         m_ctpConfig.setCTPVersion(isRun2 ? 4 : 3);
         m_ctpConfig.setL1Version(isRun2 ? 1 : 0);
         L1DataDef::setMaxThresholdsFromL1Version( m_ctpConfig.l1Version() );
         ATH_MSG_INFO( "  Determined format " << (isRun2 ? "Run 2" : "Run 1") );
      
         float emEnergyScale = isRun2 ? 2 : 1;
         const_cast<CaloInfo&>(m_ctpConfig.menu().caloInfo()).setGlobalEmScale( emEnergyScale );
         ATH_MSG_INFO( "  Setting EM energy scale to " << emEnergyScale );

         for(TriggerThreshold * thr : tmpThrVector) {

            // special cases of JetWeights and METSigParams
            if( thr->name() == "METSigParams" ) {

               string pars = thr->cableName();
               std::vector< std::string > parv = TrigConf::split( pars, "," );
               int xsSigmaScale  = lexical_cast< int, std::string >( parv[ 0 ] );
               int xsSigmaOffset = lexical_cast< int, std::string >( parv[ 1 ] );
               int xeMin         = lexical_cast< int, std::string >( parv[ 2 ] );
               int xeMax         = lexical_cast< int, std::string >( parv[ 3 ] );
               int teSqrtMin     = lexical_cast< int, std::string >( parv[ 4 ] );
               int teSqrtMax     = lexical_cast< int, std::string >( parv[ 5 ] );
               TrigConf::CaloInfo& caloInfo =
                  const_cast< TrigConf::CaloInfo& >( thresholdConfig()->caloInfo() );
               caloInfo.metSigParam().setValues( xsSigmaScale, xsSigmaOffset,
                                                 xeMin, xeMax, teSqrtMin, teSqrtMax );
            } else if( thr->name() == "JetWeights" ) {
               if(!isRun2) { // only run 1 has JetWeights, for run2 this will crash
                  string jws = thr->cableName();
                  std::vector< std::string > jwv = TrigConf::split( jws, "," );
                  TrigConf::CaloInfo& caloInfo =
                     const_cast< TrigConf::CaloInfo& >( thresholdConfig()->caloInfo() );
                  for(const string& s: jwv)
                     caloInfo.addJetWeight( lexical_cast< int, std::string >( s ) );
               }
            } else {
               m_ctpConfig.menu().addTriggerThreshold( thr );
            }
         }
         m_ctpConfig.menu().thresholdConfig().attributeThresholdNumbers();
      }


      if( update_LVL1ITD ) {
         const CondAttrListCollection* l1itemdefAttrColl = nullptr;
         CHECK( m_detstore->retrieve( l1itemdefAttrColl, TRIGGER_CONF_LVL1ITD ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 item definition" );

         CondAttrListCollection::const_iterator chIt = l1itemdefAttrColl->begin();
         for( ; chIt != l1itemdefAttrColl->end(); ++chIt ) {
            const CondAttrListCollection::ChanAttrListPair& idListPair = *chIt;
            // the attributeList for this channel (a channel corresponds to a triggerItem)
            CondAttrListCollection::ChanNum       ctpid   = idListPair.first;
            const CondAttrListCollection::AttributeList& attrList = idListPair.second;
            TrigConf::TriggerItem* l1item = m_ctpConfig.menu().findTriggerItem( ctpid );
            addThresholdsToTriggerItem( attrList, l1item,
                                        m_ctpConfig.menu().thresholdVector() );
         }
         ATH_MSG_INFO( "  Number of items: " << l1itemdefAttrColl->size() );
      }


      // Bunchgroup content
      if( update_LVL1BGC ) {
         const AthenaAttributeList* bgcontlist = nullptr;
         CHECK( m_detstore->retrieve( bgcontlist, TRIGGER_CONF_LVL1BGDEF ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 bunchgroup definition (BG names)" );
         std::vector< TrigConf::BunchGroup > bgs = readLvl1BGContent( *bgcontlist );
         m_ctpConfig.setBunchGroupSet( TrigConf::BunchGroupSet( bgs ) );
      }


      // Bunchgroup description
      if( update_LVL1BGD ) {
         const AthenaAttributeList* bgdesclist = nullptr;
         CHECK( m_detstore->retrieve( bgdesclist, TRIGGER_CONF_LVL1BGDESC ) );
         ATH_MSG_INFO( "Updating trigger configuration: LVL1 bunchgroup description "
                       << "(name + item)" );
         std::pair< std::vector< std::string >, std::map< unsigned int, unsigned char > > bgdesc =
            readLvl1BGDesc( *bgdesclist );
         // A little sanity check:
         ATH_MSG_INFO( "  Number of bunch groups : " << bgdesc.first.size() );
         // BG names
         const std::vector< std::string >& bgnames = bgdesc.first;
         for( unsigned int i = 0; i < bgnames.size(); ++i ) {
            m_ctpConfig.bunchGroupSet().setBGName( i, bgnames[ i ] );
         }
      }

//       if( updated_newHLTPs ) {
//          // new style file: prescaleset -> chainlist
//          set_ChainlistFromHltPrescaleSet();
//       } else if( updated_HLTMenu ) {
//          // old style file: chainlist -> prescaleset
//          set_HltPrescaleSetFromChainlist();
//       }

      ////changed here || for &&, if Lvl1 there but hlt not, it crashes
      if( update_LVL1MENU || update_HLTMENU ) {
         m_hltFrame.theHLTChainList().setL2LowerChainCounter( &m_ctpConfig );
      }

      if( update_HLTMENU ) {
         setEFLowerChainCounter();
      }

      // for the direct inputs we currently don't have the clock stored in cool so we take them from the TIP map
      if( (update_LVL1THR || update_LVL1PIT)  &&
          m_ctpConfig.menu().tipVector().size() != 0 ) {

         for( TriggerThreshold * const thr : m_ctpConfig.menu().thresholdConfig().getThresholdVector() ) {
            if( (thr->ttype() != L1DataDef::TOPO) && (thr->ttype() != L1DataDef::ALFA) ) {
               continue;
            }

            unsigned int connector = (thr->cableConnector()[3] - '0');
            unsigned int startbit = thr->cableStart();

            unsigned int thrTipNumber = 320 + 64 * connector + 2 * startbit;  // the tipNumber of the threshold is either this one or the next (depends on the clock)

            // for each threshold we search the TIPs for a matching name
            bool foundTipWithMatchingThrName ( false );
            for ( const TIP * tip : m_ctpConfig.menu().tipVector() ) {
               if(tip->thresholdName() == thr->name()) {
                  if( tip->tipNumber() == thrTipNumber || tip->tipNumber() == thrTipNumber+1 ) {
                     thr->setClock( tip->clock() );
                  } else {
                     ATH_MSG_INFO( "For the direct input " << thr->name() << " on bit " << startbit << " of connector " << connector
                                   << " a matching TIP was found but on a different tipNumber " << tip->tipNumber() << ". This affects CTP monitoring" );
                  }
                  foundTipWithMatchingThrName = true;
                  break;
               }
            }
            if(!foundTipWithMatchingThrName) {
               ATH_MSG_INFO( "Did not find TIP information for the direct input " << thr->name() << ". This affects CTP monitoring" );
            } else {
               ATH_MSG_DEBUG( "The direct input " << thr->name() << " was found to have clock  " << thr->clock() );
            }
         }
      }
   } else {
      // SMK from database
      
      CHECK(loadMenuFromDB( m_masterKey, m_lvl1PsKey, m_hltPsKey, m_lvl1BgKey ));

   }
   // Fire incident (used by TrigDecisionTool to update cached menu)
   ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
   if( incSvc.retrieve().isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve IncidentSvc" );
      return StatusCode::SUCCESS;
   }
   
   std::string incname = "";
   if( updated_SMK )      incname += "HLTMenuL1Menu";
   if( updated_LVL1PSK )  incname += "L1PS";
   if( updated_HLTPSK )   incname += "HLTPS";
   if( updated_LVL1BG )   incname += "HLTBG";
   incSvc->fireIncident( Incident( incname, "TrigConf" ) );

   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::DSConfigSvc::loadMenuFromDB( uint32_t smk, uint32_t l1psk, uint32_t hltpsk, uint32_t bgsk ) {

   StatusCode sc = StatusCode::SUCCESS;

   CHECK(initStorageMgr());

   if( fromDB() ) {
       
       ATH_MSG_INFO( "Load configuration from TriggerDB " << m_dbconfig->m_server
                     << " with smk " << smk
                     << ", l1psk " << l1psk
                     << ", hltpsk " << hltpsk
                     << ", bgsk " << bgsk );

      DBLoader::setEnv(DBLoader::CTPOnl);

      // L1Topo menu
      if( smk != m_topoMenu->getSMK() ) {

         m_topoMenu = unique_ptr<TXC::L1TopoMenu>(new TXC::L1TopoMenu);

         m_topoMenu->setSMK( smk );
         m_storageMgr->l1topoMenuLoader().setLevel((MSGTC::Level)msgLevel());

         try {
            if( ! m_storageMgr->l1topoMenuLoader().load( *m_topoMenu ) ) {
               ATH_MSG_ERROR("Could not load L1Topo menu");
               sc = StatusCode::FAILURE;
            } else {
               ATH_MSG_VERBOSE("L1Topo menu object loaded");
            }
         }
         catch (exception & e) {
            ATH_MSG_ERROR("Standard exception caught when loading L1Topo menu from DB: " << e.what());
            sc = StatusCode::FAILURE;
         }

      }

      // CTP menu
      if( smk != m_ctpConfig.smk() ) {

         uint32_t l1pskToLoad = m_ctpConfig.prescaleSetId();
         uint32_t bgskToLoad = m_ctpConfig.bunchGroupSetId();

         if( l1psk != 0 )
            l1pskToLoad = l1psk;

         if( bgsk != 0 )
            bgskToLoad = bgsk;

         m_ctpConfig.menu().clear();
         m_ctpConfig.setSMK( smk );
         m_ctpConfig.setPrescaleSetId( l1pskToLoad );
         m_ctpConfig.setBunchGroupSetId( bgskToLoad );

         m_ctpConfig.setLoadCtpFiles(false);

         try {
            //m_storageMgr->masterTableLoader().setLevel( MSGTC::INFO );
            if( ! m_storageMgr->masterTableLoader().load( m_ctpConfig ) ) {
               ATH_MSG_ERROR("Could not load CTPConfig object");
               sc = StatusCode::FAILURE;
            } else {
               ATH_MSG_VERBOSE("CTPConfig object loaded");
            }
         }
         catch (exception & e) {
            ATH_MSG_ERROR("Standard exception caught when loading CTP config from DB: " << e.what());
            sc = StatusCode::FAILURE;
         }

      } else {

         // Load L1 prescale set if
         // l1psk has been given and is different from the existing one
         if( l1psk != 0 && l1psk != (uint32_t) m_ctpConfig.prescaleSetId() ) {

            TrigConf::PrescaleSet pss;
            pss.setId(l1psk);
            
            try {
               if( ! m_storageMgr->prescaleSetLoader().load( pss ) ) {
                  ATH_MSG_ERROR("Could not load L1 prescale set from DB");
                  sc = StatusCode::FAILURE;
               } else {
                  ATH_MSG_VERBOSE("L1 prescale set loaded");
               }
            }
            catch (exception & e) {
               ATH_MSG_ERROR("Standard exception caught when loading L1 prescale set from DB: " << e.what());
               sc = StatusCode::FAILURE;
            }
            m_ctpConfig.setPrescaleSet( pss );
         }


         // Load bunch group set if
         // bgsk has been given and is different from the existing one
         if( bgsk != 0 && bgsk != (uint32_t)m_ctpConfig.bunchGroupSetId() ) {

            TrigConf::BunchGroupSet bgs;
            bgs.setId(bgsk);
            
            try {
               if( ! m_storageMgr->bunchGroupSetLoader().load( bgs ) ) {
                  ATH_MSG_ERROR("Could not load bunch group set from DB");
                  sc = StatusCode::FAILURE;
               } else {
                  ATH_MSG_VERBOSE("L1 bunch group set loaded");
                  m_ctpConfig.setBunchGroupSet( bgs );
               }
            }
            catch (exception & e) {
               ATH_MSG_ERROR("Standard exception caught when loading L1 bunch group set from DB: " << e.what());
               sc = StatusCode::FAILURE;
            }
         }

      }

      // HLT Menu
      if( smk != m_hltFrame.smk() ) {

         uint32_t hltpskToLoad = m_hltFrame.getPrescaleSet() ? m_hltFrame.getPrescaleSet()->id() : 0;

         if( hltpsk != 0 )
            hltpskToLoad = hltpsk;

         m_hltFrame.setSMK( smk );
         m_hltFrame.thePrescaleSetCollection().set_prescale_key_to_load( hltpskToLoad );

         try {
            if( ! m_storageMgr->hltFrameLoader().load( m_hltFrame ) ){
               ATH_MSG_ERROR("Selected keys could not be loaded");
               sc = StatusCode::FAILURE;
            }
            ATH_MSG_VERBOSE("HLTFrame configuration object loaded");
         }
         catch (exception & e) {
            ATH_MSG_ERROR("Standard exception caught when loading HLT menu from DB: " << e.what());
            sc = StatusCode::FAILURE;
         }

      } else {

         // Load HLT prescale set if
         // hltpsk has been given and is different from the existing one
         if( hltpsk != 0 && hltpsk != (uint32_t) m_hltFrame.getPrescaleSet()->id() ) {

            m_hltFrame.thePrescaleSetCollection().set_prescale_key_to_load( hltpsk );

            try {
               bool loadSuccess = dynamic_cast<TrigConf::StorageMgr*>(m_storageMgr)->hltPrescaleSetCollectionLoader().load( m_hltFrame.thePrescaleSetCollection(), 0, "" );

               if( ! loadSuccess ) {
                  ATH_MSG_ERROR("Could not load HLT prescale set from DB");
                  sc = StatusCode::FAILURE;
               } else {
                  ATH_MSG_VERBOSE("HLT prescale set loaded");
               }
            }
            catch (exception & e) {
               ATH_MSG_ERROR("Standard exception caught when loading HLT prescale set from DB: " << e.what());
               sc = StatusCode::FAILURE;
            }
         }
      }
   }

   CHECK(freeStorageMgr());
   if(m_printMenuLevel==1) {
      cout << "  ================================================================================" << endl
           << "  L1 Topo menu" << endl
           << "    algorithms: " << m_topoMenu->getL1TopoConfigAlgs().size() << endl
           << "    trigger lines: " << m_topoMenu->getL1TopoConfigOutputList().getTriggerLines().size() << endl;
   } else if(m_printMenuLevel>1) {
      m_topoMenu->print();
   }
   m_ctpConfig.print("  ",m_printMenuLevel);
   m_hltFrame.print("  ",m_printMenuLevel);

   return sc;
}

void
TrigConf::DSConfigSvc::setEFLowerChainCounter() {
   for(HLTChain* ch: m_hltFrame.theHLTChainList()) {
      if( ch->level_enum() != EF ) continue;
      unsigned int lower_chain_counter = -1;
      if(ch->lower_chain_name() != "") {
         HLTChain* lowerChain = m_hltFrame.theHLTChainList().chain(ch->lower_chain_name());
         if(lowerChain) lower_chain_counter = lowerChain->chain_counter();
      }
      ch->set_lower_chain_counter( lower_chain_counter );
   }
}


void
TrigConf::DSConfigSvc::set_ChainlistFromHltPrescaleSet() {
 
//    ATH_MSG_INFO( "Copying entries from the HLT prescale set to the chains" );

//    m_hltPsKey = m_prescaleSet->id();
//    m_hltFrame.theHLTChainList().applyPrescaleSet(m_prescaleSet);
}

void
TrigConf::DSConfigSvc::set_HltPrescaleSetFromChainlist() {

//    ATH_MSG_INFO( "Copying entries from the chains to the HLT prescale set" );

//    delete m_prescaleSet;

//    m_prescaleSet = m_hltFrame.chains().extractPrescaleSet();
//    m_prescaleSet->setId( m_hltPsKey );
}

StatusCode
TrigConf::DSConfigSvc::updatePrescaleSets(uint /*requestcount*/) {
   return StatusCode::SUCCESS;
}

// This method is called by TrigSteer on *every* event (keep it fast)
// This is never used in connection with COOL configuration data
StatusCode
TrigConf::DSConfigSvc::assignPrescalesToChains(uint /*lumiblock*/) {
   return StatusCode::SUCCESS;
}

bool 
TrigConf::DSConfigSvc::hasFolder( const std::string& folder_name ){

   if( m_folders.find( folder_name ) != string::npos ) {
      return true;
   } else {
      return false;
   }
}

