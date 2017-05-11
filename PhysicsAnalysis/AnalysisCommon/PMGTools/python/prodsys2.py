#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import  cx_Oracle,os
def ptr():
    deft  = cx_Oracle.connect("atlas_deft_r/tasks4prsys_read@adcr")
    
#    jedi  = cx_Oracle.connect("atlas_panda_reader/Waranu52@(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=adcr-adg-s.cern.ch)(PORT=10121))(ENABLE=BROKEN)(LOAD_BALANCE=on)(FAILOVER=yes)(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=adcr.cern.ch)(FAILOVER_MODE=(TYPE=SELECT)(METHOD=BASIC)(RETRIES=200)(DELAY=15))))") 
    jedi  = cx_Oracle.connect("atlas_panda_reader/Waranu52@adcr")
    
    return deft, jedi

