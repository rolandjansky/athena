# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/LumiBlockMuWriterDefault.py
# Created: May 2020, sss
# Purpose: Configure LumiBlockMuWriter.
#


from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.BeamFlags import jobproperties


def LumiBlockMuWriterDefault (name = 'LumiBlockMuWriter'):
    from AthenaCommon.AlgSequence import AthSequencer
    from AthenaCommon.AlgSequence import AlgSequence

    condSeq = AthSequencer ('AthCondSeq')
    topSequence = AlgSequence()
    if hasattr (condSeq, name) or hasattr (topSequence, name):
        return

    LumiBlockMuWriter = CompFactory.LumiBlockMuWriter # LumiBlockComps
    if jobproperties.Beam.beamType() == 'cosmics':
        condkey = ''
    else:
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        condalg = LuminosityCondAlgDefault()
        condkey = condalg.LuminosityOutputKey
        
    alg = LumiBlockMuWriter (name, LumiDataKey = condkey)

    cnvseq = None
    cnvalg = getattr (condSeq, 'xAODMaker::EventInfoCnvAlg', None)
    if cnvalg:
        cnvseq = condSeq
    else:
        cnvalg = getattr (topSequence, 'xAODMaker::EventInfoCnvAlg', None)
        if cnvalg:
            cnvseq = topSequence

    # FIXME: If EventInfoCnvAlg is in topSequence, then this needs to come
    # after it.  Otherwise, schedule to condSeq so we'll be run early.
    if cnvalg and cnvseq is topSequence:
        topSequence += alg
    else:
        condSeq += alg

    from AthenaCommon.GlobalFlags  import globalflags
    from RecExConfig.ObjKeyStore import objKeyStore

    if (cnvalg  and
        not globalflags.InputFormat.is_bytestream() and
        not objKeyStore.isInInput ("xAOD::EventInfo")):
        sgil = getattr (topSequence, 'SGInputLoader', None)
        if sgil:
            for k in sgil.Load[:]:
                if k[1].find ('EventInfo') >= 0:
                    sgil.Load.remove (k)
    return

