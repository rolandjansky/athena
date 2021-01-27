#run with e.g: Gen_tf.py --ecmEnergy=14000 --jobConfig=100000 --outputEVNTFile=EVNT.root --maxEvents=10
evgenConfig.description = "hdf5 read test"
evgenConfig.keywords = ["Z", "Jets", "SM"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
Pythia8_i.HDF5File = "/eos/home-a/afreegar/hdf5_files/Z1-g0-repack.hdf5"

PYTHIA8_nJetMax=1
PYTHIA8_TMS=30
PYTHIA8_Dparameter=0.2
PYTHIA8_Process="pp>e+e-"
PYTHIA8_nQuarksMerge=5
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
