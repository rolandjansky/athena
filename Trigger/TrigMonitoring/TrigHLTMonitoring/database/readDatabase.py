# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def readDatabase(dbconnection, masterkey, runtype, slice_name, reco_step, *vargs, **kwargs):
  if dbconnection=='sqlite':
      import sqlite3
      import sys
      conn = sqlite3.connect('test.db')
      c = conn.cursor()

      c.execute("SELECT * FROM master_table, mt2type, type, type2sig, sig_config WHERE type.name=? and type2sig.sig_config_id=sig_config.id and type2sig.type_id=type.id and mt2type.master_id=? and mt2type.type_id=type.id and sig_config.name=? and sig_config.recostep=?", (runtype, masterkey, slice_name, reco_step))

      montool_config = {}
      montoolconfigId=[]
      montoolconfigCode=[]
      montoolconfigScript=[]

      for x in c.fetchall():
         #print "Printing...", x
         montoolconfigId.append(x[9])
         montoolconfigCode.append(x[12])
         montoolconfigScript.append(x[13])


      #print montoolconfigId
      maxID=max(montoolconfigId)
      #print maxID
      exec montoolconfigCode[montoolconfigId.index(maxID)]

      #print "All configuration info now is in this dictionary:\n",montool_config

      # return the configuration for the last entry (maximal ID) of a certain slice in the database
      # return maxID,monitoring triggers,montool_config,package_name, creator_name, montoolconfigScript
      return maxID,montool_config[slice_name]['MonitoringTriggers'],montool_config[slice_name]['Config'],montool_config[slice_name]['PkgName'],montool_config[slice_name]['CreatorName'],montoolconfigScript[montoolconfigId.index(maxID)]
      conn.close()
