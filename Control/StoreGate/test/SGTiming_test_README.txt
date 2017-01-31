This program will time StoreGate record/retrieve and clearStore
opeartions.

First you need to run SGTiming_test_gen.pl, which will generate the
include files and classes. You can adjust the number of types and keys
(vars NTYPES and NKEYS). The default is 100 classes and 20
keys. Generating all the CLIDs can be quite slow, so if you are going
to run many tests, set NTYPES to the larges number you will need,
generate the files, and then set DO_OBJGEN = 0, which will prevent
regeneration of the CLIDs, and just read them from the previously
written file.

It will do the timing test for 100 iterations, clearing the store at
the end of each iteration. In each iteration of the loop it will
create all the objects, the do all the records, then do all the
retrieves, then clear the store. The timing of each section is
independent. The timing of the record phase does not include the
creation of the objects.

In DBG builds by default it does an assert on each record/retrieve
operation, and does not for the OPT builds (by checking value of the
LCG_PLATFORM env var). This can be overriden if desired.

WARNING: setting NTYPES to a large number makes for a very slow
compilation, especially in OPT builds.

As it runs, it displays the total time for each set of record /
retrieve / clearStore operations. At the end of the run, it will
display the average time per record/retrieve operation in
microseconds. This average does not include the first iteration of the
loop, which is usually twice as slow to record.
