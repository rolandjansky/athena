#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Produce calibration ntuples for fitting.
Take the calibration bytestream as input file and produce ntuples as output files."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from MuonCalibJobTransforms.MDTCalibNtupleConfig import mdtCalibNtupleConfig
from MuonCalibJobTransforms.MDTCalibNtupleConfig import DbT0TagArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import DbRTTagArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import DbT0SqlArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import DbRTSqlArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import UseAlignArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import SegOnlyArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import MooreArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import MboyArg
from MuonCalibJobTransforms.MDTCalibNtupleConfig import StandaloneArg
from MuonCalibJobTransforms.MDTCalibNtupleConfig import CalibNtupleDoSimArg 
from MuonCalibJobTransforms.MDTCalibNtupleConfig import DoSegmentT0FitArg
from MuonCalibJobTransforms.MDTCalibNtupleConfig import DoCSCArg
from MuonCalibJobTransforms.MDTCalibNtupleConfig import RawTgcArg
from MuonCalibJobTransforms.MDTCalibNtupleConfig import LumiBlockNumberFromCool
from MuonCalibJobTransforms.MDTCalibNtupleConfig import RunNumberFromCool
from MuonCalibJobTransforms.MDTCalibNtupleConfig import CosmicRun

class MDTCalibNtupleJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Alessandro De Salvo','alessandro.desalvo@roma1.infn.it'),
                                          Author('Fabrizio Petrucci','fabrizio.petrucci@roma3.infn.it') ] ,
                              skeleton='MuonCalibJobTransforms/skeleton.mdt_calib_ntuple.py',
                              help = __doc__,
                              config=mdtCalibNtupleConfig )

        #add arguments
        self.add( InputBSFileArg() )
        self.add( NtupleFileArg() )
        self.add( RunNumberArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( GeometryVersionArg() )
        self.add( ConditionsTagArg() )
        self.add( DbT0TagArg(), default='DEFAULT'  )
        self.add( DbRTTagArg(), default='DEFAULT'  )
        self.add( DbT0SqlArg(), default='DEFAULT'  )
        self.add( DbRTSqlArg(), default='DEFAULT'  )
        self.add( UseAlignArg(), default='True' )
        self.add( SegOnlyArg(), default='True' )
        self.add( MooreArg(), default='True' )
        self.add( MboyArg(), default='False' )
        self.add( StandaloneArg(), default='False' )
        self.add( DoSegmentT0FitArg(), default='False' )
        
        self.add( JobConfigArg(package='MuonCalibJobTransforms'), default='NONE' )
        self.add( CalibNtupleDoSimArg(), default='False' )
        self.add( DBReleaseArg(), default='NONE' )

        self.add( DoCSCArg(), default='False' )
        self.add( RawTgcArg(), default='False' )

        self.add( LumiBlockNumberFromCool(), default='False' )
        self.add( RunNumberFromCool(), default='False' )

        #add other features
        self.add( SQLiteSupport() )
        self.add( CosmicRun(), default='False')

# execute it if not imported
if __name__ == '__main__':
    trf = MDTCalibNtupleJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())
