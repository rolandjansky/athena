# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


#
class RpcDataType(JobProperty):
     """ Which RPC data configuration : atlas or old simulation
     """
     statusOn=True
     allowedTypes=['str']
     allowedValues=['oldSimulation','atlas']
     StoredValue='oldSimulation'

class MdtDataType(JobProperty):
     """ Which MDT data configuration : atlas or old simulation
     """
     statusOn=True
     allowedTypes=['str']
     allowedValues=['oldSimulation','atlas']
     StoredValue='oldSimulation'
 
#
class TgcDataType(JobProperty):
     """ Which TGC data configuration : atlas or old simulation
     """
     statusOn=True
     allowedTypes=['str']
     allowedValues=['oldSimulation','m3','atlas']
     StoredValue='atlas'
 
# Defines the container for the reconstruction flags  
class MuonByteStream(JobPropertyContainer):
     """ The MuonByteStream global flag/job property  container.
     """
 
# add the MuonByteStream flags container to the top container 
jobproperties.add_Container(MuonByteStream)
 
 
# I want always the following flags in the Global container  
list_jobproperties=[RpcDataType,MdtDataType,TgcDataType]
for i in list_jobproperties:
     jobproperties.MuonByteStream.add_JobProperty(i)
 
# The short name (carefull do not select same name as shot name as well. 
# otherwise problems with pickle)  
muonByteStreamFlags=jobproperties.MuonByteStream
