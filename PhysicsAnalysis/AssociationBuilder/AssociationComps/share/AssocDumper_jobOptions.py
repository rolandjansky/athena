####################################
#  AssocDumper Job Options (fragment)
#  author: Sebastien Binet
####################################

import EventKernel.ParticleDataType

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import AthenaCommon.Constants as Lvl
from AthenaCommon import CfgMgr
topSequence += CfgMgr.AssocDumper(
    "AssocDumper" ,
    INav4MomAssocs = "AtlfastMcAodAssocs",
    OutputStream   = "MsgStream",
    #OutputStream   = "atlfast.mc.assocs.ref", # a valid filename
    #OutputStream   = "stdout",                # a valid file descriptor
    #OutputStream   = "stderr",                # a valid file descriptor
    OutputLevel    = Lvl.INFO
    )

