# To use the NLeptons filter in a joboption one would need to include the following lines
    #include('MCJobOptionUtils/LHEFilter_NLeptons.py')
    #lheFilter = LHEFilter_NLeptons()
    #lheFilter.name = "MyCustomNLeptonsFilter" # usefull when combining several filters but not mandatory
    #lheFilter.NumLeptons = 1
    #lheFilter.Ptcut = 0.
    #lheFilter()

# NLeptons filter
class LHEFilter_NLeptons(BaseLHEFilter):
    def __init__(self):
        # name of the filter, usefull for logfiles
        self.name = "NLeptons"
        # set to True to invert the filter
        self.inverted = False
        # parameters for this filter
        self.NumLeptons = -1 # Negative integers for inclusive, positive integers for exclusive
        self.LeptonFlavours = [11, 13, 15] # by default only consider charged leptons
        self.Ptcut = 0. # no pT cut by default

    def pass_filter(self,Evt):
        # first count the leptons
        NLeptons = 0
        for p in Evt:
            pT = (p.px**2 + p.py**2)**0.5
            if (abs(p.pdg_id) in self.LeptonFlavours) and (pT >= self.Ptcut):
                NLeptons+=1
        # then decide if we keep the event
        if self.NumLeptons<0: # negative means inclusive
            return (NLeptons >= -self.NumLeptons)
        else:
            return (NLeptons == self.NumLeptons)
