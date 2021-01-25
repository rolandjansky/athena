#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import gc

from collections import namedtuple
from pprint import pprint

from DQUtils.db import Databases, fetch_iovs
from DQUtils.quick_retrieve import browse_coracool

from time import time

import os
os.environ['CLING_STANDARD_PCH'] = 'none' #See bug ROOT-10789
from PyCool import cool

folder_path = "/SCT/DAQ/Configuration/Module"

database = Databases.get_instance("COOLONL_SCT/COMP200")
folder = database.getFolder(folder_path)
since, until = 1221973544491128285, 1221995264779751560


def fetch_lb_timestamps(since, until):
    """
    Read the timestamps of all luminosity blocks between 
    the run-lumi IoV (since, until)
    """
    timestamps = fetch_iovs("COOLONL_TRIGGER/COMP200::/TRIGGER/LUMI/LBLB", since, until, 
                               with_channel=False, what="all")
    return timestamps[0].StartTime, timestamps[-1].EndTime, timestamps

def test_coracool():
    
    if not "<coracool>" in folder.description():
        print(f"{folder.fullPath()} is not a coracool folder")
        return
    
    variables = ["group", "id"]
    
    record = namedtuple("coracool_record", "since until channel elements")
    element = namedtuple("element", " ".join(variables))

    start = time()

    objects = browse_coracool(database, folder_path, since, until, 
        cool.ChannelSelection(), "", variables, record, element)
    
    elapsed = time()-start
    print("Took %.2f to browse_coracool" % elapsed)
      
    print(len(objects))
    
    bad = [x for x in objects if any(el.group == -1 for el in x.elements)]
    print(len(bad))
    
    return
    
def test_refcounting():    
    record = namedtuple("coracool_record", "since until channel elements")
    element = namedtuple("element", "group id")
    
    from time import time
    start = time()
    
    
    objects = None

    before_objects = set(id(x) for x in gc.get_objects())
    gc.collect()
    
    try: raise RuntimeError
    except: pass
    
    print("Objects alive before call:", len(gc.get_objects()))
    
    try:
        objects = browse_coracool(database, folder_path, since, until, 
                                  cool.ChannelSelection(), "", ["group", "id"],
                                  record, element)

    except Exception as e:
        print("Caught exception ", e)
        del e
    
    elapsed = time() - start
    print("Took %.3f to browse folder" % elapsed)
    
    assert objects is not None, 'Null return from browse_coracool'

    print("Return type:", type(objects), type(objects[0]))
    print("Result length =", len(objects))
    print("Result index 0 length =", len(objects[0]))
    print("Result index 0 =", objects[0])
    del objects

    
    print("Objects alive before cleanup:", len(gc.get_objects()))
     
    gc.collect()
    
    print("Objects alive after:", len(gc.get_objects()))
    
    after_objects = gc.get_objects()
    
    new_objects = set(id(x) for x in after_objects) - before_objects
    
    # new_objects = filter(lambda x: id(x) in new_objects and x != before_objects, after_objects)
    new_objects = [x for x in after_objects if id(x) in new_objects and x != before_objects and x != new_objects]
    
    assert new_objects == [], 'Lingering objects after garbage collection'
    
    
if __name__ == "__main__":
    
    test_coracool()
    test_refcounting()
