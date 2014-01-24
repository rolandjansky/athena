# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def readConfigFromDB(dbconnection, masterkey, runtype, reco_step, *vargs, **kwargs):
  if dbconnection=='sqlite':
      import sqlite3
      import sys
      conn = sqlite3.connect('test.db')
      c = conn.cursor()
      c.execute("SELECT * FROM master_table, mt2type, type, type2sig, sig_config WHERE type.name=? and type2sig.sig_config_id=sig_config.id and type2sig.type_id=type.id and mt2type.master_id=? and mt2type.type_id=type.id and sig_config.recostep=?", (runtype, masterkey, reco_step))

      montool_config = {}

      for x in c.fetchall():
         print "Printing...", x
         montoolconfigCode=x[12]
         exec montoolconfigCode

      print "All configuration info now is in this dictionary:\n",montool_config
      return montool_config  
      conn.close()
