#
# configuration file for upgrade pileup scripts.
# put a copy of this file in the output directory,
# then run the scripts from that directory.
#
# For instance, for generating minbias files in a
# directory under the InDetSLHC_Example hierarchy:
# % mkdir mbg
# % cd mbg
# % cp ../share/config-job.sh .
# % ../share/submit_simulation_pileup.sh 1 10
#
#==============================================================================
# general configuration
#==============================================================================
#
# LAYOUT:
#   "SLHC" alone refers to whatever is in the default database
#     (currently ATLAS-SLHC-01-00-00, or SLHC-19-20-dev1)
#   otherwise use a tag like "SLHC-19-20"
#
export LAYOUT="SLHC"

#
# execution mode:
#   PBS
#   LSF (CERN)
#   INTERACTIVE
#   TEST
#
export EXEC_MODE="INTERACTIVE"
#export EXEC_MODE="TEST"

#==============================================================================
# pileup simulation configuration
#==============================================================================
# prefix for minbias output files
export P_JOBNAME="minbiasgen"
# prefix for job names in batch queue
export P_QUEUENAME="mbg"
# number of events per minbias output file
export P_EVTMAX=100

#==============================================================================
# signal simulation configuration (multiple particles)
#==============================================================================
# prefix for signal output files
export S_JOBNAME="multimu"
# prefix for job names in batch queue
export S_QUEUENAME="multi"
# number of events per signal output file
export S_EVTMAX=1000
# particle ID for generation:  both + and - versions are generated
export PDGID=13
# eta range of signal particles
export ETAMIN="-2.5"
export ETAMAX="2.5"

#==============================================================================
# digitization configuration
#==============================================================================
# prefix for digitization output files
export D_JOBNAME="mudig"
# prefix for job names in batch queue
export D_QUEUENAME="pmu"
# number of events per digitized output file
# (signal files are read in batches of this number of events)
export D_EVTMAX=25
# path to list of minbias files
export PILEUPLIST="/data/slhc/tseng/sl5/t16/InnerDetector/InDetExample/InDetSLHC_Example/mbg-SLHC/mbg-list.txt"

#==============================================================================
# tracking configuration
#==============================================================================
# prefix for tracking output files
export T_JOBNAME="mutrk"
# prefix for job names in batch queue
export T_QUEUENAME="tmu"
# maximum number of events per tracking output file
export T_EVTMAX=1000

