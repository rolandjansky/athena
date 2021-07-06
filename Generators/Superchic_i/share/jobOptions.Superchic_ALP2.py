evgenConfig.description = "SuperChic4 MC gamma + gamma pp collisions to gamma + gamma mediated by ALP no pythia shower"
evgenConfig.keywords = ["2photon","2photon"]
#evgenConfig.weighting = 0
evgenConfig.contact = ["william.patrick.mccormack.iii@cern.ch"]

if not os.path.exists('inputs'):
    os.makedirs('inputs')
if not os.path.exists('evrecs'):
    os.makedirs('evrecs')
if not os.path.exists('outputs'):
    os.makedirs('outputs')

evgenConfig.generators += ["Superchic"]
    
from Superchic_i.SuperChicUtils import *

#class with the superchic initialization parameters.  Please see SuperChicUtils for a complete list of tunable parameters.
Init = SuperChicConfig()

hasECM = hasattr(runArgs,"ecmEnergy")
if hasECM:
    ecm = int(runArgs.ecmEnergy)
    Init.rts = str(ecm)+"d0" # set the COM collision energy (in fortran syntax)
Init.isurv = "4"                  # Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
Init.intag = "'in5'"              # for input files
Init.PDFname = "'MMHT2014lo68cl'" # PDF set name
Init.PDFmember = "0"              # PDF member
Init.proc = "68"                  # Process number (59 = gg->gg, 56 = gg->ee, 68 = gg->a->gg ); Please consult Superchic Manual https://superchic.hepforge.org/
Init.beam = "'prot'"              # Beam type ('prot', 'ion')
Init.outtg = "'out'"              # for output file name
Init.sfaci = ".true."             # Include soft survival effects
Init.diff = "'el'"                # interaction: elastic ('el'), single ('sd','sda','sdb') and double ('dd') dissociation.
hasSeed = hasattr(runArgs,"randomSeed")
if hasSeed:
    Init.iseed  = str(int(runArgs.randomSeed))
Init.genunw  = ".true."
hasNev = hasattr(runArgs,"maxEvents")
if hasNev:
    Init.nev = str(runArgs.maxEvents)
Init.erec  = "'lhe'"
Init.ymin  = "-2.4d0"              # Minimum object rapidity Y_X
Init.ymax  = "2.4d0"               # Maximum object rapidity Y_X
Init.mmin  = "6d0"                 # Minimum object mass M_X
Init.mmax  = "500d0"               # Maximum object mass M_X
Init.gencuts  = ".true."           # Generate cuts below
Init.ptxmax  = "100d0"         #cut on proton pt
Init.ptamin  = "3.0d0"             # Minimum pT of outgoing object a 
Init.ptbmin  = "3.0d0"             # Minimum pT of outgoing object b 
Init.etaamin  = "-2.4d0"           # Minimum eta of outgoing object a
Init.etaamax   = "2.4d0"           # Maximum eta of outgoing object a
Init.etabmin  = "-2.4d0"           # Minimum eta of outgoing object b
Init.etabmax   = "2.4d0"           # Maximum eta of outgoing object b
Init.acoabmax  = "100d0"

SuperChicRun(Init)


import Superchic_i.EventFiller as EF
ef = EF.LheEVNTFiller()
fileName = "evrecs/evrec"+Init.outtg[1:-1]+".dat"
ef.fileName = fileName
genSeq += ef

