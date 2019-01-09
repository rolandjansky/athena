from MadGraphControl.MadGraphUtils import *
import math

# minimum width calculation
#          m_d     m_u     m_s    m_c  m_b  m_t
mquark = [ 0.0048, 0.0023, 0.095, 1.3, 4.8, 173.34 ]
v = 246
alpha_s = 0.1

def wTSD_q(mDM,mMed,gXq,qtype):
    w = 0.0
    if ((pow(mDM,2) + pow(mquark[qtype],2)) <= pow(mMed,2)) and (pow((1 - pow(mquark[qtype],2)*pow(mMed,-2) + pow(mDM,2)*pow(mMed,-2)),2) >= 4*pow(mDM,2)*pow(mMed,-2)):
        w = mMed*pow(gXq,2)*pow((16*math.pi),-1)*(1-pow(mquark[qtype],2)*pow(mMed,-2) - pow(mDM,2)*pow(mMed,-2))*pow(pow(1-pow(mquark[qtype],2)*pow(mMed,-2) + pow(mDM,2)*pow(mMed,-2),2) - 4*pow(mDM,2)*pow(mMed,-2),0.5)
    return w

# merging parameters
qcut = 0
ktdurham=30.
dparameter=0.4
nJetMax=2
bwcutoff=15
maxjetflavor=4

#set bwcutoff
width = []
for iQuark in [0,1,2,3]:
  width.append( wTSD_q(MX,Mmed,gqX,iQuark))
width_max = max(  width )
while ( bwcutoff > 1. and width_max > 0.):
  if ( bwcutoff * width_max < 50.  ):
    break
  else:
    bwcutoff-=1

mgMed=""
if wTSD_q(MX,Mmed,gqX,0)>0.0:
  mgMed+="etad etadbar "
if wTSD_q(MX,Mmed,gqX,1)>0.0:
  mgMed+="etau etaubar "
if wTSD_q(MX,Mmed,gqX,2)>0.0:
  mgMed+="etas etasbar "
if wTSD_q(MX,Mmed,gqX,3)>0.0:
  mgMed+="etac etacbar "

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model dmS_T -modelname

define p = g d u s c d~ u~ s~ c~
define j = g d u s c d~ u~ s~ c~
define med = """+mgMed+"""
define dm = chi chi~ 
generate p p > dm med / etab etabbar etat etatbar a h z w+ w- $med NP=1 QCD=1 @1 
add process p p > j dm med / etab etabbar etat etatbar a h z w+ w- $med NP=1 QCD=2 @2 
add process p p > j j dm med / etab etabbar etat etatbar a h z w+ w- $med NP=3 QCD=3 @3 

output -f
""")
fcard.close()

#Missing Et Filter
#evgenLog.info('MET 100 filter is applied')
#include('GeneratorFilters/MissingEtFilter.py')
#filtSeq.MissingEtFilter.METCut = 100000.
#filtSeq.Expression = "MissingEtFilter"

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version'  :'3.0',
          'cut_decays'   :'F',
          'pdlabel'      :"'nn23lo1'",
          'lhaid'        :247000,
          'ickkw'        :0,
          'maxjetflavor' :maxjetflavor,
          'asrwgtflavor' :maxjetflavor,
          'ptj'          :10,
          'drjj'         :0.0,
          'etaj'         :5,
          'etab'         :5,
          'ktdurham'     :ktdurham,
          'dparameter'   :dparameter,
          'bwcutoff'     :bwcutoff}

process_dir = new_process()
	  
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', xqcut = qcut,
               nevts=runArgs.maxEvents*MG_factor,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
    
print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_dmSTmodel_paramcard.dat'])
paramcard.wait()
if not os.access('MadGraph_dmSTmodel_paramcard.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_dmSTmodel_paramcard.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        # fill in coupling strengths
        if 'fUD' in line:
            newcard.write('    1 %e # fUD \n'%(gqX))
        elif 'fCS' in line:
            newcard.write('    2 %e # fCS \n'%(gqX))
        elif 'fTB' in line:
            newcard.write('    3 0 # fTB \n')
	# fill in DM mass
        elif 'MX' in line:
            newcard.write('  1000022 %e # MX \n'%(MX))
	# fill all mediator masses with the same mass, but able here to use independent masses if desired
        elif 'MEtaD' in line:
            newcard.write('  9000006 %e # MEtaD \n'%(Mmed))
        elif 'MEtaU' in line:
            newcard.write('  9000007 %e # MEtaU \n'%(Mmed))
        elif 'MEtaS' in line:
            newcard.write('  9000008 %e # MEtaS \n'%(Mmed))
        elif 'MEtaC' in line:
            newcard.write('  9000009 %e # MEtaC \n'%(Mmed))
        elif 'MEtaB' in line:
            newcard.write('  9000014 %e # MEtaB \n'%(Mmed))
        elif 'MEtaT' in line:
            newcard.write('  9000011 %e # MEtaT \n'%(Mmed))
	# set the width calling function above
        elif 'WEtaD' in line:
            newcard.write('DECAY 9000006 %e # WEtaD \n'%(wTSD_q(MX,Mmed,gqX,0)))
        elif 'WEtaU' in line:
            newcard.write('DECAY 9000007 %e # WEtaU \n'%(wTSD_q(MX,Mmed,gqX,1)))
        elif 'WEtaS' in line:
            newcard.write('DECAY 9000008 %e # WEtaS \n'%(wTSD_q(MX,Mmed,gqX,2)))
        elif 'WEtaC' in line:
            newcard.write('DECAY 9000009 %e # WEtaC \n'%(wTSD_q(MX,Mmed,gqX,3)))
        elif 'WEtaB' in line:
            newcard.write('DECAY 9000014 %e # WEtaB \n'%(wTSD_q(MX,Mmed,gqX,4)))
        elif 'WEtaT' in line:
            newcard.write('DECAY 9000011 %e # WEtaT \n'%(wTSD_q(MX,Mmed,gqX,5)))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz', saveProcDir=True)                                                                          
                                                                                                                                                                        
#### Shower                                                                                                                                                             
evgenConfig.description = "Wimp pair monojet with dmS_T"
evgenConfig.keywords = ["monojet","exotic","BSM","WIMP"]
evgenConfig.process = "pp>chichi~j"
evgenConfig.inputfilecheck = runName                                                                                                                                    
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                              
evgenConfig.contact = ["Emily McDonald <emily.mc.donald@cern.ch>, Maria Giulia Ratti <maria.giulia.ratti@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["1000022:all = x x~ 2 0 0 " + str(MX) + " 0.0 0.0 0.0 0.0",
                            "1000022:isVisible = false"]

PYTHIA8_nJetMax=nJetMax
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process='pp>{chi,1000022}{chi~,-1000022}{etad,9000006}{etad~,-9000006}{etau,9000007}{etau~,-9000007}{etas,9000008}{etas~,-9000008}{etac,9000009}{etac~,-9000009}'
PYTHIA8_TMS=ktdurham
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")

genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = on"]
