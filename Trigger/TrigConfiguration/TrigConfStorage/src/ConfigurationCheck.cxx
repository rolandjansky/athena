/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Menu consistency checks
 */

#include <set>
#include <map>
#include <iterator>
#include <algorithm>
#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"

#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfStorage/ConfigurationCheck.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfL1Data/HelperFunctions.h"

#include <iostream>

using namespace std;

TrigConf::TrigConfTest::TrigConfTest(const std::string& name, const std::string& description, Severity s ) 
  : m_ctp(0), m_hlt(0), m_error(""), m_name(name), m_description(description), m_enabled(true), m_severity(s) {}

void TrigConf::TrigConfTest::run(const Exc_t& exceptions) { 
  if (m_enabled) execute(exceptions); 
}

bool TrigConf::TrigConfTest::status() const { 
  return (m_enabled ? isok() : true); 
}

bool TrigConf::TrigConfTest::code() const {
  return !status() && m_severity >= ERROR;
}

std::string TrigConf::TrigConfTest::severity() const {
   switch(m_severity) {
   case INFO:  return "INFO";
   case WARNING: return "WARNING";
   case ERROR: return "ERROR";
   } 
   return "";
}

bool TrigConf::TrigConfTest::isok() const {
   return m_error.size() == 0;
}

namespace TrigConf {
   bool matches_any(const std::vector<boost::regex>& exc_regex, const string& s) {
      for(const boost::regex& e: exc_regex ) {
         if(regex_match(s, e)) return true;
      }
      return false;
   }

   vector<boost::regex>
   buildExcRec(const TrigConfTest::Exc_t& exceptions) {
      vector<boost::regex> exc_regex;
      for(const string& exc: exceptions ) {
         exc_regex.push_back(boost::regex(exc+"$"));
      }
      return exc_regex;
  }
}

using namespace TrigConf;

///////////////////////// NUMBER AND RANGE OF CPT ITEMS   /////////////////////////
class NumberOfCTPItemsTest : public TrigConfTest {
public:
  NumberOfCTPItemsTest() 
    : TrigConfTest("NumberOfCTPItem", "Less than 256 items, CTP id less than 256"), 
      m_itemsNumber(0),
      m_highestCTPNumber(0)
  {}
  
  virtual void execute(const Exc_t& ) {
     if ( ! m_ctp ) return;

     m_itemsNumber = m_ctp->menu().itemVector().size();
     for ( int i = 0; i < m_itemsNumber; i++ )
        m_highestCTPNumber = std::max<int>(m_ctp->menu().itemVector()[i]->ctpId(), m_highestCTPNumber);
     if( m_itemsNumber > 512 ) 
        m_error += "More than 512 items in Lvl1 menu. ";
     if( m_highestCTPNumber >= 512 ) 
        m_error += "Some items have CTP ID>=512.";
  }
  
private:
  int m_itemsNumber;
  int m_highestCTPNumber;
};


///////////////////////// NAMING CONVENTION FOR CHAINS   /////////////////////////
class ChainsNamingConventionTest : public TrigConfTest {
public:
  ChainsNamingConventionTest() 
    : TrigConfTest("ChainsNamingConvention", "Chain names must start with <level>_"),
      m_offending("")
  {}
  
  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;
    
     for( const TrigConf::HLTChain* ch : m_hlt->getHLTChainList() ) {
        if ( ch->chain_name().find(ch->level()+"_") != 0 ) // of the level is not a prefix for the chain and it is not right 
           m_offending += ch->chain_name()+"("+ch->level()+") ";
     }
     if(m_offending.size()!=0)
        m_error = " Offending chains and their levels " + m_offending;
  }
  
private:
  std::string m_offending;
};

///////////////////////// NAMING CONVENTION FOR TRIGGER ELEMENTS   /////////////////////////
class TENamingConventionTest : public TrigConfTest {
public:
   TENamingConventionTest() 
      : TrigConfTest("TENamingConvention", "Output TE names must start with <level>_"),
        m_offending("")
   {}

   bool isExplicitTE(const std::string& teName) {
      for( const TrigConf::HLTChain* c : m_hlt->getHLTChainList() )
         for( const TrigConf::HLTSignature* s : c->signatureList() )
            for( const TrigConf::HLTTriggerElement* te : s->outputTEs() )
               if(te->name() ==  teName) return true;
      return false;
   }

  
   virtual void execute(const Exc_t&) {
      if ( ! m_hlt ) return;
     
      for(const TrigConf::HLTSequence* seq : m_hlt->getHLTSequenceList()) {
         const std::string& tename = seq->outputTE()->name();
        
         if( ( tename.find("L2") != 0 ) &&
             ( tename.find("EF") != 0 ) &&
             ( tename.find("HLT") != 0 ) ) {
            // name of output TE does not start with L2 nor EF
            //          if(!isExplicitTE(tename))
            //            m_offending += tename + ", ";
            m_offending += tename + ", ";
         }
      }
     
      if(m_offending.size()!=0)
         m_error = " Offending trigger elements " + m_offending;
   }
   
private:
   std::string m_offending;
};


///////////////////////// Max lenght of TRIGGER ELEMENTS name  /////////////////////////
class TENameLengthTest : public TrigConfTest {
public:

  TENameLengthTest() 
      : TrigConfTest("TENameLength", "Max length of TE name must be less than 100 characters"),
        m_offending("")
  {}
  
   virtual void execute(const Exc_t&) {
      if ( ! m_hlt ) return;

      unsigned int _maxTElength = 100;
     
      for(const TrigConf::HLTSequence* seq : m_hlt->getHLTSequenceList()) {
         const std::string& tename = seq->outputTE()->name();

	 if (tename.length() > _maxTElength )
	   m_offending += tename + ", ";
	 
      }	 
	 
      if(m_offending.size()!=0)
	m_error = " Following Trigger elements are too long [more than 100 characters long]: " + m_offending;
   }
  
private:
   std::string m_offending;
};


/////////////// CHECK WHETHER ALL SIGNATURES IN A GROUP ARE CONNECTED BY COMMON TEs //////////////////
////// ALL OUTPUT TEs OF A SIGNATURE ARE THE FINAL TE OF THE CHAIN OR THE INPTUT OF ANOTHER SIGNATURE   ///
////// CHECK ALSO CHANGE OF MULTIPLICITY BETWEEN SIGNATURES

struct multi_sign{
  unsigned int multiplicity;
  std::string  name;
};

class SignaturesConnectedTest : public TrigConfTest {
public:
   SignaturesConnectedTest() 
      : TrigConfTest("SignaturesConnectedTest","Signatures in the chain should be connected", INFO)  {}
  
   virtual void execute(const Exc_t&) {
      if ( ! m_hlt ) return;

      for(TrigConf::HLTChain* ch : m_hlt->getHLTChainList() ) {
    
         std::vector<struct multi_sign> nextinputTEcount; // input of each sign output

         bool isLastSignature = true;
         for( auto i = ch->signatureList().rbegin(); i != ch->signatureList().rend(); ++i ) {

            TrigConf::HLTSignature* sig = *i;

            std::vector<struct multi_sign> outputTEcount;
            createSignMultiplicity(sig->outputTEs(), outputTEcount);
	
            for( multi_sign outputMultiCount : outputTEcount) {
               bool connected = false;
               for( multi_sign inputMultiCount : nextinputTEcount ) {
                  // don't check for multiplicity changes, since ComboAlgo's can change multiplicity
                  //if( (inputMultiCount.name == outputMultiCount.name) && (inputMultiCount.multiplicity == outputMultiCount.multiplicity)){
                  if( inputMultiCount.name == outputMultiCount.name ){ 
                     connected=true; //this output is the input of the next signature
                     break;
                  }
               }

               if ( !connected ) {
                  if ( ! isLastSignature ){ // if is not the last signature in the chain
                     m_message +=  ch->chain_name() + "(TE: " + outputMultiCount.name +"),  " ;
                  }
                  nextinputTEcount.push_back(outputMultiCount); //add also itself for multiplicity != 1
                  recursivelyFindInputTE(outputMultiCount, nextinputTEcount); 	  	 	    
               }
            }
            isLastSignature = false;
         }
      }
      if( m_message.size()!=0 ) m_error = "These chains are not connected or change multiplicity: " + m_message;
      return;
   }


   // recursively find the input TEs given the outputTE, looping over sequences, and add them to the vector of string
   // return false if no input is found 
   bool recursivelyFindInputTE( const struct multi_sign & outputtename,std::vector<struct multi_sign>& inputtenames ) {
      const TrigConf::HLTSequence* seq = m_hlt->getHLTSequenceList().getSequence(outputtename.name); 
      if(!seq) return false;
      bool status = true;

      //make multi_list for inputs inside the sequence!
      std::vector<struct multi_sign> seq_input;
      createSignMultiplicity(seq->inputTEs(), seq_input);
      for( unsigned int i =0; i< seq_input.size(); i++){
         // multipl. n. of. inputs times n. of seqeunces required
         seq_input[i].multiplicity = seq_input[i].multiplicity *outputtename.multiplicity; 
         bool newo=true;
         for( unsigned int gi =0; gi< inputtenames.size(); gi++){
            if ( inputtenames[gi].name == seq_input[i].name ){
               inputtenames[gi].multiplicity++;
               newo=false;
               break;
            }
         }
         if(newo){
            inputtenames.push_back(seq_input[i]);     
         }
         status |= recursivelyFindInputTE(seq_input[i] ,inputtenames ); 
      }
      return status;
   }

   // creates a vector of (TE-name, multiplicity)
   void createSignMultiplicity(std::vector<TrigConf::HLTTriggerElement*> TElist, std::vector<struct multi_sign>& Multilist) {
      for ( TrigConf::HLTTriggerElement* te : TElist ) {
         bool newo=true;
         // find TE-name in multiList and increment count
         for( multi_sign ms : Multilist ){ 
            if (te->name() == ms.name ){ 
               ms.multiplicity++;
               newo=false;
               break;
            }
         }
         // TE-name not found, add new counter
         if(newo){
            struct multi_sign ms;
            ms.name = te->name();
            ms.multiplicity = 1;
            Multilist.push_back(ms);
         }
      }
      return;
   }

private:
   std::string m_message;
};


///////////////////////// CHAIN NAMES UNIQUE IN MENU   /////////////////////////
class ChainsNamingUniqueTest : public TrigConfTest {
public:
  ChainsNamingUniqueTest() 
    : TrigConfTest("ChainsNamingUnique", "Chain names must be unique"), m_repeated("")  {}

  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;

     std::set<std::string> chains;    

     for (const HLTChain* ch : m_hlt->getHLTChainList()) {
        if(chains.count(ch->chain_name())>0)
           m_repeated += ch->chain_name()+", ";
        else 
           chains.insert( ch->chain_name() );
     }
     if(m_repeated.size()>0)
        m_error = "The following chain names appear multiple times: " + m_repeated;
  }

private:
  std::string m_repeated;
};


///////////////////////// CHAIN COUNTERS UNIQUE IN LEVEL   /////////////////////////
class ChainsCountersUniqueTest : public TrigConfTest {
public:
  ChainsCountersUniqueTest() 
    : TrigConfTest("ChainsCountersUnique", "Chain counters must be unique"),
      m_repeatedl2(""),
      m_repeatedef("")
   {}

  virtual void execute(const Exc_t&) {

     if ( ! m_hlt ) return;

     std::set<int> l2counters;    
     std::set<int> efcounters;    

     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        std::set<int>& counters = ch->level()=="L2"?l2counters:efcounters;
        if(counters.count(ch->chain_counter())>0) {
           std::string& rep = ch->level()=="L2"?m_repeatedl2:m_repeatedef;
           rep += boost::lexical_cast<std::string,int>(ch->chain_counter())+", ";
        } else 
           counters.insert( ch->chain_counter() );
     }
     if( (m_repeatedl2.size() + m_repeatedef.size())>0)
        m_error = "The following chain counters appear multiple times in L2: " + m_repeatedl2 + "\n, and these in EF: " + m_repeatedef;
  }

private:
  std::string m_repeatedl2;
  std::string m_repeatedef;
};



///////////////////////// CHAIN COUNTER RANGE   /////////////////////////
class ChainsCounterRangeTest : public TrigConfTest {
public:
   ChainsCounterRangeTest() 
      : TrigConfTest("ChainsCounterRange", "Chain counter within [1..8095]"),
        m_offending("")
   {}

   virtual void execute(const Exc_t&) {
      if ( ! m_hlt ) return;
   
      for(const HLTChain* ch : m_hlt->getHLTChainList()) {
         if ( ch->chain_counter() > 0x1fff  || ch->chain_counter() < 1 ) 
            m_offending += ch->chain_name()+ "[" + boost::lexical_cast<std::string,int>(ch->chain_counter()) + "], ";
      }
      if(m_offending.size()>0)
         m_error = "Chains with counter outside [1..8095]: " + m_offending;
   }
   
private:
  std::string m_offending;
};


//////////////////////// AT LEAST ONE TRIGGER TYPE AND ONE STREAM TAG FOR EACH CHAIN   /////////////////////////
class StreamTagPresentTest : public TrigConfTest {
public:
  StreamTagPresentTest() 
    : TrigConfTest("StreamTagPresent", "Chain must go into at least one stream unprescaled"), m_offending("")  {}

  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;
    
     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        if ( ch->streams().size() == 0 ) {
           m_offending += ch->chain_name()+" (no stream), ";
           continue;
        }
        // need at least one unprescaled stream
        int pt1streams = 0;
        for(const HLTStreamTag *s : ch->streams()) {
           if ( s->prescale() == 1 ) pt1streams++;
           if ( s->type() == "physics" && s->prescale() != 1) m_offending += ch->chain_name() +" has phsycsis stream:" + s->stream() + " prescaled, ";
        }
        if ( pt1streams == 0 )
           m_offending += ch->chain_name()+" (no unprescaled stream), ";
     }
     if(m_offending.size()>0)
        m_error = "Chains with stream settings problems: " + m_offending;
  }

private:
  std::string m_offending;
};


//////////////////////// AT LEAST ONE GROUP IS DEFINED FOR EACH CHAIN   /////////////////////////
class GroupPresentTest : public TrigConfTest {
public:
  GroupPresentTest() 
    : TrigConfTest("GroupPresent", "Chains should belong to a group", WARNING),
      m_offending("")
  {}

  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;
     for(TrigConf::HLTChain* ch : m_hlt->getHLTChainList() ) {
        if ( ch->groups().empty() ) 
           m_offending += ch->chain_name() + ", ";
     }
     m_error = "Chains without a Group: " + m_offending;
  }

  virtual bool isok() const {
    return m_offending.size()==0;
  }

private:
  std::string m_offending;
};

//////////////////////// AT LEAST ONE BANDWIDTH GROUP IS DEFINED FOR EACH CHAIN   /////////////////////////
class BWGroupPresentTest : public TrigConfTest {
public:
  BWGroupPresentTest() 
    : TrigConfTest("BWGroupPresent", "Chains should belong to a BW group", INFO),
      m_offending("")
  {}

  virtual void execute(const Exc_t& exceptions) {
     if ( ! m_hlt ) return;

     std::vector<boost::regex> exc_regex = buildExcRec(exceptions);

     for(TrigConf::HLTChain* ch : m_hlt->getHLTChainList() ) {
        if( matches_any(exc_regex, ch->chain_name()) ) continue; // excempt
      
        bool bwgroup=false;
        for( const string it : ch->groups()) {
           if ( it.find("BW:") == 0 ) { bwgroup=true; break; }
        }
        if(!bwgroup)
           m_offending += ch->chain_name() + ", ";
     }
     m_error = "Chains without a BW Group: " + m_offending;
  }

  virtual bool isok() const {
    return m_offending.size()==0;
  }

private:
  std::string m_offending;
};



//////////////////////// UNIQUE SIGNATURE COUNTER [1,N]  /////////////////////////
class UniqueSignatureCountTest : public TrigConfTest {
public:
  UniqueSignatureCountTest() 
    : TrigConfTest("SignatureCountOrder", "Ascending signature counters"), m_offending("")  {}

  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;
     for (TrigConf::HLTChain* ch : m_hlt->getHLTChainList() ) {
        int previousSigCounter = -1;
        for(const TrigConf::HLTSignature* sig : ch->signatureList()) {
           int currentCounter = sig->signature_counter();
           if ( currentCounter <= previousSigCounter ) {
              m_offending += ch->chain_name() + ", ";
              break;
           }
           previousSigCounter = currentCounter; 	
        }
     }
     if(m_offending.size()>0)
        m_error = "Chains with out-of-order signature counts: " + m_offending;
  }

private:
  std::string m_offending;
};

class NonEmptySignature : public TrigConfTest {
public:
  NonEmptySignature()
    : TrigConfTest("NonEmptySignature", "List of trigger elements in signature not empty"), m_offending(""){}
  virtual void execute(const Exc_t&) {
     if (!m_hlt) return;

     for(const TrigConf::HLTChain* chain : m_hlt->getHLTChainList() ) {
        for(const TrigConf::HLTSignature* sig : chain->signatureList() ) {
           if (sig->outputTEs().empty()) {
              m_offending += chain->chain_name() + ", ";
           }
        }
     }
     if(! m_offending.empty() ) {
        m_error = "Chains with empty signatures: " + m_offending; 
     }
    
  }    
private:
  std::string m_offending;
};


//////////////////////// check inputs to L2 and EF  /////////////////////////
class LowerLevelMatchingTest : public TrigConfTest {
public:
  LowerLevelMatchingTest() 
    : TrigConfTest("LowerLevelMatching", "Chains seeds must exist"),
      m_chains(""),
      m_chainsEF("")
  {}
  
  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;

     std::set<std::string> l2Chains;    
     std::set<std::string> l1items;

     for(const HLTChain* ch : m_hlt->getHLTChainList())
        if ( ch->level() == "L2" ) l2Chains.insert( ch->chain_name() );
     
     if ( m_ctp ) {
        int itemsCount = m_ctp->menu().itemVector().size();
        for ( int i = 0 ; i < itemsCount; ++i ) {
           TriggerItem* item = m_ctp->menu().itemVector()[i];
           l1items.insert(item->name());
        }
     }
     //copy(l1items.begin(),l1items.end(),std::ostream_iterator<std::string>(std::cout, " | "));
  
     // loop egain for all HLT chains and check if they use non-existing seeds

     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        if(!m_ctp && ( ch->level_enum() == L2 || ch->level_enum() == HLT) ) continue; // if there is no L1 info, we can't check the L2 or HLT chains for input
        const std::string& lcn = ch->lower_chain_name();
        if ( lcn == "" ) continue;  // unseeded chains are fine
        const std::set<std::string>& lower = ch->level() == "EF"?l2Chains:l1items;
        if (lcn.find(',')!=std::string::npos) {
           std::vector<std::string> singleItems = TrigConf::split(lcn, ",");
           for(std::vector<std::string>::iterator itit = singleItems.begin(); itit!=singleItems.end(); ++itit) {
              std::string l1seed = *itit;
              TrigConf::strip(l1seed);
              if ( lower.count(l1seed) == 0 )
                 m_chains += ch->chain_name() + "(missing seed " + l1seed + "), ";
           }

           if(ch->level() == "EF" && singleItems.size() != 1) {
              m_chainsEF += ch->chain_name() + "(multiple L2 seeds " + lcn + "), ";
           }

        } else {
           if ( lower.count(lcn) == 0 )
              m_chains += ch->chain_name() + "(missing seed " + lcn + "), ";
        }
     }

     if(m_chains.size()!=0)
        m_error = "The following chains have non-existing lower chain or item: " + m_chains;

     if(m_chainsEF.size()!=0)
        m_error += "The following chains have multiple L2 inputs: " + m_chainsEF;
  }
  
private:
  std::string m_chains;
  std::string m_chainsEF;
};

// USAGE OF ALL LVL1 ITEMS AS PREREQUISITE TO AN HLT CHAIN, AND ALL L2 CHAINS TO AN EF CHAIN //
class LowerLevelUsedTest : public TrigConfTest {
public:
   LowerLevelUsedTest(int level) 
      : TrigConfTest((level==1?"SeedingL2":"SeedingEF"), (level==1?"Check for L1 items not used by L2":"Check for L2 chains not used by EF"), INFO),
        m_chains(""),
        m_items(""),
        m_level(level)
   {}
  
   virtual void execute(const Exc_t& exceptions) {
      if ( ! m_hlt ) return;
    
      std::set<std::string> chainsSeedingEF;
      std::set<std::string> itemsSeedingL2;

      for(const HLTChain* ch: m_hlt->getHLTChainList() ) {

         const std::string& lcn = ch->lower_chain_name();
         if ( lcn == "" ) continue; // unseeded chains
         std::set<std::string>& seeds = ch->level() == "EF"?chainsSeedingEF:itemsSeedingL2;
         if (lcn.find(',')!=std::string::npos) {
            std::vector<std::string> singleItems = TrigConf::split(lcn, ",");

            for(string itemname : singleItems ) {
               TrigConf::strip(itemname);
               seeds.insert( itemname );
            }

         } else {
            seeds.insert( lcn );
         }
      }

      //std::copy(itemsSeedingL2.begin(), itemsSeedingL2.end(), std::ostream_iterator<std::string>(std::cout," "));

      std::vector<boost::regex> exc_regex;
      for(const string& exc : exceptions ) {
         exc_regex.push_back(boost::regex(exc+"$"));
      }

      if(m_level==2) {
         for(const TrigConf::HLTChain* ch : m_hlt->getHLTChainList() ) {
            if (ch->level() == "EF") continue;
            if (ch->level() == "HLT") continue;
	 
            // check for excempt chains
            bool chain_is_excempt = false;
            for(const boost::regex e : exc_regex ) {
               if(regex_match(ch->chain_name(), e)) {
                  chain_is_excempt = true;
                  break;
               }
            }
            if(chain_is_excempt) continue;
	  
            if( chainsSeedingEF.count( ch->chain_name())==0 ) {
               if(m_chains!="") m_chains += ", ";
               m_chains += ch->chain_name();
            }
         }
         if(m_chains.size()>0){
            m_error = "L2 chains not seeding EF: " + m_chains;
         }
      }
     
     
      if ( m_ctp && m_level==1 ) {
       
         for(TriggerItem* item: m_ctp->menu().itemVector() ) {

            // check for exceptions
            bool item_is_excempt = false;
            for(const boost::regex e : exc_regex ) {
               if(regex_match(item->name(), e)) {
                  item_is_excempt = true;
                  break;
               }
            }
            if(item_is_excempt) continue;

            if ( itemsSeedingL2.count(item->name()) == 0 ) {
               if (m_items!="") m_items += ", ";
               m_items += item->name();
            }
         }
         if(m_items.size()>0){	  
            m_error = "CTP items not seeding L2/HLT chains: " + m_items;
         }
      }
   }
  
private:
   std::string m_chains;
   std::string m_items;
   int m_level;
};



#define INITTHR(THR) \
    unsigned int THR = THR##_max; \
    string THR##List  = "";

#define SUBTHR(THR) \
   if(thr->type()==L1DataDef::THR##Type()) { \
         THR--; \
         THR##List += thr->name() + ", "; \
   } else

#define CHECKTHR(THR) \
   if(THR!=0) m_missingThresholds += L1DataDef::THR##Type() + ", "

#define ERRORTHR(THR) \
   if(THR>0) m_error += "\n [" + THR##List + "] (required are " + char(char('0')+THR##_max/10) + char(char('0')+THR##_max%10) + ")"



// All L1Calo thresholds must be defined, even if not used
class L1CaloThrDef : public TrigConfTest {
public:
   L1CaloThrDef() 
      : TrigConfTest("L1CaloDefined", "Check if all L1 calo thresholds are defined", INFO),
        m_missingThresholds("")
   {}

  
   virtual void execute(const Exc_t&) {
      if ( !m_ctp ) return;

      int m_l1Version = m_ctp->menu().thresholdConfig().l1Version();
      
      L1DataDef::setMaxThresholdsFromL1Version(m_l1Version);
      //L1DataDef::printMaxThresholds();
      
      unsigned int em_max  = L1DataDef::typeConfig(L1DataDef::EM).max;  
      unsigned int tau_max = L1DataDef::typeConfig(L1DataDef::TAU).max;
      unsigned int jet_max = L1DataDef::typeConfig(L1DataDef::JET).max;
      unsigned int jf_max  = L1DataDef::typeConfig(L1DataDef::JF).max; //max_JF_Threshold_Number();
      unsigned int jb_max  = L1DataDef::typeConfig(L1DataDef::JB).max; //ymax_JB_Threshold_Number();
      unsigned int te_max  = L1DataDef::typeConfig(L1DataDef::TE).max; //max_TE_Threshold_Number();
      unsigned int xe_max  = L1DataDef::typeConfig(L1DataDef::XE).max; //max_XE_Threshold_Number();
      unsigned int je_max  = L1DataDef::typeConfig(L1DataDef::JE).max; //max_JE_Threshold_Number();
      unsigned int xs_max  = L1DataDef::typeConfig(L1DataDef::XS).max; //max_XS_Threshold_Number();
      INITTHR(em)
         INITTHR(tau)
         INITTHR(jet)
         INITTHR(jf)
         INITTHR(jb)
	INITTHR(te)
         INITTHR(xe)
         INITTHR(je)
         INITTHR(xs)
        
	for(const TrigConf::TriggerThreshold* thr : m_ctp->menu().thresholdConfig().thresholdVector() ) {
	  SUBTHR(em)
            SUBTHR(tau)
            SUBTHR(jet)
            SUBTHR(jf)
            SUBTHR(jb)
            SUBTHR(te)
            SUBTHR(xe)
            SUBTHR(je)
            SUBTHR(xs) {};
      }
      

      CHECKTHR(em);
      CHECKTHR(tau);
      CHECKTHR(jet);
      CHECKTHR(jf);
      CHECKTHR(jb);
      CHECKTHR(te);
      CHECKTHR(xe);
      CHECKTHR(je);
      CHECKTHR(xs);

      if(m_missingThresholds.size()>0) {
         m_error = "L1 calo thresholds that are not complete: " + m_missingThresholds;
         ERRORTHR(em);
         ERRORTHR(tau);
         ERRORTHR(jet);
         ERRORTHR(jf);
         ERRORTHR(jb);
	 ERRORTHR(te); //for now disabled
         ERRORTHR(xe);
         ERRORTHR(je);
         ERRORTHR(xs);
      }
   }
  
private:
   std::string m_missingThresholds;
};


#define INIT_THRMAP(THR) \
  thr_map[L1DataDef::THR##Type()] = std::vector<string>(THR##_max);   \
  thr_map_frep[L1DataDef::THR##Type()] = std::vector<int>(THR##_max); \
  thr_count[L1DataDef::THR##Type()] = 0;                              \
  typeName.insert(L1DataDef::THR##Type());                            

// All L1Calo thresholds must be defined, even if not used
class L1CaloThrOrder : public TrigConfTest {
public:
  L1CaloThrOrder() 
     : TrigConfTest("L1CaloThrOrder", "Check if all L1 calo thresholds are in the correct order", ERROR)
  {}

  
  virtual void execute(const Exc_t&) {
     if ( !m_ctp ) return;

     unsigned int em_max  = L1DataDef::max_EM_Threshold_Number();  // this is actually em+tau
     unsigned int tau_max = L1DataDef::max_TAU_Threshold_Number();
     unsigned int jet_max = L1DataDef::typeConfig(L1DataDef::JET).max;
     unsigned int jf_max  = L1DataDef::max_JF_Threshold_Number();
     unsigned int jb_max  = L1DataDef::max_JB_Threshold_Number();
     unsigned int te_max  = L1DataDef::max_TE_Threshold_Number();
     unsigned int xe_max  = L1DataDef::max_XE_Threshold_Number();
     unsigned int je_max  = L1DataDef::max_JE_Threshold_Number();
     unsigned int xs_max  = L1DataDef::max_XS_Threshold_Number();

     std::map<string, std::vector<string> > thr_map;
     std::map<string, std::vector<int> > thr_map_frep;
     std::map<string, int > thr_count;
     std::set<string> typeName;

     INIT_THRMAP(em);
     INIT_THRMAP(tau);
     INIT_THRMAP(jet);
     INIT_THRMAP(jf);
     INIT_THRMAP(jb);
     INIT_THRMAP(te);
     INIT_THRMAP(xe);
     INIT_THRMAP(je);
     INIT_THRMAP(xs);

     boost::regex e("(\\D+)(\\d+)([_.[:alpha:]0-9]*)");
     boost::cmatch matches;  // cmatch = match_results<const char*>

     // calculate threshold rank
     for(const TrigConf::TriggerThreshold* thr : m_ctp->menu().thresholdConfig().thresholdVector() ) {
        if(typeName.find(thr->type()) == typeName.end()) continue; // only l1 calo

        int i = -1;
        if(boost::regex_match(thr->name().c_str(), matches, e)) {
           i = 10*boost::lexical_cast<int,std::string>(string(matches[2].first, matches[2].second));  // the value
           string extra(matches[3].first, matches[3].second); // extra arg like HV or I
           if(thr->type()!=L1DataDef::jetType()) { // for jet in HI menu this doesn't work because of the order J5_win8, J5_win6, J5, J10,...
              i += matches[3].second-matches[3].first;
           } 
        } else {
           cout << thr->name() << " does not match pattern " << std::endl;
        }
        thr_map[thr->type()][thr->mapping()] = thr->name();
        thr_map_frep[thr->type()][thr->mapping()] = i;
        thr_count[thr->type()] = thr_count[thr->type()] + 1;
     }

     for(size_t i(0);i<thr_map[L1DataDef::tauType()].size();i++) {
        thr_map[L1DataDef::emType()].pop_back();
        thr_map_frep[L1DataDef::emType()].pop_back();
     }

     for(string thrname : typeName ) {
        // check the vector
        bool ordered = true;
        int pre_v = 0;
        for(int i=0; i<thr_count[thrname]; i++ ) {
           int v = thr_map_frep[thrname][i];
           if(v<pre_v) { ordered=false; break; }
           pre_v = v;
        }
        if( ! ordered ) {
//            std::cout << "Unordered: " << thrname << ": ";
//            std::copy(thr_map[thrname].begin(), thr_map[thrname].end(), std::ostream_iterator<std::string>(std::cout,", "));
//            std::cout << std::endl;

           if(m_error.size()==0) m_error = "Some calo threshold types are out of order: ";
           m_error += "\n" + thrname + ": ";
           for(string thresh : thr_map[thrname]) { m_error += thresh + ", "; }

        }
     }

  }
  
private:
};




///////////////// ALL TE'S OF A SIGNATURE MUST HAVE A PRODUCING SEQUENCE //////////////////

class SignaturesProducibleTest : public TrigConfTest {
public:
  SignaturesProducibleTest() 
    : TrigConfTest("SignaturesProducible","Sequences for all steps must exist")  {}
  
  virtual void execute(const Exc_t&) {
     if ( ! m_hlt ) return;
    
     std::set<std::string> teOutFromSeq;

     // find all output TE's from the SequenceList
     for(const HLTSequence* seq : m_hlt->getHLTSequenceList())
        teOutFromSeq.insert( seq->outputTE()->name());
     
     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        for ( unsigned int s = 0; s < ch->signatureList().size(); s++ ) {
           HLTSignature* sig = ch->signatureList()[s];
           for ( unsigned int t = 0; t < sig->outputTEs().size(); ++t ) {
              if ( teOutFromSeq.count(sig->outputTEs()[t]->name()) == 0 )
                 m_chains += ch->chain_name()+" (step "+boost::lexical_cast<std::string,int>(sig->signature_counter())+"): "+sig->outputTEs()[t]->name()+", ";
           }
        }
     }
     if(m_chains.size()>0)
        m_error = "Chains with signatures that can't be produced due to missing sequence: ";// + m_chains;
  }

  private:
  std::string m_chains;
};



////////////////////////////// UNIQUE OUTPUT TE //////////////////////////////
class SequenceOutputUniqueTest : public TrigConfTest {
public:
   SequenceOutputUniqueTest() 
      : TrigConfTest("SequenceOutputUnique", "Sequence output TE's must be unique")  {}
  
   virtual void execute(const Exc_t&) {
      if ( ! m_hlt ) return;

      std::set<std::string> teOutFromSeq;

      for(const HLTSequence* seq : m_hlt->getHLTSequenceList()) {
         if ( teOutFromSeq.count( seq->outputTE()->name() ) == 0 ) 
            teOutFromSeq.insert( seq->outputTE()->name());
         else {
            if(m_tes!="") m_tes += ", ";
            m_tes += seq->outputTE()->name();
         }
      }
      if(m_tes.size()>0)
         m_error = "TE's produced by more than one sequence: " + m_tes;
   }

private:
   std::string m_tes;
};


//////////////// INPUT TE'S OF USED SEQUENCES MUST BE OUTPUT TE OF ANOTHER SEQUENCE  /////////////////
class SequenceInputProducibleTest : public TrigConfTest {
public:
  SequenceInputProducibleTest() 
    : TrigConfTest("SequenceInputProducible", "All input TE's must be produced")  {}
  
  virtual void execute(const Exc_t&) {
     if ( !m_hlt || !m_ctp ) return;
    
     std::set<std::string> teOut;      // TE's that are output of any sequence in the sequence list
     std::set<std::string> teInForSeq; // TE's that are input to any sequence in the sequence list
    
     // find all output and input TE's from the SequenceList
     for(HLTSequence* seq : m_hlt->getHLTSequenceList()) {
        teOut.insert( seq->outputTE()->name());
        for(const TrigConf::HLTTriggerElement* te : seq->inputTEs())
           if(te->name()!="") teInForSeq.insert( te->name());
     }

     // add the threshold names to the output TE's
     for( TriggerThreshold* thr : m_ctp->menu().thresholdConfig().thresholdVector() )
        teOut.insert( thr->name() );

     // check if all inputs are mentioned in outputs
     for(const string& inTE : teInForSeq) {
        if ( teOut.count(inTE) == 0) { // input does not appear ( empty string is excepted )
           cout << "Not found as output ------------------: "  << inTE << endl;
           m_tes += inTE + ", ";
        }
     }
     if(m_tes.size() != 0)
        m_error = " The following input TEs don't appear as output TE: " + m_tes;

  }

private:
  std::string m_tes;
};



///////////////////////// TEs used by EF chains are produced by corresponding L2 chain or come from L1 /////////////////////////
class EFTEsFromSeedingL2ChainsTest : public TrigConfTest {
public:
  EFTEsFromSeedingL2ChainsTest()
    : TrigConfTest("EFTEsFromL2", "EF input TEs produced by corresponding L2 chain or be L1 ROI")
  {}
  virtual void execute(const Exc_t&) {
     if ( !m_hlt || !m_ctp ) return;

     std::map<std::string, const TrigConf::HLTChain*> chainsMap;
     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        chainsMap[ch->lower_chain_name()] = ch;
     }

     std::map<std::string, const TrigConf::HLTSequence*> sequencesMap;
     for(const HLTSequence* seq : m_hlt->getHLTSequenceList()) {
        sequencesMap[seq->outputTE()->name()] = seq;
     }


     std::set<std::string> l1thresholds;
     for(std::vector<TriggerThreshold*>::const_iterator thrIt = m_ctp->menu().thresholdConfig().thresholdVector().begin();
         thrIt != m_ctp->menu().thresholdConfig().thresholdVector().end(); ++thrIt)
        l1thresholds.insert((*thrIt)->name());


     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        if (ch->level() == "L2") // this is EF check
           continue;
        if (ch->lower_chain_name() == "") // skip unseeded
           continue;
        const TrigConf::HLTChain* efchain = ch;
        const TrigConf::HLTChain* l2chain = chainsMap[efchain->lower_chain_name()];
        std::set<std::string> l2tes = HLTUtils::allTEsProducedInL2Chain(*l2chain, m_hlt->getHLTSequenceList());
        std::set<std::string> eftes = HLTUtils::allTEsProducedInEFChain(*efchain, m_hlt->getHLTSequenceList(), l2tes);

        std::set<std::string> efinputTEs;
        std::set<std::string>::const_iterator te;

        for (te = eftes.begin(); te != eftes.end(); ++te) {	
           // find input
           const TrigConf::HLTSequence* s = sequencesMap[*te];	
           // and complain if it is not there (or it is not L1 threshold)
           if ( s == 0 && l1thresholds.find(*te) == l1thresholds.end()) {
              m_error += " missing sequence for TE: " + *te + " ";
              continue;
           }
           for ( unsigned int t = 0; t < s->inputTEs().size(); ++t ) {
              std::string name = s->inputTEs()[t]->name();
              if( name == "" ) 
                 continue;
              if ( eftes.find(name) == eftes.end())
                 efinputTEs.insert(name);
           }
        } 
      
        // ugh, all above is 3 lines in python
        for ( te = efinputTEs.begin(); te != efinputTEs.end(); ++te ) {
           if ( l2tes.find(*te) == l2tes.end() &&  l1thresholds.find(*te) == l1thresholds.end() ) {
              m_error += "Chain: "+efchain->chain_name()+"  TE: "+ (*te) + " " ;
           }
        }
     }
  }

};

///////////////////////// LVL1 ITEMS AND'ED WITH BGRP0 /////////////////////////
class BGRP0Test : public TrigConfTest {
public:
  BGRP0Test() 
    : TrigConfTest("BGRP0Test", "All Level 1 Items must be AND'ed with BGRP0"),
      m_offending("")
  {}

  void checkForBG0inTop(TriggerItemNode * const  node, bool& found) const
  {
    // BGRP0 must be in top AND in the logic
    if(node==0 || node->type()==TriggerItemNode::OR || node->type()==TriggerItemNode::NOT) return;
    if(node->type()==TriggerItemNode::OBJ &&
       node->isInternalTrigger() &&
       node->internalTriggerType() == L1DataDef::BGRP &&
       node->internalTriggerNumber() == 0
       ) {
      found = true;
      return;
    }
    for(TriggerItemNode *child : node->children())
       checkForBG0inTop(child, found);
  }

  virtual void execute(const Exc_t&) {
     if ( !m_ctp ) return;
     for(TriggerItem* trItem : m_ctp->menu().items() ) {
        if(trItem->name().find("L1_BGRP")==0) continue; // the 8 BGRP_X triggers are special
        bool found = false;
        checkForBG0inTop(trItem->topNode(),found);
        if(!found) {
           if(m_offending!="") m_offending += ", ";
           m_offending += trItem->name();
        }
     }
     if(m_offending.size()!=0)
        m_error = "Items that are not correctly AND'ed with BGRP0: " + m_offending;
  }
  
private:
  std::string m_offending;
};

///////////////////////// The seeding btween L2 and L1 is consistent /////////////////////////
class ThrSeqMismatch : public TrigConfTest {
public:
   ThrSeqMismatch() 
      : TrigConfTest("ThrSeqMismatch", "Sequences of chains must seed from TEs used to make L1 item", WARNING)
   {}
   
   typedef std::pair<boost::regex,boost::regex> ExpPair_t;
   
   virtual void execute(const Exc_t& exceptions) {
      using std::set; using std::string; using std::map;
      if ( !m_hlt || !m_ctp ) return;


      string roibased_thresholds(" FJET JF JB EM TAU MUON XE TE XS JE JET CALREQ ");

      
      vector<boost::regex> exc_regex;
      vector<ExpPair_t> exc_regex_withinput;
   
      boost::regex pattern("([^-]*)(?:->(.*))?$");
      boost::cmatch matchgroup;  // cmatch = match_results<const char*>
      for(const string& s: exceptions ) {
         if(regex_match(s.c_str(), matchgroup, pattern) ) {
            string chainPattern(matchgroup[1].first, matchgroup[1].second);
            if(matchgroup[2].second-matchgroup[2].first==0) {
               exc_regex.push_back(boost::regex(chainPattern+"$"));
            } else {
               string inputPattern(matchgroup[2].first, matchgroup[2].second);
               exc_regex_withinput.push_back(ExpPair_t(boost::regex(chainPattern+"$"),boost::regex(inputPattern+"$") ));
            }
         }
      }

      for( HLTChain* ch: m_hlt->chains() ) {

         if( ch->level()=="EF" ) continue; // chains we do not care about in this check
         if( ch->lower_chain_name() == "") continue; // unseeded
         if( ch->lower_chain_name().find(",") != string::npos) continue; // multi seeded 

         if( matches_any(exc_regex, ch->chain_name()) ) continue; // listed in exception


         vector<boost::regex> input_exps;
         for(ExpPair_t exps: exc_regex_withinput)
            if(regex_match(ch->chain_name(), exps.first))
               input_exps.push_back(exps.second);
      

         // these are the TEs that the L2 chain needs from L1
         set<string> needed_tes;
         {
            set<string> produced_tes = HLTUtils::allTEsProducedInL2Chain( *ch, m_hlt->getHLTSequenceList());
      
            set<string> input_tes    = HLTUtils::inputTEs(produced_tes, m_hlt->getHLTSequenceList());

            set_difference( input_tes.begin(), input_tes.end(),
                            produced_tes.begin(), produced_tes.end(), 
                            inserter(needed_tes, needed_tes.begin()));
         }

        
         // the notProducedInChain should be L1
         // now we need the L1 thresholds per item
         const TriggerItem* item = m_ctp->menu().item(ch->lower_chain_name());
         if ( ! item )
            continue; // other check should grab this


         set<string> item_tes; // thresholds provided by input item
         vector<TriggerThreshold*> thresholds;
         item->topNode()->getAllThresholds(thresholds); 
         for(TriggerThreshold* t: thresholds) {
            //if ( roibased_thresholds.find(" "+t->type()+" ") != string::npos )
            item_tes.insert(t->name());	  
         }
      
         // we can now check if the two sets (needed and comming from L1) are the same
         set<string> missing_tes;
         std::set_difference( needed_tes.begin(), needed_tes.end(),
                              item_tes.begin(), item_tes.end(), 
                              std::inserter(missing_tes, missing_tes.begin()));


         // special treatment 1) for XE and XS triggers: seeding a chain of L1_XE<N> provides XE<M> thresholds for M<=N
         int max_xe_provided = 0;
         int max_xs_provided = 0;
         int max_J_provided = 0;
         int max_mu_provided = 0;
         boost::regex eXE("XE(\\d+)");
         boost::regex eXS("XS(\\d+)");
         boost::regex eJ("J(\\d+)");
         boost::regex eMU("MU(\\d+)");
         boost::cmatch matchRes;  // cmatch = match_results<const char*>
         for(string providedTE : item_tes) { // find the maximum provided threshold
            if( boost::regex_match(providedTE.c_str(),matchRes,eXE)) {
               int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
               if(thrVal>max_xe_provided) max_xe_provided = thrVal;
            }
            if( boost::regex_match(providedTE.c_str(),matchRes,eXS)) {
               int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
               if(thrVal>max_xs_provided) max_xs_provided = thrVal;
            }
            if( boost::regex_match(providedTE.c_str(),matchRes,eJ)) {
               int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
               if(thrVal>max_J_provided) max_J_provided = thrVal;
            }
            if( boost::regex_match(providedTE.c_str(),matchRes,eMU)) {
               int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
               if(thrVal>max_mu_provided) max_mu_provided = thrVal;
            }
         }

         //std::cout << "\n" << ch->chain_name() << " <-- " << ch->lower_chain_name() << ": XEmax" << max_xe_provided << " XSmax" << max_xs_provided << "  Jmax=" << max_J_provided << "  MUmax=" << max_mu_provided << std::endl;

         // special treatment 2) for XS triggers: seeding a chain of L1_XS<N> provides XE10 and XS15
         for(string providedTE : item_tes) { // find the maximum provided threshold
            if( boost::regex_match(providedTE.c_str(),eXS)) {
               if(10>max_xe_provided) max_xe_provided = 10;
               if(15>max_xs_provided) max_xs_provided = 15;
            }
         }

         boost::regex eJE("JE(\\d+)");
         for(string providedTE : item_tes) { // find the maximum provided threshold
            if( boost::regex_match(providedTE.c_str(),eJE)) {
               if(10>max_J_provided) max_J_provided = 10;
            }
         }




         if(missing_tes.size()>0) {
            set<string> tmp_missing_tes;
            for(string missingTE : missing_tes) {
               if( boost::regex_match(missingTE.c_str(),matchRes,eXE)) {
                  int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
                  if(thrVal>max_xe_provided) {
                     tmp_missing_tes.insert(missingTE);
                  }
               } else if( boost::regex_match(missingTE.c_str(),matchRes,eXS)) {
                  int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
                  if(thrVal>max_xs_provided) {
                     tmp_missing_tes.insert(missingTE);
                  }
               } else if( boost::regex_match(missingTE.c_str(),matchRes,eJ)) {
                  int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
                  if(thrVal>max_J_provided) {
                     tmp_missing_tes.insert(missingTE);
                  }
               } else if( boost::regex_match(missingTE.c_str(),matchRes,eMU)) {
                  int thrVal = boost::lexical_cast<int,std::string>(string(matchRes[1].first, matchRes[1].second));
                  if(thrVal>max_mu_provided) {
                     tmp_missing_tes.insert(missingTE);
                  }
               } else {
                  tmp_missing_tes.insert(missingTE);
               }
            }
            missing_tes = tmp_missing_tes;
         }


         // remove exceptions
         if(input_exps.size()>0) {
            set<string> tmp_missing_tes;
            for(string te : missing_tes) {
               if(! matches_any(input_exps, te)) { // no exception
                  tmp_missing_tes.insert(te);
               }
            }
            missing_tes = tmp_missing_tes;
         } 


         if ( ! missing_tes.empty() ) {
	
            // skip known exceptions
//             if (  exceptions.find(ch->chain_name()) != exceptions.end() ) {
//                continue;
//             }
            m_error +=  "\n                                  " + severity() + " chain " + ch->chain_name() + " seeded by " + item->name() +" - thresholds needed:";
            for(const string& th : needed_tes ) {
               m_error += " "+th; 	  
            }
            m_error += ", thresholds from L1 seed:";
            for( const string& th : item_tes ) {
               m_error += " "+th; 	  
            }
            m_error += ", hence missing: "; // + boost::lexical_cast<string,unsigned int>(missing_tes.size());
            for( const string& th : missing_tes ) {
               m_error += " "+th;
            }
	
         }
      }
   }
};

///////////////////////// CHECK THAT RECURSIVE RUNNING DOES NOT CROSS THE LEVEL (DOES NOT REQURE L2 SEQUENCES TO RUN AT EF) /////////////////////////
class CrossLevelAlgSeeding : public TrigConfTest {
public:
  CrossLevelAlgSeeding() 
    : TrigConfTest("CrossLevelAlgSeeding", "Check that recursive running does not cross levels"),
      m_offending("")
  {}
  
  virtual void execute(const Exc_t&) {
     using std::set; using std::string;
     if ( !m_hlt || !m_ctp ) return;

     // Set of all output TE's produced at L2
     set<std::string> l1thresholds;
     for(std::vector<TriggerThreshold*>::const_iterator thrIt = m_ctp->menu().thresholdConfig().getThresholdVector().begin();
         thrIt != m_ctp->menu().thresholdConfig().getThresholdVector().end(); ++thrIt)
        l1thresholds.insert((*thrIt)->name());


     set<std::string> outputTEsAllL2Chains = HLTUtils::allTEsProducedInL2( *m_hlt );

     // loop over chains
     for(const HLTChain* ch : m_hlt->getHLTChainList()) {
        if( ch->level()=="L2" ) continue;
        if( ch->lower_chain_name()=="" ) continue;

        // set of all TE's produced in the EF chain (including the recursively produced TE's)
        set<std::string> outputTEsEFChain = HLTUtils::allTEsProducedInEFChain( *ch, m_hlt->getHLTSequenceList(), outputTEsAllL2Chains);

        // set of corresponding input TE's
        set<std::string> inputTEsEFChain = HLTUtils::inputTEs( outputTEsEFChain, m_hlt->getHLTSequenceList() );

        // remove the unseeded algorithms
        inputTEsEFChain.erase("");

        // input TE's not produced in this chain
        set<string> notProducedInEFChain;
        std::set_difference( inputTEsEFChain.begin(), inputTEsEFChain.end(),
                             outputTEsEFChain.begin(), outputTEsEFChain.end(), 
                             std::inserter(notProducedInEFChain,notProducedInEFChain.begin()));


        // corresponding L2 chain
        const TrigConf::HLTChain* lowerch = m_hlt->getHLTChainList().chain(ch->lower_chain_name());
        set<std::string> outputTEsL2Chain = HLTUtils::allTEsProducedInL2Chain( *lowerch, m_hlt->getHLTSequenceList() );
        set<string> notProducedInL2Chain;
        std::set_difference( notProducedInEFChain.begin(), notProducedInEFChain.end(),
                             outputTEsL2Chain.begin(), outputTEsL2Chain.end(), 
                             std::inserter(notProducedInL2Chain,notProducedInL2Chain.begin()));

        set<string> notProducedInL1Either;
        std::set_difference( notProducedInL2Chain.begin(), notProducedInL2Chain.end(),
                             l1thresholds.begin(), l1thresholds.end(), 
                             std::inserter(notProducedInL1Either,notProducedInL1Either.begin()));


        if ( notProducedInL1Either.size()!=0 ) {
           if(m_offending!="") m_offending += ", ";
           m_offending += ch->chain_name() + "(requires ";
           for(set<string>::iterator teIt = notProducedInL1Either.begin(); teIt != notProducedInL1Either.end(); ++teIt) {
              m_offending += (*teIt);
              m_offending += ")";
           }
        }
     }
     if(m_offending.size()!=0)
        m_error = "EF chains that recursively call L2 sequences: " + m_offending;
  }
  
private:
  std::string m_offending;
};

//////////////////////////////////////// CHECK THAT CTP IDS ARE NOT USED TWICE //////////////////////////////
class CTPCountersUnique : public TrigConfTest {
public:
  CTPCountersUnique() 
    : TrigConfTest("CTPCountersUnique", "The CPT ID is not reused by the items"),
      m_offending("")
  {}
  
  virtual void execute(const Exc_t&) {
     if ( ! m_ctp ) return;

     unsigned int itemsNumber = m_ctp->menu().itemVector().size();
     std::map<int, std::string> id_to_name;
    
     for ( unsigned int i = 0; i < itemsNumber; i++ ) {
        std::map<int, std::string>::const_iterator found = id_to_name.find(m_ctp->menu().itemVector()[i]->ctpId());
        if ( found != id_to_name.end() ) {
           // we have gor offender
           m_offending += " "+m_ctp->menu().itemVector()[i]->name()+"&"+found->second;
        }
        id_to_name[m_ctp->menu().itemVector()[i]->ctpId()] = m_ctp->menu().itemVector()[i]->name();
     }
    
     if( ! m_offending.empty() ) {
        m_error = "Found L1 items using the same CTP ID:"+m_offending;
     }
  }
private:
  std::string m_offending;
};

/////////////////////////// SYMETRIC 1. ETA CUT 2. PRESENCE IN FORWARD/BACKWARD JETS ///////////////////////////
class FJBJThresholdSymetric : public TrigConfTest {
public:
  FJBJThresholdSymetric() 
    : TrigConfTest("FJBJThresholdSymetric", "FJ/BJ thresholds symetric in eta")
  {}
  virtual void execute(const Exc_t&) {
     if ( ! m_ctp ) return;
    
     std::map<std::string, TrigConf::TriggerThreshold*> forward_jets;
     std::map<std::string, TrigConf::TriggerThreshold*> backward_jets;

     // pick the thresholds
     std::vector<TrigConf::TriggerThreshold*>::const_iterator i;     
     const std::vector<TrigConf::TriggerThreshold*>& th = m_ctp->menu().thresholdVector(); // thresholdConfig()->getJfThresholdPointer();    

    

     for ( i = th.begin(); i != th.end(); ++i ) {
        if ("JF" == (*i)->type())
           forward_jets[(*i)->name()] = *i;
        //      (*i)->print("");
     }

     //th = m_ctp->menu().thresholdConfig()->getJbThresholdPointer();

     for ( i = th.begin(); i != th.end(); ++i ) {
        if ("JB" == (*i)->type())
           backward_jets[(*i)->name()] = *i;
        // (*i)->print("");
     }

     if ( forward_jets.size() != backward_jets.size() ) {
        m_error = "Different number of JF from JB";
        return;
     }

     std::map<std::string, TrigConf::TriggerThreshold*>::const_iterator f,b;
     for ( f = forward_jets.begin(); f != forward_jets.end(); ++f ) {
        // find corresponding backward threshold
        std::string bname("JB"+f->first.substr(2));
        b = backward_jets.find( bname );
        if ( b == backward_jets.end() ) {
           m_error = "Forward Jet Threshold "+f->first + " has no corresponding Backward Jet Threshold " + bname;
           return;
        }
        //      std::cerr << "Checking " << b->first << " " << f->first << std::endl;
        // get first threshold values
        unsigned fsz = f->second->thresholdValueVector().size();
        unsigned bsz = b->second->thresholdValueVector().size();
        if ( bsz != fsz || bsz != 2 ) {
           m_offending += " ThresholdValues number wrong (not equal or != 2): " +f->first +" " +b->first;
           return;
        }
        // the eta values in the first TriggerThresholdValue shoud be identical
        const TrigConf::JetThresholdValue* fv_prio1 = dynamic_cast<const TrigConf::JetThresholdValue*>(f->second->thresholdValueVector()[0]);
        const TrigConf::JetThresholdValue* bv_prio1 = dynamic_cast<const TrigConf::JetThresholdValue*>(b->second->thresholdValueVector()[0]);
        const TrigConf::JetThresholdValue* fv_prio2 = dynamic_cast<const TrigConf::JetThresholdValue*>(f->second->thresholdValueVector()[1]);
        const TrigConf::JetThresholdValue* bv_prio2 = dynamic_cast<const TrigConf::JetThresholdValue*>(b->second->thresholdValueVector()[1]);

      
        if(fv_prio1 && fv_prio2 && bv_prio1 && bv_prio2) {
           if(fv_prio1->priority()>fv_prio2->priority()) {
              const TrigConf::JetThresholdValue* tmp = fv_prio1; fv_prio1 = fv_prio2; fv_prio2 = tmp;
           }
           if(bv_prio1->priority()>bv_prio2->priority()) {
              const TrigConf::JetThresholdValue* tmp = bv_prio1; bv_prio1 = bv_prio2; bv_prio2 = tmp;
           }

           if ( fv_prio1->etamin() != bv_prio1->etamin() || fv_prio1->etamax() != bv_prio1->etamax() ) {
              m_offending += f->first +"/" +b->first + " (";
              if (fv_prio1->etamin() != bv_prio1->etamin())
                 m_offending += "min: " + boost::lexical_cast<std::string>(fv_prio1->etamin()) + "!=" + boost::lexical_cast<std::string>(bv_prio1->etamin()) + " ";
              if (fv_prio1->etamax() != bv_prio1->etamax())
                 m_offending += "max: " + boost::lexical_cast<std::string>(fv_prio1->etamax()) + "!=" + boost::lexical_cast<std::string>(bv_prio1->etamax()) + " ";
              m_offending += "), ";
              continue;
           }
        }


        //std::cerr << " " << fv->etamin() <<  " " <<  bv->etamax() << " " << fv->etamax()  <<  " "  << bv->etamin() << std::endl;
        if ( fv_prio2->etamin() != -1* bv_prio2->etamax() || fv_prio2->etamax() != -1*bv_prio2->etamin() ) {
           m_offending += " " +f->first +"/" +b->first + " "
              + boost::lexical_cast<std::string>(fv_prio2->etamin()) + "!=-1*" +  boost::lexical_cast<std::string>(fv_prio2->etamin()) +" or "
              + boost::lexical_cast<std::string>(fv_prio2->etamax()) + "!=-1*" +  boost::lexical_cast<std::string>(bv_prio2->etamin());
        }
     }
    
     if ( !m_offending.empty())
        m_error = "The eta symmetry is violated by thresholds: " +m_offending;
    
  }
private:
  std::string m_offending;
};

////////////////////////// CHECK IF FORWARD & BACKWARD JETS ARE ALWAYS USED IN PAIRS /////////////////////////////
class FJBJThrsholdsUsedInPairs : public TrigConfTest {
public:
  FJBJThrsholdsUsedInPairs()
    : TrigConfTest("FJBJThrsholdsUsedInPairs", "FJ and BJ always used in pairs")
  {}
  virtual void execute(const Exc_t&) {
     if ( ! m_ctp ) return;

     std::string offending;

     for( TriggerItem* item : m_ctp->menu().itemVector() ) {
        std::vector<TrigConf::TriggerThreshold*> thresholds; 
        item->topNode()->getAllThresholds(thresholds);
        // filter thresholds to contain forward and backward only
        std::vector<std::string> forward; 
        std::vector<std::string> backward; 
        for(TrigConf::TriggerThreshold* t : thresholds) {
           if ("FJ" == t->type())
              forward.push_back(t->name());
           if ("BJ" == t->type())
              backward.push_back(t->name());
        }
      
        if ( ! (forward == backward) ) {
           offending += item->name();
        }
     } // EOF loop over items

     if ( !offending.empty() )
        m_error = "FJ/BJ used not symetrically in items: " +offending;
  }
};

//////////////////////////////////////// SYMETRIC ETA CUT IN FORWARD/BACKWARD JETS //////////////////////////////
class CoherentPrescales : public TrigConfTest {
public: 
  CoherentPrescales() 
    : TrigConfTest("CoherentPrescales", "Coherent Prescales correctly configured")
  {}
  virtual void execute(const Exc_t& exceptions) {
     if ( ! m_hlt ) return;
     typedef std::map<std::string, std::vector<const TrigConf::HLTChain*> > map_t;
     map_t chains;
     std::string offending;

     std::vector<boost::regex> exc_regex = buildExcRec(exceptions);

     for(const TrigConf::HLTChain * ch : m_hlt->getHLTChainList() ) {
        unsigned int cnt = 0;
        for( const std::string& gr :  ch->groups() ) {
           if (gr.find("CPS:") != std::string::npos ) {
              cnt += 1;
              chains[gr].push_back(ch);
           }
        }
        if (cnt > 1 ) {
           offending += " only one CPS grp. for chain: " + ch->chain_name();
        }
     }
     // now we have map of chain group to chains
     // check 1 all chains are from the same level (to avoid confusion)
     for(map_t::value_type& cpsgr : chains) {
        std::string level = cpsgr.second[0]->level();
        std::string item =  cpsgr.second[0]->lower_chain_name();
        for(const TrigConf::HLTChain * ch : cpsgr.second) {
           if ( ch->level() != level )
              offending += " wrong trigger level: "+ch->chain_name();
	
           if ( ch->lower_chain_name() != item) 
              offending += " wrong seed: "+ch->chain_name();

           if ( (ch->lower_chain_name().find(",") != std::string::npos)
                && matches_any(exc_regex, ch->chain_name()) )  // complain about CPS on multiseeded chains unless it is explicitelly marked OK
              offending += " multi seeded chain can not use CPS: " + ch->chain_name();
        }
     }
     if ( ! offending.empty() ) {
        m_error += offending;
     }
  }
};


//////////////////////////////////////// SYMETRIC ETA CUT IN FORWARD/BACKWARD JETS //////////////////////////////
class OverlappingThrValues : public TrigConfTest {
public: 
   OverlappingThrValues() 
      : TrigConfTest("OverlappingThrValues", "Overlapping values for L1 threshold must have different priority")
   {}

   struct priobox_t {
      priobox_t(): etamin(0), etamax(0), phimin(0), phimax(0), priority(0)
      {};
      priobox_t(int etamin_, int etamax_, int phimin_, int phimax_, int priority_):
         etamin(etamin_),
         etamax(etamax_),
         phimin(phimin_),
         phimax(phimax_),
         priority(priority_)
      {};
      int etamin, etamax, phimin, phimax, priority;
      bool overlaps(const priobox_t& other) const {
         if(etamax<=other.etamin || 
            etamin>=other.etamax ||
            phimax<=other.phimin || 
            phimin>=other.phimax) return false;
         return true;
      }
      string s_etamin() { return boost::lexical_cast<std::string>(etamin); }
      string s_etamax() { return boost::lexical_cast<std::string>(etamax); }
      string s_phimin() { return boost::lexical_cast<std::string>(phimin); }
      string s_phimax() { return boost::lexical_cast<std::string>(phimax); }
      string s_priority() { return boost::lexical_cast<std::string>(priority); }
   };

   std::vector<priobox_t> m_boxes;


   bool overlapsWithSamePriority(const priobox_t& box, priobox_t& overlap) {
      for(priobox_t b : m_boxes) {
         if(box.overlaps(b) && box.priority==b.priority) {
            overlap = b;
            return true; 
         }
      }
      return false;
   }

   virtual void execute(const Exc_t& /*exceptions*/) {
      if ( ! m_ctp ) return;
      
      std::string offending("");
      //std::vector<boost::regex> exc_regex = buildExcRec(exceptions);

      for(const TrigConf::TriggerThreshold* thr :
              m_ctp->menu().thresholdConfig().getThresholdVector() ) {
         int cntGlobalValues(0);
         m_boxes.clear();
         for( const TrigConf::TriggerThresholdValue* thrVal :  thr->thresholdValueVector() ) {
            priobox_t b;
            const TrigConf::JetThresholdValue* jetThrVal = dynamic_cast<const TrigConf::JetThresholdValue*>(thrVal);
            if(jetThrVal!=0) {
               b = priobox_t(jetThrVal->etamin(),jetThrVal->etamax(), 
                             jetThrVal->phimin(), jetThrVal->phimax(),
                             jetThrVal->priority());
            } else {
               const TrigConf::ClusterThresholdValue* clusterThrVal = dynamic_cast<const TrigConf::ClusterThresholdValue*>(thrVal);
               if(clusterThrVal!=0) {
                  b = priobox_t(clusterThrVal->etamin(),clusterThrVal->etamax(), 
                                clusterThrVal->phimin(), clusterThrVal->phimax(),
                                clusterThrVal->priority());
               } else {
                  cntGlobalValues += 1;
                  continue;
               }
            }
            priobox_t obox;
            bool overlaps = overlapsWithSamePriority(b,obox);
            if(overlaps)
               offending +=  "\n                                  " + severity() + " threshold " + thr->name() 
                  + " has overlapping values: ("+b.s_etamin()+"-"+b.s_etamax()+","+b.s_phimin()+"-"+b.s_phimax()
                  + ") has same priority " + b.s_priority()
                  + " as ("+obox.s_etamin()+"-"+obox.s_etamax()+","+obox.s_phimin()+"-"+obox.s_phimax() +")";
            
            m_boxes.push_back(b);
         }
      }
      if ( ! offending.empty() ) {
         m_error += "Found overlapping threshold values" + offending;
      }
   }
};


// Check L1 name < 400 character
class L1SeedLenghtTest : public TrigConfTest {
public:
  L1SeedLenghtTest() 
    : TrigConfTest("Testing the lenght of the L1 Seed"),
      m_chains("")
  {}
  
  virtual void execute(const Exc_t&) {
    if ( ! m_hlt ) return;
    
    std::set<std::string> l2Chains;    
    std::set<std::string> l1items;
    
    for(const HLTChain* ch : m_hlt->getHLTChainList()) {      
      const std::string& lcn = ch->lower_chain_name();
      if ( lcn == "" ) continue;  // unseeded chains are fine
      if (lcn.size()> 4000.){     
	m_chains += ch->chain_name() + ", ";
      }
      
      if(m_chains.size()!=0)
	m_error = "The following chains have a L1 Item that is too long: " + m_chains;
      
    }
  }
private:
  std::string m_chains;
};



//////////////////////////////////////// CHECK NO L1CALO ITEM HAS THRESHOLD LOWER THAN IN MINTOBPT ////////////////////////////////////////
class MinTOBPtCheck : public TrigConfTest {
public:
   MinTOBPtCheck()
      : TrigConfTest("MinimumTOBPtCheck", "Check if all L1Calo items have threshold above MinTOBPt value", WARNING),
        m_belowMinTOBpt("")
   {}


   virtual void execute(const Exc_t&) {
     if ( !m_ctp ) return;

     const TrigConf::CaloInfo caloinfo = m_ctp->menu().thresholdConfig().caloInfo();

     unsigned int em_min  = caloinfo.minTobEM().ptmin;
     unsigned int tau_min  = caloinfo.minTobTau().ptmin;
     unsigned int Jsmall_min  = caloinfo.minTobJetSmall().ptmin;
     unsigned int Jlarge_min  = caloinfo.minTobJetLarge().ptmin;

     for(const TrigConf::TriggerThreshold* thr : m_ctp->menu().thresholdConfig().thresholdVector() ) {
       if(thr->type() == "EM" || thr->type() == "TAU" || thr->type() == "JET"){

         for( const TrigConf::TriggerThresholdValue* thrVal :  thr->thresholdValueVector() ) {

	   if(thr->type() == "EM" && thrVal->ptcut() < em_min) m_belowMinTOBpt += thr->name() + ", ";
	   if(thr->type() == "TAU" && thrVal->ptcut() < tau_min) m_belowMinTOBpt += thr->name() + ", ";

	   if(thr->type() == "JET"){
	     const TrigConf::JetThresholdValue* jetThrVal = dynamic_cast<const TrigConf::JetThresholdValue*>(thrVal);

	     if(jetThrVal->windowSizeAsString() == "LARGE" && thrVal->ptcut() < Jlarge_min) m_belowMinTOBpt += thr->name() + ", ";
	     if(jetThrVal->windowSizeAsString() == "SMALL" && thrVal->ptcut() < Jsmall_min) m_belowMinTOBpt += thr->name() + ", ";
	   }
	 }
       }
     }


      if(m_belowMinTOBpt.size()>0) {
         m_error = "L1 calo thresholds that are below the MinTOBPt: " + m_belowMinTOBpt;
      }
   }

private:
   std::string m_belowMinTOBpt;
};




//////////////////////////////////////// CHECK ALL PEB CHAINS GO INTO A CALIBRATION STREAM ////////////////////////////////////////
class PEBCalibCheck : public TrigConfTest {
public:
  PEBCalibCheck()
    : TrigConfTest("PEBCalibrationStream", "All streams with PEB must be of type calibration or monitoring", WARNING),
      m_nonCalibPEB("")
  {}

  virtual void execute(const Exc_t&) {
    if ( ! m_hlt ) return;


    // find all sequences using PEB with the corresponding output TE
    std::set<std::string> teOutFromSeq;

    for (const TrigConf::HLTSequence* seq : m_hlt->getHLTSequenceList()) {
      const std::vector<std::string>& algolist = seq->algorithms();

      for( std::string algoname : algolist ){

	if( algoname.find("TrigSubDetListWriter")!= std::string::npos || algoname.find("TrigROBListWriter")!= std::string::npos || algoname.find("ScoutingStreamWriter")!= std::string::npos){
	  const std::string& tename = seq->outputTE()->name();

	  if ( teOutFromSeq.count( tename ) == 0 ) {
            teOutFromSeq.insert( tename);
	  }

	  //	  m_nonCalibPEB += algoname + ": " + tename + ", ";
	}
      }
    }

    //loop over chains comparing signature output TEs to PEB TEs
    for(const HLTChain* ch : m_hlt->getHLTChainList()) {
      for ( unsigned int s = 0; s < ch->signatureList().size(); s++ ) {
	HLTSignature* sig = ch->signatureList()[s];
	for ( unsigned int t = 0; t < sig->outputTEs().size(); ++t ) {
	  if ( teOutFromSeq.count(sig->outputTEs()[t]->name()) != 0 ){

	    for(const HLTStreamTag *s : ch->streams()) {

	      //	      m_nonCalibPEB += ch->chain_name() + " stream type: " + s->type() + ", ";
	      if ( s->type() != "calibration" && s->type() != "monitoring" )  m_nonCalibPEB += ch->chain_name() + ", stream: " + s->stream() + ", stream type: " + s->type() + ", ";
	    }
	  }

	}
      }
    }

    if(m_nonCalibPEB.size()>0) {
      m_error = "Streams that use PEB but are not of type 'calibration':  " + m_nonCalibPEB;
    }
  }

private:
   std::string m_nonCalibPEB;
};






// set of tests
ConfigurationCheck::ConfigurationCheck(TrigConf::CTPConfig* ctp, TrigConf::HLTFrame* hlt) {
  m_tests.push_back(new NumberOfCTPItemsTest());
  m_tests.push_back(new CTPCountersUnique());
  m_tests.push_back(new ChainsNamingConventionTest());
  m_tests.push_back(new TENamingConventionTest());
  m_tests.push_back(new TENameLengthTest());
  m_tests.push_back(new ChainsNamingUniqueTest());
  m_tests.push_back(new ChainsCountersUniqueTest());
  m_tests.push_back(new ChainsCounterRangeTest());
  m_tests.push_back(new StreamTagPresentTest());
  m_tests.push_back(new GroupPresentTest());
  m_tests.push_back(new BWGroupPresentTest());
  m_tests.push_back(new UniqueSignatureCountTest());
  m_tests.push_back(new NonEmptySignature());
  m_tests.push_back(new LowerLevelMatchingTest());
  m_tests.push_back(new LowerLevelUsedTest(2));
  m_tests.push_back(new LowerLevelUsedTest(1));
  m_tests.push_back(new SignaturesProducibleTest());
  m_tests.push_back(new SequenceOutputUniqueTest());
  m_tests.push_back(new SequenceInputProducibleTest());
  m_tests.push_back(new BGRP0Test());
  m_tests.push_back(new EFTEsFromSeedingL2ChainsTest());
  //m_tests.push_back(new CrossLevelAlgSeeding());
  //m_tests.push_back(new FJBJThresholdSymetric());
  m_tests.push_back(new FJBJThrsholdsUsedInPairs());
  //m_tests.push_back(new ThrSeqMismatch()); //CB temp commented out (ATR-10063)
  m_tests.push_back(new CoherentPrescales());
  m_tests.push_back(new L1CaloThrDef());
  //m_tests.push_back(new L1CaloThrOrder()); //CB 23.05.2014: disabled for now, need to check with L1Calo about eta items order
  m_tests.push_back(new OverlappingThrValues());

  m_tests.push_back(new L1SeedLenghtTest());
  //m_tests.push_back(new SignaturesConnectedTest());

  m_tests.push_back(new MinTOBPtCheck());
  m_tests.push_back(new PEBCalibCheck());

  std::vector<TrigConfTest*>::iterator testIt;
  for ( testIt = m_tests.begin(); 
	testIt != m_tests.end(); ++testIt ) {
    (*testIt)->setCTP(ctp);
    (*testIt)->setHLT(hlt);
  }
}

// destructor
ConfigurationCheck::~ConfigurationCheck() {

   std::vector<TrigConfTest*>::iterator itTest = m_tests.begin();
   std::vector<TrigConfTest*>::iterator lastTest = m_tests.end();
   for (;itTest != lastTest; itTest++) {
      delete *itTest; *itTest = 0;
   }
   m_tests.clear();
}
