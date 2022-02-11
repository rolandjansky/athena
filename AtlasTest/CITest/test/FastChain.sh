#!/usr/bin/bash
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" \
    --outputRDOFile RDO_CG.pool.root \
    --maxEvents 2 \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.experimentalDigi=["NewMerge"]' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --DataRunNumber '284500' \
    --imf False
