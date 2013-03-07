#! /usr/bin/env python

from DQUtils import IOVSet, fetch_iovs, process_iovs, define_iov_type
from DQUtils.sugar import RunLumi
from DQUtils.iov_arrangement import inverse_lblb, connect_adjacent_iovs

def main():
    runs = 185640, 185660

    lblb = fetch_iovs("LBLB", runs=runs)
    lbtime = inverse_lblb(lblb)
    beam_iovs = fetch_iovs("COOLOFL_DCS::/LHC/DCS/FILLSTATE", lbtime.first.since, lbtime.last.until)

    stable_beam_by_lb = {}

    for since_time, until_time, (lumiblock, beamstate) in process_iovs(lbtime, beam_iovs):
        if not lumiblock:
            # Not inside lumiblock
            continue
            
        lb = RunLumi(lumiblock.Run, lumiblock.LumiBlock)
        
        if lb in stable_beam_by_lb:
            # Require stable beam for ALL of the lumiblock (hence 'and')
            # Replace with 'or' to get ANY of the lumiblock
            stable_beam_by_lb[lb] = stable_beam_by_lb[lb] and beamstate.StableBeams
        else:
            stable_beam_by_lb[lb] = beamstate.StableBeams
            
        
    @define_iov_type
    def STABLEBEAM_VAL(stable):
        """
        Define an IOV type which just has .stable
        """
        
    result = (STABLEBEAM_VAL(runlb, runlb+1, state)
              for runlb, state in sorted(stable_beam_by_lb.iteritems()))
    result = IOVSet(connect_adjacent_iovs(result))

    result.pprint()
    
if __name__ == "__main__":
    main()
