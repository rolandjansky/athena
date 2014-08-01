#########################################################
#
# SimulationJobOptions/postOptions.PionDecayer.py
# Trygve Buanes
#
#########################################################

def piondecayer_setup():
    from G4AtlasApps import AtlasG4Eng
    from G4AtlasApps.SimFlags import simFlags

    AtlasG4Eng.G4Eng.load_Dict("PionDecayerDict")
    pionDecayer = AtlasG4Eng.G4Eng.gbl.PionDecayer("PionDecayer")
    pionDecayer.switchOn()

    if simFlags.PionIndex == '017981':
        pionDecayer.UseCustomDecayMode()
        pionDecayer.SetMother(531)
        pionDecayer.AddDecayProduct(-321)
        pionDecayer.AddDecayProduct(211)
    elif simFlags.PionIndex == '019250':
        pionDecayer.UseCustomDecayMode()
        pionDecayer.SetMother(531)
        pionDecayer.AddDecayProduct(-321)
        pionDecayer.AddDecayProduct(-13)
        pionDecayer.AddDecayProduct(14)
    elif simFlags.PionIndex == '019251':
        pionDecayer.UseCustomDecayMode()
        pionDecayer.SetMother(531)
        pionDecayer.AddDecayProduct(-321)
        pionDecayer.AddDecayProduct(-13)
        pionDecayer.AddDecayProduct(14)
    elif simFlags.PionIndex == 'MinBias':
        pionDecayer.SetPtmuon(2*GeV)
    elif simFlags.PionIndex == 'MinBias35':
        pionDecayer.SetPtmuon(3.5*GeV)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitPhysics", piondecayer_setup)
