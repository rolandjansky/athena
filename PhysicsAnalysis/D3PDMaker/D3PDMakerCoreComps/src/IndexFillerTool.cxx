/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IndexFillerTool.cxx 491462 2012-03-28 23:35:19Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/IndexFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2010
 * @brief Fill an index of an object within a container.
 */


#include "IndexFillerTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
IndexFillerTool::IndexFillerTool (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : BlockFillerToolImpl (type, name, parent),
    m_index (0),
    m_containerIndex (0),
    m_map (this, m_targetLabel, m_targetLabels, m_registry, m_allowMissing)
{
  declareProperty ("Target",        m_targetLabel,
                   "Label of the collection getter defining the collection "
                   "within which to index.");
  declareProperty ("Targets",       m_targetLabels,
                   "List of labels of collection getters defining the "
                   "collections within which to index. "
                   "If Target is non-empty, it is considered to come "
                   "at the start of this list.");
  declareProperty ("ContainerIndexName", m_containerIndexName,
                   "Variable name to use for the index of the container "
                   "in which the object was found.  Useful if more than "
                   "one target container is specified.  Omitted if this "
                   "is empty.");
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "The ICollectionGetterRegistryTool instance.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, then it is not considered an error for the "
                   "target getter to fail to find the input objects.");
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c fillUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode
IndexFillerTool::configureD3PD (IAddVariable* tree,
                                const std::type_info& ti)
{
  CHECK( m_map.configureD3PD (ti) );
  if (m_map.target()) {
    CHECK( configureImpl (tree, ti, m_map.target()->elementTypeinfo()) );
  }
  else {
    REPORT_MESSAGE (MSG::WARNING)
     << "Not booking index variable because no association targets exist: "
     << m_map.formatLabels();
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode IndexFillerTool::book()
{
  if (m_map.target()) {
    std::string indexdoc = "Index in ";
    std::string contdoc = "Container index: ";
    int n = 0;
    if (!m_targetLabel.empty()) {
      indexdoc += m_targetLabel;
      contdoc += "0 - " + m_targetLabel;
      ++n;
    }
    for (size_t i = 0; i < m_targetLabels.size(); i++) {
      if (n > 0) {
        indexdoc += ", ";
        contdoc += ", ";
      }
      indexdoc += m_targetLabels[i];
      std::ostringstream s;
      s << n << " - " << m_targetLabels[i];
      contdoc += s.str();
      ++n;
    }

    CHECK( addVariable ("index", m_index, indexdoc, -1) );

    if (!m_containerIndexName.empty())
      CHECK( addVariable (m_containerIndexName, m_containerIndex, contdoc, -1));
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block.
 * @param p The input object.
 * @param again Set if this is a subsequent call requested by an AGAIN return
 *
 * This is called once per object.  The type of the object at which @c p
 * points is given by the @c ti argument to @c configureD3PD.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 *
 * If the return status is the special code @c AGAIN (defined above),
 * then this filler tool wants to make multiple entries.
 * The parent should set up to capture a new `row' and run
 * through the list of block filler
 * tools again, but for this tool call @c fillAgainUntyped
 * instead of @c fillUntyped.  This should be repeated as long
 * as @c fillAgainUntyped returns @c AGAIN.
 *
 * Once @c fillUntyped returns @c AGAIN, the parent should
 * call @c fillUntyped with the same @a p argument and @c again
 * set to @c true.  This continues until @c fillUntyped returns something
 * other than @c AGAIN.
 *
 * Not all parents may support this.  In that case, returning
 * @c AGAIN will be treated as an error.
 */
StatusCode
IndexFillerTool::fillUntyped (const void* p,
                              bool /*again = false*/)
{
  if (m_index) {
    // Build the map if it doesn't exist.
    CHECK( m_map.reset() );

    // Search the index container for the pointer.
    std::pair<int, int> out = m_map.findIndexPair (p);
    *m_index = out.first;
    if (m_containerIndex)
      *m_containerIndex = out.second;
  }

  return StatusCode::SUCCESS;
}



} // namespace D3PD


