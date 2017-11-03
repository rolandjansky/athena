#!/usr/bin/env python

from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

def main(outfolder):
    with open(outfolder+"/list_unprescaled1p8e34.py","w") as outfile:
        #outfile.write(header())
        for triggerType in TriggerType:
            outfile.write( to_nice_list( TriggerAPI.getUnprescaled(TriggerPeriod.future1p8e34, triggerType), triggerType.name) )
        #outfile.write(end())

    with open(outfolder+"/list_unprescaled2e34.py","w") as outfile:
        #outfile.write(header())
        for triggerType in TriggerType:
            outfile.write( to_nice_list( TriggerAPI.getUnprescaled(TriggerPeriod.future2e34, triggerType), triggerType.name ) )
        #outfile.write(end())

def to_nice_list( alist, name ):
    thestr = "list_%s = [\n"%name
    for i in alist:
        thestr += "\t'%s',\n"%i
    thestr += "]\n\n"
    return thestr

if __name__ == "__main__":
    import sys
    sys.exit( main(sys.argv[1]) )
