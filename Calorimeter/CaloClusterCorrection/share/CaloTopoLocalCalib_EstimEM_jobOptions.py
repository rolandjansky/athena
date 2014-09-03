#
# jobOptions for replacing classification tool in the local calibration of CaloTopoClusters
# should be included after CaloTopoLocalCalib_jobOptions.py
#

CaloTopoClusterMaker.LocalCalib.ClusterClassificationToolName = "EstimEMClusterClassificationTool/EstimEM"
CaloTopoClusterMaker.LocalCalib.ClusterRecoStatus = 2 # 0 = ANY, 1 = EM, 2 = HAD, 4 = MUON

# m_debug produce dump of all important variables within classification if switched on
# useless if not debugging code
#CaloTopoClusterMaker.LocalCalib.EstimEM.m_debug = False
#CaloTopoClusterMaker.LocalCalib.EstimEM.m_eta_ene_cut = [ 1000.0, 1500.0, 1500.0, 2000.0, 3000.0, 2500.0, 1500.0, 1000.0, 1000.0, 1000.0,
#                                                          500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 500.0, 1000.0, 100.0]
CaloTopoClusterMaker.LocalCalib.EstimEM.m_debug = False
CaloTopoClusterMaker.LocalCalib.EstimEM.m_eta_ene_cut = [ 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0,
                                                          10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0]

# CaloTopoClusterMaker.LocalCalib.EstimEM.m_maxEstimEM = 0.95
CaloTopoClusterMaker.LocalCalib.EstimEM.m_maxEstimEM = 0.9

#//Slice = 0, Energy cut = 1000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
132.663473,	294.894573,	331.139293,
-107.785596,	-246.440573,	-276.101236,
0.000000,
0.323280,
0.676720
]

#//Slice = 1, Energy cut = 1500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
128.979021,	300.401733,	356.155902,
-103.525051,	-249.071825,	-305.616351,
0.026782,
0.239764,
0.733454
]

#//Slice = 2, Energy cut = 1500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
154.481371,	324.681064,	348.507498,
-131.619868,	-271.816904,	-292.970558,
0.288712,
0.116868,
0.594419
]

#//Slice = 3, Energy cut = 2000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
194.109473,	343.775140,	327.582352,
-173.469907,	-286.341491,	-273.406725,
0.387050,
0.000000,
0.612949
]

#//Slice = 4, Energy cut = 3000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
251.662496,	432.577289,	399.167737,
-225.592084,	-373.310112,	-338.955113,
0.851943,
0.000000,
0.148057
]

#//Slice = 5, Energy cut = 2500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
241.888973,	496.103004,	479.875299,
-208.019301,	-448.880910,	-420.837267,
0.219300,
0.000039,
0.780661
]

#//Slice = 6, Energy cut = 1500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
211.370580,	415.422808,	461.285739,
-179.433979,	-357.420584,	-388.137130,
0.126389,
0.000015,
0.873596
]

#//Slice = 7, Energy cut = 1000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
165.446823,	357.041653,	277.453575,
-126.755291,	-292.821654,	-204.875434,
0.201307,
0.000107,
0.798586
]

#//Slice = 8, Energy cut = 1000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
120.609114,	243.433616,	193.815015,
-99.612929,	-188.309305,	-156.097266,
0.650514,
0.130615,
0.218872
]

#//Slice = 9, Energy cut = 1000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
110.968569,	192.734331,	252.167396,
-87.691010,	-147.520772,	-204.615303,
0.317621,
0.007241,
0.675138
]

#//Slice = 10, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
93.339568,	171.812583,	233.242445,
-65.259592,	-135.165830,	-188.012997,
0.062678,
0.000027,
0.937296
]

#//Slice = 11, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
98.107290,	145.441444,	219.677958,
-71.967887,	-115.104937,	-175.675981,
0.049173,
0.092943,
0.857883
]

#//Slice = 12, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
120.735334,	167.701626,	235.382412,
-103.139723,	-134.521108,	-197.182272,
0.246193,
0.003235,
0.750571
]

#//Slice = 13, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
137.722821,	167.191712,	254.844063,
-127.026858,	-122.413116,	-207.280046,
0.535395,
0.007966,
0.456639
]

#//Slice = 14, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
142.680548,	156.070718,	271.652894,
-133.527824,	-113.730321,	-232.734727,
0.740981,
0.027905,
0.231114
]

#//Slice = 15, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
130.365661,	157.786573,	308.420753,
-118.248889,	-126.328053,	-269.821489,
0.158902,
0.103690,
0.737408
]

#//Slice = 16, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
67.302381,	95.545254,	107.705655,
-43.939677,	-62.138882,	-68.032622,
0.018009,
0.251576,
0.730415
]

#//Slice = 17, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
63.766815,	68.637942,	111.208023,
-43.958259,	-39.543322,	-68.049336,
0.326711,
0.029548,
0.643741
]

#//Slice = 18, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
68.642173,	70.598989,	106.986940,
-51.457412,	-42.706737,	-63.480334,
0.201327,
0.119328,
0.679345
]

#//Slice = 19, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
65.195601,	64.292178,	109.606089,
-46.328512,	-37.991985,	-62.099771,
0.349766,
0.000306,
0.649928
]

#//Slice = 20, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
66.376976,	64.745605,	118.831911,
-47.451523,	-39.597509,	-73.558916,
0.343002,
0.258769,
0.398228
]

#//Slice = 21, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
74.656010,	64.876017,	131.485779,
-57.942321,	-43.002364,	-87.418731,
0.504002,
0.155534,
0.340464
]

#//Slice = 22, Energy cut = 500.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
76.435092,	64.257951,	86.846559,
-59.502343,	-45.520636,	-43.579852,
0.485906,
0.018628,
0.495467
]

#//Slice = 23, Energy cut = 1000.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
80.462489,	62.335460,	-13.202530,
-64.302958,	-45.599999,	43.095554,
0.594124,
0.086763,
0.319112
]

#//Slice = 24, Energy cut = 100.000000 MeV
CaloTopoClusterMaker.LocalCalib.EstimEM.m_params += [
0.000000,	0.000000,	0.000000,
0.000000,	0.000000,	0.000000,
0.299084,
0.299084,
0.401833
]

