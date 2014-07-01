# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ContainerFlagFillerTool.py 495152 2012-04-11 15:09:02Z ssnyder $
#
# @file D3PDMakerCoreComps/python/ContainerFlagFillerTool.py
# @author scott snyder <snyder@bnl.gov>
# @date Apr, 2012
# @brief Helper to configure ContainerFlagFillerTool.
#


from D3PDMakerCoreComps import D3PDMakerCoreCompsConf


# ??? Generalize and turn this into a general utility?
def _make_getter (name, cname):
    cc = cname.split ('/')
    if len(cc) != 2:
        raise ValueError ('Bad container reference; expected TYPE/NAME: ' +
                          cname)
    (typename, sgkey) = cc
    if typename == 'INav4MomLinkContainer':
        import EventCommonD3PDMaker
        cls = EventCommonD3PDMaker.INav4MomLinkContainerGetterTool
    else:
        cls = D3PDMakerCoreComps.SGDataVectorGetterTool
    return cls (name = name, TypeName = typename, SGKey = sgkey)


class ContainerFlagFillerTool (D3PDMakerCoreCompsConf.D3PD__ContainerFlagFillerTool):
    def __init__ (self, name, FlagNames=[], Getters=[], **kwargs):
        """Helper to configure ContainerFlagFillerTool.
A flag name may be followed by a string of the form `@TYPE/SGKEY' (before
the docstring, if any).  If so, then we automatically add an
apppropriate getter to the corresponding Getters slot.
"""
        FlagNames = FlagNames[:]
        Getters = Getters[:]
        for (i,f) in enumerate (FlagNames):
            ipos = f.find ('@')
            if ipos > 0:
                while len(Getters) <= i:
                    Getters.append (None)
                jpos = f.find (':', ipos)
                if jpos < 0: jpos = len(f)
                cname = f[ipos+1:jpos]
                flag = f[:ipos]
                FlagNames[i] = f[:ipos] + f[jpos:]
                Getters[i] = _make_getter (flag + '_Getter', cname)
        D3PDMakerCoreCompsConf.D3PD__ContainerFlagFillerTool.__init__ \
          (self, name, FlagNames = FlagNames, Getters = Getters, **kwargs)
        return
                                                              
        
