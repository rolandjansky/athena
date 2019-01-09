include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')

#C1/N2 degenerate
masses['1000024'] = MassToFloat(splitConfig[4])
masses['1000023'] = MassToFloat(splitConfig[4])
masses['1000022'] = MassToFloat(splitConfig[5])
if masses['1000022']<0.5: masses['1000022']=0.5

#will be C1N2
gentype = splitConfig[2]
#will be WZ
decaytype = splitConfig[3]

njets = 2

#----------------------------------------------------------------------------------------------------
extras['event_norm']='sum'
extras['use_syst']='F'   

#----------------------------------------------------------------------------------------------------
mgprocstring = ""                                       
mgdecaystring = ""                                      
mergeproc = ""                                          
if gentype=='C1N2':                                     
   mgprocstring = 'p p > x1 n2'                         
   mergeproc = '{x1+,1000024}{x1-,-1000024}{n2,1000023}'
elif gentype=='C1pN2': 
   mgprocstring = 'p p > x1+ n2' 
   mergeproc = '{x1+,1000024}{n2,1000023}'
elif gentype=='C1mN2': 
   mgprocstring = 'p p > x1- n2' 
   mergeproc = '{x1-,-1000024}{n2,1000023}'

#----------------------------------------------------------------------------------------------------
# write out the actual processes.
process = ""
if gentype=='C1N2':
   process = '''
   generate p p > x1+ n2 $ susystrong @1
   add process p p > x1- n2 $ susystrong @1
   add process p p > x1+ n2 j $ susystrong @2
   add process p p > x1- n2 j $ susystrong @2
   add process p p > x1+ n2 j j $ susystrong @3
   add process p p > x1- n2 j j $ susystrong @3
   '''
else:
   for i in xrange(njets+1):     
      process += "%-12s %s %-10s %s / susystrong @%d\n" % ('generate' if i==0 else 'add process', 
                                                             mgprocstring,
                                                             ' j'*i,
                                                             mgdecaystring,
                                                             i+1)
print "Final process card:"
print process

gentype = 'C1N2'

#process = '''
#generate p p > x1+ n2 $ susystrong @1
#add process p p > x1- n2 $ susystrong @1
#add process p p > x1+ n2 j $ susystrong @2
#add process p p > x1- n2 j $ susystrong @2
#add process p p > x1+ n2 j j $ susystrong @3
#add process p p > x1- n2 j j $ susystrong @3
#'''

evgenLog.info('Registered generation of ~chi1+/- ~chi20 production, decay via WZ; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact  = [ "jmiguens@cern.ch","Valentina.Tudorache@cern.ch"]
evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino']
evgenConfig.description = '~chi1+/- ~chi20 production, decay via WZ in simplified model, m_C1N2 = %s GeV, m_N1 = %s GeV'%(masses['1000024'],masses['1000022'])

genSeq.Pythia8.Commands += [ "24:mMin = 0.2", "23:mMin = 0.2" ]

if len(splitConfig)>6 and ('3L' == splitConfig[6] or '3lep' in splitConfig[6]): # Use '3L' for new samples
    evgenLog.info('only 3lepton processes will be generated')
    genSeq.Pythia8.Commands += [
        "23:onMode = off", #switch off all Z decays
        "23:onIfAny = 11 13 15", # switch on Z->ll
        "24:onMode = off", #switch off all W decays
        "24:onIfAny = 11 12 13 14 15 16" # switch on W->lnu
        ]
elif len(splitConfig)>6 and '2L2J' == splitConfig[6]:
    evgenLog.info('only 2lepton+2jet processes will be generated')
    genSeq.Pythia8.Commands += [
        "23:onMode = off", #switch off all Z decays
        "23:onIfAny = 11 13 15", # switch on Z->ll
        "24:onMode = off", #switch off all W decays
        "24:onIfAny = 1 2 3 4 5" # switch on W->qqbar
        ]
else:
    evgenLog.info('inclusive processes will be generated')

#--------------------------------------------------------------
# add some filter here
#--------------------------------------------------------------

# need more events from MG due to filter - this needs to be set before MadGraphControl_SimplifiedModelPostInclude.py is run (it's set at 2 there)
# 3 is only sufficient for large mass splittings
evt_multiplier = 3

#filter for 3lep with low dM
if '2L4andJ20or2L7' in splitConfig[-1]:
    evgenLog.info('(2leptons4 and jet20) or 2leptons7 filter')

    from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
    filtSeq += MultiElecMuTauFilter("DileptonFilterLow")
    filtSeq += MultiElecMuTauFilter("DileptonFilterHigh")
    
    MultiElecMuTauFilter1 = filtSeq.DileptonFilterLow
    MultiElecMuTauFilter1.NLeptons  = 2
    MultiElecMuTauFilter1.MinPt = 4000            # low pt-cut on the lepton
    MultiElecMuTauFilter1.MaxEta = 2.8            # stay away from MS 2.7 just in case
    MultiElecMuTauFilter1.IncludeHadTaus = 0      # don't include hadronic taus
    
    MultiElecMuTauFilter2 = filtSeq.DileptonFilterHigh
    MultiElecMuTauFilter2.NLeptons  = 2
    MultiElecMuTauFilter2.MinPt = 7000             #high pt cut on the lepton  
    MultiElecMuTauFilter2.MaxEta = 2.8             # stay away from MS 2.7 just in case
    MultiElecMuTauFilter2.IncludeHadTaus = 0       # don't include hadronic taus

    include( 'GeneratorFilters/AntiKt4TruthWZJets.py')
    include( 'GeneratorFilters/JetFilter_Fragment.py')
    filtSeq.QCDTruthJetFilter.MinPt  = 20000.          # low pt-cut on the jet
    filtSeq.QCDTruthJetFilter.MaxEta = 5.0             # eta-cut on the jet
    filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt4TruthWZJets"

    filtSeq.Expression = "(DileptonFilterLow and QCDTruthJetFilter) or DileptonFilterHigh"

    evt_multiplier = 6

elif '2L7' in splitConfig[-1]:
    evgenLog.info('2leptons7 filter')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 7000.         # high pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"
    
    evt_multiplier = 6

elif '2L3andMET100' in "%s_%s" % (splitConfig[-2], splitConfig[-1]):

    evgenLog.info('2lepton3 and MET 100 filter')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 3000.         # high pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 100*GeV

    filtSeq.Expression = "MultiElecMuTauFilter and MissingEtFilter"

    evt_multiplier = 100.
    
#filter for 1 Lepton
elif '1L' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1lepton  filter is applied')

    #Cut parameters to keep events with at least one lepton
    include ('GeneratorFilters/OneLeptonFilter.py' )
    filtSeq.WZtoLeptonFilter.NeedWZleps = 1
    filtSeq.WZtoLeptonFilter.ElectronMuonNumber = 0
    filtSeq.WZtoLeptonFilter.BCKGvsSIGNAL = 0
    filtSeq.WZtoLeptonFilter.IdealReconstructionEfficiency = 0
    filtSeq.WZtoLeptonFilter.Etacut_electron = 2.6
    filtSeq.WZtoLeptonFilter.Etacut_muon = 2.6
    filtSeq.Expression = "WZtoLeptonFilter"
    
    
    evt_multiplier = 10
    evgenLog.info('changing event multiplier to 10')
        
    

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
   # New feature in Pythia 8.230+
   genSeq.Pythia8.Commands += ["Merging:Process = guess"]
   if "UserHooks" in genSeq.Pythia8.__slots__.keys():
       genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
   else:
       genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
   #
   genSeq.Pythia8.Commands += [ "1000024:spinType = 1",
                                "1000023:spinType = 1" ]
