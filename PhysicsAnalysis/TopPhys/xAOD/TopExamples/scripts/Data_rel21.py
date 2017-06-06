# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TopExamples.grid
import TopExamples.ami

# Just a few runs processed with 21.0.19.2
DATA16 = [
            'data16_13TeV.00297730.physics_Main.deriv.DAOD_{}.r9264_p3083_p3142',
            'data16_13TeV.00304008.physics_Main.deriv.DAOD_{}.r9264_p3083_p3142',
            'data16_13TeV.00307126.physics_Main.deriv.DAOD_{}.r9264_p3083_p3142',
            'data16_13TeV.00311071.physics_Main.deriv.DAOD_{}.r9264_p3083_p3142',
            'data16_13TeV.00311287.physics_Main.deriv.DAOD_{}.r9264_p3083_p3142',
            'data16_13TeV.00311321.physics_Main.deriv.DAOD_{}.r9264_p3083_p3142',
            ]

TopExamples.grid.Add('Data16_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA16]
