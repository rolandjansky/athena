from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimParamAlg
topSeq += FastCaloSimParamAlg()
ISF_HITSStream.stream1.ItemList += ["ISF_FCS_Parametrization::FCS_StepInfoCollection#MergedEventSteps"]
