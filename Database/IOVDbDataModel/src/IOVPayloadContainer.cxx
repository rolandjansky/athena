/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVDbDataModel/IOVPayloadContainer.h"

IOVPayloadContainer::~IOVPayloadContainer()
{
    // delete payload objects
    payloadVec::iterator it   = m_payloadVec.begin();
    payloadVec::iterator last = m_payloadVec.end();
    for (; it != last; ++it) delete (*it);
}

bool             
IOVPayloadContainer::merge(CondAttrListCollection* attrListColl)
{
    // We insert new payload ordered by its minimal IOVRange start
    // time.  Duplicates are removed.
    //
    // Check for an IOV overlap of the incoming attrListColl with the
    // previous one. If there is an overlap, then the previous one is
    // deleted and the new one is kept.
    //
    typedef std::vector<CondAttrListCollection*>::iterator iterator;
    iterator it = std::lower_bound(m_payloadVec.begin(), 
                                   m_payloadVec.end(),
                                   attrListColl->minRange().start(), 
                                   AttrListCollSortStart()) ;

    bool debug = true;
    debug      = false;
    if (debug) {
        
        std::cout << "IOVPayloadContainer::merge - after lowerbound" << std::endl;

        if (m_payloadVec.begin() != m_payloadVec.end()) {
            std::cout << "IOVPayloadContainer::merge - begin    range " 
                      << (*m_payloadVec.begin())->minRange()
                      << std::endl;
        }
        if (it != m_payloadVec.end()) {
            std::cout << "IOVPayloadContainer::merge - it       range " 
                      << (*it)->minRange()
                      << std::endl;
        }
        std::cout << "IOVPayloadContainer::merge - attrList range " 
                  << attrListColl->minRange()
                  << std::endl;
    }
    
        
    if (it != m_payloadVec.begin()) {
        // Check to see if the new AttrListColl matches the one before
        // the insertion point, ignoring the minRange. If they match,
        // then we reject the new one, but use the new stop for the
        // previous AttrListColl.
        iterator itPrev = it;
        --itPrev;

        if (debug) std::cout << "IOVPayloadContainer::merge - not begin " << std::endl;

        if ((*itPrev)->isSameButMinRange(*attrListColl)) {

            // New AttrListColl matches previous one. 
            // Reset IOV to earliest start and latest stop.
            // Reject insertion of the new one.

            // We must save start/stop, reset, and set start/stop
            const IOVTime& startPrev = (*itPrev)->minRange().start();
            const IOVTime& stopPrev  = (*itPrev)->minRange().stop();
            const IOVTime& startNew  = attrListColl->minRange().start();
            const IOVTime& stopNew   = attrListColl->minRange().stop();
            const IOVTime  start = (startPrev < startNew) ? startPrev : startNew;
            const IOVTime  stop  = (stopPrev  > stopNew)  ? stopPrev  : stopNew;
            (*itPrev)->resetMinRange();
            (*itPrev)->addNewStart(start);
            (*itPrev)->addNewStop(stop);

            if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange " << (*itPrev)->minRange() << std::endl;


            // Now delete or truncate all subsequent IOVs which overlap
            for (; it != m_payloadVec.end(); ++it) {

                if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, subsequent start/stop "
                                     << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                     << std::endl;

                if ((*it)->minRange().stop() <= stop) {
                    // remove attribute list collection
                    CondAttrListCollection* oldAttrListColl = (*it);
                    delete oldAttrListColl;
                    it = m_payloadVec.erase(it); 
                    //we deleted the nth element, so that now 'it'
                    //points at the unchecked n+1th element
                    
                    if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, remove payload "
                                         << std::endl;

                    if (it != m_payloadVec.end()) 
                      {
                        --it; //have to do this otherwise the check on this element will be skipped.
                        continue; // go to the next element
                      }
                }
                else if ((*it)->minRange().start() < stop) {
                    // Must truncate old IOV: reset old start to new stop
                    const IOVTime&  oldStop = (*it)->minRange().stop();
                    (*it)->resetMinRange();
                    (*it)->addNewStart(stop);
                    (*it)->addNewStop(oldStop);

                    if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, truncate start/stop "
                                         << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                         << std::endl;

                }
                // we can now leave the loop                
                break; 
            }

            return false;
        }

        if (debug) std::cout <<  "IOVPayloadContainer::merge - is NOT SameButMinRange - check stop " << (*itPrev)->minRange() << std::endl;

        if (attrListColl->minRange().start() < (*itPrev)->minRange().stop()) {
            // Must truncate prev IOV: reset prev stop to new start

            // But we must first check to see if the current IOV lies
            // completely within the previous one
            CondAttrListCollection* prevColl = *itPrev;
            if (attrListColl->minRange().stop() < prevColl->minRange().stop()) {
                CondAttrListCollection* prevColl1 = new CondAttrListCollection(*prevColl);
                prevColl1->addNewStart(attrListColl->minRange().stop());

                if (debug) std::cout <<  "IOVPayloadContainer::merge - attrListColl falls within prev IOV range: "
                                     << prevColl->minRange().start() << " " 
                                     << prevColl->minRange().stop()
                                     << std::endl;

                // Now insert the duplicated attribute list collection
                // (DO NOT increment the iterator for the next
                // insert!)
                it = m_payloadVec.insert(it, prevColl1);

                if (debug) std::cout <<  "IOVPayloadContainer::merge - duplicated previous and insert "
                                     << prevColl1->minRange().start() << " " 
                                     << prevColl1->minRange().stop()
                                     << std::endl;
            }

            const IOVTime&  oldStart = prevColl->minRange().start();
            prevColl->resetMinRange();
            prevColl->addNewStart(oldStart);
            prevColl->addNewStop(attrListColl->minRange().start());

            if (debug) std::cout <<  "IOVPayloadContainer::merge - reset prev stop "
                                 << prevColl->minRange().start() << " " << prevColl->minRange().stop()
                                 << std::endl;
        }
    }

    bool     checkOverlap = false;
    iterator last         = m_payloadVec.end();
    if (it != last) {
        // Return if duplicate
        if ( (**it) == (*attrListColl) ) {

            if (debug) std::cout <<  "IOVPayloadContainer::merge - duplicate - reject new one " << std::endl;

            return false;
        }
        // Now check if there is a partial match - payload is the
        // same, the starts are the same and the new stop is > old
        // stop

        if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange - ignoreIOVs: " << (*it)->isSameButMinRange(*attrListColl, true) << std::endl;

        if ((*it)->isSameButMinRange(*attrListColl, true)) {

            // Payload is the same, ignoring the IOVs

            // Check that starts are the same, and if new stop is
            // later keep the new one. And if the new stop is earlier
            // reject the new one.

            if ((*it)->minRange().start() == attrListColl->minRange().start() && 
                (*it)->minRange().stop()   < attrListColl->minRange().stop()) {

                // New AttrListColl extends the max of the old
                // one. Replace with the new one.

                CondAttrListCollection* old = *it;
                delete old;
                (*it) = attrListColl;
                const IOVTime& stop  = attrListColl->minRange().stop();

                if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange with later stop -> replace " << (*it)->minRange() << std::endl;

                // Now delete or truncate all subsequent IOVs which overlap
                ++it;
                for (; it != m_payloadVec.end(); ++it) {

                    if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, subsequent start/stop "
                                         << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                         << std::endl;

                    if ((*it)->minRange().stop() <= stop) {
                        // remove attribute list collection
                        CondAttrListCollection* oldAttrListColl = (*it);
                        delete oldAttrListColl;
                        it = m_payloadVec.erase(it);
                        //we deleted the nth element, so that now 'it'
                        //points at the unchecked n+1th element
                    

                        if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, remove payload "
                                             << std::endl;

                        if (it != m_payloadVec.end()) {
                          --it; //have to do this otherwise the check on this element will be skipped.
                          continue; // go to the next element
                        }
                    }
                    else if ((*it)->minRange().start() < stop) {
                        // Must truncate old IOV: reset old start to new stop
                        const IOVTime&  oldStop = (*it)->minRange().stop();
                        (*it)->resetMinRange();
                        (*it)->addNewStart(stop);
                        (*it)->addNewStop(oldStop);

                        if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, truncate start/stop "
                                             << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                             << std::endl;

                    }
                    // we can now leave the loop                
                    break; 
                }

                return true;
            }
            else if ((*it)->minRange().start() == attrListColl->minRange().start() && 
                     (*it)->minRange().stop()   > attrListColl->minRange().stop()) {

                if (debug) std::cout <<  "IOVPayloadContainer::merge - isSameButMinRange, new stop is earlier - reject "
                                     << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                     << std::endl;
                return false;
            }
            
        }
        
        checkOverlap = true; // at least one existing AttrListColl
    }

    if (checkOverlap) {
        // AttrListColls differ in content
        //
        // Look for overlap of IOVs 
        //

        // Save start iterator, and new IOV
        iterator itStart  = it;
        IOVTime  newStart = attrListColl->minRange().start();
        IOVTime  newStop  = attrListColl->minRange().stop();

        if (debug) std::cout <<  "IOVPayloadContainer::merge - checkOverlap, new     start/stop "
                             << newStart << " " << newStop
                             << std::endl;

        // If the new stop is at MAX, then the stop is not correctly
        // set. This may happen in online mode where the stop is
        // open-ended.
        // We treat this in one of two ways:
        //   1) if new start and old start are the same => set new stop to old stop
        //   2) if new start < old start => set new stop to old start
        bool isAtMax = false;
        if (newStop.isTimestamp()) isAtMax = newStop.timestamp() == IOVTime::MAXTIMESTAMP;
        else                       isAtMax = newStop.re_time()   == IOVTime::MAXRETIME;
        if (isAtMax) {

            if (debug) std::cout <<  "IOVPayloadContainer::merge - new stop at max " << std::endl;

            if (newStart == (*it)->minRange().start()) {
                // Set new stop to old stop
                attrListColl->resetMinRange();
                attrListColl->addNewStart(newStart);
                newStop = (*it)->minRange().stop();
                attrListColl->addNewStop(newStop);

                if (debug) std::cout <<  "IOVPayloadContainer::merge - set new stop to old stop " << std::endl;

            }
            else { // newStart < oldStart
                // Set new top to old start
                attrListColl->resetMinRange();
                attrListColl->addNewStart(newStart);
                newStop = (*it)->minRange().start();
                attrListColl->addNewStop(newStop);

                if (debug) std::cout <<  "IOVPayloadContainer::merge - set new stop to old start " << std::endl;

            }
        }
        
        // Move back start iterator if we are past begin and the new
        // start is less than the current start
        if (it != m_payloadVec.begin() && newStart < (*it)->minRange().start()) {

            if (debug) std::cout <<  "IOVPayloadContainer::merge - move back "
                                 << std::endl;

            --itStart;
        }
        
        if (debug) std::cout <<  "IOVPayloadContainer::merge - checkOverlap, current start/stop "
                             << (*itStart)->minRange().start() << " " << (*itStart)->minRange().stop()
                             << std::endl;

        // If old start is less than new start, set the old stop to
        // the new start (i.e. truncate the old IOV)
        if ((*itStart)->minRange().start() < newStart && newStart < (*itStart)->minRange().stop() ) {
//        if ((*itStart)->minRange().start() < newStart) {
            const IOVTime&  oldStart = (*itStart)->minRange().start();
            (*itStart)->resetMinRange();
            (*itStart)->addNewStart(oldStart);
            (*itStart)->addNewStop(newStart);

            if (debug) std::cout <<  "IOVPayloadContainer::merge - reset old stop "
                                 << (*itStart)->minRange().start() << " " << (*itStart)->minRange().stop()
                                 << " " << newStart
                                 << std::endl;

        }
        
        // Now insert the new attribute list collection
        it = m_payloadVec.insert(it, attrListColl);

        if (debug) std::cout <<  "IOVPayloadContainer::merge - insert payload "
                             << std::endl;

        // Move the iterator forward
        ++it;

        // Now delete or truncate all subsequent IOVs which overlap
        for (; it != m_payloadVec.end(); ++it) {

            if (debug) std::cout <<  "IOVPayloadContainer::merge - checkOverlap, subsequent start/stop "
                                 << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                 << std::endl;

            if ((*it)->minRange().stop() <= newStop) {
                // remove attribute list collection
                CondAttrListCollection* oldAttrListColl = (*it);
                delete oldAttrListColl;
                it = m_payloadVec.erase(it);

                if (debug) std::cout <<  "IOVPayloadContainer::merge - remove payload "
                                     << std::endl;

                if (it != m_payloadVec.end()) {
                    --it; //have to do this otherwise the check on this element will be skipped.
                    continue; // go to the next element
                }
            }
            else if ((*it)->minRange().start() < newStop) {
                // Must truncate old IOV: reset old start to new stop
                const IOVTime&  oldStop = (*it)->minRange().stop();
                (*it)->resetMinRange();
                (*it)->addNewStart(newStop);
                (*it)->addNewStop(oldStop);

                if (debug) std::cout <<  "IOVPayloadContainer::merge - checkOverlap, truncate start/stop "
                                     << (*it)->minRange().start() << " " << (*it)->minRange().stop()
                                     << std::endl;

            }
            // we can now leave the loop                
            break; 
        }
        
        return true;
    }
    


//    if (debug) std::cout <<  "IOVPayloadContainer::merge - checkStart " << checkStart << std::endl;

//     // If the AttrListColls differ in content, then we must find
//     // correct place to insert new one according to the stop
//     while (checkStart) {
//         if (attrListColl->minRange().start() == (*it)->minRange().start()) {
//             // if new stop is > current stop, move iterator forward
//             if (attrListColl->minRange().stop() > (*it)->minRange().stop()) {

//                 std::cout <<  "IOVPayloadContainer::merge - move it forward " << std::endl;

//                 ++it;
//                 if (it == last) checkStart = false;
//             }
//             // otherwise end loop
//             else checkStart = false;
//         }
//     }

    if (debug) std::cout <<  "IOVPayloadContainer::merge - no overlap, add in attrListColl " << std::endl;

    // no overlaps - add in attribute list
    m_payloadVec.insert(it, attrListColl);

    return (true);
}





