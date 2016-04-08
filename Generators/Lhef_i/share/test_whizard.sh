# Jan Schumacher Mo 28 Jul 2008 17:08:28 CEST
#
# A simple test to make sure that Whizard events can be processed.
# Assumes to be in subdir of LHEF_i.

tar czf whizard.lhe.events.tar.gz ../share/whizard.lhe.events
csc_evgen_trf.py \
  runNumber=0000 \
  firstEvent=0 \
  maxEvents=9 \
  randomSeed=1234 \
  jobConfig=../share/jobOption.fragment.WhizardPythia.py \
  outputEvgenFile=whizard.evgen.pool.root \
  inputGeneratorFile=whizard.lhe.events.tar.gz
