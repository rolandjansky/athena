# Call as UserAlgs

from JetRec.JetRecConf import *
from JetRecTools.JetRecToolsConf import *

# schedule a jet algorithm
h1topotowerJet = make_StandardJetGetter('AntiKt',0.4,'TopoTower').jetAlgorithmHandle()
h1topotowerJet.InputCollectionNames = [ "TopoTowerJets" ] 
h1topotowerJet.ForceJetCopy = False 
h1topotowerJet.OutputLevel = VERBOSE

# instantiate a IJetCalibrator tool :
offsetTool = JetCalibrationTool("CalibDummy")
offsetTool.Calibrator = OffsetCorrection("TopoTowerOffset")
offsetTool.Calibrator.CalibrationTag     = "TowerOffset"
offsetTool.Calibrator.VertexContainerKey = "VxPrimaryCandidate"
offsetTool.Calibrator.MinimumPVTracks    = 2
offsetTool.Calibrator.UseTowersPerJet    = True

# Setup the Eta binning to be used
array = [(i/10.) for i in range(-50,51)]
offsetTool.Calibrator.EtaBins = array

# Enter Eta Bins: 93 bins from -4.65 to 4.55
array = [((i/10.)+0.55) for i in range(-52,42) ]
jetcalibrationtool.Calibrator.EtaBins = array 

# Pileup parameters: bunch spacing [ns] and lumi [1E33]
bs   = jobproperties.Beam.bunchSpacing()
lumi = jobproperties.Beam.estimatedLuminosity()

# Enter Calib Constants... Just Cut and Paste from the website and format
# Those are the constants for Cone4TowerJets
jetcalibrationtool.Calibrator.BunchSpacing   = bs
jetcalibrationtool.Calibrator.Luminosity     = lumi

# Keep constants in a separate file (they are unwieldy)
include("JetCalibTools/TowerBasedOffset_corrections.py")
if (bs==25  and lumi>=1.9E33 and lumi<=2.1E33):
  offsetTool.Calibrator.CalibConstants = constants_2E33_25ns
elif (bs==450 and lumi>=0.9E32 and lumi<=1.1E32):
  offsetTool.Calibrator.CalibConstants = constants_1E32_450ns    
else:
  print "This luminosity and bunch spacing is not supported for the offset correction" 
  print "Luminosity (cm^-2 s^-1) = %f" % lumi
  print "Bunch spacing (ns)      = %d" % bs 
 
# add it to our jet alg :
h1topotowerJet.AlgTools += [offsetTool]


