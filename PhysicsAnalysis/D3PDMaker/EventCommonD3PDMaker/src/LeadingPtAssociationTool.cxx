/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/LeadingPtAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief Choose the leading Pt object from a multiple association.
 */


#include "LeadingPtAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LeadingPtAssociationTool::LeadingPtAssociationTool (const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : Base (type, name, parent),
    m_multiAssocTool (this)
{
  declareProperty ("MultiAssocTool", m_multiAssocTool,
                   "The underlying multiple association.  We'll return the "
                   "object from this association with the largest Pt.  Both "
                   "source and target types must be convertible to "
                   "INavigable4Momentum.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LeadingPtAssociationTool::initialize()
{
  CHECK( m_multiAssocTool.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c resetUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * (to @c resetUntyped)
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode LeadingPtAssociationTool::configureD3PD (IAddVariable* tree,
                                                    const std::type_info& ti)
{
  CHECK( Base::configureD3PD (tree, ti) );
  CHECK( m_multiAssocTool->configureD3PD (tree, typeid (INavigable4Momentum)) );
  CHECK( m_cnv.init<INavigable4Momentum> (m_multiAssocTool->elementTypeinfo()) );
  CHECK( m_cnvBack.init (typeid (INavigable4Momentum),
                         m_multiAssocTool->elementTypeinfo()) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const INavigable4Momentum*
LeadingPtAssociationTool::get (const INavigable4Momentum& p)
{
  const INavigable4Momentum* out = 0;
  const void* out_pp = 0;

  if ( m_multiAssocTool->resetUntyped (&p).isFailure() )
    return 0;

  while (const void* pp = m_multiAssocTool->nextUntyped()) {
    const INavigable4Momentum* mom = m_cnv.convert<INavigable4Momentum> (pp);
    if (mom && (!out || mom->pt() > out->pt())) {
      if (out)
        m_multiAssocTool->releaseElementUntyped (out_pp);
      out = mom;
      out_pp = pp;
    }
    else
      m_multiAssocTool->releaseElementUntyped (pp);
  }

  return out;
}


/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c get().  The default implementation is a no-op,
 * but if the association dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void LeadingPtAssociationTool::releaseObject (const INavigable4Momentum* p)
{
  m_multiAssocTool->releaseElementUntyped (m_cnvBack.convertUntyped (p));
}


} // namespace D3PD
