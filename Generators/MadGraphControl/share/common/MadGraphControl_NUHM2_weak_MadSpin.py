#-------------------------------------------------------------
#JobOptions for NUHM2 model with MadSpin decays
#--------------------------------------------------------------

# adding the recommended preinclude file
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )


#turn on to True if you want to save MG diagrams, process xsecs etc. 
keepOutput=False

# e.g. MC15.123456.MGPy8EG_A14N23LO_NUHM2_m12_600_weak_N2C1p_2LMET50_MadSpin.py
dsid       = str(runArgs.jobConfig[0].split('.')[1]) #123456
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
m12        = splitConfig[4] #m12
gentype    = splitConfig[6] #N2C1p
decaytype="pMSSM"

#--------------------------------------------------------------
# process lines
#--------------------------------------------------------------

process='''
import model mssm
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define f = e+ mu+ ta+ e- mu- ta- ve vm vt ve~ vm~ vt~ u u~ d d~ c c~ s s~ b b~ g
'''

if gentype=="N2C1p":
  process += '''
generate p p > n2 x1+ $ susystrong @1
add process p p > n2 x1+ j $ susystrong @2
add process p p > n2 x1+ j j $ susystrong @3
'''
elif gentype=="N2C1m":
  process += '''
generate p p > n2 x1- $ susystrong @1
add process p p > n2 x1- j $ susystrong @2
add process p p > n2 x1- j j $ susystrong @3
'''
elif gentype=="C1C1":
  process += '''
generate p p > x1+ x1- $ susystrong @1
add process p p > x1+ x1- j $ susystrong @2
add process p p > x1+ x1- j j $ susystrong @3
'''
elif gentype=="N2N1":
  process += '''
generate p p > n2 n1 $ susystrong @1
add process p p > n2 n1 j $ susystrong @2
add process p p > n2 n1 j j $ susystrong @3
'''

procs = {}
procs["N2C1p"] = {"msdecay": "decay n2 > l+ l- n1\ndecay x1+ > f f n1\n",
                  "mergeproc": "{n2,1000023}{x1+,1000024}"}
procs["N2C1m"] = {"msdecay": "decay n2 > l+ l- n1\ndecay x1- > f f n1\n",
                  "mergeproc": "{n2,1000023}{x1-,-1000024}"}
procs["C1C1"]  = {"msdecay": "decay x1+ > l+ vl  n1\ndecay x1- > l- vl~ n1\n",
                  "mergeproc": "{x1+,1000024}{x1-,-1000024}"}
procs["N2N1"]  = {"msdecay": "decay n2 > l+ l- n1\n",
                  "mergeproc": "{n2,1000023}{n1,1000022}"}

if gentype not in procs:
  raise RuntimeError("Error: proccess %s is not known or configured!"%(gentype))

njets = 2

#--------------------------------------------------------------
# MadSpin decays card
#--------------------------------------------------------------

msdecaystring = procs[gentype]["msdecay"]
msdecays = """#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
#Some options (uncomment to apply)
#
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
set BW_cut 100                # cut on how far the particle can be off-shell
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
set seed %i
set spinmode none 
# specify the decay for the final state particles

%s

# running the actual code
launch"""%(runArgs.randomSeed,msdecaystring)

madspin_card_name = "madspin_card_nuhm.dat"
mscard = open(madspin_card_name,'w')
mscard.write(msdecays)
mscard.close()

#-------------------------------------------------------------- 
# Define parameter cards
#--------------------------------------------------------------

#defining input slha file
# e.g. susy.123456.NUHM2_m12_600.slha
param_card_in = 'susy.%s.NUHM2_m12_%s.slha' % (dsid,m12)
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

include ( 'MadGraphControl/SUSYMetadata.py' )
c1Mass = mass_extract( param_card_out, ['1000024'] )
n1Mass = mass_extract( param_card_out, ['1000022'] )
masses['1000024'] = float(c1Mass[0])

#------------------------------------------------------------
# Extras dictionary..
# These settings are kept to keep this grid consistent with simplied model scenario
# To update extras if needed (See MadGraphUtils.py)
#------------------------------------------------------------
xqcut = 15             
extras['ptj']=10       
extras['ptl']=0
extras['etal']='-1.0'
extras['drll']=0.0
extras['drjl']=0.0
extras['lhe_version']='2.0'
extras['auto_ptj_mjj']='F'
extras['bwcutoff']=100 # to allow very low-mass W* and Z*
extras['use_syst']='T'
extras['sys_alpsfact']='1 0.5 2',
extras['sys_scalefact']='1 0.5 2',
extras['sys_pdf']='NNPDF30_nlo_as_0118'

#--------------------------------------------------------------
# Event filters and multipliers
#--------------------------------------------------------------

if '2LMET50' in splitConfig[-2]:
    evgenLog.info('2leptons3 and MET50 filter')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 3000.         # high pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 50*GeV

    filtSeq.Expression = "MultiElecMuTauFilter and MissingEtFilter"
    
    evt_multiplier = 100
    if gentype=="C1C1" and int(m12)>=500:
      evt_multiplier = 150
      
#-------------------------------------------------------------- 
# Pass everything to the PostInclude JO file
#--------------------------------------------------------------

evgenConfig.contact = ["judita.mamuzic@cern.ch"]
evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino']
evgenConfig.description = 'NUHM2 model with EWK production, m_C1(N2) = %s GeV, m_N1 = %s GeV' % (c1Mass,n1Mass)

#madspin_card needs to be the name of the madspin card being used
madspin_card=madspin_card_name

# adding the recommended postinclude file, this is needed to scale variations amongst other things
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

#extra settings needed to tell Pythia8 about the sparticle spin
if njets>0: 
   mergeproc = procs[gentype]["mergeproc"]
   mergeproc+="LEPTONS,NEUTRINOS"
   genSeq.Pythia8.Commands += ["Merging:Process = pp>%s" % mergeproc,
                               "1000024:spinType = 1",
                               "1000023:spinType = 1",
                               "1000022:spinType = 1"]

