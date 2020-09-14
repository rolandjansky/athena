## Base config for Epos
from Epos_i.Epos_iConf import Epos
genSeq += Epos("Epos")
evgenConfig.generators += ["Epos"]

genSeq.Epos.BeamMomentum     = -runArgs.ecmEnergy/2.0
genSeq.Epos.TargetMomentum   = runArgs.ecmEnergy/2.0
genSeq.Epos.PrimaryParticle  = 1
genSeq.Epos.TargetParticle   = 1
genSeq.Epos.Model            = 0
genSeq.Epos.TabCreate        = 0
genSeq.Epos.ParamFile        = "epos_crmc.param"

## Get files from the InstallArea
import os
os.system("get_files %s" % genSeq.Epos.ParamFile)
inputFiles = "qgsjet.dat \
              qgsjet.ncs \
              sectnu-II-03 \
              epos.initl \
              epos.iniev \
              epos.inirj \
              epos.inics \
              epos.inirj.lhc \
              epos.inics.lhc"
if not os.path.exists("tabs"):
    os.mkdir("tabs")
os.system("get_files %s" % inputFiles)
os.system("mv %s tabs/" % inputFiles)
os.system("cp tabs/epos.inirj.lhc tabs/epos.inirj.lhc.lhc")
os.system("cp tabs/epos.inics.lhc tabs/epos.inics.lhc.lhc")
