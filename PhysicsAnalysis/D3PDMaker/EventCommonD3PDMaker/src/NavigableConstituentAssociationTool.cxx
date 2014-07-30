/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/NavigableConstituentAssociationTool.h
 * @author scott snyder, W. H. Bell < W.Bell AT cern.ch > 
 * @date April, 2011
 * @brief Association between navigables and their constituents.
 */


#include "NavigableConstituentAssociationTool.h"
#include "D3PDMakerUtils/TypeNameConversions.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace {


bool checkJet (const Jet& jet)
{
  bool bad = false;
  Jet::const_iterator beg = jet.begin();
  Jet::const_iterator end = jet.end();
  for (; beg != end; ++beg) {
    const INavigable4Momentum* in4m = *beg;
    const Jet* jet2 = dynamic_cast<const Jet*>(in4m);
    if (jet2) {
      if (jet2 == &jet) bad = true;
      Jet::const_iterator beg2 = jet2->begin();
      Jet::const_iterator end2 = jet2->end();
      for (; beg2 != end2; ++beg2) {
        const INavigable4Momentum* in4m2 = *beg2;
        const Jet* jet3 = dynamic_cast<const Jet*>(in4m2);
        if (jet3 == &jet || jet3 == jet2) bad = true;
      }
    }
  }

  return !bad;
}


} // anonymous namespace


namespace D3PD {


/// Constructor.
NavigableConstituentAssociationTool::Selector::Selector
  (TypeConverter& converter)
    : m_converter (converter)
{
}

/// Check data acceptance.
bool NavigableConstituentAssociationTool::Selector::accept
  (const INavigable4Momentum* data,
   double /*weight*/) const
{
  return m_converter.convertUntyped (data) != 0;
}


/// reset condition
void NavigableConstituentAssociationTool::Selector::reset()
{
}


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
NavigableConstituentAssociationTool::NavigableConstituentAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base(type, name, parent),
      m_selector (m_converter),
      m_token (&m_selector)
{
  m_writeWeight = true;
  book().ignore(); // Avoid coverity warnings.

  declareProperty ("TypeName", m_typeName,
                   "Target type name for the association.");
  declareProperty ("WriteWeight", m_writeWeight = false,
                   "Should the association weights be saved?");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode NavigableConstituentAssociationTool::initialize()
{
  CHECK( Base::initialize() );

  // Look up the desired target type and set up the converter.
  const std::type_info* dst_ti = 0;
  CHECK( nameToTypeinfo (m_typeName,
                         dst_ti,
                         "NavigableConstituentAssociationTool") );
  if (!dst_ti) {
    REPORT_MESSAGE (MSG::ERROR)
      << "Can't find type_info for class name " << m_typeName;
    return StatusCode::FAILURE;
  }
  CHECK( m_converter.init (typeid (INavigable4Momentum), *dst_ti) );
  return StatusCode::SUCCESS;
}

  

/**
 * @brief Create any needed tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode NavigableConstituentAssociationTool::book()
{
  if (m_writeWeight)
    CHECK( addVariable ("weight", m_weight, "Constituent weight") );
  return StatusCode::SUCCESS;
}
  
  
/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode
NavigableConstituentAssociationTool::reset(const INavigable4Momentum& p)
{
  m_token.clear();

  // Protect against pathological jets.
  // FIXME: We should be able to do this through the
  // INavigable/NavigationToken interface, rather than having
  // to cast to a Jet!
  if (const Jet* j = dynamic_cast<const Jet*> (&p)) {
    if (!checkJet (*j)) {
      m_it = m_token.begin();
      return StatusCode::SUCCESS;
    }
  }

  // FIXME: would like to stop the recursion once we find the type
  // we're looking for, but NavigationToken doesn't implement that
  // for now.
  p.fillToken (m_token, 1.);
  m_it = m_token.begin();
  return StatusCode::SUCCESS;
}

  
/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const INavigable4Momentum* NavigableConstituentAssociationTool::next()
{
  if (m_it == m_token.end())
    return 0;

  if (m_writeWeight)
    *m_weight = m_it.getParameter();
  return *m_it++;
}


} // namespace D3PD

