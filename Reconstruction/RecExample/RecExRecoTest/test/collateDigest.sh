#!/bin/bash
diff $1 $2 | tee temp.log
grep nTopo $1 | sed 's/^/   /g' >& grep.log
cat grep.log temp.log >& $3 
