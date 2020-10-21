/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCondFEBIdChanMap.h
 *
 * @brief This file defines a class to map FEB IDs and channel ids 
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCondFEBIdChanMap.h,v 1.4 2008-01-31 13:29:21 schaffer Exp $
 */

#ifndef LARRAWCONDITIONS_LARCONDFEBIDCHANMAP_H
#define LARRAWCONDITIONS_LARCONDFEBIDCHANMAP_H
/**
 * @class LArCondFEBIdChanMap
 *
 * @brief This class maps FEB IDs to/from COOL channel ids
 *
 * This class is needed to store LAr conditions data into multiple
 * channels in COOL. Each COOL channel will correspond to a subset of
 * the FEB ids, and this class allows to lookup in either direction.
 *
 * The COOL channel mapping works as follows: there are N groups of
 * FEB IDs which are each mapped into a different channel. Then the
 * overall channel mapping has:
 *
 *    ngain   special channels to store the correction subsets for each gain
 *    N*ngain channels for each FEB ID group, repeated for each of the
 *            possible gains
 *
 * One may identify which channels belong to the same group via the
 * "group number". Here 0 signifies the corrections group and 1 to N
 * for each of the groups of FEB IDs.
 *
 * Note that the default is have all FEB IDs with in a single group
 *
**/

#include <vector>
#include <map>
#include <ostream>
#include "LArIdentifier/LArOnlineID.h"


class LArCondFEBIdChanMap
{
public: 

    ///  Public typedefs
    typedef unsigned int  FEBId; 
    typedef std::vector<FEBId>                FEBIdVector; 
    typedef std::vector<FEBIdVector>          ChannelMap; 
    typedef std::map<FEBId, unsigned int>     FEBIdMap; 

    /// Correction type
    enum CorrectionType {
        Original      = 0,
        PSBA          = 1001,
        PSBC          = 1002,
        PSECA         = 1003,
        PSECC         = 1004,
        EMBA          = 1005,
        EMBC          = 1006,
        EMECA         = 1007,
        EMECC         = 1008,
        HECA          = 1009,
        HECC          = 1010,
        FCALA         = 1011,
        FCALC         = 1012,
        NCORRTYPE     = 12
    };

    /// Default constructor
    LArCondFEBIdChanMap( );

    /// Constructor defining the number of channels per gain, min gain
    /// value, and the number of gain values. nchannels input should NOT
    /// include the special offset channels
    LArCondFEBIdChanMap(unsigned int nChanPerGain, 
			unsigned int minGain, 
			unsigned int nGain);

    /// destructor
    virtual ~LArCondFEBIdChanMap() = default;


    /// Access to a FEB ID vector for a given gain and  COOL channel
    const FEBIdVector&      febIdVector(unsigned int gain, 
					unsigned int coolChannel) const;

    /// Access to the cool channel number for a given FEB ID and gain -
    /// bool == false if not found
    bool                    getChannel (FEBId         febId, 
					unsigned int  gain,
					unsigned int& coolChannel )  const;
    
    /// Access to the cool channel numbers for the correction sets of
    /// a particular gain - bool == false if not found
    bool                    getCorrectionSetChannels(unsigned int  gain,
                                                     std::vector<unsigned int>& coolChannels)  const;

    /// Number of groups - minimum is 2 (1 correction group, 1 FEB ID
    /// group)
    unsigned int            nGroups() const;
    
    /// Group number of a cool channel (0 - correction group, 1-N for
    /// FEB ID groups)
    unsigned int            groupNumber(unsigned int coolChannel) const;


    /// Number of cool channels per gain
    unsigned int            channelsPerGain() const;

    /// Mininum gain value
    unsigned int            minGain() const;
    
    /// Number of gain values
    unsigned int            nGains() const;

    /// Number of offset channels - i.e. correction channels
    unsigned int            nOffsetChannels() const;

    /// Total number of channels for all gains and includes special
    /// offset channels
    unsigned int            totalChannels() const;

    /// Add in a FEBIdVector - channel number should be from 0 to
    /// NChannelsPerGain-1 
    void                    addFEBIdVector(unsigned int channel, const FEBIdVector& febIdVec);
    
    void                    dump(std::ostream& o, const LArOnlineID* onlineID=NULL) const;
    
private:

    /// Offsets for the special channels
    enum ChanMapOffets {
	NUMOFFSETCHANNELSPERGAIN    = 1, // Number of special channels per gain
	CORRECTIONSETCHANNEL        = 0  // Channel for the correction sets
    };


    /// Fill map from chan/febid vec - channel here is 0 to
    /// chansPerGain-1
    void                    fillMap(unsigned int channel, const FEBIdVector& febIdVec);

    /// File map from existing ChannelMap - e.g. on read back
    //void                    fillMap() const; 

    ChannelMap              m_febIdVecs;
    FEBIdMap                m_channelMap;
    unsigned int            m_chansPerGain;
    unsigned int            m_totalChannels;
    unsigned int            m_minGain;
    unsigned int            m_maxGain;
    unsigned int            m_nGains;
    unsigned int            m_numOffsetChannels;
    FEBIdVector             m_empty;
};


// INLINE FUNCTIONS

inline
LArCondFEBIdChanMap::LArCondFEBIdChanMap( )
	:
	m_febIdVecs(1),
	m_chansPerGain(1),
	m_totalChannels(PSBA + 3*NCORRTYPE),
	m_minGain(0),
	m_maxGain(2),
	m_nGains(3),
	m_numOffsetChannels(3)


{}

inline
LArCondFEBIdChanMap::LArCondFEBIdChanMap(unsigned int chansPerGain, 
					 unsigned int minGain, 
					 unsigned int nGains)
	:
	m_febIdVecs(chansPerGain),
	m_chansPerGain(chansPerGain),
	m_totalChannels(PSBA + 3*NCORRTYPE),
	m_minGain(minGain),
	m_maxGain(minGain + nGains),
	m_nGains(nGains),
	m_numOffsetChannels(nGains*NUMOFFSETCHANNELSPERGAIN)

{}
    
inline
void
LArCondFEBIdChanMap::fillMap(unsigned int channel, const FEBIdVector& febIdVec)
{
    // add channel to map for each feb id
    for (unsigned int i = 0; i < febIdVec.size(); ++i) {
	m_channelMap[febIdVec[i]] = channel;
    }
}

#if 0
inline
void                 
LArCondFEBIdChanMap::fillMap() const
{
    // loop over existing vector of feb id vectors and set up map for
    // each feb id. This is needed, for example on readback
    // Though these objects are not read directly, so this should
    // no longer be needed.
    for (unsigned int i = 0; i < m_febIdVecs.size(); ++i) {
	for (unsigned int j = 0; j < m_febIdVecs[i].size(); ++j) {
	    m_channelMap[m_febIdVecs[i][j]] = i;
	}
    }
}
#endif



inline
const LArCondFEBIdChanMap::FEBIdVector&   
LArCondFEBIdChanMap::febIdVector(unsigned int gain, unsigned int coolChannel) const
{

    unsigned int channel = m_chansPerGain;
    if (coolChannel >= (gain - m_minGain)*m_chansPerGain + m_numOffsetChannels) {
	channel =  coolChannel - (gain - m_minGain)*m_chansPerGain - m_numOffsetChannels;
    }
    if (channel < m_chansPerGain) {
	return (m_febIdVecs[channel]);
    }
    return (m_empty);
}

inline
bool
LArCondFEBIdChanMap::getChannel (FEBId febId, 
				 unsigned int gain, 
				 unsigned int& coolChannel )  const
{
    if (1 == m_chansPerGain) {
	// All FEB IDs in the same group
	coolChannel = (gain - m_minGain) + m_numOffsetChannels;
	return (true);
    }
    coolChannel = 9999;
    FEBIdMap::const_iterator it = m_channelMap.find(febId);
    if (it != m_channelMap.end()) {
	if (m_minGain <= gain && gain <= m_maxGain) {
	    coolChannel = (gain - m_minGain)*m_chansPerGain + (*it).second + m_numOffsetChannels;
	    return (true);
	}
    }
    return (false);
}

inline
bool
LArCondFEBIdChanMap::getCorrectionSetChannels(unsigned int  gain,
                                              std::vector<unsigned int>& coolChannels)  const
{
    coolChannels.clear();
    if (m_minGain <= gain && gain <= m_maxGain) {
        coolChannels.reserve(1 + NCORRTYPE);
        // loop over all correction types, including the "old" first
        // one which included everything 
	coolChannels.push_back( (gain - m_minGain) + CORRECTIONSETCHANNEL);
        unsigned int gainOffset = (gain - m_minGain)*NCORRTYPE;
	coolChannels.push_back(PSBA  + gainOffset);
        coolChannels.push_back(PSBC  + gainOffset);
        coolChannels.push_back(PSECA + gainOffset);
        coolChannels.push_back(PSECC + gainOffset);
        coolChannels.push_back(EMBA  + gainOffset);
        coolChannels.push_back(EMBC  + gainOffset);
        coolChannels.push_back(EMECA + gainOffset);
        coolChannels.push_back(EMECC + gainOffset);
        coolChannels.push_back(HECA  + gainOffset);
        coolChannels.push_back(HECC  + gainOffset);
        coolChannels.push_back(FCALA + gainOffset);
        coolChannels.push_back(FCALC + gainOffset);
	return (true);
    }
    return (false);
}

inline
unsigned int
LArCondFEBIdChanMap::nGroups() const
{
    return (m_chansPerGain+NUMOFFSETCHANNELSPERGAIN);
}
    
    
inline
unsigned int  
LArCondFEBIdChanMap::groupNumber(unsigned int coolChannel) const
{
    return (coolChannel/m_nGains);
}


inline
unsigned int 
LArCondFEBIdChanMap::channelsPerGain() const
{
    return (m_chansPerGain);
}

inline
unsigned int  
LArCondFEBIdChanMap::minGain() const
{
    return (m_minGain);
}
    
inline
unsigned int         
LArCondFEBIdChanMap::nGains() const
{
    return (m_nGains);
}

inline
unsigned int  
LArCondFEBIdChanMap::nOffsetChannels() const
{
    return (m_numOffsetChannels);
}


inline
unsigned int         
LArCondFEBIdChanMap::totalChannels() const
{
    return (m_totalChannels);
}

inline
void                 
LArCondFEBIdChanMap::addFEBIdVector(unsigned int channel, const FEBIdVector& febIdVec)
{
    // Make sure vector is large enough - should have been sized in
    // constructor!! 
    if (channel >= m_chansPerGain) m_febIdVecs.resize(channel + 1);
    m_febIdVecs[channel] = febIdVec; 
    fillMap(channel, febIdVec);
}

#endif // LARRAWCONDITIONS_LARCONDFEBIDCHANMAP_H

    
    
