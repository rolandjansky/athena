/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/IN4MHLVAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Test code: IN4M -> HLV
 */


#include "IN4MHLVAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "CLHEP/Vector/LorentzVector.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
IN4MHLVAssociationTool::IN4MHLVAssociationTool (const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
  : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const CLHEP::HepLorentzVector*
IN4MHLVAssociationTool::get (const INavigable4Momentum& p)
{
  m_p = p.hlv();
  return &m_p;
}


} // namespace D3PDTest
