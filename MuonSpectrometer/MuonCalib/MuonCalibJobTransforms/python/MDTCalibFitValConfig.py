# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

# define configuration properties
class MDTCalibFitValConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # list of attributes
    CalibrationSite        = String("Calibration Site", "NONE")
    PostProcessCalibration = Boolean("Set to true if you want to insert the calibration constants to the database", True)
    NoRecalculation        = Boolean("If set to true no drift ties and drift radii will be recalculated. Use this for validation", False)
    SelectStation          = String("Select Station", "BIL")
    SelectEta              = Integer("Select Eta", 3)
    SelectPhi              = Integer("Select Phi", 3)
    CalibInputLocation     = String("Input Location", None)
    outputLocation         = String("Output Location", None)
    postOptionsFitT0       = String("jobOptions fragment to be appended at the end of the T0 fit", None)
    postOptionsValT0       = String("jobOptions fragment to be appended at the end of the T0 validation", None)
    postOptionsFitRT       = String("jobOptions fragment to be appended at the end of the RT fit", None)
    postOptionsValRT       = String("jobOptions fragment to be appended at the end of the RT validation", None)

    def __init__(self,name='mdtCalibFitValConfig',metaData=None):
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'ABORT'


# make configuration object
mdtCalibFitValConfig = MDTCalibFitValConfig()
