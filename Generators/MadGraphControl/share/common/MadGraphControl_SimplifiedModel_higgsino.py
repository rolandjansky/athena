#--------------------------------------------------------------
# Standard pre-include
#
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

# uncomment this if you want to keep MadGraph tarball, e.g. for Feynman diagrams
# keepOutput=True
#--------------------------------------------------------------

#--------------------------------------------------------------
# Some options for local testing.  Shouldn't hurt anything in production.
#
# Nominal configuration for production: MadSpin decays, with 0,1,2+ parton emissions in the matrix element
#
if 'oldproc'        not in dir(): oldproc=False
if 'madgraphdecays' not in dir(): madgraphdecays=False
if 'pythiadecays'   not in dir(): pythiadecays=False
if 'newnjets'       not in dir(): newnjets=2

# special to handle MadSpin configuration via JO name:
madspindecays=False
if "MadSpin" in runArgs.jobConfig[0] and not pythiadecays and not madgraphdecays: 
  madspindecays=True
#--------------------------------------------------------------

#--------------------------------------------------------------
# Interpret the name of the JO file to figure out the mass spectrum
#
def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# split up the JO file input name to interpret it
# assumes names are like: MC15.xxxxxx.MGPy8EG_A14N23LO_SM_N2N1_302_300_2LMET50_MadSpin.py
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
config_after_UEtune="_".join(splitConfig[2:])

# interpret the generation type, so we know which processes to run.
gentype = splitConfig[3]

# Tells MGC which param card to use.
decaytype='N2_ZN1'
if "2L8" in config_after_UEtune and gentype=="C1C1":
  print ("Setting decay type to WW in Wino-like model")
  decaytype='WW'
else :
  print ("Keeping default Higgsino decay type: %s" % decaytype)
 

# interpret the mass splittings, C1/N2 nearly degenerate
dM=MassToFloat(splitConfig[4])-MassToFloat(splitConfig[5])
masses['1000023'] = -1.*MassToFloat(splitConfig[4])
masses['1000024'] = (MassToFloat(splitConfig[4])+MassToFloat(splitConfig[5]))/2.
masses['1000022'] = MassToFloat(splitConfig[5])
if "C1M" in config_after_UEtune:
  C1M = re.search("C1M[0-9]*p?[0-9]*",config_after_UEtune).group(0).replace("C1M","")
  masses['1000024'] = MassToFloat(C1M)
  print ( "Using custom mass splitting. C1M = %s, masses['1000024'] = %f" % ( C1M,  masses['1000024'] ) )
elif "2L8" in config_after_UEtune and gentype=="C1C1":
  masses['1000024'] = MassToFloat(splitConfig[4])
  print ( "Using non-default C1 mass. masses[1000024] = %f" % masses['1000024'] )
else:
  print ( "Using default mass splitting. masses['1000024'] = %f" % masses['1000024'] )
           
# will be two, unless otherwise specified.
njets=newnjets

# if we're doing madgraph decays, don't do multijets.  For your own good.
if madgraphdecays or oldproc:
  njets=0
#--------------------------------------------------------------

#--------------------------------------------------------------
# MadGraph options
#
bwcutoff = 1000
xqcut = 15             # low matching scale, following DM group recommendations
extras['ptj']=10       # low matching scale, following DM group recommendations
extras['ptl']=0
extras['ptj1min']=50
extras['etal']='-1.0'
extras['drll']=0.0
extras['drjl']=0.0
extras['lhe_version']='3.0'
extras['auto_ptj_mjj']='F'
extras['bwcutoff']=bwcutoff # to allow very low-mass W* and Z*
extras['event_norm']='sum'
extras['use_syst']='F'
#extras['sys_alpsfact']='1 0.5 2'
#extras['sys_scalefact']='1 0.5 2'
#extras['sys_pdf']='NNPDF30_nlo_as_0118'

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
if oldproc==True:
  xqcut = 0
  extras['ptj']=0
  # for the old production, use an older param card
  decaytype='N2_ZN1_oldBRs'
  madspindecays=False
  pythiadecays=True

  process += '''
generate p p > n2 n1 $ susystrong
add process p p > n2 c1 $ susystrong
add process p p > c1 c1 $ susystrong
add process p p > c1 n1 $ susystrong
'''
else:
  mgprocstring=""
  mgdecaystring=""
  msdecaystring=""
  if gentype=="N2C1" or gentype=="C1N2":
    mgprocstring= "p p > n2 c1"
    mergeproc="{n2,1000023}{x1+,1000024}{x1-,-1000024}"
    if madspindecays:
      raise RuntimeError("MadSpin decays not possible for N2C1+ and N2C1- in the same job!")
    elif madgraphdecays:
      mgdecaystring=", (n2 > l+ l- n1 $ susystrong sleptons), (c1 > all all n1 $ susystrong sleptons)"

  elif gentype=="N2C1p" or gentype=="C1pN2":
    mergeproc="w+"

    if ( '3L' in config_after_UEtune ) :
      msdecaystring="decay n2 > l+ l- n1\ndecay x1+ > l+ vl n1\n"
    else :
      msdecaystring="decay n2 > l+ l- n1\ndecay x1+ > f f n1\n"

    mgprocstring="p p > n2 x1+"
    if madgraphdecays:
      if ( '3L' in config_after_UEtune ) :
        mgdecaystring=", (n2 > l+ l- n1 $ susystrong sleptons), (x1+ > l+ vl n1 $ susystrong sleptons)"
      else :
        mgdecaystring=", (n2 > l+ l- n1 $ susystrong sleptons), (x1+ > all all n1 $ susystrong sleptons)"

  elif gentype=="N2C1m" or gentype=="C1mN2":
    mergeproc="w-"

    if ( '3L' in config_after_UEtune ) :
      msdecaystring="decay n2 > l+ l- n1\ndecay x1- > l- vl~ n1\n"
    else :
      msdecaystring="decay n2 > l+ l- n1\ndecay x1- > f f n1\n"

    mgprocstring="p p > n2 x1-"
    if madgraphdecays:
      if ( '3L' in config_after_UEtune ) :
        mgdecaystring=", (n2 > l+ l- n1 $ susystrong sleptons), (x1- > l- vl~ n1 $ susystrong sleptons)"
      else :
        mgdecaystring=", (n2 > l+ l- n1 $ susystrong sleptons), (x1- > all all n1 $ susystrong sleptons)"

  elif gentype=="C1C1":
    mergeproc="z"
    msdecaystring="decay x1+ > l+ vl  n1\ndecay x1- > l- vl~ n1\n"
    mgprocstring="p p > x1+ x1-"
    if madgraphdecays:
      mgdecaystring=", (x1+ > l+ vl n1 $ susystrong sleptons), (x1- > l- vl~ n1 $ susystrong sleptons)"

  elif gentype=="N2N1" or gentype=="N1N2":
    mergeproc="z"
    msdecaystring="decay n2 > l+ l-  n1"
    mgprocstring="p p > n2 n1"
    if madgraphdecays:
      mgdecaystring=", (n2 > l+ l- n1 $ susystrong sleptons)"

  elif gentype=="C1N1" or gentype=="N1C1":
    if madspindecays:
      raise RuntimeError("MadSpin decays not possible for N1C1+ and N1C1- in the same job!")
    mergeproc="{x1+,1000024}{x1-,-1000024}{n1,1000022}"
    mgprocstring="p p > c1 n1"
    if madspindecays:
      raise RuntimeError("MadSpin decays not possible for N1C1+ and N1C1- in the same job!")
    if madgraphdecays:
      mgdecaystring=", (c1 > lv lv n1 $ susystrong sleptons)"

  else:
    raise RuntimeError("Unknown process %s, aborting." % gentype)
 
  # skip the 0 jet events when applying a MET filter
  skip0Jet = ( 'MET' in config_after_UEtune )

  # write out the actual processes.
  processCounter = 0
  for i in xrange(njets+1):
    if i==0 and skip0Jet :
      continue
    process += "%-12s %s %-10s %s  / susystrong @%d\n" % ('generate' if processCounter==0 else 'add process', 
                                                          mgprocstring,
                                                          ' j'*(i),
                                                          mgdecaystring,
                                                          processCounter+1)
    processCounter = processCounter + 1


# print the process, just to confirm we got everything right
print "Final process card:"
print process

# change this back, to use a single parameter card for the generation
if "2L8" in config_after_UEtune and gentype=="C1C1":
  gentype = "C1C1"
else:
  gentype = "C1N1"


#
#--------------------------------------------------------------

#--------------------------------------------------------------
# Madspin configuration
#
if madspindecays==True:
  if msdecaystring=="":
    raise RuntimeError("Asking for MadSpin decays, but no decay string provided!")
  madspin_card='madspin_card_higgsino.dat'

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
set BW_cut %i                # cut on how far the particle can be off-shell         
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event   
#
set seed %i
set spinmode none
# specify the decay for the final state particles

%s

# running the actual code                       
launch"""%(bwcutoff,runArgs.randomSeed,msdecaystring))                   
  mscard.close()
  mergeproc+="LEPTONS,NEUTRINOS"
#
#--------------------------------------------------------------


#--------------------------------------------------------------
# Pythia options
#
pythia = genSeq.Pythia8
pythia.Commands += ["23:mMin = 0.2"]
pythia.Commands += ["24:mMin = 0.2"]
pythia.Commands += ["-24:mMin = 0.2"]

# information about this generation
evgenLog.info('p p > c1n1/c1n2/c1c1/n2n1 productions. ')
evgenConfig.contact  = [ "michael.hance@cern.ch" ]
evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino']
evgenConfig.description = 'SUSY Simplified Model with compressed chargino/neutralino production and decays via W/Z with MadGraph/MadSpin/Pythia8/EvtGen'
#--------------------------------------------------------------

#--------------------------------------------------------------
# add some filters here
#
# include check on dM in case we just want to ask MadGraph to generate completely degenerate Higgsinos,
# e.g. to check cross sections.
#
filters=[]
evt_multiplier=1

# lepton filter, interpreted from jobOptions
nleptonsfilterreq=0
if '1L' in config_after_UEtune:
  nleptonsfilterreq=1
elif '2L' in config_after_UEtune:
  nleptonsfilterreq=2
elif '3L' in config_after_UEtune:
  nleptonsfilterreq=3
if '1L' in config_after_UEtune:
  if nleptonsfilterreq>0 and dM>0:
    evt_multiplier *= 2
    if dM<10: 
      evt_multiplier *= 1.5

    # some samples with very small mass splittings
    # need more events to converge
    if splitConfig[3]=="C1C1":
      if int(dM)==5:
        evt_multiplier *= 3
        evgenConfig.minevents = 2000
      if int(dM)==3:
        evt_multiplier *= 6
        evgenConfig.minevents = 500
      if int(dM)==2:
        evt_multiplier *= 10
        evgenConfig.minevents = 200
    else:
      if int(dM)==3:
        evt_multiplier *= 1
        evgenConfig.minevents = 5000
      elif int(dM)<=2:
        evt_multiplier *= 1.5
        evgenConfig.minevents = 5000
        if abs(masses['1000023'])>=300:
          evt_multiplier *= 1.3
    evt_multiplier *= 1.25
    from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
    filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter")
    ElecMuTauFilter = filtSeq.MultiElecMuTauFilter
    ElecMuTauFilter.MinPt = 3000.
    ElecMuTauFilter.MaxEta = 2.8
    ElecMuTauFilter.NLeptons = nleptonsfilterreq
    ElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    if nleptonsfilterreq>1:
      # if there's a Z in the event, and we have pythia decaying things, the only way to get 2 leptons is to have Z->ll
      pythia.Commands += [
        "23:onMode = off", #switch off all Z decays
        "23:onIfAny = 11 13 15", # switch on Z->ll
        ]

    ### Above are the defaults. Now exceptions
    if "2L8" in config_after_UEtune:
      print( "2lepton8 filter is applied" )
      ElecMuTauFilter.MinPt = 8000.
      evt_multiplier = 20 # temporary, should be set based on dM, as above
    if "2L2" in config_after_UEtune:
      print( "2lepton2 filter is applied" )
      ElecMuTauFilter.MinPt = 2000.
    if "3L2" in config_after_UEtune:
      print( "3lepton2 filter is applied" )
      ElecMuTauFilter.MinPt = 2000.

    filters.append("MultiElecMuTauFilter")
else:
 if nleptonsfilterreq>0 and dM>0:
    evt_multiplier *= 4
    if dM<10: 
      evt_multiplier *= 1.5

    # some samples with very small mass splittings
    # need more events to converge
    if splitConfig[3]=="C1C1":
      if int(dM)==5:
        evt_multiplier *= 3
        evgenConfig.minevents = 2000
      if int(dM)==3:
        evt_multiplier *= 6
        evgenConfig.minevents = 500
      if int(dM)<=2:
        evt_multiplier *= 10
        evgenConfig.minevents = 200
    else:
      if int(dM)==3:
        evt_multiplier *= 2
        evgenConfig.minevents = 2000
      elif int(dM)<=2:
        evt_multiplier *= 4
        evgenConfig.minevents = 500
        if abs(masses['1000023'])>=300:
          evt_multiplier *= 1.3

    from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
    filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter")
    ElecMuTauFilter = filtSeq.MultiElecMuTauFilter
    ElecMuTauFilter.MinPt = 3000.
    ElecMuTauFilter.MaxEta = 2.8
    ElecMuTauFilter.NLeptons = nleptonsfilterreq
    ElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    if nleptonsfilterreq>1:
      # if there's a Z in the event, and we have pythia decaying things, the only way to get 2 leptons is to have Z->ll
      pythia.Commands += [
        "23:onMode = off", #switch off all Z decays
        "23:onIfAny = 11 13 15", # switch on Z->ll
        ]

    ### Above are the defaults. Now exceptions
    if "2L8" in config_after_UEtune:
      print( "2lepton8 filter is applied" )
      ElecMuTauFilter.MinPt = 8000.
      evt_multiplier = 20 # temporary, should be set based on dM, as above
    if "2L2" in config_after_UEtune:
      print( "2lepton2 filter is applied" )
      ElecMuTauFilter.MinPt = 2000.
    if "3L2" in config_after_UEtune:
      print( "3lepton2 filter is applied" )
      ElecMuTauFilter.MinPt = 2000.

    filters.append("MultiElecMuTauFilter")

# MET filter, interpreted from jobOptions
if 'MET' in config_after_UEtune and dM>0:
    evt_multiplier *= 10
    if not hasattr(filtSeq, "MissingEtFilter"):
      from GeneratorFilters.GeneratorFiltersConf import MissingEtFilter
      filtSeq += MissingEtFilter("MissingEtFilter")
     
    if not hasattr(filtSeq, "MissingEtFilterUpperCut"):
      filtSeq += MissingEtFilter("MissingEtFilterUpperCut")
     
    lowercut=int(runArgs.jobConfig[0][runArgs.jobConfig[0].find('MET')+3:runArgs.jobConfig[0].find('MET')+5])
   
    filtSeq.MissingEtFilter.METCut = lowercut*GeV
    filtSeq.MissingEtFilterUpperCut.METCut = 100000*GeV
    filters.append("MissingEtFilter and not MissingEtFilterUpperCut")

# only add these filters if we have some non-zero mass splitting.  dM=0 is a special case used to test
# output cross sections against standard benchmarks
if dM>0:
  for i in filters:
    if filtSeq.Expression == "":
      filtSeq.Expression=i
    else:
      filtSeq.Expression = "(%s) and (%s)" % (filtSeq.Expression,i)
#--------------------------------------------------------------

#--------------------------------------------------------------
# Standard post-include
#
include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')
#--------------------------------------------------------------

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