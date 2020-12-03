# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__all__ = []

from PyJobTransformsCore.TransformConfig import TransformConfig, Boolean, String, ListOfStrings

class RecConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # add properties
    restrictedESD = Boolean("write out restricted ESD information", False)
    doMinimalRec = Boolean("only perform minimal reconstruction", False)
    doBackTracking = Boolean("backtracking on", True)
    doIndetReco   = ListOfStrings("List of inner detector reconstruction algorithms to run, for example: all,newTracking,iPatRec,xKalman." +
                                  " Each name in the list gets translated into a python flag: do<name>=True" ,
                                  ['newTracking'] )
    usePrimVertexZcoordinate  = Boolean("Set the usePrimVertexZcoordinate for the IDET configuration",False)
    enableTrackSlimming  = Boolean("Control Track slimming",True)
    detectorsOff  = ListOfStrings("List of detectors to switch off",[],['ID','Muon','Calo','Truth','LVL1'])
    combinedRecoOff  = ListOfStrings("List of combined reconstruction algs to switch off",[],['doObjMissingET'])
    postOptionsESD = String("JobOptions to include at the end to customize making of ESD's",None)
    postOptionsAOD = String("JobOptions to include at the end to customize making of AOD's",None)

    def __init__(self,name='recConfig',metaData=None):
        if metaData is None: metaData = ['restrictedESD','doIndetReco','detectorsOff','combinedRecoOff','postOptionsESD','postOptionsAOD','doMinimalRec','doBackTracking','usePrimVertexZcoordinate','enableTrackSlimming']
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'INPUTEVENTS'


# make configuration object
recConfig = RecConfig()
