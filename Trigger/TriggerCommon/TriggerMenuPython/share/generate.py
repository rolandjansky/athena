



from AthenaCommon.AthenaCommonFlags import jobproperties 
from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties


#jobproperties.Global.Luminosity = "zero"
#jobproperties.Global.Luminosity.set_zero()


if 'GlobalFlags' not in dir():
    from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.Luminosity.set_zero()


from TriggerJobOpts.TriggerFlags import TriggerFlags

TriggerFlags.triggerMenuSetup="MC_InitialBeam_HI_v1"

global doTruth
doTruth=True



jobproperties.print_JobProperties('tree&value')


from TriggerMenuPython.GenerateMenu import GenerateMenu

g = GenerateMenu()
g.generate()

jobproperties.AthenaCommonFlags.EvtMax=0

jobproperties.print_JobProperties('tree&value')

raise Exception("do not know how to stop other way this job, all is OK")
