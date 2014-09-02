# Fragment to be used to create the DPD_beamspot file for tier-X processing

streamName = "DESDM_BEAMSPOT"
if not 'outputDPDname' in dir():
    print "Creating outputDPDname: BeamspotDPD.pool.root  (JW)"
    outputDPDname = 'BeamspotDPD.pool.root'

# Create my own stream
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

#Create a new stream
stream = MSMgr.NewStream(StreamName=streamName,FileName=outputDPDname) 

##-----------------------------------------------------------------------------
## Write all IOV meta data containers
## IOV stands for "Interval Of Validity". This means that in one file, there
## can be several blocks of events corresponding to one MetaData block.
##-----------------------------------------------------------------------------
stream.AddMetaDataItem( ["IOVMetaDataContainer#*"] )
stream.AddMetaDataItem( ["LumiBlockCollection#*"] )
 
##-----------------------------------------------------------------------------
## Write event-based containers 
##-----------------------------------------------------------------------------
stream.AddItem( ['EventInfo#*'] )

##-----------------------------------------------------------------------------
## Write Track and Vertex information
##-----------------------------------------------------------------------------

stream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
stream.AddItem( ["VxContainer#VxPrimaryCandidate*"] )

