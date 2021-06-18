# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This script is an example of how to run jet algorithms in AnalysisBase.
# Jet algs are configured wth the r22 new -style configuration.
# 
# simply invoke it with
#  python test_AnalysisBaseEventLoopJob.py
#
# WARNING : this is experimental. Pay attention to the TEMPORARY comments : these are still needed to run the job.
import ROOT, os

ROOT.JetSorter # TEMPORARY : ask ROOT/cppyy to load simple jet tool first. Otherwise loading other tools might result in crash.



inputFilePath = "/eos/atlas/atlascerngroupdisk/trig-jet/AOD_AthenaMTvalidations/11thRound/ttbar_withPileup/valid1/"
filePattern = 'AOD.24230077._000510.pool.root.1'

# if the above input file from trigger community disapear, we can use the one below from the tutorials :
#inputFilePath = '/cvmfs/atlas.cern.ch/repo/tutorials/asg/cern-nov2020/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_PHYS.e6337_s3126_r10201_p4172/'
#filePattern = 'DAOD_PHYS.21569875._001323.pool.root.1'


#****************************************************
# the athena-like configuration

# This makes all the Athena config module available :
import JetRecConfig.JetAnalysisCommon

from JetRecConfig.JetRecConfig import JetRecCfg
from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4Truth


# TEMPORARY : we need to run a simplistic AnaAlgorithm (and NOT a AnaReentrantAlgorithm like jet algs) as the
# 1st alg and this alg has to access a ReadHandle. Otherwise AnaReentrantAlgorithm will crash when accessing ReadHandle with "no active event".
from AthenaConfiguration.ComponentFactory import CompFactory
aa = CompFactory.TCAlg('AA')


# We need to setup the flags according to our input file :
ConfigFlags = JetRecConfig.JetAnalysisCommon.setupFlags(inputFiles=[inputFilePath+filePattern])

# Schedule 2 jet containers
# truth jets :
acc=JetRecCfg(ConfigFlags, AntiKt4Truth )

# we re-define AntiKt4LCTopo adding a suffix to the output and the modifiers :
myAk4LCtopo = AntiKt4LCTopo.clone(suffix="ReDone",
                                  modifiers = ('ConstitFourMom', 'CaloEnergies', 'Calib:T0:mc', 'Sort', 'Filter:10000', 'LArHVCorr', 'Width', 'CaloQuality',  'PartonTruthLabel', 'TruthPartonDR', 'JetDeltaRLabel:5000', 'ECPSFrac', 'ClusterMoments')
                                  # tracks moments not working yet 
                                  # 'TrackMoments', 'TrackSumMoments', 'JVF', 'JVT', 'OriginSetPV', 'Charge',
                                  )

# generate the config and append it to acc :
acc.merge(JetRecCfg( ConfigFlags, myAk4LCtopo ))

# acc contains all the algs (inputs + jet finding) needed. Just set debug mode for the last one : 
acc.algs[-1].OutputLevel = 2




#****************************************************
# EL::Job  configuration

sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )
ROOT.SH.ScanDir().filePattern( filePattern ).scan( sh, inputFilePath )
    
sh.printContent()


job = ROOT.EL.Job()
job.options().setString( ROOT.EL.Job.optSubmitDirMode, 'unique-link')
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 5 )



job.addManyAlgs( [aa] ) # TEMPORARY : schedule the simplistic alg first
# schedule our jet algs to the EL job.
job.addManyAlgs( acc )


# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, "out")
