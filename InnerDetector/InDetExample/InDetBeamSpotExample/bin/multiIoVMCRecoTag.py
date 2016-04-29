# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from InDetBeamSpotExample.COOLUtils import *
from InDetBeamSpotExample.BeamSpotData import *
from PyCool import cool

import sys

#beamspotnt.py --rl 158045 --ru 167844 -b --grl /afs/cern.ch/user/a/atlasdqm/grlgen/Archive/CombinedPerf/Tracking/Tracking_v01/data10_7TeV.periodAllYear_DetStatus-v21-pro05_CoolRunQuery-00-04-00_Tracking.xml -p data10_7TeV.AllYear -f ~atlidbs/nt/repro/beamspotnt-IndetBeampos-Oct10-Collision_7T_2010_07-v0and1.root --tag IndetBeampos-data10_7TeV-PeriodDtoI-ave-001 ave 

#beamspotnt.py --time --rl 215414 --ru 216432 -b  -p data12_8TeV.AllYear -f ~atlidbs/nt/cool/beamspotnt-COOL-Current.root summary

# beamspotman.py --srctag IndetBeampos-RunDep-MC11-MC12prep-pPbprep-001 -t IndetBeampos-RunDep-MC11-MC12prep-pPbprep-001 --destdbname OFLP200 --srcdbname OFLP200 upload IndetBeampos-RunDep-MC11-MC12prep-pPbprep-001.db

tagdict = {
    (0,141748)       : "IndetBeampos-unconstrained",                        # pre - 2009 900 GeV (unconstrained)
    (141748, 142308) : "IndetBeampos-data09_900GeV-ave-001",                # 2009 900 GeV (new)
    (142308, 142309) : "IndetBeampos-unconstrained",                        # 2009 2 TeV (unconstrained)
    (142309, 142402) : "IndetBeampos-data09_900GeV-ave-001",                # 2009 900 GeV (new)
    (142402, 142403) : "IndetBeampos-unconstrained",                        # 2009 2 TeV (unconstrained)
    (142403, 152166) : "IndetBeampos-unconstrained",                        # pre - 2010 7 TeV (unconstrained)       
    (152166, 154465) : "IndetBeampos-data10_7TeV-PeriodDtoI-ave-001",       # 2010 7 TeV (new)                 
    (154465, 154813) : "IndetBeampos-data10_900GeV-ave-001",                # 2010 900 GeV (new)               
    (154813, 168665) : "IndetBeampos-data10_7TeV-PeriodDtoI-ave-001",       # 2010 7 TeV (new)                 
    (168665, 177531) : "IndetBeampos-HI-PeriodJ-ave-001",                   # 2010 HI (new)
    (177531, 178163) : "IndetBeampos-data11_7TeV-PeriodD1toL6-ave-001",     # 2011 7 TeV (diff)
    (178163, 178264) : "IndetBeampos-data11_2p76TeV-ave-001",               # 2011 2.76 TeV (new)
    (178264, 193211) : "IndetBeampos-data11_7TeV-PeriodD1toL6-ave-001",     # 2011 7 TeV (diff)
    (193211, 195847) : "IndetBeampos-data11_hi-PeriodN1toN5-ave-001",       # 2011 HI (same)
    (195847, 200805) : "IndetBeampos-data12_8TeV-AllYear-ave-001",          # 2012 8 TeV (diff)
    (200805, 200806) : "IndetBeampos-data12_8TeV-200805-LowMu-ave-001",     # 2012 8 TeV mu=0 data (new)  
    (200806, 206881) : "IndetBeampos-data12_8TeV-AllYear-ave-001",          # 2012 8 TeV (diff)
    (206881, 206955) : "IndetBeampos-data12_8TeV-Beta90m-ave-001",          # 2012 8 TeV beta*=90m, mu=0 data (new)
    (206955, 210184) : "IndetBeampos-data12_8TeV-AllYear-ave-001",          # 2012 8 TeV (diff)
    (210184, 210185) : "IndetBeampos-data12_hip-210184-ave-001",            # 2012 HIP z=0 (same)
    (210185, 210186) : "IndetBeampos-unconstrained",                        # 2012 HIP Z=+50 (unconstrained)
    (210186, 210187) : "IndetBeampos-unconstrained",                        # 2012 HIP Z=+50 (unconstrained)
    (210187, 216399) : "IndetBeampos-data12_8TeV-AllYear-ave-001",          # 2012 8 TeV (diff)                
    (216399, 216432) : "IndetBeampos-data12_8TeV-PeriodM25ns-ave-001",      # 2012 8 TeV 25ns data (new)       
    (216432, 217946) : "IndetBeampos-data12_8TeV-AllYear-ave-001",          # 2012 8 TeV (diff)                
    (217946, 219171) : "IndetBeampos-data13_hip-PeriodAtoB-ave-001",        # 2013 HIP (same)
    (219171, 219366) : "IndetBeampos-data13_2p76TeV-AllYear-ave-001",       # 2013 2.76 TeV (same)
    (219366, 222222) : "IndetBeampos-unconstrained",                        # unused, unconstrained
    (222222, 222250) : "IndetBeampos-13TeV-0.45m-300urad",                  # DC14 (mu=30), 25ns
    (222250, 222500) : "IndetBeampos-13TeV-0.45m-300urad",                  # DC14 (mu=30), 50ns
		(222500, 222506) : "IndetBeampos-13TeV-LowMu-001",                      # mc15 50ns, low-mu
    #(222500, 222509) : "IndetBeampos-data15_13TeV-LowMu-ave-sigmazp1mm-001",# mc15 low-mu averages from data, with sigmaz inflated by +1mm (requested by Anthony Morley)
    (222506, 222508) : "IndetBeampos-data15_PbPb-ave-001",                  # Average PbPb pp beam in 2015 data
		(222508, 222510) : "IndetBeampos-data15_5TeV-ave-001",                  # Average 5 TeV pp beamspot in 2015 data
		(222510, 222525) : "IndetBeampos-13TeV-0.80m-290urad-1.24ns-2.7umr",    # mc15 50ns
    (222525, 226000) : "IndetBeampos-13TeV-0.80m-290urad-1.24ns-2.7umr",    # mc15 25ns 
    (226000, 228000) : "IndetBeampos-data15_PbPb-ave-001",                  # Average PbPb pp beam in 2015 data
    (228000, 230000) : "IndetBeampos-data15_5TeV-ave-001",                  # Average 5 TeV pp beamspot in 2015 data
    (230000, 240000) : "IndetBeampos-13TeV-0.80m-290urad-1.24ns-2.7umr",    # mc15 25ns
    (240000, 250000) : "IndetBeampos-14TeV-SigmaXY12um-SigmaZ50mm-001",     # tag for upgrade studies
    (250000, 0     ) : "IndetBeampos-13TeV-0.80m-290urad-1.24ns-2.7umr"     # mc15 25ns 
    }


tags   = [tagdict[key] for key in sorted(tagdict.iterkeys())]
runmin = [rl for (rl,ru) in sorted(tagdict.iterkeys())]
runmax = [ru for (rl,ru) in sorted(tagdict.iterkeys())]

for (rl,ru) in sorted(tagdict.iterkeys(),reverse=True):
    print "| !%s | [%s,0] - [%s,0) |" % (tagdict[(rl,ru)], rl, ru)

assert(len(runmin) == len(runmax) == len(tags))
nTag = len(tags)

tag='IndetBeampos-RunDep-MC15-BestKnowledge-002-03'
dbfile = tag + '.db'
folderHandle = openBeamSpotDbFile(dbfile, dbName = 'OFLP200', forceNew = True)

for i in range(nTag):
    BSDataColl = BeamSpotCOOL(tags[i], 'COOLOFL_INDET/OFLP200', '/Indet/Beampos')
    BSDataColl.lbMax = cool.ValidityKeyMax

    if runmax[i] == 0:
        runmax[i] = (1 << 31)-1

    totN = 0
    for BSData in BSDataColl:
        totN += 1
        if totN == 1:
            writeBeamSpotEntry(folderHandle, tag,
                               runMin=runmin[i], runMax=runmax[i],
                               status=BSData.status,
                               posX=BSData.posX,
                               posY=BSData.posY,
                               posZ=BSData.posZ,
                               sigmaX=BSData.sigmaX,
                               sigmaY=BSData.sigmaY,
                               sigmaZ=BSData.sigmaZ,
                               tiltX=BSData.tiltX,
                               tiltY=BSData.tiltY,
                               sigmaXY=BSData.sigmaXY,
                               posXErr=BSData.posXErr,
                               posYErr=BSData.posYErr,
                               posZErr=BSData.posZErr,
                               sigmaXErr=BSData.sigmaXErr,
                               sigmaYErr=BSData.sigmaYErr,
                               sigmaZErr=BSData.sigmaZErr,
                               tiltXErr=BSData.tiltXErr,
                               tiltYErr=BSData.tiltYErr,
                               sigmaXYErr=BSData.sigmaXYErr)
           

    if totN > 1:
        print "WARNING: tag had more than one IOV"
