// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IAddVariable.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Common interface for adding a variable to a tuple.
 */

#ifndef D3PDMAKERINTERFACES_IADDVARIABLE_H
#define D3PDMAKERINTERFACES_IADDVARIABLE_H

#include "GaudiKernel/StatusCode.h"
#include <typeinfo>
#include <string>


namespace D3PD {


/**
 * @brief Common interface for adding a variable to a tuple.
 *
 * This defines the interface used to add variables to a tuple.
 * Generally, each filler tool will define this interface.
 * Each of these tools will have a parent that also implements
 * this interface; the tool will then implement this interface by
 * adding an appropriate prefix/suffix to the variable name and
 * passing it to the parent.  This terminates when we reach the
 * @c ID3PD object representing the actual tuple.  This simple case
 * of adding a prefix and forwarding can be implemented using @c AddVariable.
 * Tools may do more complicated transformations as well; for example,
 * @c VectorFillerTool changes a @c T to a @c vector<T>.
 *
 * The interface to add a variable takes the variable name,
 * the variable type, and a reference to a pointer to the variable.
 * Users will generally use the templated version, for which the
 * variable type is specified implicitly via the pointer type.
 * The D3PD software will arrange that when the @c fill() method
 * is called, the supplied pointer will be set to point
 * at a valid instance of the variable.
 *
 * The variable type used must be known to CINT.  Between events,
 * the variable is cleared to the (optional) default value specified
 * in the @c addVariable call.  If it hasn't been specified,
 * the variable is cleared as follows:
 *
 *  - A variable that is not of class type is filled with 0's.
 *  - A variable that Root knows to be a container is cleared
 *    via the Root collection proxy.
 *  - If the variable's type has a clear() method, that is called.
 *  - Otherwise, the object is cleared and recreated.
 *
 * A documentation string may also be specified along with a variable.
 *
 * If any of the @c addVariable() methods are called during the constructor
 * of the tool, then the only effect will be that the pointer to the
 * variable gets cleared.  This allows one to call the @c book() method
 * from the constructor to prevent coverity warnings about uninitialized
 * pointers.
 *
 * NOTE: The dim argument is currently unimplemented.
 */
class IAddVariable
{
public:
  /// Destructor.
  virtual ~IAddVariable() {}


  /**
   * @brief Add a variable to the tuple.
   * @param name The name of the variable.
   * @param ptr Pointer to the type of the variable.
   *            The pointer need not be initialized;
   *            the D3PD software will set the pointer
   *            prior to calling @c fill().
   * @param docstring Documentation string for this variable.
   *
   * If called from the constructor, the only effect is to clear @c ptr.
   */
  template <class T>
  StatusCode addVariable (const std::string& name,
                          T* & ptr,
                          const std::string& docstring = "");


  /**
   * @brief Add a variable to the tuple.
   * @param name The name of the variable.
   * @param ptr Pointer to the type of the variable.
   *            The pointer need not be initialized;
   *            the D3PD software will set the pointer
   *            prior to calling @c fill().
   * @param docstring Documentation string for this variable.
   * @param defval Pointer to the default value to use for this variable.
   *               Only works for basic types.
   *
   * If called from the constructor, the only effect is to clear @c ptr.
   */
  template <class T, class U>
  StatusCode addVariable (const std::string& name,
                          T* & ptr,
                          const std::string& docstring,
                          const U& defval);


  /**
   * @brief Add a variable to the tuple.
   * @param name The name of the variable.
   * @param ptr Pointer to the type of the variable.
   *            The pointer need not be initialized;
   *            the D3PD software will set the pointer
   *            prior to calling @c fill().
   * @param dim Dimension for the variable.
   *            (Presently unimplemented!)
   * @param docstring Documentation string for this variable.
   *
   * If called from the constructor, the only effect is to clear @c ptr.
   */
  template <class T>
  StatusCode addDimensionedVariable (const std::string& name,
                                     T* & ptr,
                                     const std::string& dim,
                                     const std::string& docstring = "");


  /**
   * @brief Add a variable to the tuple.
   * @param name The name of the variable.
   * @param ptr Pointer to the type of the variable.
   *            The pointer need not be initialized;
   *            the D3PD software will set the pointer
   *            prior to calling @c fill().
   * @param dim Dimension for the variable.
   *            (Presently unimplemented!)
   * @param docstring Documentation string for this variable.
   * @param defval Pointer to the default value to use for this variable.
   *               Only works for basic types.
   *
   * If called from the constructor, the only effect is to clear @c ptr.
   */
  template <class T, class U>
  StatusCode addDimensionedVariable (const std::string& name,
                                     T* & ptr,
                                     const std::string& dim,
                                     const std::string& docstring,
                                     const U& defval);


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
   *
   * If called from the constructor, the only effect is to clear @c ptr.
   */
  virtual StatusCode addVariable (const std::string& name,
                                  const std::type_info& ti,
                                  void* & ptr,
                                  const std::string& docstring = "",
                                  const void* defval = 0) = 0;


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
   *
   * If called from the constructor, the only effect is to clear @c ptr.
   */
  virtual StatusCode
  addDimensionedVariable (const std::string& name,
                          const std::type_info& ti,
                          void* & ptr,
                          const std::string& dim,
                          const std::string& docstring = "",
                          const void* defval = 0) = 0;
};


} // namespace D3PD


#include "D3PDMakerInterfaces/IAddVariable.icc"


#endif // not D3PDMAKERINTERFACES_IADDVARIABLE_H
