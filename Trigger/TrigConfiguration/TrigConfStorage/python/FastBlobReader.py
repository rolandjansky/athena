# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration



import cppyy
cppyy.gbl.cool.IDatabase # force the load of the dictionary (to stay on the safe side)
from cppyy import gbl

def blob_read(self, size = -1):
    if size < 0:
        endpos = self.size()
    else:
        endpos = self.pos + size
    beginpos = self.pos
    self.pos = endpos
    buf = self.startingAddress()
    buf.SetSize(self.size())
    return buf[beginpos:endpos]

# add the new functions
getattr(gbl,"coral::Blob").read = blob_read
