include.block("TriggerRelease/TriggerConfig_TrigL2BMuMuXHypo.py")
#
# Configure a suitable _TrigL2BMuMuXHypo Algorithm instance
#
# Constructor arguments:
# level, type
# 
# e.g. level=L2, type=B0
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a list of strings to be entered in
# the sequence file. This string defines the class and instance name

class TriggerConfig_TrigL2BMuMuXHypo:
    
    def __init__(self, level, type = "B0"):
        
        if type == "B0":
            self.__instname__ = "TrigL2BMuMuXHypo_B0_"
            self.__sequence__ = "TrigL2BMuMuXHypo/TrigL2BMuMuXHypo/B0"
            self.__instname__ += level
            Algorithm(self.__instname__).AcceptAll = False
            Algorithm(self.__instname__).FEXntuple = ((not CBNTAthenaAware) and doCBNT)
            Algorithm(self.__instname__).oppositeSign = True
            Algorithm(self.__instname__).LowerBMassCut = 3000
            Algorithm(self.__instname__).UpperBMassCut = 7000
            Algorithm(self.__instname__).LowerXMassCut = 800
            Algorithm(self.__instname__).UpperXMassCut = 1000
            Algorithm(self.__instname__).Bmass = 5279.4            # mass of B-meson candidate (B0d)
            Algorithm(self.__instname__).Xmass = 896.0             # mass of X-hadron candidate (K* or phi for example)
            Algorithm(self.__instname__).X1DaugtherMass = 493.68   # mass of the first  daughter of hadronic decay X -> X1 X2 (e.g. K+)
            Algorithm(self.__instname__).X2DaugtherMass = 139.57   # mass of the second daughter of hadronic decay X -> X1 X2 (e.g. Pi-)
            Algorithm(self.__instname__).VertexingAlgo = 0         # 0 = TrigVertex, 1 = VKalVrt
            Algorithm(self.__instname__).PerformXVertexing = True  # Should we try to reconstruct X in (B -> Mu Mu X) using 2 ID tracks

    def instanceName(self):
        return self.__instname__
            
    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigBphysHypo/jobOfragment_TrigL2BMuMuXHypo.py" )
include.block( "TrigBphysHypo/jobOfragment_TrigL2BMuMuXHypo.py" )

