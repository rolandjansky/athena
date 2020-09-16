#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import gc

from collections import namedtuple
from pprint import pprint

from DQUtils.db import Databases, fetch_objects
from quick_retrieve import browse_coracool

from time import time

from os import environ
os.environ['CLING_STANDARD_PCH'] = 'none' #See bug ROOT-10789
from PyCool import cool

def fetch_lb_timestamps(since, until):
    """
    Read the timestamps of all luminosity blocks between 
    the run-lumi IoV (since, until)
    """
    timestamps = fetch_objects("LBLB", since, until, 
                               with_channel=False, what="all")
    return timestamps[0].StartTime, timestamps[-1].EndTime, timestamps

def test_coracool():
    
    folder_path = "/SCT/DAQ/Configuration/Module"
    
    database = Databases.get_instance("COOLONL_SCT/COMP200")
    folder = database.getFolder(folder_path)
    
    #print "Total:", folder.countObjects(0, 2**63-1, cool.ChannelSelection())
    
    if not "<coracool>" in folder.description():
        print "%s is not a coracool folder" % folder.fullPath()
        return
    
    #x = fetch_objects("LBLB", (142391,0), (142391,0xFFFF), what=[], with_channel=False)
    #print "Expected count:", len(x)
    
    # Run 142391 timestamps
    #since, until = 1260878894897166135, 1260879303912307635
    run = 142391
    #run = 90415
    
    since, until, dummy = fetch_lb_timestamps((run,0), (run,2**32-1))
    
    #since, until = 0, 2**63-1
    since, until = 1221973544491128285, 1221995264779751560
    
    variables = ["group", "id"]
    
    record = namedtuple("coracool_record", "since until channel elements")
    element = namedtuple("element", " ".join(variables))

    start = time()

    objects = browse_coracool(database, folder_path, since, until, 
        cool.ChannelSelection(), "", variables, record, element)
    
    elapsed = time()-start
    print "Took %.2f to browse_coracool" % elapsed  
      
    print len(objects)
    #pprint(objects[:10])
    
    bad = [x for x in objects if any(el.group == -1 for el in x.elements)]
    print len(bad)
    
    #pprint(bad)
    
    ip()
    
    return
    
def test_refcounting():    
    record = namedtuple("coracool_record", "since until channel elements")
    element = namedtuple("element", "group id")
    
    from time import time
    start = time()
    
    
    objects = None

    before_objects = map(id, gc.get_objects())
    gc.collect()
    
    try: raise RuntimeError
    except: pass
    
    print "Objects alive before call:", len(gc.get_objects())
    
    try:
        #raise RuntimeError
        objects = browse_coracool(database, folder_path, since, until, 
                                  cool.ChannelSelection(), "", ["group", "id"],
                                  record, element)

    except Exception, e:
        del e
    
    elapsed = time() - start
    print "Took %.3f to browse folder" % elapsed
    
    if objects:
        print type(objects), type(objects[0])
        
        print "Result =", len(objects)
        print type(objects)
        
        print len(objects[0])
        
        print objects[0]
        del objects

    #pprint(objects[:5])

    #x = map(lambda x: len(x[-1]), objects)
    #print "0:", x.count(0)
    #print "1:", x.count(1)
    
    print "Objects alive before cleanup:", len(gc.get_objects())    
     
    gc.collect()
    
    print "Objects alive after:", len(gc.get_objects())
    
    after_objects = gc.get_objects()
    
    new_objects = set(map(id, after_objects)) - set(before_objects)
    
    new_objects = filter(lambda x: id(x) in new_objects and x != before_objects, after_objects)
    
    print "New objects:", len(new_objects)
    #print map(type, new_objects)
    #print [len(x) for x in new_objects if hasattr(x, "__len__")]
    
    #from pprint import pprint
    #pprint(new_objects)
    
    
if __name__ == "__main__":
    
    
    from IPython.Shell import IPShellEmbed
    ip = IPShellEmbed(["-pdb"], rc_override=dict(quiet=True))
    
    test_coracool()
