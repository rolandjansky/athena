# Turn on NN tracking
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
getInDetTrigConfig("bjet")._newConfig = True
getInDetTrigConfig("bjet")._usePixelNN = True
