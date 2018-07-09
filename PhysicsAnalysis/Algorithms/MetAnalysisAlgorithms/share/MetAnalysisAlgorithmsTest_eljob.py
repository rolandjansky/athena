#!/usr/bin/env python
#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# Read the submission directory as a command line argument. You can
# extend the list of arguments with your private ones later on.
import optparse
parser = optparse.OptionParser()
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir',
                   action = 'store', type = 'string', default = 'submitDir',
                   help = 'Submission directory for EventLoop' )
( options, args ) = parser.parse_args()

# Set up (Py)ROOT.
import ROOT
import os
ROOT.xAOD.Init().ignore()

# this forces the met algorithms dictionary to be loaded before
# anything else, which works around some strange dictionary issues I
# don't understand.
ROOT.CP.MetMakerAlg ("dummy", None)
ROOT.CP.MetSignificanceAlg ("dummy", None)

from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now
dataType = "data"
#dataType = "mc"
#dataType = "afii"

if not dataType in ["data", "mc", "afii"] :
    raise Exception ("invalid data type: " + dataType)

# Set up the sample handler object. See comments from the C++ macro
# for the details about these lines.
import os
sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )
sample = ROOT.SH.SampleLocal (dataType)
if dataType == "data" :
    sample.add (os.getenv ('ASG_TEST_FILE_DATA'))
    pass
if dataType == "mc" :
    sample.add (os.getenv ('ASG_TEST_FILE_MC'))
    pass
if dataType == "afii" :
    sample.add (os.getenv ('ASG_TEST_FILE_MC_AFII'))
    pass
sh.add (sample)
sh.printContent()

# Create an EventLoop job.
job = ROOT.EL.Job()
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 500 )

# Set up the systematics loader/handler algorithm:
sysLoader = AnaAlgorithmConfig( 'CP::SysListLoaderAlg/SysLoaderAlg' )
sysLoader.sigmaRecommended = 1
job.algsAdd( sysLoader )

# Set up a selection alg for demonstration purposes
# Also to avoid warnings from building MET with very soft electrons
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
selalg = createAlgorithm( 'CP::AsgSelectionAlg', 'METEleSelAlg' )
addPrivateTool( selalg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
selalg.selectionTool.minPt = 10e3
selalg.selectionTool.maxEta = 2.47
selalg.selectionDecoration = 'selectPtEta'
selalg.particles = 'Electrons'
# We need to copy here, because w/o an output container, it's assumed
# that the input container is non-const
selalg.particlesOut = 'DecorElectrons_%SYS%'
job.algsAdd( selalg )
print( selalg ) # For debugging

# Now make a view container holding only the electrons for the MET calculation
viewalg = createAlgorithm( 'CP::AsgViewFromSelectionAlg','METEleViewAlg' )
viewalg.selection = [ 'selectPtEta' ]
viewalg.input = 'DecorElectrons_%SYS%'
viewalg.output = 'METElectrons_%SYS%'
job.algsAdd( viewalg )
print( viewalg ) # For debugging

# Include, and then set up the met analysis algorithm sequence:
from MetAnalysisAlgorithms.MetAnalysisSequence import makeMetAnalysisSequence
# Touch the ObjectType enum to trigger dict loading
from ROOT import xAOD
xAOD.Type.ObjectType
metSequence = makeMetAnalysisSequence( dataType, metSuffix="AntiKt4EMTopo",
                                       jetContainer="AntiKt4EMTopoJets", jetSystematics="(^$)",
                                       components=[
                                        {"containerName":"Muons", "regex":"(^$)", "type":xAOD.Type.Muon, "termName":"RefMuon"},
                                        {"containerName":"METElectrons_%SYS%", "regex":"(^$)", "type":xAOD.Type.Electron, "termName":"RefEle"}] )
print( metSequence ) # For debugging

# Add all algorithms to the job:
for alg in metSequence:
    job.algsAdd( alg )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, options.submission_dir )
