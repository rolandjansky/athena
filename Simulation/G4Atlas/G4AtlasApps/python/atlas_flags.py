# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
SimFlags specific to the full ATLAS simulation

Apart from the AthenaCommon.DetFlags and AthenaCommon.GlobalFlags
the simulation jobs will need specific flags that are collected
here as "SimFlags".
"""

__author__ = 'M. Gallas, A. Buckley'


from AthenaCommon.JobProperties import JobProperty


class EventFilter(JobProperty):
    """
    Sets the status of the event filters.

    The available filters for the full ATLAS simulation are:
     - EtaPhiFilters: (Filters G4PrimaryParticle objects outside of configurable Eta
        and Phi ranges. Not suitable for use with ALFA and ZDC.)
     - VertexRangeChecker: (Filters G4PrimaryVertex objects outside of
        configurable R and Z ranges.)
    """
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = { 'EtaPhiFilters': True,
                    'VertexRangeChecker': True }

    def switchFilterOff(self, filter):
       """Switch the specified filter off"""
       if filter in self.get_Value().keys():
           self.get_Value()[filter] = False
       else:
           from AthenaCommon.Logging import logging
           _sflog = logging.getLogger('SimFlags')
           _sflog.warning ( "Tried to switch off non-existent EventFilter: " + filter )

    def switchFilterOn(self, filter):
       """Switch the specified filter on"""
       if filter in self.get_Value().keys():
           self.get_Value()[filter] = True
       else:
           from AthenaCommon.Logging import logging
           _sflog = logging.getLogger('SimFlags')
           _sflog.warning ( "Tried to switch on non-existent EventFilter: " + filter )

    def switchAllFiltersOff(self):
       """Switch the specified filter off"""
       for filter in self.get_Value().keys():
           self.get_Value()[filter] = False

    def switchAllFiltersOn(self):
       """Switch the specified filter on"""
       for filter in self.get_Value().keys():
           self.get_Value()[filter] = True


class LArParameterization(JobProperty):
    """
    Switch for the shower parameterization

    Possible values are:
      0 - no parametrization
      1 - conservative default
      2 - recomended default
      3 - the fastest

    By default is set to 0.
    """
    statusOn = True
    allowedTypes = ['int']
    allowedValues = [0,1,2,3]
    StoredValue = 0


class LArHighVoltage(JobProperty):
    """
    Switch for using new LAr High Voltage functionality.

    List of detectors for which the new HV should be used, e.g.
      EMB+EMEC+FCAL+HEC
      EMB+EMEC
    etc.
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'EMB+EMEC+FCAL+HEC'


class MagneticField(JobProperty):
    """
    Magnetic field switch.

     - OracleDB: will read the field map name, shifts & Tilts from Oracle associated to a given GeoModelTag.
     - COOL: use the conditions tag to select the field setup.

    If you use the field map name it will not be access to OracleDB, and the
    shifts & tilts are set to 0.

    If you want to switch off the field, use 'SimFlags.MagneticField.set_Off()'
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['AtlasFieldSvc',
                     'bmagatlas_08_full45Sym20400.data.bz2',
                     'bmagatlas_08_fullAsym20400.data.bz2',
                     'bmagatlas03_test2.data',
                     'bmagatlas04_test1.data',
                     'bmagatlas04_test1_WithNoToroid_OnlySolAndIron.data',
                     'CustomField']
    StoredValue = 'AtlasFieldSvc'


class TightMuonStepping(JobProperty):
    """
    Switch to enable tight muon stepping parameters
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False


class MuonFieldOnlyInCalo(JobProperty):
    """
    Switch so that only muons see the B-field in the calo
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False


class ForwardDetectors(JobProperty):
    """
    Decide whether simulation must be run for forward detectors.

    If enabled, ATLAS envelope is prolonged to +-350m along the beam line.

      ForwardDetectors = 0 --> no tracking
      ForwardDetectors = 1 --> Geant4 tracking
      ForwardDetectors = 2 --> Custom tracking
    """
    statusOn = False
    allowedTypes = ['int']
    allowedValues = [0, 1, 2]
    StoredValue = 1


class WorldRRange(JobProperty):
    """
    R dimension of the ATLAS envelope
    """
    statusOn = False
    allowerTypes = ['int','float']
    StoredValue = 12500.

class WorldZRange(JobProperty):
    """
    Z dimension of the ATLAS envelope
    """
    statusOn = False
    allowedTypes = ['int','float']
    StoredValue = 22031.

class VertexFromCondDB(JobProperty):
    """
    Generate the vertex position by sampling based on the conditions database
    parameters.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True


class VertexTimeOffset(JobProperty):
    """ Turns on time offset (for beam halo events)
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False


class CavernBG(JobProperty):
    """
    Flag to turn on the simulation of cavern background (and all that goes with it)
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    StoredValue = 'Signal'
    allowedValues = [ 'Write' , 'Read' , 'Signal' , 'WriteWorld' , 'SignalWorld' ]


class CavernCuts(JobProperty):
    """
    Use specialized cuts in G4 for cavern background

    If true switches SimFlags.EventFilter off and sets
    SimFlags.WorldZRange.set_Value(24000) else switch SimFlags.EventFilter on
    (SimFlags.EventFilter off by default)
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False


class BeamPipeSimMode(JobProperty):
    """
    How the beam pipe is to be simulated, specifically referring to the
    treatments of the forward elements F47, F48, and F61 which are shielding
    blocks beyond the sensitive detectors that take up to 40% of sim CPU if not
    handled specially.

    * Normal - standard, CPU-wasteful setup.
    * FastSim - use the SimpleFastKiller in the beampipe cut regions
    * EGammaRangeCut - set a special range cut for e+, e-, and gamma in the forward cut regions
    * EGammaPRangeCut - as for EGammaRangeCut, but also setting the range cut for protons

    The range cut applied in the last two scenarios is given by the BeamPipeCut sim flag.
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['Normal', 'FastSim', 'EGammaRangeCuts', 'EGammaPRangeCuts']
    StoredValue = 'Normal'


class BeamPipeCut(JobProperty):
    """
    Special range cut, in mm, to be used for simulation in the forward beam pipe
    regions if BeamPipeSimMode is set appropriately.
    """
    statusOn = False
    allowedTypes = ['float']
    StoredValue = 100.0


class ReadTR(JobProperty):
    """
    Reads the TrackRecords.

    If set, switches on the reading of track records from the specified
    file. Useful for cosmics and cavern background simulation.
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'trackrecord.evgen.pool.root'


class WriteTR(JobProperty):
    """
    Writes the TrackRecords.

    If set, switches on the writing of track records on a different stream.
    Useful for cosmics and cavern background simulation.
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'trackrecord.evgen.pool.root'


class StoppedParticleFile(JobProperty):
    """
    Enable a fast simulation which kills anything other than SUSY particles
    meant to stop in the calorimeter, watches those SUSY particles, and
    writes them out when they drop below the magical energy threshold.

    The value assigned is the name of the file to be written out.
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'stoppedPositions.evgen.pool.root'

class IncludeParentsInG4Event(JobProperty):
    """
    Include the parents of generated particles in the G4 event if G4 knows about them.
    They will be assigned the daughters and lifetime that the generator gave them.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class KillAbortedEvents(JobProperty):
    """
    If an event is aborted by Geant4 then it should not be written out
    to the HITS file.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class FlagAbortedEvents(JobProperty):
    """
    If an event is aborted by Geant4 then it should still be written
    out to the HITS file, but with the Error flag set in
    EventInfo. When this flag is True, KillAbortedEvents should be
    False for it to have any effect.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class VertexOverrideFile(JobProperty):
    """
    File name (path) for the file that will perform the vertex override in overlay sim
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = 'VertexOverride.txt'

class VertexOverrideEventFile(JobProperty):
    """
    File name (path) for the file that will provide events for the vertex override
    in overlay simulation
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValues = 'VertexOverrideEventFile.txt'

class SimulateNewSmallWheel(JobProperty):
    """
    Will this job simulate the New Small Wheel in the Muon
    Spectrometer?
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False

class BeamEffectOptions(JobProperty):
    """
    Set options for beam effects transport
    """
    statusOn = False
    allowedTypes = ['dict']
    StoredValue = {}

class FwdStepLimitation(JobProperty):
    """
    Forward step limitation option
    """
    statusOn = False
    allowedTypes = ['float']
    StoredValue = 1000.

class TwissFileBeam1(JobProperty):
    """
    Twiss file full path for beam 1
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = None

class TwissFileBeam2(JobProperty):
    """
    Twiss file full path for beam 2
    """
    statusOn = False
    allowedTypes = ['str']
    StoredValue = None

class TwissEnergy(JobProperty):
    """
    Twiss file center of mass energy, in ATLAS units.
    Use 'None' for autoconfiguration based on input file.
    """
    statusOn = False
    allowedTypes = ['float']
    StoredValue = 8000000.

class TwissFileBeta(JobProperty):
    """
    Twiss file beta, in ATLAS units
    """
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 550.

class TwissFileNomReal(JobProperty):
    """
    Twiss file: nominal or real optics?
    """
    statusOn = False
    allowedTypes = ['str']
    allowedValues = ['nominal','real']
    StoredValue = None

class TwissFileVersion(JobProperty):
    """
    Twiss file version string
    """
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'v01'

class TRTRangeCut(JobProperty):
    """
    TRT Range cut in mm
    """
    statusOn = True
    allowedTypes = ['float']
    allowedValues = [0.05,30.0]
    StoredValue = 0.05

class IsEventOverlayInputSim(JobProperty):
    """
    Is will the output of this job be used as the input for an event
    overlay job?
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

