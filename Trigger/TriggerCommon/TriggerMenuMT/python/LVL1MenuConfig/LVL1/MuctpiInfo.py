# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class MuctpiInfo:
    def __init__(self, low_pt=0, high_pt=0, max_cand=13):
        self.low_pt = low_pt
        self.high_pt = high_pt
        self.max_cand = max_cand

    def xml(self, ind=1, step=2):
        return ind * step * ' ' + '<MuctpiInfo name="muctpi" low_pt="%i" high_pt="%i" max_cand="%i"/>\n' % (self.low_pt, self.high_pt, self.max_cand)
