# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# script to update the digitization AthFile cache 
# when a new set of input files for RTT jobs is 
# required
#

fnames = ["rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._02899.pool.root.1",
          "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105001.pythia_minbias.simul.HITS.e349_s462/HITS.024528._00030.pool.root",
          "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.005008.CavernInput.simul.HITS.e4_e365_s477/HITS.028773._00040.pool.root.1",
          "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.108863.Hijing_beamgas.simul.HITS.e4_s470/HITS.024853._00005.pool.root.1",
          "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.108852.BeamHaloInputs.simul.HITS.e4_e348_s473/HITS.025027._00007.pool.root",
          "rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10"]
import PyUtils.AthFile as af
for fname in fnames:
 af.fopen(fname) # inspect all the files

# cache is filled, save it
af.server.save_cache('digitization-afcache.ascii')

raise SystemExit("New cache created.")
