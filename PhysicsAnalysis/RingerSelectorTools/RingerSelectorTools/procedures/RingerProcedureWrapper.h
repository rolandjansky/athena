/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerProcedureWrapper.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_RINGERPROCEDUREWRAPPER_H
#define RINGERSELECTORTOOLS_PROCEDURES_RINGERPROCEDUREWRAPPER_H

// STL includes:
#include <vector>

#ifndef RINGER_STANDALONE
// xAOD includes:
# include "xAODCaloRings/CaloRings.h"
# include "xAODCaloRings/RingSetConf.h"
# include "xAODTracking/TrackParticleFwd.h"
#endif

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"
#include "RingerSelectorTools/procedures/IRingerProcedure.h"
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/TrackPatternsHolder.h"
#include "RingerSelectorTools/tools/RedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/final.h"
#include "RingerSelectorTools/tools/cxx/override.h"
#include "RingerSelectorTools/tools/cxx/is_same.h"
#include "RingerSelectorTools/tools/cxx/StaticAssert.h"
#include "RingerSelectorTools/tools/cxx/RingerUseNewCppFeatures.h"

#ifndef INCLUDE_HEADER_ONLY // Avoid circular includes
#define INCLUDE_HEADER_ONLY
#endif // INCLUDE_HEADER_ONLY
#include "RingerSelectorTools/procedures/Types.h"
#undef INCLUDE_HEADER_ONLY

/*
 * TODO List
 *
 * - Maybe we want to put PreProcessor Wrapper into the RingerCommonSelector
 *   and add to the Discriminator Wrapper a post-processing wrapper. This would 
 *   make implementation much more flexible.
 *
 * - We also need to deal with the pattern space segmentation being different
 *   from input space, previous topic would help this to be implemented, but we
 *   also would need to add the segmentation representation into the Discrimators.
 *   This means that we might want to change the configuration classes into
 *   segmentation representation, which is what really we need.
 *
 * - The PreProcessings can be propagated into a Processing space, to merge it 
 *   with the PostProcessings. They might be applied as both, that is, they
 *   will inherit from both interfaces, and we might want to apply the direct
 *   transformation, but also the inverse transformation (if it exists), that
 *   would lead the transform space into the original space.
 *
 * - Check why decoration cannot be read within root file using kBranchAccess.
 *
 * - The RingerProcedureWrapper can be divided into base templates to have its
 *   members created only for those cases where it is dependent. For instance,
 *   the collection shouldn't be dependent on seg, eta and et, but only on
 *   procedure_t template. This is also true for some methods, that shouldn't
 *   be created dependent on segment information, or even eta/et dependency.
 */

/* 
 * FIXME List:
 *
 * - Can't apply more than one PreProcessing Wrapper when on the first
 *   discrimination layer.
 *
 */

namespace Ringer {

/**
 * @class IRingerProcedureWrapperBase
 * @brief Used as base for all IRingerProcedureWrapper template specializations
 **/
class IRingerProcedureWrapperBase {
  public:
    /**
     * @brief Returns whether holden interface collection is empty.
     **/
    virtual bool empty() const = 0;

    /**
     * @brief Write collection to TDirectory
     **/
    virtual void write(TDirectory *baseDir, const char *idxStr = "") const = 0;

    /**
     * @brief Retrieve RingerProcedureWrapper name
     **/
    virtual const char* name() const = 0;

    /**
     * @brief Returns eta dependecy for this wrapper
     **/
    virtual EtaDependency etaDep() const = 0;

    /**
     * @brief Returns et dependecy for this wrapper
     **/
    virtual EtDependency etDep() const = 0;

    /**
     * @brief Release all pointed memory holden by wrapper
     **/
    virtual void releaseMemory() = 0;

    /**
     * @brief Get full wrapper name
     **/
    virtual std::string fullName() const = 0;

    /**
     * @brief Print wrapper content
     **/
    virtual void print(MSG::Level lvl = MSG::DEBUG) const = 0;

    /**
     * Ensure virtual destructor
     **/
    virtual ~IRingerProcedureWrapperBase(){;}

};

/**
 * Reduce verbosity 
 **/
typedef IRingerProcedureWrapperBase IProcWrapperBase;

/**
 * Collection of interface base
 **/
typedef std::vector<IProcWrapperBase> IProcWrapperBaseCollection;

/**
 * @class IRingerProcedureWrapper
 * @brief Primary Template for Ringer Interface Wrapper (not implemented)
 *
 * We implement specializations for each Ringer procedure type
 **/
template< typename procedure_t >
class IRingerProcedureWrapper;

/**
 * @class ProcedureHolder
 * @brief Hold procedure dependent procedure vector
 *
 * TODO Implement this
 **/
//template<typename procedure_t>
//class ProcedureHolder;


/**
 * @class RingerProcedureWrapper
 * @brief Primary Template for Ringer Interface Wrapper (not implemented).
 *
 * There is a bool template for determining the procedure specialization type:
 * it must not be explicitely declared.
 *
 * We implement specializations for each procedure type on dedicated files. 
 **/
template < 
  class procedure_t,
  /*EtaDependency*/int etaDependency = EtaIndependent,
  /*EtDependency*/int etDependency = EtIndependent,
  /*SegmentationType*/int segType = NoSegmentation,
  // Below template parameters are not supposed to be set directly by user:
  bool isPreProcessor = RingerProcedureType< procedure_t >::is_pre_processor,
  bool isDiscriminator = RingerProcedureType< procedure_t >::is_discriminator,
  bool isThreshold     = RingerProcedureType< procedure_t >::is_threshold 
>
class RingerProcedureWrapper; 

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_RINGERPROCEDUREWRAPPER_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#ifndef RINGER_PROCEDURE_INCLUDE // When included a specialized procedure type
// in place of RingerProcedureWrapper it is needed to do this because
// the procedure wrapper undefines INCLUDE_HEADER_ONLY macro when including
// Types.h. This can be avoided if we don't force including all other procedure
// types wrappers, but it is wanted to make sure that when a procedure type wrapper
// is included, every other procedure is also included so that the template
// specializations are always available independent on how you include it.
#include "RingerProcedureWrapper.icc"
#endif // RINGER_PROCEDURE_INCLUDE
#endif // INCLUDE_HEADER_ONLY
