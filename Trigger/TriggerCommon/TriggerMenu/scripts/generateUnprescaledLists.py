#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.00 $"
__doc__="Script generating lists of future unprescaled triggers when the package is built"

from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

def main(outfolder):
    with open(outfolder+"/list_unprescaled1p8e34.py","w") as outfile:
        outfile.write(header())
        for triggerType in TriggerType:
            outfile.write( to_nice_list( TriggerAPI.getUnprescaled(TriggerPeriod.future1p8e34, triggerType), triggerType.name) )

    with open(outfolder+"/list_unprescaled2e34.py","w") as outfile:
        outfile.write(header())
        for triggerType in TriggerType:
            outfile.write( to_nice_list( TriggerAPI.getUnprescaled(TriggerPeriod.future2e34, triggerType), triggerType.name ) )

def to_nice_list( alist, name ):
    thestr = "list_%s = [\n"%name
    for i in alist:
        thestr += "\t'%s',\n"%i
    thestr += "]\n\n"
    return thestr

def header():
    thestr  = '# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration\n'
    thestr += '# File was auto-generated with generateUnprescaledLists.py\n\n'
    return thestr

if __name__ == "__main__":
    import sys
    sys.exit( main(sys.argv[1]) )
