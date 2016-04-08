#
# $Id$
#
# File: D3PDMakerTest/share/D3PDMakerTest1_jo.py
# Author: snyder@bnl.gov
# Date: Jan, 2010
# Purpose: Test D3PD making.
#

## basic job configuration
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


import D3PDMakerTest
topSequence += [ D3PDMakerTest.FillerAlg(),
                 D3PDMakerTest.HitsFillerAlg() ]


from D3PDMakerRoot import D3PDMakerRootConf
d3pdsvc = D3PDMakerRootConf.D3PD__RootD3PDSvc()
svcMgr += d3pdsvc
d3pdsvc.MasterTree = ''
d3pdsvc.IndexMajor = ''
d3pdsvc.IndexMinor = ''

import D3PDMakerCoreComps
import EventCommonD3PDMaker
from EventCommonD3PDMaker.DRIndexAssociation      import DRIndexAssociation
from EventCommonD3PDMaker.DRIndexMultiAssociation import DRIndexMultiAssociation

alg = D3PDMakerCoreComps.MakerAlg ('test1', topSequence, 'test1.root')

from D3PDMakerCoreComps.D3PDObject import \
     make_SGDataVector_D3PDObject, make_SG_D3PDObject, make_Void_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation \
     import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.ContainedMultiAssociation \
     import ContainedMultiAssociation


###########################################################################


Obj1D3PDObject = \
           make_SGDataVector_D3PDObject ('D3PDTest::Obj1Container',
                                         'obj1container',
                                         'o1_')
Obj1D3PDObject.defineBlock (0, 'Obj1',  D3PDMakerTest.Obj1FillerTool)
Obj1D3PDObject.defineBlock (0, 'Obj1a', D3PDMakerTest.Obj12FillerTool)
Obj1D3PDObject.defineBlock (0, 'Def2', D3PDMakerTest.DefaultFillerTool2)


#####################


Obj2aAssoc = SimpleAssociation (Obj1D3PDObject,
                                D3PDMakerTest.Obj1Obj2AssociationTool,
                                prefix = 'o2a_')
Obj2aAssoc.defineBlock (0, 'Obj2Assoc', D3PDMakerTest.Obj2FillerTool)

Obj2bAssoc = ContainedVectorMultiAssociation \
             (Obj1D3PDObject,
              D3PDMakerTest.Obj1Obj2MultiAssociationTool,
              prefix = 'o2b_')
Obj2bAssoc.defineBlock (0, 'Obj2', D3PDMakerTest.Obj2FillerTool)
Obj2bAssoc.defineBlock (0, 'Obja2', D3PDMakerTest.Obj12FillerTool)

Obj2cAssoc = SimpleAssociation (Obj1D3PDObject,
                                D3PDMakerTest.ToObj2AssociationTool,
                                prefix = 'o2c_')
Obj2cAssoc.defineBlock (0, 'Obj2Assoc_c', D3PDMakerTest.Obj2FillerTool)

Obj2dAssoc = SimpleAssociation (Obj2cAssoc,
                                D3PDMakerTest.ToObj2AssociationTool,
                                prefix = 'o2d_',
                                blockname = 'Assoc_d')
Obj2dAssoc.defineBlock (0, 'Obj2Assoc_d', D3PDMakerTest.Obj2FillerTool)

Obj2eAssoc = ContainedMultiAssociation (Obj2cAssoc,
                                D3PDMakerTest.ToObj2MultiAssociationTool,
                                prefix = 'o2e_',
                                blockname = 'Assoc_e')
Obj2eAssoc.defineBlock (0, 'Obj2Assoc_e', D3PDMakerTest.Obj2FillerTool)


Obj1D3PDObject2 = \
           make_SGDataVector_D3PDObject ('D3PDTest::Obj1Container',
                                         'obj1',
                                         'o12_')
Obj1D3PDObject2Assoc1 = ContainedMultiAssociation \
                        (Obj1D3PDObject2,
                         D3PDMakerTest.Obj1Obj2MultiAssociationTool,
                         blockname = 'Obj1D3PDObject2Assoc1',
                         prefix = 'o2a_')
Obj1D3PDObject2Assoc1.defineBlock (0, 'Obj2a', D3PDMakerTest.Obj2FillerTool)
Obj1D3PDObject2Assoc2 = ContainedMultiAssociation \
                        (Obj1D3PDObject2,
                         D3PDMakerTest.Obj1Obj2MultiAssociationTool,
                         Which = 1,
                         blockname = 'Obj1D3PDObject2Assoc2',
                         prefix = 'o2b_')
Obj1D3PDObject2Assoc2.defineBlock (0, 'Obj2b', D3PDMakerTest.Obj2FillerTool)

Obj1D3PDObject2Assoc1_c = ContainedMultiAssociation \
                          (Obj1D3PDObject2,
                           D3PDMakerTest.ToObj2MultiAssociationTool,
                           blockname = 'Obj1D3PDObject2Assoc1_c',
                           prefix = 'o2c_')
Obj1D3PDObject2Assoc1_c.defineBlock (0, 'Obj2c', D3PDMakerTest.Obj2FillerTool)


#####################


Obj3D3PDObject = \
       make_SGDataVector_D3PDObject ('DataVector<INavigable4Momentum>',
                                     'obj3container',
                                     'o3_')
Obj3D3PDObject.defineBlock (0, 'Obj3Kin',
                            EventCommonD3PDMaker.FourMomFillerTool)
Obj3HLV = SimpleAssociation (Obj3D3PDObject,
                             D3PDMakerTest.IN4MHLVAssociationTool,
                             prefix = 'hlv_',
                             blockname = 'Assoc_hlv')
Obj3HLV.defineBlock (0, 'Obj3Kin2',
                     EventCommonD3PDMaker.FourMomFillerTool)

DRIndexMultiAssociation (Obj3D3PDObject,
                         'DataVector<INavigable4Momentum>',
                         'obj3container',
                         1.0,
                         target = 'o3_')
DRIndexAssociation (Obj3D3PDObject,
                    'DataVector<INavigable4Momentum>',
                    'obj3container',
                    1.0,
                    prefix = 'a1_',
                    target = 'o3_')



#####################


def make_obj4 (name, prefix, object_name):
    getter = D3PDMakerTest.Obj4GetterTool (name + '_Getter',
                                           SGKey = 'obj4container')
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name)

Obj4D3PDObject = D3PDMakerCoreComps.D3PDObject.D3PDObject (make_obj4, 'o4_')
Obj4D3PDObject.defineBlock (0, 'Obj4', D3PDMakerTest.Obj4FillerTool)


#####################


Obj5D3PDObject = \
       make_SGDataVector_D3PDObject ('DataVector<D3PDTest::Obj5>',
                                     'obj5container',
                                     'o5_')

Obj5D3PDObject.defineBlock (0, 'Obj5', D3PDMakerTest.Obj5FillerTool)
Obj5D3PDObject.defineBlock (0, 'Obj5aux', D3PDMakerCoreComps.AuxDataFillerTool,
                            Vars = ['anInt',
                                    'aFloat',
                                    's = aString #This is a string.',
                                    'aFourvec',
                                    'dummy < int:-999'])


###########################################################################



alg += Obj1D3PDObject(99)
alg += Obj1D3PDObject2(99)
alg += Obj1D3PDObject(99,
                      prefix = 'o1filt_',
                      getterFilter = 'obj1sel')
alg += Obj3D3PDObject (99)
alg += Obj4D3PDObject(99)
alg += Obj5D3PDObject(99)

DefD3PDObject = make_Void_D3PDObject ('def_')
DefD3PDObject.defineBlock (0, 'Def', D3PDMakerTest.DefaultFillerTool)
alg += DefD3PDObject(99)

# Testing SimHitsCollection.
from MuonD3PDMaker.MDTSimHitD3PDObject import MDTSimHitD3PDObject
alg += MDTSimHitD3PDObject (99, sgkey = 'MDTSimHits')

from MuonD3PDMaker.TrackRecordD3PDObject import TrackRecordD3PDObject
alg += TrackRecordD3PDObject (99, sgkey = 'TrackRecord',
                              exclude = ['TruthHits'])

from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
AANTupleStream('D3PD').ExistDataHeader = False


def dumpit():
    import os
    os.system ('python -m D3PDMakerTest.dumptuple1 test1.root')
import atexit
atexit.register (dumpit)


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = 3

# No stats printout
from GaudiCommonSvc.GaudiCommonSvcConf import ChronoStatSvc
chronoStatSvc = ChronoStatSvc()
chronoStatSvc.ChronoPrintOutTable = FALSE
chronoStatSvc.PrintUserTime       = FALSE
chronoStatSvc.StatPrintOutTable   = FALSE

# Elide files/errors in log messages.
import ROOT
import cppyy
ROOT.errorcheck.ReportMessage.hideErrorLocus (True)

theApp.getHandle().Dlls += [ 'D3PDMakerRoot']
theApp.getHandle().Dlls += [ 'D3PDMakerCoreComps']


