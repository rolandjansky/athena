/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DSConfigSvc.cxx 592654 2014-04-11 18:33:06Z fpastore $

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaKernel/errorcheck.h"

#include "CoolKernel/Record.h"

#include "StoreGate/DataHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Trigger configuration include(s):
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

#include "TrigConfStorage/TrigConfCoolFolderSpec.h"
#include "TrigConfStorage/TrigConfCoolL1PayloadConverters.h"
#include "TrigConfStorage/TrigConfCoolHLTPayloadConverters.h"

// Local include(s):
#include "TrigConfigSvc/DSConfigSvc.h"

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
                                    ISvcLocator* pSvcLocator )
   : AthService( name, pSvcLocator ),
     m_thrcfg( 0 ),
     m_prescaleSet( 0 ),
     m_masterKey( 0 ),
     m_lvl1PsKey( 0 ),
     m_hltPsKey( 0 ),
     m_lvl1BgKey( 0 ),
     m_configSrc( "" ),
     m_detstore( "StoreGateSvc/DetectorStore", name ),
     m_itembgs( 256, 0 ),
     m_bunchGroupSet( new TrigConf::BunchGroupSet( std::vector< TrigConf::BunchGroup >( 8 ) ) ),
     m_readyState( true ), 
     m_readLvl1Thr( false ),
     m_readLvl1BG( false ),
     m_folders( "" )
{
   declareProperty( "readLVL1Thr", m_readLvl1Thr, "Only use data we know are in the file" );
   declareProperty( "readLVL1BG", m_readLvl1BG, "Only use data we know are in the file" );
}

TrigConf::DSConfigSvc::~DSConfigSvc() {

   if( m_bunchGroupSet )  delete m_bunchGroupSet;
   if( m_prescaleSet )  delete m_prescaleSet;
}

StatusCode
TrigConf::DSConfigSvc::initialize() {

   // Notice that there's no need to call the initialization function
   // of the base class now that we inherit from AthService...

   // get detector store
   CHECK( m_detstore.retrieve() );

   ServiceHandle< IJobOptionsSvc > jobOptionsSvc( "JobOptionsSvc", name() );
   if( jobOptionsSvc.retrieve().isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve JobOptionsSvc" );
   } else {
      const Property* p =
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
TrigConf::DSConfigSvc::finalize() {
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

// not yet implemented
const TXC::L1TopoMenu*
TrigConf::DSConfigSvc::menu() const { 
   return 0;
}

const TrigConf::CTPConfig*
TrigConf::DSConfigSvc::ctpConfig() const {
  return & m_ctpConfig;
}

const TrigConf::Muctpi*
TrigConf::DSConfigSvc::muctpiConfig() const {
   REPORT_MESSAGE( MSG::ERROR ) << "The MuCTPi object is not available when running on AOD or from COOL";
   return 0;
}

const TrigConf::ThresholdConfig*
TrigConf::DSConfigSvc::thresholdConfig() const {
   return & m_ctpConfig.menu().thresholdConfig();
}

const TrigConf::HLTChainList*
TrigConf::DSConfigSvc::chainList() const {
   return & m_hltFrame.getHLTChainList();
}

const TrigConf::HLTSequenceList*
TrigConf::DSConfigSvc::sequenceList() const {
   return & m_hltFrame.getHLTSequenceList();
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
   bool update_HLTPS    = false;  // /TRIGGER/HLT/Prescales and /TRIGGER/HLT/PrescaleKey
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
   std::list< std::string >::const_iterator key_itr = keys.begin();
   std::list< std::string >::const_iterator key_end = keys.end();
   for( ; key_itr != key_end; ++key_itr ) {
      if     ( *key_itr == TRIGGER_CONF_HLTMENU    ) update_HLTMENU  = true;
      else if( *key_itr == TRIGGER_CONF_HLTKEYS    ) update_HLTKEYS  = true;
      else if( *key_itr == TRIGGER_CONF_HLTPS      ) update_HLTPS    = true;
      else if( *key_itr == TRIGGER_CONF_HLTPSK     ) update_HLTPS    = true;
      else if( *key_itr == TRIGGER_CONF_LVL1KEY    ) update_LVL1KEY  = true;
      else if( *key_itr == TRIGGER_CONF_LVL1MENU   ) update_LVL1MENU = true;
      else if( *key_itr == TRIGGER_CONF_LVL1PS     ) update_LVL1PS   = true;
      else if( *key_itr == TRIGGER_CONF_LVL1PIT    ) update_LVL1PIT  = true;
      else if( *key_itr == TRIGGER_CONF_HLTGRP     ) update_HLTGRP   = true;
      else if( *key_itr == TRIGGER_CONF_LVL1ITD    ) update_LVL1ITD  = true;
      else if( *key_itr == TRIGGER_CONF_LVL1THR    ) update_LVL1THR  = true;
      else if( *key_itr == TRIGGER_CONF_LVL1BGKEY  ) update_LVL1BGK  = true;
      else if( *key_itr == TRIGGER_CONF_LVL1BGDESC ) update_LVL1BGD  = true;
      else if( *key_itr == TRIGGER_CONF_LVL1BGDEF  ) update_LVL1BGC  = true;
   }

   bool updated_LVL1Menu = update_LVL1MENU || update_LVL1ITD || update_LVL1THR || update_LVL1PIT;
   bool updated_LVL1Ps   = update_LVL1KEY || update_LVL1PS;
   bool updated_HLTMenu  = update_HLTGRP;
   //bool updated_HLTPs    = false;
   bool updated_newHLTPs = false;
   bool updated_LVL1BG   = false;

   // read the runwise configuration keys (master + HLT prescales)
   if( update_HLTKEYS ) {

      const DataHandle< AthenaAttributeList > keysAtrList;
      CHECK( m_detstore->retrieve( keysAtrList, TRIGGER_CONF_HLTKEYS ) );

      ATH_MSG_INFO( "Updating trigger configuration: HLT keys" );

      uint32_t old_masterkey = m_masterKey;
      //uint32_t old_hltPsKey = m_hltPsKey;

      readHltConfigKeys( *keysAtrList,
                                                           m_masterKey, m_hltPsKey, m_configSrc );

      ATH_MSG_INFO( "  Configuration key : " << m_masterKey );
      ATH_MSG_INFO( "  Run-bound HLT prescale key (old style): " << m_hltPsKey );
      ATH_MSG_INFO( "  Original source   : " << m_configSrc );

      updated_HLTMenu |= ( old_masterkey != m_masterKey );
      //updated_HLTPs = ( old_hltPsKey != m_hltPsKey );
      if( m_masterKey == 0 ) updated_HLTMenu = true;
      //if( m_hltPsKey == 0 )  updated_HLTPs   = true;
   }

   // read the LVL1 Menu object
   if( update_LVL1MENU ) {

      const DataHandle< CondAttrListCollection > lvl1MenuAtrColl;
      CHECK( m_detstore->retrieve(lvl1MenuAtrColl, TRIGGER_CONF_LVL1MENU ) );

      ATH_MSG_INFO( "Updating trigger configuration: LVL1 menu" );

      m_ctpConfig.menu().clear();

      CondAttrListCollection::size_type nCh = lvl1MenuAtrColl->size();

      CondAttrListCollection::const_iterator chIt = lvl1MenuAtrColl->begin();
      for( ; chIt != lvl1MenuAtrColl->end(); ++chIt ) {

         CondAttrListCollection::ChanAttrListPair idListPair = *chIt;

         // the attributeList for this channel (a channel corresponds to a triggerItem)
         CondAttrListCollection::ChanNum       ctpid   = idListPair.first;
         CondAttrListCollection::AttributeList atrList = idListPair.second;

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

      const DataHandle< CondAttrListCollection > lvl1PitAtrColl;
      CHECK( m_detstore->retrieve( lvl1PitAtrColl, TRIGGER_CONF_LVL1PIT ) );

      ATH_MSG_INFO( "Updating trigger configuration: LVL1 PITs" );

      CondAttrListCollection::size_type nCh = lvl1PitAtrColl->size();

      CondAttrListCollection::const_iterator chIt = lvl1PitAtrColl->begin();
      for( ; chIt != lvl1PitAtrColl->end(); ++chIt ) {

         CondAttrListCollection::ChanAttrListPair idListPair = *chIt;

         // the attributeList for this channel (a channel corresponds to a PIT)
         CondAttrListCollection::ChanNum       pitNumber = idListPair.first;
         CondAttrListCollection::AttributeList atrList   = idListPair.second;

         TrigConf::PIT* pit = readLvl1InputMap( atrList );

         pit->setPitNumber( ( int ) pitNumber );   // ChanNum is a typedef of uint32
         m_ctpConfig.menu().addPit( pit );  // the menu takes the ownership of the TI
      }

      ATH_MSG_INFO( "  Number of PITs: " << nCh );
      if( msgLvl( MSG::DEBUG ) ) {
         m_ctpConfig.print();
      }
   }

   // read the HLT Menu object
   if( update_HLTMENU ) {

      const DataHandle< CondAttrListCollection > hltMenuAtrColl;
      CHECK( m_detstore->retrieve( hltMenuAtrColl, TRIGGER_CONF_HLTMENU ) );

      ATH_MSG_INFO( "Updating trigger configuration: HLT menu" );

      m_hltFrame.theHLTChainList().clear();
      m_hltFrame.theHLTSequenceList().clear();

      size_t nChains(0);
      for(CondAttrListCollection::const_iterator attrListIt = hltMenuAtrColl->begin(); attrListIt != hltMenuAtrColl->end(); attrListIt++ ) {

         CondAttrListCollection::AttributeList attrList = attrListIt->second;

         m_hltFrame.theHLTChainList().addHLTChain( createHLTChain( attrList, &m_hltFrame.theHLTSequenceList() ) );

         nChains++;;
      }
//       CondAttrListCollection::size_type nCh = hltMenuAtrColl->size();
//       for( CondAttrListCollection::ChanNum ch = 0; ch != nCh; ++ch ) {

//          // the attributeList for this channel (a channel corresponds to a chain)

//          CondAttrListCollection::const_iterator attrListIt = hltMenuAtrColl->chanAttrListPair( ch );

//          if( attrListIt == hltMenuAtrColl->end() )
//             continue;

//          CondAttrListCollection::AttributeList atrList =
//             hltMenuAtrColl->chanAttrListPair( ch )->second;
//          m_hltFrame.theHLTChainList().addHLTChain( createHLTChain( atrList, &m_hltFrame.theHLTSequenceList() ) );
         
//       }
      ATH_MSG_INFO( "  Number of chains: " << nChains );
      ATH_MSG_DEBUG( m_hltFrame.chains() );

      updated_HLTMenu = true;
   }

   if( update_HLTPS ) {

      const DataHandle< CondAttrListCollection > prescaleAL;
      const DataHandle< AthenaAttributeList > keysAL;
      const DataHandle< CondAttrListCollection > hltMenuAtrColl;

      CHECK( m_detstore->retrieve( prescaleAL, TRIGGER_CONF_HLTPS ) );
      CHECK( m_detstore->retrieve( keysAL,     TRIGGER_CONF_HLTPSK ) );
      CHECK( m_detstore->retrieve( hltMenuAtrColl, TRIGGER_CONF_HLTMENU ) );

      ATH_MSG_INFO( "Updating trigger configuration: HLT prescale key and prescales" );

      if( m_prescaleSet ) {
         delete m_prescaleSet;
         m_prescaleSet = 0;
      }
      m_prescaleSet = new HLTPrescaleSet();

      // set the key
      m_hltPsKey = readHltPrescaleKey( *keysAL );
      m_prescaleSet->setId( m_hltPsKey );

      // set the HLT merged/not merged chains
      bool mergedHLT = false;
      CondAttrListCollection::const_iterator attrListIt = hltMenuAtrColl->begin();
      CondAttrListCollection::AttributeList attrList = attrListIt->second;
      if (attrList["ChainName"].data<cool::String255>().find("HLT") != std::string::npos) mergedHLT=true;


      ATH_MSG_INFO( "  LB-bound HLT prescale key : " << m_hltPsKey );

      // set the prescale, pass-through and rerun-prescale values
      CondAttrListCollection::size_type nCh = prescaleAL->size();
      uint npositivepsL2( 0 );
      uint npositivepsEF( 0 );
      uint npositivepsHLT( 0 );

      CondAttrListCollection::const_iterator chIt = prescaleAL->begin();
      for( ; chIt != prescaleAL->end(); ++chIt ) {

         CondAttrListCollection::ChanAttrListPair idListPair = *chIt;

         // the attributeList for this channel (a channel corresponds to a triggerItem)
         CondAttrListCollection::ChanNum       ch      = idListPair.first;
         CondAttrListCollection::AttributeList atrList = idListPair.second;

         float ps, pt, rrps;
         readHltPrescale( atrList, ps, pt, rrps);
         TrigConf::HLTLevel level = (mergedHLT ? HLT :( ( ch % 2 ==0 ) ? L2 : EF));
	 unsigned int cc = ch;
	 if (! mergedHLT ) cc /= 2;
	 else cc -= 20000;

         if( ( level == L2 ) && ( ps > 0.0 ) ) ++npositivepsL2;
         if( ( level == EF ) && ( ps > 0.0 ) ) ++npositivepsEF;
         if( ( level == HLT ) && ( ps > 0.0 ) ) ++npositivepsHLT;

         m_prescaleSet->thePrescale(cc,level).
            setPrescale(ps).
            setPassThrough(pt).
            setRerunPrescale("",rrps);
      }

      ATH_MSG_INFO( "  Number of L2+EF HLT prescales: " << nCh );
      ATH_MSG_INFO( "  Number of positive prescales at L2: " << npositivepsL2 );
      ATH_MSG_INFO( "  Number of positive prescales at EF: " << npositivepsEF );
      ATH_MSG_INFO( "  Number of positive prescales at HLT: " << npositivepsHLT );

      updated_newHLTPs = true;
      if( msgLvl( MSG::DEBUG ) ) {
	  m_prescaleSet->print();
      }
   }

   // read the lb-wise configuration keys (LVL1 prescale key)
   if( update_LVL1KEY ) {

      const DataHandle< AthenaAttributeList > keysAtrList;
      CHECK( m_detstore->retrieve( keysAtrList, TRIGGER_CONF_LVL1KEY ) );

      ATH_MSG_INFO( "Updating trigger configuration: LVL1 keys" );
      readLvl1ConfigKey( *keysAtrList, m_lvl1PsKey );
      ATH_MSG_INFO( "  LVL1 prescale key: " << m_lvl1PsKey );
   }

   // read the lb-wise prescale set object
   if( update_LVL1PS ) {

      const DataHandle< CondAttrListCollection > lvl1PsAtrColl;
      CHECK( m_detstore->retrieve( lvl1PsAtrColl, TRIGGER_CONF_LVL1PS ) );

      ATH_MSG_INFO( "Updating trigger configuration: LVL1 prescales" );

      CondAttrListCollection::size_type nCh = lvl1PsAtrColl->size();

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
         CondAttrListCollection::AttributeList atrList =
            lvl1PsAtrColl->chanAttrListPair( ch )->second;

         readLvl1Prescale( atrList, ps );
         pss.setPrescale( ch, ps );
      }

      if( msgLvl( MSG::DEBUG ) ) {
         pss.print();
      }

      m_ctpConfig.setPrescaleSet( pss ); // overwrites the old one
   }

   if( update_HLTGRP ) {

      const DataHandle< CondAttrListCollection > hltgrpAtrColl;
      CHECK( m_detstore->retrieve( hltgrpAtrColl, TRIGGER_CONF_HLTGRP ) );
      ATH_MSG_INFO( "Updating trigger configuration: HLT chain groups" );


      size_t nGroups(0);
      for(CondAttrListCollection::const_iterator attrListIt = hltgrpAtrColl->begin(); attrListIt != hltgrpAtrColl->end(); attrListIt++ ) {

         CondAttrListCollection::AttributeList attrList = attrListIt->second;

         addGroupsToHltChain( attrList, m_hltFrame.theHLTChainList() );

         nGroups++;;
      }
      ATH_MSG_INFO( "  Number of groups: " << nGroups );
   }

   if( update_LVL1THR ) {

      const DataHandle< CondAttrListCollection > l1thrAtrColl;
      CHECK( m_detstore->retrieve( l1thrAtrColl, TRIGGER_CONF_LVL1THR ) );
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 thresholds" );

      CondAttrListCollection::size_type nCh = l1thrAtrColl->size();

      ATH_MSG_INFO( "  Number of thresholds: " << nCh );
      
      for( CondAttrListCollection::ChanNum ch = 0; ch != nCh; ++ch ) {
         CondAttrListCollection::AttributeList atrList =
            l1thrAtrColl->chanAttrListPair( ch )->second;
         TrigConf::TriggerThreshold* thr = createLvl1Threshold( atrList );

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

            string jws = thr->cableName();
            std::vector< std::string > jwv = TrigConf::split( jws, "," );
            TrigConf::CaloInfo& caloInfo =
               const_cast< TrigConf::CaloInfo& >( thresholdConfig()->caloInfo() );
            for(const string& s: jwv)
               caloInfo.addJetWeight( lexical_cast< int, std::string >( s ) );

         } else {
            m_ctpConfig.menu().addTriggerThreshold( thr );
         }
      }

      m_ctpConfig.menu().thresholdConfig().attributeThresholdNumbers();
   }

   if( update_LVL1ITD ) {

      const DataHandle< CondAttrListCollection > l1itemdefAtrColl;
      CHECK( m_detstore->retrieve( l1itemdefAtrColl, TRIGGER_CONF_LVL1ITD ) );
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 item definition" );

      CondAttrListCollection::const_iterator chIt = l1itemdefAtrColl->begin();
      for( ; chIt != l1itemdefAtrColl->end(); ++chIt ) {

         CondAttrListCollection::ChanAttrListPair idListPair = *chIt;

         // the attributeList for this channel (a channel corresponds to a triggerItem)
         CondAttrListCollection::ChanNum       ctpid   = idListPair.first;
         CondAttrListCollection::AttributeList atrList = idListPair.second;

         TrigConf::TriggerItem* l1item = m_ctpConfig.menu().findTriggerItem( ctpid );

         addThresholdsToTriggerItem( atrList, l1item,
                                                                      m_ctpConfig.menu().thresholdVector() );
      }
      ATH_MSG_INFO( "  Number of items: " << l1itemdefAtrColl->size() );
   }

   // Bunchgroup keys
   if( update_LVL1BGK ) {

      const DataHandle< AthenaAttributeList > bgkcoll;
      CHECK( m_detstore->retrieve( bgkcoll, TRIGGER_CONF_LVL1BGKEY ) );
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 bunchgroup keys" );

      uint32_t old_lvl1BgKey = m_lvl1BgKey;
      m_lvl1BgKey = readLvl1BGKey( *bgkcoll );
      ATH_MSG_INFO( "  LVL1 bunchgroup key : " << m_lvl1BgKey );
      m_bunchGroupSet->setId( m_lvl1BgKey ); // Update the BunchGroupSet object with the ID
      updated_LVL1BG |= ( old_lvl1BgKey != m_lvl1BgKey );

      if( ! m_lvl1BgKey ) updated_LVL1BG = true;
   }

   // Bunchgroup description
   if( update_LVL1BGD ) {

      const DataHandle< AthenaAttributeList > bgdesclist;
      CHECK( m_detstore->retrieve( bgdesclist, TRIGGER_CONF_LVL1BGDESC ) );
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 bunchgroup description "
                    << "(name + item)" );

      std::pair< std::vector< std::string >, std::map< unsigned int, unsigned char > > bgdesc =
         readLvl1BGDesc( *bgdesclist );

      // A little sanity check:
      if( bgdesc.first.size() != 8 ) {
         REPORT_MESSAGE( MSG::FATAL )
            << "The bunchgroup descriptions are not in the expected format";
         return StatusCode::FAILURE;
      }

      // the 8 BG names
      std::vector< std::string >& m_bgnames = bgdesc.first;

      for( int i = 0; i < 8; ++i ) {
         m_bunchGroupSet->setBGName( i, m_bgnames[ i ] );
      }

      for( unsigned int i = 0; i < 256; ++i ) {
         m_itembgs[ i ] = bgdesc.second[ i ];
      }
   }

   // Bunchgroup content
   if( update_LVL1BGC ) {

      const DataHandle< AthenaAttributeList > bgcontlist;
      CHECK( m_detstore->retrieve( bgcontlist, TRIGGER_CONF_LVL1BGDEF ) );
      ATH_MSG_INFO( "Updating trigger configuration: LVL1 bunchgroup definition (BG names)" );

      std::vector< TrigConf::BunchGroup > bgs = readLvl1BGContent( *bgcontlist );

      // if a previous BGS exists let's copy over the names
      if( m_bunchGroupSet ) {
         for( int i = 0; i < 8; ++i ) {
            bgs[ i ].setName( m_bunchGroupSet->bunchGroups()[ i ].name() );
         }
         delete m_bunchGroupSet;
      }

      m_bunchGroupSet = new TrigConf::BunchGroupSet( bgs );
      m_bunchGroupSet->setId( m_lvl1BgKey ); // Update the BunchGroupSet object with the ID
   }

   // update prescales in all places
   // | updated_Menu | updated_newHLTPs | comment                                       | action
   // |     yes      |       yes        | new style file, begin of new run              | prescaleset -> chainlist
   // |     no       |       yes        | new style file, begin of new lb               | prescaleset -> chainlist
   // |     yes      |       no         | old style file                                | chainlist -> prescaleset
   // |     no       |       no         | during run when only l1 prescales are updated | none

   if( updated_newHLTPs ) {
      // new style file: prescaleset -> chainlist
      set_ChainlistFromHltPrescaleSet();
   } else if( updated_HLTMenu ) {
      // old style file: chainlist -> prescaleset
      set_HltPrescaleSetFromChainlist();
   }

   ////changed here || for &&, if Lvl1 there but hlt not, it crashes
   if( updated_HLTMenu && updated_LVL1Menu ) {
      m_hltFrame.theHLTChainList().setL2LowerChainCounter( &m_ctpConfig );
   }

   if( updated_HLTMenu ) {
      setEFLowerChainCounter();
   }

   // Fire incident (used by TrigDecisionTool to update cached menu)
   ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
   if( incSvc.retrieve().isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve IncidentSvc" );
      return StatusCode::SUCCESS;
   }
   
   // prefered
   std::string incname = "";
   if( updated_HLTMenu )  incname += "HLTMenu";
   if( updated_LVL1Menu ) incname += "L1Menu";
   if( updated_LVL1Ps )   incname += "L1PS";
   if( updated_newHLTPs ) incname += "HLTPS";
   incSvc->fireIncident( Incident( incname, "TrigConf" ) );

   return StatusCode::SUCCESS;
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
 
   ATH_MSG_INFO( "Copying entries from the HLT prescale set to the chains" );

   m_hltPsKey = m_prescaleSet->id();
   m_hltFrame.theHLTChainList().applyPrescaleSet(m_prescaleSet);
}

void
TrigConf::DSConfigSvc::set_HltPrescaleSetFromChainlist() {

   ATH_MSG_INFO( "Copying entries from the chains to the HLT prescale set" );

   delete m_prescaleSet;

   m_prescaleSet = m_hltFrame.chains().extractPrescaleSet();
   m_prescaleSet->setId( m_hltPsKey );
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

