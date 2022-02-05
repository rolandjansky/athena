# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import getDefaultDetectors
# This module is based upon Control/AthenaCommon/python/DetFlags.py
# Only some flags have been migrated. A full list of what the old
# DetFlags provided is given for reference below:
# detectors : ID = bpipe pixel SCT TRT BCM DBM
#             ITk = bpipe ITkPixel ITkStrip BCMPrime (in future maybe PLR)
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


# all detectors - used in helper functions
allDetectors = [
    'Bpipe', 'Cavern',
    'BCM', 'DBM', 'Pixel', 'SCT', 'TRT',
    'BCMPrime', 'PLR', 'ITkPixel', 'ITkStrip', 'HGTD',
    'LAr', 'Tile', 'MBTS',
    'CSC', 'MDT', 'RPC', 'TGC', 'sTGC', 'MM',
    'Lucid', 'ZDC', 'ALFA', 'AFP', 'FwdRegion',
]
# all detector groups - used in helper functions
allGroups = {
    'ID': ['BCM', 'DBM', 'Pixel', 'SCT', 'TRT'],
    'ITk': ['BCMPrime', 'ITkPixel', 'ITkStrip', 'PLR'],
    'Calo': ['LAr', 'Tile', 'MBTS'],
    'Muon': ['CSC', 'MDT', 'RPC', 'TGC', 'sTGC', 'MM'],
    'Forward': ['Lucid', 'ZDC', 'ALFA', 'AFP', 'FwdRegion'],
}


def createDetectorConfigFlags():
    dcf = AthConfigFlags()

    ## Detector.Geometry* flags represent the default full geometry,
    ## autoconfigured from the geometry tag
    dcf.addFlag('Detector.GeometryBpipe', True)  # always enabled by default

    # Inner Detector
    dcf.addFlag('Detector.GeometryBCM',   lambda prevFlags : 'BCM' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryDBM',   lambda prevFlags : 'DBM' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryPixel', lambda prevFlags : 'Pixel' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometrySCT',   lambda prevFlags : 'SCT' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryTRT',   lambda prevFlags : 'TRT' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryID',    lambda prevFlags : (prevFlags.Detector.GeometryBCM or prevFlags.Detector.GeometryDBM
                                                              or prevFlags.Detector.GeometryPixel or prevFlags.Detector.GeometrySCT
                                                              or prevFlags.Detector.GeometryTRT))

    # Upgrade ITk Inner Tracker is a separate and parallel detector
    dcf.addFlag('Detector.GeometryPLR',   lambda prevFlags : 'PLR' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryBCMPrime', lambda prevFlags : 'BCMPrime' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryITkPixel', lambda prevFlags : 'ITkPixel' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryITkStrip', lambda prevFlags : 'ITkStrip' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryITk',      lambda prevFlags : (prevFlags.Detector.GeometryBCMPrime
                                                                 or prevFlags.Detector.GeometryITkPixel
                                                                 or prevFlags.Detector.GeometryITkStrip
                                                                 or prevFlags.Detector.GeometryPLR))
    # HGTD
    dcf.addFlag('Detector.GeometryHGTD', lambda prevFlags : 'HGTD' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))

    # Calorimeters
    dcf.addFlag('Detector.GeometryLAr',  lambda prevFlags : 'LAr' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion)) # Add separate em HEC and FCAL flags?
    dcf.addFlag('Detector.GeometryTile', lambda prevFlags : 'Tile' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryMBTS', lambda prevFlags : (prevFlags.Detector.GeometryLAr and 'MBTS' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion)))
    dcf.addFlag('Detector.GeometryCalo', lambda prevFlags : (prevFlags.Detector.GeometryLAr or prevFlags.Detector.GeometryTile))

    # Muon Spectrometer
    dcf.addFlag('Detector.GeometryCSC',  lambda prevFlags : 'CSC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryMDT',  lambda prevFlags : 'MDT' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryRPC',  lambda prevFlags : 'RPC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryTGC',  lambda prevFlags : 'TGC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometrysTGC', lambda prevFlags : 'sTGC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryMM',   lambda prevFlags : 'MM' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.GeometryMuon', lambda prevFlags : (prevFlags.Detector.GeometryCSC or prevFlags.Detector.GeometryMDT
                                                             or prevFlags.Detector.GeometryRPC or prevFlags.Detector.GeometryTGC
                                                             or prevFlags.Detector.GeometrysTGC or prevFlags.Detector.GeometryMM))

    # Forward detectors (disabled by default)
    dcf.addFlag('Detector.GeometryLucid',     False)
    dcf.addFlag('Detector.GeometryZDC',       False)
    dcf.addFlag('Detector.GeometryALFA',      False)
    dcf.addFlag('Detector.GeometryAFP',       False)
    dcf.addFlag('Detector.GeometryFwdRegion', False)
    dcf.addFlag('Detector.GeometryForward',   lambda prevFlags : (prevFlags.Detector.GeometryLucid or prevFlags.Detector.GeometryZDC
                                                                  or prevFlags.Detector.GeometryALFA or prevFlags.Detector.GeometryAFP
                                                                  or prevFlags.Detector.GeometryFwdRegion))

    # Cavern (disabled by default)
    dcf.addFlag('Detector.GeometryCavern', False)


    ## Detector.Enable* flags represent the default full geometry,
    ## autoconfigured from geometry flags
    # Inner Detector
    dcf.addFlag('Detector.EnableBCM',   lambda prevFlags : prevFlags.Detector.GeometryBCM)
    dcf.addFlag('Detector.EnableDBM',   lambda prevFlags : prevFlags.Detector.GeometryDBM)
    dcf.addFlag('Detector.EnablePixel', lambda prevFlags : prevFlags.Detector.GeometryPixel)
    dcf.addFlag('Detector.EnableSCT',   lambda prevFlags : prevFlags.Detector.GeometrySCT)
    dcf.addFlag('Detector.EnableTRT',   lambda prevFlags : prevFlags.Detector.GeometryTRT)
    dcf.addFlag('Detector.EnableID',    lambda prevFlags : prevFlags.Detector.GeometryID and
                                                           (prevFlags.Detector.EnableBCM or prevFlags.Detector.EnableDBM
                                                            or prevFlags.Detector.EnablePixel or prevFlags.Detector.EnableSCT
                                                            or prevFlags.Detector.EnableTRT))

    # Upgrade ITk Inner Tracker is a separate and parallel detector
    dcf.addFlag('Detector.EnablePLR',   lambda prevFlags : prevFlags.Detector.GeometryPLR)
    dcf.addFlag('Detector.EnableBCMPrime', lambda prevFlags : prevFlags.Detector.GeometryBCMPrime)
    dcf.addFlag('Detector.EnableITkPixel', lambda prevFlags : prevFlags.Detector.GeometryITkPixel)
    dcf.addFlag('Detector.EnableITkStrip', lambda prevFlags : prevFlags.Detector.GeometryITkStrip)
    dcf.addFlag('Detector.EnableITk',      lambda prevFlags : prevFlags.Detector.GeometryITk and
                                                              (prevFlags.Detector.EnableBCMPrime
                                                               or prevFlags.Detector.EnableITkPixel
                                                               or prevFlags.Detector.EnableITkStrip
                                                               or prevFlags.Detector.EnablePLR))
    # HGTD
    dcf.addFlag('Detector.EnableHGTD', lambda prevFlags : prevFlags.Detector.GeometryHGTD)

    # Calorimeters
    dcf.addFlag('Detector.EnableLAr',    lambda prevFlags : prevFlags.Detector.GeometryLAr)
    dcf.addFlag('Detector.EnableTile',   lambda prevFlags : prevFlags.Detector.GeometryTile)
    dcf.addFlag('Detector.EnableMBTS',   lambda prevFlags : prevFlags.Detector.GeometryMBTS)
    dcf.addFlag('Detector.EnableL1Calo', lambda prevFlags : (prevFlags.Detector.EnableLAr or prevFlags.Detector.EnableTile))
    dcf.addFlag('Detector.EnableCalo',   lambda prevFlags : prevFlags.Detector.GeometryCalo and
                                                            (prevFlags.Detector.EnableLAr or prevFlags.Detector.EnableTile))

    # Muon Spectrometer
    dcf.addFlag('Detector.EnableCSC',  lambda prevFlags : prevFlags.Detector.GeometryCSC)
    dcf.addFlag('Detector.EnableMDT',  lambda prevFlags : prevFlags.Detector.GeometryMDT)
    dcf.addFlag('Detector.EnableRPC',  lambda prevFlags : prevFlags.Detector.GeometryRPC)
    dcf.addFlag('Detector.EnableTGC',  lambda prevFlags : prevFlags.Detector.GeometryTGC)
    dcf.addFlag('Detector.EnablesTGC', lambda prevFlags : prevFlags.Detector.GeometrysTGC)
    dcf.addFlag('Detector.EnableMM',   lambda prevFlags : prevFlags.Detector.GeometryMM)
    dcf.addFlag('Detector.EnableMuon', lambda prevFlags : prevFlags.Detector.GeometryMuon and
                                                          (prevFlags.Detector.EnableCSC or prevFlags.Detector.EnableMDT
                                                           or prevFlags.Detector.EnableRPC or prevFlags.Detector.EnableTGC
                                                           or prevFlags.Detector.EnablesTGC or prevFlags.Detector.EnableMM))

    # Forward detectors (disabled by default)
    dcf.addFlag('Detector.EnableLucid',     False)
    dcf.addFlag('Detector.EnableZDC',       False)
    dcf.addFlag('Detector.EnableALFA',      False)
    dcf.addFlag('Detector.EnableAFP',       False)
    dcf.addFlag('Detector.EnableFwdRegion', False)
    dcf.addFlag('Detector.EnableForward',   lambda prevFlags : prevFlags.Detector.GeometryForward and
                                                               (prevFlags.Detector.EnableLucid or prevFlags.Detector.EnableZDC
                                                                or prevFlags.Detector.EnableALFA or prevFlags.Detector.EnableAFP
                                                                or prevFlags.Detector.EnableFwdRegion))

    return dcf


def _parseDetectorsList(flags, detectors):
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # load flags
    flags._loadDynaFlags('Detector')
    # first check if we have groups
    groups = [d for d in detectors if d in allGroups.keys()]
    detectors = [d for d in detectors if d not in allGroups.keys()]
    for g in groups:
        log.debug("Evaluating detector group '%s'", g)
        if g == 'Forward':
            # forward detectors are special
            for d in allGroups[g]:
                log.debug("Appending detector '%s'", d)
                detectors.append(d)
        else:
            # in case of groups only enable defaults
            for d in allGroups[g]:
                if d in getDefaultDetectors(flags.GeoModel.AtlasVersion):
                    log.debug("Appending detector '%s'", d)
                    detectors.append(d)

    # check if valid detectors are required
    for d in detectors:
        if d not in allDetectors:
            log.warning("Unknown detector '%s'", d)

    return detectors


def setupDetectorsFromList(flags, detectors, toggle_geometry=False, validate_only=False):
    """Setup (toggle) detectors from a pre-defined list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # print summary
    if validate_only:
        log.info('Required detectors: %s', detectors)
    else:
        log.info('Setting detectors to: %s', detectors)

    # go through all of the detectors and check what should happen
    for d in allDetectors:
        status = d in detectors
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags(name) != status:
                changed = True
                if validate_only:
                    log.warning("Flag '%s' should be %s but is set to %s", name, status, not status)
                else:
                    log.info("Toggling '%s' from %s to %s", name, not status, status)
                    flags._set(name, status)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags(name) != status:
                    changed = True
                    if validate_only:
                        log.warning("Flag '%s' should be %s but is set to %s", name, status, not status)
                    else:
                        log.info("Toggling '%s' from %s to %s", name, not status, status)
                        flags._set(name, status)

    return changed


def enableDetectors(flags, detectors, toggle_geometry=False):
    """Enable detectors from a list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # debugging
    log.info('Enabling detectors: %s', detectors)

    # go through all of the detectors and enable them if needed
    for d in detectors:
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags(name) is not True:
                changed = True
                log.info("Enabling '%s'", name)
                flags._set(name, True)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags(name) is not True:
                    changed = True
                    log.info("Enabling '%s'", name)
                    flags._set(name, True)

    return changed


def disableDetectors(flags, detectors, toggle_geometry=False):
    """Disable detectors from a list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # debugging
    log.info('Disabling detectors: %s', detectors)

    # go through all of the detectors and disable them if needed
    for d in detectors:
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags(name) is not False:
                changed = True
                log.info("Disabling '%s'", name)
                flags._set(name, False)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags(name) is not False:
                    changed = True
                    log.info("Disabling '%s'", name)
                    flags._set(name, False)

    return changed
