# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS forward detectors description
"""

__author__  = 'A. Dell`Acqua'

import PyG4Atlas, AtlasG4Eng
from PyG4Atlas import DetConfigurator
from SimFlags import simFlags


class ForwardRegion(object):
    """Describes the whole forward region (outside the ATLAS boundaries)"""

    def __init__(self,doDetFac=True):
        """Describes the geometry."""
        if doDetFac:
            self.atlas_ForwardRegion = PyG4Atlas.DetFacility("GeoDetector", "ForDetEnvelope:ForDetEnvelope")

    # Custom magnetic field
    def add_field( self , step_limitation = None ):

        fieldVolumes = []
        fieldVolumes += [ ['Q1', ['FwdRegion::LQXAA.1R1MagQ1'] ] ]
        fieldVolumes += [ ['Q2', ['FwdRegion::LQXBA.2R1MagQ2a', 'FwdRegion::LQXBA.2R1MagQ2b'] ] ]
        fieldVolumes += [ ['Q3', ['FwdRegion::LQXAG.3R1MagQ3'] ] ]
        fieldVolumes += [ ['D1', [ 'FwdRegion::MBXW.A4R1MagD1a', 'FwdRegion::MBXW.B4R1MagD1b', 'FwdRegion::MBXW.C4R1MagD1c',\
                                   'FwdRegion::MBXW.D4R1MagD1d', 'FwdRegion::MBXW.E4R1MagD1e', 'FwdRegion::MBXW.F4R1MagD1f'] ] ]
        fieldVolumes += [ ['D2', ['FwdRegion::LBRCD.4R1MagD2'] ] ]
        fieldVolumes += [ ['Q4', ['FwdRegion::LQYCH.4R1MagQ4'] ] ]
        fieldVolumes += [ ['Q5', ['FwdRegion::LQNDC.5R1MagQ5'] ] ]
        fieldVolumes += [ ['Q6', ['FwdRegion::LQNDD.6R1MagQ6'] ] ]
        fieldVolumes += [ ['Q7', ['FwdRegion::LQNFD.7R1MagQ7a', 'FwdRegion::LQNFD.7R1MagQ7b'] ] ]
        fieldVolumes += [ ['Q1HKick', ['FwdRegion::LQXAA.1R1MagQ1HKick'] ] ]
        fieldVolumes += [ ['Q1VKick', ['FwdRegion::LQXAA.1R1MagQ1VKick'] ] ]
        fieldVolumes += [ ['Q2HKick', ['FwdRegion::LQXBA.2R1MagQ2HKick'] ] ]
        fieldVolumes += [ ['Q2VKick', ['FwdRegion::LQXBA.2R1MagQ2VKick'] ] ]
        fieldVolumes += [ ['Q3HKick', ['FwdRegion::LQXAG.3R1MagQ3HKick'] ] ]
        fieldVolumes += [ ['Q3VKick', ['FwdRegion::LQXAG.3R1MagQ3VKick'] ] ]
        fieldVolumes += [ ['Q4VKickA', ['FwdRegion::LQYCH.4R1MagQ4VKickA'] ] ]
        fieldVolumes += [ ['Q4HKick', ['FwdRegion::LQYCH.4R1MagQ4HKick'] ] ]
        fieldVolumes += [ ['Q4VKickB', ['FwdRegion::LQYCH.4R1MagQ4VKickB'] ] ]
        fieldVolumes += [ ['Q5HKick', ['FwdRegion::LQNDC.5R1MagQ5HKick'] ] ]
        fieldVolumes += [ ['Q6VKick', ['FwdRegion::LQNDD.6R1MagQ6VKick'] ] ]

        epsMin = 1e-9
        epsMax = 1e-8
        deltaIntersection = 1e-9
        deltaOneStep = 1e-8

        for i in fieldVolumes:
            a_field = PyG4Atlas.MagneticField('ForwardRegionMgField',i[0],typefield='MapField')
            for v in i[1]:
                a_field.add_Volume(v)
                a_field.set_G4FieldTrackParameters('DeltaIntersection',  v, deltaIntersection)
                a_field.set_G4FieldTrackParameters('DeltaOneStep',       v, deltaOneStep)
                a_field.set_G4FieldTrackParameters('MaximumEpsilonStep', v, epsMax)
                a_field.set_G4FieldTrackParameters('MinimumEpsilonStep', v, epsMin)

                # limit step length
                if step_limitation is not None:
                    AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetMaxStep(v, step_limitation)

            a_field.set_FieldMapFileName(i[0])
            AtlasG4Eng.G4Eng.menu_Field.add_Field(a_field)

        if step_limitation is not None:
            AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetMaxStep('FwdRegion::ForwardRegionGeoModel', step_limitation)

    def setupTwissFiles(self):
        # Settings of optics to be used
        import os
        twiss_path = os.getenv('TwissFilesPATH')
        twiss_beam1 = None
        twiss_beam2 = None
        twiss_momentum = -1.

        if simFlags.TwissFileBeam1.statusOn:
            if os.access(simFlags.TwissFileBeam1(),os.R_OK): twiss_beam1 = simFlags.TwissFileBeam1()
            elif os.access(twiss_path+simFlags.TwissFileBeam1(),os.R_OK): twiss_beam1 = twiss_path+simFlags.TwissFileBeam1()
        if simFlags.TwissFileBeam2.statusOn:
            if os.access(simFlags.TwissFileBeam2(),os.R_OK): twiss_beam2 = simFlags.TwissFileBeam2()
            elif os.access(twiss_path+simFlags.TwissFileBeam2(),os.R_OK): twiss_beam2 = twiss_path+simFlags.TwissFileBeam2()
        if twiss_beam1 is None or twiss_beam2 is None:
            twiss_energy = '7.0TeV'
            if simFlags.TwissEnergy.statusOn:
                twiss_energy = twiss_energy = '%1.1fTeV'%(simFlags.TwissEnergy()*0.000001)
            else:
                from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
                if athenaCommonFlags.PoolEvgenInput.statusOn and len(athenaCommonFlags.PoolEvgenInput.get_Value())>0:
                    inputfile = athenaCommonFlags.PoolEvgenInput.get_Value()[0]
                    import PyUtils.AthFile as af
                    try:
                        f = af.fopen(inputfile)
                        if 'beam_energy' in f.infos and 'N/A' not in f.infos['beam_energy']:
                            if type(f.infos['beam_energy']) is list and float(f.infos['beam_energy'][0])>100.:
                                twiss_energy = '%1.1fTeV'%(float(f.infos['beam_energy'])*0.000001)
                            elif float(f.infos['beam_energy'])>100.:
                                twiss_energy = '%1.1fTeV'%(float(f.infos['beam_energy'])*0.000001)
                    except AssertionError:
                        AtlasG4Eng.G4Eng.log.error("Failed to open input file: %s", inputfile)
                        pass
            twiss_beta = '%07.2fm'%(0.001*simFlags.TwissFileBeta())
            if not (simFlags.TwissFileNomReal.statusOn and simFlags.TwissFileVersion.statusOn):
                AtlasG4Eng.G4Eng.log.error('Need to either provide file names or set file name and file version flags')
                raise Exception('Not enough information to locate Twiss files')
            twiss_nomreal = simFlags.TwissFileNomReal()
            twiss_version = simFlags.TwissFileVersion()

            # Getting paths to the twiss files, momentum calculation; you can switch to local files
            import re,math
            twiss_beam1 = os.path.join(twiss_path, twiss_energy, twiss_beta, twiss_nomreal, twiss_version, 'beam1.tfs')
            twiss_beam2 = os.path.join(twiss_path, twiss_energy, twiss_beta, twiss_nomreal, twiss_version, 'beam2.tfs')
            twiss_momentum =  math.sqrt(float(re.findall("\d+.\d+", twiss_energy)[0])**2 - (0.938e-3)**2)*1e3
        else:
            # Have to sort out twiss momentum based on file name
            tmp = twiss_beam1.split('TeV')[0]
            tmp_spot = len(tmp)
            if simFlags.TwissEnergy.statusOn:
                twiss_energy = '%1.1fTeV'%(simFlags.TwissEnergy()*0.000001)
            else:
                while True:
                    try:
                        tmp_energy = float( tmp[tmp_spot:] )
                        tmp_spot -= 1
                    except ValueError:
                        twiss_energy = float( tmp[tmp_spot+1:] )
                        break
                    pass
            import re,math
            twiss_momentum =  math.sqrt(float(re.findall("\d+.\d+", twiss_energy)[0])**2 - (0.938e-3)**2)*1e3

        # properties of the field set according to the optics settings above
        from ForwardRegionProperties.ForwardRegionPropertiesConf import ForwardRegionProperties
        fwdProperties = ForwardRegionProperties()
        fwdProperties.twissFileB1 = twiss_beam1
        fwdProperties.twissFileB2 = twiss_beam2
        fwdProperties.momentum = twiss_momentum
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += fwdProperties


class ZDC(object):
    """Describes the ATLAS ZDC detector."""

    def __init__(self):
        """Describes the geometry."""
        AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: ZDC atlas_ZDC.py :: __init__  about to make DetFacility")
        self.atlas_ZDC = PyG4Atlas.DetFacility("GeoDetector", "ZDC:ZDC")
        AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: ZDC atlas_ZDC.py :: __init__  DetFacility Done")

    def _initSD(self):
        """Describes the sensitive detector."""
        AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: atlas_ZDC.py :: _initSD  about to make SenDetector")
        self.atlas_ZDC.add_SenDetector('ZDC_SD', 'ZDCPixelSD', 'ZDCPixelSD', 'ZDC::Pixel_Logical')
        self.atlas_ZDC.add_SenDetector('ZDC_SD', 'ZDCStripSD', 'ZDCStripSD', 'ZDC::Strip_Logical')
        AtlasG4Eng.G4Eng.log.debug("ZDC Simulation :: atlas_ZDC.py :: _initSD  SenDetector Done")

    def _initPR(self):
        """Describes the physics regions."""



class ALFA(object):
     """Describes the ATLAS ALFA detector."""

     def __init__(self):
         """Describes the geometry."""
         self.alfa = PyG4Atlas.DetFacility("GeoDetector", "ALFA:ALFA")

     def _initSD(self):
         """Describes the sensitive detector."""
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::logALFA_FiberU')
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::logALFA_FiberV')
         for i_alfa_fiber in xrange(64):
             self.alfa.add_SenDetector('ALFA_G4_SD', 'ALFA_SensitiveDetector',
                                       'ALFA_SensitiveDetector', 'ALFA::logALFA_FiberU[%d]' % i_alfa_fiber)
             self.alfa.add_SenDetector('ALFA_G4_SD', 'ALFA_SensitiveDetector',
                                       'ALFA_SensitiveDetector', 'ALFA::logALFA_FiberV[%d]' % i_alfa_fiber)
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector', 'ALFA::logOD_FiberActive')
         #print 'fiber %d' % i_alfa_fiber

         # ALFA global vacuum sensors
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::B7L1_GVS')
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::A7L1_GVS')
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::A7R1_GVS')
         self.alfa.add_SenDetector('ALFA_G4_SD','ALFA_SensitiveDetector', 'ALFA_SensitiveDetector','ALFA::B7R1_GVS')


class Lucid(object):
     """Describes Lucid."""

     def __init__(self):
         """Describes the geometry."""
         self.lucid = PyG4Atlas.DetFacility("GeoDetector", "LUCID:LUCID")

     def _initSD(self):
         """Describes the sensitive detector."""
         self.lucid.add_SenDetector('LUCID_G4_SD', 'LUCID_SensitiveDetector', 'LUCID_SensitiveDetector', 'LUCID::lvPmt')

     def _initOpProcess(self):
         """Activates the optical process."""
         AtlasG4Eng.G4Eng.load_Lib("LUCID_OpProcess")


class AFP(object):
     """Describes AFP."""

     def __init__(self):
         """Describes the geometry."""
         self.AFP = PyG4Atlas.DetFacility("GeoDetector", "AFP:AFP_GeoModel")

     def _initSD(self):
         """Describes the SI sensitive detector."""
         self.AFP.add_SenDetector('AFP_G4_SD','AFP_SensitiveDetector', 'AFP_SensitiveDetector','AFP::AFP*_LogSIDSensor*')
         self.AFP.add_SenDetector('AFP_G4_SD','AFP_SensitiveDetector', 'AFP_SensitiveDetector','AFP::AFP*_LogSIDVacuumSensor*')

         """Describes the T sensitive detector."""
         self.AFP.add_SenDetector('AFP_G4_SD','AFP_SensitiveDetector', 'AFP_SensitiveDetector','AFP::AFP*_LogTDSensor*')
         #self.AFP.add_SenDetector('AFP_G4_SD','AFP_SensitiveDetector', 'AFP_SensitiveDetector','AFP::AFP*_LogRadiator*')
         #self.AFP.add_SenDetector('AFP_G4_SD','AFP_SensitiveDetector', 'AFP_SensitiveDetector','AFP::AFP*_LogVertBar*')
         #self.AFP.add_SenDetector('AFP_G4_SD','AFP_SensitiveDetector', 'AFP_SensitiveDetector','AFP::AFP*_LogHorzBar*')

     def _initOpProcess(self):
         """Activates the optical process."""
         AtlasG4Eng.G4Eng.load_Lib("LUCID_OpProcess")


class FwdRegion(object):
     """Describes beam pipe in the Forward Region"""

     def __init__(self):
         """Describes the geometry."""
         self.atlas_FwdRegion = PyG4Atlas.DetFacility("GeoDetector", "FwdRegion:ForwardRegionGeoModel")

     def _initPR(self):
         """Describes the physics regions."""
         self.atlas_FwdRegion_PhysReg=PyG4Atlas.PhysicsReg('FwdRegion')
         self.atlas_FwdRegion_PhysReg.add_Volumes('FwdRegion::ForwardRegionGeoModel')
         self.atlas_FwdRegion_PhysReg.add_Cuts('e-',200.)
         self.atlas_FwdRegion_PhysReg.add_Cuts('e+',200.)
         self.atlas_FwdRegion_PhysReg.add_Cuts('gamma',200.)
         self.atlas_FwdRegion.add_PhysicsReg(self.atlas_FwdRegion_PhysReg)
