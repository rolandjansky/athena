# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# To use the Nquarks filter in a joboption one would need to include the following lines
    #include('MCJobOptionUtils/LHEFilter_NFinalQuarks.py')
    #lheFilter_2HF = LHEFilter_NFinalQuarks()
    #lheFilter_2HF.name = "4bLHEFilter"
    #lheFilter_2HF.Numquarks = 2
    #lheFilter_2HF.Flavours = ['b', 'c']
    #lheFilter_2HF.Ptcut = 0.
    #lheFilter_2HF.pdg_ID_parents = ['25','23','6']
    #lheFilters.addFilter(lheFilter_2HF)
    #lheFilters.run_filters()

# Nquarks filter - select events with a certain number of quarks with given flavours
class LHEFilter_NFinalQuarks(BaseLHEFilter):
    def __init__(self):
        # name of the filter, usefull for logfiles
        self.name = 'Nquarks'
        # set to True to invert the filter
        self.inverted = False
        # parameters for this filter
        self.Numquarks = -1 # Negative integers for inclusive, positive integers for exclusive
        self.pdg_ID_map = {'d':1, 'u':2, 's':3, 'c':4, 'b':5, 't':6}
        self.Flavours = self.pdg_ID_map.keys() # by default consider any quark flavour
        self.Ptcut = 0. # no pT cut by default
        self.pdg_ID_parents = [] # to specify the possible pdgID of the parent of the selected quarks (e.g. 23 to select quarks from Z-boson)

    def select(self,pdgid):
        return pdgid in [ self.pdg_ID_map[flav] for flav in self.Flavours ]

    def pass_filter(self,Evt):
        # first count the quarks
        Nquarks = 0
        for p in Evt:
            # ignore particles which are not outgoing final-state
            if p.status != 1:
                continue
            # apply condition on parents if requested
            if len(self.pdg_ID_parents) != 0:
                if abs(Evt[p.mother0-1].pdg_id) not in [abs(par) for par in self.pdg_ID_parents]:
                    continue
                if abs(Evt[p.mother1-1].pdg_id) not in [abs(par) for par in self.pdg_ID_parents]:
                    continue
            pT = (p.px**2 + p.py**2)**0.5
            if self.select(abs(p.pdg_id)) and (pT >= self.Ptcut):
                Nquarks+=1
        # then decide if we keep the event
        if self.Numquarks<0: # negative means inclusive
            return (Nquarks >= -self.Numquarks)
        else:
            return (Nquarks == self.Numquarks)
