# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from collections import namedtuple, defaultdict

from DQUtils.events import process_iovs 
from DQUtils.sugar import RunLumi, iov_type
from DQUtils.iov_arrangement import split_by_channel
from DQUtils.db import fetch_iovs
from DQUtils.oracle import atlas_runs_set

from pprint import pprint

class PTAGIL_IOV(namedtuple("PTAGIL_IOV", "since until lumi tag rfp"), iov_type):
    pass

def make_run_selector(good_runs):
    def yielder(generator):
        for iov in generator:
            if iov.since.run in good_runs or iov.until.run in good_runs:
                yield iov
    return yielder
 
def make_ptag_integrated_lumi_iovs(lest_iovs, lblb_iovs, eor_iovs, rfp_iovs):
    events = process_iovs(lest_iovs, lblb_iovs, eor_iovs, rfp_iovs)
    
    for since, until, (lest, lb, eor, rfp) in events:
        if (lest and lb and eor and 
            lest.LBAvInstLumi > 0 and (lest.Valid & 0xFFFF) % 10 == 0
                                  and (lest.Valid & 0xFFFF) // 100 == 0):
            
            lb_seconds = (lb.EndTime - lb.StartTime) / 1e9
            lumi = lest.LBAvInstLumi * lb_seconds
            if lumi > 1:
                print "Hmm. Odd lumi: %.2e, %.1f" % (lumi, lb_seconds), lest
                continue
            
            print since, until, "%.3e" % lumi
            
            yield PTAGIL_IOV(since, until, lumi, eor.FilenameTag, 
                             rfp.ReadyForPhysics)

def accumulate_result(ptagil_iovs, state_iovs):
    
    class ResultStore(object):
        def __init__(self):
            self.total = 0
            self.green = 0
        
        def update(self, state, lumi):
            self.total += lumi
            
            colour = state.Code if state else None
            
            if colour == 3:
                self.green += lumi
                
        def __repr__(self):
            args = self.total, self.green, "%.1f %%" % (self.green / self.total * 100)
            return "<ResultStore total=%.3e green=%.3e green = %s>" % args
    
    result_by_tag = defaultdict(lambda: defaultdict(ResultStore))
    
    iov_by_channel = split_by_channel(state_iovs)
    channel_ids, state_iovs = zip(*sorted(iov_by_channel.items()))
    
    consider_rfp = set(("PIX0", "PIXB", "PIXEA", "PIXEC", 
                        "SCTB", "SCTEA", "SCTEC"))
    
    for since, until, states in process_iovs(ptagil_iovs, *state_iovs):
        ptagil = states[0]
        codes = states[1:]
        if not ptagil:
            continue
    
        tag_result = result_by_tag[ptagil.tag]
    
        for channel_id, state in zip(channel_ids, codes):
            if channel_id in consider_rfp and not ptagil.rfp:
                continue
            
            tag_result[channel_id].update(state, ptagil.lumi)
        
    result = {}
    for tag, part in result_by_tag.iteritems():
        result[tag] = dict(part)
    return result
    
def main():
    since, until = RunLumi(152000, 0), RunLumi(155000, 0)
    
    #since, until = RunLumi(152777, 0), RunLumi(152778, 0) - 1
    
    S = make_run_selector(atlas_runs_set())
    
    lest_iovs = S(fetch_iovs("LBLESTONL", since, until, [0]))
    lblb_iovs = S(fetch_iovs("LBLB", since, until))
    eor_iovs = S(fetch_iovs("EOR", since, until))
    rfp_iovs = S(fetch_iovs("/TDAQ/RunCtrl/DataTakingMode", since, until, 
                               database="tdaq"))
    
    args = lest_iovs, lblb_iovs, eor_iovs, rfp_iovs
    ptagil_iovs = make_ptag_integrated_lumi_iovs(*args)
    
    state_iovs = fetch_iovs("SHIFTOFL", since, until, named_channels=True) # [111, 114, 115, 121])
    
    result = accumulate_result(ptagil_iovs, state_iovs)
        
    from pprint import pprint
    
    pprint(result["data10_7TeV"])
    
    
    
if __name__ == "__main__":

    from IPython.Shell import IPShellEmbed
    ip = IPShellEmbed(["-pdb"], rc_override=dict(quiet=True))

    main()
