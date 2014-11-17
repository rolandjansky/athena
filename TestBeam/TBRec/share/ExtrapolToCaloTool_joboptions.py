if not 'TTC_ExtrapolatorInstance' in dir():
    TTC_ExtrapolatorInstance = 'TTC_Extrapolator'
# from AW 
from TrkConfigurableWrapper.ConfigurableWrapper import ConfigurableWrapper


from TrkExTools.AtlasExtrapolator import AtlasExtrapolator

theAtlasExtrapolator=AtlasExtrapolator()
ToolSvc+=theAtlasExtrapolator
TTC_Extrapolator = ConfigurableWrapper(theAtlasExtrapolator)
