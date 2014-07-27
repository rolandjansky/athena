## Herwig++ job option file for Susy SU3 2-parton -> 2-sparticle processes

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

## Get basic Herwig++ Atlas tune params
from Herwigpp_i import config as hw
cmds = hw.energy_cmds(7000) + hw.base_cmds() + hw.lo_pdf_cmds("cteq6ll.LHpdf") + hw.ue_tune_cmds("CTEQ6L1-UE-EE-7000-3")

## Add Herwig++ parameters for this process
cmds += """

## Generate the process in MSSM equivalent to 2-parton -> 2-sparticle processes in Fortran Herwig
## Read the MSSM model details
read MSSM.Hwpp250.model
cd /Herwig/NewPhysics

#incoming parton
insert HPConstructor:Incoming 0 /Herwig/Particles/g
insert HPConstructor:Incoming 1 /Herwig/Particles/u
insert HPConstructor:Incoming 2 /Herwig/Particles/ubar
insert HPConstructor:Incoming 3 /Herwig/Particles/d
insert HPConstructor:Incoming 4 /Herwig/Particles/dbar
insert HPConstructor:Incoming 5 /Herwig/Particles/s
insert HPConstructor:Incoming 6 /Herwig/Particles/sbar
insert HPConstructor:Incoming 7 /Herwig/Particles/c
insert HPConstructor:Incoming 8 /Herwig/Particles/cbar
insert HPConstructor:Incoming 9 /Herwig/Particles/b
insert HPConstructor:Incoming 10 /Herwig/Particles/bbar

#outgoing sparton (squark and gluino)
insert HPConstructor:Outgoing 0 /Herwig/Particles/~g
insert HPConstructor:Outgoing 1 /Herwig/Particles/~u_L
insert HPConstructor:Outgoing 2 /Herwig/Particles/~u_Lbar
insert HPConstructor:Outgoing 3 /Herwig/Particles/~d_L
insert HPConstructor:Outgoing 4 /Herwig/Particles/~d_Lbar
insert HPConstructor:Outgoing 5 /Herwig/Particles/~s_L
insert HPConstructor:Outgoing 6 /Herwig/Particles/~s_Lbar
insert HPConstructor:Outgoing 7 /Herwig/Particles/~c_L
insert HPConstructor:Outgoing 8 /Herwig/Particles/~c_Lbar
insert HPConstructor:Outgoing 9 /Herwig/Particles/~b_1
insert HPConstructor:Outgoing 10 /Herwig/Particles/~b_1bar
insert HPConstructor:Outgoing 11 /Herwig/Particles/~t_1
insert HPConstructor:Outgoing 12 /Herwig/Particles/~t_1bar

insert HPConstructor:Outgoing 13 /Herwig/Particles/~u_R
insert HPConstructor:Outgoing 14 /Herwig/Particles/~u_Rbar
insert HPConstructor:Outgoing 15 /Herwig/Particles/~d_R
insert HPConstructor:Outgoing 16 /Herwig/Particles/~d_Rbar
insert HPConstructor:Outgoing 17 /Herwig/Particles/~s_R
insert HPConstructor:Outgoing 18 /Herwig/Particles/~s_Rbar
insert HPConstructor:Outgoing 19 /Herwig/Particles/~c_R
insert HPConstructor:Outgoing 20 /Herwig/Particles/~c_Rbar
insert HPConstructor:Outgoing 21 /Herwig/Particles/~b_2
insert HPConstructor:Outgoing 22 /Herwig/Particles/~b_2bar
insert HPConstructor:Outgoing 23 /Herwig/Particles/~t_2
insert HPConstructor:Outgoing 24 /Herwig/Particles/~t_2bar

#outgoing gaugino (chargino and neutralino)
insert HPConstructor:Outgoing 25 /Herwig/Particles/~chi_10
insert HPConstructor:Outgoing 26 /Herwig/Particles/~chi_20
insert HPConstructor:Outgoing 27 /Herwig/Particles/~chi_30
insert HPConstructor:Outgoing 28 /Herwig/Particles/~chi_40
insert HPConstructor:Outgoing 29 /Herwig/Particles/~chi_1+
insert HPConstructor:Outgoing 30 /Herwig/Particles/~chi_2+
insert HPConstructor:Outgoing 31 /Herwig/Particles/~chi_1-
insert HPConstructor:Outgoing 32 /Herwig/Particles/~chi_2-

#outgoing slepton
insert HPConstructor:Outgoing 33 /Herwig/Particles/~e_L-
insert HPConstructor:Outgoing 34 /Herwig/Particles/~nu_eL
insert HPConstructor:Outgoing 35 /Herwig/Particles/~mu_L-
insert HPConstructor:Outgoing 36 /Herwig/Particles/~nu_muL
insert HPConstructor:Outgoing 37 /Herwig/Particles/~tau_1-
insert HPConstructor:Outgoing 38 /Herwig/Particles/~nu_tauL

insert HPConstructor:Outgoing 39 /Herwig/Particles/~e_L+
insert HPConstructor:Outgoing 40 /Herwig/Particles/~nu_eLbar
insert HPConstructor:Outgoing 41 /Herwig/Particles/~mu_L+
insert HPConstructor:Outgoing 42 /Herwig/Particles/~nu_muLbar
insert HPConstructor:Outgoing 43 /Herwig/Particles/~tau_1+
insert HPConstructor:Outgoing 44 /Herwig/Particles/~nu_tauLbar

insert HPConstructor:Outgoing 45 /Herwig/Particles/~e_R-
insert HPConstructor:Outgoing 46 /Herwig/Particles/~nu_eR
insert HPConstructor:Outgoing 47 /Herwig/Particles/~mu_R-
insert HPConstructor:Outgoing 48 /Herwig/Particles/~nu_muR
insert HPConstructor:Outgoing 49 /Herwig/Particles/~tau_2-
insert HPConstructor:Outgoing 50 /Herwig/Particles/~nu_tauR

insert HPConstructor:Outgoing 51 /Herwig/Particles/~e_R+
insert HPConstructor:Outgoing 52 /Herwig/Particles/~nu_eRbar
insert HPConstructor:Outgoing 53 /Herwig/Particles/~mu_R+
insert HPConstructor:Outgoing 54 /Herwig/Particles/~nu_muRbar
insert HPConstructor:Outgoing 55 /Herwig/Particles/~tau_2+
insert HPConstructor:Outgoing 56 /Herwig/Particles/~nu_tauRbar

## Read the SUSY spectrum file (SLHA format)
setup MSSM/Model susy_SU4_isajet_slha.txt
"""

## Set the command vector
topAlg.Herwigpp.Commands = cmds.splitlines()
