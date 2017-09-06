import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

######################################################
######################################################
#define filter that accepts events based on whether
#they have MET > something
######################################################
######################################################
class METFilter( PyAthena.Alg ):

    def __init__ ( self, name = "METFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(METFilter, self).__init__(**kw)

        #storegate container names
        self.metCollectionType = kw.get('metCollectionType', "MissingET")
        self.metCollectionName = kw.get('metCollectionName', "MET_RefFinal")
        self.cutEtMin          = kw.get('cutEtMin', 30.0*Units.GeV)
        ## Get the storgate handle
        self.storeGateSvc     = None

    def initialize(self):
        ## Import needed modules
        import ROOT

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure        
        return StatusCode.Success

    def execute(self):
        # Try to access container.
        try:
            met = self.storeGateSvc.retrieve( self.metCollectionType, self.metCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.metCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success   

        if met.et()>  self.cutEtMin :
            self.setFilterPassed(True)
        else:
            self.setFilterPassed(False)

        return StatusCode.Success

    def finalize(self):
        self.msg.info( '==> finalize %s...' % self.name() )
        return StatusCode.Success

######################################################
######################################################
#define filter that accepts events based on list of
#run and event numbers
######################################################
######################################################
class EventFilter( PyAthena.Alg ):

    def __init__ ( self, name = "EventFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(EventFilter, self).__init__(**kw)
        #storegate container names
        self.eventinfoCollectionType = kw.get('eventinfoCollectionType', "EventInfo")
        #need to change this for data/mc, check teh input file to find out what the collection name is: checkFile.py filename
        self.eventinfoCollectionName = kw.get('eventinfoCollectionName', "ByteStreamEventInfo") #data
        #self.eventinfoCollectionName = kw.get('eventinfoCollectionName', "MyEvent") #another possibility for data
        #self.eventinfoCollectionName = kw.get('eventinfoCollectionName', "McEventInfo") #mc
        ## Get the storgate handle
        self.storeGateSvc     = None
        #list of run,events to accept
        self.run_event_list = list()
        self.eventPass = False

    def initialize(self):
        ## Import needed modules
        import ROOT

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure        
        #while read in a line from the file
        #requires a text file with format (very primitive parsing, no comments allowed):
        #run1 evt1
        #run2 evt2
        #run3 evt3
        data = open('run_evt_numbers.txt', 'r')
        for line in data.readlines():
            run = int((line.split())[0])
            evt = int((line.split())[1])
            self.run_event_list.append((run, evt))
        return StatusCode.Success

    def execute(self):
        # Try to access container.
        try:
            evt = self.storeGateSvc.retrieve( self.eventinfoCollectionType, self.eventinfoCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.eventinfoCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success   

        try:
            id = evt.event_ID()
            run_number = id.run_number()
            event_number = id.event_number()
        except Exception:
            print 'failed to get run number and event number'

        if (run_number, event_number) in self.run_event_list:
            self.setFilterPassed(True)
        else:
            self.setFilterPassed(False)
        return StatusCode.Success

    def finalize(self):
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

###################################################################################
###################################################################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# keep all containers from the input
ExcludeList=[]

###################################################################################
#MET Filter
#comment this section out if you don't want it
###################################################################################
theJob += METFilter("MyStream_METFilter")
theJob.MyStream_METFilter.cutEtMin         = 15.0*Units.GeV

streamName = "METStream"
fileName   = "/afs/cern.ch/user/a/ayurkewi/scratch0/METperf_1540/Reconstruction/MissingETPerformance/run/output/METSkim.root"
METStreamObject=MSMgr.NewPoolStream( streamName, fileName )
METStreamObject.AcceptAlgs( [ "MyStream_METFilter"] )

dpdOutput.addBasicOutput(streamName)
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)

###################################################################################
#Run - Event Filter
#comment this section out if you don't want it
###################################################################################
theJob += EventFilter("MyStream_EventFilter")
theJob.MyStream_EventFilter.OutputLevel = 1

streamName2 = "EventStream"
fileName2   = "/afs/cern.ch/user/a/ayurkewi/scratch0/METperf_1540/Reconstruction/MissingETPerformance/run/output/RunEventSkim.root"
EventStreamObject=MSMgr.NewPoolStream( streamName2, fileName2 )
EventStreamObject.AcceptAlgs( [ "MyStream_EventFilter"] )

dpdOutput.addBasicOutput(streamName2)
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName2,ExcludeList)
###################################################################################
