/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsContainerBase.h
 *
 * @brief This file contain an implementation base class for
 * LArConditionsContainer 
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArConditionsContainerBase.h,v 1.8 2008-10-03 17:46:39 schaffer Exp $
 */

#ifndef LARRAWCONDITIONS_LARCONDITIONSCONTAINERBASE_H
#define LARRAWCONDITIONS_LARCONDITIONSCONTAINERBASE_H
/**
 * @class LArConditionsContainerBase
 *
 * @brief This is a non-template implementation base class for
 * LArConditionsContainer 
 *
 * This class contains implementation details in a non-template form
 * allowing it code to be compiled into a library.
 *
**/

#include "LArRawConditions/LArCondFEBIdChanMap.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <string>

// Forward delcarations
class LArOnlineID;
class CaloCell_ID;
class LArCablingService;
class MsgStream;

class LArConditionsContainerBase
{
public: 

    /// Grouping type
    enum GroupingType {
        Unknown      = 0,
	SingleGroup,
	SubDetectorGrouping,
	FeedThroughGrouping,
	ExtendedFTGrouping,
	ExtendedSubDetGrouping
    };

    typedef LArCondFEBIdChanMap::FEBIdVector  FEBIdVector;
    
    /// Default constructor
    LArConditionsContainerBase( );

    /// constructor defining grouping type
    LArConditionsContainerBase(GroupingType type);

    /// destructor
    virtual ~LArConditionsContainerBase();

    /// initialize 
    StatusCode                  initializeBase(); 

    /// allow group type to be set externally - need to (re)initialize
    /// after setting grouping type
    void                        setGroupingType(GroupingType type);

    /// allow group type to be set externally based on a string
    /// returns an FAILURE and a message to logStr if unknown string is supplied
    StatusCode                  setGroupingType(const std::string& groupingStr, MsgStream& logStr);

    /// provide access to online id helper
    const LArOnlineID*          onlineHelper() const;

    /// provide access to offline id helper
    const CaloCell_ID*          offlineHelper() const;

    /// provide access to cabling service id helper
    LArCablingService*          larCablingSvc() const;

    /// Access to a FEB ID vector for a given gain and  COOL channel
    const FEBIdVector&          febIdVector(unsigned int gain, 
					    unsigned int coolChannel) const;

    /// provide access to flag which determines corrections are
    /// applied at initialize, i.e. when conditions are read
    /// in. Default is true. This is a class (static) variable which
    /// applied to ALL conditions in a job!
    /// If one wants to change the value of the flag, pass in arguments:
    //    setFlag = true and flag = <changed value>
    static bool                 applyCorrectionsAtInit(bool setFlag = false, bool flag = true);
    
protected:

    StatusCode     initGrouping(); 

    std::string    groupingTypeToString() const;


    // Get correction index for online id and gain
    void correctionIndexAndCoolChannel (HWIdentifier id, 
                                        unsigned int gain,
                                        unsigned int& index,
                                        unsigned int& coolChannel);

    GroupingType                m_groupType;
    LArCondFEBIdChanMap         m_febIdChanMap;
    const LArOnlineID*          m_onlineHelper;
    const CaloCell_ID*          m_offlineHelper;
    LArCablingService*          m_larCablingSvc;

    // Table to map from Cool channel number to index in
    // CondMultChanCollection - when writing this can be all mixed up 
    std::vector<unsigned int>   m_channelToMultChanCollIndex;
    bool                        m_isInitialized;

};


// INLINE FUNCTIONS


#endif // LARRAWCONDITIONS_LARCONDITIONSCONTAINERBASE_H
