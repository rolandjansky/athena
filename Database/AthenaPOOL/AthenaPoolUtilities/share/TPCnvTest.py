# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
##
# @file AthenaPoolUtilities/share/TPCnvTest.py
# @author sss
# @date Nov 2015
# @brief Helper for testing reading old data files.
#
# This JO can be used to test the reading of an old data file.
#
# Set the variable `infile' to the file to be read.
# If this is not an absolute path, it is looked for under
#  /afs/cern.ch/atlas/maxidisk/d33/referencefiles.
# Set the variable keys to the list of SG keys to dump.
# A dump will be made to the file given by appending `.dump'
# to the base part of infile.
# The dump will be compared against a reference file in share.
#

import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PyUtils.moduleExists import moduleExists
from PyUtils.fprint import fprintln

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool

from future import standard_library
standard_library.install_aliases()
import subprocess

# Make sure we don't have a stale file catalog.
import os
if os.path.exists ('PoolFileCatalog.xml'):
    os.remove ('PoolFileCatalog.xml')

if (not 'ATLAS_REFERENCE_TAG' in globals() and
    'ATLAS_REFERENCE_TAG' in os.environ):
    ATLAS_REFERENCE_TAG = os.environ['ATLAS_REFERENCE_TAG']

refpaths = [os.environ.get ('ATLAS_REFERENCE_DATA', None),
            '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art',
            '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs',
            '/afs/cern.ch/atlas/maxidisk/d33/referencefiles']
if infile.startswith ('rtt:'):
    infile = infile[4:]


from AthenaPoolUtilities.DumperConfig import find_file, Dumper
svcMgr.EventSelector.InputCollections        = [ find_file (infile, refpaths) ]

from AthenaCommon.DetFlags      import DetFlags
if not globals().get ('noMuon',False):
    DetFlags.detdescr.Muon_setOn()
    if moduleExists ('MuonRecExample'):
        import MuonRecExample.MuonAlignConfig
if not globals().get ('noID',False):
    DetFlags.detdescr.ID_setOn()

from AthenaCommon.JobProperties import jobproperties
if jobproperties.Global.DetDescrVersion.isDefault():
    jobproperties.Global.DetDescrVersion = 'ATLAS-R1-2012-03-01-00'
have_atlas_geo = moduleExists ('AtlasGeoModel')
if have_atlas_geo:
    import AtlasGeoModel.GeoModelInit
    import AtlasGeoModel.SetGeometryVersion
    svcMgr.GeoModelSvc.IgnoreTagDifference = True

if have_atlas_geo and moduleExists ('TrkEventCnvTools') and moduleExists ('MuonEventCnvTools'):
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    from TrkEventCnvTools import TrkEventCnvToolsConf
    EventCnvSuperTool = TrkEventCnvToolsConf.Trk__EventCnvSuperTool('EventCnvSuperTool')
    from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperSvc
    svcMgr += Muon__MuonIdHelperSvc("MuonIdHelperSvc",HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM())
    ToolSvc += EventCnvSuperTool

dumper = Dumper ('dumper', infile, keys, refpaths)
topSequence += [dumper]
