#########################################################
#
# Sets EMEC range cut to 0.2 mm (default 0.1 mm)
#
#########################################################

def set_emec_rangecuts():
    from G4AtlasApps import AtlasG4Eng
    print 'Changing EMEC range cuts'
    if not 'LArMgr:LArMgr' in AtlasG4Eng.G4Eng.Dict_DetFacility:
        print 'set_emec_rangecuts ERROR : LArMgr:LArMgr Det Facility not found!!'
        return
    theFac = AtlasG4Eng.G4Eng.Dict_DetFacility['LArMgr:LArMgr']
    if not 'EMEC' in theFac.Dict_Regions:
        print 'set_emec_rangecuts ERROR : EMEC Region not found!!'
        return
    emec = theFac.Dict_Regions['EMEC']

    emec.Dict_Cuts['e-'] = 0.2
    emec.Dict_Cuts['e+'] = 0.2
    emec.Dict_Cuts['gamma'] = 0.2

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitPhysicsRegions", set_emec_rangecuts)

