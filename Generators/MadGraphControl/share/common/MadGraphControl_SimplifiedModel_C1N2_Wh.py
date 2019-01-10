include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')

#C1/N2 degenerate
masses['1000024'] = MassToFloat(splitConfig[5])
masses['1000023'] = MassToFloat(splitConfig[5])
masses['1000022'] = MassToFloat(splitConfig[6])
if masses['1000022']<0.5: masses['1000022']=0.5

extras['event_norm']='sum'
extras['use_syst']='F'

gentype = splitConfig[2]
decaytype = 'Whall'

if splitConfig[4] == 'hall':
  descriptionid= 'h -> all'
elif splitConfig[4] == 'hbb':
  descriptionid= 'h -> bb' 
elif splitConfig[4] == 'hgg':
  descriptionid= 'h -> gamma gamma'

process = '''
generate p p > x1+ n2 $ susystrong \@1
add process p p > x1- n2 $ susystrong \@2
add process p p > x1+ n2 j $ susystrong \@3
add process p p > x1- n2 j $ susystrong \@4
add process p p > x1+ n2 j j $ susystrong \@5
add process p p > x1- n2 j j $ susystrong \@6
'''
njets = 2

evgenLog.info('Registered generation of ~chi1+/- ~chi20 production, decay via Wh '+descriptionid+'; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact  = [ "olsson@cern.ch, acervell@cern.ch, keisuke.yoshihara@cern.ch" ]
evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino']
evgenConfig.description = '~chi1+/- ~chi20 production, decay via Wh, %s, in simplified model, m_C1N2 = %s GeV, m_N1 = %s GeV'%(descriptionid,masses['1000024'],masses['1000022'])

genSeq.Pythia8.Commands += [ "24:mMin = 0.2", "23:mMin = 0.2" ]

if 'lep' in runArgs.jobConfig[0].split("_")[-1]:
     if splitConfig[4] == 'hbb': # for 1Lbb analysis
       evgenLog.info('force higgs decay to bbbar')
       decays ={'25':'''DECAY 25 4.088E-03 #h0 decays
#       BR              NDA     ID1       ID2
     1.00000000E+00    2           5        -5   # BR(H1 -> b bbar)
#
       '''}
     elif splitConfig[4] == 'hgg': # for 1L gamma gamma analysis
       evgenLog.info('force higgs decay to gamma gamma')
       decays ={'25':'''DECAY 25 4.088E-03 #h0 decays
#       BR              NDA     ID1       ID2
     1.00000000E+00    2          22        22   # BR(H1 -> gamma gamma)
#
       '''}
     evgenLog.info('only W->lep will be generated')
     genSeq.Pythia8.Commands += [
        "24:onMode = off", #switch off all W decays
        "24:onIfAny = 11 13 15", # switch on W->lnu
     ]

elif 'had' in runArgs.jobConfig[0].split("_")[-1]: # for full had analysis 
     if splitConfig[4] == 'hgg': # for 1L gamma gamma analysis
       evgenLog.info('force higgs decay to gamma gamma')
       decays ={'25':'''DECAY 25 4.088E-03 #h0 decays
#       BR              NDA     ID1       ID2
     1.00000000E+00    2          22        22   # BR(H1 -> gamma gamma)
#
       '''}
     elif splitConfig[4] == 'hbb': # for full had analysis
       evgenLog.info('force higgs decay to bbbar')
       decays ={'25':'''DECAY 25 4.088E-03 #h0 decays
#       BR              NDA     ID1       ID2
     1.00000000E+00    2           5        -5   # BR(H1 -> b bbar)
#
       '''}
     evgenLog.info('only W->hadron will be generated')
     genSeq.Pythia8.Commands += [
        "24:onMode = off", #switch off all W decays
        "24:onIfAny = 1 2 3 4 5" # switch on W->qqbar
     ]

elif 'all' in runArgs.jobConfig[0].split("_")[-1]: # for inclusive analysis 
     if splitConfig[4] == 'hgg': # for gamma gamma analysis
       evgenLog.info('force higgs decay to gamma gamma')
       decays ={'25':'''DECAY 25 4.088E-03 #h0 decays
#       BR              NDA     ID1       ID2
     1.00000000E+00    2          22        22   # BR(H1 -> gamma gamma)
#
       '''}
     elif splitConfig[4] == 'hbb': # for bb analysis
       evgenLog.info('force higgs decay to bbbar')
       decays ={'25':'''DECAY 25 4.088E-03 #h0 decays
#       BR              NDA     ID1       ID2
     1.00000000E+00    2           5        -5   # BR(H1 -> b bbar)
#
       '''}
     evgenLog.info('only W->inclusive will be generated')
     genSeq.Pythia8.Commands += [
        "24:onMode = on" #switch on all W decays
     ]


#--------------------------------------------------------------
# filters
#--------------------------------------------------------------

if '2L7' in runArgs.jobConfig[0].split("_")[-1]:
   evgenLog.info('2L7 filter is applied')
   include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
   filtSeq.MultiElecMuTauFilter.NLeptons  = 2
   filtSeq.MultiElecMuTauFilter.MinPt = 7000.         # high pt-cut on the lepton
   filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
   filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus
   filtSeq.Expression = "MultiElecMuTauFilter"
   evt_multiplier = 15.

if 'tau' in runArgs.jobConfig[0].split("_")[-1]:
   evgenLog.info('1 tau filter is applied')
   include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
   filtSeq.MultiElecMuTauFilter.NLeptons  = 1
   filtSeq.MultiElecMuTauFilter.MinPt = 1e10            # extreme high pt-cut on the lepton, only want taus
   filtSeq.MultiElecMuTauFilter.MaxEta = 2.8            # stay away from MS 2.7 just in case
   filtSeq.MultiElecMuTauFilter.MinVisPtHadTau = 15000. # pt-cut on the visible hadronic tau 
   filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 1      # include hadronic taus
   filtSeq.Expression = "MultiElecMuTauFilter"
   evt_multiplier = 50.

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )


if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"] # New feature in Pythia 8.230+
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
      genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
      genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
   
    #genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}{n2,1000023}",
    #                             "1000024:spinType = 1",
    #                            "1000023:spinType = 1" ]
				 
