#!/bin/sh
# art-type: build
# art-ci: master
rm -rf newJOtest.py
get_files TrigUpgradeTest/newJOtest.py
python newJOtest.py # generate pickle
athena --threads=1 newJOtest.pkl
