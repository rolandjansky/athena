#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Produce calibration fits and validate them in 4 steps.
Take the calibration ntuples as input files and produce fitted data."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from MuonCalibJobTransforms.mdt_calib_fit_trf import MDTCalibFitJobTransform
from MuonCalibJobTransforms.mdt_calib_val_trf import MDTCalibValJobTransform


class MDTCalibFitValJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Alessandro De Salvo','alessandro.desalvo@roma1.infn.it'),
                                          Author('Fabrizio Petrucci','fabrizio.petrucci@roma3.infn.it') ] ,
                              help = __doc__ )

        #add arguments
        self.add( CalibNtupleFileListArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( SelectChamberArg(), default='[BML,4, 4]' )
        self.add( JobConfigArg(package='MuonCalibJobTransforms'), default='NONE' )

    def runJob(self):
        # get the list of arguments to be used in sub-transforms
        argDict = self.argumentOriginalDict()
        # Fit T0 phase
        t0fit = MDTCalibFitJobTransform()
        t0fit.setJobReportOptions('None')
        argDict['calibrationAlgorithm'] = argDict['ClassicT0']
        report = t0fit.exeArgDict( argDict )
        # only run t0 validation if the T0 making was successfull
        if (report.exitCode() == 0):
            # print out short report on first stage
            t0fit.dumpReport('Summary,Errors')
            # T0 validation phase
            t0val = MDTCalibValJobTransform()
            t0val.setJobReportOptions('Summary')
            argDict['calibrationAlgorithm'] = argDict['T0Val']
            reportT0val = t0val.exeArgDict( argDict )
            report.addReport( reportT0val )
        # Fit RT phase
        rtfit = MDTCalibFitJobTransform()
        rtfit.setJobReportOptions('None')
        argDict['calibrationAlgorithm'] = argDict['ClassicAutocal']
        reportRT = rtfit.exeArgDict( argDict )
        report.addReport( reportRT )
        # only run rt validation if the rt making was successfull
        if (reportRT.exitCode() == 0):
            # print out short report on first stage
            rtfit.dumpReport('Summary,Errors')
            # rt validation phase
            rtval = MDTCalibValJobTransform()
            rtval.setJobReportOptions('Summary')
            argDict['calibrationAlgorithm'] = argDict['RtVal']
            reportRTval = rtval.exeArgDict( argDict )
            report.addReport( reportRTval )

        return report


# execute it if not imported
if __name__ == '__main__':
    trf = MDTCalibFitJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())
