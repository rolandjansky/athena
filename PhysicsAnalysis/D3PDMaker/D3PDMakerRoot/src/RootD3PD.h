// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerRoot/src/RootD3PD.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Root-based D3PD tree.
 */

#ifndef D3PDMAKERROOT_ROOTD3PD_H
#define D3PDMAKERROOT_ROOTD3PD_H


// STL include(s):
#include <vector>
#include <string>
#include <map>


#include "D3PDMakerInterfaces/ID3PD.h"
class TTree;
class TClass;


namespace D3PD {


// Helper for clearing tuple variables.
namespace Root {
class Cleartable;
}
class FakeProxy;


/**
 * @brief Root-based D3PD tree.
 *
 * This provides an @c ID3PD interface for a Root tree.
 * The tree is passed to the constructor.  Ownership of the tree remains
 * with Root.
 *
 * If there are several trees being made, one may be designated as the master;
 * the other trees will be made friends of it.  (For AANT, this is generally
 * CollectionTree.)
 */
class RootD3PD
  : public ID3PD
{
public:
  /**
   * @brief Constructor.
   * @param tree The underlying Root tree.
   * @param master The name of the master tree.  Null if no master.
   * @param allowedNames Names allowed to be put into the output tree
   * @param vetoedNames Names not allowed to end up in the output tree
   * @param basketSize The branch basket size.  -1 to use Root default.
   * @param entryOffsetLen The branch entry offset buffer size.
   *                       -1 to use Root default.
   */
  RootD3PD (TTree* tree,
            const std::string& master,
            const std::vector< std::string >& allowedNames =
            std::vector< std::string >(),
            const std::vector< std::string >& vetoedNames =
            std::vector< std::string >(),
            int basketSize = -1,
            int entryOffsetLen = -1);


  /// Destructor.
  ~RootD3PD();


  /**
   * @brief Add a variable to the tuple.
   * @param name The name of the variable.
   * @param type The type of the variable.
   * @param ptr Pointer to the type of the variable.
   *            The pointer need not be initialized;
   *            the D3PD software will set the pointer
   *            prior to calling @c fill().
   * @param docstring Documentation string for this variable.
   * @param defval Pointer to the default value to use for this variable.
   *               Null for no default (generally means to fill with zeros).
   *               Of the type given by @c ti.
   *               Only works for basic types.
   */
  virtual StatusCode addVariable (const std::string& name,
                                  const std::type_info& ti,
                                  void* & ptr,
                                  const std::string& docstring = "",
                                  const void* defval = 0) override;


  /**
   * @brief Add a variable to the tuple.
   * @param name The name of the variable.
   * @param type The type of the variable.
   * @param ptr Pointer to the type of the variable.
   *            The pointer need not be initialized;
   *            the D3PD software will set the pointer
   *            prior to calling @c fill().
   * @param dim Dimension for the variable.
   *            (Presently unimplemented!)
   * @param docstring Documentation string for this variable.
   * @param defval Pointer to the default value to use for this variable.
   *               Null for no default (generally means to fill with zeros).
   *               Of the type given by @c ti.
   *               Only works for basic types.
   */
  virtual StatusCode
  addDimensionedVariable (const std::string& name,
                          const std::type_info& ti,
                          void* & ptr,
                          const std::string& dim,
                          const std::string& docstring = "",
                          const void* defval = 0) override;


  /**
   * @brief Capture the current state of all variables and write to the tuple.
   */
  virtual StatusCode capture () override;


  /**
   * @brief Clear all the tuple variables.
   */
  virtual StatusCode clear () override;


  /// Currently unimplemented --- see design note.
  virtual StatusCode redim (const Dim_t* ptr) override;


  /**
   * @brief Return the underlying root tree.
   */
  TTree* tree() const;


  /**
   * @brief Return the name of the master tree.
   */
  const std::string& master() const;


  /**
   * @brief Add a new piece of metadata to the tuple.
   * @param key - The key for this object.
   *              Any existing object will be overwritten.
   * @param obj - Pointer to the object to write.
   * @param ti - Type of the object to write.
   *
   * The interpretation of the @c key parameter is up to the concrete
   * D3PD implementation.  However, a key name with a trailing slash
   * NAME/ indicates that all metadata items with this name should
   * be grouped together in a collection called NAME (for example,
   * in a Root directory with that name).
   */
  virtual StatusCode addMetadata (const std::string& key,
                                  const void* obj,
                                  const std::type_info& ti) override;


  /**
   * @brief Set the name of a pool file to which we should attach.
   * @param poolFile The name of the pool file to which we should attach.
   *
   * If we want a D3PD tree to end up in a pool file, we can't in general
   * do it from RootD3PDSvc::make, since the file usually won't have
   * been opened yet.  Instead, call this method to have the RootD3PD
   * object remember the name of the pool file.  On the first capture(),
   * we'll look for the pool file in root's list of files and then
   * associate the tree with it.
   */
  void setPoolFile (const std::string& poolFile);

  
  /**
   * @brief Try to attach to a pool file, if we haven't yet done so.
   */
  StatusCode attachPoolFile();


private:
  /// Try to convert from a std::type_info to a TClass.
  /// On failure, write an error and return null.
  TClass* getClass (const std::type_info& ti);

  /// Decide if a given variable name is allowed
  bool isAllowed (const std::string& name);

  /// Create a variable in memory only
  StatusCode addFakeVariable (const std::string& name,
                              const std::type_info& ti,
                              void*& ptr);

  /// The underlying root tree.
  TTree* m_tree;

  /// The name of the master tree.
  std::string m_master;

  /// Specified basket size.
  int m_basketSize;

  /// Specified entry offset buffer length.
  int m_entryOffsetLen;

  /// Helper to clear variables.
  Root::Cleartable* m_cleartable;

  /// List of names/regular expressions allowed to be put into
  /// the output tree
  std::vector< std::string > m_allowedNames;

  /// List of names/regular expressions not allowed to be put into
  /// the output tree
  std::vector< std::string > m_vetoedNames;

  /// "Fake" variables, only kept in memory
  std::map< std::string, FakeProxy* > m_fakeVars;

  /// If set, the name of a pool data file to which we should attach ourself.
  std::string m_poolFile;

  // Disallow copying.
  RootD3PD (const RootD3PD&) = delete;
  RootD3PD& operator= (const RootD3PD&) = delete;
};


} // namespace D3PD


#endif // not D3PDMAKERROOT_ROOTD3PD_H
