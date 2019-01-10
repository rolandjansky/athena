from MadGraphControl.MadGraphUtils import *

safefactor=3
nevents=5000*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor

mode=0
#Defaults for run_card.dat
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'nn23lo1'",
           'parton_shower':'PYTHIA8',
           'ptj':'20',
           'ptb':'20',
           'pta':'25',
           'ptjmax':'-1',
           'ptbmax':'-1',
           'ptamax':'-1',
           'etaj':'-1',
           'etab':'-1',
           'etaa':'2.7',
           'etajmin':'0',
           'etabmin':'0',
           'etaamin':'0',
           'mmaa':'60',
           'mmaamax':'200',
           'mmbb':'45',
           'mmbbmax':'-1',
           'mmjj':'25',
           'mmjjmax':'-1',
           'drjj':'0.4',
           'drbb':'0.4',
           'draa':'0.4',
           'drbj':'0.4',
           'draj':'0.4',
           'drab':'0.4',
           'drjjmax':'-1',
           'drbbmax':'-1',
           'draamax':'-1',
           'drbjmax':'-1',
           'drajmax':'-1',
           'drabmax':'-1' }


fcard = open('proc_card_mg5.dat','w')
runName='run_01'     
if (runArgs.runNumber == 344460):
    fcard.write("""
    import model sm
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    define x = b b~
    generate p p > x x a a @0
    add process p p > a a x x j @1
    output -f""")
    fcard.close()
    runName='aabb'
elif (runArgs.runNumber == 344461):
    fcard.write("""
    import model sm
    define p = g u c d b s u~ c~ d~ s~ b~
    define j = g u c d s u~ c~ d~ s~
    define x = b b~
    generate p p > a x x j @0
    add process p p > a x x j j @1
    output -f""")
    fcard.close()
    runName='abbj'
elif (runArgs.runNumber == 344462):
    fcard.write("""
    import model sm
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u d s u~ d~ s~
    define x = c c~
    define y = b b~
    generate p p > a a j j @0
    add process p p > j x a a @1
    add process p p > j y a a @2
    add process p p > j j j a a @3
    add process p p > j j x a a @4
    add process p p > j j y a a @5
    output -f""")
    fcard.close()
    runName='aajj'
elif (runArgs.runNumber == 344465):
    fcard.write("""
    import model sm
    define p = g u c d s b u~ c~ d~ s~ b~ 
    define j = g u c d s b u~ c~ d~ s~ b~
    define jl = g u d s u~ d~ s~ 
    define x = b b~
    generate p p > x x jl jl @0
    add process p p > x x jl jl j @1
    output -f""")
    fcard.close()
    runName='bbjj'
elif (runArgs.runNumber == 344463):
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define x = c c~
    generate p p > x x a a @0
    add process p p > x x a a j @1
    output -f""")
    fcard.close()
    runName='aacc'
elif (runArgs.runNumber == 344464):
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define x = c c~
    generate p p > x x j a @0
    add process p p > x x j a j @1

    output -f""")
    fcard.close()
    runName='accj'
elif (runArgs.runNumber == 361599):
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    generate p p > a a Z , Z > b b~ @0
    add process p p > a a Z j , Z > b b~ @1    
    output -f""")
    fcard.close()
    runName='aaZbb'

elif (runArgs.runNumber == 344820):
    fcard.write("""
    import model sm
    define p = g u c d s b u~ c~ d~ s~ b~ 
    define j = g u c d s b u~ c~ d~ s~ b~
    define jl = g u d s u~ d~ s~ 
    define x = b b~
    generate p p > x x jl jl
    output -f""")
    fcard.close()
    runName='bbjj'

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',
               xqcut=0,nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

print_cards()
    
process_dir = new_process()
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')  

evgenConfig.contact = ['Jahred Adelman']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
