# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

# This module is based upon Control/AthenaCommon/python/DetFlags.py
# Only some flags have been migrated. A full list of what the old
# DetFlags provided is given for reference below:
# detectors : ID = bpipe pixel SCT TRT BCM DBM
#             Forward = Lucid ZDC ALFA AFP FwdRegion
#             LAr = em HEC FCal
#             Calo = em HEC FCal Tile
#             Muon = MDT CSC TGC RPC sTGC Micromegas
#             Truth
#             BField
#             FTK
# tasks:
#   geometry : setup the geometry
#   dcs : DCS information is available
#   simulate : simulate
#   simulateLVL1 : LVL1 simulation
#   detdescr : setup detector description (for anything which is not geant)
#   pileup   : collect hits from physics and min bias events
#   digitize : hit -> RDO (raw data objects)
#   makeRIO  : RDO -> RIO (Reconstruction Input Objects)
#   writeBS  : write RDO byte stream
#   readRDOBS : read RDO from byte stream
#   readRDOPool : read RDO from pool
#   readRIOBS : read RIO directly from BS
#   writeRDOPool : write RDO in pool
#   readRIOPool  : read RIO from pool
#   writeRIOPool : write RIO in pool
#   overlay : overlay setup

def createDetectorConfigFlags():
    dcf=AthConfigFlags()

    #Detector.Geometry - merger of the old geometry and detdescr tasks
    dcf.addFlag('Detector.GeometryBpipe', False)
    dcf.addFlag('Detector.GeometryBCM',   False)
    dcf.addFlag('Detector.GeometryDBM',   False)
    dcf.addFlag('Detector.GeometryPixel', False)
    dcf.addFlag('Detector.GeometrySCT',   False)
    dcf.addFlag('Detector.GeometryTRT',   False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.GeometryID',    lambda prevFlags : (prevFlags.Detector.GeometryBCM or prevFlags.Detector.GeometryDBM or
                                                              prevFlags.Detector.GeometryPixel or prevFlags.Detector.GeometrySCT or
                                                              prevFlags.Detector.GeometryTRT))
    dcf.addFlag('Detector.GeometryLAr',   False) # Add separate em HEC and FCAL flags?
    dcf.addFlag('Detector.GeometryTile',  False)
    dcf.addFlag('Detector.GeometryCalo',  lambda prevFlags : (prevFlags.Detector.GeometryLAr or prevFlags.Detector.GeometryTile))
    dcf.addFlag('Detector.GeometryCSC',   False)
    dcf.addFlag('Detector.GeometryMDT',   False)
    dcf.addFlag('Detector.GeometryRPC',   False)
    dcf.addFlag('Detector.GeometryTGC',   False)
    dcf.addFlag('Detector.GeometrysTGC',  False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.GeometryMM',    False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.GeometryMuon',  lambda prevFlags : (prevFlags.Detector.GeometryCSC or prevFlags.Detector.GeometryMDT or
                                                              prevFlags.Detector.GeometryRPC or prevFlags.Detector.GeometryTGC or
                                                              prevFlags.Detector.GeometrysTGC or prevFlags.Detector.GeometryMM))
    dcf.addFlag('Detector.GeometryLucid', False)
    dcf.addFlag('Detector.GeometryZDC',   False)
    dcf.addFlag('Detector.GeometryALFA',  False)
    dcf.addFlag('Detector.GeometryAFP',   False)
    dcf.addFlag('Detector.GeometryFwdRegion',False)
    dcf.addFlag('Detector.GeometryForward',lambda prevFlags : (prevFlags.Detector.GeometryLucid or prevFlags.Detector.GeometryZDC or
                                                               prevFlags.Detector.GeometryALFA or prevFlags.Detector.GeometryAFP or
                                                               prevFlags.Detector.GeometryFwdRegion))
    dcf.addFlag('Detector.GeometryCavern',False)
    dcf.addFlag('Detector.Geometry', lambda prevFlags : (prevFlags.Detector.GeometryBpipe or prevFlags.Detector.GeometryID or
                                                         prevFlags.Detector.GeometryCalo or prevFlags.Detector.GeometryMuon or
                                                         prevFlags.Detector.GeometryForward or prevFlags.Detector.GeometryCavern))

    #Detector.Simulate
    dcf.addFlag('Detector.SimulateBpipe', False)
    dcf.addFlag('Detector.SimulateBCM',   False)
    dcf.addFlag('Detector.SimulateDBM',   False)
    dcf.addFlag('Detector.SimulatePixel', False)
    dcf.addFlag('Detector.SimulateSCT',   False)
    dcf.addFlag('Detector.SimulateTRT',   False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.SimulateID',    lambda prevFlags : (prevFlags.Detector.SimulateBCM or prevFlags.Detector.SimulateDBM or
                                                              prevFlags.Detector.SimulatePixel or prevFlags.Detector.SimulateSCT or
                                                              prevFlags.Detector.SimulateTRT))
    dcf.addFlag('Detector.SimulateLAr',   False) # Add separate em HEC and FCAL flags?
    dcf.addFlag('Detector.SimulateTile',  False)
    dcf.addFlag('Detector.SimulateCalo',  lambda prevFlags : (prevFlags.Detector.SimulateLAr or prevFlags.Detector.SimulateTile))
    dcf.addFlag('Detector.SimulateCSC',   False)
    dcf.addFlag('Detector.SimulateMDT',   False)
    dcf.addFlag('Detector.SimulateRPC',   False)
    dcf.addFlag('Detector.SimulateTGC',   False)
    dcf.addFlag('Detector.SimulatesTGC',  False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.SimulateMM',    False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.SimulateMuon',  lambda prevFlags : (prevFlags.Detector.SimulateCSC or prevFlags.Detector.SimulateMDT or
                                                              prevFlags.Detector.SimulateRPC or prevFlags.Detector.SimulateTGC or
                                                              prevFlags.Detector.SimulatesTGC or prevFlags.Detector.SimulateMM))
    dcf.addFlag('Detector.SimulateLucid', False)
    dcf.addFlag('Detector.SimulateZDC',   False)
    dcf.addFlag('Detector.SimulateALFA',  False)
    dcf.addFlag('Detector.SimulateAFP',   False)
    dcf.addFlag('Detector.SimulateFwdRegion',False)
    dcf.addFlag('Detector.SimulateForward',lambda prevFlags : (prevFlags.Detector.SimulateLucid or prevFlags.Detector.SimulateZDC or
                                                               prevFlags.Detector.SimulateALFA or prevFlags.Detector.SimulateAFP or
                                                               prevFlags.Detector.SimulateFwdRegion))
    dcf.addFlag('Detector.SimulateCavern',False)
    dcf.addFlag('Detector.Simulate',      lambda prevFlags : (prevFlags.Detector.SimulateBpipe or prevFlags.Detector.SimulateID or
                                                              prevFlags.Detector.SimulateCalo or prevFlags.Detector.SimulateMuon or
                                                              prevFlags.Detector.SimulateForward or prevFlags.Detector.SimulateCavern))

    #Detector.Overlay
    dcf.addFlag('Detector.OverlayBCM',   False)
    dcf.addFlag('Detector.OverlayDBM',   False)
    dcf.addFlag('Detector.OverlayPixel', False)
    dcf.addFlag('Detector.OverlaySCT',   False)
    dcf.addFlag('Detector.OverlayTRT',   False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.OverlayID',    lambda prevFlags : (prevFlags.Detector.OverlayBCM or prevFlags.Detector.OverlayDBM or
                                                              prevFlags.Detector.OverlayPixel or prevFlags.Detector.OverlaySCT or
                                                              prevFlags.Detector.OverlayTRT))
    dcf.addFlag('Detector.OverlayLAr',   False) # Add separate em HEC and FCAL flags?
    dcf.addFlag('Detector.OverlayTile',  False)
    dcf.addFlag('Detector.OverlayCalo',  lambda prevFlags : (prevFlags.Detector.OverlayLAr or prevFlags.Detector.OverlayTile))
    dcf.addFlag('Detector.OverlayCSC',   False)
    dcf.addFlag('Detector.OverlayMDT',   False)
    dcf.addFlag('Detector.OverlayRPC',   False)
    dcf.addFlag('Detector.OverlayTGC',   False)
    dcf.addFlag('Detector.OverlaysTGC',  False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.OverlayMM',    False) # Set default according to prevFlags.GeoModel.Run?
    #Forward Detector Overlay not supported yet
    dcf.addFlag('Detector.OverlayMuon',  lambda prevFlags : (prevFlags.Detector.OverlayCSC or prevFlags.Detector.OverlayMDT or
                                                             prevFlags.Detector.OverlayRPC or prevFlags.Detector.OverlayTGC or
                                                             prevFlags.Detector.OverlaysTGC or prevFlags.Detector.OverlayMM))
    dcf.addFlag('Detector.Overlay',      lambda prevFlags : (prevFlags.Detector.OverlayID or prevFlags.Detector.OverlayCalo or
                                                             prevFlags.Detector.OverlayMuon))
    return dcf