#specify the following with this fragment:
#proc_name : the name of the process - will match the name of the process directory
#proc_card : the proc_card string without including the output line
#param_card : the LOCATION of the param card to use (can be retrieved with get_files)
#param_card_extras : dictionary of dictionaries
#run_card : the LOCATION of the param card to use (can be retrieved with get_files)
#run_card_extras: dictionary ... should not specify pdlabel and lhaid, as those are provided in this fragment

from MadGraphControl import MadGraphUtils

#check the variables, adding bits if necessary:
try:
   proc_card
except NameError:
   raise RuntimeError("You must specify a proc_card string")

try:
   proc_name
except NameError:
   proc_name = ""

old_proc_card = proc_card
proc_card = ""
for line in old_proc_card.splitlines():
   if "output" in line:
      print("Ignoring output line: %s" % line)
   else: proc_card += line + '\n'

try:
   param_card
except NameError:
   print("No param_card given, will use the one from the process directory..");
   param_card = ""


try:
   param_card_extras
except NameError:
   param_card_extras={}

try:
   run_card
except NameError:
   print("No run_card given, will use the default one for this process (LO: $MADPATH/Template/LO/Cards/run_card.dat)");
   run_card = ""

try:
   run_card_extras
except NameError:
   run_card_extras = {}


#modify the run_card_extras 
if 'pdlabel' in run_card_extras:
   raise RuntimeError("Cannot specify a pdlabel in run_card_extras, as this is set for you")

if 'lhaid' in run_card_extras:
   raise RuntimeError("Cannot specify a lhaid in run_card_extras, as this is set for you")

run_card_extras['pdlabel'] = 'lhapdf'
run_card_extras['lhaid'] = '247000'#NNDF23_lo_as_0130_qed pdf set



safefactor=1.1 #generate extra 10% events in case any fail showering
nevents = 5000*safefactor
if runArgs.maxEvents > 0: nevents = runArgs.maxEvents*safefactor

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
   beamEnergy = runArgs.ecmEnergy / 2.
else:
   raise RuntimeError("No center of mass energy found")

#make the proc card
fcard = open('proc_card_mg5.dat','w')
fcard.write(proc_card + """
output %s -f
""" % proc_name)
fcard.close()

#does mg5 to determine all the diagrams etc ... generates the process directory corresponding to proc_name
process_dir = MadGraphUtils.new_process()

if proc_name != "" and process_dir != proc_name:
   raise RuntimeError("Unexpected process_dir %s when expected %s" % (process_dir,proc_name))

#create the param_card 
if os.access('param_card.dat',os.R_OK): 
   print("Deleting old param_card.dat")
   os.remove('param_card.dat')
if param_card == "": param_card = "%s/Cards/param_card.dat" % process_dir

if( MadGraphUtils.build_param_card(param_card_old=param_card,param_card_new='param_card.dat',params=param_card_extras) == -1):
   raise RuntimeError("Could not create param_card.dat")

#create the run card: FIXME: Should check for success
if os.access('run_card.dat',os.R_OK): 
   print("Deleting old run_card.dat")
   os.remove('run_card.dat')
if run_card == "": run_card = MadGraphUtils.get_default_runcard(proc_dir=process_dir)
if MadGraphUtils.build_run_card(run_card_old=run_card,run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=run_card_extras)==-1:
   raise RuntimeError("Could not create run_card.dat")


#ready to generate
MadGraphUtils.generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=process_dir)

#lifetime
try:
   addLifetimes
   if addLifetimes:
      MadGraphUtils.add_lifetimes(threshold=1e-20)
except NameError:
   print("addLifetimes parameter not given. Lifetimes will not be added to the lhe file.");

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(proc_name)

MadGraphUtils.arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')


evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")



