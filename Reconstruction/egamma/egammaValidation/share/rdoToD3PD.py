# $Id$
#
# @file D3PDMakerConfig/share/AODToEgammaD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Example for building an egamma D3PD from an AOD.
#


###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-egamma/rtt/mc10_7TeV.106046.PythiaZee_no_filter.recon.ESD.e574_s933_s946_r1835_ESD.213775._000003.pool.root"]
# athenaCommonFlags.FilesInput= ["/build/atlas/rds/data/data11_7TeV.00179804.physics_Egamma.merge.AOD.f363_m806/data11_7TeV.00179804.physics_Egamma.merge.AOD.f363_m806._lb0287-lb0288._0001.1"]


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'egamma.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteEgammaD3PD = True
prodFlags.WriteEgammaD3PD.FileName = tupleFileOutput
prodFlags.WriteEgammaD3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1
#athenaCommonFlags.EvtMax = 5

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey    = 'GEN_AOD'
D3PDMakerFlags.ElectronSGKey = 'ElectronAODCollection'
D3PDMakerFlags.PhotonSGKey   = 'PhotonAODCollection'
#D3PDMakerFlags.DoTrigger = False


###################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec

rec.doESD.set_Value_and_Lock(True) # if false, all algorithms are switched off by defaults

rec.doCBNT.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False) 

rec.doMuon.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False) 

rec.DPDMakerScripts.append( "D3PDMakerConfig/EgammaD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doTrigger.set_Value_and_Lock(False) 
include( "RecExCommon/RecExCommon_topOptions.py" )

## ###################################################################
## # Add in egFilter
## #

## from egammaFilt.egammaFiltConf import egFilter
## my_egFilter = egFilter('egamma_egFilter')


## # Choose either Z or J/psi filtering (default is Z)
## my_egFilter.DoZFilt    = False
## my_egFilter.DoJpsiFilt = True
## my_egFilter.DoTrigger  = True  # default is to not do trigger
## # my_egFilter.OutputLevel= DEBUG

## # Add filter to sequence
## preseq  = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())
## preseq  += my_egFilter

## # And set as filter for both main and trigger streams
## EgammaD3PDStream.filterSeq += egFilter('egamma_egFilter')
## EgammaTriggerD3PDStream.filterSeq += egFilter('egamma_egFilter')
