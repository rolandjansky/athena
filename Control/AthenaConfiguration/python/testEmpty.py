# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#Generate CA pickle files from athena and CA
from __future__ import print_function
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
acc = ComponentAccumulator()
print("bootstrap_test.pkl")
with open("bootstrap_test.pkl", "wb") as p:
    acc.store( p, nEvents=10, useBootStrapFile=True, threaded=False )
    p.close()

print("bootstrap_CA_test.pkl")
with open("bootstrap_CA_test.pkl", "wb") as p:
    acc.store( p, nEvents=10, useBootStrapFile=False, threaded=False )
    p.close()
