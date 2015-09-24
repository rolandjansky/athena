# The content is automatically a copy of the input
#
# Creation: David Cote, 30 November 2008
#           IdProjFilter filter by Hong Ma

# Import needed modules
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

if primDPD.ApplySkimming():
    from PrimaryDPDMaker.IdProjFilter import IdProjFilter
    idpFilter=IdProjFilter(name="IDProjFilter")
    topSequence += idpFilter

#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
streamName = primDPD.WriteRAWCommDPD_IDPROJ.StreamName
fileName   = buildFileName( primDPD.WriteRAWCommDPD_IDPROJ )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
bs=MSMgr.NewByteStream( streamName, fileName ) 

#set max output file size to 15 GB
bs.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()
bs.bsOutputSvc.MaxFileMB = primDPD.OutputRAWMaxFileSize()
bs.bsOutputSvc.MaxFileNE = primDPD.OutputRAWMaxNEvents()

if primDPD.ApplySkimming():
    bs.AddAcceptAlgs("IDProjFilter")



