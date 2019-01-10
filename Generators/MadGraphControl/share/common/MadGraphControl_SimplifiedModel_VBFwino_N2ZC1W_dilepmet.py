#//////////////////////////////////////////////////////////////////////////////
# Standard pre-include
#--------------------------------------------------------------
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )
keepOutput=False
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Some additional options
# Nominal conditions:
#   -- Pythia parton shower
#   -- 2 parton emmission in matrix element
#   -- Don't use madgraph decays
#   -- Using madspin is ideal for correct m(ll) distributions
#--------------------------------------------------------------
if 'madgraphdecays' not in dir(): madgraphdecays=False
if 'pythiadecays'   not in dir(): pythiadecays=True
if 'newnjets'       not in dir(): newnjets=2
# special to handle MadSpin configuration via JO name:
madspindecays=False
if "MadSpin" in runArgs.jobConfig[0]:
    madspindecays=True
    pythiadecays=False
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Intrepret Masses
#--------------------------------------------------------------
def MassToFloat(s):
    if "p" in s:
        return float(s.replace("p", "."))
    return float(s)

# split up the JO file input name to interpret it
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
# decaytype is fixed.  This really only tells MGC which param card to use.
decaytype='WZ' # Pure Wino-Bino 
# interpret the mass splittings, C1/N2 nearly degenerate
dM=MassToFloat(splitConfig[4])-MassToFloat(splitConfig[5])
print "Mass splitting = " , dM
masses['1000022'] =  1.0 * MassToFloat(splitConfig[5]) # LSP mass (bino)
masses['1000023'] =  1.0 * MassToFloat(splitConfig[4]) # N2, wino-like
masses['1000024'] =  1.0 * MassToFloat(splitConfig[4]) # C1 = N2, wino-like

# interpret the generation type, so we know which processes to run.
gentype = splitConfig[3]
if not 'VBF' in gentype:
    raise RuntimeError("This control file is only for Wino-Bino VBF production")
gentype = gentype.rstrip('VBF')

# will be two, unless otherwise specified.
njets=newnjets
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# MadGraph5 Options
#--------------------------------------------------------------
xqcut = 15             # low matching scale, following DM group recommendations
extras['ptj']=12
extras['ptl']=0
extras['etal']='-1.0'
extras['drll']=0.0
extras['drjl']=0.0
extras['mmjj']=200 # Enrich with target process
#extras['deltaeta']=3.0
extras['lhe_version']='3.0'
extras['auto_ptj_mjj']='F'
extras['bwcutoff']=100 # To allow very low-mass W* and Z*
extras['event_norm']='sum'
extras['use_syst']='F'

process='''
define c1 = x1+ x1-
define w = w+ w-
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define lv = e+ mu+ ta+ e- mu- ta- ve vm vt ve~ vm~ vt~
define f = e+ mu+ ta+ e- mu- ta- ve vm vt ve~ vm~ vt~ u u~ d d~ c c~ s s~ b b~ g
'''
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Configure the process definitions.
#--------------------------------------------------------------
mgprocstring=""
if 'C1C1' in gentype:
    mergeproc="Merging:Process = pp>{n2,1000023}{x1+,1000024}{x1-,-1000024}"
    msdecaystring="decay x1+ > l+ vl  n1\ndecay x1- > l- vl~  n1\n" # C1C1 decay to l nu l nu gives us our target 2L final state
    mgprocstring="p p > x1+ x1- j j"
elif 'N2C1m' in gentype or 'C1mN2' in gentype:
    mergeproc="Merging:Process = pp>{n2,1000023}{x1-,-1000024}"
    msdecaystring="decay n2 > l+ l- n1\ndecay x1- > j j n1\n" # N2 to leptons, C1 to hadrons
    mgprocstring="p p > n2 x1- j j"
elif 'N2C1p' in gentype or 'C1pN2' in gentype:
    mergeproc="Merging:Process = pp>{n2,1000023}{x1+,1000024}"
    msdecaystring="decay n2 > l+ l- n1\ndecay x1+ > j j n1\n" # N2 to leptons, C1 to hadrons
    mgprocstring="p p > n2 x1+ j j"
else:
    raise RuntimeError("Unknown process %s, aborting." % gentype)

if 'QED' in gentype:
    # Want to isolate the QED contribution, turn off QCD
    process += "%-12s %s / susystrong QED=99 QCD=0 @%d\n" % ('generate', mgprocstring, 1)
    evgenLog.info("Generating pure QED process!")
else:
    # Inclusive process, by default
    process += "%-12s %s / susystrong QED=99 QCD=99 @%d\n" % ('generate', mgprocstring, 1)
    evgenLog.info("Generating inclusive process!")

# print the process, just to confirm we got everything right
print "Final process card:"
print process

# change this back, to use a single parameter card for the generation
gentype = "C1N2" # Use given pure Wino-Bino param file
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Madspin configuration
#--------------------------------------------------------------
if madspindecays==True:
    if msdecaystring=="":
        raise RuntimeError("Asking for MadSpin decays, but no decay string provided!")
    madspin_card='madspin_card_vbf_higgsino.dat'
    mscard = open(madspin_card,'w')

    mscard.write("""
#************************************************************
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
launch""" % (runArgs.randomSeed,msdecaystring))

    mscard.close()
    mergeproc+="LEPTONS,NEUTRINOS"
#
#--------------------------------------------------------------


#//////////////////////////////////////////////////////////////////////////////
# Pythia options
#--------------------------------------------------------------
pythia = genSeq.Pythia8
pythia.Commands += ["23:mMin = 0.2"]
pythia.Commands += ["24:mMin = 0.2"]
pythia.Commands += ["-24:mMin = 0.2"]

# information about this generation
evgenLog.info('Registered generation of wino-bino like ~chi2/~chi1/~chi1+/~chi1- production via VBF, decay via SM W/Z to 2 leptons; grid point '+str(runArgs.runNumber)+' decoded into LSP mass point ' +str(masses['1000022'])+ ' with ' +str(dM)+ ' GeV splitting')
evgenConfig.contact  = [ "andrew.todd.aukerman@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'VBF', 'MSSM', 'chargino', 'neutralino', 'winobino']
evgenConfig.description = 'SUSY Simplified Model with compressed higgsino-like chargino/neutrlino with VBF production and decays via a SM W/Z to 2L'
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Adding some filters - MET and Lepton Filters
# include check on dM in case we just want to ask MadGraph to generate completely degenerate Wino-Binos,
# e.g. to check cross sections.
#--------------------------------------------------------------
evt_multiplier = 1
filters=[]

# lepton filter, interpreted from jobOptions
nleptonsfilterreq=0
if   '1L' in runArgs.jobConfig[0]:
    nleptonsfilterreq=1
elif '2L' in runArgs.jobConfig[0]:
    nleptonsfilterreq=2

if (nleptonsfilterreq > 0) and (dM > 0):
    evt_multiplier *= 2
    if   dM>=20:
        evt_multiplier *= 2
    if   dM>=10:
        evt_multiplier *= 3
    elif dM>=5:
        evt_multiplier *= 4
    elif dM>=4:
        evt_multiplier *= 5
    elif dM>=3:
        evt_multiplier *= 8
    elif dM>=2:
        evt_multiplier *= 10 
    elif dM>=1:
        evt_multiplier *= 15
    else:
        raise RuntimeError("Couldn't properly set evt_multiplier (mass splitting below 1 GeV not currently allowed), exiting")


    from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter

    filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter")
    ElecMuTauFilter = filtSeq.MultiElecMuTauFilter
    ElecMuTauFilter.MinPt = 2000.
    ElecMuTauFilter.MaxEta = 2.8
    ElecMuTauFilter.NLeptons = nleptonsfilterreq
    ElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filters.append("MultiElecMuTauFilter")

# MET filter, interpreted from jobOptions
if 'MET' in runArgs.jobConfig[0] and dM > 0:
    if not hasattr(filtSeq, "MissingEtFilter"):
        from GeneratorFilters.GeneratorFiltersConf import MissingEtFilter
        filtSeq += MissingEtFilter("MissingEtFilter")

    if not hasattr(filtSeq, "MissingEtFilterUpperCut"):
        filtSeq += MissingEtFilter("MissingEtFilterUpperCut")

    lowercut = int(runArgs.jobConfig[0][runArgs.jobConfig[0].find('MET')+3:runArgs.jobConfig[0].find('MET')+5])

    filtSeq.MissingEtFilter.METCut          = lowercut*GeV
    filtSeq.MissingEtFilterUpperCut.METCut  = 100000*GeV
    filters.append("MissingEtFilter and not MissingEtFilterUpperCut")

evgenLog.info("evt_multiplier == " + str(evt_multiplier))
# only add these filters if we have some non-zero mass splitting.  dM=0 is a special case used to test
# output cross sections against standard benchmarks
if dM > 0:
    for i in filters:
        if filtSeq.Expression == "":
            filtSeq.Expression=i
        else:
            filtSeq.Expression = "(%s) and (%s)" % (filtSeq.Expression,i)
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Standard post-include
#--------------------------------------------------------------
include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')
#//////////////////////////////////////////////////////////////////////////////


#//////////////////////////////////////////////////////////////////////////////
# Merging options
#--------------------------------------------------------------
if njets > 0:
    genSeq.Pythia8.Commands += [ "%s" % mergeproc,
                                 "1000024:spinType = 1",
                                 "1000023:spinType = 1",
                                 "1000022:spinType = 1" ]
#//////////////////////////////////////////////////////////////////////////////
