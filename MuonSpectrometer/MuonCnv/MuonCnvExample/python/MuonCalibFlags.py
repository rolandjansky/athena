# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing module %s",__name__)

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

from MuonCnvUtils import setJobPropertyDefault
logMuon = logging.getLogger(__name__)

def setDefault(prop,value):
    return setJobPropertyDefault(prop,value,logMuon)

################################################################################
# MDT calibration flags
################################################################################
class MdtCalib(JobPropertyContainer):
    def setDefaults(self):
        global globalflags,beamFlags
        if globalflags.DataSource == 'data' and beamFlags.beamType == 'collisions':
            setDefault(self.correctMdtRtForBField,True)
        else:
            setDefault(self.correctMdtRtForBField,False)
        if globalflags.DataSource == 'data':
            setDefault(self.correctMdtRtForTimeSlewing,True)
            setDefault(self.mdtPropagationSpeedBeta, 0.85)
        else:
            setDefault(self.correctMdtRtForTimeSlewing,False)
            setDefault(self.mdtPropagationSpeedBeta, 1.0)

jobproperties.add_Container(MdtCalib)

mdtCalibFlags = jobproperties.MdtCalib

##read mdt tube calibration from blob-folders
class readMDTCalibFromBlob(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

mdtCalibFlags.add_JobProperty(readMDTCalibFromBlob)

# @brief Apply B-field correction to drift times
class correctMdtRtForBField(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

mdtCalibFlags.add_JobProperty(correctMdtRtForBField)

# @brief Apply time slewing correction to drift time
class correctMdtRtForTimeSlewing(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

mdtCalibFlags.add_JobProperty(correctMdtRtForTimeSlewing)

# @brief use ML-RT functions from COOL
class useMLRt(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

mdtCalibFlags.add_JobProperty(useMLRt)

# @brief Apply rt scaling correction (multilayer t_max diff)
# Never used, replaced by using ML-RTs
class applyRtScaling(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

mdtCalibFlags.add_JobProperty(applyRtScaling)

# @brief Apply RT wiresag correction
class correctMdtRtWireSag(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

mdtCalibFlags.add_JobProperty(correctMdtRtWireSag)

## @brief Source for MDT t0s and rts
class mdtCalibrationSource(JobProperty):
    statusOn=True
    allowedTypes=['str']
    StoredValue="MDT"

mdtCalibFlags.add_JobProperty(mdtCalibrationSource)

class mdtPropagationSpeedBeta(JobProperty):
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.85

mdtCalibFlags.add_JobProperty(mdtPropagationSpeedBeta)

# now that all flags are added, set their default values
mdtCalibFlags.setDefaults()

################################################################################
# CSC calibration flags
################################################################################
class CscCalib(JobPropertyContainer):
    def setDefaults(self):
        pass
        # nothing for the moment


jobproperties.add_Container(CscCalib)

cscCalibFlags = jobproperties.CscCalib


class CscPedFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscPedFromLocalFile)


class CscNoiseFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscNoiseFromLocalFile)


class CscPSlopeFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscPSlopeFromLocalFile)


class CscStatusFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscStatusFromLocalFile)


class CscRmsFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscRmsFromLocalFile)


class CscF001FromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscF001FromLocalFile)


class CscT0BaseFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscT0BaseFromLocalFile)


class CscT0PhaseFromLocalFile(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

cscCalibFlags.add_JobProperty(CscT0PhaseFromLocalFile)


# now that all flags are added, set their default values
cscCalibFlags.setDefaults()


class EventTag(JobProperty):
    """Tag that chooses input to use for calibration"""
    statusOn=False
    allowedTypes=['str']
    StoredValue='MoMu'


class Mode(JobProperty):
    """Choose the (MDT) calibration mode.
    'ntuple'       : write standard calibration ntuple (up to segments)
    'trackNtuple'  : write standard ntuple + tracks
    'regionNtuple' : write one ntuple per calibration region
    't0Classic'    : do classic t0 calibration
    't0MT'         : do MT t0 calibration
    'rtClassic'    : do classic rt calibration
    'rtAnalytic'   : do analytic rt calibration"""
    statusOn=False
    allowedTypes=['str']
    allowedValues=[ 'ntuple', 'trackNtuple', 'regionNtuple']
    StoredValue='ntuple'


class MuonCalib(JobPropertyContainer):
    """Flags to configure the Muon Calibration algorithms"""
    def setDefaults(self):
        """For backwards compatibility: Use old flags if values are not yet set"""
        if self._locked:
            # properties can not be set if locked
            return
        import __main__
        mainvars = vars(__main__)
        # The source
        if not self.EventTag.statusOn:
            self.EventTag.set_On()
        # The destination
        if not self.Mode.statusOn:
            # order is important: last one wins
            oldFlags = [ 'doCalibFullNtuple' ,
                         'doTrackNtuple' ,
                         'doCalibRegionNtuple']

            oldtonew = {
                'doCalibFullNtuple'   : 'ntuple'       ,
                'doCalibRegionNtuple' : 'regionNtuple' ,
                'doTrackNtuple'       : 'trackNtuple' }
    
            for oldflag in oldFlags:
                newflag = oldtonew[oldflag]
                try:
                    oldValue = mainvars[oldflag]
                except KeyError:
                    pass
                else:
                    if oldValue:
                        self.Mode = newflag
                        # print a deprecated warning
                        log.warning( '%s flag is deprecated. Use muonCalibFlags.Mode = %r instead (%s)',
                                     oldflag, newflag,
                                     'from ' + __name__ + ' import muonCalibFlags' )

            # if still not set, set to default
            if not self.Mode.statusOn:
                self.Mode.set_On()


# initialise container
jobproperties.add_Container(MuonCalib)
muonCalibFlags = jobproperties.MuonCalib
for f in [ EventTag, Mode ]:
    muonCalibFlags.add_JobProperty(f)
del f

