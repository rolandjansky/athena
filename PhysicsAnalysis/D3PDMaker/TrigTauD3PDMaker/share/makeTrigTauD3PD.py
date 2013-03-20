from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    # Zee AOD
    '/home/blackbird/tmp/mc08.106052.PythiaZtautau.merge.AOD.e347_s462_s520_r809_r838_tid095955/AOD.095955._000115.pool.root.1',
#    '/home/blackbird/tmp/mc09_7TeV.106052.PythiaZtautau.recon.ESD.e468_s624_s633_r1085_tid112174_00/ESD.112174._000738.pool.root.1',
#    '/home/blackbird/tmp/data09_900GeV.00141811.physics_L1Calo.recon.ESD.r988_tid101483_00/ESD.101483._000058.pool.root.1',
    ]
#athenaCommonFlags.EvtMax = -1
athenaCommonFlags.EvtMax = 10

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.AutoConfiguration = ['everything']
# rec.AutoConfiguration.set_Value_and_Lock(['ConditionsTag'])

include ("RecExCommon/RecExCommon_topOptions.py")

#-------------------------------------------------------------------
# Add extra algorithms
#-------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

d3pd_output_name = 'trigtau.root' 
from TrigTauD3PDMaker.TrigTauD3PD import TrigTauD3PD
alg = TrigTauD3PD(d3pd_output_name)
        
# from AthenaCommon.AppMgr import ServiceMgr
# ServiceMgr.StoreGateSvc.Dump = True

print 'TopAlg'
print job
