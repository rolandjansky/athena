from TBDetDescrAlg.TBDetDescrAlgConf import *
theTBDetDescrLoader=TBDetDescrLoader()

theTBDetDescrLoader.TBElementContainer = "TBElementCnt"
theTBDetDescrLoader.TBDetDescrManager  = "TBDetDescrMgr"

from AthenaCommon.DetFlags import DetFlags

if not 'doSim' in dir():
    # apparently we are not running RecExTB,
    # most probably running simulation or digitization

    Stream1.ItemList+=["TBElementContainer#*"]

    if DetFlags.writeRDOPool.any_on():
        # looks like this is digitization job
        # just copy container from input file to output file

        theTBDetDescrLoader.ReadAction = 1
        theTBDetDescrLoader.WriteAction = 0

    else:
        # looks like this is simulation job
        # need to initialize TBDetDesrManager in Detector Store from FADS
        include( "TBDetDescrCnv/TBDetDescrCnv_jobOptions.py" )

        # do not read anything from StoreGate
        theTBDetDescrLoader.ReadAction = 0
        # and write TBDetDescrContainer to StoreGate every event
        theTBDetDescrLoader.WriteAction = 2

else:
    # doSim can be set to False by TileCal jobOptions
    # make independent cross-check

    if DetFlags.writeRDOPool.any_on():
        # looks like this is digitization job
        # just copy container from input file to output file

        Stream1.ItemList+=["TBElementContainer#*"]
        theTBDetDescrLoader.ReadAction = 1
        theTBDetDescrLoader.WriteAction = 0

    else:
        # reconstruction job
        # reading ByteStream or POOL file with digits
    
        if doSim:
            # data from simulation, read TBDetDescr just once
            theTBDetDescrLoader.ReadAction = 1
            theTBDetDescrLoader.WriteAction = 0
        else:
            # real data, create dummy TBDetDescrManager
            theTBDetDescrLoader.ReadAction = -1
            theTBDetDescrLoader.WriteAction = 0


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += theTBDetDescrLoader
