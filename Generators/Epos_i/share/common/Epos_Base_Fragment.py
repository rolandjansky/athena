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

import subprocess
paramcard = subprocess.Popen(['get_files','-data',genSeq.Epos.ParamFile])
paramcard.wait()
if not os.access(genSeq.Epos.ParamFile, os.R_OK):
  raise RuntimeError('Could not get param card '+genSeq.Epos.ParamFile)
inputFiles = "dpmjet.dat \
              phojet_fitpar.dat \
              sectnu-II-04 \
              qgsdat-II-04.lzma \
              epos.inidi \
              epos.initl \
              epos.iniev \
              epos.inirj \
              epos.inics \
              epos.inirj.lhc \
              epos.inics.lhc"
if not os.path.exists("tabs"):
    os.mkdir("tabs")

inputcard = subprocess.Popen(['get_files','-data',inputFiles])
inputcard.wait()
os.system("mv %s tabs/" % inputFiles)
os.system("cp tabs/epos.inirj.lhc tabs/epos.inirj.lhc.lhc")
os.system("cp tabs/epos.inics.lhc tabs/epos.inics.lhc.lhc")
