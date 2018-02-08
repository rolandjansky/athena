#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# integrator_readDb.py
# Nils Gollub <nils.gollub@cern.ch>, 2008-02-07

import sys

#=== process command line arguments
usage = "Usage: integrator_readDb.py [\"yyyy-mm-dd hh:mm:ss\"]"
if (len(sys.argv)>2) or ("-h" in sys.argv) or ("--help" in sys.argv):
    print usage
    sys.exit(0)
sys.argv.pop(0)
pointInTime = -1
if len(sys.argv)>0:
    pointInTime = sys.argv.pop(0)
    
#=== get a logger
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("int_readDb")

#=== open the database
from TileCalibBlobPython import TileCalibTools
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'READONLY')

#=== get a blob writer
blobReader = TileCalibTools.TileBlobReader(db,"/TILE/V01/INTEGRATOR")

#=== write out the comment
comment = blobReader.getComment(pointInTime)
log.info("Comment: \"%s\"" % comment)

#=== write out all values
for ros in xrange(1,5):
    for mod in xrange(64):

        #=== get the drawer
        cd = blobReader.getDrawer(ros,mod,pointInTime)
        #=== check if data is avialable and continue if not
        if not cd: continue
        
        for pmt in xrange(48):
            for gain in xrange(6):
                outStr = "ros/mod/pmt/gain = %i/%2i/%2i/%i : " % (ros,mod,pmt,gain)
                outStr+= "gain=%f+/-%f "   % (cd.getData(pmt,gain,0),cd.getData(pmt,gain,1))
                outStr+= "ped=%f+/-%f "    % (cd.getData(pmt,gain,4),cd.getData(pmt,gain,5))
                outStr+= "rms=%f+/-%f "    % (cd.getData(pmt,gain,6),cd.getData(pmt,gain,7))
                outStr+= "dac=%3i chi2=%f" % (cd.getData(pmt,gain,3),cd.getData(pmt,gain,2))
                log.info(outStr)


#=== close the database connection
db.closeDatabase()
