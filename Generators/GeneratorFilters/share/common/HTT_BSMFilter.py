#--------------------------------------------------------------
# Filter for applying to BSM samples to select only
# events where at least one BSM particle is inside one of
# the specified eta and phi ranges.
# Detector areas available in HTTSim are:
# phi= (0.3, 0.5)
# eta= (0.1, 0.3), (0.7, 0.9), (1.2, 1.4), (2.0, 2.2), (3.2, 3.4)
# where the eta ranges are not symmetrised.
# katherine.pachal@cern.ch
#--------------------------------------------------------------
#

from GeneratorFilters.GeneratorFiltersConf import MultiParticleFilter

# Region 1 of 5
filter_R1 = MultiParticleFilter("filter_region1")
filtSeq += filter_R1
filtSeq.filter_region1.useIsBSM = True
filtSeq.filter_region1.particleStatus = [1]
filtSeq.filter_region1.etaMinParticle = 0.1
filtSeq.filter_region1.etaMaxParticle = 0.3
filtSeq.filter_region1.useAbsEta = False
filtSeq.filter_region1.phiMinParticle = 0.2
filtSeq.filter_region1.phiMaxParticle = 0.6

# Region 2 of 5
filter_R2 = MultiParticleFilter("filter_region2")
filtSeq += filter_R2
filtSeq.filter_region2.useIsBSM = True
filtSeq.filter_region2.particleStatus = [1]
filtSeq.filter_region2.etaMinParticle = 0.7
filtSeq.filter_region2.etaMaxParticle = 0.9
filtSeq.filter_region2.useAbsEta = False
filtSeq.filter_region2.phiMinParticle = 0.2
filtSeq.filter_region2.phiMaxParticle = 0.6

# Region 3 of 5
filter_R3 = MultiParticleFilter("filter_region3")
filtSeq += filter_R3
filtSeq.filter_region3.useIsBSM = True
filtSeq.filter_region3.particleStatus = [1]
filtSeq.filter_region3.etaMinParticle = 1.2
filtSeq.filter_region3.etaMaxParticle = 1.4
filtSeq.filter_region3.useAbsEta = False
filtSeq.filter_region3.phiMinParticle = 0.2
filtSeq.filter_region3.phiMaxParticle = 0.6

# Region 4 of 5
filter_R4 = MultiParticleFilter("filter_region4")
filtSeq += filter_R4
filtSeq.filter_region4.useIsBSM = True
filtSeq.filter_region4.particleStatus = [1]
filtSeq.filter_region4.etaMinParticle = 2.0
filtSeq.filter_region4.etaMaxParticle = 2.2
filtSeq.filter_region4.useAbsEta = False
filtSeq.filter_region4.phiMinParticle = 0.2
filtSeq.filter_region4.phiMaxParticle = 0.6

# Region 5 of 5
filter_R5 = MultiParticleFilter("filter_region5")
filtSeq += filter_R5
filtSeq.filter_region5.useIsBSM = True
filtSeq.filter_region5.particleStatus = [1]
filtSeq.filter_region5.etaMinParticle = 3.2
filtSeq.filter_region5.etaMaxParticle = 3.4
filtSeq.filter_region5.useAbsEta = False
filtSeq.filter_region5.phiMinParticle = 0.2
filtSeq.filter_region5.phiMaxParticle = 0.6

# And turn them on
filtSeq.Expression = "filter_region1 or filter_region2 or filter_region3 or filter_region4 or filter_region5"
