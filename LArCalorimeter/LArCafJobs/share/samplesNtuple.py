# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from LArCafJobs.LArCafJobsConfig import DefaultShapeDumper
DefaultShapeDumper('LArShapeDumper', 'FREE', noiseSignifCut = 5, doShape = True, doTrigger = True, caloType = 'EMHECFCAL')

