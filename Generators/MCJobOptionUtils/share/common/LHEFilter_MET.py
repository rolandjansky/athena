# To use the MET filter in a joboption one would need to include the following lines
    #include('MCJobOptionUtils/LHEFilter_MET.py')
    #lheFilter = LHEFilter_MET()
    #lheFilter.name = "MyCustomMETFilter" # usefull when combining several filters but not mandatory
    #lheFilter.MET = 10 # in GEV
    #lheFilter()

# MET filter
# code adapted from the original filter in ATLMCPROD-4564
class LHEFilter_MET(BaseLHEFilter):
    def __init__(self):
        # name of the filter, usefull for logfiles
        self.name = "MET"
        # set to True to invert the filter
        self.inverted = False
        # parameters for this filter
        self.MET = 0. # default: no MET cut

    def pass_filter(self,Evt):
        met_px = 0.
        met_py = 0.

        for p in Evt:
            if abs(p.pdg_id) in (12, 14, 16):
                met_px += p.px
                met_py += p.py
            elif abs(p.pdg_id) == 15:
                # We consider taus as invisible, as the tau decay
                # is not handled by Powheg but produces additional
                # neutrinos. In principle this is overcompensating
                # but this only leads to a slightly higher filter
                # efficiency and thus slightly less CPU time saving.
                met_px += 0.66*p.px # assume tau -> nu nu mu as "worst case" regarding met
                met_py += 0.66*p.py
        met = (met_px**2 + met_py**2)**0.5
        if met < self.MET:
            return False
        else:
            return True
