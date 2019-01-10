
from MadGraphControl.MadGraphUtils import *

# General settings
nevents=150000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None
cluster_queue=None

# Merging settings
maxjetflavor=5
ickkw=0
nJetMax=4
ktdurham=30      
dparameter=0.2   


### DSID lists (extensions can include filters, systematics samples, etc.)
Wenu_5fl_NpX=[363600,363601,363602,363603,363604,363605,363606,363607,
              363608,363609,363610,363611,363612,363613,363614,363615,
              363616,363617,363618,363619,363620,363621,363622,363623]     

Wmunu_5fl_NpX=[363624,363625,363626,363627,363628,363629,363630,363631,
               363632,363633,363634,363635,363636,363637,363638,363639,
               363640,363641,363642,363643,363644,363645,363646,363647]   

Wtaunu_5fl_NpX=[363648,363649,363650,363651,363652,363653,363654,363655,429450,
                363656,363657,363658,363659,363660,363661,363662,363663,
                363664,363665,363666,363667,363668,363669,363670,363671] 

nevents_forHT={'lowHT':100000,'midlowHT':60000,'midHT':40000,'midhighHT':30000,'highHT':15000}




### Electrons
if runArgs.runNumber in Wenu_5fl_NpX:
    mgproc="""
generate p p > eall veall @0
add process p p > eall veall j @1
add process p p > eall veall j j @2
add process p p > eall veall j j j @3
add process p p > eall veall j j j j @4
"""
    name='Wenu_NpX'
    process="pp>LEPTONS,NEUTRINOS"
    
    nevents=nevents_forHT[HTrange]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20

### Muons    
elif runArgs.runNumber in Wmunu_5fl_NpX:
    mgproc="""
generate p p > muall vmall @0
add process p p > muall vmall j @1
add process p p > muall vmall j j @2
add process p p > muall vmall j j j @3
add process p p > muall vmall j j j j @4
"""
    name='Wmunu_NpX'
    process="pp>LEPTONS,NEUTRINOS"
    
    nevents=nevents_forHT[HTrange]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
    
### Taus
elif runArgs.runNumber in Wtaunu_5fl_NpX:
    mgproc="""
generate p p > taall vtall @0
add process p p > taall vtall j @1
add process p p > taall vtall j j @2
add process p p > taall vtall j j j @3
add process p p > taall vtall j j j j @4
"""
    name='Wtaunu_NpX'
    process="pp>LEPTONS,NEUTRINOS"
    
    nevents=nevents_forHT[HTrange]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
    
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define eall = e+ e-
define veall = ve ve~
define muall = mu+ mu-
define vmall = vm vm~
define taall = ta+ ta-
define vtall = vt vt~
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'    : '3.0',
           'cut_decays'     : 'F', 
           'pdlabel'        : "'lhapdf'",
           'lhaid'          : 260000,  
           'maxjetflavor'   : maxjetflavor,
           'asrwgtflavor'   : maxjetflavor,
           'ickkw'          : 0,
           'ptj'            : 20,      
           'ptb'            : 20,          
           'ihtmin'         : ihtmin,
           'ihtmax'         : ihtmax,
           'mmjj'           : 0,
           'drjj'           : 0,
           'drll'           : 0,
           'drjl'           : 0.,       
           'ptl'            : 0,
           'etal'           : 10,
           'etab'           : 6,
           'etaj'           : 6,
           'ktdurham'       : ktdurham,    
           'dparameter'     : dparameter  }
##########################################################################################
process_dir = new_process(grid_pack=gridpack_dir)
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()

##############################
## Modify unweighting fudge
fudge='100d0'
unwgt_loc=gridpack_dir+'SubProcesses/unwgt.f'
oldunwgt = open(unwgt_loc,'r')
newunwgt = open(unwgt_loc+'.tmp','w')
for line in oldunwgt:
    if 'data fudge/15d0/' in line:
        newunwgt.write(line.replace('15d0',fudge))
        mglog.info('Updating unwgt.f fudge value to %s'%(fudge))
    else:
        newunwgt.write(line)
oldunwgt.close()
newunwgt.close()
shutil.move(unwgt_loc+'.tmp',unwgt_loc)

##############################
## On-the-fly recompilation
mglog.info('Re-compiling gridpack on-the-fly.')
os.unlink(gridpack_dir+'/lib/libLHAPDF.a')
#os.symlink('/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.4/sw/lcg/external/MCGenerators_lcgcmt67c/lhapdf/6.1.5/x86_64-slc6-gcc47-opt/lib/libLHAPDF.a',gridpack_dir+'/lib/libLHAPDF.a')
os.symlink(os.environ['LHAPDF_DATA_PATH'].split(':')[0].split('share/')[0]+'lib/libLHAPDF.a',gridpack_dir+'/lib/libLHAPDF.a')
os.environ['lhapdf']='-L$LD_LIBRARY_PATH liblhapdf'

os.chdir(gridpack_dir+'SubProcesses/')
mgsubproc = open('subproc.mg','r')
for line in mgsubproc:
    os.chdir(line.strip())
    makeclean = subprocess.Popen(['make','clean'])
    makeclean.wait()
    make = subprocess.Popen(['make'])
    make.wait()
    os.chdir('../')
os.chdir('../../')
mglog.info('Re-compilation done.')
##############################



generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=nJobs,proc_dir=process_dir,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,gridpack_compile=False,
         cluster_type=cluster_type,cluster_queue=cluster_queue,
         nevents=nevents,random_seed=runArgs.randomSeed)

arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')
#### Shower 
evgenConfig.contact  = [ "nicolas.gilberto.gutierrez.ortiz@cern.ch" ]
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=['W','jets']
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'
if hasattr(runArgs,'outputTXTFile'):                                                                                                                         
    runArgs.inputGeneratorFile=runArgs.outputTXTFile
    evgenConfig.inputfilecheck=runArgs.outputTXTFile.split('.tar.gz')[0]  


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")    
include("Pythia8_i/Pythia8_MadGraph.py")

PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process                                                      
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")                         

genSeq.Pythia8.Commands += ["Merging:unorderedASscalePrescrip = 0"]
