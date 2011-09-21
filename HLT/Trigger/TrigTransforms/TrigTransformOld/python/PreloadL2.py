# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import eformat
import time
import pickle

counter        = 0
summary        = file("pereventL2","w")
total        = []

def serializeIt(obj):
        serial        = file("pereventL2.pickle","w")
        pickle.dump(obj,serial)

def modify(event):
        global counter
        global summary
        global total
        start=time.time()
        # I can use global_id even in L2 because this is a second pass  
        mystr = "%-20d %-20d %-30f\n" % (counter,event.global_id(),start)
        total.append((counter,event.global_id(),start))
        summary.write(mystr)
        serializeIt(total)
        counter+=1
        return event
