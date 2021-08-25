evgenConfig.description = "SuperChic4 MC gamma + gamma pp collisions at 13000 GeV to 2 muons with dissociation on both side"
evgenConfig.keywords = ["2photon","2muon","dissociation"]
evgenConfig.contact = ["william.patrick.mccormack.iii@cern.ch", "oldrich.kepka@cern.ch"]

from Superchic_i.SuperChicUtils import SuperChicConfig, SuperChicRun

#class with the superchic initialization parameters.  Please see SuperChicUtils for a complete list of tunable parameters.
scConfig = SuperChicConfig(runArgs)

scConfig.isurv = 4                  # Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
scConfig.PDFname = 'MMHT2014lo68cl' # PDF set name
scConfig.PDFmember = 0              # PDF member
scConfig.proc = 57                  # Process number (59 = gg->gg, 56 = gg->ee, 68 = gg->a->gg ); Please consult Superchic Manual https://superchic.hepforge.org/
scConfig.beam = 'prot'              # Beam type ('prot', 'ion')
scConfig.sfaci = True             # Include soft survival effects
scConfig.diff = 'el'                # interaction: elastic ('el'), single ('sd','sda','sdb') and double ('dd') dissociation.
scConfig.genunw  = True
scConfig.ymin  = -5.0              # Minimum object rapidity Y_X
scConfig.ymax  = 5.0               # Maximum object rapidity Y_X
scConfig.mmin  = 20                 # Minimum object mass M_X
scConfig.mmax  = 2000               # Maximum object mass M_X
scConfig.gencuts  = True           # Generate cuts below
scConfig.ptxmax  = 100         #cut on proton pt
scConfig.ptamin  = 10.0             # Minimum pT of outgoing object a 
scConfig.ptbmin  = 10.             # Minimum pT of outgoing object b 
scConfig.etaamin  = -2.5           # Minimum eta of outgoing object a
scConfig.etaamax   = 2.5           # Maximum eta of outgoing object a
scConfig.etabmin  = -2.5           # Minimum eta of outgoing object b
scConfig.etabmax   = 2.5           # Maximum eta of outgoing object b
scConfig.acoabmax  = 100


SuperChicRun(scConfig, genSeq)
include('Superchic_i/Pythia8_EL_Common.py')
