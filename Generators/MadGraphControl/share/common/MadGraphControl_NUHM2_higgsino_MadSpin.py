#-------------------------------------------------------------
#JobOptions for NUHM2 model with MadSpin decays
#--------------------------------------------------------------

# adding the recommended preinclude file
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

#turn on to True if you want to save MG diagrams, process xsecs etc. 
keepOutput=False

# e.g. MC15.123456.MGPy8EG_A14N23LO_NUHM2_m12_600_weak_N2C1p_2LMET50_MadSpin.py
dsid        = str(runArgs.jobConfig[0].split('.')[-3]) #123456
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
mu          = splitConfig[4] #mu
m12         = splitConfig[6] #m12
gentype     = splitConfig[7] #N2C1p
decaytype   ="pMSSM"
#print dsid, mu, m12, gentype

#--------------------------------------------------------------
# process lines
#--------------------------------------------------------------

process='''
import model MSSM_SLHA2-full
define c1 = x1+ x1-
define w = w+ w-
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define lv = e+ mu+ ta+ e- mu- ta- ve vm vt ve~ vm~ vt~
define f = e+ mu+ ta+ e- mu- ta- ve vm vt ve~ vm~ vt~ u u~ d d~ c c~ s s~ b b~ g
define sleptons = el- el+ er- er+ mul- mul+ mur- mur+ ta1- ta1+ ta2- ta2+ h+ h- svt svm sve svt~ svm~ sve~
'''

if gentype=="N2C1p":
  process += '''
generate p p > n2 x1+ j / susystrong @1
add process p p > n2 x1+ j j / susystrong @2
'''
elif gentype=="N2C1m":
  process += '''
generate p p > n2 x1- j / susystrong @1
add process p p > n2 x1- j j / susystrong @2
'''
elif gentype=="C1C1":
  process += '''
generate p p > x1+ x1- j / susystrong @1
add process p p > x1+ x1- j j / susystrong @2
'''
elif gentype=="N2N1":
  process += '''
generate p p > n2 n1 j / susystrong @1
add process p p > n2 n1 j j / susystrong @2
'''

procs = {}
procs["N2C1p"] = {"msdecay": "decay n2 > l+ l- n1\ndecay x1+ > f f n1\n",
                  "mergeproc": "w+"}
procs["N2C1m"] = {"msdecay": "decay n2 > l+ l- n1\ndecay x1- > f f n1\n",
                  "mergeproc": "w-"}
procs["C1C1"]  = {"msdecay": "decay x1+ > l+ vl n1\ndecay x1- > l- vl~ n1\n",
                  "mergeproc": "w"}
procs["N2N1"]  = {"msdecay": "decay n2 > l+ l- n1\n",
                  "mergeproc": "z"}

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
set BW_cut 1000             # cut on how far the particle can be off-shell (inv.mass squared)
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
set seed %i
set spinmode none 
# specify the decay for the final state particles

%s

# running the actual code
launch"""%(runArgs.randomSeed,msdecaystring)

madspin_card = "madspin_card_nuhm.dat"
mscard = open(madspin_card,'w')
mscard.write(msdecays)
mscard.close()

#-------------------------------------------------------------- 
# Define parameter cards
#--------------------------------------------------------------

#defining input slha file
# e.g. susy.123456.NUHM2_m12_600.slha
param_card_in = 'susy.%s.NUHM2_mu_%s_m12_%s.slha' % (dsid,mu,m12)
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out)

include ( 'MadGraphControl/SUSYMetadata.py' )
c1Mass = mass_extract( param_card_out, ['1000024'] )
n1Mass = mass_extract( param_card_out, ['1000022'] )
masses['1000024'] = float(c1Mass[0])

#------------------------------------------------------------
# Extras dictionary..
# These settings are kept to keep this grid consistent with simplied model scenario
# To update extras if needed (See MadGraphUtils.py)
#------------------------------------------------------------
xqcut = 15             # low matching scale, following DM group recommendations
extras['ptj']=10       # low matching scale, following DM group recommendations
extras['ptj']=0       # low matching scale, following DM group recommendations
extras['ptl']=0
extras['ptj1min']=50
extras['etal']='-1.0'
extras['drll']=0.0
extras['drjl']=0.0
extras['lhe_version']='3.0'
extras['auto_ptj_mjj']='F'
extras['bwcutoff']=1000 # to allow very low-mass W* and Z* (inv.mass)
extras['event_norm']='sum'
extras['use_syst']='F'
#extras['use_syst']='T'
#extras['sys_alpsfact']='1 0.5 2'
#extras['sys_scalefact']='1 0.5 2'
#extras['sys_pdf']='NNPDF30_nlo_as_0118'

#--------------------------------------------------------------
# Event filters and multipliers
#--------------------------------------------------------------

if '2L2MET75' in splitConfig[-2]:
    evgenLog.info('2leptons2 and MET75 filter')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 2000.         # low lepton pt cut
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 75*GeV
    filtSeq.MissingEtFilterUpperCut.METCut = 100000*GeV

    filtSeq.Expression = "MultiElecMuTauFilter and MissingEtFilter"
    
    evt_multiplier = 10
    if gentype=="C1C1" and int(m12)>=500:
      evt_multiplier = 15
    if int(m12)>=6000:
      evt_multiplier = 50
      if gentype=="N2N1":
        evgenConfig.minevents = 1000
      else:
        evgenConfig.minevents = 2000
      
#-------------------------------------------------------------- 
# Pass everything to the PostInclude JO file
#--------------------------------------------------------------

evgenConfig.contact = ["judita.mamuzic@cern.ch"]
evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino']
evgenConfig.description = 'NUHM2 model with EWK production, m_C1(N2) = %s GeV, m_N1 = %s GeV' % (c1Mass,n1Mass)

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

# Using the "guess" option for Pythia8 is broken for N2C1m production
# Bugfix followup at https://its.cern.ch/jira/browse/AGENE-1643
# When problem fixed change to this implementation:

# extra settings needed to tell Pythia8 about the sparticle spin
#if njets>0: 
#    # use "guess" option for merging of processes, 
#    # works with Pythia 8.230 or higher, use 19.2.5.33.4
#    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
#    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
#        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
#    else:
#        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
#    genSeq.Pythia8.Commands += ["1000024:spinType = 1",
#                                "1000023:spinType = 1",
#                                "1000022:spinType = 1"]

