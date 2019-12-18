This is CREST Client C++ Library.
This library realises the client API to the CREST Server.

=============================================================

EXAMPLES

The simple examples are in the crest_example.cxx. This file is in 
the /doc folder.

WARNING! The examples uses the test versions of the CREST servers. 
If they do not respond please edit their network addresses in the 
crest_example.cxx and recompile it.


==============================================================

CrestApi project structure:

CrestApi - this directory conrains the header files for the library (CrestApi.h).
data - this directory contains the data files used in the CrestApi examples.
doc - this directory contains examples for the library
scripts - this directory contains the bash scripts used to create some objects in the CREST Server.
src - this directory contains the library source code (CrestApi.cxx).
test - this directory contains the ATLAS test (test.cxx) It has to be written later.

CMakeList.txt - CMake file for the package CrestApi.

