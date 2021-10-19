/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LAr conditions data
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArConditionsSubset_p1.h,v 1.2 2008-01-23 13:52:08 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LARCONDITIONSSUBSET_P1_H
#define LARCONDTPCNV_LARCONDITIONSSUBSET_P1_H 
/**
 * @class LArConditionsSubset_p1
 *
 * @brief persistent class container of LAr conditions data. Holds the
 * generic information a LArConditionsSubset, i.e. not the details for
 * each conditions object
 *
 *
**/

#include <vector>
#include "LArRawConditions/LArConditionsContainerBase.h" //For groupingType enum

/// Size of channel vector, per FEB
// This enum is only used by legacy _pN persistent versions! 
// The recent versions use the subsetSize() method for compatibilty with SuperCells
enum Subset_size {
    NCHANNELPERFEB = 128
};


class LArConditionsSubset_p1
{
public: 

    // LArConditionsSubset_p1 contain the persistent data for the
    // set of LArConditionsSubsets for a given folder.
    //
    // Conditions data are normally organized by feb, identified by
    // the febid. Each feb then has 128 channels of conditions data.
    // In addition, there are corrections stored one per channel with
    // their channel id.
    //
    // In certain cases, not all 128 channels will have conditions
    // data. For normal conditions, space data may arise from channels
    // that could not be treated, or other failures. For MC
    // conditions, this data may be sparse due to the use of implied
    // symmetry of the detector and so only a subset of febs and a
    // subset of their channels are present.
    //
    // For the 'normal' dense set of conditions, it is sufficient to
    // save the set of febids since the number of channels is known.
    //
    // For sparse conditions, we save both the febid and a bit map
    // indicating which channels contain data.
    //
    // For corrections, we save separately the channel ids.
    //
    //

    /// Default constructor
    LArConditionsSubset_p1( ) 
            :
            m_gain(0),
            m_channel(0),
            m_groupingType(0)
        {}

    std::vector<unsigned int>  m_febIds; 
    std::vector<unsigned int>  m_corrChannels; 
    std::vector<unsigned int>  m_febsWithSparseData; 
    unsigned int               m_gain; 
    unsigned int               m_channel;
    unsigned int               m_groupingType;

    unsigned int               subsetSize() const {
      return  m_groupingType == LArConditionsContainerBase::SuperCells ? 320 : 128;}
    
};



#endif // LARCONDTPCNV_LARCONDITIONSSUBSET_P1_H
