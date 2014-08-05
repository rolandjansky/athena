print "WARNING obsolete joboption. Please TestBeam/TBRec/python/ExtrapolTrackToCalo configurables instead "

if not 'TTC_ExtrapolatorInstance' in dir():
    TTC_ExtrapolatorInstance = 'TTC_Extrapolator'
# from AW 
from TrkConfigurableWrapper.ConfigurableWrapper import ConfigurableWrapper


from TrkExTools.AtlasExtrapolator import AtlasExtrapolator

theAtlasExtrapolator=AtlasExtrapolator()
ToolSvc+=theAtlasExtrapolator
TTC_Extrapolator = ConfigurableWrapper(theAtlasExtrapolator)
# default name Trk::Extrapolator/AtlasExtrapolor used by default ( to be changed)

