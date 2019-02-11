# JetRecConfig/share

## JobOptions contents

### JetRecTestCfg.py

This module encodes an example/test job for jet reconstruction,
which builds a small number of standard jet collections and writes
them to an output xAOD file.

Call `python JetRecTestCfg.py -H` for help.

### test_JetDefinitions_properties.py

This script tests whether jet definition properties correctly
update all necessary information when they are altered by
a user, for example when using a deepcopy-update pattern to
construct new JetConstit or JetDefinition instances.

Should be moved to a test/ dir and turned into a unit test.
