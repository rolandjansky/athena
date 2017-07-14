# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Main configuration module for the ATLAS test-beams

CTB(2004): ATLAS Combined Test Beam
Tile standalone test beam 2000-2003
LAr  standalone test beams at H6
"""

__author__ = 'M. Gallas'


import math
from AthenaCommon               import PhysicalConstants
from AthenaCommon.AppMgr        import theApp
from AthenaCommon.Include       import include
from AthenaCommon.GlobalFlags   import globalflags
from AthenaCommon.DetFlags      import DetFlags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.BeamFlags     import jobproperties
import PyG4Atlas, AtlasG4Eng
from SimSkeleton import SimSkeleton




class TBSimSkeleton(SimSkeleton):
    """
    Defines a common SimSkeleton for the test beams.
    """

    @classmethod
    def _do_metadata(self):
        """
        Setup and add metadata to the HIT file
        """
        import AtlasG4Eng
        AtlasG4Eng.G4Eng.log.verbose('TBSimSkeleton :: _do_metadata :: starting')
        from G4AtlasApps.G4Atlas_Metadata import createTBSimulationParametersMetadata
        createTBSimulationParametersMetadata()

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if (athenaCommonFlags.PoolHitsOutput.statusOn) :
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : Recording Metadata')
            from AthenaServices.AthenaServicesConf import AthenaOutputStream
            stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
            stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]
            AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_metadata done')
        else :
            AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_metadata nothing done')



#--- CTB 2004 ----------------------------------------------------------
class CtbSim(TBSimSkeleton):
    """ Defines the simulation entity for the
        ATLAS Combined Test Beam (2004)
    """
    def __init__(self):
        if not(AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton')):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton']=self
            AtlasG4Eng.G4Eng.Name="CTB_G4Sim"         # assigns a name
        else:
            AtlasG4Eng.G4Eng.log.warning(' SimSkeleton: the simulation '+\
            'has already a skeleton, you can find it in the '+\
            'G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(self):
        """ Place to handle JobProperties .
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties starting')

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.extra_flags:
            simFlags.load_ctb_flags()

        # - switch off tasks
        DetFlags.pileup.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.digitize.all_setOff()
        DetFlags.overlay.all_setOff()
        DetFlags.readRDOPool.all_setOff()
        DetFlags.makeRIO.all_setOff()
        DetFlags.writeBS.all_setOff()
        DetFlags.readRDOBS.all_setOff()
        DetFlags.readRIOBS.all_setOff()
        DetFlags.readRIOPool.all_setOff()
        DetFlags.writeRIOPool.all_setOff()
        DetFlags.writeRDOPool.all_setOff()

        # - see if there are beam-conditions
        if (simFlags.BeamConditions.statusOn and
            simFlags.BeamConditions.get_Value()):
                try:
                    AtlasG4Eng.G4Eng.log.info(' SimCtbKernel: '+\
                           ' loading CTB beam-conditions from the'+\
                           ' CTB run-condition file !!')
                    BeamCond=__import__(modulepath,globals(),locals(),prepath).BeamCond
                    beam_cond_obj=BeamCond.retrieve_Run(simFlags.RunNumber.get_Value())
                    beam_cond_obj._map()
                except:
                    AtlasG4Eng.G4Eng.log.warning(' SimCtbKernel: '+
                    'No particular beam conditions found for the run '+\
                    str(simFlags.RunNumber.get_Value()))
        # - switch off non-existing detectors
        DetFlags.FCal_setOff()
        DetFlags.HEC_setOff()
        # - Muons off for 13.0.0 release
        DetFlags.Muon_setOff()
        AtlasG4Eng.G4Eng.log.info('SimCtbKernel : Muons are not supported in releases >= 13.0.0')
        # -  photon runs under run-conditions. For all the photon runs with
        # run number > 2860 SCT and Pixel were not at the beam
        # -- LAr material studies
        if (simFlags.SimLayout.get_Value()=='ctbh8_lar-material'):
            DetFlags.SCT_setOff()
            DetFlags.Muon_setOff()
        if (simFlags.SimLayout.get_Value()=='ctbh8_calodata'):
            DetFlags.ID_setOff()
            DetFlags.Muon_setOff()
        #
        # Note: for combined    runs tag 04 in IDET
        #           photon      runs tag 08
        #           larmaterial runs tag 01
        if simFlags.SimLayout.get_Value() == 'ctbh8_combined' and not simFlags.IdetOracleTag.statusOn:
            simFlags.IdetOracleTag = "InnerDetector-CTB-04-01"
        elif simFlags.SimLayout.get_Value() == 'ctbh8_phothon'and not simFlags.IdetOracleTag.statusOn:
            simFlags.IdetOracleTag = "InnerDetector-CTB-08-01"
        elif simFlags.SimLayout.get_Value() == 'ctbh8_lar-material' and not simFlags.IdetOracleTag.statusOn:
            simFlags.IdetOracleTag = "InnerDetector-CTB-01-01"
        else:
            simFlags.IdetOracleTag.set_On()
        #
        globalflags.DataSource = "geant4"
        globalflags.DetGeo = "ctbh8"
        ## mgallas this is a hack! needed for GeoModel and IOVDbSvc
        # TODO: Wow! Is this really still needed?
        import __main__
        __main__.DetDescrVersion = 'CTB'
        __main__.globalflags = globalflags
        globalflags.DetDescrVersion = 'CTB'

        ## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
        from AthenaCommon.AppMgr import ServiceMgr
        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
        ServiceMgr += IOVDbSvc()
        if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
            msg = "AtlasSimSkeleton._do_jobproperties :: conditions tag has not been set and is no longer guessed by SimAtlasKernel. "
            msg += "You must set the globaltags.ConditionsTag flag in your job options."
            AtlasG4Eng.G4Eng.log.error(msg)
            raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
        if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
            ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

        ## Print out flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')

        jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties done')


    @classmethod
    def _do_external(self):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'starting')
        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        Geo2G4Svc=Geo2G4Svc()
        theApp.CreateSvc += [ "Geo2G4Svc" ]
        ServiceMgr +=Geo2G4Svc
        # We do not use top transforms for the CTB geometry
        Geo2G4Svc.GetTopTransform = False

        #--- GeoModel stuff ----------------------------------------------------
        from AtlasGeoModel import SetGeometryVersion

        from AthenaCommon.Configurable import Configurable
        if Configurable.allConfigurables.get('GeoModelSvc'):
            GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
        else:
            GeoModelSvc=theApp.service('GeoModelSvc')
        GeoModelSvc.AlignCallbacks =False

        from G4AtlasApps.SimFlags import simFlags
        GeoModelSvc.InDetVersionOverride =simFlags.IdetOracleTag.get_Value()

        if(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B-00'):
            GeoModelSvc.TileVersionOverride='TileTB-3B-00'
        elif(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B3EB-00'):
            GeoModelSvc.TileVersionOverride='TileTB-3B3EB-00'

        from AtlasGeoModel import GeoModelInit
        if(DetFlags.Calo_on()):
            # Common for the Calo
            include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
            include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
            if(DetFlags.em_on()):
                include("LArDetDescr/LArDetDescr_H8_joboptions.py")
        ##--- Persistency of test-beam layout ----------------------------------
        if jobproperties.AthenaCommonFlags.PoolHitsOutput.statusOn :
            include( "TBDetDescrAthenaPool/TBDetDescrAthenaPool_joboptions.py" )
            from TBDetDescrAlg.TBDetDescrAlgConf import TBDetDescrLoader
            from AthenaCommon.AlgSequence import AlgSequence
            job=AlgSequence()
            TBDetDescrLoader=TBDetDescrLoader()
            job+=TBDetDescrLoader
            TBDetDescrLoader.TBElementContainer = "TBElementCnt"
            TBDetDescrLoader.TBDetDescrManager  = "TBDetDescrMgr"
            include( "TBDetDescrCnv/TBDetDescrCnv_jobOptions.py" )
            # do not read anything from StoreGate
            TBDetDescrLoader.ReadAction = 0
            # and write TBDetDescrContainer to StoreGate every event
            TBDetDescrLoader.WriteAction = 2
            TBDetDescrLoader.OutputLevel = 5
        #
        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'done')

    @classmethod
    def do_GeoSD(self):
        """ Defines the geometry and SD
        """
        # - Get a handle to the RecEnv menu
        MenuRecordEnvelopes=AtlasG4Eng.G4Eng.menu_RecordEnvelope()
        # - Recording envelopes definitions
        from ctb_common import CTBRecordingEnvelopes

        # - CTB  common volumes -
        from ctb_common import ctb_top_volumes
        from ctb_common import ctb_beampipes_and_magnetsupstream
        from ctb_common import ctb_dump_and_muonmagnets
        from ctb_common import ctb, idet, muon, mbpsid

        ctb_top_volumes()

        # recording envelope added to IDET volume
        ## rc=CTBRecordingEnvelopes.CtbCaloEntryLayer()
        ## MenuRecordEnvelopes.add_RecEnvelope(rc.recenv)

        from G4AtlasApps.SimFlags import simFlags
        if (simFlags.SimLayout.get_Value()=='ctbh8_photon'):
            ctb_beampipes_and_magnetsupstream(1)
        elif simFlags.BeamConditions.get_Value():
            ctb_beampipes_and_magnetsupstream(2)
        else:
            ctb_beampipes_and_magnetsupstream()


        if not(DetFlags.Muon_on()):
            ctb_dump_and_muonmagnets(0,1)

        if (simFlags.LArFarUpstreamMaterial.statusOn and
            simFlags.LArFarUpstreamMaterial.get_Value()):
            from ctb_common import LArFarUpstreamMaterial
            if not(simFlags.BeamConditions.statusOn and
                simFlags.BeamConditions.get_Value()):
               larupmaterial=LArFarUpstreamMaterial(0)
               ## rc1=CTBRecordingEnvelopes.LArFarUpstreamMaterialExitLayer(0)
            else:
               larupmaterial=LArFarUpstreamMaterial(1)
               ## rc1=CTBRecordingEnvelopes.LArFarUpstreamMaterialExitLayer(1)
            ## MenuRecordEnvelopes.add_RecEnvelope(rc1.recenv)

        # -- Inner detector
        if(DetFlags.ID_on()):
            if(DetFlags.geometry.pixel_on()):
                from ctb_idet import Pixel
                pixel=Pixel()
                pixel._initSD()
                if(simFlags.IdetOracleTag.get_Value()=="InnerDetector-CTB-01-01" or
                     simFlags.SimLayout.get_Value()=='ctbh8_lar-material'):
                    pixel.ctb_pixel.df.MoveTo(pixel.ctb_pixel_position1)
                    AtlasG4Eng.G4Eng.add_DetFacility(pixel.ctb_pixel,ctb)
                else:
                    pixel.ctb_pixel.df.MoveTo(pixel.ctb_pixel_position2)
                    AtlasG4Eng.G4Eng.add_DetFacility(pixel.ctb_pixel,mbpsid)
            if(DetFlags.geometry.SCT_on()):
                from ctb_idet import SCT
                sct=SCT()
                sct._initSD()
                AtlasG4Eng.G4Eng.add_DetFacility(sct.ctb_sct,mbpsid)
            if(DetFlags.geometry.TRT_on()):
                from ctb_common import ctbtrt
                if(simFlags.IdetOracleTag.get_Value()=="InnerDetector-CTB-03-01" or
                   simFlags.IdetOracleTag.get_Value()=="InnerDetector-CTB-09-01"):
                   from ctb_common import ctbtrt_position2
                   ctbtrt.df.MoveTo(ctbtrt_position2)
                elif(simFlags.IdetOracleTag.get_Value()=="InnerDetector-CTB-01-01" or
                     simFlags.SimLayout.get_Value()=='ctbh8_lar-material'):
                   from ctb_common import ctbtrt_position3
                   ctbtrt.df.MoveTo(ctbtrt_position3)
                else:
                   from ctb_common import ctbtrt_position1
                   ctbtrt.df.MoveTo(ctbtrt_position1)
                AtlasG4Eng.G4Eng.add_DetFacility(ctbtrt,idet)
                from ctb_idet import TRT
                trt=TRT()
                trt._initSD()
                AtlasG4Eng.G4Eng.add_DetFacility(trt.ctb_trt,ctbtrt)
                # TRT process
                trt._initTRTProcess()

        # -- Calorimeters
        if(DetFlags.Calo_on()):
            # eta rotation and translation
            eta=simFlags.Eta.get_Value()
            ThetaY=-(PhysicalConstants.pi*0.5)+2*math.atan(math.exp(-eta))
            DeltaX=float(2298-6208)/math.cosh(eta)+6208
            ctb_calo_rotation=math.degrees(ThetaY)
            offsetY=0.0
            offsetZ=0.0
            if simFlags.CaloOffsetY.statusOn:
               offsetY=simFlags.CaloOffsetY()
               AtlasG4Eng.G4Eng.log.info('SimSkeleton: CALO envelope offsetY '\
                                                +str(offsetY))
            if simFlags.CaloOffsetZ.statusOn:
               offsetZ=simFlags.CaloOffsetZ()
               AtlasG4Eng.G4Eng.log.info('SimSkeleton: CALO envelope offsetZ '\
                                                +str(offsetZ))
            if (eta>=0 and eta<1.201):
                from ctb_common import calo
                if simFlags.SimLayout.get_Value()=='ctbh8_lar-material':
                    # here we resize the calo envelope
                    if (simFlags.LArMaterialAluThickness.get_Value()>0 and \
                        simFlags.LArMaterialAluThickness.get_Value()<101):
                        CaloInnerRadius=950-50-\
                               simFlags.LArMaterialAluThickness.get_Value()-5
                        calo.df.SetRinSurface(1,CaloInnerRadius)
                        calo.df.SetRinSurface(2,CaloInnerRadius)
                if (simFlags.LArMuonChamber.statusOn and
                    not(simFlags.SimLayout.get_Value()=='ctbh8_lar-material')):
                    # here we resize the calo envelope
                    CaloInnerRadius=950-120
                    calo.df.SetRinSurface(1,CaloInnerRadius)
                    calo.df.SetRinSurface(2,CaloInnerRadius)
                AtlasG4Eng.G4Eng.add_DetFacility(calo,ctb)
                calo_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(DeltaX,offsetY,offsetZ)
                calo.df.RotateY(ThetaY)
                calo.df.MoveTo(calo_pos)
                AtlasG4Eng.G4Eng.log.info('SimSkeleton: CALO envelope is rotated by ThetaY '+str(ThetaY))
                AtlasG4Eng.G4Eng.log.info('SimSkeleton: CALO envelope center is at (x,y,z) : '\
                                              +str(DeltaX)+','+str(offsetY)+','+str(offsetZ))
                if(DetFlags.em_on()):
                    from atlas_calo import PyLArG4RunControler
                    LArG4RunControl=PyLArG4RunControler('PyLArG4RunControl',\
                                                        'LArG4RunControlDict')
                    if (simFlags.LArEMBenergyCor.get_Value()):
                        LArG4RunControl.LArG4BarrelOptions.EMBCurr(True)
                    else:
                        LArG4RunControl.LArG4BarrelOptions.EMBCurr(False)
                    if (AtlasG4Eng.G4Eng.log.level <= AtlasG4Eng.G4Eng.log.info) : LArG4RunControl.LArG4BarrelOptions.printMe()
                    from  ctb_calo import LAr
                    LArDetector=LAr()
                    if (simFlags.CalibrationRun.statusOn and
                        (simFlags.CalibrationRun.get_Value()=='LAr' or
                         simFlags.CalibrationRun.get_Value()=='LAr+Tile')):
                            LArDetector._initSD(1)
                    else:
                        AtlasG4Eng.G4Eng.log.info('before InitSD(0)')
                        LArDetector._initSD(0)
                        AtlasG4Eng.G4Eng.log.info('after InitSD(0)')
                    LArDetector._initPR()
                    AtlasG4Eng.G4Eng.add_DetFacility(LArDetector.ctb_lar,calo)
                    if simFlags.SimLayout.get_Value()=='ctbh8_lar-material':
                        if (simFlags.LArMaterialAluThickness.get_Value()>0 and \
                            simFlags.LArMaterialAluThickness.get_Value()<101 ):
                            from ctb_common import larmaterial
                            larmaterial_position= \
                             AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(CaloInnerRadius+\
                              simFlags.LArMaterialAluThickness.get_Value()*0.5+5,0.0,\
                              -(CaloInnerRadius+ simFlags.LArMaterialAluThickness.get_Value()*0.5+\
                               5)*math.tan(ThetaY))
                            larmaterial.df.SetDx(simFlags.LArMaterialAluThickness.get_Value()*0.5)
                            larmaterial.df.MoveTo(larmaterial_position)
                            AtlasG4Eng.G4Eng.add_DetFacility(larmaterial,calo)
                    if (simFlags.LArMuonChamber.statusOn and
                        not(simFlags.SimLayout.get_Value()=='ctbh8_lar-material')):
                        from ctb_common import EquivalentLArMuonChamber
                        LArMuC= EquivalentLArMuonChamber()
                        LArMuC.MuonChamber.position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(CaloInnerRadius+\
                                          5.,0.,437.5)
                        LArMuC.MuonChamber.df.MoveTo(LArMuC.MuonChamber.position)
                        AtlasG4Eng.G4Eng.add_DetFacility(LArMuC.MuonChamber,calo)

                if(DetFlags.Tile_on()):
                   from  ctb_calo import TileDet
                   if (simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B-00'):
                       tiledet=TileDet('CTB-3B')
                   elif(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B3EB-00'):
                       tiledet=TileDet('CTB-3B3E')
                   tilesd=tiledet.SD()
                   AtlasG4Eng.G4Eng.add_DetFacility(tiledet.ctb_tile,calo)
            elif(eta<0 or eta>1.201):
                raise ValueError,('THE ETA VALUE MUST BE IN  RANGE [0,1.2]!!!'+
                        'The selected value %s is not in the range.' %eta)

        # -- Muon system
        if(DetFlags.geometry.Muon_on()):
            from ctb_muon import ctb_muon
            AtlasG4Eng.G4Eng.add_DetFacility(ctb_muon,muon)
            ctb_dump_and_muonmagnets(1,1)
            from ctb_muon import muonsel
            AtlasG4Eng.G4Eng.add_DetFacility(muonsel,ctb_muon)

        # -- Ancillary detectors
        # upstream ancillary:
        if(simFlags.AncillaryUpstream.statusOn):
            from ctb_common import AncillaryUpstream
            if(simFlags.SimLayout.get_Value()=='ctbh8_photon'):
                AncillaryUpstream('PeriodC')
                simFlags.AncillaryUpstream='PeriodC'
            elif(simFlags.SimLayout.get_Value()=='ctbh8_lar-material' or \
                 simFlags.SimLayout.get_Value()=='ctbh8_calodata'):
                AncillaryUpstream('PeriodA')
                simFlags.AncillaryUpstream='PeriodA'
        # upstream ancillary (ctb-combined mode):
        if(simFlags.AncillaryUpstream.statusOn  and  \
            not(simFlags.BeamConditions.statusOn and \
                 simFlags.BeamConditions.get_Value())):
            from ctb_common import AncillaryUpstream
            AncillaryUpstream(simFlags.AncillaryUpstream.get_Value())
        # adding hit recording to the S1 scintillator
        if(AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('S1')):
            from atlas_utilities import ScintillatorSD
            GenScintSD=ScintillatorSD(['S1'])
        # calo ancillary
        if(simFlags.AncillaryLArTile.statusOn):
            eta=simFlags.Eta.get_Value()
            if (eta>=0 and eta<1.201):
                if(DetFlags.Tile_on()):
                    from ctb_common import LArTileAncillary
                    if(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B-00'):
                        LArTileAncillary(eta,'CTB-3B')
                    elif(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B3EB-00'):
                        LArTileAncillary(eta,'CTB-3B3E')
            elif(eta<0 or eta>1.201):
                raise ValueError,('ETA VALUE MUST BE IN [0,1.2]!!!'+
                   'The selected value %s is not in the range.' %eta)
        # muon ancillary
        if(simFlags.AncillaryMuon.statusOn):
            from ctb_common import MuonAncillary
            if(DetFlags.geometry.Muon_on()):
                MuonAncillary(1)
            else:
                MuonAncillary(0)

        # -- Lead foil
        if(simFlags.LeadUpstreamAbs.statusOn):
            from ctb_common import leadfoil

    ## @classmethod
    ## def do_MagField(self):
    ##     """ Defines the magnetic field configurations.
    ##
    ##         (constant and magnetic field map supported)
    ##     """
    ##     # - Magnetic fields
    ##     #   - Magnetic Field Map only in the IDET volume
    ##     from G4AtlasApps.SimFlags import simFlags
    ##     if simFlags.MagFieldMap.statusOn:
    ##         if(AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('MBPSID') and
    ##            AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('IDET')):
    ##             from ctb_field import CTBFieldMap
    ##             ctbfieldmap=CTBFieldMap(simFlags.MagFieldMap.get_Value(),\
    ##             simFlags.MagFieldMapCalculation.get_Value())
    ##         else:
    ##            AtlasG4Eng.G4Eng.log.warning('CtbSim: magnetic fields: \
    ##                                               No MBPSID or IDET volumes')
    ##     #   - Constant Fields
    ##     if(simFlags.MagnetMBPSIDBz.statusOn and
    ##        not(simFlags.MagFieldMap.statusOn)) :
    ##         if AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('MBPSID'):
    ##             from ctb_field import CTBConstantField
    ##             B_MBPSID=CTBConstantField('MBPSID')
    ##             B_MBPSID.field.Bz=simFlags.MagnetMBPSIDBz.get_Value()
    ##             AtlasG4Eng.G4Eng.menu_Field.add_Field(B_MBPSID.field)
    ##         else:
    ##             AtlasG4Eng.G4Eng.log.warning('CtbSim: magnetic fields: \
    ##                                                 No MBPSID volume')
    ##     if simFlags.MagnetMBPLBy.statusOn:
    ##         if AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('MBPL'):
    ##             from ctb_field import CTBConstantField
    ##             B_MBPL=CTBConstantField('MBPL')
    ##             B_MBPL.field.By=simFlags.MagnetMBPLBy.get_Value()
    ##             AtlasG4Eng.G4Eng.menu_Field.add_Field(B_MBPL.field)
    ##         else:
    ##             AtlasG4Eng.G4Eng.log.warning('CtbSim: magnetic fields: \
    ##                                         No MBPL volume')
    ##     if simFlags.MagnetMBPS2By.statusOn:
    ##         if AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('MBPS2'):
    ##             from ctb_field import CTBConstantField
    ##             B_MBPS2=CTBConstantField('MBPS2')
    ##             B_MBPS2.field.By=simFlags.MagnetMBPS2By.get_Value()
    ##             AtlasG4Eng.G4Eng.menu_Field.add_Field(B_MBPS2.field)
    ##         else:
    ##             AtlasG4Eng.G4Eng.log.warning('CtbSim: magnetic fields: \
    ##                                         No MBPS2 volume')
    ##     if simFlags.MagnetMBPL12Bz.statusOn:
    ##         if AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('MBPL12'):
    ##             from ctb_field import CTBConstantField
    ##             B_MBPL12=CTBConstantField('MBPL12')
    ##             B_MBPL12.field.Bz=simFlags.MagnetMBPL12Bz.get_Value()
    ##             AtlasG4Eng.G4Eng.menu_Field.add_Field(B_MBPL12.field)
    ##         else:
    ##             AtlasG4Eng.G4Eng.log.warning('CtbSim: magnetic fields: \
    ##                                         No MBPL12 volume')
    ##     if simFlags.MagnetMBPL13By.statusOn:
    ##         if AtlasG4Eng.G4Eng.Dict_DetFacility.has_key('MBPL13'):
    ##             from ctb_field import CTBConstantField
    ##             B_MBPL13=CTBConstantField('MBPL13')
    ##             B_MBPL13.field.By=simFlags.MagnetMBPL13By.get_Value()
    ##             AtlasG4Eng.G4Eng.menu_Field.add_Field(B_MBPL13.field)
    ##         else:
    ##             AtlasG4Eng.G4Eng.log.warning('CtbSim: magnetic fields: \
    ##                                         No MBPL13 volume')


#--- Tile TB 2000-2003  ------------------------------------------------
class Tile2000_2003(TBSimSkeleton):
    """
       Describes the old standalone Tile test beams (2000-2003)
       in the same context as CTB. The mother volume (with Geant4
       name 'CTB') is now a box 20x20x20 m

    """
    def __init__(self):
        if not(AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton')):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton']=self
            AtlasG4Eng.G4Eng.Name="Tile2000_2003"  # assigns a name
        else:
            G4AtlasEngine.log.warning(' SimSkeleton: the simulation '+\
            'has already a skeleton, you can find it in the '+\
            'G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(self):
        """ Place to handle JobProperties .
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties starting')

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.extra_flags:
            simFlags.load_tbtile_flags()

        # - switch off tasks
        DetFlags.pileup.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.digitize.all_setOff()
        DetFlags.overlay.all_setOff()
        DetFlags.readRDOPool.all_setOff()
        DetFlags.makeRIO.all_setOff()
        DetFlags.writeBS.all_setOff()
        DetFlags.readRDOBS.all_setOff()
        DetFlags.readRIOBS.all_setOff()
        DetFlags.readRIOPool.all_setOff()
        DetFlags.writeRIOPool.all_setOff()
        DetFlags.writeRDOPool.all_setOff()

        # not needed detectors
        DetFlags.ID_setOff()
        DetFlags.Muon_setOff()
        DetFlags.FCal_setOff()
        DetFlags.HEC_setOff()
        DetFlags.em_setOff()
        #
        globalflags.DataSource = "geant4"
        globalflags.DetGeo = "ctbh8"
        # mgallas this is a hack! needed for GeoModel and IOVDbSvc
        # TODO: Wow! Is this really still needed?
        import __main__
        __main__.DetDescrVersion = 'CTB'
        __main__.globalflags = globalflags
        globalflags.DetDescrVersion = 'CTB'

        ## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
        from AthenaCommon.AppMgr import ServiceMgr
        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
        ServiceMgr += IOVDbSvc()
        if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
            msg = "AtlasSimSkeleton._do_jobproperties :: conditions tag has not been set and is no longer guessed by SimAtlasKernel. "
            msg += "You must set the globaltags.ConditionsTag flag in your job options."
            AtlasG4Eng.G4Eng.log.error(msg)
            raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
        if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
            ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

        ## Print flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')
        jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties done')

    @classmethod
    def _do_external(self):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'starting')
        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        Geo2G4Svc=Geo2G4Svc()
        theApp.CreateSvc += [ "Geo2G4Svc" ]
        ServiceMgr +=Geo2G4Svc
        Geo2G4Svc.GetTopTransform = False

        from AtlasGeoModel import SetGeometryVersion
        from AthenaCommon.Configurable import Configurable
        if Configurable.allConfigurables.get('GeoModelSvc'):
            GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
        else:
            GeoModelSvc=theApp.service('GeoModelSvc')
        GeoModelSvc.AlignCallbacks =False

        from G4AtlasApps.SimFlags import simFlags
        if (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_2B2EB'):
            # 2 Barrels + 2 Extended Barrels
            GeoModelSvc.TileVersionOverride='TileTB-2B2EB-00'
        elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_2B1EB'):
            # 2 Barrels + 1 Extended Barrel
            GeoModelSvc.TileVersionOverride='TileTB-2B1EB-00'
        elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_3B'):
            # 3 Barrels
            GeoModelSvc.TileVersionOverride='TileTB-3B-00'
        elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_5B'):
            # 5 Barrels
            GeoModelSvc.TileVersionOverride='TileTB-5B-00'

        from AtlasGeoModel import GeoModelInit
        if(DetFlags.Calo_on()):
            # Common for the Calo
            include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
            include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
        ##--- Persistency of test-beam layout ----------------------------------
        if jobproperties.AthenaCommonFlags.PoolHitsOutput.statusOn :
            include( "TBDetDescrAthenaPool/TBDetDescrAthenaPool_joboptions.py" )
            from TBDetDescrAlg.TBDetDescrAlgConf import TBDetDescrLoader
            from AthenaCommon.AlgSequence import AlgSequence
            job=AlgSequence()
            TBDetDescrLoader=TBDetDescrLoader()
            job+=TBDetDescrLoader
            TBDetDescrLoader.TBElementContainer = "TBElementCnt"
            TBDetDescrLoader.TBDetDescrManager  = "TBDetDescrMgr"
            include( "TBDetDescrCnv/TBDetDescrCnv_jobOptions.py" )
            # do not read anything from StoreGate
            TBDetDescrLoader.ReadAction = 0
            # and write TBDetDescrContainer to StoreGate every event
            TBDetDescrLoader.WriteAction = 2
            TBDetDescrLoader.OutputLevel = 5

        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'done')

##   \@classmethod
##   def do_GeoSD(self):
##      """ Defines the geometry and SD
##      """
##      # -- Resize the World volume
##      from ctb_common import ctb
##      ctb.df.SetDx(20000)
##      ctb.df.SetDy(20000)
##      ctb.df.SetDz(20000)
##      # -- Calorimeters
##      AtlasG4Eng.G4Eng.add_DetFacility(ctb,ctb)
##      from G4AtlasApps.SimFlags import simFlags
##      if(DetFlags.Calo_on()):
##           # Check the consistency of the flags
##           if(simFlags.Eta.statusOn and
##              (simFlags.Theta.statusOn or
##              simFlags.Z.statusOn)):
##               raise ValueError,('THE ETA PARAMETER CAN NOT BE' +
##                       'SET TOGETHER WITH THETA AND Z')
##           elif(not(simFlags.Theta.statusOn and
##                    simFlags.Z.statusOn)and
##                not(simFlags.Eta.statusOn)):
##               raise ValueError,('THETA AND Z ARE NOT SET')
##           DeltaY=0.0
##           if(simFlags.Y.statusOn):
##               DeltaY=-simFlags.Y.get_Value()
##               if abs(DeltaY) > 600:
##                   raise ValueError,('THE Y VALUE MUST BE IN [-600,600] !!! '+
##                         'The selected value %s is not in the range.' %DeltaY)
##           PhiZ=0.0
##           if(simFlags.Phi.statusOn):
##               PhiZ=-math.radians(simFlags.Phi.get_Value())
##               AtlasG4Eng.G4Eng.log.debug('SimCtbKernel'+
##                   'Tile table phi rotation, phi ='+str(simFlags.Phi.get_Value()))
##               if (math.fabs(PhiZ)>0.15):
##                   raise ValueError,('THE PHI VALUE MUST BE IN [-8.5,8.5]!!! '+
##                   'The selected value %s not in range.' %simFlags.Phi.get_Value())
##           if(simFlags.Eta.statusOn):
##               # Mode 1 -> User enters only eta
##               eta=simFlags.Eta.get_Value()
##               if abs(eta) < 1.1:
##                   AtlasG4Eng.G4Eng.log.debug('SimCtbKernel'+
##                   'Tile table rotation: Mode 1 (eta only), eta ='+str(eta))
##                   ThetaY=-(PhysicalConstants.pi*0.5)+2*math.atan(math.exp(-eta))
##                   DeltaX=float(2298-6208)/math.cosh(eta)+6208
##                   DeltaF=0.0
##                   DeltaZ=0.0
##                   ctb_calo_rotation=math.degrees(ThetaY)
##               else:
##                   raise ValueError,('THE ETA VALUE MUST BE IN [-1.1,1.1]!!! '+
##                           'The selected value %s is not in the range.' %eta)
##           elif(simFlags.Theta.statusOn and simFlags.Z.statusOn):
##               theta=simFlags.Theta.get_Value()
##               z=simFlags.Z.get_Value()
##               eta=0.0
##               if abs(theta) < 60.01:
##                   AtlasG4Eng.G4Eng.log.debug('SimCtbKernel'+
##                    'Tile table rotation: Mode 2 (theta and Z), theta ='+
##                    str(theta)+' Zimp ='+str(z))
##                   # Mode 2 -> User enters theta!=+/-90 and Z
##                   # Z is the distance from center of the module to the desired
##                   # entrace point calculated at R=2290 (start of tilecal
##                   # module)
##                   ctb_calo_rotation=theta
##                   ThetaY=math.radians(theta)
##                   DeltaX=2298.0+3910.0*(1.0-math.cos(ThetaY))
##                   DeltaF=(2290.0*math.tan(-ThetaY)-z)
##                   DeltaZ=DeltaF*math.cos(ThetaY)
##               elif abs(abs(theta)-90.0) < 0.01:
##                   AtlasG4Eng.G4Eng.log.debug('SimCtbKernel'+
##                    'Tile table rotation: Mode 3 (theta=+/-90 and Z), theta ='+
##                    str(theta)+' R ='+str(z))
##                   # Mode 3 -> User enters theta=(+/-)90 and Z
##                   # Z is the distance from ATLAS center to corresponding
##                   # tilerow
##                   # e.g center of first tile row is at 2300 + 100/2 = 2350
##                   sign=int(theta>0)*2-1
##                   ctb_calo_rotation=sign*90.0
##                   ThetaY=sign*math.radians(90.0)
##                   # DeltaX = ctb shift + tile_rmin + module_length/2
##                   DeltaX=2298.0+2290.0+5640.0/2
##                   DeltaF=0.0
##                   DeltaZ=-sign*math.fabs(z)
##               else:
##                   raise ValueError,('THETA MUST BE IN [-60,60] or +/-90 !!! '+
##                        'The selected value %s is not in the range.' %theta)
##           else:
##               print 'Tile table rotation: ERROR unknown rotation mode'
##               raise ValueError,('UNKNOWN MODE - NEITHER ETA NOR THETA AND Z ARE SET')
##           from ctb_common import calo
##           AtlasG4Eng.G4Eng.add_DetFacility(calo,ctb)
##           calo_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(DeltaX,DeltaY,DeltaZ)
##           calo.df.RotateZ(PhiZ)
##           calo.df.RotateY(ThetaY)
##           calo.df.MoveTo(calo_pos)
##           if(DetFlags.Tile_on()):
##              from  ctb_calo import TileDet
##              # different from CTB
##              tiledet=TileDet('TB')
##              AtlasG4Eng.G4Eng.add_DetFacility(tiledet.ctb_tile,calo)
##              tilesd=tiledet.SD()
##           # ancillary: only the MuonWall
##           from ctb_common import MuonWall
##           MuonW=MuonWall(eta,ThetaY,DeltaF)
##           AtlasG4Eng.G4Eng.add_DetFacility(MuonW.ctb_muonwall,calo)


#--- LAr TB 2002-2004  ------------------------------------------------
class LArH6_TB(TBSimSkeleton):
    """
       Describes the old standalone LAr test beams at H6

    """
    def __init__(self):
        if not(AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton')):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton']=self
            AtlasG4Eng.G4Eng.Name="LArH6"         # assigns a name
        else:
            G4AtlasEngine.log.warning(' SimSkeleton: the simulation '+\
            'has already a skeleton, you can find it in the '+\
            'G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(self):
        """ Place to handle JobProperties .
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties starting')

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.extra_flags:
            simFlags.load_tbLArH6_flags()

        # - switch off tasks
        DetFlags.pileup.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.overlay.all_setOff()
        DetFlags.digitize.all_setOff()
        DetFlags.readRDOPool.all_setOff()
        DetFlags.makeRIO.all_setOff()
        DetFlags.writeBS.all_setOff()
        DetFlags.readRDOBS.all_setOff()
        DetFlags.readRIOBS.all_setOff()
        DetFlags.readRIOPool.all_setOff()
        DetFlags.writeRIOPool.all_setOff()
        DetFlags.writeRDOPool.all_setOff()

        DetFlags.ID_setOff()
        DetFlags.Muon_setOff()
        DetFlags.Tile_setOff()

        globalflags.DataSource = "geant4"
        globalflags.DetGeo = "ctbh6"
        # mgallas this is a hack! needed for GeoModel and IOVDbSvc
        # TODO: Wow! Is this really still needed?
        import __main__
        __main__.globalflags = globalflags
        if simFlags.SimLayout.get_Value() == "tb_LArH6_2003":
            globalflags.DetDescrVersion = 'ATLAS-H6-2003-02'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()
        elif simFlags.SimLayout.get_Value() == "tb_LArH6_2002":
            globalflags.DetDescrVersion = 'ATLAS-H6-2002-01'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()
        elif simFlags.SimLayout.get_Value() == "tb_LArH6EC_2002":
            globalflags.DetDescrVersion = 'ATLAS-TBEC-01'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()
        elif simFlags.SimLayout.get_Value() == "tb_LArH6_2004":
            globalflags.DetDescrVersion = 'ATLAS-H6-2004-00'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()

        ## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
        from AthenaCommon.AppMgr import ServiceMgr
        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
        ServiceMgr += IOVDbSvc()
        if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
            msg = "AtlasSimSkeleton._do_jobproperties :: conditions tag has not been set and is no longer guessed by SimAtlasKernel. "
            msg += "You must set the globaltags.ConditionsTag flag in your job options."
            AtlasG4Eng.G4Eng.log.error(msg)
            raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
        if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
            ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

        ## Print flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')
        jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties done')

    @classmethod
    def _do_external(self):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'starting')

       #from LArH6Detectors import Det_Layouts
        from tbLArH6_calo import Det_Layouts

        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        Geo2G4Svc=Geo2G4Svc()
        theApp.CreateSvc += [ "Geo2G4Svc" ]
        ServiceMgr += Geo2G4Svc
        # We do not use top transforms for the CTB geometry
        Geo2G4Svc.GetTopTransform = False

        #--- GeoModel stuff ----------------------------------------------------
        from AtlasGeoModel import SetGeometryVersion
        from AtlasGeoModel import GeoModelInit

        from AthenaCommon.Configurable import Configurable
        if Configurable.allConfigurables.get('GeoModelSvc'):
            GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
        else:
            GeoModelSvc=theApp.service('GeoModelSvc')
        GeoModelSvc.AlignCallbacks = False

        if(DetFlags.Calo_on()):
            # Common for the Calo
            include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
            include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'done')

        # mgallas do more here: this is not yet supported

    @classmethod
    def do_GeoSD(self):
        """ Defines de geometry and SD
        """
        #from G4AtlasApps.SimFlags import simFlags
        #from tbLArH6_calo import LArWorld
        #larworld=LArWorld(simFlags.SimLayout.get_Value())
        #AtlasG4Eng.G4Eng.add_DetFacility(larworld.world,larworld.world)
        if DetFlags.Calo_on():
            if DetFlags.geometry.LAr_on():
                from atlas_calo import PyLArG4RunControler
                LArG4RunControl=PyLArG4RunControler('PyLArG4RunControl',\
                                                    'LArG4RunControlDict')
                #from tbLArH6_calo import LArH6
                #LArH6Detector=LArH6()
                #if simFlags.CalibrationRun.statusOn:
                #    if simFlags.CalibrationRun.get_Value()=='LAr':
                #        LArH6Detector._initSD(1)
                #else:
                #    LArH6Detector._initSD(0)
                #LArH6Detector._initPR()
                #AtlasG4Eng.G4Eng.add_DetFacility(LArH6Detector.H6_lar,\
                #                                          larworld.world)

    @classmethod
    def do_UserActions(self):
        # ADS: this is not needed anymore with the migration to the new MT UAs
        #from G4AtlasServices.G4AtlasUserActionConfig import UAStore
        #from G4AtlasApps.SimFlags import simFlags
        #if (simFlags.SimLayout.get_Value()=='tb_LArH6_2004'):
        #    # ADS FIXME is it ok to add this system action here?
        #    UAStore.addSystemAction('LArHitsH6EventAction',['EndOfEvent'])
        #    UAStore.addAction('LArGeoH62004EventAction',['EndOfEvent'])
        #    if simFlags.LArTB_H6Step.statusOn:
        #       if simFlags.LArTB_H6Step.get_Value():
        #           UAStore.addAction('LArGeoH62004SteppingAction',['Step'])
        #           UAStore.addAction('RadLenNtuple',['BeginOfEvent','EndOfEvent','Step'])

        return
