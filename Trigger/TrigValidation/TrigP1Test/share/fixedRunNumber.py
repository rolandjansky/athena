from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

def modify(part):
   from config.dal import module as dal_module
   dal = dal_module('dal','daq/schema/core.schema.xml')
   var = dal.Variable('TDAQ_RUN_NUMBER_CONNECT')
   var.Name = 'TDAQ_RUN_NUMBER_CONNECT'
   var.Value = 'FIXED_NUMBER:${PWD}/runnumber.txt'

   part.ProcessEnvironment += [var]
   return part

