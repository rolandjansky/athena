include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

splits = runArgs.jobConfig[0].split('_')

masses['1000024'] = MassToFloat(splits[4])
masses['1000023'] = MassToFloat(splits[4])
masses['1000022'] = MassToFloat(splits[5].split('.')[0])

splitting = 0.5
if len(splits) > 6:
  splitting = MassToFloat(splits[6].split('.')[0])
  
if(splitting < 1):
  evgenLog.info("Using mass splitting of %f for mslep" % splitting)  
  mslep = ( masses['1000024'] - masses['1000022'] )*splitting + masses['1000022']
else:
  mslep = ( masses['1000024'] - splitting )
  
if len(splits) > 7:
  msquark = MassToFloat(splits[7].split('.')[0])
  masses['1000001'] = msquark
  masses['1000002'] = msquark
  masses['1000003'] = msquark
  masses['1000004'] = msquark
  masses['1000005'] = msquark
  masses['1000006'] = msquark
  masses['1000021'] = msquark
  evgenLog.info("  Squark / gluino mass is %f GeV" % msquark)

evgenLog.info("  Stau / sneutrino mass is %f GeV" % mslep)

masses['1000015'] = mslep
masses['1000016'] = mslep
# NOTE: stau2 is not used (decoupled as in Run-1)
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = 'C1C1'
gentype_JO = str(splits[2])
decaytype = str(splits[3])

if gentype_JO != gentype+'VBF':
  raise RuntimeError("This control file is only for "+gentype+"VBF production")

mjjmin = 350
detajj = 3.0
ptjet1min = 50


process = '''
add process p p > x1+ x1+ j j QED=99 QCD=99
add process p p > x1- x1- j j QED=99 QCD=99
add process p p > x1+ x1- j j QED=99 QCD=99
'''
njets = 2

evgenLog.info('Registered generation of ~chi1+/~chi1- production via VBF, decay via stau; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact  = [ "kparker@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'VBF', 'gaugino', 'chargino', 'stau']
evgenConfig.description = '~chi1+/~chi1- production via VBF, decay via stau in simplified model, m_C1 = %s GeV, m_stau = %s GeV, m_N1 = %s GeV' % (masses['1000024'],masses['1000015'],masses['1000022'])

# need more events from MG due to filter - this needs to be set before MadGraphControl_SimplifiedModelPostInclude.py is run
# evt_multiplier = 3 is only sufficient for large mass splittings
evt_multiplier = 20

extras = {'ptj'         : 20,
          'etaj'        : 5.0,
          'mmjj'        : mjjmin,
          'deltaeta'    : detajj,
          'ptj1min'     : ptjet1min
          }


include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}",
                                 "1000024:spinType = 1" ]


#--------------------------------------------------------------
# Lepton filter
#--------------------------------------------------------------
from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
filtSeq += MultiElecMuTauFilter("DileptonFilter")
filtSeq += MultiElecMuTauFilter("TauFilter")

MultiElecMuTauFilter1 = filtSeq.DileptonFilter
MultiElecMuTauFilter1.NLeptons  = 2
MultiElecMuTauFilter1.MinPt = 5000.
MultiElecMuTauFilter1.MaxEta = 2.7
MultiElecMuTauFilter1.MinVisPtHadTau = 15000. # pt-cut on the visible hadronic tau
MultiElecMuTauFilter1.IncludeHadTaus = 1      # include hadronic taus

MultiElecMuTauFilter2 = filtSeq.TauFilter
MultiElecMuTauFilter2.NLeptons  = 1
MultiElecMuTauFilter2.MinPt = 1e10
MultiElecMuTauFilter2.MaxEta = 2.7
MultiElecMuTauFilter2.MinVisPtHadTau = 15000. # pt-cut on the visible hadronic tau
MultiElecMuTauFilter2.IncludeHadTaus = 1      # include hadronic taus
filtSeq.Expression = "(DileptonFilter and TauFilter)"
