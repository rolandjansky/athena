#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList

ConfigFlags._loadDynaFlags('GeoModel')
ConfigFlags._loadDynaFlags('Detector')

test_tags = [
    'ATLAS-R1-2012-03-01-00',   # example Run 1
    'ATLAS-R2-2016-01-00-01',   # example Run 2
    'ATLAS-R3S-2021-01-00-01',  # example Run 3
    'ATLAS-P2-ITK-23-00-03'     # example Run 4
]

flags_runs = {t: ConfigFlags.clone() for t in test_tags}

for tag in test_tags:
    flags = flags_runs[tag]
    flags.GeoModel.AtlasVersion = tag

    print(f'{flags.GeoModel.AtlasVersion} - {flags.GeoModel.Run}')
    flags.dump('Detector.(Geometry|Enable)', True)
    print()

    # test flags that change with time
    assert flags.Detector.EnableBCM == (tag != 'ATLAS-P2-ITK-23-00-03')
    assert flags.Detector.EnableDBM == (tag in ['ATLAS-R2-2016-01-00-01', 'ATLAS-R3S-2021-01-00-01'])
    assert flags.Detector.EnablePixel == (tag != 'ATLAS-P2-ITK-23-00-03')
    assert flags.Detector.EnableSCT == (tag != 'ATLAS-P2-ITK-23-00-03')
    assert flags.Detector.EnableTRT == (tag != 'ATLAS-P2-ITK-23-00-03')
    assert flags.Detector.EnableITkPixel == (tag == 'ATLAS-P2-ITK-23-00-03')
    assert flags.Detector.EnableITkStrip == (tag == 'ATLAS-P2-ITK-23-00-03')
    assert flags.Detector.EnableCSC == (tag in ['ATLAS-R1-2012-03-01-00', 'ATLAS-R2-2016-01-00-01'])
    assert flags.Detector.EnablesTGC == (tag in ['ATLAS-R3S-2021-01-00-01', 'ATLAS-P2-ITK-23-00-03'])
    assert flags.Detector.EnableMM == (tag in ['ATLAS-R3S-2021-01-00-01', 'ATLAS-P2-ITK-23-00-03'])

# test setup for Run 2
flags = flags_runs['ATLAS-R2-2016-01-00-01']

assert not setupDetectorsFromList(flags, ['ID', 'Calo', 'Muon'], validate_only=True)
print()
assert setupDetectorsFromList(flags, ['Muon'], validate_only=True)
print()
assert setupDetectorsFromList(flags, ['Muon'])
print()
assert setupDetectorsFromList(flags, ['ID'], toggle_geometry=True)
print()
