# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
This module adds the magnetic fields to the CTB simulation
"""

__author__  = "M. Gallas"


import os, os.path
import PyG4Atlas, AtlasG4Eng


# CTB constant magnetic fields
class CTBConstantField(object):
    """ Constant fields for the CTB

        The available fields_names are:
         + MBPSID --> only Bz component
         + MBPL   --> only By component
         + MBPS2  --> only By component
         + MBPL12 --> only Bz component
         + MBPL13 --> only By component

        the field component value is set with the SimFlags. The
        name at the FADS level will be:
           'ConstantField'+field_name
        All the fields attached to the simu entity can be found in:
            AtlasG4Eng.G4Eng.Dict_Fields.keys()
    """
    def __init__(self, field_name):
        self.ValidMagNames = ['MBPSID','MBPL','MBPS2','MBPL12','MBPL13']
        self.field = PyG4Atlas.MagneticField('G4Field', 'ConstantField'+field_name, volume=field_name+'::'+field_name)


class CTBFieldMap(object):
    """ Magnetic Field map

        Defines the magnetic field map in the IDET volume, and it afects
      only to the MBPSID magnet. The MBPS2 and MBPL magnets and the
      magnets in use during the photon runs are simulated with a
      constant field.

      * Possible 'fieldmap_name' values (see ctb_flags:MagFieldMap):

       + mbps1-all-id-800-mbps2-muons-800.data
      (only this file is in CVS)

      other available from ../project/magfield/CTB
     (at CERN site /afs/cern.ch/atlas/project/magfield/CTB)

      + mbps1-200-source.data
      + mbps1-500-source.data
      + mbps1-300-source.data
      + mbps1-850-source.data
      + minus-mbps1-850-source.data
      + minus-mbps1-500-source.data
      + minus-mbps1-300-source.data
      + minus-mbps1-200-source.data
      + minus-mbps1-100-source.data
      + mbps1-100-source.data

      this are linked from path above or from a local copy in the run
      directory

      * Field calculation:
               mode = 0 -> standard way
               mode = 1 -> symmetric calculation
    """
    def __init__(self,fieldmap_name,mode):
        dataPathList = os.environ['DATAPATH'].split(os.pathsep)
        for i in dataPathList:
            if 'CTB' in i and 'magfield' in i:
                self.mag_data_path = i
                AtlasG4Eng.G4Eng.log.debug('ctb_field::CTBFieldMap found the datapath for field maps at %s', self.mag_data_path)
        ## Restrict the field to the IDET volume
        if mode == 0:
            mapfield_CTB = PyG4Atlas.MagneticField('G4Field', 'G4AtlasFieldSvc', typefield='MapField')
        elif mode == 1:
            AtlasG4Eng.G4Eng.log.debug('ctb_field::CTBFieldMap uses symetric B field calculation')
            mapfield_CTB = PyG4Atlas.MagneticField('G4Field', 'G4AtlasFieldSvc', typefield='MapField')
        ## Use the track parameters requested by G. Gorfine
        mapfield_CTB.set_G4FieldTrackParameters('DeltaIntersection', 'IDET::IDET', 0.00001)
        mapfield_CTB.set_G4FieldTrackParameters('DeltaOneStep', 'IDET::IDET', 0.0001)
        mapfield_CTB.set_G4FieldTrackParameters('MaximumEpsilonStep', 'IDET::IDET', 0.001)
        mapfield_CTB.set_G4FieldTrackParameters('MinimumEpsilonStep', 'IDET::IDET', 0.00001)

        if fieldmap_name == 'mbps1-all-id-800-mbps2-muons-800.data':
            mapfield_CTB.set_FieldMapFileName(fieldmap_name)
        else:
            AtlasG4Eng.G4Eng.log.debug('ctb_field::CTBFieldMap links magnetic field map from DATAPATH')
            if os.path.exists('magempty'):
                os.unlink('magempty')
            try:
                os.symlink(os.path.join(self.mag_data_path,fieldmap_name),
                           os.path.join(os.getcwd(),'magempty'))
            except Exception:
                AtlasG4Eng.G4Eng.log.debug('ctb_field::CTBFieldMap DATAPATH not found --> try local link')
                os.symlink(os.path.join(os.getcwd(),fieldmap_name),
                           os.path.join(os.getcwd(),'magempty'))
            mapfield_CTB.set_FieldMapFileName('magempty')

        AtlasG4Eng.G4Eng.menu_Field.add_Field(mapfield_CTB)
