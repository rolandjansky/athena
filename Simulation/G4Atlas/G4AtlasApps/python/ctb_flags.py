# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
CTB simulation flags.

Apart from the AthenaCommon.DetFlags and AthenaCommon.GlobalFlags the simulation
jobs for the CTB (2004) needs specific flags that are collected here.
"""

__author__ =  'M. Gallas'


from AthenaCommon.JobProperties import JobProperty

## Change some SimFlags defaults for CTB sim
from G4AtlasApps.SimFlags import jobproperties
jobproperties.SimFlags.DoLArBirk = False

class AncillaryUpstream(JobProperty):
    """
    Sets the ancillary detectors and upstram material.

    There are three possible configurations (PeriodA, PeriodB, PeriodC)
    described in:
    "ATLAS Barrel Combined Run in 2004 Test Beam Setup and its evolutions"
    https://edms.cern.ch/file/406980/3/TB2004_layout_v06.pdf

    The default is set to 'PeriodB'. For the photon runs is always
    'PeriodC' and for the LAr material studies is 'PeriodA'.
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['PeriodA','PeriodB','PeriodC']
    StoredValue = 'PeriodB'


class AncillaryLArTile(JobProperty):
    """
    Sets the ancillary detectors in the LAr-Tile table.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True


class AncillaryMuon(JobProperty):
    """
    Sets the ancillary detectors in the Muon region.
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True


class BeamConditions(JobProperty):
    """
    Enable the use of the beam information stored for each
    run in the CtbRunConditions file (if this is available).

    NB. For the moment the BeamConditions contain the
    beam divergence at ~ -1m and in this case all the
    upstream material is replaced by an equiv material
    block. It works only with the ctbh8_combined layout.
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = True


class CaloOffsetY(JobProperty):
    """
    Sets an offset in the Y coordinate of the CALO envelope.
    """
    statusOn = False
    allowedTypes = ['float','int']
    StoredValue = 0.0


class CaloOffsetZ(JobProperty):
    """
    Sets an offset in the Z coordinate of the CALO envelope.
    """
    statusOn = False
    allowedTypes = ['float','int']
    StoredValue = 0.0


class DumpMuon(JobProperty):
    """
    Dump for the Muon detector.

    In H8 CTB there is a Muon dump that was placed in between the
    CALO region and MUON region. Depending on the runs the dump
    was in or out.
    Default value = False (out of the beam).
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False


class Eta(JobProperty):
    """
    Eta value.

    In the CTB test beam simulation Eta means that the LAr-Tile
    table was rotated in Y axis and shifted in X by:

    ThetaY = -(pi/2)+2*atan(math.exp(-Eta))
    DeltaX = (2298-6208)/cosh(Eta)+6208

    - The default status is True and StoredValue = 0.44
    - Values from 0 to 1.2
    """
    statusOn = True
    allowedTypes = ['int','float']
    StoredValue = 0.44


class IdetOracleTag(JobProperty):
    """
    Oracle tag for the Inner detector geometries.

    Values: 'InnerDetector-CTB-01/03/04/05/06/08/09'

    Note: since 02/02/06 the ORACLE_tag names for the IDET have
          added "-01" at the end. The geometry will is the same
          only stuff like the TRT timing, driftimes and other
          TRT digitization aspects.
    """
    allowedTypes = ['str']
    allowedValues = ['InnerDetector-CTB-01-01','InnerDetector-CTB-03-01',
                     'InnerDetector-CTB-04-01','InnerDetector-CTB-05-01',
                     'InnerDetector-CTB-06-01','InnerDetector-CTB-08-01',
                     'InnerDetector-CTB-09-01']
    StoredValue = 'InnerDetector-CTB-04-01'


class LArEMBenergyCor(JobProperty):
    """
    EMBenergyCor correction in the LAr
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True


class LArFarUpstreamMaterial(JobProperty):
    """
    Sets an extra amount of in the beam line

    (13.35mm ~0.15RadLengths) Al Rad Length = 8.9cm at
    -20 meters or at -1020 mm in case
    simFlags.BeamConditions.Value==True
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False


class LArMaterialAluThickness(JobProperty):
    """
    Sets the thickness of the Aluminium blocks
    used for the LAr material studies.

    Possible values from 0 to 100 mm
    The typical values are:
      0
      25 (1 block )
      50 (2 blocks)
      75 (2 blocks)

    The transversal dimensions are 200x200 mm
    The distance from the 1st block and the cryo is 60 mm
    """
    allowedTypes = ['float']
    StoredValue = 0.


class LArMuonChamber(JobProperty):
    """
    Sets an equivalent Al material corresponding to the
    Muon Chamber in front of the LAr cryostat

    The muon chamber was installed on 18/10/2004. From
    then on all the runs had the muon chamber in front!
    In terms of run numbers that is all numbers > 2102181
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False


class LeadUpstreamAbs(JobProperty):
    """
    Lead upstream absorber

    Default value = False (out of the beam).
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False


class MagFieldMap(JobProperty):
    """
    Magnetic Field map for the CTB.

    This flag overwrite the constant field value installed in the
    MBPSID magnet with the field map given by the file here specified.
    The rest of the magnets will be constant fields.
    """
    allowedTypes = ['str']
    allowedValues = ['mbps1-all-id-800-mbps2-muons-800.data',
                   'mbps1-200-source.data', 'mbps1-500-source.data',
                   'mbps1-300-source.data', 'mbps1-850-source.data',
                   'minus-mbps1-850-source.data',
                   'minus-mbps1-500-source.data', 'minus-mbps1-300-source.data',
                   'minus-mbps1-200-source.data', 'mbps1-100-source.data','minus-mbps1-100-source.data','']
    StoredValue = ''


class MagFieldMapCalculation(JobProperty):
    """
    Sets different calculations of the magnetic field map.

    0 --> non symmetrical calculation (standard)
    1 --> symmetrical calculation from the first half of the MBPSID magnet.
    """
    statusOn = True
    allowedTypes = ['int']
    allowedValues = [0,1]
    StoredValue = 0


class MagnetMBPLBy(JobProperty):
    """
    Magnetic "By" component of the MBPL magnet.

    The MBPL magnet has been installed in front of the Muon barrel.
    It is a 2m version of the MBP magnets with a gap of 110 mm.
    The maximum value is 2T.

    Expected value in teslas.
    """
    allowedTypes = ['int','float']
    StoredValue = 0.


class MagnetMBPL12Bz(JobProperty):
    """
    Magnetic "Bz" component of the MBPL12 magnet.

    Magnet used during the photon runs. It is placed upstream
    the MBPSID magnet. It is a 2m version of the MBP magnets
    with a gap of 110 mm.

    Expected value in teslas.
    """
    allowedTypes = ['int','float']
    StoredValue = 0.


class MagnetMBPL13By(JobProperty):
    """
    Magnetic "By" component of the MBPL13 magnet.

    Magnet used during the photon runs. It is placed upstream
    the MBPSID magnet. It is a 2m version of the MBP magnets
    with a gap of 110 mm.

    Expected value in teslas.
    """
    allowedTypes = ['int','float']
    StoredValue = 0.


class MagnetMBPSIDBz(JobProperty):
    """
    Magnetic "Bz" component of the MBPSID magnet.

    MBPSID is a 1m long MBP magnet with a gap of 230 mm.
    The maximun field is 1.4 Teslas.

    Expected value in teslas.
    """
    allowedTypes = ['int','float']
    StoredValue = 0.


class MagnetMBPS2By(JobProperty):
    """
    Magnetic "By" component of the MBPS2 magnet.

    It is a 1m version of the MBP magnets with a gap of 110 mm.
    Expected value in teslas.
    """
    allowedTypes = ['int','float']
    StoredValue = 0.


class GeoModelTileVersion(JobProperty):
    """
    This allows to overwrite the Tile GeoModel version

    During the 'ctbh8_combined' Tile had only 3 Barrels
    the non-sysmetric configuration 3B+3EB and phatom
    was only during July 2004.
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['TileTB-3B-00','TileTB-3B3EB-00']
    StoredValue = 'TileTB-3B-00'
