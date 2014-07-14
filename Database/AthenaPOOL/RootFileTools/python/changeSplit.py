# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
trans = PyAthena.cobs('/data/ilija/AOD.067184.big.pool.root','/data/ilija/split0.pool.root')
# trans.mkProject()

trans.setSplitLevel(0)
trans.changeSplitLevel()