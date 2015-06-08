/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_EVENTINFO_H
# define EVENTINFO_EVENTINFO_H 1
/**
 * @file EventInfo.h
 *
 * @brief This class provides general information about an event. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventInfo.h,v 1.15 2009-03-30 17:24:37 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>
#ifndef GAUDIKERNEL_CLASSID_H
# include "GaudiKernel/ClassID.h"
#endif
#include <vector>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class EventID;
class EventType;
class MixingEventSelector;
class TagInfoMgr;
class TriggerInfo;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class EventInfo
 *
 * @brief This class provides general information about an event. 
 *  Event information is provided by the accessors:
 *   EventID*      event_ID()     const;
 *   EventType*    event_type()   const;
 *   TriggerInfo*  trigger_info() const;
 *
 */

class EventInfo {
public:

    /// \name enum for event flags which indicate the subsystem
    /// detector to which the flags belong
    //@{
    enum EventFlagSubDet { Pixel, SCT, TRT, LAr, Tile, Muon, ForwardDet, Core, Background, Lumi, nDets };
    //@}
    /// \name enum for event errors states
    //@{
    enum EventFlagErrorState { NotSet, Warning, Error };
    //@}


    /// \name enum for bits in Background EventFlag word
    //@{


    enum BackgroundEventFlag { MBTSTimeDiffHalo, MBTSTimeDiffCol,
                               LArECTimeDiffHalo, LArECTimeDiffCol,
                               PixMultiplicityHuge, PixSPNonEmpty,
                               SCTMultiplicityHuge, SCTSPNonEmpty,
                               CSCTimeDiffHalo, CSCTimeDiffCol,
                               BCMTimeDiffHalo, BCMTimeDiffCol,
                               MuonTimingCol, MuonTimingCosmic,
                               MBTSBeamVeto,
                               BCMBeamVeto,
                               LUCIDBeamVeto,
                               HaloMuonSegment, HaloClusterShape,
                               HaloMuonOneSided, HaloMuonTwoSided,
                               HaloTileClusterPattern,
                               BeamGasPixel,
                               CosmicStandAlone, CosmicStandAloneTight,
                               CosmicCombined, CosmicCombinedTight,
                               BkgdResvBit1, BkgdResvBit2,
                               BkgdResvBit3, BkgdResvBit4,
                               BkgdResvBit5,
                               NBackgroundWords
    };
    //@}


    /// \name structors
    //@{
    EventInfo();
    EventInfo(EventID* id, EventType* type);
    EventInfo(EventID* id, EventType* type, TriggerInfo* trig_info);
    EventInfo(const EventInfo& e);
    EventInfo& operator=(const EventInfo& e);
#if __cplusplus > 201100
    EventInfo(EventInfo&& e);
    EventInfo& operator=(EventInfo&& e);
#endif
    virtual ~EventInfo();
    //@}

    /// \name DataObject-like clid accessors
    //@{
    static const CLID& classID();
    const CLID& clID() const;
    //@}
  
    /// \name Event information accessors
    //@{

    ///the unique identification of the event.
    EventID*            event_ID        () const;

    ///the type of the event, e.g. simulation, testbeam, etc
    EventType*          event_type      () const;

    /// trigger information (ptr may be NULL)
    TriggerInfo*        trigger_info    () const;

    /// event flags for a particular sub-detector
    unsigned int        eventFlags(EventFlagSubDet subDet) const;

    /// check for a event flag bit for a particular sub-detector
    bool                isEventFlagBitSet(EventFlagSubDet subDet, unsigned char bit) const;

    ///
    /// Access to number of interactions per crossing:
    ///
    ///  average interactions per crossing for the current (or actual) BCID - for in-time pile-up
    float               actualInteractionsPerCrossing() const;

    ///  average interactions per crossing for all BCIDs - for out-of-time pile-up
    float               averageInteractionsPerCrossing() const;



    /// return error state for a particular sub-detector
    EventFlagErrorState errorState(EventFlagSubDet subDet) const;
    //@}

    /// \name Event information setting
    //@{
    /// Add TriggerInfo to existing object    
    void                setTriggerInfo(TriggerInfo*);

    /// Set event flag for a particular sub detector - maximun size is
    /// 28 bits. The bits beyond this will be ignored. Returns true is
    /// successfully inserted (i.e. valid subDet).
    bool                setEventFlags(EventFlagSubDet subDet, unsigned int flags);

    /// Set a bit for an event flag of a particular sub detector. bit
    /// range is 0 - 27. Returns true is successfully inserted
    /// (i.e. valid subDet and bit value). Use 'set = false' to unset
    /// the bit, default it true.
    bool                setEventFlagBit(EventFlagSubDet subDet, unsigned char bit, bool set = true);

    /// Set error state flag for a particular sub detector. Returns
    /// true is successfully inserted (i.e. valid subDet).
    bool                setErrorState(EventFlagSubDet subDet, EventFlagErrorState errorState);


    ///
    /// Setting the number of interactions per crossing:
    ///
    ///  actual interactions per crossing for the current BCID - for in-time pile-up
    void                setActualInteractionsPerCrossing(float interactions);

    ///  average interactions per crossing for all BCIDs - for out-of-time pile-up
    void                setAverageInteractionsPerCrossing(float interactions);
    //@}
  
protected:
    friend class MixingEventSelector;
    void setEventID(EventID* pid);

    friend class TagInfoMgr;
    void setEventType(EventType*);

private:
    enum { EF_BITS                = 0x0FFFFFFF, 
           EF_ERROR_BITS          = 0xF0000000, 
           EF_ERROR_SHIFT         = 28,
           ACTUAL_INTERACTS_BITS  = 0x0000FFFF,
           AVE_INTERACTS_BITS     = 0xFFFF0000,
           AVE_INTERACTS_SHIFT    = 16
    };
    EventID*                    m_event_ID;
    EventType*                  m_event_type;
    TriggerInfo*                m_trigger_info;
    std::vector<unsigned int>   m_event_flags;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline EventID*
EventInfo::event_ID     () const
{
    return (m_event_ID);
}

inline EventType*
EventInfo::event_type   () const
{
    return (m_event_type);
}

inline TriggerInfo*   
EventInfo::trigger_info    () const
{
    return (m_trigger_info);
}

inline unsigned int
EventInfo::eventFlags(EventFlagSubDet subDet) const
{
    if ((unsigned)subDet < m_event_flags.size()) return (EF_BITS & m_event_flags[subDet]);
    return (0xFFFFFFFF);
}

inline bool
EventInfo::isEventFlagBitSet(EventFlagSubDet subDet, unsigned char bit) const
{
    if (subDet < m_event_flags.size() && bit < EF_ERROR_SHIFT) return ((1u << bit) & m_event_flags[subDet]);
    return (false);
}


/// test for error state for a particular sub-detector
inline EventInfo::EventFlagErrorState 
EventInfo::errorState(EventFlagSubDet subDet) const
{
    if ((unsigned)subDet < m_event_flags.size()) {
        return (EventFlagErrorState((EF_ERROR_BITS & m_event_flags[subDet]) >> EF_ERROR_SHIFT));
    }
    return NotSet;
}

    ///  actual interactions per crossing for the current BCID - for in-time pile-up
inline float
EventInfo::actualInteractionsPerCrossing() const 
{
    float result = -999.99f;
    if (Lumi < m_event_flags.size()) {
        unsigned int interactions = m_event_flags[Lumi];
        result = (float)(interactions & ACTUAL_INTERACTS_BITS);
        result *= 0.01;
    }
    return result;
}
    

    ///  average interactions per crossing for all BCIDs - for out-of-time pile-up
inline float
EventInfo::averageInteractionsPerCrossing() const
{
    float result = -999.99f;
    if (Lumi < m_event_flags.size()) {
        unsigned int interactions = m_event_flags[Lumi];
        result = (float)((interactions & AVE_INTERACTS_BITS) >> AVE_INTERACTS_SHIFT);
        result *= 0.01;
    }
    return result;
}



inline const CLID& 
EventInfo::clID() const
{ 
  return classID(); 
}

#ifndef CLIDSVC_CLASSDEF_H
# include "SGTools/CLASS_DEF.h"
#endif
CLASS_DEF( EventInfo , 2101 , 1 )

inline const CLID& 
EventInfo::classID() 
{ 
  return ClassID_traits<EventInfo>::ID(); 
}

#endif // EVENTINFO_EVENTINFO_H


