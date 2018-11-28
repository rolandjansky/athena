###
# Finally, set up the ByteStreamEmonController
###

#First get the partition to which the controller shall publish
try :
  import os
  publishToPartition = os.environ['TDAQ_PARTITION']
except KeyError :
  publishToPartition = "EventDisplays"
  mlog.warning("TDAQ_PARTITION not defined in environment, using %s as default"%publishToPartition)

#Setup and configure ISProperty
from ISProperty.ISPropertyConf import ISPropertySvc
ISPropertySvc = ISPropertySvc(PartitionName=publishToPartition,ForcePublish=True)
ServiceMgr += ISPropertySvc
### NOTE: make sure ISProperty is created before BSEmonCtrlSvc,
###       otherwise the shutdown sequence will fail
theApp.CreateSvc += ['ISPropertySvc']

#Now create and configure the ByteStreamEmonController
from ByteStreamEmonController.ByteStreamEmonControllerConf import BSEmonCtrlSvc

#Create an instance with a stream-specific name
ByteStreamEmonCtrlSvc = BSEmonCtrlSvc(AppName+"-BSEmonCtrlSvc")
ServiceMgr += ByteStreamEmonCtrlSvc

#Add to list of services created right-away (otherwise not referenced)
theApp.CreateSvc += ["BSEmonCtrlSvc/"+AppName+"-BSEmonCtrlSvc"]

###
# Switch output level if requested
###
if not 'BSEmonMsgLvl' in dir():
   BSEmonMsgLvl=WARNING

ByteStreamEmonCtrlSvc.OutputLevel=BSEmonMsgLvl
ISPropertySvc.OutputLevel=BSEmonMsgLvl

if BSEmonMsgLvl <= DEBUG:
 print "\n\n\t ISPropertySvc setup:\n",ISPropertySvc,"\n\n"
 print "\n\n\t ByteStreamEmonControl setup:\n",ByteStreamEmonCtrlSvc,"\n\n"

