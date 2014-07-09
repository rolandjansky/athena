/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/IndexMap.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Cache pointer -> index mappings for a Getter used for index assocs.
 */


#include "D3PDMakerUtils/IndexMap.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "boost/foreach.hpp"


namespace D3PD {


/**
 * @brief Constructor: for a single target.
 * @param parent The parent tool.
 * @param targetLabel Property for the label of the getter defining
 *                    the target collection.
 * @param registry Property for the ICollectionGetterRegistryTool instance.
 * @param allowMissing Property for the allowMissing flag.
 */
IndexMap::IndexMap (INamedInterface* parent,
                    const std::string& targetLabel,
                    ToolHandle<ICollectionGetterRegistryTool>& registry,
                    const bool& allowMissing)
  : m_parent (parent),
    m_targetLabel (&targetLabel),
    m_targetLabels (0),
    m_registry (&registry),
    m_getters (0),
    m_allowMissing (allowMissing),
    m_valid (false)
{
}


/**
 * @brief Constructor.
 * @brief Constructor: for multiple targets.
 * @param targetLabel Property for the label of the getter defining
 *                    the target collection.  If non-empty, this
 *                    will be considered to be at the start
 *                    of targetLabels.
 * @param targetLabels Property for the label of the getter defining
 *                     the target collection.
 * @param registry Property for the ICollectionGetterRegistryTool instance.
 * @param allowMissing Property for the allowMissing flag.
 */
IndexMap::IndexMap (INamedInterface* parent,
                    const std::string& targetLabel,
                    const std::vector<std::string>& targetLabels,
                    ToolHandle<ICollectionGetterRegistryTool>& registry,
                    const bool& allowMissing)
  : m_parent (parent),
    m_targetLabel (&targetLabel),
    m_targetLabels (&targetLabels),
    m_registry (&registry),
    m_getters (0),
    m_allowMissing (allowMissing),
    m_valid (false)
{
}


/**
 * @brief Constructor: passing the list of getters directly.
 * @param parent The parent tool.
 * @param getters Property for the list of getters.
 * @param allowMissing Property for the allowMissing flag.
 */
IndexMap::IndexMap (INamedInterface* parent,
                    ToolHandleArray<ICollectionGetterTool>& getters,
                    const bool& allowMissing)
  : m_parent (parent),
    m_targetLabel (0),
    m_targetLabels (0),
    m_registry (0),
    m_getters (&getters),
    m_allowMissing (allowMissing),
    m_valid (false)
{
}


/**
 * @brief Common part of configuration.
 */
StatusCode IndexMap::configureCommon ()
{
  ServiceHandle<IIncidentSvc> incSvc ("IncidentSvc", "IndexMap");
  CHECK( incSvc.retrieve() );
  incSvc->addListener (this, "EndEvent");

  if (m_registry) {
    // Look up targets by label.
    CHECK( m_registry->retrieve() );

    if (!m_targetLabel->empty()) {
      ICollectionGetterTool* target = 0;
      CHECK( (*m_registry)->get (*m_targetLabel, m_parent, target) );
      m_targets.push_back (target);
    }
    if (m_targetLabels) {
      for (size_t i = 0; i < m_targetLabels->size(); i++) {
        ICollectionGetterTool* target = 0;
        CHECK( (*m_registry)->get ((*m_targetLabels)[i], m_parent, target) );
        m_targets.push_back (target);
      }
    }
  }
  else {
    // Use direct list of getters.
    CHECK( m_getters->retrieve() );
    for (size_t i = 0; i < m_getters->size(); i++)
      m_targets.push_back (&*(*m_getters)[i]);
  }

  m_converters.resize (m_targets.size());
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * If this is used, the pointer type passed in should match
 * that of the target container(s).
 */
StatusCode IndexMap::configureD3PD ()
{
  CHECK( configureCommon() );

  // Configure the converters to expect the types of the
  // target containers.
  for (size_t i = 0; i < m_targets.size(); i++) {
    if (m_targets[i])
      CHECK( m_converters[i].init (m_targets[i]->elementTypeinfo(),
                                   m_targets[i]->elementTypeinfo()) );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param The pointer type that will be passed into the find methods.
 *
 * If this is used, the pointer type passed in should match
 * that of the target container(s).
 */
StatusCode IndexMap::configureD3PD (const std::type_info& ti)
{
  CHECK( configureCommon() );

  // Configure the converters.
  for (size_t i = 0; i < m_targets.size(); i++) {
    if (m_targets[i])
      CHECK( m_converters[i].init (m_targets[i]->elementTypeinfo(), ti) );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the getter defining the first collection within which to index.
 */
ICollectionGetterTool* IndexMap::target()
{
  for (size_t i = 0; i < m_targets.size(); i++) {
    if (m_targets[i])
      return m_targets[i];
  }
  return 0;
}


/**
 * @brief Return the getter defining the i'th collection within which to index.
 */
ICollectionGetterTool* IndexMap::target (unsigned int i)
{
  if (i >= m_targets.size())
    return 0;
  return m_targets[i];
}


/**
 * @brief Return the number of valid targets.
 */
int IndexMap::ntargets()
{
  int n = 0;
  for (size_t i = 0; i < m_targets.size(); i++) {
    if (m_targets[i])
      ++n;
  }
  return n;
}


/**
 * @brief Incident handler.
 * @param inc The incident.
 */
void IndexMap::handle (const Incident &inc)
{
  if (inc.type() == "EndEvent")
  {
    m_map.clear();
    m_valid = false;
  }
}


/**
 * @brief Call before asking for an index.  Rebuilds cache if needed.
 */
StatusCode IndexMap::reset()
{
  if (!m_valid) {
    for (size_t i = 0; i < m_targets.size(); i++) {
      ICollectionGetterTool* target = m_targets[i];
      if (target) {
        CHECK( target->reset (m_allowMissing) );
        int idx = 0;
        while (const void* obj = target->nextUntyped() ) {
          const void* cobj = m_converters[i].convertUntyped (obj);
          if (cobj)
            m_map.insert (std::make_pair (cobj, std::make_pair(idx++, (int)i)));
          target->releaseObjectUntyped (obj);
        }
      }
    }
    m_valid = true;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Find the index corresponding to an element.
 * @param p The pointer to find.
 *
 * Returns -1 if the pointer isn't found.
 */
int IndexMap::findIndex (const void* p)
{
  return findIndexPair(p).first;
}



/**
 * @brief Find the index corresponding to an element
 *        within a specific container.
 * @param p The pointer to find.
 * @param i The index of the target in which to search.
 *
 * Returns -1 if the pointer isn't found.
 */
int IndexMap::findIndex (const void* p, unsigned int i)
{
  int ret = -1;
  for (map_t::iterator it = m_map.find (p);
       it != m_map.end() && it->first == p;
       ++it)
  {
    if (it->second.second == static_cast<int>(i)) {
      ret = it->second.first;
      break;
    }
  }
  return ret;
}



/**
 * @brief Find the (index,container) pair corresponding to an element.
 * @param p The pointer to find.
 *
 * Returns (-1,-1) if the pointer isn't found.
 */
std::pair<int,int> IndexMap::findIndexPair (const void* p)
{
  map_t::iterator it = m_map.find (p);
  std::pair<int, int> ret (-1, -1);
  if (it == m_map.end())
    return ret;

  // In case of multiple matches, return the first (smallest index)
  // matching container.
  ret = it->second;
  for (++it; it != m_map.end() && it->first == p; ++it) {
    if (it->second.second < ret.second)
      ret = it->second;
  }

  return ret;
}



/**
 * @brief Return the valid flag.
 */
bool IndexMap::valid() const
{
  return m_valid;
}


/** 
 * @brief Return list of all configured targets as a comma-separated string.
 */
std::string IndexMap::formatLabels() const
{
  std::string out = *m_targetLabel;
  if (m_targetLabels) {
    BOOST_FOREACH (const std::string& l, *m_targetLabels) {
      if (!out.empty())
        out += ",";
      out += l;
    }
  }
  return out;
}


/// Dummies needed to satisfy IInterface requirements.
unsigned long IndexMap::addRef() { return 0; }
unsigned long IndexMap::release() { return 0; }
StatusCode IndexMap::queryInterface(const InterfaceID& /*riid*/,
                                    void** /*ppvInterface*/)
{ return StatusCode::FAILURE; }



} // namespace D3PD
