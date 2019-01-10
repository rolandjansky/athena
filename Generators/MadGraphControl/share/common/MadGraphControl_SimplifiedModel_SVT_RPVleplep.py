include('MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py')

import math

joboptparts = runArgs.jobConfig[0].strip('\.py').split('_')
channel = joboptparts[3]

gentype = joboptparts[2]
decaytype = 'leplep'
evgenLog.info('RPV generation of a stau neutrino and RPV decay into leptons in the channel %s' % channel)

masses['1000016'] = float(joboptparts[-1].lstrip('m'))

LambdaLQD_111 = 0.11 
LambdaLLE_112 = 0.07

MpWidth = (3 * LambdaLQD_111**2 + 2 * LambdaLLE_112**2) * masses['1000016'] / (16 * math.pi)

#-----------------------#
# Set the correct xqcut #
#-----------------------#
xqcut = 500
if masses['1000016'] < xqcut * 4 :
    xqcut = masses['1000016'] * 0.25
#-------------------------------------#

mgproc = None

if 'mutau' in channel:
    decays = {'1000016' : """DECAY 1000016 """ + str(MpWidth) + """ # Width sneutrino Tau
##           BR         NDA      ID1       ID2
0.500000     2     13     -15    #BR(svt -> mu tau)
0.500000     2     -13     15    #BR(svt -> mu tau)
""" }
    mgproc="""
    generate p p > svt {decay1} @1
    add process p p > svt~ {decay2} @2
    add process p p > svt j {decay1} @3
    add process p p > svt~ j {decay2} @4
    add process p p > svt j j {decay1} @5
    add process p p > svt~ j j {decay2} @6
    """.format(decay1=", (svt  > mu ta)",decay2=", (svt~ > mu ta)")
    
    
elif 'etau' in channel:
    decays = {'1000016' : """DECAY 1000016 """ + str(MpWidth) + """ # Width sneutrino Tau
##           BR         NDA      ID1       ID2
0.500000     2     11     -15    #BR(svt -> e tau)
0.500000     2     -11     15    #BR(svt -> e tau)
""" }
    mgproc="""
    generate p p > svt {decay1} @1
    add process p p > svt~ {decay2} @2
    add process p p > svt j {decay1} @3
    add process p p > svt~ j {decay2} @4
    add process p p > svt j j {decay1} @5
    add process p p > svt~ j j {decay2} @6
    """.format(decay1=", (svt  > e ta)",decay2=", (svt~ > e ta)")

    
elif 'emu' in channel:
    decays = {'1000016' : """DECAY 1000016 """ + str(MpWidth) + """ # Width sneutrino Tau 
##           BR         NDA      ID1       ID2
0.500000     2     11     -13    #BR(svt -> e mu)    
0.500000     2     -11     13    #BR(svt -> e mu)
""" }
 
    mgproc="""
    generate p p > svt {decay1} @1
    add process p p > svt~ {decay2} @2
    add process p p > svt j {decay1} @3
    add process p p > svt~ j {decay2} @4
    add process p p > svt j j {decay1} @5
    add process p p > svt~ j j {decay2} @6
    """.format(decay1=", (svt  > e mu)",decay2=", (svt~ > e mu)")


process = """
import model RPVMSSM_UFO
define e = e- e+
define mu = mu- mu+
define ta = ta- ta+
"""+mgproc

njets = 2

m_description = None
if not 'tau' in channel:
    m_description = 'MadGraph+Pythia8+A14NNPDF23LO production dd~>svt>%s in RPV model, m_svt = %s GeV'%(channel,masses['1000016'])
else:
    m_description = 'MadGraph+Pythia8+A14NNPDF23LO production dd~>svt>%s (tauhad filter) in RPV model, m_svt = %s GeV'%(channel,masses['1000016'])

evgenConfig.description = m_description
evgenConfig.keywords = ['SUSY', 'RPV','sneutrino','resonance' ]
evgenConfig.contact = ["Quanyin Li <quli@cern.ch>", "Antonio Salvucci <antonio.salvucci@cern.ch>"]
evgenConfig.generators = ['MadGraph', 'Pythia8', 'EvtGen']

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

#---------------#
# TauHad filter #
#---------------#
if 'tau' in channel:
    evgenLog.info('Apply Filter: 1 hadronic tau')
    from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
    filtSeq += MultiElecMuTauFilter("TauHadFilter")
    MultiElecMuTauFilter = filtSeq.TauHadFilter
    MultiElecMuTauFilter.NLeptons  = 1
    MultiElecMuTauFilter.MinPt = 1e10            # no light leptons will pass this pT threshold
    MultiElecMuTauFilter.MaxEta = 10.0
    MultiElecMuTauFilter.MinVisPtHadTau = 10000. # pt-cut on the visible hadronic tau
    MultiElecMuTauFilter.IncludeHadTaus = 1      # include hadronic taus
    #filtSeq.Expression = "(TauHadFilter)"
    evt_multiplier = 4

if njets > 0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{svt,1000016}{svt~,-1000016}"]
