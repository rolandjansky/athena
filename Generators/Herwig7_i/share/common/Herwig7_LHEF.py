# initialize Herwig7 generator configuration object for showering of LHE files
include("EvgenProdTools/merge_lhe_files.py")

from Herwig7_i.Herwig7_iConf import Herwig7 
from Herwig7_i.Herwig7ConfigLHEF import Hw7ConfigLHEF

genSeq += Herwig7()
Herwig7Config = Hw7ConfigLHEF(genSeq, runArgs)

# handle compressed LHE files passed via runArgs.inputGeneratorFile
import glob
from AthenaCommon import Logging
athMsgLog = Logging.logging.getLogger('Herwig7_i/Herwig7_LHEF.py')
if ".tar.gz" in runArgs.inputGeneratorFile or ".tgz" in runArgs.inputGeneratorFile:
  athMsgLog.info("inputGeneratorFile '{}' is compressed - will look for uncompressed LHE file".format(runArgs.inputGeneratorFile))
  lhe_files = glob.glob("*._*.ev*ts")
  athMsgLog.info("Number of lhe files {}".format(len(lhe_files)))
  if len(lhe_files) == 0:
    raise RuntimeError("Could not find uncompressed LHE file")
  if len(lhe_files) > 1:
    athMsgLog.info("Found more than one uncompressed LHE file: {}".format(lhe_files))
#    my_lhe_file = "merged_lhef.events"
# skeleton.GENtoEVGEN splits the file name on "._" (in rel. 20.7.9.9.6,MCProd,
# so insert this in the merged file name - to make it run also for this release)
    my_lhe_file = "merged_lhef._0.events"
    merge_lhe_files(lhe_files, my_lhe_file )
    lhe_files[0] = my_lhe_file
  athMsgLog.info("Using uncompressed LHE file '{}' as inputGeneratorFile".format(lhe_files[0]))
  runArgs.inputGeneratorFile = lhe_files[0]

