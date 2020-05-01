# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("TrackRecordGenerator.TrackRecordGeneratorConfig.getInput_TrackRecordGenerator", "TrackRecordGenerator")
addAlgorithm("TrackRecordGenerator.TrackRecordGeneratorConfig.getTrackRecordCosmicGenerator", "TrackRecordCosmicGenerator")
