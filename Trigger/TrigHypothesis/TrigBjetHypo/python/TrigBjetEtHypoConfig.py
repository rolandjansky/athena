# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypo

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV

def getBjetEtHypoInstance( instance, version, cut ):
    cutValue = int(cut.replace("GeV",""))
    return BjetEtHypo( instance=instance, version=version, cut=cutValue, name=instance+"BjetEtHypo_"+version+"_"+cut )

def getBjetEtHypoNoCutInstance( instance, version ):
    return BjetEtHypoNoCut( instance=instance, version=version, name=instance+"BjetEtHypoNoCut_"+version )


class BjetEtHypo (TrigBjetEtHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetEtHypo, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        AllowedInstances = ["EF", "MuJetChain", "GSC"]
        AllowedVersions  = ["StartSequence","Btagging"]

        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None

        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        self.AcceptAll = False        
        self.Instance  = instance
        self.Version   = version
        self.EtThreshold = cut*GeV
            
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"

        if instance=="GSC" :
            self.JetKey = "GSCJet"

class BjetEtHypoNoCut (TrigBjetEtHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetEtHypoNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        AllowedInstances = ["EF", "MuJet"]
        AllowedVersions  = ["Btagging"]

        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None

        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.AcceptAll = True
        self.Instance  = instance
        self.Version   = version

        if instance=="MuJet" :
            self.JetKey = "FarawayJet"



