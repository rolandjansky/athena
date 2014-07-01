// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SelectedParticlesFilterTool.h 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/srcSelectedParticlesFilterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief A collection getter that filters the results of another
 *        based on the contents of a @c SelectedParticles object.
 */


#ifndef SELECTEDPARTICLESFILTERTOOL_H
#define SELECTEDPARTICLESFILTERTOOL_H


#include "D3PDMakerUtils/CollectionGetterFilterToolImpl.h"
#include "GaudiKernel/ToolHandle.h"
class SelectedParticles;


namespace D3PD {


class IObjGetterTool;


/**
 * @brief A collection getter that filters the results of another
 *        based on the contents of a @c SelectedParticles object.
 *
 * This collection getter tool takes another collection getter tool
 * as a parameter and applies a selection given by a @c SelectedParticles
 * object.
 *
 * Properties:
 *   Getter: The nested collection getter tool.
 *   SelectionGetter: Tool to retrieve the @c SelectedParticles object.
 */
class SelectedParticlesFilterTool
  : public CollectionGetterFilterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SelectedParticlesFilterTool (const std::string& type,
                               const std::string& name,
                               const IInterface* parent);


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode initialize();


  /**
   * @brief Return the element type of the collection.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const;


  /**
   * @brief Reset the iteration to the start of the collection.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return failure if the container cannot be retrieved.
   */
  virtual StatusCode reset (bool allowMissing = false);


  /**
   * @brief Return a pointer to the next element in the collection.
   *
   * Return 0 when the collection has been exhausted.
   */
  virtual const void* nextUntyped();


  /**
   * @brief Release an object retrieved from the getter.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c nextUntyped().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElementUntyped (const void* p);


private:
  /// Property: Getter for the @c SelectedParticles object.
  ToolHandle<IObjGetterTool> m_selgetter;

  /// Current @c SelectedParticles object.
  const SelectedParticles* m_selection;

  /// Current iteration index.
  size_t m_index;
};


} // namespace D3PD


#endif // not SELECTEDPARTICLESFILTERTOOL_H
