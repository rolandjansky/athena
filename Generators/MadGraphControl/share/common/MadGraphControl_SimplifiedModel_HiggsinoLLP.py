include ('MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py')

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
gentype = str(splitConfig[2])
decaytype = str(splitConfig[3])
masses['1000022'] = float(splitConfig[4])
if masses['1000022']<0.5: masses['1000022']=0.5

# ----------------------------------------------
#   delta m (C1,N1) [GeV] and lifetime [ns]
# ----------------------------------------------

mN = int (masses['1000022'])
deltaM = -1.000 
lifetime = -1.000

# ---------------------------------------------
# pure higgsino mass, deltaM, lifetime 
# for more information: arxiv/hep-ph/9804359 and arXiv:1703.09675
#  95 GeV: (254,0.067), 100 GeV: (257,0.064), 120 GeV: (269,0.055), 140 GeV: (278, 0.049), 
# 150 GeV: (281,0.048), 160 GeV: (284,0.046), 200 GeV: (296,0.040), 240 GeV: (304, 0.037),
# 1000 GeV: (340, 0.026)
# ---------------------------------------------
  
if mN == 95:     
  deltaM = 0.254
  lifetime = 0.067
elif mN == 100:  
  deltaM = 0.257
  lifetime = 0.064
elif mN == 120:  
  deltaM = 0.269
  lifetime = 0.055
elif mN == 140:  
  deltaM = 0.278
  lifetime = 0.049
elif mN == 150:  
  deltaM = 0.281
  lifetime = 0.048
elif mN == 160:  
  deltaM = 0.284
  lifetime = 0.046
elif mN == 200:  
  deltaM = 0.296
  lifetime = 0.040
elif mN == 240:  
  deltaM = 0.304
  lifetime = 0.037
elif mN == 1000: 
  deltaM = 0.340
  lifetime = 0.026
else:
  raise RunTimeError("ERROR: deltaM and lifetime are not correctly set")

masses['1000024'] = masses['1000022'] + deltaM
masses['1000023'] = masses['1000022'] * (-1)

if str(splitConfig[5]) == "0p1":
  lifetime = 0.1
elif str(splitConfig[5]) == "0p2":
  lifetime = 0.2
elif str(splitConfig[5]) == "0p3":
  lifetime = 0.3

# ----------------------------------------------
#   process lines
# ----------------------------------------------

if gentype == "C1pN1":
  process = '''
generate p p > x1+ n1 $ susystrong @1
add process p p > x1+ n1 j $ susystrong @2
add process p p > x1+ n1 j j $ susystrong @3
'''
  mergeproc = "{x1+,1000024}{n1,1000022}"
elif gentype == "C1mN1":
  process = '''
generate p p > x1- n1 $ susystrong @1
add process p p > x1- n1 j $ susystrong @2
add process p p > x1- n1 j j $ susystrong @3
'''
  mergeproc = "{x1-,-1000024}{n1,1000022}"
elif gentype == "C1C1":
  process = '''
generate p p > x1+ x1- $ susystrong @1
add process p p > x1+ x1- j $ susystrong @2
add process p p > x1+ x1- j j $ susystrong @3
'''
  mergeproc = "{x1+,1000024}{x1-,-1000024}"
njets = 2

# ----------------------------------------------
#   generator filter 
# ----------------------------------------------

evt_multiplier = 4

if 'MET50' in "%s" % (splitConfig[-1]):
    evgenLog.info('MET 50 filter is applied')
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 50*GeV
    filtSeq.MissingEtFilter.UseChargedNonShowering = True
    filtSeq.Expression = "MissingEtFilter"
    evt_multiplier = 10

# ----------------------------------------------
#   evgen configs 
# ----------------------------------------------

EleBR = 0.030
MuBR  = 0.015

evgenLog.info('Registered generation of ~chi1+/- ~chi20 production (long-lived higgsinos)'+str(runArgs.runNumber))

evgenConfig.contact  = ["keisuke.yoshihara@cern.ch"]
evgenConfig.keywords += ['simplifiedModel','gaugino', 'neutralino', 'chargino', 'longLived']
evgenConfig.description = '~chi1+/- ~chi20 production (long-lived higgsinos), m_C1 = %s GeV, m_N1 = %s GeV, lifetime = %s ns' %(masses['1000024'],masses['1000022'], lifetime)

if lifetime != 0:
   evgenConfig.specialConfig = 'AMSBC1Mass=%s*GeV;AMSBN1Mass=%s*GeV;AMSBC1ToEleBR=%s;AMSBC1ToMuBR=%s;AMSBC1Lifetime=%s*ns;preInclude=SimulationJobOptions/preInclude.AMSB.py' % (masses['1000024'],masses['1000022'],EleBR,MuBR,lifetime)

gentype = "C1N2" # dummy gentype to set xqcut correctly

# AGENE-1542: disable syscalc and set event_norm explicitly
extras['event_norm']='sum'
extras['use_syst']='F'

include ('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

# ----------------------------------------------
#   pythia8 commands etc 
# ----------------------------------------------

genSeq.Pythia8.Commands += ["1000022:all = Chi Chi~ 1 0 0 %s" % (masses['1000022'])]

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>%s" % mergeproc,
                                 "1000022:spinType = 1",
                                 "1000024:spinType = 1" ]

bonus_file = open('pdg_extras.dat','w')
bonus_file.write( '1000024 Chargino 100.0 (MeV/c) boson Chargino 1\n')
bonus_file.write( '-1000024 Anti-chargino 100.0 (MeV/c) boson Chargino -1\n')
bonus_file.close()
 
testSeq.TestHepMC.G4ExtraWhiteFile='pdg_extras.dat'

import os
os.system("get_files %s" % testSeq.TestHepMC.G4ExtraWhiteFile)

