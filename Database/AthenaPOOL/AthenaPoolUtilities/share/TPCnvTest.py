##
# $Id$
#
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

if (not globals().has_key ('ATLAS_REFERENCE_TAG') and
    os.environ.has_key ('ATLAS_REFERENCE_TAG')):
    ATLAS_REFERENCE_TAG = os.environ['ATLAS_REFERENCE_TAG']

refpaths = [os.environ.get ('ATLAS_REFERENCE_DATA', None),
            '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art',
            '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs',
            '/afs/cern.ch/atlas/maxidisk/d33/referencefiles']
if infile.startswith ('rtt:'):
    infile = infile[4:]


def find_file (fname):
    for p in refpaths:
        if p:
            path = os.path.join (p, fname)
            if os.path.exists (path):
                return path
    print 'ERROR: Cannot find file: ', fname
    return None

svcMgr.EventSelector.InputCollections        = [ find_file (infile) ]

from AthenaCommon.DetFlags      import DetFlags
if not globals().get ('noMuon',False):
    DetFlags.detdescr.Muon_setOn()
if not globals().get ('noID',False):
    DetFlags.detdescr.ID_setOn()

from AthenaCommon.JobProperties import jobproperties
if jobproperties.Global.DetDescrVersion.isDefault():
    jobproperties.Global.DetDescrVersion = 'ATLAS-R1-2012-03-01-00'
import imp
have_atlas_geo = True
try:
    imp.find_module ('AtlasGeoModel')
except ImportError:
    have_atlas_geo = False
if have_atlas_geo:
    import AtlasGeoModel.GeoModelInit
    import AtlasGeoModel.SetGeometryVersion
    svcMgr.GeoModelSvc.IgnoreTagDifference = True

    #svcMgr.GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].HasSTgc = False

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from TrkEventCnvTools import TrkEventCnvToolsConf
EventCnvSuperTool = TrkEventCnvToolsConf.Trk__EventCnvSuperTool('EventCnvSuperTool')
EventCnvSuperTool.MuonCnvTool.IdHelperTool.HasSTgc = (CommonGeometryFlags.Run() in ["RUN3", "RUN4"])
EventCnvSuperTool.MuonCnvTool.IdHelperTool.HasMM = (CommonGeometryFlags.Run() in ["RUN3", "RUN4"])
ToolSvc += EventCnvSuperTool

#
# If a new xAOD variable appears, print a warning, but don't treat
# it as a failure.
#
def checknewvars (output):
    names = set()
    for l in output.split ('\n'):
        if not l: continue
        if l.startswith ('+++') or l.startswith ('---'): continue
        if l[0] == '-': return None
        if l[0] == '+':
            pos = l.find (':')
            if l.startswith ('+     ') and pos > 0:
                names.add (l[6:pos])
            else:
                return None
    l = list(names)
    l.sort()
    return l
            

if not globals().has_key ('get_dumper_fct'):
    from PyDumper.Dumpers import get_dumper_fct
from AthenaPython import PyAthena
class Dumper (PyAthena.Alg):
    def __init__ (self, name):
        PyAthena.Alg.__init__ (self, name)
        return

    def initialize (self):
        self.sg = PyAthena.py_svc ('StoreGateSvc')
        self.ofile_name = os.path.basename (infile) + '.dump'
        refbase = os.path.basename (infile) + '.ref'
        self.reffile_name = '../share/' + refbase
        if not os.path.exists (self.reffile_name):
            self.reffile_name = '../' + self.reffile_name

        if not os.path.exists (self.reffile_name) and globals().has_key ('ATLAS_REFERENCE_TAG'):
            from AthenaCommon.Utils.unixtools import find_datafile
            r = find_datafile (ATLAS_REFERENCE_TAG)
            if r:
                self.reffile_name = os.path.join (r, ATLAS_REFERENCE_TAG,
                                                  refbase)

        if not os.path.exists (self.reffile_name) and globals().has_key ('ATLAS_REFERENCE_TAG'):
            self.reffile_name = find_file (os.path.join (ATLAS_REFERENCE_TAG,
                                                         refbase))

        self.ofile = open (self.ofile_name, 'w')
        self.icount = 0
        return 1

    def finalize (self):
        self.ofile.close()
        ret, output = subprocess.getstatusoutput ('diff -u %s %s' % (self.reffile_name, self.ofile_name))
        if ret != 0:
            newvars = checknewvars (output)
            if newvars:
                print 'WARNING: new xAOD variables ', newvars
            else:
                print 'ERROR running diff with reference'
                print output
        return 1

    def execute (self):
        print >> self.ofile, 'Event index', self.icount
        self.icount += 1
        for k in keys:
            nmax = None
            apos = k.find ('@')
            if apos >= 0:
                nmax = int (k[apos+1:])
                k = k[:apos]
            print >> self.ofile, '-->', k

            store = self.sg
            spos = k.find ('/')
            if spos >= 0:
                store = PyAthena.py_svc (k[:spos])
                k = k[spos+1:]
            
            spos = k.find ('#')
            if spos >= 0:
                (typ, k) = k.split ('#')
                o = store.retrieve (typ, k)
            else:
                o = store[k]
            dumper = get_dumper_fct (type (o), self.ofile, nmax = nmax)
            dumper (o)
        print >> self.ofile, '\n'
        return 1

dumper = Dumper ('dumper')
topSequence += [dumper]
