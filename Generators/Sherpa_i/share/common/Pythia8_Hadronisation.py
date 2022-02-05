## Setting path to Pythia8 data
import os
if os.environ['AtlasVersion']=="21.6.82":
    os.environ['PYTHIA8DATA']='/cvmfs/atlas.cern.ch/repo/sw/software/21.6/sw/lcg/releases/LCG_88b/MCGenerators/pythia8/245p3.lhcb7/x86_64-centos7-gcc62-opt/share/Pythia8/xmldoc/'
else:
    os.environ['PYTHIA8DATA']=os.environ['PY8PATH']+'/share/Pythia8/xmldoc/'
## Adjusting ANY other Pythia8 settings via the PYTHIA8 block overrides these settings here.
## So you have to include them again in your own settings
## even if you do not wish to explicitely change their values.
genSeq.Sherpa_i.Parameters += """
SHERPA_LDADD: SherpaPythia
FRAGMENTATION: Pythia8
PYTHIA8:
  DECAYS: true
  PARAMETERS:
    - Main:timesAllowErrors: 500
    - ParticleDecays:limitTau0: on
    - ParticleDecays:tau0Max: 10.0
"""
