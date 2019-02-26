## @file SelectEvents_jobOptions.py
#
# @brief Select a list of event numbers given in (global) python variable EventList
#
# EventList can either be a python list:
# EventList = [ 3, 6, 26, 640 ]
# or a filename:
# EventList = "myeventlist.txt"
# which should be an ascii file with one event number per line
#
#
# @author Martin Woudstra


from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence import AthSequencer
masterSequence = AthSequencer('AthMasterSeq')
from GaudiSequencer.PyComps import PyEvtFilter
masterSequence += PyEvtFilter ('EventNumberFilterAlg')

def eventNumber_filter_fct(fname):
    """generate a filter fonction from a file containing lines
    with one event number per line
    TODO: more protections against ill formated file's contents
    """
    evt_db = []
    evtFile = open(fname, 'r')
    for line in evtFile:
      evt_db.append(int(line))
    evtFile.close()
    print 'Read',len(evt_db),'event numbers from file',fname
    return evt_db

##     def file_based_event_filter_fct(runNo,evtNo):
##       result = evtnbr in evt_db
##       print 'file_based_event_filter_fct(%r,%r) => %r',runNo,evtNo,result
##       return result

##     return file_based_event_filter_fct


eventFilterAlg = masterSequence.EventNumberFilterAlg
if type(EventList) in (list,tuple):
    eventFilterAlg.evt_list = EventList
elif type(EventList) == str:
    # filename with list of event numbers (one per line)
    eventFilterAlg.evt_list = eventNumber_filter_fct(EventList)
else:
    raise RuntimeError("Can not handle EventList variable of type %r" % type(EventList) )
eventFilterAlg.filter_policy = 'accept'
if globalflags.InputFormat == "bytestream":
    eventFilterAlg.evt_info = 'ByteStreamEventInfo'
else: # POOL
    eventFilterAlg.evt_info = '' # take whatever is there
