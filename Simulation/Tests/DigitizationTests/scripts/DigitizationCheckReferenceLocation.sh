#!/bin/bash

DigitizationTestsVersion="dev"
if [[ $ATLAS_RELEASE_BASE == *"20.7.X.Y-VAL"* ]]; then
    DigitizationTestsVersion="20.7.X.Y-VAL"
elif [[ $ATLAS_RELEASE_BASE == *"20.7"* ]]; then
    DigitizationTestsVersion="20.7.X"
elif [[ $ATLAS_RELEASE_BASE == *"20.9"* ]]; then
    DigitizationTestsVersion="20.9"
elif [[ $ATLAS_RELEASE_BASE == *"20.20"* ]]; then
    DigitizationTestsVersion="20.20.X"
elif [[ $ATLAS_RELEASE_BASE == *"mig7"* ]]; then
    DigitizationTestsVersion="20.3"
elif [[ $ATLAS_RELEASE_BASE == *"20.3"* ]]; then
    DigitizationTestsVersion="20.3"
elif [[ $ATLAS_RELEASE_BASE == *"20.1"* ]]; then
    DigitizationTestsVersion="20.1.X"
elif [[ $ATLAS_RELEASE_BASE == *"21.0"* ]]; then
    DigitizationTestsVersion="21.0.X"
elif [[ $ATLAS_RELEASE_BASE == *"21.3"* ]]; then
    DigitizationTestsVersion="21.3.X"
elif [[ $ATLAS_RELEASE_BASE == *"22.0-mc20"* ]] || [[ $ATLAS_RELEASE_BASE == *"22.0.41."* ]]; then
    DigitizationTestsVersion="22.0-mc20"
fi

export DigitizationTestsVersion
