#########################################################
#
# Sets FCAL range cut to 0.2 mm (default 0.03 mm)
#
#########################################################

def set_fcal_rangecuts():
    from G4AtlasApps import AtlasG4Eng
    print 'Changing FCAL range cuts'
    if not 'LArMgr:LArMgr' in AtlasG4Eng.G4Eng.Dict_DetFacility:
        print 'set_fcal_rangecuts ERROR : LArMgr:LArMgr Det Facility not found!!'
        return
    theFac = AtlasG4Eng.G4Eng.Dict_DetFacility['LArMgr:LArMgr']
    if not 'FCAL' in theFac.Dict_Regions:
        print 'set_fcal_rangecuts ERROR : FCAL Region not found!!'
        return
    fcal = theFac.Dict_Regions['FCAL']

    fcal.Dict_Cuts['e-'] = 0.2
    fcal.Dict_Cuts['e+'] = 0.2
    fcal.Dict_Cuts['gamma'] = 0.2

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitPhysicsRegions", set_fcal_rangecuts)

