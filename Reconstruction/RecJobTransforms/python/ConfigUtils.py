#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# First define a list of functions
# Then call them from PredefinedConfigFunctions(key,runArgs) at the end

def ConfigureFieldAndGeo(runArgs):
    print "Method ConfigureFieldAndGeo..."
    from PATJobTransforms.GeoConfig import GetRunNumber
    RunNumber=GetRunNumber(runArgs)
    print "Configuring geometry and field from RunNuber",RunNumber
    from RecExCommission.RecExCommission_MagneticFieldConfig import setFieldConfig
    setFieldConfig(RunNumber)
    return

#------------------------
def SetBeamType(runArgs):
    print "Method SetBeamType..."
    BeamType='NONE'
    if hasattr(runArgs,"beamType"):
        BeamType=runArgs.beamType
    elif hasattr(runArgs,"outputESDFile") or hasattr(runArgs,"outputAODFile") or hasattr(runArgs,"inputESDFile") or hasattr(runArgs,"inputAODFile"):
        print "Attempting to set beamType from outputESD or outputAOD file..."
        print "known cosmics projects: data08_cos, data08_cosmag"
        print "known single beam projects: data08_1beam, data08_1beammag"
        print "know collisions projects: data08_coll900, data08 "

        if hasattr(runArgs,"outputESDFile"): fileName=runArgs.outputESDFile
        elif hasattr(runArgs,"outputAODFile"): fileName=runArgs.outputAODFile
        elif hasattr(runArgs,"inputESDFile"): fileName=runArgs.inputESDFile[0]
        elif hasattr(runArgs,"inputAODFile"): fileName=runArgs.inputAODFile[0]
        #base on a file named like this:
        #run08_cos.00077585.physics_HLT_Cosmics_NIM4.recon.ESD.o4_r509_tid024433/ESD.024433._00001.pool.root.2
        #or directly
        #data08_cosmag.00090415.physics_CosmicMuons.recon.ESD.o4_f68._lb0001._sfo02._0001

        #if several directories, take the first part
        subNames=fileName.split('/')
        firstName=subNames[0]
        #from run08_cos.ESD.pool.root, split by dots and project is the first part
        project = firstName.split('.')[0]
        print 'project = ',project
        if project=="data08_cos" or project=="data08_cosmag": BeamType='cosmics'
        elif project=="data08_1beam" or project=="data08_1beammag": BeamType='singlebeam'
        elif project=="data08" or project=="data08_coll900": BeamType='collisions'
        else:
            print "\n\nWARNING - unknown project name '%s'\n\n"%project
            print "Will try to guess project from 'data', '_cos', '_1beam', '_coll' keywords..."
            if project.startswith('data'):
                if project.rfind('_cos')>0: BeamType='cosmics'
                elif project.rfind('_1beam')>0: BeamType='singlebeam'
                elif project.rfind('_coll')>0: BeamType='collisions'                
            if BeamType is 'NONE': 
                print "Unexpected project name. Maybe the project position is wrong?"
                print "Trying to search the complete file name for known projects... [%s]"%fileName
                if fileName.rfind("data08_cos")>0 or fileName.rfind("data08_cosmag")>0: BeamType='cosmics'
                elif fileName.rfind("data08_1beam")>0 or fileName.rfind("data08_1beammag")>0: BeamType='singlebeam'
                elif fileName.rfind("data08.")>0 or fileName.rfind("data08_coll900")>0: BeamType='collisions'
                else:
                    raise RuntimeError("(2) Unable to determine beamType from file named '%s'"%fileName)
            
    if BeamType is 'NONE':            
        raise RuntimeError("(3) Unable to determine beamType from file name nor from direct argument")
    else:
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.beamType.set_Value_and_Lock( BeamType )
        print "The beamType is set to",BeamType

    return


def PredefinedConfigFunctions(key,runArgs):
    if key is "FieldAndGeo":
        ConfigureFieldAndGeo(runArgs)
    elif key is "BeamType":
        SetBeamType(runArgs)
    else:
        raise RuntimeError("Unknown key '%s' in PredefinedConfigFunctions(key,runArgs)"%key)
    return

