#!/usr/bin/bash
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

Reco_tf.py --AMI q431 --multithreaded True --steering='doRAWtoALL' \
           --preExec 'all:from AthenaMonitoring.DQMonFlags import DQMonFlags; DQMonFlags.doMonitoring=True; DQMonFlags.doNewMonitoring=True; DQMonFlags.doHLTMon=False' \
           --imf False --maxEvents 500 --outputAODFile=myAOD.pool.root --outputHISTFile=myHIST.root
