/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMbD3PDMaker/src/associators/BCM_RDO_CollectionRawDataAssocTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2010
 * @brief Associate from a BCM_RDO_Collection to its constituent BCM_RawData
 *        objects.
 */


#include "TrigMbD3PDMaker/associators/BCM_RDO_CollectionRawDataAssocTool.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "InDetBCM_RawData/BCM_RawData.h"
#include "AthenaKernel/errorcheck.h"


namespace TrigMbD3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
BCM_RDO_CollectionRawDataAssocTool::BCM_RDO_CollectionRawDataAssocTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

/**
 * @brief Start the iteration for a new association.
 * @param coll The object from which to associate.
 */
StatusCode
BCM_RDO_CollectionRawDataAssocTool::reset (const BCM_RDO_Collection &coll)
{
  m_it = coll.begin();
  m_end = coll.end();
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const BCM_RawData* BCM_RDO_CollectionRawDataAssocTool::next()
{
  while (m_it != m_end) {
    if ((*m_it)->getPulse1Width() > 0) {
      return *m_it++;
    }
    ++m_it;
  }
  return 0;
}


} // namespace TrigMbD3PD
