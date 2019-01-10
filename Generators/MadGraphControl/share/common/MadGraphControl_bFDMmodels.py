from MadGraphControl.MadGraphUtils import *

evgenConfig.contact = ["Priscilla Pani <ppani@cern.ch>"]
evgenConfig.description = "DM bFDM models mchi = %.1f GeV, mphi = %.1f GeV, coupling gx = %.5f " % (mchi,mphi,gx)
evgenConfig.keywords = ["exotic","BSM","WIMP"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

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

fcard = open('proc_card_mg5.dat','w')

xptb = 0
ptj = 20


fcard.write("""
import model DM_Bflavored --modelname
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
generate p p > n1 n1~ 
add process p p > n1 n1~ j
add process p p > n1 n1~ j j
add process p p > n1 n1~ j j j
output -f
""")
nJetMax=3
ktdurham=30
process="pp>{n1,1000022}{n1~,-1000022}"
dokt = "T"
lhaid = "263000"
maxjetflavor = "5"
evgenConfig.process = "pp>bchichi"

fcard.close()

#Filters
if (filter_string == "T"):
    evgenLog.info('1 Jet 30 and MET 100 filter is applied')
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 100000.
    filtSeq.Expression = "MissingEtFilter"
    include( 'GeneratorFilters/JetFilterAkt6.py')
    filtSeq.QCDTruthJetFilter.MinPt = 30. * 1000.
    filtSeq.QCDTruthJetFilter.MaxPt = 13000. * 1000.
    filtSeq.QCDTruthJetFilter.MaxEta = 3.2
    filtSeq.QCDTruthJetFilter.MinEta = -3.2
    filtSeq.Expression = "QCDTruthJetFilter and MissingEtFilter"

elif (filter_string == "VT"):
    evgenLog.info('1 Jet 150 and MET 100 filter is applied')
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 100000.
    filtSeq.Expression = "MissingEtFilter"
    include( 'GeneratorFilters/JetFilterAkt6.py')
    filtSeq.QCDTruthJetFilter.MinPt = 150. * 1000.
    filtSeq.QCDTruthJetFilter.MaxPt = 13000. * 1000.
    filtSeq.QCDTruthJetFilter.MaxEta = 3.2
    filtSeq.QCDTruthJetFilter.MinEta = -3.2
    filtSeq.Expression = "QCDTruthJetFilter and MissingEtFilter"


paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_bFDM.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_bFDM.dat',os.R_OK):
    raise RuntimeError( 'ERROR: Could not get param card' )
elif os.access('param_card.dat',os.R_OK):
    raise RuntimeError( 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.')
else:
    oldcard = open('MadGraph_param_card_bFDM.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if 'MX' in line:
            newcard.write('  1000022 %e # MX\n'%(mchi))
        elif 'Mphix' in line:
            newcard.write('   1000005 %e # Mphix\n'%(mphi))
        elif 'gx' in line:
            print "PPP: changing coupling to", gx
            print "write to the card"
            print ' 1 %f # gx \n '%(gx)
            newcard.write(' 1 %f # gx \n '%(gx))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()


process_dir = new_process()
runName='test'

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
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', xqcut=0, 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
  
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

evgenConfig.generators += ["EvtGen"]

if nJetMax>0:
    genSeq.Pythia8.Commands += ["Merging:doKTMerging = on",
                                "Merging:ktType = 1",
                                "Merging:nJetMax = %i"%nJetMax,
                                "Merging:Process = %s"%process,
                                "Merging:TMS = %f"%ktdurham, 
                                "Merging:nQuarksMerge = %s" %maxjetflavor,
                                "Merging:Dparameter = 0.4",
                                ]

