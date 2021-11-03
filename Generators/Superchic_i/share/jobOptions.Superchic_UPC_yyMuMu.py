evgenConfig.description = "Superchic gamma + gamma UPC collisions at 5020 GeV"
evgenConfig.keywords = ["2photon","2lepton"]
#evgenConfig.weighting = 0
evgenConfig.contact = ["prabhakar.palni@cern.ch"]
evgenConfig.generators += ["Superchic"]

from Superchic_i.SuperChicUtils import SuperChicConfig, SuperChicRun

#class with the superchic initialization parameters.  Please see SuperChicUtils for a complete list of tunable parameters.
scConfig = SuperChicConfig(runArgs)

scConfig.isurv = 4                  # Model of soft survival (from 1 -> 4, corresponding to arXiv:1306.2149)
scConfig.PDFname = 'MMHT2014lo68cl' # PDF set name
scConfig.PDFmember = 0              # PDF member
scConfig.proc = 59                  # Process number (59 = gg->gg, 56 = gg->ee, 68 = gg->a->gg ); Please consult Superchic Manual https://superchic.hepforge.org/
scConfig.beam = 'ion'              # Beam type ('prot', 'ion')
scConfig.sfaci = False             # Include soft survival effects
scConfig.ncall = 100000            # Number of calls for preconditioning
scConfig.itmx = 1                 # Number of iterations for preconditioning
scConfig.prec = 1.0                # precision

SuperChicRun(scConfig, genSeq)

import Superchic_i.EventFiller as EF
ef = EF.LheEVNTFiller()
ef.fileName = scConfig.outputLHEFile()
outputFileName = "outputs/output"+scConfig.outtg+".dat"
ef.outputFileName = outputFileName
genSeq += ef
