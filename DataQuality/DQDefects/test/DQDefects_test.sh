#!/bin/bash

NOSEPATH=`python -c 'import nose; f=nose.__file__; i=f.find("/lib/"); print f[0:i]'`
$NOSEPATH/bin/nosetests -v DQDefects