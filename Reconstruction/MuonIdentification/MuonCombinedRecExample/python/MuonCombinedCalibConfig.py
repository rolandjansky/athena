# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Logging import logging
from AthenaCommon import BeamFlags
beamFlags = jobproperties.Beam

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonCnvExample.MuonCalibFlags import muonCalibFlags
from RecExConfig.RecFlags import rec

# configure calib algs for standalone
if muonRecFlags.doCalibNtuple():
    from MuonRecExample import MuonAlignConfig
    from MuonCnvExample import setupMuonCalibNtuple
    setupMuonCalibNtuple()
elif muonRecFlags.doCalib():
    from MuonRecExample import MuonAlignConfig
    from MuonCnvExample import setupMuonCalib
    setupMuonCalib()
else:
    logMuon.warning("Loading %s but not setting up any MuonCalibration or Ntuple" % __name__ )
# NB. call setDefaults *after* import MuonCalibConfig
muonCalibFlags.setDefaults()

log = logging.getLogger("MuonCombinedRec")

topSequence = AlgSequence()

#
# Move MuonCalibExtraTree to end of topSequence and configure for combined
#s

from PyUtils.MetaReaderPeeker import metadata, convert_itemList
def checkContainerInInputFile(container_type, container_name):
    if container_type in convert_itemList(layout='dict'):
        if container_name in convert_itemList(layout='dict')[container_type]:
            return True
    return False


def StacoMuonCollectionAvailable():
	return False

def MuidMuonCollectionAvailable():
	return False

def MuonsMuonCollectionAvailable():
        if rec.readESD() and checkContainerInInputFile("Analysis::MuonContainer", "Muons"):
           return True
        if muonCombinedRecFlags.doxAOD() and muonCombinedRecFlags.doCombinedFit():
           return True
        if  hasattr(topSequence, 'BuildMuonCollection'):
           return True
        return False


try:
    MuonCalibExtraTreeAlg = topSequence.MuonCalibExtraTreeAlg
    del topSequence.MuonCalibExtraTreeAlg
except AttributeError:
    log.debug('No MuonCalibExtraTreeAlg in topSequence - not touching it')
else:
    log.info('Adding MuonCombined tracks to MuonCalibExtraTreeAlg')
    # configure alg for combined reco

    from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__ExtraTreeMuonFillerTool
    if MuidMuonCollectionAvailable():
        muidFillerTool=MuonCalib__ExtraTreeMuonFillerTool("MuidFillerTool")
        muidFillerTool.MuonContainer="MuidMuonCollection"
        muidFillerTool.AuthorOffset=0
        muidFillerTool.SegmentAuthors=[4]
        ToolSvc+=muidFillerTool
        MuonCalibExtraTreeAlg.TrackFillerTools.append(muidFillerTool)
    if 	StacoMuonCollectionAvailable():
        stacoFillerTool=MuonCalib__ExtraTreeMuonFillerTool("StacoMuonFillerTool")
        stacoFillerTool.MuonContainer="StacoMuonCollection"
        stacoFillerTool.AuthorOffset=100
        stacoFillerTool.SegmentAuthors=[3]
        ToolSvc+=stacoFillerTool
        MuonCalibExtraTreeAlg.TrackFillerTools.append(stacoFillerTool)
    # and put back algorithm at the end, after the other extra conversion algs
    topSequence += MuonCalibExtraTreeAlg
    if MuonsMuonCollectionAvailable():
        muonsFillerTool=MuonCalib__ExtraTreeMuonFillerTool("MuonsMuonFillerTool")
        muonsFillerTool.MuonContainer="Muons"
        muonsFillerTool.AuthorOffset=200
        muonsFillerTool.SegmentAuthors=[5]
        ToolSvc+=muonsFillerTool
        MuonCalibExtraTreeAlg.TrackFillerTools.append(muonsFillerTool)


#
# move MuonCalibExtraTreeTriggerAlg to the end of topSequence
#
try:
    MuonCalibExtraTreeTriggerAlg = topSequence.MuonCalibExtraTreeTriggerAlg
    del topSequence.MuonCalibExtraTreeTriggerAlg
except AttributeError:
    log.debug('No MuonCalibExtraTreeTriggerAlg in topSequence - not touching it')
else:
    topSequence += MuonCalibExtraTreeTriggerAlg


# cleanup
del log
