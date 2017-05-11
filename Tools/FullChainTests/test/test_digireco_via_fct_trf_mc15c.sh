#!/bin/sh
#
#art-description: digireco
#art-queue: long
#
fct_trf.py --config=$(echo $CMAKE_PREFIX_PATH | tr ":" "\n" | grep AtlasOffline)/src/Tools/FullChainTests/test/digireco_MC15c.cfg --runDigi --runReco

