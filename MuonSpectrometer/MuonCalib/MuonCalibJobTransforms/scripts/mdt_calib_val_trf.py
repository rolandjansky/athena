#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Validate calibration fits.
Reads the calibration database and validaes the constants therein"""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from MuonCalibJobTransforms.MDTCalibValConfig import mdtCalibValConfig
from MuonCalibJobTransforms.MDTCalibValConfig import ValAlgorithmArg
from MuonCalibJobTransforms.MDTCalibValConfig import SelectChamberArg
from MuonCalibJobTransforms.MDTCalibValConfig import CalibSiteArg
from MuonCalibJobTransforms.MDTCalibValConfig import HeaderIdArg

class MDTCalibValJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Alessandro De Salvo','alessandro.desalvo@roma1.infn.it'),
                                          Author('Fabrizio Petrucci','fabrizio.petrucci@roma3.infn.it') ] ,
                              skeleton='MuonCalibJobTransforms/skeleton.mdt_calib_validate.py',
                              help = __doc__,
                              config=mdtCalibValConfig )

        #add arguments
        self.add( SelectChamberArg())
        self.add( MaxEventsArg() )
        self.add( GeometryVersionArg() )
        self.add( ValAlgorithmArg(['T0Val','RtVal']), default='T0Val' )
        self.add( CalibSiteArg(), default='RM' )
        self.add( JobConfigArg(package='MuonCalibJobTransforms'), default='NONE' )
        self.add( DBReleaseArg(), default='NONE' )
        self.add( HeaderIdArg(), default='-1' )

        #add other features
        self.add( SQLiteSupport() )

# execute it if not imported
if __name__ == '__main__':
    trf = MDTCalibValJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())
