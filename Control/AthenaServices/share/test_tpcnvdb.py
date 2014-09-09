# simple jobo to test the loading of t/p cnvs.

theApp.EvtMax = 1
theApp.run(1)

import AthenaServices.TPCnvDb as tpcnvdb
sc = tpcnvdb.test_load_alltpcnv()

theApp.exit(sc)
