# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AccumulatorCache import AccumulatorCache
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import ProductionStep
from G4AtlasServices.G4AtlasServicesConfigNew import PhysicsListSvcCfg


@AccumulatorCache
def load_files_for_quirks_scenario(flags):
    from ExtraParticles.PDGHelpers import getPDGTABLE
    if getPDGTABLE('PDGTABLE.MeV'):
        quirk_firststring=1e-6 #mm
        quirk_maxboost=1e-2
        quirk_maxmerge=1e-6 #mm
        quirk_debugenabled=False
        quirk_debugdist=1000 #mm
        quirk_debugsteps=1000
        simdict = flags.Input.SpecialConfiguration
        quirk_mass = float(simdict["MASS"])
        quirk_charge = float(simdict["CHARGE"])
        quirk_pdgid = int(simdict["PDGID"])
        quirk_stringforce = float(simdict["STRINGFORCE"])

        f = open('PDGTABLE.MeV', 'a')
        f.write("M%8d                          %.8E +0.0E+00 -0.0E+00 Quirk               +\n" % (quirk_pdgid, quirk_mass))
        f.write("W%8d                          0.E+00         +0.0E+00 -0.0E+00 Quirk               +\n" % quirk_pdgid)
        f.close()

        if flags.Common.ProductionStep == ProductionStep.Simulation:
            f = open('quirks_setup.txt', 'w')
            for x in [quirk_mass, quirk_charge, quirk_pdgid, quirk_stringforce, quirk_firststring, quirk_maxboost, quirk_maxmerge, quirk_maxmerge]:
                f.write(repr(x) + "\n")
            if quirk_debugenabled:
                f.write("1\n")
                f.write(repr(quirk_debugdist) + "\n")
                f.write(repr(quirk_debugsteps) + "\n")
            else:
                f.write("0\n")
            f.close()
        del quirk_firststring, quirk_maxboost, quirk_maxmerge, quirk_debugenabled, quirk_debugdist, quirk_debugsteps, doG4SimConfig, simdict, f, quirk_mass, quirk_charge, quirk_pdgid, quirk_stringforce


def QuirkPhysicsToolCfg(flags, name="QuirkPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools( CompFactory.QuirkPhysicsToolTool(name, **kwargs) )
    return result


## def getDebugSteppingActionTool(name="G4UA::DebugSteppingActionTool", **kwargs): # FIXME missing functionality
##     from Quirks.QuirksConf import G4UA__DebugSteppingActionTool
##     from G4AtlasApps.SimFlags import simFlags
##     # use configuration passed through the flags
##     if name in simFlags.UserActionConfig.get_Value().keys():
##         for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
##             kwargs.setdefault(prop,value)
##
##     return G4UA__DebugSteppingActionTool(name, **kwargs)


def QuirksCfg(flags):
    result = ComponentAccumulator()
    load_files_for_quirks_scenario(flags)
    if flags.Common.ProductionStep == ProductionStep.Simulation:
        result.merge(PhysicsListSvcCfg(flags))
        result.getService("PhysicsListSvc").PhysOption += [ result.popToolsAndMerge(QuirkPhysicsToolCfg(flags)) ]
    return result
