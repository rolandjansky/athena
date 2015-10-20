from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data') #force to use the data database
#globalflags.DataSource.set_Value_and_Lock('geant4') #force to use the mc database

#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_data()
#GlobalFlags.DataSource.set_geant4()

## FIXME This part should go in the transform skeleton(s) in the future.
if 'runArgs' in dir():
    if hasattr(runArgs,'DataRunNumber'):
        from RecExConfig.RecFlags import rec
        rec.projectName = 'data'+str(runArgs.DataRunNumber%100)

