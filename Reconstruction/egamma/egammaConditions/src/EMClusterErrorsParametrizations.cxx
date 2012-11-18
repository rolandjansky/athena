/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMClusterErrorsParametrizations

    This is a simeple class/struct that stores all the different types of
    parametrizations for a given event

    @author Jovan.Mitrevski@cern.ch
**/
/////////////////////////////////////////////////////////////////////////////

#include "egammaConditions/EMClusterErrorsParametrizations.h"

EMClusterErrorsParametrizations::EMClusterErrorsParametrizations()
{
  m_energyParametrizations.resize(NUM_MATRICES);
  m_etaParametrizations.resize(NUM_MATRICES);
  m_phiParametrizations.resize(NUM_MATRICES);
  m_etaPosParametrizations.resize(NUM_MATRICES);
}

EMClusterEnergyErrorsMatrix& EMClusterErrorsParametrizations::energyMatrix(Type t)
{
  return m_energyParametrizations[t];
}

EMClusterEtaErrorsMatrix& EMClusterErrorsParametrizations::etaMatrix(Type t)
{
  return m_etaParametrizations[t];
}

EMClusterPhiErrorsMatrix& EMClusterErrorsParametrizations::phiMatrix(Type t)
{
  return m_phiParametrizations[t];
}

EMClusterEtaPosErrorsMatrix& EMClusterErrorsParametrizations::etaPosMatrix(Type t)
{
  return m_etaPosParametrizations[t];
}
