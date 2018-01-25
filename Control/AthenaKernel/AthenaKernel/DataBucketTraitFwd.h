// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataBucketTraitFwd.h,v 1.2 2008-04-08 16:05:32 ssnyder Exp $

/**
 * @file AthenaKernel/DataBucketTrait.h
 * @author scott snyder
 * @date Mar, 2008
 * @brief Forward declaration of DataBucketTrait template.
 */


#ifndef ATHENAKERNEL_DATABUCKETTRAITFWD_H
#define ATHENAKERNEL_DATABUCKETTRAITFWD_H


namespace SG {


  /**
   * @brief Metafunction to find the proper @c DataBucket class for @c T.
   *
   * See in Storable_conversions.h for full documentation.
   */
  template <class T, class U = T>
  struct DataBucketTrait;


} // namespace SG


#endif // not ATHENAKERNEL_DATABUCKETTRAITFWD_H
