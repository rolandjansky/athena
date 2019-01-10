from MadGraphControl.MadGraphUtils import *



runName='test'


fcard = open('proc_card_mg5.dat','w')



mphi = float(runArgs.jobConfig[0].split('_')[5].replace("p",""))
mchi = float(runArgs.jobConfig[0].split('_')[6].replace("c",""))
evgenLog.info('Processing model with masses: (mphi, mchi) = (%e,%e)' %(mphi, mchi))

filter_string = runArgs.jobConfig[0].split('_')[7].replace(".py","")

drjj = 0.0 
if model_string == "ttscalar":

    fcard.write("""
import model DMScalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
generate p p > t t~ chi chi~ 
add process p p > t t~ j chi chi~ 
output -f
""")

    xptb = 0
    ptj = 20


    nJetMax=1
    ktdurham=40
    if mphi/4 > 40:
        ktdurham = mphi/4
    evgenLog.info('ktdurham set to %i' %ktdurham)    
    process="pp>{phi,1000000}tt~"
    removedecays = "on"
    dokt = "T"
    lhaid = "263000"
    maxjetflavor = "5"
    evgenConfig.process = "pp>ttchichi"

elif model_string == "ttpseudo":
    fcard.write("""
import model DMPseudoscalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
generate p p > t t~ chi chi~ 
add process p p > t t~ j chi chi~
output -f
""")

    xptb = 0
    ptj = 20

    
    nJetMax=1
    ktdurham=40
    if mphi/4 > 40:
        ktdurham = mphi/4
    evgenLog.info('ktdurham set to %i' %ktdurham)    
    process="pp>{phi,1000000}tt~"
    removedecays = "on"
    dokt = "T"
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

    xptb = 30
    ptj = 30

    nJetMax=1
    ktdurham=30
    process="pp>{chi,1000022}{chi~,-1000022}bb~"
    removedecays = "off"
    dokt = "T"
    lhaid = "263400"
    maxjetflavor = "4"


elif model_string == "bbpseudo":
    fcard.write("""
import model DMPseudoscalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ 
define j = g u c d s u~ c~ d~ s~ 
generate p p > b b~ chi chi~ 
add process p p > b b~ j chi chi~
output -f
""")
    xptb = 30
    ptj = 30

    nJetMax=1
    ktdurham=30
    process="pp>{chi,1000022}{chi~,-1000022}bb~"
    removedecays = "off"
    dokt = "T"
    lhaid = "263400"
    maxjetflavor = "4"

elif model_string == "Wtscalar":

    fcard.write("""
import model DMScalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
generate p p > t w- chi~ chi 
add process p p > t~ w+ chi~ chi 
output -f
""")

    xptb = 0
    ptj = 20
    drjj = 0.4 

    nJetMax=0
    ktdurham=0
    dokt = "F"
    lhaid = "263000"
    maxjetflavor = "5"
    evgenConfig.process = "pp>tWchichi"

elif model_string == "Wtpseudo":
    fcard.write("""
import model DMPseudoscalarMed_loop --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
generate p p > t w- chi~ chi 
add process p p > t~ w+ chi~ chi 
output -f
""")

    xptb = 0
    ptj = 20
    drjj = 0.4 

    nJetMax=0
    ktdurham=0
    dokt = "F"
    lhaid = "263000"
    maxjetflavor = "5"
    evgenConfig.process = "pp>tWchichi"



fcard.close()


if (filter_string == "1LMET60orMET150"):
    evgenLog.info('1lepton and MET 60 filter or MET 150 is applied')
    include ( 'GeneratorFilters/LeptonFilter.py' )
    filtSeq.LeptonFilter.Ptcut  = 20000.
    filtSeq.LeptonFilter.Etacut = 2.8 
    
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 60000.


    filtSeq += MissingEtFilter("MissingEtFilterHard")
    filtSeq.MissingEtFilterHard.METCut = 150000.

    filtSeq.Expression = "(LeptonFilter and MissingEtFilter) or MissingEtFilterHard"    


if (filter_string == "2L"):
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 18000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 2
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0

    filtSeq.Expression = "MultiElecMuTauFilter"    

if (filter_string == "MET90andJET80"):
    evgenLog.info('MET 90 and jet 80')
    
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 90000.

    include( 'GeneratorFilters/JetFilterAkt6.py')
    filtSeq.QCDTruthJetFilter.MinPt = 80. * 1000.
    filtSeq.QCDTruthJetFilter.MaxPt = 13000. * 1000.
    filtSeq.QCDTruthJetFilter.MaxEta = 3.2
    filtSeq.QCDTruthJetFilter.MinEta = -3.2
    
    filtSeq.Expression = "MissingEtFilter and QCDTruthJetFilter"



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

extras = {'lhe_version':'2.0', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : lhaid,
          'ickkw'      : '0',
          'ktdurham'   : ktdurham,          
          'maxjetflavor':maxjetflavor,
          'xptb':xptb,
          'ptj':ptj,
          'drjj':drjj,
          }

if dokt == 'T': 
   extras['dokt'] = dokt

process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', xqcut=0, 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)


print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_ScalarDM.dat'])

paramcard.wait()
if not os.access('MadGraph_param_card_ScalarDM.dat',os.R_OK):
    raise RuntimeError( 'ERROR: Could not get param card' )
elif os.access('param_card.dat',os.R_OK):
    raise RuntimeError( 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.')
else:
    oldcard = open('MadGraph_param_card_ScalarDM.dat','r')
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

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir,nevents=nevents,random_seed=runArgs.randomSeed)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)              

outputDS=runName+'._00001.events.tar.gz'
runArgs.inputGeneratorFile = outputDS                                                            
evgenConfig.description = 'DM+%s with matching, m_med = %s GeV, m_chi = %s GeV'%(model_string, mphi, mchi)
evgenConfig.keywords = ["exotic","BSM","WIMP", "SUSY"]

evgenConfig.inputfilecheck = runName                                                                                                                                    
evgenConfig.contact = ["Priscilla Pani <ppani@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

if nJetMax<0:
    evgenLog.fatal('njets is not set')
    raise RuntimeError('njet is not set')


import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts


genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            "1000022:all = Chi Chi~ 1 0 0 %s " %mchi,
                           ]
if nJetMax>0:
    genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = %s" %removedecays,
                                "Merging:doKTMerging = on",
                                "Merging:ktType = 1",
                                "Merging:nJetMax = %i"%nJetMax,
                                "Merging:Process = %s"%process,
                                "Merging:TMS = %f"%ktdurham, 
                                "Merging:nQuarksMerge = %s" %maxjetflavor,
                                "Merging:Dparameter = 0.4",
                                ]

