from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

def JetMonGetAxisRange():
   
   import math
   
   from AthenaCommon.BeamFlags import jobproperties

   axisranges = {}

#   mlog.info ("Beam type and energy: %s , %s" %(jobproperties.Beam.beamType,jobproperties.Beam.energy))

   axisranges["radBins"]  = [  0.0,        1.0,      0.025     ] # radial bins
   axisranges["etaBins"]  = [ -5.0,        5.0,      0.2       ] # eta bins
   axisranges["phiBins"]  = [ -math.pi,   math.pi,   0.2       ] # phi bins
   axisranges["massBins"] = [  0.0*GeV,  30.0*GeV,  0.2* GeV  ] # mass bins
   axisranges["radBinsHD"]  = [  0.0,        1.0,      0.1      ]
   axisranges["etaBinsHD"]  = [ -5.0,        5.0,      0.1      ]
   axisranges["phiBinsHD"] = [ -math.pi,   math.pi,   0.1      ]
   axisranges["massBinsHD"] = [  0.0*GeV,  30.0*GeV,  0.2*GeV  ]
   axisranges["njBins"]  = [ 0.,        50.0,      1.      ]

   if jobproperties.Beam.beamType == 'cosmics' :

	   # -- 1-dim binning
	   axisranges["eBins"]    = [  0.0*GeV, 1500.0*GeV, 10.0*GeV   ] # energy bins
	   axisranges["etBins"]    = [  0.0*GeV, 1500.0*GeV, 10.0*GeV   ] # energy bins
	   axisranges["econBins"]    = [  -2.0*GeV, 80.0*GeV, 0.5*GeV   ] # energy bin
	   # -- 2-dim binning
	   axisranges["eBinsHD"]    = [  0.0*GeV, 1500.0*GeV, 20.0*GeV  ]
	   axisranges["etBinsHD"]    = [  0.0*GeV, 1500.0*GeV, 20.0*GeV  ]
   
   elif jobproperties.Beam.beamType == 'collisions' :
   
	   if jobproperties.Beam.energy == 450.*GeV:
	   	# -- 1-dim binning
	   	axisranges["eBins"]    = [  0.0*GeV, 500.0*GeV, 5.0*GeV   ] # energy bin
	   	axisranges["etBins"]    = [  0.0*GeV, 200.0*GeV, 2.0*GeV   ] # energy bin
	   	axisranges["econBins"]    = [  -2.0*GeV, 80.0*GeV, 0.4*GeV   ] # energy bin
	   	# -- 2-dim binning
	   	axisranges["eBinsHD"]    = [  0.0*GeV, 500.0*GeV, 5.0*GeV  ]
	   	axisranges["etBinsHD"]    = [  0.0*GeV, 200.0*GeV, 2.0*GeV  ]

	   elif jobproperties.Beam.energy == 3.5*TeV:

	   	# -- 1-dim binning
	   	axisranges["eBins"]    = [  0.0*GeV, 3500.0*GeV, 5.0*GeV   ] # energy bin
	   	axisranges["etBins"]    = [  0.0*GeV, 1000.0*GeV, 2.0*GeV   ] # energy bin
	   	axisranges["econBins"]    = [  -2.0*GeV, 200.0*GeV, 0.6*GeV   ] # energy bin
	   	# -- 2-dim binning
	   	axisranges["eBinsHD"]    = [  0.0*GeV, 1000.0*GeV, 5.0*GeV  ]
	   	axisranges["etBinsHD"]    = [  0.0*GeV, 500.0*GeV, 2.0*GeV   ] # energy bin

	   else:	

	   	# -- 1-dim binning
	   	axisranges["eBins"]    = [  0.0*GeV, 2000.0*GeV, 5.0*GeV   ] # energy bin ! tuned for the p+Pb run (it was 5000.0*GeV)
	   	axisranges["etBins"]    = [  0.0*GeV, 500.0*GeV, 2.0*GeV   ] # energy bin ! tuned for the p+Pb run (it was 1000.0*GeV)
	   	axisranges["econBins"]    = [  -2.0*GeV, 120.0*GeV, 0.6*GeV   ] # energy bin
	   	# -- 2-dim binning
	   	axisranges["eBinsHD"]    = [  0.0*GeV, 1000.0*GeV, 5.0*GeV  ]
	   	axisranges["etBinsHD"]    = [  0.0*GeV, 500.0*GeV, 2.0*GeV   ] # energy bin

   else:
  	   # -- 1-dim binning
	   axisranges["eBins"]    = [  0.0*GeV, 1000.0*GeV, 5.0*GeV   ] # energy bins
	   axisranges["etBins"]    = [  0.0*GeV, 200.0*GeV, 2.0*GeV   ] # energy bins
	   axisranges["econBins"]    = [  -2.0*GeV, 200.0*GeV, 0.5*GeV   ] # energy bin
	   # -- 2-dim binning
	   axisranges["eBinsHD"]    = [  0.0*GeV, 1000.0*GeV, 5.0*GeV  ]
	   axisranges["etBinsHD"]    = [  0.0*GeV, 200.0*GeV, 2.0*GeV  ]
           
   from AthenaMonitoring.DQMonFlags import DQMonFlags
   if DQMonFlags.monManDataType == 'heavyioncollisions':
           axisranges["njBins"]  = [ 0.,        100.0,      1.      ]
           # -- 1-dim binning
           axisranges["eBins"]    = [  0.0*GeV, 5000.0*GeV, 5.0*GeV   ] # energy bin
           axisranges["etBins"]    = [  0.0*GeV, 1000.0*GeV, 2.0*GeV   ] # energy bin
           axisranges["econBins"]    = [  -2.0*GeV, 120.0*GeV, 0.6*GeV   ] # energy bin
           # -- 2-dim binning
           axisranges["eBinsHD"]    = [  0.0*GeV, 1000.0*GeV, 5.0*GeV  ]
           axisranges["etBinsHD"]    = [  0.0*GeV, 500.0*GeV, 2.0*GeV   ] # energy bin           

   return axisranges

