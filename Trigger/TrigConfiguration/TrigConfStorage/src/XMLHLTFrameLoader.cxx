/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLBoostHelper.h"
#include "./XMLHLTFrameLoader.h"

#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTPrescale.h"

#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/replace.hpp"

#include <sstream>
#include <iostream>

using namespace std;

bool
TrigConf::XMLHLTFrameLoader::load( HLTFrame& frame ) {

   if ( !is_a("HLT_MENU") ) return false;

   TRG_MSG_INFO("Start loading the HLT menu in mode " << (frame.mergedHLT() ? "MERGED" : "UNMERGED")  );

   frame.clear();

   frame.setName(m_data->second.get<string>("<xmlattr>.menu_name",""));
   
   buildHLTSequenceList( frame );
   
   buildHLTChainList( frame );
   
   // fill prescale set from the HLT chains
   std::string strval("");
   HLTPrescaleSet* pss = frame.thePrescaleSetCollection().setPrescaleSet( new HLTPrescaleSet() );
   if( readAttribute(m_data->second, "prescale_set_name", strval) ) pss->setName(strval);
   
   for(const HLTChain* ch : frame.getHLTChainList())
      pss->setPrescale( ch->prescales(), ch->chain_counter(), str2lvl(ch->level()) );

   return true;
}


void
TrigConf::XMLHLTFrameLoader::buildHLTChainList( HLTFrame& frame ) {

   int ival(0);
   float fval(0);
   bool bval(false);
   std::string strval("");


   ptree sl = m_data->second.get_child("CHAIN_LIST",ptree());
   pair<ptree::const_assoc_iterator, ptree::const_assoc_iterator> r = sl.equal_range("CHAIN");
   for(ptree::const_assoc_iterator i = r.first; i != r.second; ++i) {

      HLTChain* aChain = new HLTChain();
      HLTPrescale& ps = aChain->prescales();

      if( readAttribute(i->second, "chain_name", strval) )               aChain->set_chain_name( strval );
      if( readAttribute(i->second, "chain_counter", ival) )              aChain->set_chain_counter( ival );
      if( readAttribute(i->second, "lower_chain_name", strval) )         aChain->set_lower_chain_name( strval );
      if( readAttribute(i->second, "level", strval) )                    aChain->set_level( strval );
      if( readAttribute(i->second, "prescale", fval) )                   aChain->set_prescale( fval );
      if( readAttribute(i->second, "pass_through", fval) )               aChain->set_pass_through( fval );
      if( readAttribute(i->second, "rerun_prescale", fval))              ps.setRerunPrescale("1",fval);
      if( readAttribute(i->second, "EBstep", ival) )                     aChain->set_EB_after_step( ival );


      ptree prescales = i->second.get_child("PRESCALES", ptree());
      // information in <PRESCALES> overwrites the chain attributes
      if( readAttribute(prescales, "prescale", fval) )                   aChain->set_prescale( fval );
      if( readAttribute(prescales, "pass_through", fval) )               aChain->set_pass_through( fval );
      // -------- find the signatures/TriggerTypes/StreamingTags etc  in the chain -------------------------------
      for(const ptree::value_type& v : prescales ) {
         if( readAttribute(v.second, "target", strval) &&
             readAttribute(v.second, "prescale", fval) )
            ps.setRerunPrescale( strval, fval );
      }

      for(const ptree::value_type& v : i->second.get_child("TRIGGERTYPE_LIST") ) {
         HLTTriggerType* aTT = new HLTTriggerType();
         if( readAttribute(v.second, "bit", ival) ) aTT->set_bit( ival );
         aChain->triggerTypeList().push_back( aTT );
      }

      for(const ptree::value_type& v : i->second.get_child("STREAMTAG_LIST") ) {
         HLTStreamTag* aST = new HLTStreamTag();
         if( readAttribute(v.second, "stream", strval) ) aST->set_stream( strval );
         if( readAttribute(v.second, "type", strval) )   aST->set_type( strval );
         if( readAttribute(v.second, "obeyLB", bval) ) aST->set_obeyLB( bval );
         if( readAttribute(v.second, "prescale", fval) ) aST->set_prescale( fval );
         aChain->addStream(aST);
      }

      for(const ptree::value_type& v : i->second.get_child("GROUP_LIST") ) {
         if( readAttribute(v.second, "name", strval) ) aChain->addGroup( strval );
      }

      for(const ptree::value_type& v : i->second.get_child("SIGNATURE_LIST") ) {
         HLTSignature* aSig = new HLTSignature();
         if( readAttribute(v.second, "signature_counter", ival) ) aSig->set_signature_counter( ival );
         if( readAttribute(v.second, "logic", ival) )             aSig->set_logic( ival );
         for(const ptree::value_type& vte : v.second ) {
            if( readAttribute(vte.second, "te_name", strval) )
               aSig->outputTEs().push_back( new HLTTriggerElement( stringToId( strval ), strval ) );
         }
         aChain->signatureList().push_back( aSig );
      }

      aChain->createSignatureLabels();

      // add to target's list
      frame.theHLTChainList().addHLTChain( aChain );
   }

   // assert that we don't have a mixed menu
   bool foundL2EFChain(false);
   bool foundHLTChain(false);
   for( const TrigConf::HLTChain* chain : frame.getHLTChainList() ) {
      if(chain->level_enum() == TrigConf::HLT) {
         foundHLTChain = true;
      } else {
         foundL2EFChain = true;
      }
   }
   bool foundMixedMenu = foundL2EFChain && foundHLTChain;
   if(foundMixedMenu) {
      TRG_MSG_FATAL("Found mixed (L2/EF and HLT) menu, which I can't deal with");
      return;
   }

   // merge chainlist if needed
   if( frame.mergedHLT() ) {
      bool needMerging(false);
      for( const TrigConf::HLTChain* chain : frame.getHLTChainList() ) {
         if(chain->level_enum() != TrigConf::HLT)
            needMerging = true;
      }

      if(needMerging) {
         TRG_MSG_DEBUG("Call merging of " << frame.getHLTChainList().size() << " L2 and EF chains");
         HLTTEUtils::mergeHLTChainList2(frame);
      }
   }

   TRG_MSG_INFO("Loaded " << frame.getHLTChainList().size() << " chains");

   //   std::cout<<"FPP: XMLHLTFrameLoader::load HLTFrame.mergedHLT() is " << nfTarget.mergedHLT() <<" and chainlist size is "<< nfTarget.getHLTChainList().chains() <<std::endl;

}


void
TrigConf::XMLHLTFrameLoader::buildHLTSequenceList( HLTFrame& frame ) {

   ptree sl = m_data->second.get_child("SEQUENCE_LIST",ptree());
   pair<ptree::const_assoc_iterator, ptree::const_assoc_iterator> r = sl.equal_range("SEQUENCE");
   for(ptree::const_assoc_iterator i = r.first; i != r.second; ++i) {

      HLTSequence* aSeq = new HLTSequence();

      string inputTE = i->second.get<string>("<xmlattr>.input");
      std::vector<string> iTEs;
      boost::split(iTEs, inputTE, boost::is_any_of(" "));
      std::vector<HLTTriggerElement*> inputTEs;
      for(const string& te : iTEs)
         if(te!="")
            inputTEs.push_back( new HLTTriggerElement( stringToId( te ), te ) );
      aSeq->set_inputTEs( inputTEs );
      
      string algo = i->second.get<string>("<xmlattr>.algorithm");
      boost::split(aSeq->algorithms(), algo, boost::is_any_of(" "));

      string outputTE = i->second.get<string>("<xmlattr>.output");
      aSeq->set_outputTE( new HLTTriggerElement( stringToId( outputTE ), outputTE ) );


      string topoStart = i->second.get<string>("<xmlattr>.topo_start_from","");
      if( topoStart != "") {
         HLTTriggerElement* topoStartTE = new HLTTriggerElement( stringToId(topoStart), topoStart );
         aSeq->set_topoStartTE( topoStartTE );
      }

      frame.theHLTSequenceList().addHLTSequence( aSeq );
   }

   TRG_MSG_INFO("Loaded " << frame.getHLTSequenceList().size() << " sequences");

}

int TrigConf::XMLHLTFrameLoader::stringToId( const string& theTE ) {
   // create unique string for TE

   // search if TE already in list (don't fill TE twice !)
   for (std::vector<std::pair<int,string> >::iterator p = m_idstrTable.begin(); p != m_idstrTable.end(); p++)
      if (theTE == p->second) return p->first;

   // not found: attribute identifier and add to list
   int id = -1;

   // this convention is inherited from old TrigConfig package (Frame.cxx)
   if      (theTE == "EMROI"  ) id = 0;
   else if (theTE == "MUONROI") id = 1;
   else if (theTE == "JETROI" ) id = 2;
   else if (theTE == "TAUROI" ) id = 3;
   else                         id = m_idstrTable.size() + 10;

   m_idstrTable.push_back( std::pair<int,string>( id, theTE ) );

   return id;
}

string TrigConf::XMLHLTFrameLoader::stringFromId( int theID ) {
   // create unique string for TE

   // search if TE already in list (don't fill TE twice !)
   for (std::vector<std::pair<int,string> >::iterator p = m_idstrTable.begin(); p != m_idstrTable.end(); p++)
      if (theID == p->first) return p->second;

   return "";
}
