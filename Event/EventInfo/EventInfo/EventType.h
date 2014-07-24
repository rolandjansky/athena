// dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_EVENTTYPE_H
# define EVENTINFO_EVENTTYPE_H 1
/**
 * @file EventType.h
 *
 * @brief This class provides general information about an event. 
 *  It extends EventInfo with a list of sub-evts (the original
 *  and the bkg ones) 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventType.h,v 1.14 2009-05-18 08:31:07 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include <string>

/**
 *   @class EventType
 * 
 *   @brief This class represents the "type of event" where the type is given
 *   by one or more "characteristics".
 * 
 *   Standard characteristics:
 * 
 *     - IS_SIMULATION  // false means IS_DATA
 *     - IS_TESTBEAM    // false means IS_FROM_ATLAS_DET
 *     - IS_CALIBRATION // false means IS_PHYSICS
 *     .
 *     
 *     Since an event may have MORE than one characteristic, a testbeam
 *     simulation event would respond true to first two of the above
 *     characteristics, whereas an offline simulation event would
 *     respond true to ONLY IS_SIMULATION.
 * 
 *     These are set with:
 *
 * @verbatim
         void 		add_type	(EventTypeCode type_code);
   @endverbatim
 *
 *     where the possible EventTypeCode's are provided as constants,
 *     e.g.:
 * 
 * @verbatim
         static const EventTypeCode	IS_SIMULATION;
   @endverbatim
 * 
 *     Thus, one would set IS_SIMULATION by:
 * 
 * @verbatim
         an_event_type.set_type_bit(EventType::IS_SIMULATION);
   @endverbatim
 * 
 * 
 *  User-defined characteristics:
 * 
 *     There is a possible to set and get a "user-defined"
 *     characteristic in terms of a string:
 * 
 * @verbatim
       void 		add_type	(const string& user_type);
       const string&	user_type	(void) const;
   @endverbatim
 *     
 *   Access to the full set of characteristics:
 * 
 *     This is possible via:
 * 
 * @verbatim
         BitMaskIterator 	bit_mask_begin	(void) const;
         BitMaskIterator 	bit_mask_end	(void) const;
   @endverbatim
 * 
 * 
 *  Implementation details:
 * 
 *     The full set of characteristics is provided by static
 *     constants. One may add new event characteristics BOTH by adding
 *     more static constants AND by providing the cooresponding new
 *     boolean methods.
 */
class EventType {
public:

    /// @name typedefs
    //@{
    typedef     unsigned int                        number_type;
    typedef	std::vector<bool>                   BitMask;  
    typedef     BitMask::const_iterator             BitMaskIterator;  
    typedef     BitMask::size_type                  EventTypeCode;  
    typedef     std::pair<std::string,std::string>  NameTagPair;
    typedef     std::vector<NameTagPair>            NameTagPairVec;
    //@}

    /// \name structors
    //@{
    EventType();
    virtual ~EventType();
    //@}

    /// Less than comparision needed to create e.g. set<EventType>
    bool                operator<       (const EventType& rhs) const;

    /// \name Accessors
    //@{
    /// Tests for standard characteristics
    bool 		test		(EventTypeCode type_code) const;

    /// Access to standard characteristics in string for - for print out
    std::string         typeToString    () const;

    /// Access to user type
    const std::string&	user_type	(void) const;

    /// Access DetDescr tags
    void                get_detdescr_tags(NameTagPairVec& pairs) const;

    /// Access DetDescr tags as a single string
    const std::string&  get_detdescr_tags() const;

    /// Access to the MC generator channel number (was used as run
    /// number for generator events)
    number_type         mc_channel_number() const;

    /// Access to the MC generator event number
    uint64_t            mc_event_number() const;

    /// Access to  MC weight
    float               mc_event_weight (unsigned int iweight = 0) const;

    /// Total number of  MC weights
    unsigned int        n_mc_event_weights () const;

    //@}

    /// @name The set of possible characteristic codes:

    /// true: IS_SIMULATION, false: IS_DATA
    static const EventTypeCode	IS_SIMULATION;  

    /// true: IS_TESTBEAM, false: IS_FROM_ATLAS_DET
    static const EventTypeCode	IS_TESTBEAM;    

    /// true: IS_CALIBRATION, false: IS_PHYSICS
    static const EventTypeCode	IS_CALIBRATION; 
    //@}    

    /// \name Iterator access to full set of standard characteristics
    //@{
    BitMaskIterator 	bit_mask_begin	(void) const;
    BitMaskIterator 	bit_mask_end	(void) const;
    //@}    

    /// \name Set characteristics:
    //@{
    /// Add a new event type
    void 		add_type	   (EventTypeCode type_code);

    /// Add user (string) type
    void 		set_user_type	   (const std::string& user_type);

    /// Add in the MC generator channel number (aka gen run number)
    void                set_mc_channel_number(number_type chan);

    /// Add in the MC generator event number
    void                set_mc_event_number(uint64_t evt);

    /// Add in MC weight. For more than 1 weight, add with iweight > 0.
    void                set_mc_event_weight(float weight, 
                                            unsigned int iweight = 0, 
                                            unsigned int nWeightsMax = 0);

    /// Reset DetDescr tags - to remove them from old EventInfo
    /// objects being read in
    void                reset_detdescr_tags();

    //@}

private:
    BitMask 		m_bit_mask; 
    std::string 	m_user_type;
    std::vector<float>  m_mc_event_weights;
    number_type         m_mc_channel_number;
    uint64_t            m_mc_event_number;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFO_EVENTTYPE_H
