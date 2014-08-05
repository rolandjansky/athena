
## ************************************************************
## $Id: CondDB_jobOptions.py,v 1.1 2008-09-23 13:29:11 mgwilson Exp $
## ************************************************************

from IOVDbSvc.CondDB import conddb

## Allow retrieval of the length of luminosity blocks
conddb.addFolder('TRIGGER','/TRIGGER/LUMI/LBLB')

