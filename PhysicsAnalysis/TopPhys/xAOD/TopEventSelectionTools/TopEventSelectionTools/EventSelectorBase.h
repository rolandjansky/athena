/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTORBASE_H_
#define EVENTSELECTORBASE_H_

#include "TopEvent/Event.h"

#include "TopParticleLevel/ParticleLevelEvent.h"

namespace top {

/**
 * @brief This should apply event-level cuts and perform  simple plotting on
 * top::Event objects.
 *
 * This is the base class, and all classes that do stuff
 * to events should inherit from it.
 */
class EventSelectorBase {
public:
    /**
     * @brief Default constructor, doesn't do anything.
     */
    EventSelectorBase() {}

    /**
     * @brief Default destructor, doesn't do anything.
     */
    virtual ~EventSelectorBase() {}

    /**
     * @brief Copy is not allowed.
     */
    EventSelectorBase(const EventSelectorBase&) = delete;

    /**
     * @brief Move is not allowed.
     */
    EventSelectorBase(const EventSelectorBase&&) = delete;

    /**
     * @brief Assignment is not allowed.
     */
    EventSelectorBase& operator=(const EventSelectorBase &) = delete;

    /**
     * @brief This does stuff based on the information in an event.
     *
     * The idea is that you implement this to return either true or false
     * based on the information held within top::Event.  If this returns true
     * then the event is kept.  If it returns false then the event is removed.
     *
     * @param top::Event The current event.
     * @return true if the event should be kept, false otherwise.
     */
    virtual bool apply(const top::Event&) const = 0;

    /*!
     * @brief This does stuff based on the information in a particle level event.
     *
     * The idea is that you implement this to return either true or false, based
     * on the information held within the top::ParticleLevelEvent. If this
     * function returns true, then the event is kept, otherwise it is removed.
     * The function has a default implementation (which returns true) because it
     * is expected that many EventSelector objects do not operate on
     * ParticleLevelEvent objects.
     *
     * @param top::ParticleLevelEvent the current particle level event.
     * @param true if the event should be kept (i.e. it passed the selector
     * criteria), false otherwise.
     */
    virtual bool applyParticleLevel( const top::ParticleLevelEvent & ) const { return true; }

    /**
     * @brief A human readable name.
     *
     * Mostly used for printing the cut and value to the screen.  Must be
     * implemented for each tool.
     */
    virtual std::string name() const = 0;
};

} //namespace top

#endif
