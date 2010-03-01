# @file: INav4MomDumper_jobOptions.py
# @author: Sebastien Binet <binet@cern.ch>
# @purpose: a simple example on how to use the INav4Mom dumper

## get a handle on the topSequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get definition of output levels
from AthenaCommon.Constants import *

##################################
# Dump the INavigable4MomentumCollections
##################################
from AtlfastConversionAlgs.tests import INav4MomItemList
from EventCommonAlgs.EventCommonAlgsConf import INav4MomDumper
topSequence += INav4MomDumper(
    INav4Moms    = [ "AtlfastParticleJetContainer",
                     "AtlfastElectronCollection" ],
    OutputStream = "inav4moms.log",
    OutputLevel  = INFO
    )
