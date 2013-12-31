#==================================
# Setup Physics D3PD
#==================================

from QcdD3PDMaker.QcdD3PDMakerFlags import *
QcdD3PDMakerFlagsSetPhysics()

#==================================
# Common Settings
#==================================

from RecExConfig.RecFlags import rec
rec.UserAlgs+=['QcdD3PDMaker/QcdD3PD_preSetup.py']
include('QcdD3PDMaker/QcdD3PDMakerPreSetup.common.py')
