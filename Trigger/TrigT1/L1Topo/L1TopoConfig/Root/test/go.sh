#!/bin/bash

cd ../../cmt
cmt config
cmt make
cd ../src/test

TrigConfTopoReader L1TopoMenu_smk_2323.xml