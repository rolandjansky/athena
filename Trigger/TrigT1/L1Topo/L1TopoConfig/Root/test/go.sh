#!/bin/bash

cd ../../cmt
cmt config
cmt make
cd ../src/test

TrigConfTopoReader test.xml