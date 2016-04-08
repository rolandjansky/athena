# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty

class CosmicFilterVolumeName(JobProperty):
    """
    Declare the volume used to do cosmics filtering.

    Values are G4 volume names, e.g. TRT_Barrel, TRT_EC, SCT_Barrel, Pixel, InnerDetector, Calo, CaloEntryLayer, Muon, ...
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    StoredValue = 'CaloEntryLayer'


class CosmicFilterVolumeName2(JobProperty):
    """
    Declare a second volume used to do cosmics filtering.

    Values are G4 volume names, e.g. TRT_Barrel, TRT_EC, SCT_Barrel, Pixel, InnerDetector, Calo, CaloEntryLayer, Muon, ...
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    StoredValue = 'NONE'


class CosmicPtSlice(JobProperty):
    """
    Name of the pT slice used for cosmics simulation.
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    allowedValues = [ 'slice1', 'slice2', 'slice3', 'slice4', 'NONE' ]
    StoredValue = 'NONE'


class CosmicFilterID(JobProperty):
    """
    Particle ID to be filtered in cosmics processing.
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    StoredValue = '13'


class CosmicFilterPTmin(JobProperty):
    """
    Particle minimum pT to be filtered in cosmics processing (in MeV).
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    StoredValue = "5000"


class CosmicFilterPTmax(JobProperty):
    """
    Particle maximum pT to be filtered in cosmics processing (in MeV).
    """
    statusOn = False
    allowedTypes = [ 'str' ]
    StoredValue = "6000"


class PixelEC_cosmic_SCsetup(JobProperty):
    """
    Pixel EndCap C cosmic scintillator setup.
      1: Test setup with four scintillators: top, bottom, lower right and left
      2: Test setup with two scintillators: top and bottom
    """
    statusOn = False
    allowedTypes = ['int']
    allowedValues = [1,2]
    StoredValue = 1


