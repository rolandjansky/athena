# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# database entries for https://twiki.cern.ch/twiki/bin/view/AtlasComputing/ConfiguredFactory#Factory_functions_vs_derived_cla
# Valerio Ippolito - Harvard University
from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm('MagFieldServices.MagFieldServicesSetup.GetFieldMapCondAlg', 'AtlasFieldMapCondAlg')
addAlgorithm('MagFieldServices.MagFieldServicesSetup.GetFieldCacheCondAlg', 'AtlasFieldCacheCondAlg')
