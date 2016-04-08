include.block("PixelConditionsServices/PixelDCSSvc_jobOptions.py")

from PixelConditionsServices.PixelConditionsServicesConf import PixelDCSSvc

##### set folders for Dec06 cosmics run data ########

## ServiceMgr += PixelDCSSvc(RegisterCallback = TRUE,
##                           TemperatureFolder = "/PIXEL/DCS/DEC06/TEMPERATURE",
##                           HVFolder = "/PIXEL/DCS/DEC06/HV",
##                           FSMStatusFolder = "/PIXEL/DCS/DEC06/FSMSTATUS",
##                           FSMStateFolder = "/PIXEL/DCS/DEC06/FSMSTATE"
##                           )


##### set folders for default DCS db ########

ServiceMgr += PixelDCSSvc(RegisterCallback = TRUE,
                          TemperatureFolder = "/PIXEL/DCS/TEMPERATURE",
                          HVFolder = "/PIXEL/DCS/HV",
                          FSMStatusFolder = "/PIXEL/DCS/FSMSTATUS",
                          FSMStateFolder = "/PIXEL/DCS/FSMSTATE",
                          TemperatureFieldName = "temperature",
                          HVFieldName = "HV",
                          FSMStatusFieldName = "FSM_status",
                          FSMStateFieldName = "FSM_state"
                          )

from IOVDbSvc.CondDB import conddb


#### use Dec06 cosmics db ########
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/TEMPERATURE ")
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/HV ")
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/FSMSTATUS ")
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/DEC06/FSMSTATE ")

##### use default db ########
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=OFLP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/TEMPERATURE <tag>PixDCSTemperature-00-00-00</tag>")
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=OFLP200;user=ATLAS_COOL_READER</dbConnection>/PIXEL/DCS/HV <tag>PixDCSHV-00-00-00</tag>")
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=OFLP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/FSMSTATUS <tag>PixDCSFSMStatus-00-00-00</tag>")
## conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=OFLP200;user=ATLAS_COOL_READER</dbConnection> /PIXEL/DCS/FSMSTATE <tag>PixDCSFSMState-00-00-00</tag>")

## tag = '<tag>PixDCS-00-00-00</tag>'

conddb.addFolder("DCS_OFL","/PIXEL/DCS/TEMPERATURE")
conddb.addFolder("DCS_OFL","/PIXEL/DCS/HV")
conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATUS")
conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATE")


# Module status information from FSM is not reliable at the moment
# So we relaxe the FSM requirements: Accept state = ON and status = WARNING
#

# temporarily workaround incomplete conditions data for MC
#  by only enabling the usage of dcs in the pixel conditions summary service for data
isData = (globalflags.DataSource == 'data')
if hasattr(ServiceMgr,'PixelConditionsSummarySvc'):
    ServiceMgr.PixelConditionsSummarySvc.UseDCS =  isData
    ServiceMgr.PixelConditionsSummarySvc.IsActiveStates = [ 'READY', 'ON' ]
    ServiceMgr.PixelConditionsSummarySvc.IsActiveStatus = [ 'OK', 'WARNING' ]
else :
    from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
    ServiceMgr += PixelConditionsSummarySvc( UseDCS = isData )
    ServiceMgr.PixelConditionsSummarySvc.IsActiveStates = [ 'READY', 'ON' ]
    ServiceMgr.PixelConditionsSummarySvc.IsActiveStatus = [ 'OK', 'WARNING' ]
