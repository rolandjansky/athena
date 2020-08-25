# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 

# default configuration of TrigInDetAccelerationSvc

# import the base class

from TrigInDetAccelerationService.TrigInDetAccelerationServiceConf import TrigInDetAccelerationSvc


class TrigInDetAccelerationSvcDefault( TrigInDetAccelerationSvc )  :

    def __init__(self, name="TrigInDetAccelerationSvcDefault"):
        # call base class constructor
        TrigInDetAccelerationSvc.__init__( self, name="TrigInDetAccelerationSvc")
        self.NumberOfDCs = 8



