# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
from EventBookkeeperTools.PyFilterTester import PyFilterTester
topSequence = AlgSequence()
topSequence += PyFilterTester('myFilter')

theApp.EvtMax = 10
