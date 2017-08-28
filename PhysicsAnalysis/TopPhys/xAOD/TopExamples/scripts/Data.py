# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TopExamples.grid
import TopExamples.ami

# Reprocessed 2015 data
# includes runs in:
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/GoodRunsLists/data15_13TeV/20160622/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml
DATA15 = ['data15_13TeV.periodD.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667',
          'data15_13TeV.periodE.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667',
          'data15_13TeV.periodF.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667',
          'data15_13TeV.periodG.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667',
          'data15_13TeV.periodH.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667',
          'data15_13TeV.periodJ.physics_Main.PhysCont.DAOD_{}.grp15_v01_p2667']

# Full 2016 data (periods A-L, lumi = 33257.2 pb^{-1})
# includes runs in:
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/GoodRunsLists/data16_13TeV/20161101/data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml
DATA16 = ['data16_13TeV.periodA.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2667',
          'data16_13TeV.periodB.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2667',
          'data16_13TeV.periodC.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2667_p2689',
          'data16_13TeV.periodD.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2689',
          'data16_13TeV.periodE.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2689',
          'data16_13TeV.periodF.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2689',
          'data16_13TeV.periodG.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2769',
          'data16_13TeV.periodI.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2769',
          'data16_13TeV.periodK.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2769',
          'data16_13TeV.periodL.physics_Main.PhysCont.DAOD_{}.grp16_v01_p2769_p2840',]

TopExamples.grid.Add('Data15_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA15]
TopExamples.grid.Add('Data15_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA15]
TopExamples.grid.Add('Data15_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA15]
TopExamples.grid.Add('Data15_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA15]

TopExamples.grid.Add('Data16_TOPQ1').datasets = [i.format('TOPQ1') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ2').datasets = [i.format('TOPQ2') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ3').datasets = [i.format('TOPQ3') for i in DATA16]
TopExamples.grid.Add('Data16_TOPQ4').datasets = [i.format('TOPQ4') for i in DATA16]
