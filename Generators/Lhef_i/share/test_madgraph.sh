# Jan Schumacher Mo 28 Jul 2008 17:08:28 CEST
#
# A simple test to make sure that Madgraph events can be processed.
# Assumes to be in subdir of LHEF_i.

# Sightly hackish, but it is only a test
cp ../share/lhefmadgraph.unweighted_events whizard.lhe.events

tar czf whizard.lhe.events.tar.gz whizard.lhe.events
csc_evgen_trf.py \
  runNumber=0000 \
  firstEvent=0 \
  maxEvents=9 \
  randomSeed=1234 \
  jobConfig=../share/jobOption.fragment.WhizardPythia.py \
  outputEvgenFile=madgraph.evgen.pool.root \
  inputGeneratorFile=whizard.lhe.events.tar.gz
