#Generate CA pickle files from athena and CA
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
acc = ComponentAccumulator()
print "bootstrap_test.pkl"
with file("bootstrap_test.pkl", "w") as p:
    acc.store( p, nEvents=10, useBootStrapFile=True, threaded=False )
    p.close()

print "bootstrap_CA_test.pkl"
with file("bootstrap_CA_test.pkl", "w") as p:
    acc.store( p, nEvents=10, useBootStrapFile=False, threaded=False )
    p.close()
