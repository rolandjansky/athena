
# check if the reco algorithm can be found and pick the cut values (to be
# added to histogram titles)
def LArNoisyROCutHelper(property):
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    if hasattr(job, 'LArNoisyROAlg'):
        alg = getattr(job, 'LArNoisyROAlg')

        Qcut = None
        if hasattr(alg, 'CellQualityCut'):
            Qcut = getattr(alg, 'CellQualityCut')
        else:
            Qcut = alg.getDefaultProperty('CellQualityCut')

        if hasattr(alg, property):
            return '(>'+str(getattr(alg, property))+' chan with Q>'+str(Qcut)+')'
        else:
            return '(>'+str(alg.getDefaultProperty(property))+' chan with Q>'+str(Qcut)+')'


    else:
        return ''

# check if the reco algorithm can be found and pick the cut value to define bad events
def LArNoisyROFEBCutHelper():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    print job
    if hasattr(job, 'LArNoisyROAlg'):
        alg = getattr(job, 'LArNoisyROAlg')

        BadFEBCut = None
        if hasattr(alg, 'BadFEBCut'):
            BadFEBCut = getattr(alg,'BadFEBCut')
        else:
            BadFEBCut = alg.getDefaultProperty('BadFEBCut')

        return BadFEBCut

    else:
        return 9999999




# --- for athena online running --
if 'EventBlockSize' not in dir():
    EventBlockSize=0


from LArMonTools.LArMonToolsConf import LArNoisyROMon

OnlineMode=athenaCommonFlags.isOnline()

# variable for testing on ESD
try:
    LArNoisyROMonForceTrigger
except NameError:
    LArNoisyROMonForceTrigger = False


theLArNoisyROMon = LArNoisyROMon(name="LArNoisyROMon")
theLArNoisyROMon.IsOnline = OnlineMode
theLArNoisyROMon.ProcessNEvents= EventBlockSize
theLArNoisyROMon.NoisyFEBDefStr = LArNoisyROCutHelper('BadChanPerFEB')
theLArNoisyROMon.BadFEBCut = LArNoisyROFEBCutHelper()

from RecExConfig.RecFlags import rec
if rec.doTrigger or LArNoisyROMonForceTrigger:
    theLArNoisyROMon.doTrigger = True
    theLArNoisyROMon.EFNoiseBurstTriggers = [
        "EF_j165_u0uchad_LArNoiseBurst",
        "EF_j30_u0uchad_empty_LArNoiseBurst",
        "EF_j55_u0uchad_firstempty_LArNoiseBurst",
        "EF_j55_u0uchad_empty_LArNoiseBurst",
        "EF_xe45_LArNoiseBurst",
        "EF_xe55_LArNoiseBurst",
        "EF_xe60_LArNoiseBurst",
        "EF_j55_u0uchad_firstempty_LArNoiseBurstT",
        "EF_j100_u0uchad_LArNoiseBurstT",
        "EF_j165_u0uchad_LArNoiseBurstT",
        "EF_j130_u0uchad_LArNoiseBurstT",
        "EF_j35_u0uchad_empty_LArNoiseBurst",
        "EF_j35_u0uchad_firstempty_LArNoiseBurst",
        "EF_j80_u0uchad_LArNoiseBurstT"
        ]
    theLArNoisyROMon.L1NoiseBurstTriggers = [
        "L1_J75",
        "L1_J10_EMPTY",
        "L1_J30_FIRSTEMPTY",
        "L1_J30_EMPTY",
        "L1_XE40",
        "L1_XE50",
        "L1_XE50_BGRP7",
        "L1_XE70"
        ]

    # for TrigDecisionTool
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['ds']
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()
else:
    theLArNoisyROMon.doTrigger = False

ToolSvc += theLArNoisyROMon
LArMon.AthenaMonTools += [theLArNoisyROMon]
