# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
trans = PyAthena.cobs('/data/ilija/AOD.067184.big.pool.root','/data/ilija/tmp.pool.root')
# trans.mkProject()

# resizing BS and adding CL=7
trans.setTreeToSkip('##Links')
trans.setTreeToSkip('##Shapes')
trans.setTreeToSkip('##Params')
trans.setTreeMemory(10*1024,'POOLContainer_DataHeader')
trans.resizeBaskets() 

trans1 =  PyAthena.cobs('/data/ilija/tmp.pool.root','/data/ilija/oBS_orderedByEvent.pool.root')
trans1.order(2)
