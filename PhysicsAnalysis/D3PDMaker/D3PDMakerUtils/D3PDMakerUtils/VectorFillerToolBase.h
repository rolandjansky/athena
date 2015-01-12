// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/VectorFillerToolBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Common code for wrapping filled variables up into a vector.
 */


#ifndef D3PDMAKERUTILS_VECTORFILLERTOOLBASE_H
#define D3PDMAKERUTILS_VECTORFILLERTOOLBASE_H


#include "ObjectMetadata.h"
#include "D3PDMakerInterfaces/IAddVariable.h"
#include "D3PDMakerInterfaces/IBlockFillerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <list>
#include <string>
#include <typeinfo>
class TVirtualCollectionProxy;


namespace D3PD {


class IIteration;


/**
 * @brief Common code for wrapping filled variables up into a vector.
 *
 * In several places we want to be able to run a set of block filler
 * tools over a set of objects and wrap the results up in vectors.
 * This is used at least in @c VectorFillerTool and
 * @c ContainedVectorMultiAssociationFillerTool.  This class factors
 * out the common behavior involved in wrapping results up in vectors.
 *
 * The actual iteration over a set of objects is expressed by the
 * interface @c IIteration.  The way this class is used is as follows:
 *
 *   - Call @c doConfigure from your @c configureD3PD method.
 *     Pass in the @c IIteration instance.
 *
 *   - Call @c doBook from your @c book method.
 *
 *   - To do a fill, first do whatever's needed to prepare the
 *     @c IIteration instance to iterate over the desired object set.
 *     Then pass it to @c doFill.
 *
 *     @c doFill will iterate over the objects in the @c IIteration;
 *     for each of these, it will loop over the block filler tools.
 *
 *     The results will be collected inside of vectors.
 *
 * This tool has the following properties:
 *
 *   BlockFillers - List of IBlockFillerTool instances.
 *   Prefix       - Prefix to add to variable names for this block.
 *   NrowName     - Name of the variable for the count of rows.
 *                  Omitted if empty.
 *                  Default: `n'.
 *   NobjName     - Name of the variable for the count of objects.
 *                  (May be less than the number of rows if AGAIN is used.)
 *                  Omitted if empty (default).
 *   ObjIndexName - Name of the variable for the object index.
 *                  Omitted if empty (default).
 */
class VectorFillerToolBase
  : public AthAlgTool,
    public IAddVariable
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VectorFillerToolBase (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize();


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
                                  const void* defval = 0);


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
                          const void* defval = 0);


protected:
  /**
   * @brief Configure the tool.
   * @param tree The parent D3PD tree.
   * @param it The iteration object we'll use.  (Used to get the elt type.)
   *
   * This is called during initialization.
   */
  StatusCode doConfigure (IAddVariable* tree, IIteration& it);


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  StatusCode doBook();


  /**
   * @brief Fill objects from an @c IIteration.
   * @param it Iteration object.  Must have already been reset.
   * @param sizeHint Hint for the expected size of the iteration.
   *
   * This will loop over the objects in the iteration; for each object,
   * it will loop over the contained block filler tools.
   */ 
  StatusCode doFill (IIteration& it, size_t sizeHint);


  /// Metadata about the variables created by this tool
  ObjectMetadata m_metadata;


  /// Property: Variable prefix for this block.
  std::string m_prefix;


private:
  /// Property: The list of block filler tools.
  ToolHandleArray<IBlockFillerTool> m_blockFillers;

  /// Property: Name of the variable for the count of rows.
  /// Omitted if empty.
  std::string m_nrowName;

  /// Property: Name of the variable for the count of objects.
  /// Omitted if empty.
  std::string m_nobjName;

  /// Property: Name of the variable for the object index.
  /// Omitted if empty.
  std::string m_objIndexName;

  /// The parent tree.
  IAddVariable* m_tree;

  /// Variable for the row count.
  int* m_nrow;

  /// Variable for the object count.
  int* m_nobj;

  /// Variable for the object index.
  int* m_objIndex;


  /**
   * @brief Description for a single variable.
   *
   * We need to be able to resize the vector and quickly adjust the
   * pointer to the next entry.  So we save the root collection proxy
   * and the element-to-element to offset.  So to move to a new element,
   * all we need to do is to add the offset to the variable pointer.
   *
   * The vector must have a root dictionary (but not necessarily
   * a reflex dictionary).
   */
  class Var
  {
  public:
    /**
     * @brief Constructor.
     * @param name The variable name (without prefix).
     * @param ti The element type of the variable.
     * @param ptr The variable pointer provided by the block filler tool.
     * @param dim Reserved.
     * @param defval Pointer to the default value to use for this variable.
     *               Null for no default.
     *               Of the type given by @c ti.
     *               Only works for basic types.
     *               We take ownership of this.
     * @param defsize Size of the object pointed at by defval.
     */
    Var (const std::string& name,
         const std::type_info& ti,
         void* & ptr,
         const std::string& dim,
         char* defval,
         size_t defsize);


    /**
     * @brief Initialize the variable.
     * @param tree The parent tree.
     * @param prefix The variable name prefix.
     */
    StatusCode init (IAddVariable* tree,
                     const std::string& prefix);


    /**
     * @brief Resize the vector.
     * @param sz The new vector size.
     * @param pos After the resize, move the variable pointer to this element.
     */
    void resize (size_t sz, size_t pos);


    /**
     * @brief Move the variable pointer to the next element.
     */
    void next();


    /**
     * @brief Free allocated storage.
     *
     * Don't do this from the destructor since these guys get used in a vector.
     */
    void free();


  private:
    /// The variable name.
    std::string m_name;

    /// The type of the variable element (for each object).
    const std::type_info* m_ti;

    /// Pointer to the variable pointer.
    char** m_addr;

    /// Documentation string.
    std::string m_docstring;

    /// The root collection proxy.
    TVirtualCollectionProxy* m_proxy;

    /// Offset between collection elements.
    size_t m_valdiff;

    /// Pointer to the vector instance.
    void* m_contptr;

    /// Pointer to the default value for this variable,
    /// if one has been requested.  We own this object.
    char* m_default;

    /// Size of the object pointed to by m_default.
    /// (Should generally be the same as m_valdiff, but it may possibly
    /// be smaller due to alignment considerations.)
    size_t m_defsize;
  };


  /**
   * @brief Description for the list of variables.
   *
   * This is a list of @c Var objects, with a few convenient
   * extensions for performing operations on all variables.
   *
   * Note: has to be a list rather than a vector, since we want to be
   * able to extend it without changing the addresses of existing elements.
   */
  struct Vars
    : public std::list<Var>
  {
    /// Destructor.
    ~Vars();


    /**
     * @brief Resize all vector.
     * @param sz The new vector size.
     * @param pos After the resize, move the variable pointers to this element.
     */
    void resize_all (size_t sz, size_t pos = 0);


    /**
     * @brief Move all variable pointers to the next element.
     */
    void next_all ();
  };

  /// All booked variables.
  Vars m_vars;


  /**
   * @brief Extend vectors if needed.
   * @param n Number of entries currently made in the vectors.
   * @param sz Current size of the vectors.
   */
  void maybeExtend (size_t& n, size_t& sz);


  /**
   * @brief Make one loop over block filler tools.
   * @param obj Object being processed.
   * @param objndx The index of the object we're currently processing.
   * @param n Number of entries currently made in the vectors.
   * @param sz Current size of the vectors.
   * @param codes_in Vector of status returns for each tool
   *                 from the first @c fill call.
   * @param codes_out[out] Vector of status returns for each tool
   *                       from this @c fill call.
   *
   * This function may be called more than once for an object
   * if @c AGAIN was requested.
   *
   * We loop over all block filler tools.  For each, we call
   * @c fillUntyped with the @c again parameter set depending
   * on whether @c codes_in for this tool is @c AGAIN.
   * We collect the return codes from each tool in @c codes_out.
   */
  StatusCode fillOnce (const void* obj,
                       size_t objndx,
                       size_t& n,
                       size_t& sz,
                       const std::vector<unsigned long>& codes_in,
                       std::vector<unsigned long>& codes_out);
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_VECTORFILLERTOOLBASE_H
