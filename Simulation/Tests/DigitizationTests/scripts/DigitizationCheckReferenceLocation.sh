#!/bin/sh 

export DigitizationTestsVersion="dev"
if [[ $ATLAS_RELEASE_BASE == *"20.7"* ]]; then 
    DigitizationTestsVersion="20.7.X"
elif [[ $ATLAS_RELEASE_BASE == *"20.20"* ]]; then 
    DigitizationTestsVersion="20.20.X"
elif [[ $ATLAS_RELEASE_BASE == *"mig7"* ]]; then
    DigitizationTestsVersion="20.3"
elif [[ $ATLAS_RELEASE_BASE == *"20.3"* ]]; then
    DigitizationTestsVersion="20.3"
elif [[ $ATLAS_RELEASE_BASE == *"20.1"* ]]; then
    DigitizationTestsVersion="20.1.X"
fi


