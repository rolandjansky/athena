## Algorithm sequence 
from AthenaCommon.AlgSequence import AlgSequence 
topSeq = AlgSequence()

## Simulation flags 
from G4AtlasApps.SimFlags import SimFlags 
SimFlags.load_atlas_flags()
SimFlags.ForwardDetectors.set_On()
SimFlags.ForwardDetectors=2
SimFlags.RunNumber = 222500 
SimFlags.MagneticField.set_On()
SimFlags.MagneticField.lock()
SimFlags.BeamEffectOptions.set_On()
SimFlags.BeamEffectOptions ={'HalfXing_x_b1':0.0,
                             'HalfXing_x_b2':0.0,
                             'HalfXing_y_b1':170E-6,
       	       	       	     'HalfXing_y_b2':-170E-6,
                             'Sigma_px_b1':35.E-6,
       	    	       	     'Sigma_px_b2':35.E-6,
                             'Sigma_py_b1':35.E-6,
       	    	       	     'Sigma_py_b2':35.E-6}

from AthenaCommon.AppMgr import ServiceMgr
from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
ServiceMgr+=BeamCondSvc(useDB=False,
                          posX=0.0,
                          posY=0.0,
                          posZ=0.0,
                          sigmaX=0.1,
                          sigmaY=0.1,
                          sigmaZ=50.0,
                          tiltX=0.0,
                          tiltY=0.0 
                          )


SimFlags.EventFilter.set_On()
SimFlags.EventFilter = { 'EtaPhiFilters': False, # 
                    'VertexPositioner': True, # This will set up the vertex smearing
                    'VertexRangeChecker': True, # This will prevent crashes
                    'PrimaryEventRotations' : False,
                    'BeamEffectTransformation': False } # This is what does the beam divergence 



from ForwardTransportSvc.ForwardTransportSvcConf import ForwardTransportSvc
ForwardTransSvc = ForwardTransportSvc()
ForwardTransSvc.TwissFile1	= "ForwardTransport/2022-01-25/alfaTwiss1.txt_beta0.4m_13TeV_-170murad_2017_ATS"    # LHC twiss file for beam1
ForwardTransSvc.TwissFile2	= "ForwardTransport/2022-01-25/alfaTwiss2.txt_beta0.4m_13TeV_-170murad_2017_ATS"    # LHC twiss file for beam2
ForwardTransSvc.PositionC1	= 149       # Z position [m] of collimator1
ForwardTransSvc.PositionC2	= 184       # Z position [m] of collimator2
ForwardTransSvc.ApertureC1	= 999       # X aperture[mm] of collimator1
ForwardTransSvc.ApertureC2	= 999       # X aperture[mm] of collimator2
ForwardTransSvc.EndMarker	= 204.888   # Z position [m] of the detector (141.580 for ZDC, 236.888 for ALFA)
ForwardTransSvc.TransportFlag   = 1
ForwardTransSvc.EtaCut          = 6.5
ForwardTransSvc.XiCut           = 0.7
ForwardTransSvc.FillRootTree    = False
ForwardTransSvc.RootFilePath    = "."
ForwardTransSvc.McCollection    = "TruthEvent"

theApp.CreateSvc += ["ForwardTransportSvc"]

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += ForwardTransSvc

