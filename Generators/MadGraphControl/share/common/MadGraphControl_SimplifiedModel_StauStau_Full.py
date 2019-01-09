# 
# Job options for event generation with direct stau production.
# To be used for MC16.
# 
# Created for
#   https://its.cern.ch/jira/browse/ATLMCPROD-5646
# 
# (85)
# 
# $Id$
# 


### includes
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 


### helpers
def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)


### parse job options name 
# N.B. 60 chars limit on PhysicsShort name...
filename = runArgs.jobConfig[0][:-3] # cut off trailing .py
additional_options = filename.split("_")[5:]
evgenLog.info("DirectStau: Additional options: " + str(additional_options))


# Extract job settings/masses etc.
gentype    = "SlepSlep" # determines xqcut via MadGraphUtils.py from masses["1000011"]
decaytype  = "dstau"
mslep      = StringToFloat(filename.split('_')[3]) 
mn1        = StringToFloat(filename.split('_')[4])
masses['1000011'] = mslep
masses['1000013'] = mslep
masses['1000015'] = mslep
masses['2000011'] = mslep
masses['2000013'] = mslep
masses['2000015'] = mslep
masses['1000022'] = mn1

# Stau-stau + 2 partons, do stau decay in MadGraph (this is different from MC15 setup), no mixed mode
process = '''
generate    p p > ta1- ta1+    , (ta1- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @1
add process p p > ta1- ta1+ j  , (ta1- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @2
add process p p > ta1- ta1+ j j, (ta1- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @3
add process p p > ta2- ta2+    , (ta2- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @4
add process p p > ta2- ta2+ j  , (ta2- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @5
add process p p > ta2- ta2+ j j, (ta2- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @6
'''
njets = 2
useguess = True

evgenLog.info('DirectStau: generation of stau-pair production; grid point ' + str(runArgs.runNumber) + ' decoded into mass point ' + str(mslep) + ' ' + str(mn1))

evgenConfig.contact = [ "mann@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'stau']
evgenConfig.description = 'Direct stau-pair production in simplified model, m_stauLR = %s GeV, m_N1 = %s GeV' % (mslep, mn1)


# Filter and event multiplier 
evt_multiplier = 2

if '2L8' in additional_options:

    # Filter that was used in MC15
    evgenLog.info('DirectStau: 2lepton8 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 1    # include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    # set higher evt_multiplier when using filter
    evt_multiplier = 4

elif 'TFilt' in additional_options:
  
    # new filter for MC16
    evgenLog.info('DirectStau: Tau filter is applied.')
                  
    if not hasattr(filtSeq, "TauFilter" ):
        from GeneratorFilters.GeneratorFiltersConf import TauFilter
        filtSeq += TauFilter("TauFilter" )
 
    # pt and eta filter thresholds
    filtSeq.TauFilter.EtaMaxe   = 2.6 # 2.47
    filtSeq.TauFilter.EtaMaxmu  = 2.8 # 2.7
    filtSeq.TauFilter.EtaMaxhad = 2.6 # 2.5

    filtSeq.TauFilter.Ptcute    = 15.e3 # 18 GeV - 3*1 GeV
    filtSeq.TauFilter.Ptcutmu   = 13.e3 # 15 GeV - 4*0.4 GeV
    filtSeq.TauFilter.Ptcuthad  = 15.e3 # (20 GeV)

    filtSeq.TauFilter.UseNewOptions = False

    filtSeq.TauFilter.Ntaus = 2
    filtSeq.Expression = "TauFilter"    
        
    # set higher evt_multiplier when using filter
    evt_multiplier = 4

else:
  
    evgenLog.info('DirectStau: No filter is applied')

# need more at low stau masses
if mslep < 120: 
  evt_multiplier = 8


if 'mmix' in additional_options:
  
    # use maximally mixed staus
    evgenLog.info("DirectStau: Maxmix scenario selected.")
    decaytype = "dstaumaxmix" 


if 's11' in additional_options:
  
    # only do stau1 modes
    evgenLog.info("DirectStau: Only do stau1-stau1 production modes.")
    process = '''
    generate    p p > ta1- ta1+    , (ta1- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @1
    add process p p > ta1- ta1+ j  , (ta1- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @2
    add process p p > ta1- ta1+ j j, (ta1- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @3
    '''
    useguess = False
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ta1-,1000015}{ta1+,-1000015}"]

if 's22' in additional_options:
  
    # only do stau2 modes
    evgenLog.info("DirectStau: Only do stau2-stau2 production modes.")
    process = '''
    generate    p p > ta2- ta2+    , (ta2- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @1
    add process p p > ta2- ta2+ j  , (ta2- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @2
    add process p p > ta2- ta2+ j j, (ta2- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @3
    '''
    useguess = False
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ta2-,2000015}{ta2+,-2000015}"]


if 's12' in additional_options:

    # include mixed production modes
    evgenLog.info("DirectStau: Include mixed production modes.")
    process += '''
    add process p p > ta1- ta2+    , (ta1- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @7
    add process p p > ta1- ta2+ j  , (ta1- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @8
    add process p p > ta1- ta2+ j j, (ta1- > ta- n1), (ta2+ > ta+ n1)   $ susystrong @9
    add process p p > ta2- ta1+    , (ta2- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @10
    add process p p > ta2- ta1+ j  , (ta2- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @11
    add process p p > ta2- ta1+ j j, (ta2- > ta- n1), (ta1+ > ta+ n1)   $ susystrong @12
    '''


Dm = filter(lambda x: x.startswith("Dm"), additional_options)
if Dm:
  
    # introduce mass splitting
    deltaM = int(Dm[0][2:])
    masses['2000011'] = mslep + deltaM
    masses['2000013'] = mslep + deltaM
    masses['2000015'] = mslep + deltaM
    evgenLog.info('DirectStau: Mass of stau2 (etc.) set to %d GeV' % (masses['2000015']))
  

# AGENE-1542: disable syscalc and set event_norm explicitly
extras['event_norm']='sum'
extras['use_syst']='F'

# post include
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

# AGENE-1511: new CKKW-L "guess" merging features
if njets>0:
  if useguess:
    evgenLog.info('Using Merging:Process = guess')
    genSeq.Pythia8.Commands += ["Merging:Process = guess"] 
    if "UserHooks" in genSeq.Pythia8.__slots__.keys(): 
      genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO'] 
    else: 
      genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'    
  else:
    evgenLog.info('Using standard merging syntax: ' + str(genSeq.Pythia8.Commands))
    
   
