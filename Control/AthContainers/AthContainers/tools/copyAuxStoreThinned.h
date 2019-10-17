// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/tools/copyAuxStoreThinned.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Helper to copy an aux store while applying thinning.
 */


#ifndef ATHCONTAINERS_COPYAUXSTORETHINNED_H
#define ATHCONTAINERS_COPYAUXSTORETHINNED_H


class IThinningSvc;


namespace SG {


class IConstAuxStore;
class IAuxStore;
class ThinningDecisionBase;


/**
 * @brief Helper to copy an aux store while applying thinning.
 * @param orig Source aux store from which to copy.
 * @param copy Destination aux store to which to copy.
 * @param svc The thinning service.
 *
 * @c orig and @c copy are both auxiliary store objects.
 * The data from @c orig will be copied to @c copy, with individual
 * elements removed according to thinning recorded for @c orig in @c svc.
 *
 * [To be removed as part of MT thinning work; use the variant below instead.]
 */
void copyAuxStoreThinned (const SG::IConstAuxStore& orig,
                          SG::IAuxStore& copy,
                          IThinningSvc* svc);


/**
 * @brief Helper to copy an aux store while applying thinning.
 * @param orig Source aux store from which to copy.
 * @param copy Destination aux store to which to copy.
 * @param dec The thinning decision for this object.
 *
 * @c orig and @c copy are both auxiliary store objects.
 * The data from @c orig will be copied to @c copy, with individual
 * elements removed according to thinning recorded in @c dec.
 */
void copyAuxStoreThinned (const SG::IConstAuxStore& orig,
                          SG::IAuxStore& copy,
                          const SG::ThinningDecisionBase* dec);


/**
 * @brief For compatibility with old code, that passed `0' as the third arg.
 *        (Can be removed when old thinning is removed.)
 */
void copyAuxStoreThinned (const SG::IConstAuxStore& orig,
                          SG::IAuxStore& copy,
                          int);

                         
} // namespace SG


#endif // not ATHCONTAINERS_COPYAUXSTORETHINNED_H
