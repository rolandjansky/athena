# Run a copy of this file (here named MC15.999999.LhefPy8_Example.py) like
# Generate_tf.py --inputGeneratorFile=Zp2200-S95A40.lhe.events --runNumber=999999 --jobConfig=MC15.999999.LhefPy8_Example.py --ecmEnergy=13000 --maxEvents=100 --randomSeed=1234 --outputEVNTFile=test.pool.root
# Note that your input file's name should end with .events to be compatible with what the transform expects

# Short description. Goes into AMI - make this useful!
evgenConfig.description = "LHEF Reading example"
# Must be among the allowed keywords
evgenConfig.keywords = ["exotic"]
# Your name here
evgenConfig.contact = ['Clever Physicist <Albert.Einstein@cern.ch>']

# Generic pythia8 base fragment - feel free to include a specific tune!
include("Pythia8_i/Pythia8_Base_Fragment.py")
# Tell the job that we are using an LHEF input
include("Pythia8_i/Pythia8_LHEF.py")
# Specify the LHEF generator - here generic, but specific is better
evgenConfig.generators += ["Lhef"]
