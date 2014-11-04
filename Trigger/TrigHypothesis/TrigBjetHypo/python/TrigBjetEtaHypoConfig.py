# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtaHypo

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV


def getBjetEtaHypoInstance( instance, cut ):
    return BjetEtaHypo( instance=instance, cut=cut, name=instance+"BjetEtaHypo" )


class BjetEtaHypo (TrigBjetEtaHypo):
    __slots__ = []
    
    def __init__(self, instance, cut, name):
        super( BjetEtaHypo, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        AllowedInstances = ["L2","EF"]
 
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        self.AcceptAll = False        
        self.Instance  = instance
        self.EtaMax = cut
 



