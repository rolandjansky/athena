#For the moment, this requires:
# cmt co -r AthenaPython-00-03-27 Control/AthenaPython

from AthenaCommon.AlgSequence import AlgSequence
from EventBookkeeperTools.FilterTester import FilterTester
topSequence=AlgSequence()
topSequence+= FilterTester('myFilter')

theApp.EvtMax=10
