// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/IndexMap.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Cache pointer -> index mappings for a Getter used for index assocs.
 *
 * This can handle indexing into multiple containers at once;
 * pass targetLabels to the constructor to specify the list of containers.
 * findIndexPair can then be used to return the index within the container
 * and the container index.
 * (The interface is a little odd for reasons of backwards compatibility
 * with the previous single-target only case.)
 */


#ifndef D3PDMAKERUTILS_INDEXMAP_HPP
#define D3PDMAKERUTILS_INDEXMAP_HPP


#include "D3PDMakerUtils/TypeConverter.h"
#include "CxxUtils/unordered_map.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>


namespace D3PD {


class ICollectionGetterRegistryTool;
class ICollectionGetterTool;


class IndexMap
  : public IIncidentListener
{
public:
  /**
   * @brief Constructor: for a single target.
   * @param parent The parent tool.
   * @param targetLabel Property for the label of the getter defining
   *                    the target collection.
   * @param registry Property for the ICollectionGetterRegistryTool instance.
   * @param allowMissing Property for the allowMissing flag.
   */
  IndexMap (INamedInterface* parent,
            const std::string& targetLabel,
            ToolHandle<ICollectionGetterRegistryTool>& registry,
            const bool& allowMissing);


  /**
   * @brief Constructor: for multiple targets.
   * @param parent The parent tool.
   * @param targetLabel Property for the label of the getter defining
   *                    the target collection.  If non-empty, this
   *                    will be considered to be at the start
   *                    of targetLabels.
   * @param targetLabels Property for the label of the getter defining
   *                     the target collection.
   * @param registry Property for the ICollectionGetterRegistryTool instance.
   * @param allowMissing Property for the allowMissing flag.
   */
  IndexMap (INamedInterface* parent,
            const std::string& targetLabel,
            const std::vector<std::string>& targetLabels,
            ToolHandle<ICollectionGetterRegistryTool>& registry,
            const bool& allowMissing);


  /**
   * @brief Constructor: passing the list of getters directly.
   * @param parent The parent tool.
   * @param getters Property for the list of getters.
   * @param allowMissing Property for the allowMissing flag.
   */
  IndexMap (INamedInterface* parent,
            ToolHandleArray<ICollectionGetterTool>& getters,
            const bool& allowMissing);


  /// Configure during initialization: type-check.
  /// If this is used, the pointer type passed in should match
  /// that of the target container(s).
  StatusCode configureD3PD ();


  /**
   * @brief Configure during initialization: type-check.
   * @param The pointer type that will be passed into the find methods.
   */
  StatusCode configureD3PD (const std::type_info& ti);


  /// Return the getter defining the first collection within which to index.
  ICollectionGetterTool* target();


  /// Return the getter defining the i'th collection within which to index.
  ICollectionGetterTool* target (unsigned int i);


  /// Return the number of valid targets.
  int ntargets();


  /// Incident handler.
  virtual void handle (const Incident &inc);


  /// Call before asking for an index.  Rebuilds cache if needed.
  StatusCode reset();


  /**
   * @brief Find the index corresponding to an element.
   * @param p The pointer to find.
   *
   * Returns -1 if the pointer isn't found.
   */
  int findIndex (const void* p);


  /**
   * @brief Find the index corresponding to an element
   *        within a specific container.
   * @param p The pointer to find.
   * @param i The index of the target in which to search.
   *
   * Returns -1 if the pointer isn't found.
   */
  int findIndex (const void* p, unsigned int i);


  /**
   * @brief Find the (index,container) pair corresponding to an element.
   * @param p The pointer to find.
   *
   * Returns (-1,-1) if the pointer isn't found.
   */
  std::pair<int, int> findIndexPair (const void* p);


  /// Return the valid flag.
  bool valid() const;


  /** 
   * @brief Return list of all configured targets as a comma-separated string.
   */
  std::string formatLabels() const;


  /// Dummies needed to satisfy IInterface requirements.
  virtual unsigned long addRef();
  virtual unsigned long release();
  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvInterface);




private:
  /// Common part of configuration.
  StatusCode configureCommon ();


  /// The parent tool.
  INamedInterface* m_parent;

  /// Property for the label of the getter defining the target collection.
  const std::string* m_targetLabel;

  /// Property for the label of the getter defining the list of
  /// target collections.
  const std::vector<std::string>* m_targetLabels;

  /// Property for the ICollectionGetterRegistryTool instance.
  ToolHandle<ICollectionGetterRegistryTool>* m_registry;

  /// Property for a direct list of getters.
  ToolHandleArray<ICollectionGetterTool>* m_getters;

  /// Property for the allowMissing flag.
  const bool& m_allowMissing;

  /// Getters defining the collections within which to index.
  std::vector<ICollectionGetterTool*> m_targets;

  /// The pointer -> (index,target) map.
  typedef SG::unordered_multimap<const void*, std::pair<int, int> > map_t;
  map_t m_map;

  /// Flag if the map is valid.
  bool m_valid;

  /// Converter for each target.
  std::vector<TypeConverter> m_converters;
};


} // namespace D3PD



#endif // not D3PDMAKERUTILS_INDEXMAP_HPP
