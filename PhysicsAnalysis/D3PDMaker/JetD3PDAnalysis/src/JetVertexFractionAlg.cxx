/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDAnalysis/src/JetVertexFraction.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2011
 * @brief Store in UserData the JVF vector for a jet.
 */


#include "JetVertexFractionAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "JetMomentTools/JetVertexAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
JetVertexFractionAlg::JetVertexFractionAlg (const std::string& name,
                                            ISvcLocator* svcloc)
  : UDAlgBase (name, svcloc),
    m_getter (this),
    m_jvfTool (this)
{
  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");
  declareProperty ("JetVertexAssociationTool", m_jvfTool ,
                   "Tool to calculate jet vertex fractions,");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode JetVertexFractionAlg::initialize()
{
  CHECK( UDAlgBase::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<Jet>() );
  CHECK( m_jvfTool.retrieve() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode JetVertexFractionAlg::execute()
{
  std::vector<double> jvf;
  std::vector<int> ntrk_vector;
  std::vector<double> trkpt;

  CHECK (m_jvfTool->setupEvent() );
  CHECK( m_getter->reset() );
  while (const Jet* jet = m_getter->next<Jet>())
  {
    CHECK( m_jvfTool->getJVF( jet, jvf, ntrk_vector, trkpt) )  ;
    CHECK( deco (*jet, "fullJVF",    jvf) );
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
