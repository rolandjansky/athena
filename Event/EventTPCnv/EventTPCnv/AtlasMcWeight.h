/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_ATLASMCWEIGHT_H
#define EVENTTPCNV_ATLASMCWEIGHT_H 1
/**
 * @file AtlasMcWeight.h
 *
 * @brief Helper functions for MC weight insider the namespace
 * AtlasMcWeight 
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov>, Borut Kersevan <borut.kersevan@cern.ch>
 *
 * $Id: AtlasMcWeight.h 542280 2013-03-25 12:34:31Z schaffer $
 */

namespace AtlasMcWeight {

    typedef unsigned int number_type;

    /// Convert weight from unsigned to double
    double decode(number_type binnedWeight);

}

#endif
