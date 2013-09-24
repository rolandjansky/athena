##########################################################
# User Digitization Job Options

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax = 10

jobproperties.AthenaCommonFlags.PoolHitsInput=["~/data/HITS.024842._00010.pool.root.2"]

jobproperties.AthenaCommonFlags.PoolRDOOutput="RDO.pool.root"


#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
#from DigitizationFlags import jobproperties

#jobproperties.Digitization.doMinimumBias=True
#jobproperties.Digitization.minBiasInputCols=["/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00001.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00002.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00003.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00004.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00005.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00006.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00007.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00008.pool.root.2",
#                                             "/data/atlashiggs1/rel12_hit/MinBias/misal1_csc11.005001.pythia_minbias.simul.HITS.v12003104_tid004278._00009.pool.root.2"]

#jobproperties.Digitization.doCavern=True
#jobproperties.Digitization.cavernInputCols=["/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00001.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00002.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00003.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00004.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00005.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00006.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00007.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00008.pool.root.1",
#                                            "/data/atlashiggs1/rel12_hit/Cavern/misal1_csc11.007901.cavernbg_sf01.simul.HITS.v12003104_tid004002._00009.pool.root.1"]

jobproperties.Digitization.doInDetNoise=True
jobproperties.Digitization.doCaloNoise=True
jobproperties.Digitization.doMuonNoise=True

# Detector description
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-02-00-00'

##########################################################
