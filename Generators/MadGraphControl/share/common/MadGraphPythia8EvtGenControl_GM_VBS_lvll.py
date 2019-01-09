from MadGraphControl.MadGraphUtils import *

nevents=40000
mode=0


### DSID lists
#So I free 344705-344722 and reserve only 344590-344692 and 344695-344704 for this request.
DSIDDic = {344590:'225',344591:'250',344592:'275',344593:'350',344594:'1000',344595:'1100',344596:'1200',344597:'1300',344598:'1400',344599:'1500',344600:'1600',344601:'1700',344602:'1000',344603:'1100',344604:'1200',344605:'1300',344606:'1400',344607:'1500',344608:'1600',344609:'1700',344610:'1800',344611:'1900',344612:'2000',344613:'2200',344614:'2400',344615:'2600',344616:'2800',344617:'3000',344618:'200',344619:'225',344620:'250',344621:'275',344622:'300',344623:'350',344624:'400',344625:'500',344626:'600',344627:'700',344628:'800',344629:'900',344630:'1000',344631:'1100',344632:'1200',344633:'1300',344634:'1400',344635:'1500',344636:'1600',344637:'1700',344638:'1800',344639:'1900',344640:'2000',344641:'2200',344642:'2400',344643:'2600',344644:'2800',344645:'3000', 344646:'200',344647:'225',344648:'250',344649:'275',344650:'300',344651:'350',344652:'400',344653:'500',344654:'600',344655:'700',344656:'800',344657:'900',344658:'1000',344659:'1100',344660:'1200',344661:'1300',344662:'1400',344663:'1500',344664:'1600',344665:'1700',344666:'1800',344667:'1900',344668:'2000',344669:'2200',344670:'2400',344671:'2600',344672:'2800',344673:'3000', 344674:'1800',344675:'1900',344676:'2000',344677:'2200',344678:'2400',344679:'2600',344680:'2800',344681:'500',344682:'600',344683:'700',344684:'800',344685:'900',344686:'1000',344687:'1100',344688:'1200',344689:'1300',344690:'1400',344691:'1500',344692:'1600',344695:'1700',344696:'1800',344697:'1900',344698:'2000',344699:'2200',344700:'2400',344701:'2600',344702:'2800',344703:'3000',344704:'3000'}
mass = DSIDDic[runArgs.runNumber]

doqqll=False
dolvll=True
dolvqq=False     
doqqqq=False
doqqvv=False

mgm_str = str(runArgs.jobConfig[0])
             
#for s in mgm_str.split("_"):
#    ss=s.replace(".py","")  
#    print ss
#    if ss=='qqll':
#        print "qqll"
#        doqqll=True
#    if ss=='lvll':
#        print "lvll"
#        dolvll=True
#    if ss=='lvqq':
#        print "lvqq"
#        dolvqq=True
#    if ss=='qqqq':
#        print "qqqq"
#        doqqqq=True
#    if ss=='qqvv':
#        print "qqvv"
#        doqqvv=True
### Cards
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    define l+ = e+ mu+
    define l- = e- mu-
    define vl = ve vm
    define vl~ = ve~ vm~
    import model GM_UFO
    """)
fcard.close()

fcard = open('proc_card_mg5.dat','a')
if doqqll:
    print "qqll"
    fcard.write("""
        generate    p p > H5p  > w+ z j j QCD=0, w+ > j j, z > l+ l- @1
    add process p p > H5p~ > w- z j j QCD=0, w- > j j, z > l+ l- @2
    output -f
    """)
    fcard.close()

if dolvll:
    print "lvll"
    fcard.write("""
        generate    p p > H5p  > w+ z j j QCD=0, w+ > l+ vl, z > l+ l- @1
    add process p p > H5p~ > w- z j j QCD=0, w- > l- vl~, z > l+ l- @2
    output -f
    """)
    fcard.close()

if dolvqq:
    print "lvqq"
    fcard.write("""
        generate    p p > H5p  > w+ z j j QCD=0, w+ > l+ vl, z > j j @1
    add process p p > H5p~ > w- z j j QCD=0, w- > l- vl~, z > j j @2
    output -f
    """)
    fcard.close()

if doqqqq:
    print "qqqq"
    fcard.write("""
        generate    p p > H5p  > w+ z j j QCD=0, w+ > j j, z > j j @1
        add process p p > H5p~ > w- z j j QCD=0, w- > j j, z > j j @2
    output -f
    """)
    fcard.close()
if doqqvv:
    print "qqvv"
    fcard.write("""
        generate    p p > H5p  > w+ z j j QCD=0, w+ > j j, z > vl vl~ @1
    add process p p > H5p~ > w- z j j QCD=0, w- > j j, z > vl vl~ @2
    output -f
    """)
    fcard.close()


extras = {
    'pdlabel':"'lhapdf'",
    'lhaid':"247000",
    'ptj':"15",
    'pta':"0",
    'ptb':"0",
    'etal':"2.7",
    'drjj':"0",
    'drll':"0",
    'draa':"0",
    'draj':"0",
    'drjl':"0",
    'dral':"0",
    'sys_pdf':"NNPDF23_lo_as_0130_qed.LHgrid" }
    
build_run_card(run_card_old='MadGraph_run_card_GM.dat',run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=runArgs.ecmEnergy/2.,extras=extras)

print_cards()
    
runName='GM_VBS_H5p_lvll_'+mass+'_ISHdot5_qcd0'

process_dir = new_process(card_loc='proc_card_mg5.dat')

evgenConfig.generators = ["MadGraph"]

#generate(run_card_loc='run_card.dat',param_card_loc='MadGraph_param_card_GMm'+mass+'.dat',mode=mode,proc_dir=process_dir,run_name=runName)
generate(run_card_loc='run_card.dat',param_card_loc='MadGraph_param_card_GMm'+mass+'.dat',mode=mode,proc_dir=process_dir,run_name=runName)

############################
# Shower JOs will go here

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
   njobs = os.environ.pop('ATHENA_PROC_NUMBER')
   # Try to modify the opts underfoot
   if not hasattr(opts,'nprocs'): print 'Warning: Did not see option!'
   else: opts.nprocs = 0
   print opts

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',saveProcDir=True)

include("Pythia8_i/Pythia8_A14_NNPDF23LO_Common.py")
include("Pythia8_i/Pythia8_LHEF.py")
include("Pythia8_i/Pythia8_EvtGen.py")
evgenConfig.generators += ["MadGraph", "Pythia8"]
evgenConfig.contact = ["Benjamin Freund <Benjamin.Freund@cern.ch>"]
genSeq.Pythia8.Commands += [
    "Init:showAllParticleData = on",
    "Next:numberShowLHA = 10",
    "Next:numberShowEvent = 10"]
evgenConfig.description = 'MadGraph_GM_VBS_H5p'
evgenConfig.keywords+=['BSM','VBS','Higgs','WZ','leptonic']
#stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_EffDM'
#runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'  
