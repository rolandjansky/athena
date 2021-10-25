
assert hasattr(genSeq, "Herwig7")

# THEP8I_PATH is set during compilation, now add to LD_LIBRARY_PATH when running event generation with the TheP8I plugin
TheP8Ipath=os.environ['THEP8IPATH']
os.environ["LD_LIBRARY_PATH"]=TheP8Ipath+"/lib64/ThePEG"+":"+TheP8Ipath+"/lib/ThePEG"+":"+os.environ["LD_LIBRARY_PATH"]

try:
      TheP8I_PYTHIA8DATA=os.environ['PYTHIA8DATA']
except:
      TheP8I_PYTHIA8DATA=os.environ['PY8PATH'] + '/share/Pythia8/xmldoc/'
os.environ['PYTHIA8DATA']=TheP8I_PYTHIA8DATA

Herwig7Config.add_commands("""
mkdir /TheP8I
create TheP8I::StringFragmentation  /TheP8I/Handlers/StringFrag/StringFragmenter    libTheP8I.so
create TheP8I::TheP8IStrategy  /TheP8I/StdStrategy  libTheP8I.so
cd /Herwig/Generators
set EventGenerator:EventHandler:HadronizationHandler /TheP8I/Handlers/StringFrag/StringFragmenter
set EventGenerator:Strategy /TheP8I/StdStrategy
""")
