from MadGraphControl.MadGraphUtils import *
import math

# input settings

nameR=None
nameModel=None
nameModelType=None
codeR=None
gs=None
blockG=None
if model=='dmA' or model=='dmV':
    nameR='xi'
    nameModel=model
    nameModelType='dmA'
    codeR=101
    blockG='HIDDEN'
    gs={'gxichi': gDM,
        'gxiU': gSM,
        'gxiD': gSM,
        'gxiS': gSM,
        'gxiC': gSM,
        'gxiB': gSM,
        'gxiT': gSM}
elif model=='dmS' or model=='dmPS':
    nameR='phim'
    nameModel='DMScalarMed_loop' if model=='dmS' else 'DMPseudoscalarMed_loop'
    nameModelType='ScalarDM'
    codeR=1000000
    blockG='DMMASS'
    gs={'gSM': gSM,
        'gDM': gDM}
else:
    raise RuntimeError("Unknown model.")

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


mg_proc=["generate p p > %s a, %s > j j"%(nameR,nameR),
         "add process p p > %s a, %s > b b~"%(nameR,nameR)]

process='pp>(%s>jj)a'%nameR

# MadgGaph configuration
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model %s

%s

output -f
"""%(nameModel,'\n'.join(mg_proc)))
fcard.close()

process_dir = new_process()

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0', 
          'cut_decays' :'F', 
          'pdlabel'    :"'lhapdf'",
          'lhaid'      :263000}
extras['drjj']=0. # For boosted searches
try: 
  extras['pta']=pta # For boosted searches (@asogaard)
except NameError: pass
try:  
  extras['etaa']=etaa # For boosted searches (@asogaard)
except NameError: pass

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               xqcut=0,
               nevts=int(runArgs.maxEvents*2./filteff),
               rand_seed=runArgs.randomSeed,
               beamEnergy=beamEnergy,
               extras=extras)

build_param_card(param_card_old='MadGraph_param_card_%s.dat'%nameModelType,param_card_new='param_card.dat',
                 masses={'1000022':mDM,
                         str(codeR):mR},
                 decays={str(codeR):'DECAY %d %f # W%s'%(codeR,widthR,nameR)},
                 params={blockG:gs}
                 )

print_cards()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          
                                                                                                                                                                        
#### Shower
evgenConfig.description = "Zprime sample - model %s"%nameModel
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = process
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ["Karol Krizka <kkrizka@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["%d:all = %s void 3 0 0 %d" %(codeR,nameR,mR),
                            "1000022:all = chi chi~ 2 0 0 %d" %(mDM),
                            "1000022:isVisible = false"]

# Add filter
include("GeneratorFilters/DirectPhotonFilter.py")
filtSeq.DirectPhotonFilter.Ptmin = phminpt*GeV
filtSeq.DirectPhotonFilter.Ptmax = 10000.*GeV
