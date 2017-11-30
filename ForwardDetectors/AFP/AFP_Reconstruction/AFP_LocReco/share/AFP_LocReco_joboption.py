#==============================================================
# Job options file for the ALFA_LocRec package
#==============================================================
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence         import AlgSequence
TopLocRecSeq  = AlgSequence()

#-- SiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf import AFP_SIDLocReco, AFP_SIDLocRecoTool, AFPSiDBasicKalmanTool
siDLocReco = AFP_SIDLocReco("AFP_SIDLocReco")
TopLocRecSeq += siDLocReco

from AFP_SiClusterTools.AFP_SiClusterToolsConf import AFPSiClusterTool
clusterTool=AFPSiClusterTool("AFPSiClusterTool")
ToolSvc+=clusterTool

# cluster algorithm
# from AFP_SiClusterTools.AFP_SiClusterToolsConf import AFPSiClusterBasicNearestNeighbour
# clusterNeighbour=AFPSiClusterBasicNearestNeighbour("AFPSiClusterBasicNearestNeighbour")
# ToolSvc+=clusterNeighbour

# clusterTool.clusterAlgTool = clusterNeighbour

# AFP_SIDLocReco.clusterTool=clusterTool

# cluster to local CS transformation
from AFP_SiClusterTools.AFP_SiClusterToolsConf import AFPSiRowColToLocalCSTool
rowColToLocal=AFPSiRowColToLocalCSTool("AFPSiRowColToLocalCSTool")
ToolSvc+=rowColToLocal

# rowColToLocal.numberOfLayersInStations = [2, 2, 2, 2]

# rowColToLocal.transformations = [1111, 1112, 1113, 1114,
#                                  1121, 1122, 1123, 1124,
#                                  1131, 1132, 1123, 1134,
#                                  #second layer first station transformation matrix
#                                  1211, 1212, 1213, 1214,
#                                  1221, 1222, 1223, 1224,
#                                  1231, 1232, 1223, 1234,
#                                  # first layer second station
#                                  2111, 2112, 2113, 2114,
#                                  2121, 2122, 2123, 2124,
#                                  2131, 2132, 2123, 2134,
#                                  #second layer first station transformation matrix
#                                  2211, 2212, 2213, 2214,
#                                  2221, 2222, 2223, 2224,
#                                  2231, 2232, 2223, 2234,
#                                  # first layer third station
#                                  3111, 3112, 3113, 3114,
#                                  3121, 3122, 3123, 3124,
#                                  3131, 3132, 3123, 3134,
#                                  #second layer first station transformation matrix
#                                  3211, 3212, 3213, 3214,
#                                  3221, 3222, 3223, 3224,
#                                  3231, 3232, 3223, 3234,
#                                  # first layer fourth station
#                                  4111, 4112, 4113, 4114,
#                                  4121, 4122, 4123, 4124,
#                                  4131, 4132, 4123, 4134,
#                                  #second layer first station transformation matrix
#                                  4211, 4212, 4213, 4214,
#                                  4221, 4222, 4223, 4224,
#                                  4231, 4232, 4223, 4234
#                                  ]
                                 

# rowColToLocal.transformationsErr = [111100, 111200, 111300, 111400,
#                                  112100, 112200, 112300, 112400,
#                                  113100, 113200, 112300, 113400,
#                                  #second layer first station transformation matrix
#                                  121100, 121200, 121300, 121400,
#                                  122100, 122200, 122300, 122400,
#                                  123100, 123200, 122300, 123400,
#                                  # first layer second station
#                                  211100, 211200, 211300, 211400,
#                                  212100, 212200, 212300, 212400,
#                                  213100, 213200, 212300, 213400,
#                                  #second layer first station transformation matrix
#                                  221100, 221200, 221300, 221400,
#                                  222100, 222200, 222300, 222400,
#                                  223100, 223200, 222300, 223400,
#                                  # first layer third station
#                                  311100, 311200, 311300, 311400,
#                                  312100, 312200, 312300, 312400,
#                                  313100, 313200, 312300, 313400,
#                                  #second layer first station transformation matrix
#                                  321100, 321200, 321300, 321400,
#                                  322100, 322200, 322300, 322400,
#                                  323100, 323200, 322300, 323400,
#                                  # first layer fourth station
#                                  411100, 411200, 411300, 411400,
#                                  412100, 412200, 412300, 412400,
#                                  413100, 413200, 412300, 413400,
#                                  #second layer first station transformation matrix
#                                  421100, 421200, 421300, 421400,
#                                  422100, 422200, 422300, 422400,
#                                  423100, 423200, 422300, 423400
#                                  ]
                                 

clusterTool.rowColToLocalCSTool = rowColToLocal


# Prepare track reconstruction algorithm tools - one for each station
kalmanTool1 = AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool1")
kalmanTool1.stationID=0
ToolSvc += kalmanTool1

# kalmanTool1.observationModel=[11, 12, 13, 14, # first row of the matrix
#                               21, 22, 23, 24] # second row of the matrix

# kalmanTool1.observationNoise=[1011, 1012, # first row of the matrix
#                               1021, 1022] # second row of the matrix

# kalmanTool1.processNoiseCov=[2011, 2012, 2013, 2014, # first row of the matrix
#                              2021, 2022, 2023, 2024, # second row of the matrix
#                              2031, 2032, 2033, 2034, # third row of the matrix
#                              2041, 2042, 2043, 2044] # fourth row of the matrix

# kalmanTool1.aposterioriCov=[3011, 3012, 3013, 3014, # first row of the matrix
#                              3021, 3022, 3023, 3024, # second row of the matrix
#                              3031, 3032, 3033, 3034, # third row of the matrix
#                              3041, 3042, 3043, 3044] # fourth row of the matrix

# # number of layers in each station, size of the vector determines number of stations
# kalmanTool1.numberOfLayersInStation = 3

# # Maximal distance at which cluster can be joined to the track
# kalmanTool1.maxAllowedDistance = 10

# # Minimal number of clusters in track. If there are less clusters track is rejected
# kalmanTool1.minClustersNumber = 3

kalmanTool2 = AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool2")
kalmanTool2.stationID=1
ToolSvc += kalmanTool2

kalmanTool3 = AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool3")
kalmanTool3.stationID=2
ToolSvc += kalmanTool3

kalmanTool4 = AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool4")
kalmanTool4.stationID=3
ToolSvc += kalmanTool4

trackRecoTool=AFP_SIDLocRecoTool("AFP_SIDLocRecoTool")
ToolSvc += trackRecoTool

trackRecoTool.recoTools = [kalmanTool1, kalmanTool2, kalmanTool3, kalmanTool4]
trackRecoTool.clusterTool=clusterTool

siDLocReco.recoTool = trackRecoTool


#-- TiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf      import AFP_TDLocReco
TopLocRecSeq += AFP_TDLocReco("AFP_TDLocReco")

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
if globalflags.DataSource()=='data':
        AFP_TDLocReco.DataType = 1
else:
        AFP_TDLocReco.DataType = 0

# select algo for timing reco and parameters
AFP_TDLocReco.ListAlgoTD = ["TDZVertex"]
AFP_TDLocReco.AmpThresh = 10.
