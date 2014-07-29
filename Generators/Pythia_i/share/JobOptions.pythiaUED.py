#########################################
# r14 example jO
# Pythia 6.4.20 UED swicthes description
#########################################
#
# Integer switches
#
#      IUED(1) =  The main UED ON(=1)/OFF(=0) switch  
#                 Default value = 0
#
#      IUED(2) =  On/Off switch for the extension to (N+4)-dimensional gravity
#                 (switching it on enables gravity-mediated LKP decay): ON(=1)/OFF(=0)  
#                 Default value = 0
#
#      IUED(3) =  The number of KK excitation quark flavors  
#                 Default value = 5
#
#      IUED(4) =  N, the number of large extra dimensions where only the graviton propagates.
#                 Only used when IUED(2)=1.  
#                 Default value = 6 (can be set to 2, 4 or 6)
# 
#      IUED(5) =  Selects whether the code takes Lambda (=0) or Lambda*R (=1) as input. 
#                 See also   {RUED(3:4)}.  
#                 Default value = 0
#
#      IUED(6) =  Selects whether the KK particle masses include radiative corrections (=1) 
#                 or are nearly degenerate m_KK ~ 1/R (=0).  
#                 Default value = 1
#
# Real switches
#
#      RUED(1) =  1/R, the curvature of the extra dimension  
#                 Default value = 1000 GeV
#
#      RUED(2) =  M_D, the (4+N)-dimensional Planck scale. Only used when IUED(2)=1.  
#                 Default value = 5000 GeV
#
#      RUED(3) =  Lambda, the cutoff scale. Used when IUED(5)=0.
#                 Default value = 20000 GeV
#
#      RUED(4) =  Lambda*R, the cutoff scale times the radius of the extra dimension.
#                 Used when IUED(5)=1.  
#                 Default value = 20

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

include ( "EvgenJobOptions/MC8_Pythia_Common.py" )

theApp.EvtMax = 10


from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
                                 "PYTHIA_INIT 820021 2347532"]

Pythia.PythiaCommand += ["pysubs msel 0",
                         "pyinit win 10000",    
                         "pypued iued 1 1",      # turn ON UED =1 
                         "pypued iued 2 1",      # turn ON =1 gravity mediated decays
                         "pypued iued 3 5",      # number of KK quark flavors = 5
                         "pypued iued 4 6",      # number or large extra dimensions = 6 for grav.med.deca.
                         "pypued iued 5 1",      # Lambda*R is fixed to the RUED(4) value
                         "pypued iued 6 1",      # KK masses radiative corrections turned ON
                         "pysubs msub 311 1",    # all
                         "pysubs msub 312 1",    # UED
                         "pysubs msub 313 1",    # production
                         "pysubs msub 314 1",    # processes
                         "pysubs msub 315 1",    # are
                         "pysubs msub 316 1",    # turned
                         "pysubs msub 317 1",    # ON
                         "pysubs msub 318 1",    #
                         "pysubs msub 319 1",    #
                         "pypued rued 1 1000",   # 1/R = 1000 GeV
                         "pypued rued 2 5000.",  # M_D = 5000 GeV for grav.med.dec.
                         "pypued rued 3 20000."] # Lambda = 20000 GeV if IUED(5)=0
