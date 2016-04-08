#!/usr/bin/env python

if __name__=="__main__":

    """example how to get the HLT rates from COOL"""

    runs = [165815, 165817]
    triggerpattern = "L2_e5"

    from TrigStatsFromCool.HLTRatesCoolReader import getCounts, getL2Counts, getEFCounts

    # getCounts returns L2 and EF rates
    l2rates, efrates = getCounts(runs, triggerpattern)

    # one can also do just one level (faster)
    # l2rates = getL2Counts(runs, triggerpattern)

    for run in runs:
        print "Run",run,"with triggers",l2rates[run].triggernames()

        for r in l2rates[run]['L2_e5_medium'].rates():
            print "LB=",r.lb(),", rate=",r.rate(),"Hz, counts=",r.counts(),", duration=",r.duration(),"sec","*" if not r.hasL1A() else ""

