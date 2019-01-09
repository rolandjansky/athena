from MadGraphControl.MadGraphUtils import *


fcard = open('proc_card_mg5.dat','w')

#defaults changed for bb
xptb = 0
ptj = 20
#evt_multiplier = 2

if model_string == "ttscalar":
    fcard.write("""
import model DMScalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~
generate p p > t t~ chi chi~
output -f
""")
    nJetMax=0
    ktdurham=0
    process="pp>{n1,1000022}{n1~,-1000022}tt~"
    dokt = "F"
    lhaid = "263000"
    maxjetflavor = "5"
    evgenConfig.process = "pp>ttchichi"
    
elif model_string == "ttpseudo":
    fcard.write("""
import model DMPseudoscalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~
generate p p > t t~ chi chi~
output -f
""")
    nJetMax=0
    ktdurham=0
    process="pp>{n1,1000022}{n1~,-1000022}tt~"
    dokt = "F"
    lhaid = "263000"
    maxjetflavor = "5"
    evgenConfig.process = "pp>ttchichi"

elif model_string == "bbscalar":
    fcard.write("""
import model DMScalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
generate p p > b b~ chi chi~
add process p p > b b~ j chi chi~
output -f
""")
    nJetMax=3
    ktdurham=30
    process="pp>{n1,1000022}{n1~,-1000022}bb~"
    dokt = "T"
    lhaid = "263400"
    maxjetflavor = "4"
    xptb = 30
    ptj = 30
    evgenConfig.process = "pp>bbchichi"


elif model_string == "bbpseudo":
    fcard.write("""
import model DMPseudoscalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
generate p p > b b~ chi chi~
add process p p > b b~ j chi chi~
output -f
""")
    nJetMax=3
    ktdurham=30
    process="pp>{n1,1000022}{n1~,-1000022}bb~"
    dokt = "T"
    lhaid = "263400"
    maxjetflavor = "4"
    xptb = 30
    ptj = 30
    evgenConfig.process = "pp>bbchichi"


elif model_string == "bFDM":
    fcard.write("""
import model DM_Bflavored --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
generate p p > n1 n1~ 
add process p p > n1 n1~ j
add process p p > n1 n1~ j j
output -f
""")
    nJetMax=2
    ktdurham=30
    process="pp>{n1,1000022}{n1~,-1000022}"
    dokt = "T"
    lhaid = "263000"
    maxjetflavor = "5"
    evgenConfig.process = "pp>bchichi"

fcard.close()

if (filter_string == "T") and (model_string == "ttscalar" or model_string == "ttpseudo"):
    evgenLog.info('1lepton or MET 60 filter is applied')
    include ( 'GeneratorFilters/LeptonFilter.py' )
    filtSeq.LeptonFilter.Ptcut  = 20000.
    filtSeq.LeptonFilter.Etacut = 2.8 
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 60000.
    filtSeq.Expression = "LeptonFilter or MissingEtFilter"
elif (filter_string == "T") and (model_string == "bbscalar" or model_string == "bbpseudo"):
    evgenLog.info('MET 60 filter is applied')
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 60000.
    filtSeq.Expression = "MissingEtFilter"
elif (filter_string == "T") and (model_string == "bFDM"):
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 60000.
    filtSeq.Expression = "MissingEtFilter"
    include('GeneratorFilters/HeavyFlavorHadronFilter.py')
    filtSeq.HeavyFlavorHadronFilter.RequestBottom=False
    filtSeq.HeavyFlavorHadronFilter.RequestCharm=False
    filtSeq.HeavyFlavorHadronFilter.Request_cQuark=False
    filtSeq.HeavyFlavorHadronFilter.Request_bQuark=True
    filtSeq.HeavyFlavorHadronFilter.RequestSpecificPDGID=False
    filtSeq.HeavyFlavorHadronFilter.RequireTruthJet=False
    filtSeq.HeavyFlavorHadronFilter.bPtMin=10*GeV
    filtSeq.Expression = "HeavyFlavorHadronFilter or MissingEtFilter"
    



if evt_multiplier>0:
    if runArgs.maxEvents>0:
        nevents=runArgs.maxEvents*evt_multiplier
    else:
        nevents=5000*evt_multiplier


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : lhaid,
	  'ickkw'      : '0',
          'ktdurham'   : ktdurham,
          'dokt'       : dokt,
          'maxjetflavor':maxjetflavor,
          'xptb':xptb,
          'ptj':ptj,
          }
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat', xqcut=0, 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()


if model_string == "bFDM":
    paramcard = subprocess.Popen(['get_files','-data','MadGraph_bFDM_paramcard.dat'])
    paramcard.wait()
    if not os.access('MadGraph_bFDM_paramcard.dat',os.R_OK):
        print 'ERROR: Could not get param card'
    elif os.access('param_card.dat',os.R_OK):
        print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
    else:
        oldcard = open('MadGraph_bFDM_paramcard.dat','r')
        newcard = open('param_card.dat','w')
        for line in oldcard:
            if 'MX' in line:
                newcard.write('  1000022 %e # MX\n'%(mchi))
            elif 'Mphix' in line:
                newcard.write('   1000005 %e # Mphix\n'%(mphi))
            elif 'gx' in line:
                newcard.write(' 1 %d # gx \n '%(gx))
            elif 'wphix' in line:
                newcard.write('wphix auto\n')
            else:
                newcard.write(line)
        oldcard.close()
        newcard.close()

else:
    parcardname = 'MadGraph_param_card_ScalarDM.dat'
    paramcard = subprocess.Popen(['get_files','-data',parcardname])
    paramcard.wait()
    if not os.access(parcardname,os.R_OK):
        print 'ERROR: Could not get param card'
    elif os.access('param_card.dat',os.R_OK):
        print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
    else:
        oldcard = open(parcardname,'r')
        newcard = open('param_card.dat','w')
        for line in oldcard:
            if 'Mchi' in line:
                newcard.write('  1000022 %e # Mchi\n'%(mchi))
            elif 'Mphi' in line:
                newcard.write('   1000000 %e # Mphi\n'%(mphi))
            elif 'Gphi' in line:
                newcard.write(' 1 0 # Gphi \n ')
            elif 'Fphi' in line:
                newcard.write(' 1 0 # Fphi \n ')        
            elif 'DECAY 1000000' in line:
                newcard.write('DECAY 1000000 %e\n'%(Wphi))
            else:
                newcard.write(line)
        oldcard.close()
        newcard.close()

process_dir = new_process()
runName='test'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          
                                                                                                                                                                        
#### Shower                                                                                                                                                             
evgenConfig.description = "DM simplified scalar models "+model_string
evgenConfig.keywords = ["exotic","BSM","WIMP"]

evgenConfig.inputfilecheck = runName                                                                                                                                    
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                              
evgenConfig.contact = ["Priscilla Pani <ppani@cern.ch>"]

# Pythia8 setup
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

if nJetMax<0:
    evgenLog.fatal('njets is not set')
    raise RuntimeError('njet is not set')


import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts





genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            "1000022:all = Chi Chi~ 1 0 0 %s " %mchi,
                           ]
if nJetMax>0:
    genSeq.Pythia8.Commands += ["Merging:doKTMerging = on",
                                "Merging:ktType = 1",
                                "Merging:nJetMax = %i"%nJetMax,
                                "Merging:Process = %s"%process,
                                "Merging:TMS = %f"%ktdurham, 
                                "Merging:nQuarksMerge = %s" %maxjetflavor,
                                "Merging:Dparameter = 0.4",
                                ]

