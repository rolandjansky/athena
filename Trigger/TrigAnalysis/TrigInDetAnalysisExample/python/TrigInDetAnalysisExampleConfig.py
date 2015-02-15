


from AthenaCommon.Constants import INFO,ERROR,FALSE,TRUE,DEBUG,VERBOSE

from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase

class TrigTestCosmic (TrigTestBase):
    __slots__ = []
    def __init__(self, name, histoPathBase):
        super( TrigTestCosmic, self ).__init__(name,histoPathBase)

        self.OutputLevel = VERBOSE
        self.SliceTag = "HLT/IDMonCosmic"
        self.AnalysisConfig = "Tier0" 
        self.ntupleChainNames = [] 

        if 'primary_idcosmic' in dir(): 
            for chain in primary_idcosmic:
                self.ntupleChainnames += chain+":InDetTrigTrackingxAODCnv_CosmicsN_EFID"
                self.ntupleChainnames += chain+":InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"                
        else: 
            self.ntupleChainNames = ["HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
                                     "HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"]

 





