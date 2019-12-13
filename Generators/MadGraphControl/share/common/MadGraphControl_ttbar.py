from MadGraphControl.MadGraphUtils import *
import fileinput
import shutil
import subprocess
import os

# DSID list of ttbar samples
tt_nonallhad = [410225, 410274, 407045, 407046, 407047, 407048]
tt_dilepton  = [410226, 410275]
tt_allhad    = [410227,410368]

b_filter     = [410274, 410275]
HT1k5_filter     = [407045]
HT1k_1k5_filter     = [407046]
HT6c_1k_filter     = [407047]
MET200_filter     = [407048]
pT300_cut     = [410368]

# LHE production
tt_inclusive_LHE = [410440, 412121, 421109]

# for matching with H7
herwig_dsids   = [412121, 421109]

enhanced_bwcut = [410440, 412121, 421109]
#thisDSID       = runArgs.runNumber
if hasattr(runArgs,'runNumber'):
   thisDSID = int(runArgs.runNumber)
else:
   parDSID = str(runArgs.jobConfig[0])
   if len(parDSID)==6 and parDSID.isdigit():
      thisDSID = int(parDSID)
   else:
      thisDSID = int(parDSID[-6:])
print "DSID is ",thisDSID

# --------------------------------------------------------------
#  Some global production settings
# --------------------------------------------------------------
# Make some excess of events - make sure we protect against maxEvents=-1
nevents=1.1*runArgs.maxEvents if runArgs.maxEvents>0 else 5500
mode=0

lhaid=260000
pdfmin=260001
pdfmax=260100

reweight_scale = '.true.'
reweight_PDF   = '.true.'
pdflabel='lhapdf'
maxjetflavor=5
parton_shower='PYTHIA8'
muR_over_ref  = 1.0
muF1_over_ref = 1.0
muF2_over_ref = 1.0
dyn_scale = '10'    # user-defined scale -> Dominic's definition of mt+1/2*(pt^2+ptx^2)
lhe_version=3
bwcut = 15

if thisDSID in herwig_dsids:
    parton_shower='HERWIGPP'

# --------------------------------------------------------------
#  Setting up the process
# --------------------------------------------------------------

proc = 'generate p p > t t~ [QCD]'
name = ""
wdecay = ""
nLeptons = -99

# Number of leptons
if thisDSID in tt_allhad:
    nLeptons = 0
    evgenConfig.description += ', allhad'
    name = "ttbar_NLO5FS_allhad"
    wdecay = "decay t > w+ b, w+ > j j \ndecay t~ > w- b~, w- > j j \n"
elif thisDSID in tt_nonallhad:
    nLeptons = -1
    # Adjust the number of events to account of filter efficiency
    nevents = nevents*2.0
    evgenConfig.description += ', non-allhad'
    name = "ttbar_NLO5FS_nonallhad"
    wdecay = "decay t > w+ b, w+ > all all \ndecay t~ > w- b~, w- > all all \n"
elif thisDSID in tt_dilepton:
    nLeptons = 2
    evgenConfig.description += ', dilepton'
    name = "ttbar_NLO5FS_dilepton"
    wdecay = "define lv = e+ mu+ ta+ ve vm vt e- mu- ta- ve~ vm~ vt~ \ndecay t > w+ b, w+ > lv lv \ndecay t~ > w- b~, w- > lv lv \n"
elif thisDSID in tt_inclusive_LHE:
    evgenConfig.description += ', inclusive'
    nevents = 16500 # LHE production for inclusive production w/o taking filters in account, the latter depend on multiple LHE files (r20.7!)
    evgenConfig.minevents = 1
    name = "ttbar_NLO5FS_inclusive"
    wdecay = "decay t > w+ b, w+ > all all \ndecay t~ > w- b~, w- > all all \n"
else:
    raise RuntimeError("DSID %i is not recoginsed for this control files."%(thisDSID))

if thisDSID in enhanced_bwcut:
    bwcut = 50


# Adjust nEvents for the filters

if thisDSID in HT1k5_filter:
    nevents = nevents*750
if thisDSID in HT1k_1k5_filter:
    nevents = nevents*150
if thisDSID in HT6c_1k_filter:
    nevents = nevents*30
if thisDSID in MET200_filter:
    nevents = nevents*100


# B-filtering
if thisDSID in b_filter:
    nevents = nevents*25

stringy = 'madgraph.'+str(thisDSID)+'.MadGraph_'+str(name)

# Proc card writing
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm-no_b_mass
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
"""+proc+"""
output -f
""")
fcard.close()

# --------------------------------------------------------------
#  Check the beam energy
# --------------------------------------------------------------

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

# --------------------------------------------------------------
#  Start building the cards
# --------------------------------------------------------------
process_dir = new_process()

# Cook the setscales file for the user defined dynamical scale
fileN = process_dir+'/SubProcesses/setscales.f'
mark  = '      elseif(dynamical_scale_choice.eq.10) then'
rmLines = ['ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc',
           'cc      USER-DEFINED SCALE: ENTER YOUR CODE HERE                                 cc',
           'cc      to use this code you must set                                            cc',
           'cc                 dynamical_scale_choice = 0                                    cc',
           'cc      in the run_card (run_card.dat)                                           cc',
           'write(*,*) "User-defined scale not set"',
           'stop 1',
           'temp_scale_id=\'User-defined dynamical scale\' ! use a meaningful string',
           'tmp = 0',
           'cc      USER-DEFINED SCALE: END OF USER CODE                                     cc'
           ]

for line in fileinput.input(fileN, inplace=1):
    toKeep = True
    for rmLine in rmLines:
        if line.find(rmLine) >= 0:
           toKeep = False
           break
    if toKeep:
        print line,
    if line.startswith(mark):
        print """
c         Q^2= mt^2 + 0.5*(pt^2+ptbar^2)
          xm2=dot(pp(0,3),pp(0,3))
          tmp=sqrt(xm2+0.5*(pt(pp(0,3))**2+pt(pp(0,4))**2))
          temp_scale_id='mt**2 + 0.5*(pt**2+ptbar**2)'
              """

if thisDSID in pT300_cut:
    fileCuts = process_dir+'/SubProcesses/cuts.f'

    mark= 'c$$$      do i=1,nexternal   ! loop over all external particles'
    rmLines = ['c$$$      do i=1,nexternal   ! loop over all external particles']
    for line in fileinput.input(fileCuts, inplace=1):
        toKeep = True
        for rmLine in rmLines:
            if line.find(rmLine) >= 0:
                toKeep = False
                break
        if toKeep:
            print line,
        if line.startswith(mark):
            print """

c$$$C EXAMPLE: cut on top quark pT
      passcuts_user=.false.
      do i=1,nexternal
        if (istatus(i).eq.1 .and. abs(ipdg(i)).eq.6) then
c$$$C apply the pT cut
            if ( p(1,i)**2+p(2,i)**2 .gt. 300d0**2) then
c$$$C momenta do not pass cuts. Set passcuts_user to false
               passcuts_user=.true.
               return
            endif
        endif
      enddo
                  """

# Decay with MadSpin
madspin_card_loc='madspin_card.dat'
mscard = open(madspin_card_loc,'w')
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
set max_weight_ps_point 500  # number of PS to estimate the maximum for each event
set Nevents_for_max_weigth 500
set BW_cut %i
set seed %i
define j = g u c d s b u~ c~ d~ s~ b~
%s
launch
"""%(bwcut, runArgs.randomSeed, wdecay))
mscard.close()

# --------------------------------------------------------------
#  Additional run card options
# --------------------------------------------------------------
run_card_extras = { 'lhaid'         : lhaid,
                    'pdlabel'       : "'"+pdflabel+"'",
                    'parton_shower' : parton_shower,
                    'maxjetflavor'  : maxjetflavor,
                    'reweight_scale': reweight_scale,
                    'reweight_PDF'  : reweight_PDF,
                    'PDF_set_min'   : pdfmin,
                    'PDF_set_max'   : pdfmax,
                    'muR_over_ref'  : muR_over_ref,
                    'muF1_over_ref' : muF1_over_ref,
                    'muF2_over_ref' : muF2_over_ref,
                    'dynamical_scale_choice' : dyn_scale,
                    'jetalgo'   : '-1',  # use anti-kT jet algorithm
                    'jetradius' : '0.4', # set jet cone size of 0.4
                    'ptj'       : '0.1', # minimum jet pT
                    'req_acc'   : '0.001',
                    }

# Run card
build_run_card(
      run_card_old = get_default_runcard(proc_dir=process_dir),
      run_card_new = 'run_card.dat',
      nevts = nevents,
      rand_seed = runArgs.randomSeed,
      beamEnergy = beamEnergy,
      xqcut=0.,
      extras = run_card_extras
      )

# Param card
paramNameToCopy      = 'aMcAtNlo_param_card_loopsmnobmass.dat' #'aMcAtNlo_param_card_tt.dat'
paramNameDestination = 'param_card.dat'
paramcard            = subprocess.Popen(['get_files','-data',paramNameToCopy])
paramcard.wait()
if not os.access(paramNameToCopy,os.R_OK):
    raise RuntimeError("ERROR: Could not get %s"%(paramNameToCopy))
shutil.copy(paramNameToCopy,paramNameDestination)

# Print the cards
print_cards()

# --------------------------------------------------------------                                                                                                                    # Set everything up and run
# --------------------------------------------------------------
generate(run_card_loc='run_card.dat',
         param_card_loc='param_card.dat',
         madspin_card_loc='madspin_card.dat',
         mode=mode,
         proc_dir=process_dir,
         run_name=name,
         required_accuracy=0.001)

outputDS=arrange_output(run_name=name,
               proc_dir=process_dir,
               outputDS=stringy+'._00001.events.tar.gz',
               lhe_version=lhe_version)


# --------------------------------------------------------------
# Run Pythia 8 Showering
# --------------------------------------------------------------

if not thisDSID in herwig_dsids:
    evgenConfig.generators    += ["aMcAtNlo", "Pythia8"]
    evgenConfig.description    = 'MG5_aMC@NLO+Pythia8+EvtGen '+name+' OTF, A14 NNPDF 2.3 LO, ME NNPDF 3.0 NLO, using scale sqrt(sum_i mT(i)**2/2)), for i = top quarks'
    evgenConfig.keywords      += [ 'SM', 'top']
    evgenConfig.contact        = [ 'ian.connelly@cern.ch', 'steffen.henkelmann@cern.ch' ]
    runArgs.inputGeneratorFile=outputDS    
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
    include("Pythia8_i/Pythia8_aMcAtNlo.py")
elif thisDSID in herwig_dsids:
    runArgs.inputGeneratorFile=outputDS
    evgenConfig.generators += ["aMcAtNlo", "Herwig7"]
    evgenConfig.description = 'MG5_aMC@NLO+Herwig7+EvtGen '+name+' OTF, H7p1 default tune, ME NNPDF 3.0 NLO, using scale sqrt(sum_i mT(i)**2/2)), for i = top quarks'
    evgenConfig.tune = "H7.1-Default"
    include("Herwig7_i/Herwig7_LHEF.py")
    Herwig7Config.me_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")
    Herwig7Config.tune_commands()
    Herwig7Config.lhef_mg5amc_commands(lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")
    include("Herwig7_i/Herwig71_EvtGen.py")
    Herwig7Config.run()
else:
    theApp.finalize()
    theApp.exit()




# --------------------------------------------------------------
# Apply TTbarWToLeptonFilter
# --------------------------------------------------------------
# Only truely needed for non-all hadronic decays
if nLeptons != -99:
    include("GeneratorFilters/TTbarWToLeptonFilter.py")
    filtSeq.TTbarWToLeptonFilter.NumLeptons = nLeptons
    filtSeq.TTbarWToLeptonFilter.Ptcut      = 0.0


# --------------------------------------------------------------
# If required by JO, apply b-filter
# --------------------------------------------------------------
if thisDSID in b_filter:
    include('GeneratorFilters/TTbarPlusHFFilter.py')
    filtSeq.TTbarPlusBFilter.SelectB   = True

if thisDSID in MET200_filter:
  include('GeneratorFilters/MissingEtFilter.py')
  filtSeq.MissingEtFilter.METCut = 200*GeV

if thisDSID in HT6c_1k_filter:
  include('GeneratorFilters/HTFilter.py')
  filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
  filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
  filtSeq.HTFilter.MinHT = 600.*GeV # Min HT to keep event
  filtSeq.HTFilter.MaxHT = 1000.*GeV # Max HT to keep event
  filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
  filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
  filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT

if thisDSID in HT1k_1k5_filter:
  include('GeneratorFilters/HTFilter.py')
  filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
  filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
  filtSeq.HTFilter.MinHT = 1000.*GeV # Min HT to keep event
  filtSeq.HTFilter.MaxHT = 1500.*GeV # Max HT to keep event
  filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
  filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
  filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT

if thisDSID in HT1k5_filter:
  include('GeneratorFilters/HTFilter.py')
  filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
  filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
  filtSeq.HTFilter.MinHT = 1500.*GeV # Min HT to keep event
  filtSeq.HTFilter.MaxHT = 9000.*GeV # Max HT to keep event
  filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
  filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
  filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT
