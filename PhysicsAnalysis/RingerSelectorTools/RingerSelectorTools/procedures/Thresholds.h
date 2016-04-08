/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Thresholds.h 694257 2015-09-10 22:45:27Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_THRESHOLDS_H
#define RINGERSELECTORTOOLS_PROCEDURES_THRESHOLDS_H

//#undef NDEBUG

// STL includes:
#include <vector>

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"
#include "RingerSelectorTools/procedures/IRingerProcedure.h"

// Local tools:
#include "RingerSelectorTools/tools/RingerIOVarDepObj.h"
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/RedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/final.h"
#include "RingerSelectorTools/tools/cxx/override.h"

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

  private:
    float m_threshold;
  protected:
    UniqueThreshold(const float threshold):m_threshold(threshold){;}
  public:

    UniqueThreshold():
      m_threshold(0){;}

    /** Execute threshold */
    virtual void execute(const std::vector<float> &input,
        std::vector<bool> &output) const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    /** Ensure virtual dtor */
    virtual ~UniqueThreshold(){;}
    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(UniqueThreshold,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( UniqueThreshold )

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

} // namespace Discrimination
} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_THRESHOLDS_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "Thresholds.icc"
#endif // INCLUDE_HEADER_ONLY

