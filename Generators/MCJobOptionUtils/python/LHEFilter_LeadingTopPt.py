# To use the LeadingTopPt filter in a joboption one would need to include the following lines
    #include('MCJobOptionUtils/LHEFilter_LeadingTopPt.py')
    #lheFilter = LHEFilter_LeadingTopPt()
    #lheFilter.name = "MyCustomLeadingTopPt" # usefull when combining several filters but not mandatory
    #lheFilter.Ptcut = 300000. # at least 300GeV for the leading top
    #lheFilter()

# NLeptons filter
class LHEFilter_LeadingTopPt(BaseLHEFilter):
    def __init__(self):
        # name of the filter, usefull for logfiles
        self.name = "LeadingTopPt"
        # set to True to invert the filter
        self.inverted = False
        # parameters for this filter
        self.Ptcut = 0. # no pT cut by default

    def pass_filter(self,Evt):
        # first count the leptons
        leadTopPt = 0.
        for p in Evt:
            if abs(p.pdg_id) !=6:
                continue
            pT = (p.px**2 + p.py**2)**0.5
            if pT>leadTopPt:
                leadTopPt = pT
        # then decide if we keep the event
        if leadTopPt>=self.Ptcut:
            return True
        else:
            return False
