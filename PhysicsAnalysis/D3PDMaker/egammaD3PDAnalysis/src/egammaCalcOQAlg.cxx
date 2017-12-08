/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaCalcOQAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Calculate egamma OQ flags and store as UserData.
 */


#include "egammaCalcOQAlg.h"
#include "clusterHasCells.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "egammaEvent/egamma.h"
#include "AthenaKernel/errorcheck.h"


namespace {


class EgammaTmp
  : public xAOD::Egamma
{
public:
  EgammaTmp() {}
  virtual xAOD::Type::ObjectType type() const { return xAOD::Type::Electron; }
  EgammaTmp& operator= (const xAOD::Egamma& other)
  {
    if (this != &other) {
      xAOD::Egamma::operator= (other);
    }
    return *this;
  }
};


} // anonymous namespace


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
egammaCalcOQAlg::egammaCalcOQAlg (const std::string& name,
                                  ISvcLocator* svcloc)
  : UDAlgBase (name, svcloc),
    m_getter (this),
    m_egammaOQFlagsBuilder( "egammaOQFlagsBuilder" )
{
  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");

  declareProperty ("egammaOQFlagsBuilder",
                   m_egammaOQFlagsBuilder,
                   "Tool for calculating the OQ flag");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaCalcOQAlg::initialize()
{
  CHECK( UDAlgBase::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() );
  CHECK( m_egammaOQFlagsBuilder.retrieve() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaCalcOQAlg::execute()
{
  CHECK (m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>()) {
    unsigned int oq = 0;
    // This only works if the cluster has cells available.
    if (clusterHasCells (eg->caloCluster())) {
      EgammaTmp neweg;
      neweg.makePrivateStore();
      neweg = *eg;
      CHECK( m_egammaOQFlagsBuilder->execute (&neweg) );
      static SG::AuxElement::Accessor<unsigned int> var_oq ("OQ");
      oq = var_oq(neweg);
    }

    static SG::AuxElement::Decorator<unsigned int> var_oq ("OQ");
    var_oq(*eg) = oq;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
