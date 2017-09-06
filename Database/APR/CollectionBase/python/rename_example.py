# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

colsvc = ROOT.pool.CollectionService()
read_only_flag = False
col=colsvc.handle('SimplePoolCollection1','RootCollection', '', read_only_flag)
col.schemaEditor().renameColumn('EventNumber', 'OldEvNumb')
col.commit()
col.close()
