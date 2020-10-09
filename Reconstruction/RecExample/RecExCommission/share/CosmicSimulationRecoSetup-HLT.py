LArDigitKey='FREE'

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'commis'
globalflags.DataSource.set_Value_and_Lock('geant4')

from RecExConfig.RecFlags import rec
rec.Commissioning.set_Value_and_Lock(True)
rec.oldFlagTopSteering.set_Value_and_Lock(False)
#rec.doTruth.set_Value_and_Lock(False)
#rec.doJiveXML.set_Value_and_Lock(True)

if rec.readRDO :
    rec.doTrigger.set_Value_and_Lock(True)
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    #####for l1 simulation configuration
    tf.doCosmicSim.set_Value_and_Lock(True)
    #############
    tf.readLVL1configFromXML.set_Value_and_Lock(True)
    tf.readHLTconfigFromXML.set_Value_and_Lock(True)

    tf.triggerMenuSetup.set_Value_and_Lock('cosmic_default')


# for running over data with field on need this to be setto true
from AthenaCommon.BFieldFlags import jobproperties

# set the field properties based on DetDescrVersion
if globalflags.DetDescrVersion.get_Value().find('ATLAS-GEONSF-') >= 0 :
    jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIMCT-00-00-00')
    print('configuring reco with solenoid off, toroid on from DetDescrVersion:',globalflags.DetDescrVersion.get_Value())
    
if globalflags.DetDescrVersion.get_Value().find('ATLAS-GEO-') >= 0 :
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIMCST-00-00-00')
    print('configuring reco with solenoid on, toroid on from DetDescrVersion:',globalflags.DetDescrVersion.get_Value())
    
if globalflags.DetDescrVersion.get_Value().find('ATLAS-GEONF-') >= 0 :
    jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIMC-00-00-00')
    print('configuring reco with solenoid off, toroid off from DetDescrVersion:',globalflags.DetDescrVersion.get_Value())
        
if globalflags.DetDescrVersion.get_Value().find('ATLAS-GEONTF-') >= 0 :
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIMCS-00-00-00')
    print('configuring reco with solenoid on, toroid off from DetDescrVersion:',globalflags.DetDescrVersion.get_Value())
 
