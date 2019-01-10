include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')

masses['1000022'] = float(splitConfig[5])  #-- chi_10    LSP 
masses['1000023'] = float(splitConfig[4])  #-- chi_20    NLSP

if masses['1000022']<0.5: 
  masses['1000022']=0.5

#will be ZH
gentype = splitConfig[2]
#will be N2N1
decaytype = splitConfig[3]
xqcut = 25 

process = '''
generate p p > h1 z, (h1 > n2 n1) , (z > l+ l-) @1 
'''
njets = 0
evgenLog.info('Registered generation of h1 z production, decay via  h > n2 n1, z > l+ l-; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000023']) + ' ' + str(masses['1000022']) )

evgenConfig.contact  = [ "khilesh.mistry@cern.ch" ]
evgenConfig.keywords += ['Z', 'neutralino', 'lepton']
evgenConfig.description = 'h1 z production, decay via n2, n1 in simplified model, m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000023'],masses['1000022'])

#--------------------------------------------------------------
# add some filter here
#--------------------------------------------------------------

# need more events from MG due to filter - this needs to be set before MadGraphControl_SimplifiedModelPostInclude.py is run (it's set at 2 there)
if '2L5' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('2leptons5 filter')
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.NLeptons  = 2
  filtSeq.MultiElecMuTauFilter.MinPt = 5000.         # low pt-cut on the lepton
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus
  filtSeq.Expression = "MultiElecMuTauFilter"
  evt_multiplier = 1.5

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
