#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList, enableDetectors, disableDetectors

ConfigFlags._loadDynaFlags('GeoModel')
ConfigFlags._loadDynaFlags('Detector')

test_tags = [
    'ATLAS-R1-2012-03-01-00',   # example Run 1
    'ATLAS-R2-2016-01-00-01',   # example Run 2
    'ATLAS-R3S-2021-01-00-01',  # example Run 3
    'ATLAS-P2-ITK-24-00-00'     # example Run 4
]

flags_runs = {t: ConfigFlags.clone() for t in test_tags}

for tag in test_tags:
    flags = flags_runs[tag]
    flags.GeoModel.AtlasVersion = tag

    print(f'{flags.GeoModel.AtlasVersion} - {flags.GeoModel.Run.value}')
    flags.dump('Detector.(Geometry|Enable)', True)
    print()

    # test flags that change with time
    assert flags.Detector.EnableBCM == (tag != 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableDBM == (tag in ['ATLAS-R2-2016-01-00-01', 'ATLAS-R3S-2021-01-00-01'])
    assert flags.Detector.EnablePixel == (tag != 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableSCT == (tag != 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableTRT == (tag != 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableITkPixel == (tag == 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableITkStrip == (tag == 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableHGTD == (tag == 'ATLAS-P2-ITK-24-00-00')
    assert flags.Detector.EnableCSC == (tag in ['ATLAS-R1-2012-03-01-00', 'ATLAS-R2-2016-01-00-01'])
    assert flags.Detector.EnablesTGC == (tag in ['ATLAS-R3S-2021-01-00-01', 'ATLAS-P2-ITK-24-00-00'])
    assert flags.Detector.EnableMM == (tag in ['ATLAS-R3S-2021-01-00-01', 'ATLAS-P2-ITK-24-00-00'])


# test setup for Run 2
flags = flags_runs['ATLAS-R2-2016-01-00-01']

print("Test: validate ['ID', 'Calo', 'Muon']")
assert not setupDetectorsFromList(flags, ['ID', 'Calo', 'Muon'], validate_only=True)
print()

print("Test: validate ['Muon']")
assert setupDetectorsFromList(flags, ['Muon'], validate_only=True)
print()

print("Test: setup ['Muon']")
assert setupDetectorsFromList(flags, ['Muon'])
assert flags.Detector.EnableCSC
assert not flags.Detector.EnableMM
assert not flags.Detector.EnablePixel
print()

print("Test: setup ['Geometry.Muon']")
assert not setupDetectorsFromList(flags, ['Muon'])
flags.Detector.GeometryMuon = False
assert not flags.Detector.EnableMuon
print()

print("Test: setup ['ID'] (geometry included)")
assert setupDetectorsFromList(flags, ['ID'], toggle_geometry=True)
assert flags.Detector.EnablePixel
assert flags.Detector.GeometryPixel
assert flags.Detector.EnableSCT
assert flags.Detector.GeometrySCT
assert flags.Detector.EnableTRT
assert flags.Detector.GeometryTRT
assert not flags.Detector.EnableCSC
assert not flags.Detector.GeometryCSC
assert not flags.Detector.EnableMM
print()

print("Test: enable ['ID'] (geometry included)")
assert not enableDetectors(flags, ['ID'], toggle_geometry=True)
print()

print("Test: disable ['ID'] (geometry included)")
assert disableDetectors(flags, ['ID'], toggle_geometry=True)
assert not flags.Detector.EnablePixel
assert not flags.Detector.GeometryPixel
assert not flags.Detector.EnableSCT
assert not flags.Detector.GeometrySCT
assert not flags.Detector.EnableTRT
assert not flags.Detector.GeometryTRT
print()

print("Test: enable ['MDT', 'RPC']")
assert enableDetectors(flags, ['MDT', 'RPC'])
assert flags.Detector.EnableMDT
assert not flags.Detector.GeometryMDT
assert flags.Detector.EnableRPC
assert not flags.Detector.GeometryRPC
assert not flags.Detector.EnableCSC
assert not flags.Detector.GeometryCSC
print()

print("Test: enable ['LAr']")
assert enableDetectors(flags, ['LAr'])
assert flags.Detector.EnableLAr
assert flags.Detector.EnableL1Calo
print()

print("Test: enable ['Forward']")
assert enableDetectors(flags, ['Forward'])
assert flags.Detector.EnableLucid
assert flags.Detector.EnableZDC
assert flags.Detector.EnableALFA
assert flags.Detector.EnableAFP
assert flags.Detector.EnableFwdRegion
print()


# test setup for Run 4
flags = flags_runs['ATLAS-P2-ITK-24-00-00']

print("Test: validate ['ITk', 'HGTD', 'Calo', 'Muon']")
assert not setupDetectorsFromList(flags, ['ITk', 'HGTD', 'Calo', 'Muon'], validate_only=True)
print()

print("Test: enable ['ITkStrip']")
assert setupDetectorsFromList(flags, ['ITkStrip'], toggle_geometry=True)
print()
