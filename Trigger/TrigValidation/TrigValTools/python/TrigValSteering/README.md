# TrigValSteering
##### Python package configuring and steering Trigger ART tests


### Basic concepts
Each Trigger ART test is represented by an instance of the class `Test`. It consists of several steps in two categories
stored in two member variables `exec_steps` and `check_steps`. The exec steps constitute the main part of the
test and correspond to one athena job each. They can also execute athenaHLT or a transform. The check steps are
additional post-processing steps analysing and verifying the logs and outputs from the exec steps. All steps share
a common base class `Step`. 


### Modules
**Common** -- Common variables and functions used by other modules  
**Test** -- The `Test` class describing a Trigger ART test to be configured and executed  
**Step**  -- Base class `Step` used by ExecSteps and CheckSteps  
**ExecStep**  -- The `ExecStep` class representing an athena job or transform  
**CheckSteps**  -- A collection of post-processing step definitions. Provides also the `default_check_steps` function  
**Input**  -- Interface between python steering and JSON file defining common samples (TrigValTools/share/TrigValInputs.json)  


### How to implement a Trigger ART test
1. Create a python script `test_<prefix>_<name>.py` in the `test` directory of a package. The name has to follow the Trigger ART naming convention and include package-specific prefix (e.g. trigAna for TrigAnalysisTest).
1. Start with `#!/usr/bin/env python` to make sure ART will execute it with the default python interpreter.
1. Include all necessary art declarations, minimally art-description, art-type and art-include.
1. Import the required modules from **TrigValTools.TrigValSteering**
1. Define the exec step(s)
1. Optionally define the check steps, although typically `CheckSteps.default_check_steps(test)` can be used. After
adding the default check steps, the configuration of a specific step can still be modified. To get a handle to a
specific test, you  can use the `Test.get_step(step_name)` method.
1. Define the `Test` object and assign the exec and check steps.
1. Finish with `sys.exit(test.run())` to make sure the exit code is propagated.


### Example
Let's say the below file is `TriggerTest/test/test_trig_example.py`
```python
#!/usr/bin/env python

# art-description: example trigger test
# art-type: build
# art-include: master/Athena

# Import modules needed to build a test configuration
from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

# Define the main exec step
ex = ExecStep.ExecStep()
ex.type = 'athena' # can be also athenaHLT, Reco_tf, Trig_reco_tf or 'other'
ex.job_options = 'PackageName/myJobOptionsFile.py'
ex.input = 'ttbar' # select input from common samples by keyword; explicit input is also possible

# Define the test itself
test = Test.Test()
test.art_type = 'build' # should match the art-type declaration at the top
test.exec_steps = [ex] # add our exec step
test.check_steps = CheckSteps.default_check_steps(test) # add default post-processing

# Execute the test and return its exit code
import sys
sys.exit(test.run())
```

### Dry run
During test development, it is useful to perform a "dry run" which goes through the test configuration and prints all
commands to be executed without executing them. Dry run can be achieved by setting the environment variable
`TRIGVALSTEERING_DRY_RUN` to 1. It can be done globally in a shell, but typically one would set it individually for
each execution. For example, running:
```
TRIGVALSTEERING_DRY_RUN=1 test_trigP1_FullMenu_build.py
```
will execute this particular test in a dry run mode.
