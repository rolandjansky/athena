/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/TrigConfCoolL1PayloadConverters.h"
#include "TrigConfStorage/TrigConfCoolFolderSpec.h"

#include "CoolKernel/Record.h"
#include "CoolKernel/IFolder.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListException.h"

#include "CTPfragment/CTPdataformatVersion.h"

#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/MuonThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/XsThresholdValue.h"
#include "TrigConfL1Data/NimThresholdValue.h"
#include "TrigConfL1Data/METSigParam.h"

#include <iostream>
#include <iomanip>
#include <bitset>

#include "boost/lexical_cast.hpp"
#include <boost/algorithm/string.hpp>    

using boost::lexical_cast;

using namespace std;
using namespace cool;
using namespace TrigConf;



Record
TrigConfCoolL1PayloadConverters::createLvl1MenuPayload( cool::IFolderPtr fld, const TriggerItem& item ) {
   Record payload(fld->payloadSpecification());
   payload["ItemName"].setValue<cool::String255>(item.name());
   payload["ItemVersion"].setValue<cool::UInt32>(item.version());
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1ItemDefPayload( cool::IFolderPtr fld, const TrigConf::TriggerItem & item ) {
   Record payload(fld->payloadSpecification());
   vector<string> thresholdlist;
   string logic, conditions;
   item.buildLogic(logic, thresholdlist);
   vector<string>::iterator thrIt = thresholdlist.begin();
   for(;thrIt!=thresholdlist.end();++thrIt) {
      if(thrIt!=thresholdlist.begin()) conditions += ";";
      conditions += *thrIt;
   }
   payload["Logic"].setValue<cool::String255>(logic);
   payload["ConditionsList"].setValue<cool::String4k>(conditions);
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1JetWeightPayload( cool::IFolderPtr fld, const vector<int>& jetweights ) {
   Record payload(fld->payloadSpecification());
   std::string thrValDef("");
   for(std::vector<int>::const_iterator wIt = jetweights.begin(); wIt!=jetweights.end(); ++wIt) {
      if(wIt != jetweights.begin()) thrValDef += ",";
      thrValDef += lexical_cast<std::string,int>(*wIt);
   }
   payload["Threshold"].setValue<cool::String255>("JetWeights");
   payload["ThresholdValue"].setValue<cool::String4k>(thrValDef);
   payload["Cable"].setValue<cool::String255>("");
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1METSigPayload( cool::IFolderPtr fld, const METSigParam& metSigParams ) {
   Record payload(fld->payloadSpecification());
   string thrValDef("");
   thrValDef += lexical_cast<std::string,int>(metSigParams.xsSigmaScale()) + ",";
   thrValDef += lexical_cast<std::string,int>(metSigParams.xsSigmaOffset()) + ",";
   thrValDef += lexical_cast<std::string,int>(metSigParams.xeMin()) + ",";
   thrValDef += lexical_cast<std::string,int>(metSigParams.xeMax()) + ",";
   thrValDef += lexical_cast<std::string,int>(metSigParams.teSqrtMin()) + ",";
   thrValDef += lexical_cast<std::string,int>(metSigParams.teSqrtMax());
   payload["Threshold"].setValue<cool::String255>("METSigParams");
   payload["ThresholdValue"].setValue<cool::String4k>(thrValDef);
   payload["Cable"].setValue<cool::String255>("");
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1ThresholdPayload( cool::IFolderPtr fld, const TrigConf::TriggerThreshold & thr ) {
   Record payload(fld->payloadSpecification());
   // TriggerThreshold
   string thrDef("");
   thrDef += thr.name(); thrDef += ",";
   thrDef += lexical_cast<std::string,int>(thr.version()); thrDef += ",";
   thrDef += thr.type(); thrDef += ",";
   thrDef += lexical_cast<std::string,int>(thr.mapping()); thrDef += ",";
   thrDef += lexical_cast<std::string,bool>(thr.active());


   // TriggerThresholdValue
   string thrValDef("");
   vector<TriggerThresholdValue*>::const_iterator thrValIt = thr.thresholdValueVector().begin();
   vector<TriggerThresholdValue*>::const_iterator thrValEnd = thr.thresholdValueVector().end();
   for(;thrValIt!=thrValEnd;++thrValIt) {
      TriggerThresholdValue* thrVal = *thrValIt;
      
      string valName          = thrVal->name();
      string valType          = thrVal->type();
      string valThresholdval  = lexical_cast<string,float> (thrVal->ptcut());
      string valEtamin        = lexical_cast<string,int>   (thrVal->etamin());
      string valEtamax        = lexical_cast<string,int>   (thrVal->etamax());
      string valPhimin        = lexical_cast<string,int>   (thrVal->phimin());
      string valPhimax        = lexical_cast<string,int>   (thrVal->phimax());
      string valWindow        = lexical_cast<string,int>   (thrVal->window());
      string valEm_isolation  = "63";
      string valHad_isolation = "63";
      string valHad_veto      = "63";
      string valPriority      = lexical_cast<string,float>(thrVal->priority());

      if ( thr.type()==L1DataDef::emType() ||
           thr.type()==L1DataDef::tauType() ) {
         ClusterThresholdValue* cluThrVal = dynamic_cast<ClusterThresholdValue*>(thrVal);
         if(cluThrVal) {
            valEm_isolation  = lexical_cast<string,float>(cluThrVal->emIsolation());  
            valHad_isolation = lexical_cast<string,float>(cluThrVal->hadIsolation());
            valHad_veto      = lexical_cast<string,float>(cluThrVal->hadVeto());
         }
      } // for the other types nothing needs to be done

      if(thrValIt!=thr.thresholdValueVector().begin()) thrValDef += ";"; // separation between TTV's
      thrValDef += valName;          thrValDef += ",";
      thrValDef += valType;          thrValDef += ",";
      thrValDef += valThresholdval;  thrValDef += ",";
      thrValDef += valEtamin;        thrValDef += ",";
      thrValDef += valEtamax;        thrValDef += ",";
      thrValDef += valPhimin;        thrValDef += ",";
      thrValDef += valPhimax;        thrValDef += ",";
      thrValDef += valEm_isolation;  thrValDef += ",";
      thrValDef += valHad_isolation; thrValDef += ",";
      thrValDef += valHad_veto;      thrValDef += ",";
      thrValDef += valWindow;        thrValDef += ",";
      thrValDef += valPriority;   
   }


   // Cable info
   std::string cableDef("");
   cableDef += thr.cableName(); cableDef += ",";
   cableDef += thr.cableCtpin(); cableDef += ",";
   cableDef += thr.cableConnector(); cableDef += ",";
   cableDef += lexical_cast<std::string,int>(thr.cableStart()); cableDef += ",";
   cableDef += lexical_cast<std::string,int>(thr.cableEnd());

   // fill the payload
   payload["Threshold"].setValue<cool::String255>(thrDef);
   payload["ThresholdValue"].setValue<cool::String4k>(thrValDef);
   payload["Cable"].setValue<cool::String255>(cableDef);
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1ConfigKeyPayload( cool::IFolderPtr fld, unsigned int lvl1PrescaleKey ) {
   Record payload(fld->payloadSpecification());
   payload["Lvl1PrescaleConfigurationKey"].setValue<cool::UInt32>(lvl1PrescaleKey);
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1PrescalesPayload( cool::IFolderPtr fld, int prescale) {
   Record payload(fld->payloadSpecification());
   if(payload["Lvl1Prescale"].storageType()==cool::StorageType::Int32)
      payload["Lvl1Prescale"].setValue<cool::Int32>(prescale);
   else
      payload["Lvl1Prescale"].setValue<cool::Int64>(prescale);
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1PrescalesPayload( cool::IFolderPtr fld, int64_t prescale) {
   Record payload(fld->payloadSpecification());
   if(payload["Lvl1Prescale"].storageType()==cool::StorageType::Int32)
      payload["Lvl1Prescale"].setValue<cool::Int32>(prescale);
   else
      payload["Lvl1Prescale"].setValue<cool::Int64>(prescale);
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1BGKeyPayload( cool::IFolderPtr fld, unsigned int lvl1BunchgroupKey) {
   Record payload(fld->payloadSpecification());
   payload["Lvl1BunchGroupConfigurationKey"].setValue<cool::UInt32>(lvl1BunchgroupKey);
   return payload;
}


Record
TrigConfCoolL1PayloadConverters::createLvl1BGContentPayload( cool::IFolderPtr fld, const BunchGroupSet& bgs) {

   Record payload(fld->payloadSpecification());

   // the output vector
   vector<unsigned char> codes(2 * TrigConfCoolFolderSpec::mBGContentBlobSize, 0 );

   std::vector<const BunchGroup*> bgOrdered(16,(const BunchGroup*)0);

   for(const BunchGroup & bgp: bgs.bunchGroups())
      bgOrdered.at(bgp.internalNumber()) = &bgp;




   for(int bgC=0; bgC<16; ++bgC) {
      const BunchGroup* bg = bgOrdered[bgC];
      if(bg==0) continue;

      // mask describes the current bunchgroup (BG1: 1, BG2: 2, BG3: 4, ..., BG8: 128)

      unsigned char mask = (1 << (bgC % 8) );
      unsigned int bgOffset = bgC < 8 ? 0 : TrigConfCoolFolderSpec::mBGContentBlobSize; // for BG 8-15 we use the higher part of the blob

      for(int b : bg->bunches()) {
         if(b<0 || b >= TrigConfCoolFolderSpec::mBGContentBlobSize) {
            // this is a serious error which can't be ignored
            std::string errorMsg =
               "Bunch group " + to_string(bgC) + " contains bunch number "
               + to_string(b) + " which is outside the range [0,3563]";
            throw std::runtime_error(errorMsg);
         }
         codes[static_cast<unsigned int>(b) + bgOffset] |= mask;
      }
   }

   std::cout << "LVL1 bunchgroups [list 16 bunches per line]:" << std::endl;
   coral::Blob blob;

   blob.resize( codes.size() );

   unsigned char* p = static_cast<unsigned char*>(blob.startingAddress());

   for (size_t i=0; i < codes.size() ; ++i,++p) {
      *p = codes[i];

      cout << string(std::bitset<8>(codes[i]).to_string<char,std::char_traits<char>,std::allocator<char> >());
      if((i+1)%16==0) cout << endl;
      else cout << " ";
   }
   cout << endl;

   payload["BunchCode"].setValue<coral::Blob>( blob );


   return payload;
}





Record
TrigConfCoolL1PayloadConverters::createLvl1BGDescPayload( cool::IFolderPtr fld, const BunchGroupSet& bgs, const Menu& lvl1Menu) {

   CTPdataformatVersion ctpDataformat(lvl1Menu.thresholdConfig().ctpVersion());

   Record payload(fld->payloadSpecification());


   for(const BunchGroup& bunchgroup: bgs.bunchGroups()) {

      unsigned int internalNumber = bunchgroup.internalNumber();

      const string bgname = bunchgroup.name();

      std::string payloadKey = "BunchGroup" + boost::lexical_cast<std::string,int>(internalNumber);

      if(payload.specification().exists(payloadKey)) {
         std::cout << "Write bunch group  " << payloadKey << ": '"<< bgname <<"'" << std::endl;
         payload[payloadKey].setValue<cool::String255>( bgname );
      } else {
         std::cout << "WARNING: COOL DB schema has not yet made fields for bunchgroup " << internalNumber << " (" << bgname << ")" << endl;
      }
   }

   cout << "Writing bunch group description with size " << ctpDataformat.getMaxTrigItems() << endl;

   vector<unsigned char> codes( ctpDataformat.getMaxTrigItems() ,0);
   for(TriggerItem* item: lvl1Menu.items() ) {

      unsigned int ctpid = item->ctpId();
      if(ctpid >= codes.size())
         throw runtime_error("Item ctpid exceeds blob size!");
      const TriggerItemNode* topNode = item->topNode();
      if(topNode==0)
         throw runtime_error("Item " + item->name() + " has no definition nodes attached");
      vector<bool> v1;
      // get a vector of size 8, element true means the bunch group is
      // in the item definition - assumes AND logic only!
      topNode->getAllBunchGroups(v1);
      for(size_t i = 0; i<v1.size();++i) {
         if(v1[i]) codes[ctpid] |= (1<<i);
      }

      cout << "Item " << setw(3) << setfill(' ') << ctpid << " bunch-group code " 
           << string(bitset<8>(codes[ctpid]).to_string<char,char_traits<char>,allocator<char> >())
           << endl;
   }

   // Get the blob, resize it to actual size, and push codes into it
   //   coral::Blob& blob = payload["ItemToBunchGroupMap"].data<coral::Blob>();
   coral::Blob blob;
   blob.resize( codes.size() );
   unsigned char* p = static_cast<unsigned char*>(blob.startingAddress());
   for (size_t i=0; i < codes.size(); ++i,++p) {
      *p = codes[i];
   }

   payload["ItemToBunchGroupMap"].setValue<coral::Blob>( blob );

   return payload;
}




// ------------------------------------------------------------
// createLvl1InputMapPayload(std::string pitMap)
// ------------------------------------------------------------
cool::Record 
TrigConfCoolL1PayloadConverters::createLvl1InputMapPayload( cool::IFolderPtr fld, const TrigConf::TIP& tip)
{
   Record payload(fld->payloadSpecification());
   payload["ThresholdName"].setValue<cool::String255>(tip.thresholdName());
   payload["CtpinSlot"].setValue<cool::UChar>(static_cast<unsigned char>(tip.slot()));
   payload["CtpinConnector"].setValue<cool::UChar>(static_cast<unsigned char>(tip.connector()));
   payload["ThresholdBit"].setValue<cool::UChar>(static_cast<unsigned char>(tip.thresholdBit()));
   payload["CableBit"].setValue<cool::UChar>(static_cast<unsigned char>(tip.cableBit()));
   payload["ThresholdMapping"].setValue<cool::UChar>(static_cast<unsigned char>(tip.thresholdMapping()));
   payload["ThresholdActive"].setValue<cool::Bool>(tip.thresholdActive());
   return payload;
}

// ------------------------------------------------------------
// createLvl1MonMapPayload(std::string monMap)
// ------------------------------------------------------------
cool::Record
TrigConfCoolL1PayloadConverters::createLvl1MonMapPayload( cool::IFolderPtr fld, 
                                                          const std::string& type,
                                                          const uint16_t& bgId,
                                                          const std::string& threshName,
                                                          const std::string& slot,
                                                          const std::string& con,
                                                          const std::string& mult,
                                                          const std::string& start,
                                                          const std::string& end,
                                                          const std::string& active,
                                                          const std::string& monName,
                                                          const std::string& CounterLogic)
{
   Record payload(fld->payloadSpecification());
   payload["CounterType"].setValue<cool::String255>(type);
   payload["BunchGroupId"].setValue<cool::UChar>(static_cast<unsigned char>(bgId));
   payload["ThresholdName"].setValue<cool::String255>(threshName);
   payload["CtpinSlot"].setValue<cool::String255>(slot);
   payload["CtpinConnector"].setValue<cool::String255>(con);
   payload["Multiplicity"].setValue<cool::String255>(mult);
   payload["ThresholdBitStart"].setValue<cool::String255>(start);
   payload["ThresholdBitEnd"].setValue<cool::String255>(end);
   payload["ThresholdActive"].setValue<cool::String255>(active);
   payload["CounterName"].setValue<cool::String255>(monName);
   payload["CounterLogic"].setValue<cool::String255>(CounterLogic);
   return payload;
}


TriggerItem * 
TrigConfCoolL1PayloadConverters::createLvl1TriggerItem( const coral::AttributeList & al ) {
   TriggerItem * item = new TriggerItem();
   item->setName   ( al["ItemName"].   data<cool::String255>() );
   item->setVersion( al["ItemVersion"].data<cool::UInt32>() );
   return item;
}


void
TrigConfCoolL1PayloadConverters::addThresholdsToTriggerItem( const coral::AttributeList & al, TriggerItem * item,
                                                             const vector<TriggerThreshold*>& thrs) {
   string logic = al["Logic"].data<cool::String255>();
   vector<string> condList = split(al["ConditionsList"].data<cool::String4k>(), ";");

   item->setCondition(logic, condList, thrs);
}


TriggerThreshold *
TrigConfCoolL1PayloadConverters::createLvl1Threshold( const coral::AttributeList & al ) {
   string thrDef    = al["Threshold"].data<cool::String255>();
   string thrValDef = al["ThresholdValue"].data<cool::String4k>();
   string cableDef  = al["Cable"].data<cool::String255>();
   
   // TriggerThreshold
   TriggerThreshold * thr = new TriggerThreshold();
   vector<string> thrDefV = split(thrDef,",");
   thr->setName(thrDefV[0]);

   // there was a late request for adding the JetWeights to COOL since
   // we don't want to add new folders to the schema, we added the
   // twelve jetweights as a special Threshold to the COOL/Threshold folder
   if(thr->name()=="JetWeights" || thr->name()=="METSigParams") {
      thr->setCableName(thrValDef);
      return thr;
   }

   thr->setVersion(lexical_cast<int, string>(thrDefV[1]));
   thr->setType(thrDefV[2]);
   thr->setMapping(lexical_cast<int, string>(thrDefV[3]));
   thr->setActive(lexical_cast<bool, string>(thrDefV[4]));

   // internal triggers have no mapping (and are usually not stored in COOL, except by the online L1)
   // in order to insert them in the threshold vector at the right position they get a mapping according to their name
   // BGRP0,BGRP1,BGRP2,BGRP3,BGRP4...
   // RNDM0,RNDM1,RNDM2,...
   // PCLK0,...
   if(thr->isInternal()) {
      string::size_type pos = thr->name().find_first_of("0123456789");
      int mapping = boost::lexical_cast<int,string>(thr->name().substr(pos));
      thr->setMapping( mapping );
   }

   // TriggerThresholdValues
   vector<string> thrAllValDefV = split(thrValDef,";");
   for(const string& thrAllVal: thrAllValDefV) {
      if(thrAllVal=="") continue;
      vector<string> thrValDefV = split(thrAllVal,",");   

      string valName         = thrValDefV[0];
      string valType         = thrValDefV[1];
      float valThresholdval  = lexical_cast<float, string>(thrValDefV[2]);
      int   valEtamin        = lexical_cast<int,   string>(thrValDefV[3]);
      int   valEtamax        = lexical_cast<int,   string>(thrValDefV[4]);
      int   valPhimin        = lexical_cast<int,   string>(thrValDefV[5]);
      int   valPhimax        = lexical_cast<int,   string>(thrValDefV[6]);
      float valEm_isolation  = lexical_cast<float, string>(thrValDefV[7]);
      float valHad_isolation = lexical_cast<float, string>(thrValDefV[8]);
      float valHad_veto      = lexical_cast<float, string>(thrValDefV[9]);
      int   valWindow        = lexical_cast<int,   string>(thrValDefV[10]);
      float valPriority      = lexical_cast<float, string>(thrValDefV[11]);

      TriggerThresholdValue* thrv = TriggerThreshold::createThresholdValue( thr->type() );
      if(thrv) {
         thrv->setName(valName);      
         thrv->setType(valType);      
         thrv->setName(valName);
         thrv->setType(valType);
         thrv->setPtcut(valThresholdval);
         thrv->setPriority(valPriority);
         thrv->setEtaMin(valEtamin);
         thrv->setEtaMax(valEtamax);
         thrv->setPhiMin(valPhimin);
         thrv->setPhiMax(valPhimax);
         thrv->setWindow(valWindow);
         ClusterThresholdValue* cluthrval = dynamic_cast<ClusterThresholdValue*>(thrv);
         if(cluthrval) {
            cluthrval->setEmIsolation(valEm_isolation);
            cluthrval->setHadIsolation(valHad_isolation);
            cluthrval->setHadVeto(valHad_veto);
         }
      }
      thr->addThresholdValue(thrv);
   }

   // add the cable information
   vector<string> cableDefV = split(cableDef,",");
   string cableName      = cableDefV[0];
   string cableCtpin     = cableDefV[1];
   string cableConnector = cableDefV[2];
   int    cableStart     = lexical_cast<int,std::string>(cableDefV[3]);
   int    cableEnd       = lexical_cast<int,std::string>(cableDefV[4]);
   thr->setCableName     ( cableName );
   thr->setCableCtpin    ( cableCtpin);
   string ctpin(cableCtpin);
   boost::to_lower(ctpin);
   if( ctpin == "ctpcore" ) {
      thr->setInput( "ctpcore" );
   } else {
      thr->setInput( "ctpin" );
   }
   thr->setCableConnector( cableConnector);
   thr->setCableStart    ( cableStart );
   thr->setCableEnd      ( cableEnd );

   return thr;
}


vector<BunchGroup>
TrigConfCoolL1PayloadConverters::readLvl1BGContent( const coral::AttributeList & al) {
   const coral::Blob& blob = al["BunchCode"].data<coral::Blob>();

   // check blob size - if we ever change the blob size, we have to work with schema versions here
   if(blob.size() != 3564 && blob.size() != 2 * 3564)
      throw runtime_error("Read BLOB for BunchCode of unexpected size!");


   unsigned int numberBG = (blob.size() == 3564) ? 8 : 16;

   vector<BunchGroup> bgV(numberBG);
   unsigned int bgIdx(0);
   for(BunchGroup & bg : bgV) {
      bg.setInternalNumber(bgIdx++);
   }
   
   const unsigned char* p = static_cast<const unsigned char*>(blob.startingAddress());

   for (size_t bunch = 0; bunch < 3564; ++bunch, ++p) {
      unsigned char mask = *p;
      for(int i=0; i<8;i++) {
         if( (mask>>i) & 0x1) bgV[i].addBunch(bunch);
      }
   }

   if(blob.size() == 2 * 3564) { // Run 2 with have 8 more BG
      for (size_t bunch = 0; bunch < 3564; ++bunch, ++p) {
         unsigned char mask = *p;
         for(int i=0; i<8;i++)
            if( (mask>>i) & 0x1) bgV[8+i].addBunch(bunch);
      }
   }
   return bgV;
}


pair< vector<string>, map<unsigned int,unsigned char> >
TrigConfCoolL1PayloadConverters::readLvl1BGDesc(const coral::AttributeList & al) {
   vector<string> names;
   names.push_back(al["BunchGroup0"].data<cool::String255>());
   names.push_back(al["BunchGroup1"].data<cool::String255>());
   names.push_back(al["BunchGroup2"].data<cool::String255>());
   names.push_back(al["BunchGroup3"].data<cool::String255>());
   names.push_back(al["BunchGroup4"].data<cool::String255>());
   names.push_back(al["BunchGroup5"].data<cool::String255>());
   names.push_back(al["BunchGroup6"].data<cool::String255>());
   names.push_back(al["BunchGroup7"].data<cool::String255>());
   try {
      names.push_back(al["BunchGroup8"].data<cool::String255>());
      names.push_back(al["BunchGroup9"].data<cool::String255>());
      names.push_back(al["BunchGroup10"].data<cool::String255>());
      names.push_back(al["BunchGroup11"].data<cool::String255>());
      names.push_back(al["BunchGroup12"].data<cool::String255>());
      names.push_back(al["BunchGroup13"].data<cool::String255>());
      names.push_back(al["BunchGroup14"].data<cool::String255>());
      names.push_back(al["BunchGroup15"].data<cool::String255>());
    }
   catch(const coral::AttributeListException &) {} // run2

   std::map<unsigned int,unsigned char> codes;
   const coral::Blob& blob = al["ItemToBunchGroupMap"].data<coral::Blob>();
   // check blob size - if we ever change the blob size, have to work
   // with schema versions here

   if(blob.size() != 256 && blob.size() != 512)
      throw runtime_error("Read BLOB for ItemToBunchGroupMap of unexpected size!");

   const unsigned char* p = static_cast<const unsigned char*>(blob.startingAddress());
   for (long i = 0; i < blob.size(); ++i,++p) {
      unsigned char mask = (*p);
      codes.insert( make_pair(i,mask) );
   }
   return make_pair(names,codes);
}


void
TrigConfCoolL1PayloadConverters::readLvl1ConfigKey( const coral::AttributeList & al,
                   unsigned int & lvl1PrescaleKey )
{
   lvl1PrescaleKey = al["Lvl1PrescaleConfigurationKey"].data<cool::UInt32>();
}


void
TrigConfCoolL1PayloadConverters::readLvl1Prescale( const coral::AttributeList& al, int & prescaleValue)
{
   if ( al["Lvl1Prescale"].specification().type() == typeid(cool::Int32) )
      prescaleValue = al["Lvl1Prescale"].data<cool::Int32>();
   else
      prescaleValue = al["Lvl1Prescale"].data<cool::Int64>();
}


void
TrigConfCoolL1PayloadConverters::readLvl1Prescale( const coral::AttributeList& al, int64_t & prescaleValue)
{
   if ( al["Lvl1Prescale"].specification().type() == typeid(cool::Int32) )
      prescaleValue = al["Lvl1Prescale"].data<cool::Int32>();
   else
      prescaleValue = al["Lvl1Prescale"].data<cool::Int64>();
}


int
TrigConfCoolL1PayloadConverters::readLvl1BGKey( const coral::AttributeList & al)
{
   return al["Lvl1BunchGroupConfigurationKey"].data<cool::UInt32>();
}


PIT*
TrigConfCoolL1PayloadConverters::readLvl1InputMap( const coral::AttributeList & al)
{
   TrigConf::PIT* pit = new TrigConf::PIT();
   pit->setThresholdName( al["ThresholdName"].data<cool::String255>() );
   pit->setCtpinSlot( static_cast<uint16_t>(al["CtpinSlot"].data<cool::UChar>()) );
   pit->setCtpinConnector( static_cast<uint16_t>(al["CtpinConnector"].data<cool::UChar>()) );
   pit->setThresholdBit( static_cast<uint16_t>(al["ThresholdBit"].data<cool::UChar>()) );
   pit->setCableBit( static_cast<uint16_t>(al["CableBit"].data<cool::UChar>()) );
   pit->setThresholdMapping( static_cast<uint16_t>(al["ThresholdMapping"].data<cool::UChar>()) );
   pit->setThresholdActive( al["ThresholdActive"].data<cool::Bool>() );

   return pit;
}

TIP*
TrigConfCoolL1PayloadConverters::readLvl1TIPMap( const coral::AttributeList & al)
{
   TrigConf::TIP* tip = new TrigConf::TIP();
   tip->setThresholdName( al["ThresholdName"].data<cool::String255>() );
   tip->setSlot( static_cast<uint16_t>(al["CtpinSlot"].data<cool::UChar>()) );
   tip->setConnector( static_cast<uint16_t>(al["CtpinConnector"].data<cool::UChar>()) );
   tip->setThresholdBit( static_cast<uint16_t>(al["ThresholdBit"].data<cool::UChar>()) );
   tip->setCableBit( static_cast<uint16_t>(al["CableBit"].data<cool::UChar>()) );
   tip->setThresholdMapping( static_cast<uint16_t>(al["ThresholdMapping"].data<cool::UChar>()) );
   tip->setThresholdActive( al["ThresholdActive"].data<cool::Bool>() );

   return tip;
}

std::vector<TrigConf::ThresholdMonitor*>
TrigConfCoolL1PayloadConverters::readLvl1MonMap( const cool::IRecord & payload)
{
   vector<ThresholdMonitor*> vec;
   vector<string> names   = split(payload["ThresholdName"].data<cool::String255>(),":");
   vector<string> slots   = split(payload["CtpinSlot"].data<cool::String255>(),":");
   vector<string> cons    = split(payload["CtpinConnector"].data<cool::String255>(),":");
   vector<string> mults   = split(payload["Multiplicity"].data<cool::String255>(),":");
   vector<string> starts  = split(payload["ThresholdBitStart"].data<cool::String255>(),":");
   vector<string> ends    = split(payload["ThresholdBitEnd"].data<cool::String255>(),":");
   vector<string> actives = split(payload["ThresholdActive"].data<cool::String255>(),":");

   string monname = payload["CounterName"].data<cool::String255>();
   string type    = payload["CounterType"].data<cool::String255>();
   //std::string counterlogic = payload["CounterLogic"].data<cool::String255>();
   unsigned char bgId = payload["BunchGroupId"].data<cool::UChar>();

   // check vectors are all the same size
   if( 
       names.size() != slots.size() ||
       names.size() != cons.size() ||
       names.size() != mults.size() ||
       names.size() != starts.size() ||
       names.size() != ends.size() ||
       names.size() != actives.size()
       )
      throw std::runtime_error("Inconsistent mon vector sizes!");

   for(size_t i = 0 ; i < names.size(); ++i) {
      TrigConf::ThresholdMonitor* mon = new TrigConf::ThresholdMonitor();
      // Not possible yet. mon->setLogic(counterlogic[i]);
      mon->setName(monname);
      mon->setBunchGroupId( static_cast<int>(bgId) );
      mon->setCounterType(type);

      mon->setThresholdName(names[i]);
      mon->setCtpinSlot(         lexical_cast<uint16_t,std::string>(slots[i]) );
      mon->setCtpinConnector(    lexical_cast<uint16_t,std::string>(cons[i]) );      
      mon->setThresholdStartBit( lexical_cast<int,std::string>(starts[i]) );
      mon->setThresholdEndBit(   lexical_cast<int,std::string>(ends[i]) );
      mon->setMultiplicity(      lexical_cast<int,std::string>(mults[i]) );
      bool active = (actives[i]=="ENABLED");
      mon->setThresholdActive( active );
      vec.push_back(mon);
   }
   return vec;
}



