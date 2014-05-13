# +===========================================================================+
# +                                                                           +
# + Author ........: F. Ledroit                                               +
# + Institut ......: ISN Grenoble                                             +
# + Creation date .: 04/02/2003                                               +
# + Subject .......: Job Option file to be used with zebra input              +
# + ------------------------------------------------------------------------- +
# This jobOptions file is for creating LArTTL1 from simulated Hits.
# Not to be used for TB.
include( "LArSim/LArSim_MC_jobOptions.py" )

# switch neighbour initialisation off (default = true = perform init.)
#DetDescrCnvSvc.DoInitNeighbours = FALSE

# The following are properties for LArL1Sim:
#
# --------------------------------------------------------
# ............ declare the used top algo.
# --------------------------------------------------------
theApp.Dlls += ["LArL1Sim"]
theApp.TopAlg += [ "LArTTL1Maker"]
#
# --------------------------------------------------------
# ............ Algorithms Private Options
# --------------------------------------------------------
#
#
# .................. sub-detector hit containers
# 
#
LArTTL1Maker = Algorithm( "LArTTL1Maker" )
LArTTL1Maker.SubDetectors = "LAr_All"
# LArTTL1Maker.SubDetectors = "LAr_EmBarrel"
# LArTTL1Maker.SubDetectors = "LAr_EmEndCap"
# LArTTL1Maker.SubDetectors = "LAr_Em"
# LArTTL1Maker.SubDetectors = "LAr_HEC"
#
# .................. set the TTL1 container name
#
LArTTL1Maker.EmTTL1ContainerName = "LArTTL1EM"
LArTTL1Maker.HadTTL1ContainerName = "LArTTL1HAD"
#
# ............ if true, noise added in all subdetectors (default = true)
#
#LArTTL1Maker.NoiseOnOff = FALSE
