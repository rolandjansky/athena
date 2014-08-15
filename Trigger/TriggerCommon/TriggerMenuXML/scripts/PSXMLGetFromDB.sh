#!/bin/bash

if [[ "$AtlasProject" == "AtlasP1HLT" || "$AtlasProject" == "AtlasCAFHLT" || "$AtlasProject" == "AtlasTestHLT" ]]; then

    get_files -scripts cnvXML.py
    get_files -xmls TriggerCosts_1e33.xml

    #export NICOS_ATLAS_RELEASE=16.1.1.2

    if [[ "$NICOS_ATLAS_RELEASE" != "" ]]; then
        release=$NICOS_ATLAS_RELEASE
    else
        release=0
    fi

    useDB=False

#All files taken from MC DB (L1 only hence no HLT ps given)
#python ../scripts/TriggerMenuXML-rules-download.py 100,500,1000,1500,2000 ${release} ${useDB}

fi
