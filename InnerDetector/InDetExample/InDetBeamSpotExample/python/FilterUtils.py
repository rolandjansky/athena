# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id $
"""
This library defines different classes for filtering events in athena.
Intended for use in JobRunner templates.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'


# General setup
from AthenaCommon.AlgSequence import AthSequencer
# Leave it named 'filterSeq' for now to not
# break JOs importing this file. Should be changed
# to masterSeq to avoid confusion...
filterSeq = AthSequencer("AthMasterSeq")
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena


#
# Event filtering based on BCID
#
class BCIDFilter(PyAthena.Alg):
    def __init__(self,bcidList,name='BCIDFilter'):
        super(BCIDFilter,self).__init__(name=name)
        self.bcidList = bcidList
        return

    def initialize(self):
        print 'BCIDFilter:  Found %i BCIDs to accept:  ' % len(self.bcidList), self.bcidList
        if not self.bcidList:
            print 'BCIDFilter: WARNING: Empty bcidList - will accept all events'
        self.sg = PyAthena.py_svc('StoreGateSvc')
        return StatusCode.Success
    
    def execute(self):
        if self.bcidList:
            eventID = self.sg.retrieve('EventInfo','ByteStreamEventInfo').event_ID()
            bcid = eventID.bunch_crossing_id()
            #print bcid, bcid in self.bcidList
            self.setFilterPassed(bcid in self.bcidList)
        else:
            self.setFilterPassed(True)
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success


#
# Event filtering based on LB number
#
class LBFilter(PyAthena.Alg):
    def __init__(self,lbList,name='LBFilter'):
        super(LBFilter,self).__init__(name=name)
        self.lbList = lbList
        return

    def initialize(self):
        print 'LBFilter:  Found %i LBs to accept:  ' % len(self.lbList), self.lbList
        if not self.lbList:
            print 'LBFilter: WARNING: Empty lbList - will accept all events'
        self.sg = PyAthena.py_svc('StoreGateSvc')
        return StatusCode.Success
    
    def execute(self):
        if self.lbList:
            eventID = self.sg.retrieve('EventInfo','ByteStreamEventInfo').event_ID()
            lb = eventID.lumi_block()
            #print lb, lb in self.lbList
            self.setFilterPassed(lb in self.lbList)
        else:
            self.setFilterPassed(True)
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success


#
# LumiScan filter for filter and setting pseudo-LB numbers
#
from time import asctime,gmtime

class LbInfo:
    def __init__(self,line):
        self.line = line.strip()
        l = line.split()
        self.lbNumber = int(l[0]) 
        #self.point = int(l[0])
        self.startTime = int(l[1])
        self.endTime = int(l[2])
        self.separation = float(l[3])

    def __repr__(self):
        #return self.line
        s = 'Point %2d:  %s - %s UTC (deltaT = %5.2f s), nom.sep. = %f' % (self.lbNumber,asctime(gmtime(self.startTime/1e9)),asctime(gmtime(self.endTime/1e9)),(self.endTime-self.startTime)/1.e9,self.separation)
        return s

    def match(self,sec,nsec):
        # Time now in nsec
        t = sec*1e9 + nsec
        #t = sec*1e3 + nsec/1e6
        return (t>=self.startTime) and (t<self.endTime)

    def isScanPoint(self):
        #return (self.endTime-self.startTime > 25e9)
        return True

class LumiBlockFilter(PyAthena.Alg):
    def __init__(self,lbData,name='LumiBlockFilter'):
        super(LumiBlockFilter,self).__init__(name=name)
        self.lbList = []        

        # Treat lbData as a file containing the pLB info if it is a string or a direct list of pLB info if it is a list
        lbInfo = open(lbData) if isinstance(lbData,basestring) else lbData

        for l in lbInfo:
            if l[0]=='#': continue
            self.lbList.append(LbInfo(l))

        if type(lbInfo)==file: lbInfo.close()

        print
        print 'Found',len(self.lbList),'(pseudo)LB entries:'
        for i in range(len(self.lbList)):
            print 'Data for new LB %3i:  ' % self.lbList[i].lbNumber, self.lbList[i]
        print
        return

    def initialize(self):
        self.sg = PyAthena.py_svc('StoreGateSvc')
        return StatusCode.Success
    
    def execute(self):
        eventID = self.sg.retrieve('EventInfo','ByteStreamEventInfo').event_ID()
        foundEvent = False
        for lb in self.lbList:
            if lb.match(eventID.time_stamp(),eventID.time_stamp_ns_offset()) and lb.isScanPoint():
                #print 'Match found with LB %3i (data: %s)' % (lb.lbNumber,lb)
                eventID.set_lumi_block(lb.lbNumber)
                foundEvent = True
                break
        self.setFilterPassed(foundEvent)
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success


#
# Event filtering based on z of primary vertex position
#
class ZFilter(PyAthena.Alg):
    def __init__(self,zRange,name='ZFilter'):
        super(ZFilter,self).__init__(name=name)
        self.zRange = zRange
        self.zMin = zRange[0]
        self.zMax = zRange[1]
        return

    def initialize(self):
        print 'ZFilter: accepting only primary vertices from',self.zMin,'to',self.zMax
        self.sg = PyAthena.py_svc('StoreGateSvc')
        return StatusCode.Success
    
    def execute(self):
        vxContainer = self.sg.retrieve("VxContainer","VxPrimaryCandidate")
        #print 'vxContainer size =',vxContainer.size()
        accept = False
        for i in range(vxContainer.size()):
            if vxContainer[i].vertexType() != 1: continue   # Only use primary vertex candidates
            z = vxContainer[i].recVertex().position().z()
            accept = (z >= self.zMin) and (z <= self.zMax)
            #print z, accept
            break
        self.setFilterPassed(accept)
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success
