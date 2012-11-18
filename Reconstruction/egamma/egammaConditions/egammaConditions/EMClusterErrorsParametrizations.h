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

#ifndef EMCLUSTERRORSPARAMETRIZATIONS_H
#define EMCLUSTERRORSPARAMETRIZATIONS_H

#include <vector>

#include "egammaConditions/EMClusterEnergyErrorsMatrix.h"
#include "egammaConditions/EMClusterEtaErrorsMatrix.h"
#include "egammaConditions/EMClusterPhiErrorsMatrix.h"
#include "egammaConditions/EMClusterEtaPosErrorsMatrix.h"

class EMClusterErrorsParametrizations 
{
  // friend class EMClusterErrorsTool;
public:
  EMClusterErrorsParametrizations();
  enum Type {ELECTRON = 0, PHOTON, NUM_MATRICES}; // eventually add conversion; NUM_MATRICES must be last

  // for reading
  const EMClusterEnergyErrorsMatrix&  getEnergyMatrix(Type) const;
  const EMClusterEtaErrorsMatrix& getEtaMatrix(Type) const;
  const EMClusterPhiErrorsMatrix& getPhiMatrix(Type) const;
  const EMClusterEtaPosErrorsMatrix& getEtaPosMatrix(Type) const;

  // for writing (only for use by EMClusterErrorsTool)
  EMClusterEnergyErrorsMatrix&  energyMatrix(Type);
  EMClusterEtaErrorsMatrix& etaMatrix(Type);
  EMClusterPhiErrorsMatrix& phiMatrix(Type);
  EMClusterEtaPosErrorsMatrix& etaPosMatrix(Type);
private:
  std::vector<EMClusterEnergyErrorsMatrix> m_energyParametrizations;
  std::vector<EMClusterEtaErrorsMatrix> m_etaParametrizations;
  std::vector<EMClusterPhiErrorsMatrix> m_phiParametrizations;
  std::vector<EMClusterEtaPosErrorsMatrix> m_etaPosParametrizations;
};

inline const EMClusterEnergyErrorsMatrix& EMClusterErrorsParametrizations::getEnergyMatrix(EMClusterErrorsParametrizations::Type t) const
{
  return m_energyParametrizations.at(t);
}
  
inline const EMClusterEtaErrorsMatrix& EMClusterErrorsParametrizations::getEtaMatrix(EMClusterErrorsParametrizations::Type t) const
{
  return m_etaParametrizations.at(t);
}

inline const EMClusterPhiErrorsMatrix& EMClusterErrorsParametrizations::getPhiMatrix(EMClusterErrorsParametrizations::Type t) const
{
  return m_phiParametrizations.at(t);
}

inline const EMClusterEtaPosErrorsMatrix& EMClusterErrorsParametrizations::getEtaPosMatrix(EMClusterErrorsParametrizations::Type t) const
{
  return m_etaPosParametrizations.at(t);
}

#endif
