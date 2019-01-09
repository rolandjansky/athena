include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 
def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# C1/N2 denegenerate  
dM=StringToFloat(runArgs.jobConfig[0].split('_')[4])-StringToFloat(runArgs.jobConfig[0].split('_')[5])
masses['1000023'] = StringToFloat(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = StringToFloat(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = StringToFloat(runArgs.jobConfig[0].split('_')[5])

madspindecays=False
if "MadSpin" in runArgs.jobConfig[0] and dM<100.:
  madspindecays=True


# Set Gen and Decay types 
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

# MadGraph options

bwcutoff = 15
extras['bwcutoff']=bwcutoff 
"""
xqcut = 15             # low matching scale, following DM group recommendations
extras['ptj']=10       # low matching scale, following DM group recommendations #tried
extras['ptl']=0
#extras['ptj1min']=50
extras['etal']='-1.0'
extras['drll']=0.0
extras['drjl']=0.0
#extras['lhe_version']='3.0'
extras['auto_ptj_mjj']='F'
"""
extras['event_norm']='sum'
extras['use_syst']='F'

process ='''
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


# x1+x1- + 2 partons inclusive
process += '''
generate p p > x1+ x1- / susystrong @1
add process p p > x1+ x1- j / susystrong @2
add process p p > x1+ x1- j j / susystrong @3
'''
njets = 2
mergeproc="{x1+,1000024}{x1-,-1000024}"
msdecaystring="decay x1+ > l+ vl  n1\ndecay x1- > l- vl~ n1\n"

####################
print "Final process card:"
print process
# Madspin configuration
if madspindecays==True:
  madspin_card='madspin_card_C1C1_WW.dat'
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




#####################






# Pythia options
pythia = genSeq.Pythia8
pythia.Commands += ["23:mMin = 0.2"]
pythia.Commands += ["24:mMin = 0.2"]
pythia.Commands += ["-24:mMin = 0.2"]
pythia.Commands += ["23:onMode = off"]                # switch off all Z decays
pythia.Commands += ["23:onIfAny = 11 13 15"]          # switch on Z->ll
pythia.Commands += ["24:onMode = off"]                # switch off all W decays
pythia.Commands += ["24:onIfAny = 11 12 13 14 15 16"] # switch on W->lnu
pythia.Commands += ["-24:onMode = off"]                # switch off all W decays
pythia.Commands += ["-24:onIfAny = -11 -12 -13 -14 -15 -16"] # switch on W->lnu

evgenLog.info('Registered generation of ~chi1+/~chi1- production, decay via WW; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact = [ "marilea.reale@cern.ch", "primaver@cern.ch"]
evgenConfig.keywords += ['SUSY', 'gaugino', 'chargino', 'slepton', 'sneutrino']
evgenConfig.description = '~chi1+/~chi1- production, decay via WW in simplified model, m_C1N2 = %s GeV, m_N1 = %s GeV'%(masses['1000023'],masses['1000022'])

# Filter and event multiplier 
evt_multiplier = 20

if '2L8' in runArgs.jobConfig[0]:
    evgenLog.info('2lepton8 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"
   

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>%s" % mergeproc,
                               "1000024:spinType = 1",
                               "1000023:spinType = 1",
                               "1000022:spinType = 1" ]


