#!/usr/bin/bash
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

format=$1
inputAOD=$2

Reco_tf.py --AMI q431 \
           --steering doRAWtoALL \
           --inputAODFile ${inputAOD} \
           --outputDAODFile pool.root \
           --reductionConf ${format} \
           --preExec 'AODtoDAOD:from AthenaCommon.DetFlags import DetFlags;DetFlags.detdescr.all_setOff();DetFlags.BField_setOn()'
