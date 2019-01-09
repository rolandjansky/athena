from MadGraphControl.MadGraphUtils import *

if not hasattr(runArgs,'runNumber'):
  raise RunTimeError("No run number found.")

#--------------------------------------------------------------
# MG5 Proc card
#--------------------------------------------------------------
# SUSY NUH2:
# Strong Production in MadGraph,
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    import model mssm-full
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    define sewk = el- el+ er- er+ mul- mul+ mur- mur+ ta1- ta1+ ta2- ta2+ n1 n2 n3 n4 x1- x1+ x2- x2+ sve sve~ svm svm~ svt svt~
    define sstrong = go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~
    define compressed = n1 n2 x1- x1+
    define accessibleweak = n3 n4 x2- x2+
    define accessiblestrong = go t1 t1~
    generate p p > accessibleweak accessibleweak QED=99 QCD=99 @1
    add process p p > accessibleweak accessibleweak j QED=99 QCD=99 @2
    output -f
    """)
fcard.close()

#     add process p p > accessibleweak accessibleweak j QED=99 QCD=99 @2

njets=0

runArgs.qcut = 100. # default MLM matching scale

if njets==0:
    runArgs.qcut = 0

beamEnergy=6500
if hasattr(runArgs,'ecmEnergy'): 
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  raise RunTimeError("No center of mass energy found.")

evt_multiplier = 1.1
if njets>0:
    evt_multiplier*=2.0

evt_multiplier*=10.0


nevts = runArgs.maxEvents if runArgs.maxEvents > 0 else 5000
# Build MadGraph param and run cards

# Change defaults for run_card.dat
extras = {  'lhe_version':'2.0',
            'ktdurham':runArgs.qcut ,
            'cut_decays':'F' ,
            'pdlabel':"'nn23lo1'",
            'lhaid':'247000',
            'parton_shower':'PYTHIA8',
            'drjj':0.0 }

run_name=runArgs.jobConfig[0][runArgs.jobConfig[0].find("NUHM2"):runArgs.jobConfig[0].find("m12")+7]

param_card_name = "susy.{0}.".format(runArgs.runNumber)+run_name+".slha"

run_card_name = build_run_card(run_card_old=get_default_runcard(),xqcut=runArgs.qcut,
                                       nevts=nevts*evt_multiplier,
                                       rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)



print_cards()

from PyJobTransformsCore.trfutil import get_files
get_files(param_card_name, keepDir=True, errorIfNotFound=True)

# Generate the new process
process_dir = new_process(card_loc='proc_card_mg5.dat')
generate(run_card_loc=run_card_name,param_card_loc=param_card_name,mode=0,njobs=1,
         run_name=run_name,proc_dir=process_dir)


# Arrange the output
runArgs.inputGeneratorFile="madgraph.{0}.Madgraph_{1}._00001.events.tar.gz".format(runArgs.runNumber,runArgs.jobConfig[0][runArgs.jobConfig[0].find("A14N23LO"):runArgs.jobConfig[0].find(".py")])

#skip_events = 0
if hasattr(runArgs,'skipEvents'): skip_events = runArgs.skipEvents
arrange_output(run_name=run_name,proc_dir=process_dir,
                       outputDS=runArgs.inputGeneratorFile)





#--------------------------------------------------------------
# Pythia8 showering with A14_NNPDF23LO
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            ]
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = on",
                                "Merging:nJetMax = "+str(njets),
                                "Merging:doKTMerging = on",
                                "Merging:TMS = "+str(runArgs.qcut),
                                "Merging:ktType = 1",
                                "Merging:Dparameter = 0.4",
                                "Merging:nQuarksMerge = 4"]
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}t1t1~"]

#print genSeq.Pythia8.Commands


include("GeneratorFilters/MultiElecMuTauFilter.py")

MultiElecMuTauFilter = filtSeq.MultiElecMuTauFilter
MultiElecMuTauFilter.NLeptons  = 2
MultiElecMuTauFilter.MaxEta = 2.7
MultiElecMuTauFilter.MinPt = 7000.0
MultiElecMuTauFilter.MinVisPtHadTau = 15000.0
MultiElecMuTauFilter.IncludeHadTaus = True


#--------------------------------------------------------------
# MC15 metadata
#--------------------------------------------------------------
evgenConfig.contact = ["adebened@cern.ch"]
evgenConfig.description = "NUHM2 test model with weak production, m_12 = %s GeV"%(runArgs.jobConfig[0].split('_')[4])
#evgenConfig.inputfilecheck = runArgs.inputGeneratorFile.split('._0')[0]
evgenConfig.keywords = ["SUSY","NUHM2"]
evgenConfig.process = "weak/weak"


