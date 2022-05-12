/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "HepMCWeightSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListException.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandle.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "boost/algorithm/string.hpp"


/**
 * @brief Standard Gaudi initialize.
 */
StatusCode HepMCWeightSvc::initialize()
{
  ATH_CHECK( m_eventInfoKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Record weight names to metadata if none have yet been set.
 * @param weightNames Map of names to record.
 * @param ctx Current event context.
 *
 * Does nothing if weight names have already been set or read.
 */
StatusCode
HepMCWeightSvc::setWeightNames (const WeightMap& weightNames,
                                const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  std::scoped_lock lock (m_mutex);

  // Ignore any attempt to set 'nothing' for the weight names.
  if (weightNames.size()==0 ||
      (weightNames.size()==1 && weightNames.begin()->first=="0") )
  {
    return StatusCode::SUCCESS;
  }

  if (m_weights[0].m_chanNum != 0 || m_nextWeight != 0) {
    // We only allow setting of weightNames ONCE! ...
    // if the weights have ever been loaded, we will not allow
    // them to be set again!
    // Effectively means the weights are only set in evgen jobs.
    return StatusCode::SUCCESS;
  }

  unsigned int chanNum = ctx.eventID().run_number();

  WeightInfo& w = m_weights[m_nextWeight++];
  w.m_chanNum = chanNum;
  w.m_weightNames = weightNames;
  w.fillVec();

  // Create and register the metadata containter with these weight names 
  // Use the MetaDataTool to do this 
  ATH_CHECK( m_metaDataTool->registerFolder("/Generation/Parameters","Metadata created during Event Generation") );

  // Create a new attributelist collection for it ...
  auto cont = std::make_unique<CondAttrListCollection> (true /* use regular timestamps, not run-lumiblock timestamps */) ;

  // Create a single attribute list.
  CondAttrListCollection::AttributeList  myAttributes;

  // Store as strings ... when read back in we use a gaudi parser to parse the list
  myAttributes.extend("HepMCWeightNames","string");
  myAttributes.extend("HepMCWeightSvcVersion","int");
  myAttributes["HepMCWeightSvcVersion"].data<int>() = 2;
      
  std::string stringToStore = Gaudi::Utils::toString( weightNames );

  myAttributes["HepMCWeightNames"].data<std::string>() = stringToStore;

  // Use the run-number as the 'channel' ... all weightnames should be the same for the same channel 
  bool add_status = cont->add(chanNum, myAttributes);
  if (!add_status) {
    ATH_MSG_INFO("Failed to add AttributeList for weight " << stringToStore);
  }

  ATH_MSG_INFO("Storing /Generation/Parameters :: WeightNames = " << stringToStore);

  // And assign it to 'channel 0' .. consistent with other metadata
  ATH_CHECK( m_metaDataTool->addPayload("/Generation/Parameters", cont.release()) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the current weight names.
 */
HepMCWeightSvc::WeightMap
HepMCWeightSvc::weightNames (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  std::scoped_lock lock (m_mutex);
  size_t i = getWeightIndex (ctx);
  if (i < NWEIGHTS) {
    return m_weights[i].m_weightNames;
  }

  ATH_MSG_WARNING("Unable to load weightnames from metadata ... do not trust the weightnames!");
  return WeightMap();
}


/**
 * @brief Return the current weight names.
 */
std::vector<std::string>
HepMCWeightSvc::weightNameVec (const EventContext& ctx /*= Gaudi::Hive::currentContext()*/)
{
  std::scoped_lock lock (m_mutex);
  size_t i = getWeightIndex (ctx);
  if (i < NWEIGHTS) {
    return m_weights[i].m_weightNameVec;
  }

  ATH_MSG_WARNING("Unable to load weightnames from metadata ... do not trust the weightnames!");
  return std::vector<std::string>();
}


/**
 * @brief Return the `channel number' for the current event.
 * @param ctx Current event context.
 *
 * Either the MC channel number or the run number if that hasn't been set.
 */
unsigned long HepMCWeightSvc::getChanNum (const EventContext& ctx) const
{
  // Try first to get the MC channel number from EventInfo.
  SG::ReadHandle ei (m_eventInfoKey, ctx);
  if (ei.isValid()) {
    unsigned long chanNum = ei->mcChannelNumber();
    if (chanNum > 0) return chanNum;
  }
  // Fall back to run number.
  return ctx.eventID().run_number();
}


/**
 * @brief Try to load weight names from metadata.
 * @param chanNum Channel number for which to load weights.
 *
 * Returns the index in m_weights of the loaded weights.
 */
unsigned int HepMCWeightSvc::loadWeights (unsigned long chanNum)
{
  // Must be called holding the service mutex.

  if (!m_enabled) return NWEIGHTS;

  std::map<std::string, int> in; 

  Athena::ToolLock toolLock (*m_metaDataTool);
  const IOVMetaDataContainer* cont = m_metaDataTool->findMetaDataContainer ("/Generation/Parameters");

  // Return quietly if container isn't there.
  if (cont) {
    if (cont->payloadContainer()->size() == 0) return NWEIGHTS;

    unsigned long chanNumRead = chanNum;
    // If there is only one collection of weights, then we just load that one.
    if (cont->payloadContainer()->at(0)->size()==1) {
      chanNumRead = cont->payloadContainer()->at(0)->chanNum(0);
    }

    const coral::Attribute& attr = cont->payloadContainer()->at(0)->attributeList(chanNumRead)["HepMCWeightNames"];

    int version = 1;
    try {
      version = cont->payloadContainer()->at(0)->attributeList(chanNumRead)["HepMCWeightSvcVersion"].data<int>();
    } catch(const coral::AttributeListException&) {
      version = 1; //no version available so assume version 1
    }

    std::string weightNames = attr.data<std::string>();

    //protect against malformed weightnames - see ATLMCPROD-3103
    //this actually only came about because the Gaudi::Utils::toString method was not used
    //in setWeightNames below. I've now corrected that, but now the fear is that these
    //replacements will interfere with those too!
    //so use a versioning system now
    if(version==1) {
      boost::replace_all(weightNames, "{'", "{\"");
      boost::replace_all(weightNames, "':", "\":");
      boost::replace_all(weightNames, ", '", ", \"");
    }
   
    ATH_MSG_DEBUG("Loading weightnames: " << weightNames);

    if( Gaudi::Parsers::parse(in,weightNames).isFailure() ) return NWEIGHTS;
  }

  size_t iweight = m_nextWeight;
  if (++m_nextWeight >= NWEIGHTS) {
    m_nextWeight = 0;
  }

  WeightInfo& w = m_weights[iweight];
  w.m_chanNum = chanNum;
  w.m_weightNames.clear();

  for(const auto& i : in) {
    w.m_weightNames[i.first] = i.second;
  }
  w.fillVec();

  return iweight;
}


/**
 * @brief Return the index in m_weights for the current event.
 * @param ctx The current event context.
 *
 * Returns NWEIGHTS on failure.
 */
unsigned int HepMCWeightSvc::getWeightIndex (const EventContext& ctx)
{
  unsigned long chanNum = getChanNum (ctx);
  if (chanNum != 0) {
    for (size_t i = 0; i < NWEIGHTS; i++) {
      if (m_weights[i].m_chanNum == chanNum) {
        return i;
      }
    }
    return loadWeights (chanNum);
  }
  return NWEIGHTS;
}


/**
 * @brief Initialize sorted vector from map.
 */
void HepMCWeightSvc::WeightInfo::fillVec()
{
  m_weightNameVec.clear();
  using WPair = std::pair<std::string, unsigned long int>;
  std::vector<WPair> sorted; 
  for(const auto& i : m_weightNames) {
    sorted.emplace_back (i);
  }
  std::sort (sorted.begin(), sorted.end(),
             [](const WPair& a, const WPair& b) {return a.second > b.second; });
  for (auto& a: sorted) {
    m_weightNameVec.emplace_back (std::move (a.first));
  }
}
