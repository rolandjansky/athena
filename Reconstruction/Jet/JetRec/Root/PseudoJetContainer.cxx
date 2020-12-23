/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetContainer.cxx

#include "JetRec/PseudoJetContainer.h"
#include "JetRec/IConstituentExtractor.h"
#include <map>
#include "xAODJet/Jet.h"
#include "fastjet/PseudoJet.hh"
#include "JetRec/LineFormatter.h" // helper class for debug printing
#include <sstream>
#include <ios>
#include <iostream>
#include <exception>

using fastjet::PseudoJet;

PseudoJetContainer::PseudoJetContainer() {
  checkInvariants("PseudoJetContainer()");
}

PseudoJetContainer::PseudoJetContainer(std::unique_ptr<const IConstituentExtractor> c,
                                       const std::vector<PseudoJet> & vecPJ,
                                       bool debug){

  m_debug = debug;
  
  if (vecPJ.empty()){
    m_emptyExtractors.insert(std::move(c));
    return;
  }

  if (m_debug){checkInConstituents(vecPJ, "constuctor");}
  m_allConstituents = vecPJ;

  // the limits of the Extractor index range correposnd to the 
  // numbering of the EDM objects in th extractors.
  m_extractorRanges.emplace_back(0, 
                                 vecPJ.back().user_index(), 
                                 std::move(c));
  if (m_debug){checkInvariants("PseudoJetContainer(vcPJ, c)");}
}

bool
PseudoJetContainer::extractConstituents(xAOD::Jet& jet, 
                                        const std::vector<PseudoJet>& inConstits) const
{
  
  // check container integrity if m_debug ==  true
  if (m_debug){checkInvariants("extractConstituents()");}
  
  // set up mapping to acquire the EDM indices by Extractor
  std::map<const IConstituentExtractor*, std::vector<int>> sorter;

  // add an entry into the sorter for each extractor.
  // this way each extractor will be called, possibly with
  // an empty index list - this will happen if no pseudojet correponding
  // to the extractor is received. But an empty list is used 
  // by the extractors to fill zeros into the jet.
  for(const auto& er : m_extractorRanges){
    sorter.emplace(er.m_e.get(), std::vector<int>{});
  }

  // see header file for explanation of empty extractors.
  for(const auto& e : m_emptyExtractors){
    sorter.emplace(e.get(), std::vector<int>{});
  }

  // check whether inputs are lgal if m_debug == true
  if (m_debug){checkInConstituents(inConstits, "extractConsituents()");}
  
  for (const auto& pj : inConstits){
    
    int pj_ind = pj.user_index();

    // fastjet clustering may add in pseudojets with the default (-1) index.
    // ignore these.
    if (pj_ind == -1) {continue;} 

    // The index of the pseudojet and the low limit 
    // the extractorRange table (which acts as an offset), identifies which
    // extractorRanges entry to use, and the EDM object that corrsponds to the
    // pseudojet.

    auto e = std::find_if(m_extractorRanges.begin(),
                          m_extractorRanges.end(),
                          [pj_ind](const ExtractorRange& r){
                            return pj_ind >= r.m_lo and pj_ind <= r.m_hi;});

    if(e == m_extractorRanges.end()){return false;}
   
    sorter[(*e).m_e.get()].push_back(pj_ind - (*e).m_lo);
  }

  // send the jet to the each extractor with a vector of EDM
  // indices for that extractor
  for(auto s : sorter){s.first -> addToJet(jet, s.second);}
  return true;
}


// fill xAOD jet with constit&ghosts extracted from final PSeudoJet
bool PseudoJetContainer::extractConstituents(xAOD::Jet& jet, 
                                             const PseudoJet& finalPJ) const {

  std::vector<PseudoJet> inConstituents = finalPJ.constituents();
  return extractConstituents(jet, inConstituents);
}

const std::vector<PseudoJet>* PseudoJetContainer::casVectorPseudoJet() const {
  if (m_debug){checkInvariants("asVectorPseudoJet()");}
  return &m_allConstituents;
}

void PseudoJetContainer::append(const PseudoJetContainer* other) {
  // Adds the extractors and pseudo jets  for other to this container.
  
  // add the other extractor and its range to the ExtractorRanges table
  int offset = m_extractorRanges.empty() ? 0 : m_extractorRanges.back().m_hi+1;

  std::transform(other->m_extractorRanges.begin(),
                 other->m_extractorRanges.end(),
                 std::back_inserter(m_extractorRanges),
                 [offset](const ExtractorRange& e){return e.bump(offset);});

  // copy over the pseudojets for the extractor being processed,
  // and set the user index to match th extractorRange limits.

  
  std::transform(other->m_allConstituents.begin(),
                 other->m_allConstituents.end(),
                 std::back_inserter(m_allConstituents),
                 [offset](fastjet::PseudoJet pj){
                   pj.set_user_index(pj.user_index() + offset);return pj;}
                 );

  for(const auto &e : other->m_emptyExtractors){m_emptyExtractors.emplace(e->clone());}

  if (m_debug){checkInvariants("append()");}
}

std::string PseudoJetContainer::toString(int level, int extLevel) const {
  std::ostringstream oss{"PseudoJetContainer at: ", std::ios::ate};
    oss << this
        << "\ndump levels: "<< level << " " << extLevel << '\n'
        << " ExtractorRanges ranges: [" << m_extractorRanges.size() <<"] " ;
    for(const auto& er : m_extractorRanges){
      oss << "(" << er.m_lo << "," <<er.m_hi << "), ";
    }
  
  oss << " empty extractors: " << m_emptyExtractors.size();
  oss << " Pseudojets: " << m_allConstituents.size();
  
  if (level > 0){
    oss << "\n Extractor dump: \n";
    for(const auto& er : m_extractorRanges){
      oss << "Extractor at [" ;
      oss << er.m_e.get();
      oss << "]\n";
      oss << er.m_e->toString(extLevel) << '\n';
    }
  }

  if(level > 1){ oss << dumpPseudoJets();}
 
  return oss.str();
}

std::string PseudoJetContainer::dumpPseudoJets() const {
  std::ostringstream oss{"PseudoJetContainer at: ", std::ios::ate};
  oss <<"\n PseudoJet energies\n";
  for(auto& p : m_allConstituents){
    oss << "pseudojet user ind " << p.user_index()
        << " E " << p.e() << " " << p.eta() << '\n';
  }
  
  return oss.str();
}


bool PseudoJetContainer::checkInvariants(const std::string& from) const {
  
  if(!m_debug){return true;}
  
  std::ostringstream 
    oss("PseudoJetContainer checkInvariants called from: ",
        std::ios::ate);
  oss << from << " ";
  
  if (m_extractorRanges.empty()){
    if(! m_allConstituents.empty()){
      oss << "No extractors, but pseudojets present";
      return bad_invariants_exit(oss);
    }
    return true;
  }

  // have at least one extractor
  // m_lo == m_hi possible if an empty EDM container has been read in.
  for(auto ex : m_extractorRanges){
    if (ex.m_lo < 0 or ex.m_lo > ex.m_hi){
      oss << "bad extractor limits: " << ex.m_lo << " " << ex.m_hi;
      return bad_invariants_exit(oss);
    }
  }
                                         
  // have at least one extractor with non-zero limits
  auto npj = m_allConstituents.size();
  auto uExtCount = (m_extractorRanges.back()).m_hi;
  if (uExtCount < 0){
    oss << " Illegal extractor range limit " << uExtCount;
    return bad_invariants_exit(oss);
  }

  std::size_t uuExtCount = uExtCount;
  
  //The user index for a pseudojet is an index into the EDM objet 
  // array. The comparison here is between th size of the PseudoJet
  // array and the index of th EDM objects. When each EDM object
  // gives rise to a PsedoJet, these numbers differ by 1. 
  // Not all EDM objcts give rise to a PseudoJet, so an inequality 
  // is tested here.
  if (npj > uuExtCount + 1){
    oss << " Mismatch between pseudo jet count/upper Extractor limit "
        << npj << "/" << uuExtCount;
    return bad_invariants_exit(oss);
  }
  
  // have pseudoJets
  
  auto firstInd = m_allConstituents[0].user_index();
  if (firstInd != 0){
    oss << " first pseudojet does not have index 0 " << firstInd;
    return bad_invariants_exit(oss);
  }

  auto nent = m_extractorRanges.size();
  
  for (std::size_t i = 0; i < nent-1; ++i){
    auto cur = m_extractorRanges[i];
    auto next = m_extractorRanges[i+1];
    if (cur.m_hi != next.m_lo - 1){
      oss << "Hi limit for extractor " << i  
          << " != low limit for extractor " << i+1; 
      return bad_invariants_exit(oss);
    }
  }
  
  if(!checkPseudoJetVector(m_allConstituents, "checkInvariants()")) {
    return bad_invariants_exit(oss);
  }

  for(const auto& pj : m_allConstituents){
    try{
      pj.e();
    } catch(...){
      oss << "PseudoJet.e() fails of PJ in m_allConstituents";
      return bad_invariants_exit(oss);
    }
  }

  for(const ExtractorRange& er : m_extractorRanges){
    if (!((er.m_e)->checkIntegrity())){
        oss << "IConstituentExtractor fails integrity test";
        return bad_invariants_exit(oss);
      }
  }
    
  oss << "No of PseudoJets: "<< m_allConstituents.size()
      << " No of Extractors " << m_extractorRanges.size() <<" all OK";
  
  return true;
}

bool PseudoJetContainer::checkPseudoJetVector(const std::vector<PseudoJet>& v, 
                                              const std::string& from) const{
  
  // Pseudojets in th vector v contain the indices into the
  // the EDM object vector held in the Extractors. If there is more than
  // one extractor, an extractor dependent offset is added to index.
  // The indices should be ordered, and start from a value >= 0.

  if(!m_debug){return true;}

  std::ostringstream 
    oss("PseudoJetContainer checkPseudoJetVector called from: ",
        std::ios::ate);
  oss << from << " ";
  
  if(v.empty()){
    oss << "Empty pseudojet vector " ;
    return true;  // empty not a problem
  }

  oss <<"PseudoJetVector energy dump:\n";
  std::vector<float> energies;
  energies.reserve(v.size());
  std::transform(v.begin(),
                 v.end(),
                 std::back_inserter(energies),
                 [](const PseudoJet& p){return p.e();});

  LineFormatter formatter(10);  // dump the energies, 10 values/line.
  oss << formatter(energies);

      
  bool ok{true};
  if (v[0].user_index() < 0){
    ok = false;
    oss << "First pj has index < 0: " << v[0].user_index();
  }
    
  std::vector<int> indices(v.size());
  std::transform(v.begin(),
                 v.end(), 
                 std::back_inserter(indices),
                 [](const PseudoJet & p){return p.user_index();});
  
  if(!std::is_sorted(indices.begin(), indices.end())){
    oss << "Pseudojets out of order ";
    ok = false;
  }  

  if(ok){oss << v.size() << " checked ok";}
 
  return ok;
}

bool 
PseudoJetContainer::checkInConstituents(const std::vector<PseudoJet>& v,
                                        const std::string& from) const {

  if(!m_debug){return true;}
  
  std::ostringstream 
    oss("PseudoJetContainer checkInConstituents called from: ",
        std::ios::ate);
  oss << from << " ";
  oss << v.size() << " entries\n";
  
  std::vector<double> energies;
  energies.reserve(v.size());
  std::transform(v.begin(),
                 v.end(), 
                 std::back_inserter(energies),
                 [](const PseudoJet & p){return p.e();});
  
  
  LineFormatter formatter(10);  // dump the energies, 10 values/line.
  oss << formatter(energies);
  
  for(const auto& pj: v){
    if(pj.user_index() == -1){oss << "PJ index -1, E() " << pj.E() << '\n';}
  }

  std::vector<int> indices;
  indices.reserve(v.size());
  std::transform(v.begin(),
                 v.end(), 
                 std::back_inserter(indices),
                 [](const PseudoJet & p){return p.user_index();});
  
  oss <<"PseudoJet indices\n";
  oss << formatter(indices);


  std::map<int, int> counter;
  if(!m_extractorRanges.empty()) {
    int lo = m_extractorRanges[0].m_lo;
    int hi = m_extractorRanges.back().m_hi;
    
    for (auto ind : indices){
      // ind = -1 pseudojets can be produced by fastjet.
      if (ind == -1){continue;}
      if (ind < lo or ind > hi){++(counter[ind]);}
    }
  }
  
  bool bad = !counter.empty();
  
  if(bad){
    oss <<"Out of range values[counts]: ";

    for(auto ent: counter){oss << ent.first << "/" << ent.second << '\n';}
    oss << "Extractor ranges: \n";
    for(const auto& er: m_extractorRanges){
      oss << er.m_lo << " " << er.m_hi << '\n';
    }

    bad_invariants_exit(oss);
  }
  
  
  return true;
}

bool 
PseudoJetContainer::bad_invariants_exit(const std::ostringstream& oss) const {

  auto errMsg = oss.str();

  if (m_debug) {
    std::cerr << errMsg << '\n';
    throw std::runtime_error(errMsg);
  }

  return false;
}
  
std::size_t PseudoJetContainer::size() const {
  return m_allConstituents.size();
}

bool  PseudoJetContainer::debug() const{ return m_debug;}
void  PseudoJetContainer::debug(bool b) const {m_debug  = b;}
std::ostream& operator << (std::ostream& os, const PseudoJetContainer& c){
  os <<  c.toString(0);
  return os;
}
