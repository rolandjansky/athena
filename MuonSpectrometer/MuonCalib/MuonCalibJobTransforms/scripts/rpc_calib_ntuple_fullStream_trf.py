#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Produce calibration ntuples
Take the calibration bytestream as input file and produce ntuples as output files."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from MuonCalibJobTransforms.RPCCalibNtupleConfig import rpcCalibNtupleConfig,FieldMaskArg,ConditionsTagArg, resNtupleArg, recoAlgArg, trigNtupleArg,PRDNtupleArg,extrNtupleArg

class RPCCalibNtupleJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Andrea Di Simone','andrea.di.simone@cern.ch')] ,
                              skeleton='MuonCalibJobTransforms/skeleton.rpc_calib_ntuple_fullStream.py',
                              help = __doc__,
                              config=rpcCalibNtupleConfig )

        #add arguments
        self.add( InputBSFileArg() )
        self.add( NtupleFileArg() )
        self.add( RunNumberArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( GeometryVersionArg() )
        self.add( FieldMaskArg(), default="111")
        self.add( ConditionsTagArg(),default="COMCOND-REPC-001-03")
        self.add( DBReleaseArg(), default='NONE' )
        self.add( resNtupleArg(), default=True)
        self.add( trigNtupleArg(), default=False)
        self.add( PRDNtupleArg(), default=False)
        self.add( extrNtupleArg(), default=False)
        self.add( recoAlgArg(['Moore','MuonBoy']), default='MuonBoy' )
        self.add( JobConfigArg(package='MuonCalibJobTransforms'), default='NONE' )

# execute it if not imported
if __name__ == '__main__':
    trf = RPCCalibNtupleJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())
