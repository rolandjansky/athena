/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>

#include "TrigConfL1Data/HelperFunctions.h"

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTFrame.h"

#include "TrigConfHLTData/HLTUtils.h"

using namespace TrigConf;
using namespace std;

std::mutex HLTUtils::s_mutex;

std::string HLTUtils::s_newCategory("##NewCategory");

//
// \brief function used to generate uniqu  ID (integer) from string
//        In fact uniqueness is not 100% guaranteed. Thread safe
// \param s string to be hashed
//
// \return key for this string (this function never fails)
//
namespace HashChecking {
  typedef std::map<HLTHash, std::string> HashMap;
  typedef std::map<std::string, HashMap> CategoryMap;
  static CategoryMap AllHashesByCategory;
  void checkGeneratedHash (HLTHash hash,  const std::string& s,   const std::string& category) {
    std::lock_guard<std::mutex> lock(HLTUtils::s_mutex);
    HashMap& hashes = AllHashesByCategory[category];
    if ( hashes[hash] == "" )
      hashes[hash] = s;
    else if ( hashes[hash] != s )
      throw std::domain_error("Hashes the same for category: "+category
			      + " and elements "+ hashes[hash] + " "+ s );
  }
}

HLTHash HLTUtils::string2hash( const std::string& s, const std::string& category )
{
  // hash function (based on available elswhere ELF hash function)
  // uniqueness tested in MC way; contact me for details
  // email: Tomasz.Bold@cern.ch
  HLTHash hash;
  hash = 0xd2d84a61;
  int i;

  for ( i = (int)s.size()-1; i >= 0; --i )
    hash ^= ( hash >> 5) + s[i] + ( hash << 7 );

  for ( i = 0; i < (int)s.size(); ++i )
    hash ^= ( hash >> 5) + s[i] + ( hash << 7 );

    
  HashChecking::checkGeneratedHash(hash, s, category);

  return hash;
}

const std::string HLTUtils::hash2string( HLTHash hash, const std::string& category ) {
  std::lock_guard<std::mutex> lock(s_mutex);
  HashChecking::CategoryMap::const_iterator mapForCategoryIt = HashChecking::AllHashesByCategory.find(category);
  if ( mapForCategoryIt == HashChecking::AllHashesByCategory.end() ) {
    return "UNKNOWN CATEGORY";
  }
  HashChecking::HashMap::const_iterator hashMapIt = mapForCategoryIt->second.find(hash);
  if ( hashMapIt == mapForCategoryIt->second.end() ) 
    return "UNKNOWN HASH ID";
  return hashMapIt->second;
}

void HLTUtils::hashes2file( const std::string& fileName) {
  std::lock_guard<std::mutex> lock(s_mutex);
  std::ofstream fout(fileName);
  for (const auto& categoryMapElement : HashChecking::AllHashesByCategory) {
    const std::string& categoryName = categoryMapElement.first;
    const HashChecking::HashMap& categoryMap = categoryMapElement.second;
    fout << s_newCategory << std::endl << categoryName << std::endl;
    for (const auto& mapElement : categoryMap) {
      const HLTHash hash = mapElement.first;
      std::string name = mapElement.second;
      name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
      fout << hash << std::endl << name << std::endl;
    }
  }
}

void HLTUtils::file2hashes( const std::string& fileName) {
  std::ifstream fin(fileName);
  if (!fin.is_open()) {
    return;
  }
  std::string line;
  std::string category;
  // Note: this method is a to aid with development/debugging. 
  // It won't be used in production code, hence it is light on error checking.
  while(std::getline(fin, line)) {
    if (line == s_newCategory) {
      std::getline(fin, category);
      continue;
    }
    HLTHash hash = std::stoul(line);
    std::string name;
    std::getline(fin, category);
    HLTHash check = string2hash(name, category);
    if (check != hash) {
      std::cerr << "Inconsistency in file2hashes(" << fileName << ") function,"
                   " item " << name << " has hash " << hash << " not " << check << std::endl;
    }
  }
}


/*****
 * This is to support discover in configuration recursively called sequences
 ****/
class CompareSequenceOutput {
public:
   CompareSequenceOutput( unsigned int hashId ) { m_hashId = hashId; }
   CompareSequenceOutput( const std::string& name ) { m_hashId = HLTUtils::string2hash(name, "TE"); }
   bool operator()( TrigConf::HLTSequence* const & s ) { return  s->outputTE()->hashId() == m_hashId; }
private:
   unsigned int m_hashId;
};


void
recursivelyFindInputTEs( unsigned int teId, const TrigConf::HLTSequenceList& seqList, std::vector<unsigned int>& tes ) {
   const TrigConf::HLTSequence * seq = seqList.getSequence(teId);
   if ( seq==0 ) return;
   for(TrigConf::HLTTriggerElement* te : seq->inputTEs() ) {
      tes.push_back(te->hashId());
      recursivelyFindInputTEs( te->hashId(), seqList, tes);
   }
}

void recursivelyFindInputTEs( std::string tename, const TrigConf::HLTSequenceList& seqList, std::vector<std::string>& tes ) {
  const TrigConf::HLTSequence* seq = seqList.getSequence(tename);
  if ( seq==0 ) return;

  for( TrigConf::HLTTriggerElement* te : seq->inputTEs() ) {
    tes.push_back(te->name());
    recursivelyFindInputTEs(te->name(), seqList, tes);
  }
}

namespace TrigConf {

   void
   recursivelyFindOutputTEs( const std::string& tename,
                             const TrigConf::HLTSequenceList& sequenceList, 
                             std::set<std::string>& tes,
                             int level,
                             const std::set<std::string>* veto = 0 )
   {
      if ( veto && veto->find(tename) != veto->end() ) return;
      const TrigConf::HLTSequence* seq = sequenceList.getSequence(tename);
      if ( seq==0 ) return;

      if(level==2) seq->setL2(); 
      else if(level==3) seq->setEF(); 
      else if(level==4) seq->setHLT();

      tes.insert(tename);
      for( TrigConf::HLTTriggerElement* te : seq->inputTEs() )
         recursivelyFindOutputTEs( te->name(), sequenceList, tes, level, veto);
   }

   void
   recursivelyFindOutputTEsWithLevelFromSequence( const std::string& tename,
                                                  const TrigConf::HLTSequenceList& sequenceList, 
                                                  std::set<std::string>& tes, int level ) {
      
      // check if sequence exists that outputs this TE
      const TrigConf::HLTSequence* seq = sequenceList.getSequence(tename); 
      if ( seq==0 ) return;

      // it must know in which level it is running
      if(! seq->isSet()) { 
         std::stringstream str;
         str << "Sequence " << seq->outputTE()->name() << " has no level set " << std::endl;
         throw std::runtime_error(str.str());
      }

      // it must be the level that is requested
      if( (level==2 && !seq->inL2()) || (level==3 && !seq->inEF()) || (level==4 && !seq->inHLT()) ) return;

      // now insert the te and continue to check
      tes.insert(tename);
      for( TrigConf::HLTTriggerElement*  te : seq->inputTEs() )
         recursivelyFindOutputTEsWithLevelFromSequence( te->name(), sequenceList, tes, level);
   }
}

/*****************************/


std::vector<unsigned int>
HLTUtils::allTEsProducedInLevel( const std::string& level, 
                                 const HLTChainList& chainList,
                                 const HLTSequenceList& sequenceList) {
   

   set<unsigned int> outTEs;
   for( HLTChain* ch : chainList) {
      if ( ch->level() != level ) continue;
      for( HLTSignature* sig : ch->signatureList() ) {
         for( HLTTriggerElement* te : sig->outputTEs() )
            outTEs.insert( te->hashId() );
      }
   }
   // we have now big list of TEs mentioned in chains ... what with this which are not mentioned
   // but are executed because recursion in sequences
   std::vector<unsigned int> newTEs;
   for( unsigned int teId : outTEs ) 
      recursivelyFindInputTEs(teId, sequenceList, newTEs);
   copy(newTEs.begin(), newTEs.end(),inserter(outTEs,outTEs.begin()));

   // now turn into vector and order (unnecessary) but I don't want to break the interface
   std::vector<unsigned int> tes;
   copy(outTEs.begin(), outTEs.end(), inserter(tes,tes.begin()));
   sort(tes.begin(), tes.end());
   return tes;
}


vector<string>
HLTUtils::explicitChainTEs(const HLTChain& ch) {
   vector<string> tes;
   for( HLTSignature* sig : ch.signatureList() ) {
      for( HLTTriggerElement* te : sig->outputTEs() )
         tes.push_back( te->name() );
   } 
   sort(tes.begin(), tes.end());
   std::vector<std::string>::iterator l = unique(tes.begin(), tes.end());
   tes.erase(l, tes.end());
   return tes;
}


std::vector<std::string>
HLTUtils::implicitChainTEs(const HLTChain& ch, const HLTSequenceList& sequenceList) {
   vector<string> tes = explicitChainTEs(ch);

   vector<string> newTes;
   for(const string& teId : tes )
      recursivelyFindInputTEs(teId, sequenceList, newTes);
  
   sort(tes.begin(), tes.end());
   vector<string>::iterator l = unique(tes.begin(), tes.end());
   tes.erase(l, tes.end());
   return tes;
}


// ________________________________________________________________________________
std::set<std::string>
TrigConf::HLTUtils::allTEsProducedInL2( const HLTFrame& frame ) {

   const HLTChainList& chainList = frame.getHLTChainList();
   const HLTSequenceList& sequenceList = frame.getHLTSequenceList();

   set<string> l2tes;
   for( HLTChain* chain : chainList ) {
      if ( chain->level() == "EF" ) continue;
      set<string> tes = allTEsProducedInL2Chain( *chain, sequenceList);
      copy(tes.begin(), tes.end(), std::inserter(l2tes,l2tes.begin()));
   }
   return l2tes;
}


// ________________________________________________________________________________
std::set<std::string>
TrigConf::HLTUtils::allTEsProducedInEF( const HLTFrame& frame,
                                        std::set<std::string>* l2tes ) 
{
   const HLTChainList& chainList = frame.getHLTChainList();
   const HLTSequenceList& sequenceList = frame.getHLTSequenceList();

   set<string> level2TEs = l2tes?*l2tes:allTEsProducedInL2(frame);

   set<string> eftes;
   for( HLTChain* chain : chainList ) {
      if ( chain->level() == "L2" ) continue;
      set<string> tes = allTEsProducedInEFChain( *chain, sequenceList, level2TEs);
      copy(tes.begin(), tes.end(), std::inserter(eftes,eftes.begin()));
   }
   return eftes;
}


// ________________________________________________________________________________
std::set<std::string>
TrigConf::HLTUtils::allTEsProduced( const HLTFrame& frame )
{
   using std::set; using std::string; using std::vector;

   set<string> tes = allTEsProducedInL2( frame );
   set<string> eftes = allTEsProducedInEF( frame, &tes );

   std::copy(eftes.begin(),eftes.end(),
             std::inserter(tes,tes.begin()));
   return tes;
}






// ________________________________________________________________________________
std::set<std::string>
TrigConf::HLTUtils::inputTEs( const std::set<std::string>& TEs,
                              const TrigConf::HLTSequenceList& sequenceList) {

   using std::set; using std::string;

   // get the input TEs
   set<string> inputTEs;
   for(set<string>::iterator te = TEs.begin(); te != TEs.end(); ++te) {
      const TrigConf::HLTSequence* seq = sequenceList.getSequence(*te);
      if(seq==0) {
         std::stringstream str;
         str << "TE " << *te << " is not produced by any sequence " << std::endl;
         throw std::runtime_error(str.str());
      }
      for(vector<HLTTriggerElement*>::const_iterator teIt = seq->inputTEs().begin(); teIt != seq->inputTEs().end(); ++teIt)
         inputTEs.insert((*teIt)->name());
   }
   return inputTEs;
}



// ________________________________________________________________________________
std::set<std::string>
TrigConf::HLTUtils::allTEsProducedInL2Chain( const TrigConf::HLTChain& chain,
                                             const TrigConf::HLTSequenceList& sequenceList)
{
   using std::set; using std::string; using std::vector;

   set<string> l2tes;
   // explicitly produced TEs
   const vector< HLTSignature * >& sigVec = chain.signatureList();
   for ( vector< HLTSignature * >::const_iterator sig = sigVec.begin(); sig != sigVec.end(); ++sig ) {
      const vector< HLTTriggerElement * >& teVec = (*sig)->outputTEs();
      for ( vector< HLTTriggerElement * >::const_iterator te = teVec.begin(); te != teVec.end(); ++te)
         l2tes.insert((*te)->name());
   }

   // get the input TEs
   set<string> inputTEs;
   for(set<string>::iterator outTEIt = l2tes.begin(); outTEIt != l2tes.end(); ++outTEIt) {
      const TrigConf::HLTSequence* seq = sequenceList.getSequence(*outTEIt);
      if(seq==0) {
         std::stringstream str;
         str << "TE " << *outTEIt << " is not produced by any sequence " << std::endl;
         throw std::runtime_error(str.str());
      }
      seq->setL2();
      for(vector<HLTTriggerElement*>::const_iterator teIt = seq->inputTEs().begin(); teIt != seq->inputTEs().end(); ++teIt)
         inputTEs.insert((*teIt)->name());
   }

   // still to check
   set<string> check;
   std::set_difference(inputTEs.begin(),inputTEs.end(), l2tes.begin(),l2tes.end(), std::inserter(check,check.begin()));

   for(set<string>::iterator teIt = check.begin(); teIt != check.end(); ++teIt)
      recursivelyFindOutputTEs( *teIt, sequenceList, l2tes, 2 );

   return l2tes;
}




// ________________________________________________________________________________
std::set<std::string>
TrigConf::HLTUtils::allTEsProducedInEFChain( const TrigConf::HLTChain& chain,
                                             const TrigConf::HLTSequenceList& sequenceList,
                                             const std::set<std::string>& l2tes )
{
   using std::set; using std::string; using std::vector;

   set<string> eftes;
   // explicitly produced TEs
   const vector< HLTSignature * >& sigVec = chain.signatureList();
   for ( vector< HLTSignature * >::const_iterator sig = sigVec.begin(); sig != sigVec.end(); ++sig ) {
      const vector< HLTTriggerElement * >& teVec = (*sig)->outputTEs();
      for ( vector< HLTTriggerElement * >::const_iterator te = teVec.begin(); te != teVec.end(); ++te)
         eftes.insert((*te)->name());
   }

   // get the input TEs
   set<string> inputTEs;
   for(set<string>::iterator outTEIt = eftes.begin(); outTEIt != eftes.end(); ++outTEIt) {
      const TrigConf::HLTSequence* seq = sequenceList.getSequence(*outTEIt);
      if(seq==0) {
         std::stringstream str;
         str << "TE " << *outTEIt << " is not produced by any sequence " << std::endl;
         throw std::runtime_error(str.str());
      }
      seq->setEF();
      for(vector<HLTTriggerElement*>::const_iterator teIt = seq->inputTEs().begin(); teIt != seq->inputTEs().end(); ++teIt)
         inputTEs.insert((*teIt)->name());
   }

   // still to check
   set<string> check;
   std::set_difference(inputTEs.begin(),inputTEs.end(), eftes.begin(), eftes.end(), std::inserter(check,check.begin()));

   for(const string& te : check)
      recursivelyFindOutputTEs( te, sequenceList, eftes, 3, &l2tes );

   return eftes;
}




// ________________________________________________________________________________
std::string
TrigConf::HLTUtils::ChainCondenseDisplay( const HLTChain& chain,
                                          const HLTFrame& frame )
{
   using std::string; using std::set; using std::vector; using TrigConf::HLTTriggerElement;
   using TrigConf::HLTSignature; using TrigConf::HLTSequenceList;

   const HLTSequenceList& sequenceList = frame.getHLTSequenceList();

   std::string teCat("");

   set<string> writtenOutput;
   set<string> neededInput;
  
   unsigned int currentSigPos = 1;

   const vector<HLTSignature*> & signatures = chain.signatureList();
   vector<HLTSignature*>::const_iterator sigIt = signatures.begin();
   for(; sigIt!=signatures.end(); ++sigIt) {
      unsigned int sigCounter = (*sigIt)->signature_counter();
      while(sigCounter>currentSigPos) {
         teCat +=";"; // separate signature by ';'
         currentSigPos++;
      }
      const vector<HLTTriggerElement*> & outputTEs = (*sigIt)->outputTEs();
      vector<HLTTriggerElement*>::const_iterator teIt = outputTEs.begin();
      for(;teIt!=outputTEs.end();++teIt) {
         if(teIt!=outputTEs.begin()) teCat +=","; // separate sequences by ','
         const std::string& outTEName((*teIt)->name());
         const TrigConf::HLTSequence* seq = sequenceList.getSequence(outTEName);
         if(seq==0) {
            std::stringstream str;
            str << "Chain " << chain.chain_name() << " at step " << sigCounter
                << " requires TE " << outTEName << ", which is not produced by any sequence " << std::endl;
            throw std::runtime_error(str.str());
         }
         teCat += seq->concise();

         // store info about input and output TE's
         for(vector<HLTTriggerElement*>::const_iterator teIt = seq->inputTEs().begin(); teIt != seq->inputTEs().end(); ++teIt)
            neededInput.insert((*teIt)->name());
         writtenOutput.insert(seq->outputTE()->name());
      }
   }

   teCat = "|" + teCat;

   set<string> stillToWrite;
   std::set_difference(neededInput.begin(),neededInput.end(),
                       writtenOutput.begin(),writtenOutput.end(),
                       std::inserter(stillToWrite,stillToWrite.begin()));

   set<string> allStillToWrite(stillToWrite);
   int ilevel = (chain.level()=="L2")?2:3;
   for( const string& te : stillToWrite)
      recursivelyFindOutputTEsWithLevelFromSequence( te, sequenceList, allStillToWrite, ilevel );

   bool first=true;
   for(set<string>::iterator teIt = allStillToWrite.begin(); teIt != allStillToWrite.end(); ++teIt) {
      const std::string& outTEName(*teIt);
      const TrigConf::HLTSequence* seq = sequenceList.getSequence(outTEName);
      if(seq==0) continue;
      if(!first) { first=false; teCat = "," + teCat; } // separate sequences by ','  // TODO careful not to change COOL format JS
      teCat = seq->concise() + teCat;
   }
   return teCat;
}


// ________________________________________________________________________________
std::vector< std::string >
TrigConf::HLTUtils::splitGroups(const std::string& s)
{
   // turns "(str1),(str2),(str3)" into ["str1", "str2", "str3"]
   std::string::size_type currentOpen = 0;

   std::vector< std::string > result;

   bool openParenthesis = false;
   for(std::string::size_type pos = 0; pos!=s.size(); ++pos) {
      char cc = s[pos];
      if(cc=='(') {
         if(openParenthesis) {
            std::stringstream str;
            str << "Two many open parenthesis in " << s << std::endl;
            throw std::runtime_error(str.str());
         }
         openParenthesis = true;
         currentOpen = pos;
      }
      if(cc==')') {
         if(!openParenthesis) {
            std::stringstream str;
            str << "Two many closing parenthesis in " << s << std::endl;
            throw std::runtime_error(str.str());
         }
         openParenthesis = false;
         result.push_back(std::string(s, currentOpen+1, pos-currentOpen-1));
      }
   }
   return result;
}

TrigConf::HLTSequence*
TrigConf::HLTUtils::buildSequence(const std::string& desc)
{
   // builds a sequence from a string "inputTE1,inputTE2,outputTE", (can be just "outputTE")
   std::vector<std::string> tes = TrigConf::split(desc,",");
   
   vector<TrigConf::HLTTriggerElement*> input;
   for(std::vector<std::string>::iterator teit = tes.begin(); teit!=tes.end(); ++teit) {
      input.push_back(new TrigConf::HLTTriggerElement(0,*teit));
   }
   TrigConf::HLTTriggerElement* outte = input.back(); input.pop_back();

   TrigConf::HLTSequence* seq = new TrigConf::HLTSequence( input, outte, vector<string>());

   return seq;
}


//merge L2 and EF chains
void TrigConf::HLTUtils::mergeHLTChainList( HLTFrame& frame) {

   if(! frame.mergedHLT()) return;
  
   vector<HLTChain*>       tmpL2chains; 
   vector<HLTChain*>       tmpEFchains; 
   for( TrigConf::HLTChain* aChain : frame.theHLTChainList() ){
      if  (aChain->level() == "L2"){
         tmpL2chains.push_back( aChain ); //add in the tmp list
      }
      else if  (aChain->level() == "EF"){
         tmpEFchains.push_back( aChain ); //add in the tmp list
      }
   }
   //delete the current chainlist content
   frame.theHLTChainList().clear();

   // for (  vector<HLTChain*>::iterator aChain = frame.getHLTChainList().begin(); aChain != frame.getHLTChainList().end(); ) {
   //   if  ((*aChain)->level() == "L2"){
   //     tmpL2chains.push_back( *aChain ); //add in the tmp list
   //     frame.getHLTChainList().chains().erase(aChain); // erase L2 chains pointers
   //   }
   //   else aChain++; //iterate if not erase
   // }


   for (  vector<HLTChain*>::iterator cEF = tmpEFchains.begin(); cEF != tmpEFchains.end(); ++cEF ) {
      //  for (  vector<HLTChain*>::iterator cEF = frame.getHLTChainList().begin(); cEF != frame.getHLTChainList().end(); ++cEF ) {
      // iterator of the signature list
      std::vector<TrigConf::HLTSignature*>::iterator it = (*cEF)->signatureList().begin();       
      string cname = (*cEF)->chain_name();
      if ( size_t index = ((*cEF)->chain_name().find("EF_")) != std::string::npos ){
         cname.replace(index-1, 3, "HLT_");// why index must be scaled by 1 ?
         (*cEF)->set_chain_name(cname);
      }
      (*cEF)->set_level("HLT");
      (*cEF)->set_EB_after_step(0);//in case EF chain is not seeded by any L2 chain
      for (  vector<HLTChain*>::iterator cL2 = tmpL2chains.begin(); cL2 != tmpL2chains.end(); ++cL2 ) {
         if ((*cL2)->chain_name() == (*cEF)->lower_chain_name()){
            // insert the L2 signatures:
            (*cEF)->signatureList().insert (it,(*cL2)->signatureList().begin(),(*cL2)->signatureList().end()); // add the L2 signatures, before EF	 
            // update the EF signature counters
            for (unsigned int sig=0; sig < (*cEF)->signatureList().size(); sig++){
               (*cEF)->signatureList()[ sig ]->set_signature_counter( sig + 1 );
            }
            // set the lower chain name as the L2 lower
            (*cEF)->set_lower_chain_name((*cL2)->lower_chain_name());
            (*cEF)->set_lower_chain_counter((*cL2)->lower_chain_counter());
            // set the prescales, rerun_prescales and pass_through
            int prescale(0);
            if ((*cL2)->prescale()==-1. || (*cEF)->prescale()==-1.) prescale=-1.;
            else prescale=(*cL2)->prescale()*(*cEF)->prescale();
            (*cEF)->set_prescale(prescale);
            int rerun_prescale(0);
            if (((*cL2)->rerun_prescale("")).second<0 || ((*cEF)->rerun_prescale("")).second<0) rerun_prescale=-1.;
            else rerun_prescale=((*cL2)->rerun_prescale("")).second * ((*cEF)->rerun_prescale("")).second;
            (*cEF)->set_rerun_prescale(rerun_prescale);
            int pass_through(0);
            if ((*cL2)->pass_through()==-1. || (*cEF)->pass_through()==-1.) pass_through=-1.;
            else pass_through=(*cL2)->pass_through()*(*cEF)->pass_through();
            (*cEF)->set_pass_through(pass_through);
            // add the EB_after_step
            (*cEF)->set_EB_after_step( (*cL2)->signatureList().size());
            /*    	std::cout << "FPP: changed name to "  << (*cEF)->chain_name() 
                  << " level to "                     << (*cEF)->level() 
                  << "; set lower_chain_name to "     << (*cEF)->lower_chain_name() 
                  << "; set EB_after_step="           << (*cEF)->EB_after_step() 
                  << "; set prescale="                << (*cEF)->prescale()
                  << "; set rerun_prescale="          << (*cEF)->rerun_prescale()
                  << "; set pass_through="            << (*cEF)->pass_through()
                  << "; added signatures tot="        << (*cEF)->signatureList().size()<< std::endl;  */
            break;
         }
      }
      frame.theHLTChainList().addHLTChain(*cEF);
   }

   //std::cout<<"FPP HLTUtils::mergeHLTChains(): frame.getHLTChainList().chains().size()="<<frame.getHLTChainList().size()<<std::endl;
   return;
}




void
TrigConf::HLTUtils::mergeL2EFPrescales( HLTChain* hltchain, const HLTPrescale& l2ps, const HLTPrescale& efps) {
   HLTPrescale hltps;

   bool disabled = l2ps.disabled() || efps.disabled();

   float prescale = fabs( l2ps.prescale() * efps.prescale() ) * (disabled?-1:1);
   hltps.setPrescale(prescale);
   
   hltchain->set_prescales(hltps);
}

void
TrigConf::HLTUtils::mergeHLTChainList2( HLTFrame& frame) {

   if(! frame.mergedHLT()) return;
   
   // we need to keep the steps in sync so first we find the last step in L2
   unsigned int lastL2step(0);
   for( TrigConf::HLTChain* chain : frame.chains() ) {
      if(chain->level_enum() != L2) continue;
      lastL2step = max(lastL2step, chain->lastStep());
   }

   // now make copies of all EF chains, the steps are
   // 1. make a deep copy of all EF chains
   // 2. give them level HLT and modify the name
   // 3. insert the signatures (deep copies) of the matching L2 chains at the front
   // 4. set the input L1Item condition
   // 5. merge the prescales, streams

   vector<HLTChain*> newHLTchains; 
   for( TrigConf::HLTChain* chain : frame.chains() ) {
      if( chain->level_enum() != EF ) continue;


      // make the copy (deep copy of all signatures and output TEs)
      HLTChain * hltChain = new HLTChain(*chain);
      newHLTchains.push_back(hltChain);

      // keep track of chain counters (for correct prescale merging)
      hltChain->mergeCounter.ef = hltChain->chain_counter();

      // set the name and level to HLT
      // if name starts with EF_ it is replace with HLT_, otherwise HLT_ is prepended
      hltChain->set_level("HLT");
      string oldname = chain->name();
      unsigned int basepos = (oldname.substr(0,3)=="EF_")?3:0; 
      hltChain->setName( "HLT_" + oldname.substr(basepos) );

      
      hltChain->set_EB_after_step(0); //in case EF chain is not seeded by any L2 chain

      // shift the step counters of by +lastL2Step (note that there is no step=0)
      hltChain->shiftStepCounter(lastL2step);


      // find the lower chain
      const HLTChain * l2Chain = frame.chains().chain( hltChain->lower_chain_name() );

      // if no seeding l2 chain is found, nothing needs to be done
      if(l2Chain) {

         hltChain->mergeCounter.l2 = l2Chain->chain_counter();

         // insert l2 steps at the beginning
         // first need to create a deep copy
         vector<HLTSignature*> l2sig_deepcopy;
         for(HLTSignature* sig : l2Chain->signatures())
            l2sig_deepcopy.push_back(new HLTSignature(*sig));

         hltChain->signatureList().insert( hltChain->signatureList().begin(),
                                           l2sig_deepcopy.begin(), l2sig_deepcopy.end());

         // set L1 input
         hltChain->set_lower_chain_name( l2Chain->lower_chain_name() );
         hltChain->set_lower_chain_counter( l2Chain->lower_chain_counter() );

         // set the EB start after old L2 chain
         hltChain->set_EB_after_step( l2Chain->signatureList().size());
         
         // set prescales, pass throughs, rerun prescales, stream prescales
         
         // prescale (product of L2 and EF, disabled if either were disabled)

         mergeL2EFPrescales(hltChain, l2Chain->prescales(), hltChain->prescales());
//          bool disabled = l2Chain->prescale()<0 || hltChain->prescale()<0;
//          float prescale = l2Chain->prescale() * hltChain->prescale();
//          prescale = fabs(prescale) * (disabled?-1:1);
//          hltChain->prescales().setPrescale(prescale);
      }
      
      // TODO: rerun prescales, streams, 
      //       int rerun_prescale(0);
      //       if (((*cL2)->rerun_prescale("")).second<0 || ((*cEF)->rerun_prescale("")).second<0) rerun_prescale=-1.;
      //       else rerun_prescale=((*cL2)->rerun_prescale("")).second * ((*cEF)->rerun_prescale("")).second;
      //       (*cEF)->set_rerun_prescale(rerun_prescale);
      //       int pass_through(0);
      //       if ((*cL2)->pass_through()==-1. || (*cEF)->pass_through()==-1.) pass_through=-1.;
      //       else pass_through=(*cL2)->pass_through()*(*cEF)->pass_through();
      //       (*cEF)->set_pass_through(pass_through);
      //       // add the EB_after_step
      //       (*cEF)->set_EB_after_step( (*cL2)->signatureList().size());
      
      // TODO deal with L2-only chains

   }

   // delete the current chainlist content
   frame.theHLTChainList().clear();

   // and add the new chains
   for( HLTChain* chain : newHLTchains )
      frame.theHLTChainList().addHLTChain(chain);

   return;
}
