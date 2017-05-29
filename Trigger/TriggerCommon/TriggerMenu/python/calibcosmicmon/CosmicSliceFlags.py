# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Cosmics slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'A. Cerri, after X. Wu example'
__version__="$Revision: 1.40 $"
__doc__="Cosmic slice specific flags  "


_flags = []

class SliceList(JobProperty):
    """ List of slices provided by the Cosmics slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[] 
    
_flags.append(SliceList)

class CommandLineProperties(JobProperty):
    """ properties which will be affected by setting a global with the same name """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(CommandLineProperties)

class Use3Streams(JobProperty):
    """ UseSingleBeam7BitL1Menu """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(Use3Streams)

class UseSingleBeam3BitL1Menu(JobProperty):
    """ UseSingleBeam3BitL1Menu """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(UseSingleBeam3BitL1Menu)

class UseSingleBeam7BitL1Menu(JobProperty):
    """ UseSingleBeam7BitL1Menu """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(UseSingleBeam7BitL1Menu)

class testCosmic(JobProperty):
    """ testCosmic """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(testCosmic)

class BFieldOn(JobProperty):
    """ is B Field on? """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(BFieldOn)

class GlobalTag(JobProperty):
    """ GlobalTag """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue="OFLCOND-CSC-00-01-00"
_flags.append(GlobalTag)

class DontWriteOutAllTEStream(JobProperty):
    """ DontWriteOutAllTEStream """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(DontWriteOutAllTEStream)

class useUniqueMuonEFInstance(JobProperty):
    """ useUniqueMuonEFInstance """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(useUniqueMuonEFInstance)

class useUniqueMuFastInstance(JobProperty):
    """ useUniqueMuFastInstance """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(useUniqueMuFastInstance)

class useCOMCONDDB(JobProperty):
    """ use Commissioning condition DB """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(useCOMCONDDB)

class useCOMCONDDBInLAr(JobProperty):
    """ use Commissioning condition DB in LAr setup """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(useCOMCONDDBInLAr)

class UsedLVL1Thresholds(JobProperty):
    """ used LVL1 Thresholds """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(UsedLVL1Thresholds)

class UsedLVL1Signatures(JobProperty):
    """ used LVL1 Thresholds """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(UsedLVL1Signatures)

class filterEmptyROB(JobProperty):
    """ filterEmptyROB """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(filterEmptyROB)

class forceLVL2Accept(JobProperty):
    """ forceLVL2Accept """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(forceLVL2Accept)

class FakeLVL1(JobProperty):
    """ FakeLVL1 """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(FakeLVL1)

class FakeLVL1Threshold(JobProperty):
    """ FakeLVL1Threshold """ 
    statusOn=True
    allowedTypes=['str']
    StoredValue='MU3'
_flags.append(FakeLVL1Threshold)

class FakeLVL1ROIEta(JobProperty):
    """ FakeLVL1ROIEta """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=-0.5
_flags.append(FakeLVL1ROIEta)

class FakeLVL1ROIPhi(JobProperty):
    """ FakeLVL1ROIPhi """ 
    statusOn=True
    allowedTypes=['float']
    StoredValue=-1.5
_flags.append(FakeLVL1ROIPhi)

class T2CaloFakeLVL1InLVL2(JobProperty):
    """ T2CaloFakeLVL1InLVL2 """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(T2CaloFakeLVL1InLVL2)

class isRealData(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(isRealData)

class doM3Data(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(doM3Data)

class doM4Data(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(doM4Data)

class doEFIDCosmics(JobProperty):
    """ run the Empty slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doEFIDCosmics)

class doIDHitsHypo(JobProperty):
    """ run the Dummy slice """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doIDHitsHypo)

class L2HitsHypoNTRTDC(JobProperty):
    """ number of TRT DC in IDHITSHYPO selection (L2) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(L2HitsHypoNTRTDC)

class EFHitsHypoNTRTDC(JobProperty):
    """ number of TRT DC in IDHITSHYPO selection (EF) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(EFHitsHypoNTRTDC)

class L2HitsHypoNTRTHTDC(JobProperty):
    """ number of TRT HTDC in IDHITSHYPO selection (L2) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(L2HitsHypoNTRTHTDC)

class EFHitsHypoNTRTHTDC(JobProperty):
    """ number of TRT HTDC in IDHITSHYPO selection (EF) """ 
    statusOn=True
    allowedTypes=['int']
    StoredValue=0
_flags.append(EFHitsHypoNTRTHTDC)

for x in SliceList.StoredValue:
    exec """class do%s(JobProperty):
\t ' run the %s slice '
\t statusOn=True
\t allowedTypes=['bool']
\t StoredValue=False
_flags.append(do%s)
""" % (x,x,x)

for x in ['StandaloneThreshold',
          'TrackingThreshold',
          'CalibrationThreshold',
          'StandaloneThreshold_V2',
          'TrackingThreshold_V2',
          'CalibrationThreshold_V2']:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='EM3'
_flags.append(%s)
""" % (x,x,x)

for x in ['StandaloneSignature',
          'TrackingSignature',
          'CalibrationSignature']:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_EM3'
_flags.append(%s)
""" % (x,x,x)

for x in ['StandaloneSignature_V2',
          'TrackingSignature_V2',
          'CalibrationSignature_V2']:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_EM3_EMPTY'
_flags.append(%s)
""" % (x,x,x)

for x in ['TGCSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU0_TGC'
_flags.append(%s)
""" % (x,x,x)

for x in ['TGCThreshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU0_TGC'
_flags.append(%s)
""" % (x,x,x)

for x in ['RPCSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['RPCThreshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['WidePatternRPCSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU0_LOW_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['WidePatternRPCThreshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU0_LOW_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['MuonSignature_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU0_EMPTY'
_flags.append(%s)
""" % (x,x,x)

for x in ['MuonThreshold_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU0'
_flags.append(%s)
""" % (x,x,x)



for x in ['CaloSignature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_J10'
_flags.append(%s)
""" % (x,x,x)

for x in ['CaloSignature_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_J10_EMPTY'
_flags.append(%s)
""" % (x,x,x)


for x in ['CaloThreshold',
          'CaloThreshold_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='J10'
_flags.append(%s)
""" % (x,x,x)


for x in ['Signature',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['Threshold',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU6_RPC'
_flags.append(%s)
""" % (x,x,x)

for x in ['Signature_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='L1_MU6_EMPTY'
_flags.append(%s)
""" % (x,x,x)

for x in ['Threshold_V2',
          ]:
    exec """class %s(JobProperty):
\t ' %s '
\t statusOn=True
\t allowedTypes=['str']
\t StoredValue='MU6'
_flags.append(%s)
""" % (x,x,x)




class signatures(JobProperty):
    """ signatures in Cosmic slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= SliceList.StoredValue
_flags.append(signatures)

class LVL2Payload(JobProperty):
    """ LVL2 payload """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(LVL2Payload)

class EFPayload(JobProperty):
    """ EF payload """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(EFPayload)

# create container

class CosmicSlice(JobPropertyContainer, CommonSliceHelper):
    """ Cosmic Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CosmicSlice)

# add add common slice flags
TriggerFlags.CosmicSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.CosmicSlice.add_JobProperty(flag)
del _flags

# make an alias
CosmicSliceFlags = TriggerFlags.CosmicSlice

