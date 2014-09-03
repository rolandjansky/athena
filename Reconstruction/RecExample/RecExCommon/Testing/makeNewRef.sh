#!/bin/bash

gmake check

cat ../run/RecExCommonTesting.log_stripped ../run/TheLogs | \
  sed "s/^Current Job/Referencefile/g" > ../run/newRef
