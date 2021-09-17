evgenConfig.description = "SuperChic4 MC gamma + gamma pp collisions to gamma + gamma mediated by ALP no pythia shower"
evgenConfig.keywords = ["2photon","2photon"]
#evgenConfig.weighting = 0
evgenConfig.contact = ["william.patrick.mccormack.iii@cern.ch"]
evgenConfig.generators += ["Superchic"]
    
from Superchic_i.SuperChicUtils import SuperChicConfig, SuperChicRun

#class with the superchic initialization parameters.  Please see SuperChicUtils for a complete list of tunable parameters.
scConfig = SuperChicConfig(runArgs)

scConfig.isurv = 4                  # Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
scConfig.PDFname = 'MMHT2014lo68cl' # PDF set name
scConfig.PDFmember = 0              # PDF member
scConfig.proc = 68                  # Process number (59 = gg->gg, 56 = gg->ee, 68 = gg->a->gg ); Please consult Superchic Manual https://superchic.hepforge.org/
scConfig.beam = 'prot'              # Beam type ('prot', 'ion')
scConfig.sfaci = True             # Include soft survival effects
scConfig.diff = 'el'                # interaction: elastic ('el'), single ('sd','sda','sdb') and double ('dd') dissociation.
scConfig.genunw  = True
scConfig.ymin  = -2.4              # Minimum object rapidity Y_X
scConfig.ymax  = 2.4               # Maximum object rapidity Y_X
scConfig.mmin  = 6                 # Minimum object mass M_X
scConfig.mmax  = 500               # Maximum object mass M_X
scConfig.gencuts  = True           # Generate cuts below
scConfig.ptxmax  = 100         #cut on proton pt
scConfig.ptamin  = 3.0            # Minimum pT of outgoing object a 
scConfig.ptbmin  = 3.0             # Minimum pT of outgoing object b 
scConfig.etaamin  = -2.4           # Minimum eta of outgoing object a
scConfig.etaamax   = 2.4           # Maximum eta of outgoing object a
scConfig.etabmin  = -2.4           # Minimum eta of outgoing object b
scConfig.etabmax   = 2.4           # Maximum eta of outgoing object b
scConfig.acoabmax  = 100


SuperChicRun(scConfig, genSeq)

import Superchic_i.EventFiller as EF
ef = EF.LheEVNTFiller()
ef.fileName = scConfig.outputLHEFile()
outputFileName = "outputs/output"+scConfig.outtg+".dat"
ef.outputFileName = outputFileName
genSeq += ef

