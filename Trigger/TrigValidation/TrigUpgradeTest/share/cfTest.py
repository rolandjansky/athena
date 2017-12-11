###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataFlow( True )

# include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
# svcMgr.ByteStreamInputSvc.FullFileName = [ "./input.data" ]

# # This is the list of proxies to set up so that retrieval attempt will trigger the BS conversion
# if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
#     from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
#     svcMgr += ByteStreamAddressProviderSvc()
# svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RoIBResult/RoIBResult" ]

# Event-level algorithm sequence


# from SGComps.SGCompsConf import SGInputLoader
# topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
# topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','RoIBResult') ]

from AthenaCommon.CFElements import stepSeq


data = {'noreco': [';', ';', ';']}  # in the lists there are the events

data['emclusters'] = ['eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:35000;',
                      'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;',
                      'eta:-0.6,phi:1.7,et:9000;']

data['msmu']  = [';',
                 'eta:-1.2,phi:0.7,pt:6500; eta:-1.1,phi:0.6,pt:8500;',
                 'eta:-1.7,phi:-0.2,pt:9500;']

data['ctp'] = [ 'HLT_g100',  'HLT_2g50 HLT_e20', 'HLT_mu20 HLT_mu8 HLT_2mu8 HLT_mu8_e8' ]

data['l1emroi'] = ['1,1,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.2,0,EM3,EM7',
                   '-0.6,0.2,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM50',
                   '-0.6,1.5,0,EM3,EM7,EM7']

data['l1muroi'] = ['0,0,0,MU0;',
                   '-1,0.5,0,MU6,MU8; -1,0.5,0,MU6,MU8,MU10',
                   '-1.5,-0.1,0,MU6,MU8']

data['tracks'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                  'eta:0.5,phi:0,pt:130000; eta:1,phi:-1.2,pt:60000;eta:-1.2,phi:0.7,pt:6700; eta:-1.1,phi:0.6,pt:8600;',
                  'eta:-0.6,phi:1.7,et:9000;'] # no MU track for MS candidate 'eta:-1.7,phi:-0.2,pt:9500;'

data['mucomb'] = [';',
                  'eta:-1.2,phi:0.7,pt:6600; eta:-1.1,phi:0.6,pt:8600;',
                  ';']

data['electrons'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                     ';',
                     ';']
data['photons'] = ['eta:1,phi:1,pt:130000;',
                   ';',
                   ';']


from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)

include("TrigUpgradeTest/L1CF.py")
include("TrigUpgradeTest/HLTCF.py")



from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
def emHTool(name):
    v = int(name[5:])*1000
    return HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=v, Property="et")


muChains  = [ 'HLT_mu20', 'HLT_mu8', 'HLT_2mu8' ]
eChains   = [ 'HLT_e20' ]
gChains   = [ 'HLT_g100', 'HLT_2g50' ]
mueChains = [ 'HLT_mu8_e8' ]

steps = [ parOR("step%i" % i) for i in range(5)]
stepNo = 0
steps[stepNo] += seqFilter( "Step1MU", Inputs=["L1MU"], Outputs=["step0MU"], Chains=muChains )
steps[stepNo] += seqFilter( "Step1MU_E", Inputs=["L1MU", "L1EM"], Outputs=["step0MU","step0EM"], Chains=mueChains )
steps[stepNo] += seqFilter( "Step1EM", Inputs=["L1EM"], Outputs=["step0EM"], Chains=(eChains + gChains)  )

emHypo = hypo("Step1ElGamHypo", Input="EMClusters", Output="EMDecisions")
emHypoTools = [ emHTool("HLT_e2"), emHTool("HLT_e3"), emHTool("HLT_e5"),
                emHTool("HLT_g5"), emHTool("HLT_g7"), emHTool("HLT_g15") ]
emHypo.HypoTools = emHypoTools 
emHypo += emHypoTools

def msMuHTool(name):
    v = int(name[6:])*1000
    return HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=v, Property="pt")

msMuHypo =  hypo("Step1MuHypo", Input="MSMuons", Output="MSMUonDecisions")
msMuHypoTools = [ msMuHTool("HLT_mu6"), msMuHTool("HLT_mu8"),  msMuHTool("HLT_mu10")  ]
msMuHypo.HypoTools = msMuHypoTools
msMuHypo += msMuHypoTools

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg

stepNo += 1 
em1 = stepSeq( "em1", useExisting("Step1EM"), [ reco("EMRoIs", Output="EMRoIs"),
                                         reco("CaloClustering", FileName="emclusters.dat", Output="EMClusters"),
                                         emHypo ] )
steps[stepNo] += em1
mu1 = stepSeq("mu1", useExisting("Step1MU"), [ reco("MURoIs", Output="MURoIs"),
                                        reco("muMSRecAlg", FileName="msmu.dat", Output="MSMuons"),
                                        msMuHypo ] )
steps[stepNo] += mu1

mue1 = stepSeq("mue1", useExisting("Step1MU_E"), [ useExisting("CaloClustering"), useExisting("muMSRecAlg"),
                                            HLTTest__TestComboHypoAlg("mueHypo1", OutputLevel=DEBUG, Input1="EMClusters", Input2="MSMuons",
                                                                      Output1="step1MUEDecisionsEM", Output2="step1MUEDecisionsMU",
                                                                      Property1="et", Property2="pt", Threshold1=8000, Threshold2=8000,
                                                                      DecisionLabel="HLT_mu8_e8" )  ])
steps[stepNo] += mue1


stepNo += 1
steps[stepNo] += seqFilter("Step2MU", Inputs=["step1MUDecisions"], Outputs=["step2MU"], Chains=muChains )
steps[stepNo] += seqFilter("Step2MU_E", Inputs=[ "step1MUEDecisionsEM", "step1MUEDecisionsMU"], Outputs=["step2MUE_EM", "step2MUE_MU"], Chains=mueChains  )
steps[stepNo] += seqFilter("Step2E", Inputs=["step1EDecisions"], Outputs=["step2E"], Chains=eChains  )
steps[stepNo] += seqFilter("Step2G", Inputs=["step1GDecisions"], Outputs=["step2G"], Chains=gChains )


muCombHypo = hypo("Step2MuHypo", Input="CombMuons", Output="CombMuonDecisions")
muCombHypoTools = [ msMuHTool("HLT_mu6"), msMuHTool("HLT_mu8"),  msMuHTool("HLT_mu10")  ] # this tools are the same as MS ones,
muCombHypo.HypoTools = muCombHypoTools
muCombHypo += muCombHypoTools


eHypo = hypo("Step2ElHypo", Input="Electrons", Output="ElectronDecisions")
eHypoTools = [ emHTool("HLT_e2"), emHTool("HLT_e3"), emHTool("HLT_e5") ]
eHypo.HypoTools = emHypoTools 
eHypo += emHypoTools

gHypo = hypo("Step2GammHypo", Input="Photons", Output="PhotonDecisions")
gHypoTools = [ emHTool("HLT_e2"), emHTool("HLT_e3"), emHTool("HLT_e5") ]
gHypo.HypoTools = emHypoTools 
gHypo += emHypoTools

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestMerger
def merger(name, Inputs, Output ):
    m = HLTTest__TestMerger( "M_"+name, Inputs=Inputs, Output=Output )
    allAlgs[name] = m
    return m

stepNo += 1
mu2 = stepSeq("mu2", useExisting("Step2MU"), [ reco("TrigFastTrackFinder", FileName="tracks.dat", Output="Tracks"),  reco("MuonRecAlg", FileName="mucomb.dat", Output="CombMuons"), muCombHypo ] )
steps[stepNo] += mu2

e2 = stepSeq( "e2" , useExisting("Step2E"), [ useExisting("TrigFastTrackFinder"), reco("ElectronRecAlg", FileName="electrons.dat", Output="Electrons"), eHypo ] )
steps[stepNo] += e2

g2 = stepSeq("g2", useExisting("Step2G"), [ reco("GRoIs", "noreco.dat"), reco("PhotonRecAlg", FileName="photons.dat", Output="Photons"), gHypo ])
steps[stepNo] += g2
stepNo += 1

#mue2 = stepSeq("mue2", useExisting("Step2MU_E"), [ useExisting("TrkRoIs", "noreco.dat"), useExisting("TrigFastTrackFinder"), reco("MuonRecAlg", "mucomb.dat"),  reco("ElectronRecAlg"), hypo("Step2MuEHypo") ])
#steps[stepNo] += mue2


theApp.EvtMax = 3

TopHLTSeq += addSteps(steps)


