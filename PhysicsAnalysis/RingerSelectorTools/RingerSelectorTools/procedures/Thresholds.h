/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Thresholds.h 783557 2016-11-10 21:00:43Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_THRESHOLDS_H
#define RINGERSELECTORTOOLS_PROCEDURES_THRESHOLDS_H

//#undef NDEBUG

// STL includes:
#include <vector>
#include <limits>

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"
#include "RingerSelectorTools/procedures/IRingerProcedure.h"

// Local tools:
#include "RingerSelectorTools/tools/RingerIOVarDepObj.h"
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/RedirectMsgStream.h"

/**
 * @brief Namespace dedicated for Ringer utilities
 **/
namespace Ringer {

/**
 * @brief Namespace dedicated for Ringer Discrimination utilities
 **/
namespace Discrimination {

/// Threshold interfaces
/// @{
class UniqueThreshold : virtual public IThreshold,
                          public RedirectMsgStream
{

  RINGER_IO_VARDEP_BASE(UniqueThreshold)

  public:

    UniqueThreshold():
      m_threshold(0){;}

    /** Execute threshold */
    virtual void execute(const std::vector<float> &input,
        std::vector<bool> &output, const DepVarStruct &depVar) const override final;

    float threshold() const { return m_threshold; }
    void setThreshold(float val) { m_threshold = val; }

    /** Ensure virtual dtor */
    virtual ~UniqueThreshold(){;}

  protected:
    UniqueThreshold(const float threshold):m_threshold(threshold){;}

  private:
    float m_threshold;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(UniqueThreshold,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( UniqueThreshold )

/// @}

class LinearPileupCorrectionThreshold : virtual public IThreshold,
                          public RedirectMsgStream
{

  RINGER_IO_VARDEP_BASE(LinearPileupCorrectionThreshold)

  public:

    LinearPileupCorrectionThreshold():
      m_intercept(0.),
      m_slope(0.),
      m_maxPileupValue(std::numeric_limits<float>::infinity()){;}

    /** Execute threshold */
    virtual void execute(const std::vector<float> &input,
        std::vector<bool> &output, const DepVarStruct &depVar) const override final;

    float intercept() const { return m_intercept; }
    float slope() const { return m_slope; }
    float maxPileupValue() const { return m_maxPileupValue; }

    void setIntercept(float val) { m_intercept = val; }
    void setSlope(float val) { m_slope = val; }
    void setMaxPileupValue(float val) { m_maxPileupValue = val; }

    /** Ensure virtual dtor */
    virtual ~LinearPileupCorrectionThreshold(){;}

  protected:
    LinearPileupCorrectionThreshold( const float intercept, const float slope
                                   , const float maxPileupValue = std::numeric_limits<float>::infinity()):
      m_intercept(intercept),
      m_slope(slope),
      m_maxPileupValue(maxPileupValue){;}

  private:
    float m_intercept;
    float m_slope;
    float m_maxPileupValue;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(LinearPileupCorrectionThreshold,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( LinearPileupCorrectionThreshold )

/// @}


/**
 * @brief Unique threshold
 **/
class UniqueThresholdVarDep : virtual public IThresholdVarDep,
                              public RingerIOVarDepObj < UniqueThresholdVarDep >,
                              public UniqueThreshold
{
  RINGER_IO_VARDEP_OBJ(UniqueThresholdVarDep, UniqueThreshold)
  public:

    /** Default ctor (needed by ClassDef) */
    UniqueThresholdVarDep(){;}

    /** Obj ctor */
    UniqueThresholdVarDep(const float threshold):
      UniqueThreshold(threshold){;}

    /** Ensure virtual dtor */
    virtual ~UniqueThresholdVarDep(){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(UniqueThresholdVarDep,1)
};

/**
 * @brief Linear pile-up correction threshold with variable dependency
 **/
class LinearPileupCorrectionThresholdVarDep : virtual public IThresholdVarDep,
                              public RingerIOVarDepObj < LinearPileupCorrectionThresholdVarDep >,
                              public LinearPileupCorrectionThreshold
{
  RINGER_IO_VARDEP_OBJ(LinearPileupCorrectionThresholdVarDep, LinearPileupCorrectionThreshold)
  public:

    /** Default ctor (needed by ClassDef) */
    LinearPileupCorrectionThresholdVarDep(){;}

    /** Obj ctor */
    LinearPileupCorrectionThresholdVarDep( const float intercept, const float slope
                                         , const float maxPileupValue = std::numeric_limits<float>::infinity() ):
      LinearPileupCorrectionThreshold(intercept, slope, maxPileupValue){;}

    /** Ensure virtual dtor */
    virtual ~LinearPileupCorrectionThresholdVarDep(){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(LinearPileupCorrectionThresholdVarDep,1)
};
} // namespace Discrimination
} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_THRESHOLDS_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "Thresholds.icc"
#endif // INCLUDE_HEADER_ONLY

