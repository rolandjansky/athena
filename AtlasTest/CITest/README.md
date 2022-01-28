Tests defined in this package are run for every merge request. Only experts should modify
these as they have a significant impact on the CI turnaround time.

[TOC]

## Test definition
- Test are defined in separate files for each project (e.g. [`Athena.cmake`](Athena.cmake)).
- Tests should have a short self-explanatory name. Do not add the word "test" to the name itself.
- If tests depend on each other consider using a common basename and delimit the steps with an underscore, 
  e.g. `Muon_digi`, `Muon_reco`.
- Use the dedicated [`atlas_add_citest`](cmake/CITestFunctions.cmake) command for test definitions,
  which is an extension of the regular [`atlas_add_test`](https://twiki.cern.ch/twiki/bin/view/AtlasComputing/SoftwareDevelopmentWorkBookCMakeInAtlas#atlas_add_test) command. It has a few extra arguments and sets
  different defaults suitable for CI tests.
- Additional properties can be set using the `PROPERTIES` keyword. See the 
[cmake documentation](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#test-properties) for a full list.

**Simple tests** should be added inline:
```cmake
atlas_add_citest( q221
   SCRIPT Reco_tf.py --AMI q221 )
```
For more **complex commands**, or **any command that contains a semicolon (`;`)** use a dedicated script. 
Either one available within the release:
```cmake
atlas_add_citest( Digitization_NewConfig
   SCRIPT DigitizationConfigNew_test.py )
```
or add a script to the [`test/`](test/) folder:
```cmake
atlas_add_citest( FastChain
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/FastChain.sh )
```
**Test dependencies** can be declared via the `DEPENDS` keyword
```cmake
atlas_add_citest( Test1 ... )
atlas_add_citest( Test2 ...
   DEPENDS Test1
   [PROPERTIES REQUIRED_FILES ../Test1/myAOD.pool.root]
)
```
The optional `REQUIRED_FILES` property ensures that the test is not even started 
if the input file is missing. These tests then appear as "Not Run" in the test summary
(instead of "Failed"). Use a relative path to the other test's working directory.

For **MT/MP-tests**, add the number of required CPU cores (used for job scheduling):
```cmake
atlas_add_citest( ...
   PROPERTIES PROCESSORS 8 )
```
Rather than matching the number of actual cores used, this number should reflect the
expected system load. E.g. if a job runs with 8 threads but the system load during running is
significant lower, one can reduce this number to allow other jobs to run in parallel.

### Post-processing
All tests defined with `atlas_add_citest` run a [default post-processing script](cmake/citest_post.sh.in) 
that checks the log file for errors using [`noerror.sh`](../TestTools/).
An additional post-processing script can be specified with the 
`POST_EXEC_SCRIPT` keyword. The overall test result will be the exit code of that 
post-processing script! The orignal test result is stored in the
`${ATLAS_CTEST_TESTSTATUS}` environment variable and can be used in the post-processing
script if needed.

To temporarily ignore an error message, use:
```cmake
atlas_add_citest( ...
   LOG_IGNORE_PATTERN "my error to ignore" )
```

New post-processing scripts should be made as general as possible and named as
[`test/checkXYZ.sh`](test/).


## Running the tests
The tests in this package can be run locally like any other unit tests after compiling
the package locally and running `ctest`.

To avoid running the CI tests as part of the regular unit testing in the nightly build, 
the tests are disabled by default. To enable them (e.g. for CI builds), the following
needs to be added to the `cmake` command line:
```sh
cmake -DATLAS_ENABLE_CI_TESTS=TRUE ...
```
This is done already by default for partial `WorkDir` builds so a regular developer does
not need to worry about this.

All tests in this package carry the "CITest" label and therefore they can be included/excluded
in `ctest` runs, via:
```sh
ctest -L CITest   # only run CI tests
ctest -LE CITest  # run all tests, except CI tests
```

## Internals
- All tests are run in a separate working directory in the build area: `AtlasTest/CITest/CMakeFiles/ciTestRun/<test>/` where `<test>` is the name used in `atlas_add_citest`.
- The main test log file within the working directory is named `<test>.log`. 
- If you want to verify the final command that is being run (useful e.g. to debug issues with quotes), check the content of `AtlasTest/CITest/test-bin/<test>.exe` in the build area.
