## Tauola config
from Tauolapp_i.Tauolapp_iConf import TauolaPP
genSeq += TauolaPP()

TauolaPP.decay_mode_same = 0;
TauolaPP.decay_mode_opposite = 0;
TauolaPP.decay_particle = 15;
TauolaPP.tau_mass = 1.77684;
TauolaPP.spin_correlation=True;
TauolaPP.setRadiation=True;
TauolaPP.setRadiationCutOff=0.01; 

evgenConfig.generators += [ "TauolaPP" ]
