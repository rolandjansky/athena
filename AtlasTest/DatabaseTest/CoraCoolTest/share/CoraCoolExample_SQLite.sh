#!/bin/bash
# test for CoraCoolExample, writing and reading from a local SQLite file
echo Start date is `date`
# test with bulk operations - no references to avoid COOL SV backinserter limit
CoraCoolExample.exe "sqlite://X;schema=test0.db;dbname=TESTCORA" wrbpj /CoraCool/Test1 3 10 5 5 7 9
# test without bulk but using references, only one IOV
CoraCoolExample.exe "sqlite://X;schema=test1.db;dbname=TESTCORA" wrba /CoraCool/Test1 3 10 5 5 7 9
echo End date is `date`
exit
