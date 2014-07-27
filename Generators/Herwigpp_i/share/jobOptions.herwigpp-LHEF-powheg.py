## Job options file for Herwig++, reading an NLO Les Houches Accord event file

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(7000) + hw.base_cmds() + \
    hw.nlo_pdf_cmds("CT10.LHgrid", "cteq6ll.LHpdf") + hw.ue_tune_cmds("CTEQ6L1-UE-EE-7000-3") + \
    hw.lhef_cmds(nlo=True)

## Add to commands
cmds += """
## Some particle decay modes
# cd /Herwig/Particles
# set b:Stable Stable
# set bbar:Stable Stable
# set tau+:Stable Stable
# set tau-:Stable Stable
# set t:Synchronized Not_synchronized
# set tbar:Synchronized Not_synchronized

# setup t     6 t    174.2 1.5759 14 0  2  3 2 0
# setup tbar -6 tbar 174.2 1.5759 14 0 -2 -3 2 0

# set t/t->b,u,dbar;:OnOff                   On
# set t/t->b,c,sbar;:OnOff                   On
# set t/t->nu_mu,mu+,b;:OnOff                Off
# set t/t->nu_e,e+,b;:OnOff                  Off
# set t/t->nu_tau,tau+,b;:OnOff              Off
# set t/t->b,sbar,u;:OnOff                   On
# set t/t->b,c,dbar;:OnOff                   On
# set t/t->b,bbar,c;:OnOff                   On

# set tbar/tbar->bbar,ubar,d;:OnOff          Off
# set tbar/tbar->bbar,cbar,s;:OnOff          Off
# set tbar/tbar->nu_ebar,e-,bbar;:OnOff      On
# set tbar/tbar->nu_mubar,mu-,bbar;:OnOff    On
# set tbar/tbar->nu_taubar,tau-,bbar;:OnOff  On
# set tbar/tbar->bbar,s,ubar;:OnOff          Off
# set tbar/tbar->bbar,cbar,d;:OnOff          Off
# set tbar/tbar->b,bbar,cbar;:OnOff          Off
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()
