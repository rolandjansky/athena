#Author: Will Buttinger <will@cern.ch>

#Usage instructions: (see e.g. https://gitlab.cern.ch/atlas/athena/tree/21.6/Generators/MadGraphControl/share/common/MadGraphControl_HAHM.py for an 
example usage) #specify the following with this fragment:
#proc_name : the name of the process - will match the name of the process directory ... if not specified, we will take it from the 'output' line of the proc_card string
#proc_card : the proc_card string (with or without including the output line)
#param_card : the LOCATION of the param card to use (can be retrieved with get_files)
#param_card_extras : dictionary of dictionaries
#run_card : the LOCATION of the param card to use (can be retrieved with get_files)
#run_card_extras: dictionary ... should not specify pdlabel and lhaid, as those are provided in this fragment
#save_proc_dir: bool ... if true, then wont delete the process directory (given by the proc_name)
#you can also specify a post_lhe_hook method to call code after the lhe making
#to specifically support long-lived particles, this joboption provides a method to lifetimes to particles
#e.g. you can do
#def post_lhe_hook():
#  add_lifetimes(pdgId=32,avgtau=2000)


def add_lifetimes(pdgId, avgtau):
    #first ensure the lifetimes wont cause problems with TestHepMC
    #relax the cuts on displaced vertices and non G4 particles
    print("Updating TestHepMC to allow for displaced vertices")
    testSeq.TestHepMC.MaxTransVtxDisp = 100000000 #in mm
    testSeq.TestHepMC.MaxVtxDisp = 100000000 #in mm
    testSeq.TestHepMC.MaxNonG4Energy = 100000000 #in MeV
    import random
    random.seed(runArgs.randomSeed)
    #lifetime function
    def lifetime(myavgtau = 21):
        import math
        t = random.random()
        return -1.0 * myavgtau * math.log(t)
    rname = 'Test' #comes from the common joboptions
    if os.path.isfile(process_dir+'/Events/'+rname+'/unweighted_events.lhe'):
           os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')
    import subprocess
    unzip1 = subprocess.Popen(['gunzip',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz'])
    unzip1.wait()
    
    oldlhe = open(process_dir+'/Events/'+rname+'/unweighted_events.lhe','r')
    newlhe = open(process_dir+'/Events/'+rname+'/unweighted_events2.lhe','w')
    init = True
    for line in oldlhe:
        if init==True:
            newlhe.write(line)
            if '</init>' in line:
                init = False
        else:  
            if 'vent' in line:
                newlhe.write(line)
                continue
            newline = line.rstrip('\n')
            columns = newline.split()
            pdgid = columns[0]
            if pdgid == str(pdgId):
                #replace second to last column with the new lifetime
                columns[len(columns)-2] = "%.11E" % (lifetime(avgtau))
                newlhe.write(" ".join(columns) + "\n")           
            else:
                newlhe.write(line)
    
    oldlhe.close()
    newlhe.close()
    zip1 = subprocess.Popen(['gzip',process_dir+'/Events/'+rname+'/unweighted_events2.lhe'])
    zip1.wait()
    import shutil
    shutil.move(process_dir+'/Events/'+rname+'/unweighted_events2.lhe.gz',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz')
    os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')


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

hasOutputLine=False
for line in proc_card.splitlines():
   if "output" in line:
      #must be a word on its own
      ss = line.split(" ")
      if "output" not in ss: continue
      hasOutputLine=True
      #look for the word after output ... that is the proc_name
      ss = line.split(" ")
      if proc_name == "": proc_name = ss[ss.index("output")+1]
      elif ss[ss.index("output")+1] != proc_name:
         raise RuntimeError("proc_name (you gave %s) must match that given in proc_card's output line (which is %s)" % (proc_name,ss[ss.index("output")+1]))


if not hasOutputLine:
   proc_card += "\noutput %s -f" % proc_name

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

try:
   save_proc_dir
except NameError:
   save_proc_dir=False


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
fcard.write(proc_card)
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

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(proc_name)

if "post_lhe_hook" in dir():
   print("Calling post_lhe_hook....")
   post_lhe_hook()

MadGraphUtils.arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',saveProcDir=save_proc_dir)

if 'ATHENA_PROC_NUMBER' in os.environ:
   njobs = os.environ.pop('ATHENA_PROC_NUMBER')
   if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
   else: opts.nprocs = 0

evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")



