include.block ("RegistrationServices/RegistrationStreamTool_jobOptions.py")

########### Registration Stream Tool Options ################

from RegistrationServices.RegistrationServicesConf import RegistrationStreamTool as ConfiguredRegistrationStreamTool
RegistrationStreamTool = ConfiguredRegistrationStreamTool(
   CollectionType="ExplicitROOT",
   Connection="",
   OutputCollection="",
   CollectionOpenMode="CREATE_AND_OVERWRITE",
   PersistencySvc="EventPersistencySvc",
   StoreName="StoreGateSvc"
)
ToolSvc += RegistrationStreamTool
print      RegistrationStreamTool


