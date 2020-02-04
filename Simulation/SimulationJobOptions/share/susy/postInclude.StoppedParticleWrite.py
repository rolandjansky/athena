from G4AtlasApps import AtlasG4Eng
simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
if not 'DECAYS' in simdict:
    simdict['DECAYS'] = True
    printfunc ('INFO: Added DECAYS=True to simdict')
else:
    printfunc ('WARNING: DECAYS key already set to',simdict['DECAYS'],'not sure if I should change that.')

