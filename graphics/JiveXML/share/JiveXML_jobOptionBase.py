

### Create a new algorithm 
from JiveXML.JiveXMLConf import JiveXML__AlgoJiveXML
theEventData2XML = JiveXML__AlgoJiveXML(
    name = "AlgoJiveXML",

)

### Set the atlas release version
### Change of variable name in SLC5, patch from
### by Luca Fiorini. jpt 30Jan10
import os
try:
   theEventData2XML.AtlasRelease = os.environ["AtlasVersion"]
except Exception:
   treatException("Could not set AthenaVersion")
   theEventData2XML.AtlasRelease = os.environ["AtlasBaseVersion"]

### Write the event data to disk
# WARNING: this option is only provided for backward compatibility and will
#          be removed in the future
theEventData2XML.WriteToFile = True

### Online mode allows you to send events to Atlantises over a network connection
# WARNING: this option is only provided for backward compatibility and will
#          be removed in the future
theEventData2XML.OnlineMode = False

### Enable this to recreate the geometry XML files for Atlantis
theEventData2XML.WriteGeometry = False

### import and create top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

### add the AlgoJiveXML
topSequence += theEventData2XML

