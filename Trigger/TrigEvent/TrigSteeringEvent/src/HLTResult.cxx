/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/HLTExtraData.h"

#include "AthenaKernel/CLASS_DEF.h"
#include <cassert>
#include <algorithm>
#include <numeric>
#include <utility>
#include <map>
#include <set>
using namespace HLT;

///
/// \file HLTResult.cxx
/// \author andreas.hoecker@cern.ch till.eifert@cern.ch nicolas.berger@cern.ch tomasz.bold@cern.ch

/// version of HLTResult class: change this upon schema evolution
const uint32_t HLTResult::m_HLTResultClassVersion = 3;

/// constructor
HLTResult::HLTResult() :
      m_extraData{nullptr}
{
  m_headerResult.resize(IndNumOfFixedBit);
  m_headerResult[IndHLTResultClassVersion]    = m_HLTResultClassVersion;
  //std::cerr << "HLTResult instantiated with version " << m_HLTResultClassVersion << std::endl; 
  // set all fixed bits, if there will be an error then they will be OK ??? mean showing an error
  setLvl1Id(0xffffffff);
  setAccepted(false);

  setHLTStatus(HLT::FATAL);

  setLvlConverterStatus(HLT::FATAL);

  setPassThrough(false);

  setHLTLevel(HLT::UNKNOWN);

  setNumOfSatisfiedSigs(0);

  setErrorCoordinates(0, 0xffffffff); // quite unique way of saying that this was not used

  setCreatedOutsideHLT(true);

  setHLTResultTruncated(false);
}

/// copy constructor
HLTResult::HLTResult(const HLTResult& rhs)
  : m_headerResult{rhs.m_headerResult},
    m_chainsResult{rhs.m_chainsResult},
    m_navigationResult{rhs.m_navigationResult},
    m_navigationResult_DSonly{rhs.m_navigationResult_DSonly},
    m_extras{rhs.m_extras},
    m_id_name{rhs.m_id_name},
    m_id_name_DSonly{rhs.m_id_name_DSonly},
    m_modID_id_name{rhs.m_modID_id_name},
    m_navigationResultCuts{rhs.m_navigationResultCuts},
    m_navigationResultCuts_DSonly{rhs.m_navigationResultCuts_DSonly},
    m_extraData{rhs.m_extraData ? new HLTExtraData{*rhs.m_extraData} : nullptr}
{
}

// move ctor
HLTResult::HLTResult(HLTResult&& rhs)
  : HLTResult{}
{
  swap(*this, rhs);
}

// unifying assignment operator
HLTResult& HLTResult::operator=(HLTResult rhs)
{
  swap(*this, rhs);
  return *this;
}

/// destructor
HLTResult::~HLTResult() {
  delete m_extraData;
}

void HLTResult::listOfModIDs(std::vector<unsigned int>& mod_ids) const{

  for (std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::const_iterator map_it = m_modID_id_name.begin(); 
      map_it != m_modID_id_name.end(); ++map_it)
    mod_ids.push_back((*map_it).first);
}

std::vector<unsigned int> HLTResult::listOfModIDs() const{

  std::vector<unsigned int> mod_ids;
  for (std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::const_iterator map_it = m_modID_id_name.begin(); 
      map_it != m_modID_id_name.end(); ++map_it)
    mod_ids.push_back((*map_it).first);
  return mod_ids;
}

bool HLTResult::error() const {
  return (getHLTStatus().action() >= HLT::Action::ABORT_CHAIN) ? true : false; 
}

uint32_t HLTResult::error_bits() const {
  uint32_t bits = 0;
  // result of erros in previous levels (not correct previous level result)
  if (getLvlConverterStatus().action() != HLT::Action::CONTINUE)  bits |= 0x1;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::NO_LVL1_ITEMS)     bits |= 0x2;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::NO_LVL2_CHAINS)    bits |= 0x4;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::WRONG_HLT_RESULT)  bits |= 0x8;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::NO_HLT_RESULT)     bits |= 0x10;

  // result of erros in this level
  if (getHLTStatus().action() == HLT::Action::ABORT_JOB)             bits |= 0x100;
  if (getHLTStatus().action() == HLT::Action::ABORT_EVENT)           bits |= 0x200;
  if (getHLTStatus().action() == HLT::Action::ABORT_CHAIN)           bits |= 0x400;
  if (getHLTStatus().reason() == HLT::Reason::TIMEOUT)               bits |= 0x800;
  if (getHLTStatus().reason() == HLT::Reason::NAV_ERROR)             bits |= 0x1000;
  return bits;
}

unsigned int HLTResult::estimateSize() const {
  return IndNumOfFixedBit
      + m_chainsResult.size() + 1      // size(one word) and payload
      + m_navigationResult.size() + 1
      + m_extras.size() + 1;
}

namespace
{
  /*
   * Serialize an indivisible section of the result (e.g. header)
   */
  bool serialize_indivisible(uint32_t* output,
                             int& data_size,
                             const std::vector<uint32_t>& indivisible,
                             unsigned int umax_size,
                             bool truncating,
                             bool first=false)
  {
    auto size_indivisible = indivisible.size();
    auto size_needed = data_size + size_indivisible + (first ? 0 : 1); /*
                                          we add a word for the size unless this
                                          is the first indivisible we serialize
                                          */

    if(truncating && size_needed > umax_size)
      return false; // we do not have space for this indivisible

    if (!first)
      output[data_size++] = size_indivisible; // follow existing data with size

    std::copy(std::begin(indivisible),
              std::end(indivisible),
              output + data_size);
    data_size += size_indivisible;

    return true;
  }

  /*
   * serialize, the collections in nav whose boundaries are provided into output
   * output must have enough space. The pairs in boundaries must specify proper
   * boundaries. That is, each pair "bpair" must respect the following
   * conditions:
   *  1) bpair.first in [0, nav.size())
   *  2) bpair.second in [bpair.first, nav.size()]
   */
  void serialize_collections(uint32_t * output,
      int& data_size,
      const std::vector<uint32_t>& nav,
      const std::vector<std::pair<unsigned, unsigned>>& boundaries)
  {
    const auto nav_begin = std::begin(nav);
    for(const auto& bpair : boundaries)
    {
      assert(bpair.first < nav.size());
      assert(bpair.second >= bpair.first);
      assert(bpair.second <= nav.size());

      std::copy(nav_begin + bpair.first,
                nav_begin + bpair.second,
                output + data_size);
      data_size += bpair.second - bpair.first;
    }
  }

  /*
   * Find, in the provided cuts, the boundaries of the collections that are in
   * idname_include but not in idname_exclude
   */
  void
  find_cuts(const std::set<std::pair<CLID, std::string>>& collections,
            const std::vector<std::pair<CLID, std::string>>& idname_include,
            const std::vector<std::pair<CLID, std::string>>& idname_exclude,
            std::vector<unsigned int> cuts,
            std::vector<std::pair<unsigned int, unsigned int>>& out)
  {
    for(const auto& coll : collections)
    {
      if(std::find(std::begin(idname_exclude),
                   std::end(idname_exclude),
                   coll) == std::end(idname_exclude))
      { // collection not marked for exclusion
        auto it = std::find(std::begin(idname_include),
                            std::end(idname_include),
                            coll);
        if(it != std::end(idname_include)) // this
        { // collection marked for inclusion
          auto pos = std::distance(std::begin(idname_include), it);
          out.emplace_back(cuts.at(pos + 1),
                           cuts.at(pos + 2));
        }
      }
    }
  }

  /*
   * Addition of the size of a collection, as specified by its cut boundaries,
   * to an existing (running) sum
   */
  inline
  unsigned add_collection_to_size(unsigned current_size,
                                  const std::pair<unsigned, unsigned>& pair)
  {
    return current_size + pair.second - pair.first;
  }

  /*
   * Calculate the total size of the collections whose boundaries are provided
   */
  inline
  unsigned int
  calc_colls_size(const std::vector<std::pair<unsigned, unsigned>>& boundaries)
  {
    return std::accumulate(std::begin(boundaries), std::end(boundaries), 0,
                           add_collection_to_size);
  }
}

auto HLTResult::findDSCuts(unsigned int mod_id) const -> CutPairVecs
{

  auto cuts_dsonly = CutPairs{{0, m_navigationResultCuts_DSonly.at(1)}}; /* the
                              first cut pair contains the preamle for DS
                              navigation: {version, NavSize, TEsSize, NumTEs} */
  auto cuts_reg = CutPairs{}; // starts empty

  auto modid_coll_it = m_modID_id_name.find(mod_id);
  if(modid_coll_it != std::end(m_modID_id_name))
  {
    const auto& collections = modid_coll_it->second;
    find_cuts(collections,
              m_id_name_DSonly,
              {},
              m_navigationResultCuts_DSonly,
              cuts_dsonly); // cuts for collections only in the DS result
    find_cuts(collections,
              m_id_name,
              m_id_name_DSonly,
              m_navigationResultCuts,
              cuts_reg); // cuts for collections in the normal result
  }

  return std::make_pair(cuts_dsonly, cuts_reg);
}

/* static */
unsigned int HLTResult::calc_total_size_DS(unsigned int ds_nav_size)
{
  static constexpr auto num_size_words = 3u;
  return num_size_words + IndNumOfFixedBit + ds_nav_size;
}

bool HLTResult::serialize_navigation_reg(uint32_t* output,
                                         int& data_size,
                                         unsigned int umax_size,
                                         bool truncating) const
{
  auto calc_size_needed = [data_size](unsigned int nav_size)
                          { return nav_size + data_size + 1; }; /* given a
                                             navigation size, this gives the
                                             total space needed in the output */
  auto is_within_size = [umax_size, &calc_size_needed](unsigned int cut)
                        { return calc_size_needed(cut) <= umax_size; }; /* tells
                                             whether the given cut would make it
                                             within the allowed size */

  if(static_cast<unsigned int>(data_size) < umax_size)
  { // there is still some space (even if we end up truncating)
    const auto tot_nav_size = m_navigationResult.size(); // cache the total size
    auto cut_nav_size = tot_nav_size;                    // init cut size
    auto endnav = std::end(m_navigationResult);          // and cut point

    if(truncating && calc_size_needed(tot_nav_size) > umax_size)
    { // Truncation falls in navigation, so find the last cut that still fits
      auto cutit = std::find_if(m_navigationResultCuts.rbegin(),
                                m_navigationResultCuts.rend(),
                                is_within_size);
      cut_nav_size = (cutit == m_navigationResultCuts.rend()) ? 0 : *cutit;
      endnav -= tot_nav_size - cut_nav_size;
    }

    assert(endnav == std::begin(m_navigationResult) + cut_nav_size); /* endnav
                                 now marks the cutting point, while cut_nav_size
                                 says how many nav words we are copying */

    output[data_size++] = cut_nav_size;
    copy(std::begin(m_navigationResult), endnav, output+data_size);
    data_size += cut_nav_size;

    return endnav == std::end(m_navigationResult); /* whether we went with the
                                                      full navigation */
  }

  return false; // we did not even have space for the size
}

bool HLTResult::serialize_navigation_DS(uint32_t* output,
                                        int& data_size,
                                        unsigned int umax_size,
                                        unsigned int nav_size,
                                        const CutPairVecs& dscuts,
                                        bool truncating) const
{
  if(nav_size + data_size < umax_size)
  { // we have space for the navigation + 1 size word
    output[data_size++] = nav_size;
    auto index_for_size_in_nav_preamble = data_size + 1;

    serialize_collections(output, data_size, m_navigationResult_DSonly,
                          dscuts.first);
    serialize_collections(output, data_size, m_navigationResult, dscuts.second);

    output[index_for_size_in_nav_preamble] = nav_size; /* the total size needs
      to be replaced in the navigation preamble as it was originally
      calculated with all data scouting collections lumped together in the
      navigation. This needs to be corrected once headers can be changed and
      serialization can be requested for each module ID separately.
      TODO: remove when changing headers */
  }
  else
  {
    assert(truncating);

    if(static_cast<unsigned int>(data_size) < umax_size)
      output[data_size++] = 0; /* we can still write the size; (partial
                                  navigation not supported in DS) */
  }

  return !truncating;
}

bool HLTResult::serialize_bootstrap(uint32_t*& output,
                                    int& data_size,
                                    bool& truncating,
                                    int max_size,
                                    unsigned int estimated_size)
{
  assert(!data_size);
  assert(!output);

  auto umax_size = static_cast<unsigned int>(max_size);
  output = new uint32_t[std::min(umax_size, estimated_size)];

  truncating = max_size >= 0 && estimated_size > umax_size;
  setHLTResultTruncated(truncating);

  // serialize header
  return serialize_indivisible(output, data_size, m_headerResult, umax_size,
                               truncating, /*first*/true);
}

inline
bool HLTResult::serialize_body_regular(uint32_t* output,
                                       int& data_size,
                                       unsigned int umax_size,
                                       bool truncating) const
{
  return serialize_indivisible(output, data_size, m_chainsResult, umax_size,
                               truncating) &&
         serialize_navigation_reg(output, data_size, umax_size, truncating) &&
         serialize_indivisible(output, data_size, m_extras, umax_size,
                               truncating);
}

bool HLTResult::serialize_body_DS(uint32_t* output,
                                  int& data_size,
                                  unsigned int umax_size,
                                  unsigned int nav_size,
                                  const CutPairVecs& dscuts,
                                  bool truncating) const
{
  if(static_cast<unsigned int>(data_size) < umax_size)
  { // still at least space for chains size
    output[data_size++] = 0; // no chains
    if(serialize_navigation_DS(output, data_size, umax_size, nav_size, dscuts,
                               truncating)
       && static_cast<unsigned int>(data_size) < umax_size)
    { // we managed to serialize without truncating and have at least 1word left
      output[data_size++] = 0; // no extras
      return true; // no truncation
    }
  }

  return false;
}

bool HLTResult::serialize_regular(uint32_t*& output,
                                  int& data_size,
                                  int max_size)
{
  updateExtras();
  bool truncating;
  auto estim_size = estimateSize();
  return serialize_bootstrap(output, data_size, truncating,
                             max_size, estim_size) &&
         serialize_body_regular(output, data_size,
                                max_size, truncating);
}

bool HLTResult::serialize_DS(uint32_t*& output,
                             int& data_size,
                             int max_size,
                             unsigned int mod_id)
{
  assert(mod_id);

  bool truncating;
  auto dscuts = findDSCuts(mod_id);
  auto navsize = calc_colls_size(dscuts.first) + calc_colls_size(dscuts.second);

  return serialize_bootstrap(output, data_size, truncating, max_size,
                             calc_total_size_DS(navsize)) &&
         serialize_body_DS(output, data_size, max_size, navsize, dscuts,
                           truncating);
}

inline
bool HLTResult::serialize( std::vector<uint32_t>& output )
{
  return serialize(output, /*mod_id*/ 0);
}

bool HLTResult::serialize(std::vector<uint32_t>& output,
                          const unsigned int mod_id)
{
  uint32_t * aux = nullptr;
  int data_size = 0;

  auto ret = serialize(aux, data_size, /*max_size*/ -1, mod_id);
  auto uptr = std::unique_ptr<uint32_t[]>{aux}; // takes care of deletion

  output.reserve(data_size);
  std::copy(aux, aux + data_size, std::back_inserter(output));

  return ret;
}

inline
bool HLTResult::serialize(uint32_t*& output,
                          int& data_size,
                          const int max_size,
                          const unsigned int mod_id)
{
  return mod_id ? serialize_DS(output, data_size, max_size, mod_id)
                : serialize_regular(output, data_size, max_size);
}

inline
void HLTResult::updateExtras()
{
  if (m_extraData)
  { // the extraData object has been used, so serialize again into m_extras
    m_extras.clear();
    m_extraData->serialize(m_extras);
  }
}

bool HLTResult::deserialize( const std::vector<uint32_t>& source ) {
  return unpackFromStorable(source);
}


bool HLTResult::deserialize(  uint32_t* source, const int data_size ) {
  if (data_size == 0 ) return false;
  std::vector<uint32_t> rawResult(&source[0], &source[data_size]);
  return unpackFromStorable(rawResult);
}

bool HLTResult::unpackFromStorable(const std::vector<uint32_t>& raw)
{
  if (raw.empty())
    return false;

  // default assumption: fixed bits of raw data are same as present class
  unsigned int rawIndNumOfFixedBit = HLTResult::IndNumOfFixedBit;

  unsigned version = raw[0];

  // different version handling
  if ( version != m_HLTResultClassVersion){
    if (version == 1) {
      // version 1 had only 12 InitBits
      rawIndNumOfFixedBit = 12;
    } else if (version == 2 ) {
      // nothing to be done
    } else { // case we don't know about
      std::cerr << "HLTResult::unpackFromStorable has noticed a class version mismatch and does not know how to translate: raw data version " << raw[0] << " current class version " << m_HLTResultClassVersion << std::endl; 
      return false;
    }

  }
  if ( raw.size() < HLTResult::IndNumOfFixedBit )
    return false;
  m_headerResult.clear();
  m_headerResult.reserve(HLTResult::IndNumOfFixedBit);
  m_headerResult.insert(m_headerResult.end(), &raw[0], &raw[rawIndNumOfFixedBit]);

  // fill up with zeros so use of HLTResult::IndNumOfFixedBit of other indices past the end doesn't break
  m_headerResult.insert(m_headerResult.end(), HLTResult::IndNumOfFixedBit-rawIndNumOfFixedBit, 0);

  if ( raw.size() ==  rawIndNumOfFixedBit )
    return true; // that's OK, we have just empty event, no processing started

  // chains
  uint32_t offset = rawIndNumOfFixedBit;
  uint32_t sizeOfChains = raw[offset];
  offset++;

  uint32_t readEnd = offset + sizeOfChains;
  bool truncation = false;

  if ( readEnd > raw.size() ){
    readEnd = raw.size();
    truncation = true;
  }


  m_chainsResult.clear();
  m_chainsResult.reserve( sizeOfChains );
  m_chainsResult.insert( m_chainsResult.end(), &raw[offset], &raw[readEnd] );
  offset += sizeOfChains;

  if (truncation) {
    if ( isHLTResultTruncated() ) // if truncation was marked by creator it is OK
      return true;
    else // we need to report problems as there was extra truncation in the way which might have been done w/o any care about content
      return false;
  }

  // navigation
  uint32_t sizeOfNavigation = raw[offset];
  offset++;
  uint32_t trueSizeOfNavigation = sizeOfNavigation;
  // check if offset is not beyond size of raw result  


  readEnd = offset + sizeOfNavigation;
  if (readEnd > raw.size()) {
    readEnd = raw.size();
    truncation = true;
    trueSizeOfNavigation = raw.size() - offset;
  }


  if ( offset > readEnd ) 
    return true;

  m_navigationResult.clear();
  m_navigationResult.reserve(trueSizeOfNavigation);
  m_navigationResult.insert(m_navigationResult.end(), &raw[offset], &raw[readEnd]);


  if (truncation) {
    if ( isHLTResultTruncated() )
      return true;
    else
      return false;
  }
  offset += sizeOfNavigation;

  if (version >= 3) {
    // extras
    uint32_t sizeOfExtras = raw[offset];
    offset++;

    readEnd = offset + sizeOfExtras;
    if (readEnd > raw.size()) {
      readEnd = raw.size();
      truncation = true;
    }

    if ( offset > readEnd )
      return true;

    m_extras.clear();
    m_extras.reserve(sizeOfExtras);
    m_extras.insert(m_extras.end(),  &raw[offset], &raw[readEnd]);
    if (truncation) {
      if ( isHLTResultTruncated() )
        return true;
      else
        return false;
    } 
  }

  return true;
}


unsigned int HLTResult::size() const {
  return m_headerResult.size() +  m_chainsResult.size() + m_navigationResult.size() + m_extras.size();
}

std::vector<unsigned int> HLTResult::partSizes() const {
  std::vector<unsigned int> sizes;
  sizes.reserve(4);
  sizes.push_back(m_headerResult.size());
  sizes.push_back(m_chainsResult.size());
  sizes.push_back(m_navigationResult.size());
  sizes.push_back(m_extras.size());
  return sizes;
}

bool HLTResult::isEmpty() const
{
  return (m_chainsResult.empty() && m_navigationResult.empty());
}


void HLTResult::setConfigSuperMasterKey(uint32_t key) {
  if (m_HLTResultClassVersion >=2 )
    m_headerResult[IndConfigSuperMasterKey] = key;
}


uint32_t HLTResult::getConfigSuperMasterKey() const {
  if ( m_headerResult[IndHLTResultClassVersion] >=2 )
    return m_headerResult[IndConfigSuperMasterKey];
  else
    return 0;
}

void HLTResult::setConfigPrescalesKey(uint32_t key) {
  if (m_HLTResultClassVersion >=2 )
    m_headerResult[IndConfigPrescalesKey] = key;
}

uint32_t HLTResult::getConfigPrescalesKey() const {
  if ( m_headerResult[IndHLTResultClassVersion] >=2 )
    return m_headerResult[IndConfigPrescalesKey];
  else
    return 0;      
}


HLTExtraData& HLTResult::getExtraData()
{
  // On-demand deserialization of m_extras
  if (m_extraData==0) {
    m_extraData = new HLTExtraData(m_extras);
  }  
  return *m_extraData;
}

void HLT::swap(HLTResult& lhs, HLTResult& rhs)
{
  using std::swap;
  
  swap(lhs.m_headerResult, rhs.m_headerResult);
  swap(lhs.m_chainsResult, rhs.m_chainsResult);
  swap(lhs.m_navigationResult, rhs.m_navigationResult);
  swap(lhs.m_navigationResult_DSonly, rhs.m_navigationResult_DSonly);
  swap(lhs.m_extras, rhs.m_extras);
  swap(lhs.m_id_name, rhs.m_id_name);
  swap(lhs.m_id_name_DSonly, rhs.m_id_name_DSonly);
  swap(lhs.m_modID_id_name, rhs.m_modID_id_name);
  swap(lhs.m_navigationResultCuts, rhs.m_navigationResultCuts);
  swap(lhs.m_navigationResultCuts_DSonly, rhs.m_navigationResultCuts_DSonly);
}

