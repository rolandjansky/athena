#--------------------------------------------------------------
# Standard pre-include
#
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

#--------------------------------------------------------------
# Some options for local testing.  Shouldn't hurt anything in production.
#
# Nominal configuration for production: MadSpin decays, with 0,1,2+ partom emissions in the matrix element
#
# special to handle MadSpin configuration via JO name:
madspindecays=False
if any([x in runArgs.jobConfig[0] for x in ["C1pN2","C1mN2","C1C1","C1pC1m","C1mC1p"]]):
  madspindecays=True

#--------------------------------------------------------------
# Interpret the name of the JO file to figure out the mass spectrum
#
def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

#--------------------------------------------------------------
# split up the JO file input name to interpret it
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')

for iCfg in range(len(splitConfig)):
   print "opt %d: %s"%(iCfg,splitConfig[iCfg])

# decaytype is fixed. This really only tells MGC which param card to use.
# In principle this does nothing since it gets overwritten, but we leave
# it in for clarity! The JO needs to have "_WZ_" in the proper position.
decaytype='WZ' 

# interpret the generation type, so we know which processes to run.
gentype = splitConfig[2]
decaytype = splitConfig[3]
if decaytype=="WW": decaytype="WZ"
filtertype = splitConfig[-2]

# interpret the mass splittings, C1/N2 nearly degenerate
dM=MassToFloat(splitConfig[4])-MassToFloat(splitConfig[5])
masses['1000023'] = MassToFloat(splitConfig[4])
masses['1000024'] = MassToFloat(splitConfig[4])
masses['1000022'] = MassToFloat(splitConfig[5])

# custom chargino mass--technically not a filter, but is included as part
# of the string including the filter
if "C1M" in filtertype:
  C1M = re.search("C1M[0-9]*p?[0-9]*",filtertype).group(0).replace("C1M","")
  masses['1000024'] = MassToFloat(C1M)
  print ( "Using custom mass splitting. C1M = %s, masses['1000024'] = %f" % ( C1M,  masses['1000024'] ) )
else:
  print ( "Using default mass splitting. masses['1000024'] = %f" % masses['1000024'] )


# max number of jets will be two, unless otherwise specified.
njets=2

#--------------------------------------------------------------
# MadGraph options
# 
extras['ptj']=10       # low matching scale, following DM group recommendations
extras['ptl']=0
extras['etal']='-1.0'
extras['drll']=0.0
extras['drjl']=0.0
extras['lhe_version']='3.0'
extras['auto_ptj_mjj']='F'
extras['bwcutoff']=1000 # to allow very low-mass W* and Z*
extras['event_norm']='sum'
extras['use_syst']='F'
#extras['sys_alpsfact']='1 0.5 2'
#extras['sys_scalefact']='1 0.5 2'
#extras['sys_pdf']='NNPDF30_nlo_as_0118'

if "2L" in filtertype or "MET" in filtertype :
    xqcut = 15         # low matching scale, following DM group recommendations

# use minimum lead jet pT when applying MET filter
if 'MET' in filtertype :
    extras['ptj1min']=50

process='''
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
# 
#--------------------------------------------------------------

#--------------------------------------------------------------
# Configure the process definitions.
#
mgprocstring=""
mgdecaystring=""
msdecaystring=""
if gentype=="C1N2":
  mgprocstring="p p > c1 n2"
  mergeproc="{x1+,1000024}{x1-,-1000024}{n2,1000023}"
  if madspindecays:
     raise RuntimeError("MadSpin decays not possible for N2C1+ and N2C1- in the same job!")
elif gentype=="C1pN2":
  mgprocstring="p p > x1+ n2"
  msdecaystring="decay n2 > l+ l- n1\ndecay x1+ > l+ vl n1\n"
  if "2L" in filtertype: # If 2L filter is used, take C1-> ff N1
    msdecaystring="decay n2 > l+ l- n1\ndecay x1+ > f f n1\n"
  if "1L" in filtertype: # For 1L +1T analysis. Filter 1L but still force a 2L decay C1-> ff N1
    msdecaystring="decay n2 > l+ l- n1\ndecay x1+ > f f n1\n"  
  mergeproc="w+"
elif gentype=="C1mN2":
  mgprocstring="p p > x1- n2"
  msdecaystring="decay n2 > l+ l- n1\ndecay x1- > l- vl~ n1\n"
  if "2L" in filtertype: # If 2L filter is used, take C1-> ff N1
    msdecaystring="decay n2 > l+ l- n1\ndecay x1- > f f n1\n"
  if "1L" in filtertype: # For 1L +1T analysis. Filter 1L but still force a 2L decay C1-> ff N1
    msdecaystring="decay n2 > l+ l- n1\ndecay x1- > f f n1\n"
  mergeproc="w-"
elif gentype=="C1C1" or gentype=="C1pC1m" or gentype=="C1mC1p":
  mgprocstring="p p > x1- x1+"
  msdecaystring="decay x1+ > l+ vl n1\ndecay x1- > l- vl~ n1\n"
  mergeproc="z"
else:
  raise RuntimeError("Unknown process %s, aborting." % gentype)

# skip the 0 jet events when applying a MET filter
skip0Jet = ( 'MET' in filtertype )

# write out the actual processes.
processCounter = 0
for i in xrange(njets+1):      
  if i==0 and skip0Jet:
     continue
  process += "%-12s %s %-10s %s  / susystrong @%d\n" % ('generate' if processCounter==0 else 'add process', 
                                                          mgprocstring,
                                                          ' j'*i,
                                                          mgdecaystring,
                                                          processCounter+1)
  processCounter = processCounter + 1

# print the process, just to confirm we got everything right
print "Final process card:"
print process

# change this back, to use a single parameter card for the generation
gentype = "C1N2"
#
#--------------------------------------------------------------

#--------------------------------------------------------------
# Madspin configuration
#
if madspindecays==True:
  if msdecaystring=="":
    raise RuntimeError("Asking for MadSpin decays, but no decay string provided!")
  madspin_card='madspin_card_C1N2_WZ.dat'

  mscard = open(madspin_card,'w')  

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
#Some options (uncomment to apply)                                           
#                                                                            
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
set BW_cut 1000              # cut on how far the particle can be off-shell         
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event   
#
set seed %i
set spinmode none
# specify the decay for the final state particles

%s

# running the actual code                        
launch"""%(runArgs.randomSeed,msdecaystring))                    
  mscard.close()
  mergeproc+="LEPTONS,NEUTRINOS"

#--------------------------------------------------------------
# Pythia options
#
pythia = genSeq.Pythia8
pythia.Commands += ["23:mMin = 0.2"]
pythia.Commands += ["24:mMin = 0.2"]
pythia.Commands += ["23:onMode = off"]                # switch off all Z decays
pythia.Commands += ["23:onIfAny = 11 13 15"]          # switch on Z->ll
pythia.Commands += ["24:onMode = off"]                # switch off all W decays
pythia.Commands += ["24:onIfAny = 11 12 13 14 15 16"] # switch on W->lnu

# information about this generation
evgenLog.info('p p > c1n2 productions. ')
evgenConfig.contact  = [ "sara.alderweireldt@cern.ch" ]
evgenConfig.keywords += ['SUSY','gaugino', 'chargino', 'neutralino']
evgenConfig.description = 'SUSY Simplified Model with compressed chargino/neutralino production and decays via W/Z with MadGraph/MadSpin/Pythia8'

#--------------------------------------------------------------
# add some filters here
#
# include check on dM in case we just want to ask MadGraph to generate completely degenerate Higgsinos,
# e.g. to check cross sections.
#
filters=[]
evt_multiplier=1

#--------------------------------------------------------------------------------------------------------------
# lepton filter, interpreted from jobOptions
# Using 3L filter with pt_l >= 2 or 3 GeV
if "3L" in filtertype :

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 3

    # set the lepton pt filter threshold
    if "3L2" in filtertype :
        evgenLog.info('3leptons2 filter')
        filtSeq.MultiElecMuTauFilter.MinPt = 2000.         # low pt-cut on the lepton
    elif "3L3" in filtertype :
        evgenLog.info('3leptons3 filter')
        filtSeq.MultiElecMuTauFilter.MinPt = 3000.         # low pt-cut on the lepton

    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    # some samples with very small mass splittings
    # need more events to converge
    evt_multiplier = 8
    if int(dM)<5: 
      evt_multiplier *= 10
    elif int(dM)<=10: 
      evt_multiplier *= 3
    elif int(dM)<=20:
      evt_multiplier *= 2
    elif int(dM)<=70:
      evt_multiplier *= 1.5

# Using 2L filter with pt_l >= 3 GeV
if "2L" in filtertype:

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2

    if "2L2" in filtertype:
        evgenLog.info('2leptons2 filter')
        filtSeq.MultiElecMuTauFilter.MinPt = 2000.         # low pt-cut on the lepton
    elif "2L3" in filtertype:
        evgenLog.info('2leptons3 filter')
        filtSeq.MultiElecMuTauFilter.MinPt = 3000.         # low pt-cut on the lepton

    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"
    filters.append("MultiElecMuTauFilter")

    evt_multiplier = 20
    # some samples with very small mass splittings
    # need more events to converge
    if abs(dM) <= 2:
        evt_multiplier = 300
        evgenConfig.minevents = 500
    elif abs(dM) <= 3:
        evt_multiplier = 150
        evgenConfig.minevents = 2000
    elif abs(dM) <= 5:
        evt_multiplier = 100
    elif abs(dM) <= 20:
        evt_multiplier = 70
    elif abs(dM) <= 30:
        evt_multiplier = 50
        
if "1L" in filtertype:
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 1

    evgenLog.info('1leptons2 filter')
    filtSeq.MultiElecMuTauFilter.MinPt = 2000.         # low pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"
    filters.append("MultiElecMuTauFilter")

    evt_multiplier = 1
    # some samples with very small mass splittings
    # need more events to converge
    if abs(dM) <= 1:
        evt_multiplier = 20
        evgenConfig.minevents = 5000
    elif abs(dM) <= 2:
        evt_multiplier = 10
        evgenConfig.minevents = 5000
    elif abs(dM) <= 3:
        evt_multiplier = 5
        evgenConfig.minevents = 2000
    elif abs(dM) <= 5:
        evt_multiplier = 5
    elif abs(dM) <= 20:
        evt_multiplier = 5
    elif abs(dM) <= 30:
        evt_multiplier = 5

# Using MET filter
if "MET" in filtertype:
    evt_multiplier *= 2
    if not hasattr(filtSeq, "MissingEtFilter"):
        from GeneratorFilters.GeneratorFiltersConf import MissingEtFilter
        filtSeq += MissingEtFilter("MissingEtFilter")
 
    if not hasattr(filtSeq, "MissingEtFilterUpperCut"):
        filtSeq += MissingEtFilter("MissingEtFilterUpperCut")
 
    if "MET50" in filtertype:
        lowercut=50
    elif "MET75" in filtertype:
        lowercut=75

    filtSeq.MissingEtFilter.METCut = lowercut*GeV
    filtSeq.MissingEtFilterUpperCut.METCut = 100000*GeV
    filters.append("MissingEtFilter and not MissingEtFilterUpperCut")

#optimized for R21 compressed Wino/Bino grid
if "2L2MET75" in filtertype:
    evt_multiplier = 20
    # some samples with very small mass splittings
    # need more events to converge
    if abs(dM) <=1.5:
        evt_multiplier = 150
    elif abs(dM) <= 2:
        evt_multiplier = 100
    elif abs(dM) <= 3:
        evt_multiplier = 50
    elif abs(dM) <= 5:
        evt_multiplier = 25

evgenLog.info('Set event multiplier to %d'%evt_multiplier) 

for i in filters:
    if filtSeq.Expression == "":
        filtSeq.Expression=i
    else:
        filtSeq.Expression = "(%s) and (%s)" % (filtSeq.Expression,i)

#--------------------------------------------------------------
# Standard post-include
#
include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

#--------------------------------------------------------------
# Merging options
#
if njets>0:
  # "Merging:mayRemoveDecayProducts = on" -- maybe this is needed?  actually already included in PostInclude
  genSeq.Pythia8.Commands += [ "Merging:Process = pp>%s" % mergeproc,
                               "1000024:spinType = 1",
                               "1000023:spinType = 1",
                               "1000022:spinType = 1" ]
#--------------------------------------------------------------