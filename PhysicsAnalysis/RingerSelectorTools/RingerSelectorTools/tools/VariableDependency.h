/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VariableDependency.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef  RINGERSELECTORTOOLS_TOOLS_VARIABLEDEPENDENCY_H
#define  RINGERSELECTORTOOLS_TOOLS_VARIABLEDEPENDENCY_H

// STL includes:
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iostream>

// ROOT includes:
#include <TDirectory.h>

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"


/**
 * @brief Namespace dedicated for Ringer utilities
 **/
namespace Ringer {

/**
 * @struct et, eta, pileup_estimation
 * @brief Extra patterns decribing particle interation process.
 *
 * Holds dependent variables values.
 **/
class DepVarStruct {
public:
  /// Holds particle transverse energy
  const float et;
  /// Holds eta position
  const float eta;
  /// Holds the currently being used pile-up estimation
  const float pileupEstimation;

  DepVarStruct(const float et, const float eta, const float pileupEstimation = 0.):
    et(et),
    eta(eta),
    pileupEstimation(pileupEstimation){;}
};

//class IVariableDependency
//{
//  public:
//    /**
//     * Check if depVar is within this procedure range
//     **/
//    virtual bool isWithinRange(const DepVarStruct &depVar) const = 0;
//
//    /**
//     * Check if eta is within this procedure eta range
//     **/
//    virtual bool isWithinEtaRange(const float eta) const = 0;
//
//    /**
//     * Set this procedure to be eta dependent:
//     **/
//    virtual void setEtaDep(const float etaMin, const float etaMax) = 0;
//
//    /**
//     * Check if et is within this procedure et range
//     **/
//    virtual bool isWithinEtRange(const float et) const = 0;
//
//    /**
//     * Set this procedure to be et dependent:
//     **/
//    virtual void setEtDep(const float etMin, const float etMax) = 0;
//
//    /**
//     * Set this procedure to be et, eta dependent:
//     **/
//    virtual void setEtaEtDep(const float etaMin, const float etaMax,
//        const float etMin, const float etMax) = 0;
//
//    virtual ~IVariableDependency(){;}
//};

/**
 * @class VariableDependency
 * @brief Interface for possible Eta and Et variable dependency.
 **/
class VariableDependency /*: virtual public IVariableDependency*/
{
  public:

    /**
     * Ctor for independent
     **/
    VariableDependency()
      : m_etaDependency(EtaDependency::EtaIndependent),
        m_etDependency(EtDependency::EtIndependent),
        m_etaMin(0), m_etaMax(0),
        m_etMin(0), m_etMax(0),
        m_type{""}{;}

    /**
     * Ctor for eta dependent case
     **/
    VariableDependency(const float etaMin, const float etaMax,
        EtaDependency etaDependency)
      : m_etaDependency(etaDependency),
        m_etDependency(EtDependency::EtIndependent),
        m_etaMin(etaMin),
        m_etaMax(etaMax),
        m_etMin(0),
        m_etMax(0),
        m_type{""}{;}

    /**
     * Ctor for et dependent case
     **/
    VariableDependency(const float etMin, const float etMax,
        EtDependency etDependency)
      : m_etaDependency(EtaDependency::EtaIndependent),
        m_etDependency(etDependency),
        m_etMin(etMin),
        m_etMax(etMax),
        m_type{""}{;}

    /**
     * Ctor for et/eta dependent case
     **/
    VariableDependency(const float etaMin, const float etaMax,
        const float etMin, const float etMax)
      : m_etaDependency(EtaDependency::EtaDependent),
        m_etDependency(EtDependency::EtDependent),
        m_etaMin(etaMin),
        m_etaMax(etaMax),
        m_etMin(etMin),
        m_etMax(etMax),
        m_type{""}{;}

    /**
     * Check if depVar is within this procedure range
     **/
    bool isWithinRange(const DepVarStruct &depVar) const /*override
      final*/;

    /**
     * Check if eta is within this procedure range
     **/
    bool isWithinEtaRange(const float eta) const /*override
      final*/;

    /**
     * Returns whether it is et dependent
     **/
    EtaDependency etaDep() const /*override final*/;

    /**
     * Get eta min
     **/
    float etaMin() const /*override final*/;

    /**
     * Get eta max
     **/
    float etaMax() const /*override final*/;

    /**
     * Get et min
     **/
    float etMin() const /*override final*/;

    /**
     * Get et max
     **/
    float etMax() const /*override final*/;

    /**
     * Set this procedure to be eta dependent:
     **/
    void setEtaDep(const float etaMin, const float etaMax) /*override
      final*/;

    /**
     * Check if et is within this procedure et range
     **/
    bool isWithinEtRange(const float et) const /*override
      final*/;

    /**
     * Returns whether it is et dependent
     **/
    EtDependency etDep() const /*override final*/;

    /**
     * Set this procedure to be et dependent
     **/
    void setEtDep(const float etMin, const float etMax) /*override
      final*/;

    /**
     * Set this procedure to be et dependent
     **/
    void setEtaEtDep(const float etaMin, const float etaMax,
        const float etMin, const float etMax) /*override
      final*/;

    /**
     * Return this variable dependency string
     *
     * This is not a virtual method.
     **/
    const char* type() const;

    /**
     * Read variable dependency from directory
     **/
    static void read(VariableDependency *varDep,
        TDirectory *configDir,
        unsigned writtenVersion);

    /**
     * Write variable dependency to directory
     *
     * This is not a virtual method.
     **/
    void writeDepInfo(TDirectory *configDir) const;

    /**
     * Ensure virtual destruction (needed if creating dictionary from this
     * class)
     **/
    virtual ~VariableDependency(){;}

  private:

    /// Whether there is eta dependency
    EtaDependency m_etaDependency;
    /// Whether there is et dependency
    EtDependency m_etDependency;

    /// Eta bounded region where RingerProcedure may be applied
    float m_etaMin, m_etaMax;

    /// Et bounded region where RingerProcedure may be applied
    float m_etMin, m_etMax;

    /// The string holder for type:
    mutable std::string m_type;

};

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_TOOLS_VARIABLEDEPENDENCY_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "VariableDependency.icc"
#endif // INCLUDE_HEADER_ONLY

