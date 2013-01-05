# $Id: AODToZPrimeMMD3PD.py 522215 2012-10-18 18:34:05Z jcogges $
#
# @file ExoticsD3PDMaker/share/AODToZPrimeMMD3PD.py
# @author Hulin Wang <Hulin.Wang@cern.ch>
# @author James Coggeshall <jcogges2@illinois.edu>
# @date April 2012
# @brief Example for building an Exotics ZPrimeMM D3PD from an AOD.
#


###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if not globals().get('inputFiles'):
#    athenaCommonFlags.FilesInput= ["AOD.pool.root"]
    athenaCommonFlags.FilesInput= ["/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0241._0001.1"]
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0243._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0250._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0278._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0279._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0296._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0378._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0424-lb0425._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0440-lb0441._0001.1",
#                                   "/mnt/data/jcogges/data/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126/data12_8TeV.00201113.physics_Muons.merge.AOD.f437_m1126._lb0547-lb0548._0001.1"]
else:
    athenaCommonFlags.FilesInput= inputFiles


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'zprimemm.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteZPrimeMMD3PD = True
prodFlags.WriteZPrimeMMD3PD.FileName = tupleFileOutput
prodFlags.WriteZPrimeMMD3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = 1000

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.DoTrigger = False


###################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "ExoticsD3PDMaker/ZPrimeMMD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
include( "RecExCommon/RecExCommon_topOptions.py" )
