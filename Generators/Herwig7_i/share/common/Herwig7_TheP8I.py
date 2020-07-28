
assert hasattr(genSeq, "Herwig7")

#The path is hardcoded, exactly in a way that is done for OpenLoops
try:
# This should be a prefix      
      TheP8I_path=os.environ['THEP8I_PATH']
except:
      TheP8I_path='/cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/thep8i/2.0.0/x86_64-slc6-gcc62-opt'
os.environ["LD_LIBRARY_PATH"]=TheP8I_path+"/lib64/ThePEG"+":"+TheP8I_path+"/lib/ThePEG"+":"+os.environ["LD_LIBRARY_PATH"]

try:
      TheP8I_PYTHIA8DATA=os.environ['PYTHIA8DATA']
except:
      TheP8I_PYTHIA8DATA='/cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/pythia8/244/x86_64-slc6-gcc62-opt/share/Pythia8/xmldoc/'
os.environ['PYTHIA8DATA']=TheP8I_PYTHIA8DATA



Herwig7Config.add_commands("""
mkdir /TheP8I
create TheP8I::StringFragmentation  /TheP8I/Handlers/StringFrag/StringFragmenter    libTheP8I.so
create TheP8I::TheP8IStrategy  /TheP8I/StdStrategy  libTheP8I.so
cd /Herwig/Generators
set EventGenerator:EventHandler:HadronizationHandler /TheP8I/Handlers/StringFrag/StringFragmenter
set EventGenerator:Strategy /TheP8I/StdStrategy
""")
