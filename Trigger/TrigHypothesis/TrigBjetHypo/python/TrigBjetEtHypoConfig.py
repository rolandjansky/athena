# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypo

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV


thresholdsStartSequence = {
    '0GeV'   : 0,
    '15GeV'  : 15,
    '35GeV'  : 35,
}

thresholdsBtagging = {
    '0GeV'   : 0,
    '10GeV'  : 10,
    '15GeV'  : 15,
    '25GeV'  : 25,
    '35GeV'  : 35,
    '45GeV'  : 45,
    '50GeV'  : 50,
    '55GeV'  : 55,
    '60GeV'  : 60,    
    '65GeV'  : 65,    
    '70GeV'  : 70,
    '75GeV'  : 75,        
    '80GeV'  : 80,
    '110GeV' : 110,
    '145GeV' : 145,
    '150GeV' : 150,
    '165GeV' : 165,    
    '175GeV' : 175,
    '180GeV' : 180,
    '220GeV' : 220,
    '225GeV' : 225,
    '240GeV' : 240,    
    '280GeV' : 280,
    '300GeV' : 300,
    '360GeV' : 360,
}


def getBjetEtHypoInstance( instance, version, cut ):
    return BjetEtHypo( instance=instance, version=version, cut=cut, name=instance+"BjetEtHypo_"+version+"_"+cut )

def getBjetEtHypoNoCutInstance( instance, version ):
    return BjetEtHypoNoCut( instance=instance, version=version, name=instance+"BjetEtHypoNoCut_"+version )


class BjetEtHypo (TrigBjetEtHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetEtHypo, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        AllowedInstances = ["EF"]
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

        if version=="StartSequence" :
            self.EtThreshold = thresholdsStartSequence[cut]*GeV
        elif version=="Btagging" :
            self.EtThreshold = thresholdsBtagging[cut]*GeV



class BjetEtHypoNoCut (TrigBjetEtHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetEtHypoNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        AllowedInstances = ["EF"]
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



