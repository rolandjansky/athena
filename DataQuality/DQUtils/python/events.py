# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from .sugar import IOVSet, RunLumi, RunLumiType
from .utils import worst

from heapq import heapreplace, heappush, heappop
from collections import defaultdict

import logging
log = logging.getLogger("DQUtils.events")

def build_states_emptyiovs(*iovsets):
    """
    Build a states and empty_iovs list.
    """
    empty_iovs, states = [], []
    
    for index, iovset in enumerate(iovsets):
        empty_iov = None
        if isinstance(iovset, IOVSet) and iovset.iov_type:
            empty_iov = iovset.iov_type.empty()
            if iovset and "channel" in empty_iov._fields:
                empty_iov = empty_iov._replace(channel=iovset.first.channel)
                
        empty_iovs.append(empty_iov)
        states.append(empty_iov)
    
    return empty_iovs, states
    
def process_iovs(*iovsets):
    """
    Given a list of input iovsets containing each one channel, yield the state 
    of each iovset in the largest (since, until) range possible. The returned
    states is the same length as the number of arguments to this function.
    
    Example usage:
    
    iovset = fetch_iovs("SHIFTOFL", runs=152166)
    channels, iovsets = zip(*sorted(iovset.by_channel))
    # `iovsets` here is a list of lists, each containing the iovs for one channel
    for since, until, states in process_iovs(*iovsets):
        print "From", since, "to", until
        for state in states:
            print "", state.channel, "is :", state.Code
    
    or:
    
    dcsofl_sct   = fetch_iovs("DCSOFL",   runs=152166, channels="SCTEA")
    shiftofl_sct = fetch_iovs("SHIFTOFL", runs=152166, channels="SCTEA")
    for since, until, (dcsofl, shiftofl) in process_iovs(dcsofl, shiftofl):
        print ("From", since, "to", until,
               "dcsofl=", dcsofl.Code,
               "shiftofl=", shiftofl.Code)
    
    Pitfall: IOVs can exist on the database where there are no state changes.
             process_iovs will emit
    """
    
    empty_iovs, states = build_states_emptyiovs(*iovsets)
    last_position = None
    
    for position, index, beginning, iov in iov_yielder(*iovsets):
        if last_position != position:
            if last_position is not None:
                assert last_position < position, "Inputs inconsistent. since !< until."
                yield last_position, position, states
            last_position = position
            
        if beginning:
            states[index] = iov
        else:
            states[index] = empty_iovs[index]

def process_iovs_changed(*iovsets):
    """
    Like process_iovs but as well as a states list, also yields the set of 
    indices which changed since the last yield.
    """
    empty_iovs, states = build_states_emptyiovs(*iovsets)
    last_position = None
    changed_set = set()
    
    for position, index, beginning, iov in iov_yielder(*iovsets):
        if last_position != position:
            if last_position is not None:
                assert last_position < position, "Inputs inconsistent. since !< until."
                yield last_position, position, states, changed_set
                changed_set.clear()
                
            last_position = position
        
        changed_set.add(index)
        if beginning:
            states[index] = iov
        else:
            states[index] = empty_iovs[index]
            
def process_iovs_mc(*iovsets):
    """
    process iovs for multiple channels
    """
    
    # Find the complete range
    range_iov = IOVSet(iovs.range_iov for iovs in iovsets if iovs).range_iov
    range_iov = IOVSet([range_iov])
    
    iovsets = [iovs.by_channel for iovs in iovsets]
    channels = set(iov for iovs in iovsets for iov in iovs.keys())
    
    for channel in sorted(channels):
        chan_iovsets = [iovs[channel] for iovs in iovsets]
        for since, until, states in process_iovs(range_iov, *chan_iovsets):
            yield since, until, channel, states[1:]
          
def iov_yielder(*iovs):
    """
    Yields sets of iovs in "position" order. Each IoV gets yielded for its start
    and for its end. (beginning = True and False respectively)
    """
    eventqueue = []
    for index, iterable in enumerate(iovs):
        it = iter(iterable)
        try:
            iov = next(it)
        except StopIteration:
            pass
        else:
            heappush(eventqueue, (iov.since, True, iov, index, it))
    
    while eventqueue:
        position, beginning, iov, index, it = eventqueue[0]
        
        yield position, index, beginning, iov
        
        if beginning:            
            next_key = iov.until, False
            
        else:            
            try:
                iov = next(it)
            except StopIteration:
                heappop(eventqueue)
                continue
                
            next_key = iov.since, True
                
        heapreplace(eventqueue, next_key + (iov, index, it))

def quantize_lb(since, until, states):
    log.info("Evaluating: %s %s %s", since, until, states)
    if len(states) == 1:
        return states.pop()._replace(since=since, until=until)
    
    result = worst(iov.Code for iov in states)
    return states.pop()._replace(since=since, until=until, Code=result)

def quantize_iovs_intermediate(lbtime, iovs):
    
    # "n" => 'new' this past lumi block with respect to the end event
    #ncurrent_events, nended_events = defaultdict(set), defaultdict(set)
    current_events, ended_events = set(), set()
    iov_type = None
    change_in_this_lb = False
    
    for position, index, beginning, iov in iov_yielder(lbtime, iovs):
        
        if index == 0:
            # LBTIME events
            lb_n = RunLumi(iov.Run, iov.LumiBlock)
            log.info("LB: %s %s %s %s", ">" if beginning else "]", lb_n, iov.since, iov.until)
            
            if current_events:
                if beginning:
                    current_events.difference_update(ended_events)
                    ended_events.clear()
                    
                else:
                    #quantized_state = worst(iov.Code for iov in current_events)
                        
                    yield lb_n, change_in_this_lb, current_events 
                    # yield #iov_type(lb_n, lb_n+1, channel, quantized_state)
                        
        else:
            if iov_type is None:
                iov_type = type(iov)
            change_in_this_lb = True
            log.info("Event: %s", iov)
            (current_events if beginning else ended_events).add(iov)
            
def default_quantizing_function(current_events):
    return worst(iov.Code for iov in current_events)

def quantize_iovs(lbtime, iovs, quantizer=default_quantizing_function):
    from .iov_arrangement import connect_adjacent_iovs
    iovs = connect_adjacent_iovs(iovs)
    
    current_events, ended_events = set(), set()
    need_evaluation = channel = iov_type = None
    last_lb_n = lb_n = RunLumi(0)
    # ps => 'previous state'
    previous_state = ps_since = ps_until = None
    
    for position, index, beginning, iov in iov_yielder(lbtime, iovs):
        
        if index == 0:
            # Luminosity block started or finished
            
            lb_n = RunLumi(iov.Run, iov.LumiBlock)
            #print "LB:", ">" if beginning else "]", lb_n, iov.since, iov.until,
            
            # We passed a run boundary - need emit.
            emit_needed = lb_n.run != last_lb_n.run
            if emit_needed:
                need_evaluation = lb_n + 2
                
            last_lb_n = lb_n
            
            # Defines whether something happened which requires 
            do_evaluation = ended_events or need_evaluation >= lb_n
            
            
            #if do_evaluation:
            #    print "#",
            #else:
            #    print need_evaluation >= lb_n, need_evaluation, lb_n,
            
            if do_evaluation and beginning:
                # LB start: Remove events which finished in the last LB.
                if ended_events:
                    need_evaluation = lb_n + 2
                    current_events -= ended_events
                    ended_events.clear()
                                
            elif do_evaluation:
                new_state = quantizer(current_events)
                #print len(current_events),
                
                if previous_state != new_state or emit_needed:
                    #print "State change!",
                    # State changed. Emit the previous IoV if there was one,
                    # record when this state started.
                    
                    #print "State: ", lb_n, previous_state, "=>", new_state,
                    if previous_state is not None:
                        # This LB changed
                        iov = iov_type(ps_since, ps_until, 
                                       channel, previous_state)
                        #print
                        #print "== Emitted", iov,
                        yield iov
                        
                    ps_since, previous_state = lb_n, new_state
            
            if not beginning:
                # The current state should last at least as long as until
                # the end of the current lumiblock
                ps_until = lb_n + 1
            
            #print 
            
        else:
            #print "---", ">" if beginning else "]", iov
            if iov_type is None:
                # First IoV we have come across.
                iov_type, channel = type(iov), iov.channel

            # Something happened. We need to evaluate this and next lumiblock
            need_evaluation = lb_n + 2
                
            # Record this iov in the current or ended events set
            (current_events if beginning else ended_events).add(iov)

    if previous_state is not None:
        # There exists a state which hasn't been emitted yet. Do so.
        iov = iov_type(ps_since, ps_until, channel, previous_state)
        #print "Finished:", iov
        yield iov

def quantize_iovs_slow_interm(lbtime, iovs, 
                              quantizer=default_quantizing_function):
    from .iov_arrangement import connect_adjacent_iovs
    iovs = connect_adjacent_iovs(iovs)
    
    current_events, ended_events = set(), set()
    evaluate_until = state = channel = iov_type = None
    lb_n = last_lb_n = RunLumi(0)
    
    for position, index, beginning, iov in iov_yielder(lbtime, iovs):
        
        if index == 0:
            # Luminosity block started or finished
            
            if beginning:
                # LB start: Remove events which finished since the last LB.
                current_events -= ended_events
                ended_events.clear() 
                
            else:
                lb_n = RunLumi(iov.Run, iov.LumiBlock)
                if lb_n.run != last_lb_n.run:
                    evaluate_until = lb_n + 2
                last_lb_n = lb_n
                    
                if evaluate_until >= lb_n:
                    state = quantizer(current_events)
                
                if state is not None:
                    iov = iov_type(lb_n, lb_n+1, channel, state)
                    yield iov
        else:
            if iov_type is None:
                # First IoV we have come across.
                iov_type, channel = type(iov), iov.channel
                
            # Record this iov in the current or ended events set
            (current_events if beginning else ended_events).add(iov)
            evaluate_until = lb_n + 2

def quantize_iovs_slow(lbtime, iovs, quantizer=default_quantizing_function):
    from .iov_arrangement import connect_adjacent_iovs
    result = quantize_iovs_slow_interm(lbtime, iovs, quantizer)
    return connect_adjacent_iovs(result)

def quantize_iovs_slow_mc(lbtime, iovs, quantizer=default_quantizing_function):
    from .iov_arrangement import connect_adjacent_iovs
    iovs = [connect_adjacent_iovs(chan_iovs) for chan_iovs in iovs]
    
    iov_type = None
    current_events, ended_events = defaultdict(set), defaultdict(set)
    current_state = defaultdict()
    evaluate_until = defaultdict(int)
    resultstore = defaultdict(lambda: IOVSet(iov_type=iov_type))
    lb_n = last_lbn = RunLumi(0)
    
    for position, index, beginning, iov in iov_yielder(lbtime, *iovs):
        
        if index == 0:
            # Luminosity block started or finished
            
            if beginning:
                # LB start: Remove events which finished in the last LB.
                for key, c_ended_events in ended_events.items():
                    if key in current_events:
                        current_events[key] -= c_ended_events
                        c_ended_events.clear()
                
            else:
                #lb_n = RunLumi(iov.Run, iov.LumiBlock)
                lb_n = iov.Run << 32 | iov.LumiBlock
                
                if last_lbn >> 32 != lb_n >> 32:
                    # We passed a run boundary. Need to evaluate everything for
                    # the next run or two.
                    for channel in current_events:
                        evaluate_until[channel] = lb_n + 2
                
                for channel, active_events in current_events.items():
                    
                    if evaluate_until[channel] >= lb_n:
                        state = current_state[channel] = quantizer(active_events)
                    
                    state = current_state[channel]
                    
                    if state is not None:
                        part = (RunLumiType(lb_n), RunLumiType(lb_n+1), 
                                channel, state)
                        resultstore[channel].add(*part)
                last_lbn = lb_n
        else:
            if iov_type is None: iov_type = type(iov)
            c = iov.channel
            # Record this iov in the current or ended events set
            (current_events[c] if beginning else ended_events[c]).add(iov)
            evaluate_until[c] = lb_n + 2
            
    return [value for key, value in sorted(resultstore.items())]
