#!/bin/bash

#mkdir /datadisk1/temp2/HITAG; cd /datadisk1/temp2/HITAG
#set +o noglob
#for f in ~/nfs3/zerobias_skim/data11_hi.*MinBiasOverlay*TAG*/*; do ln -s $f; done
#cd -

root -l -b -q HITAGprinter_run.C 

#mkdir HI_files
#mv HI_filter.txt HI_files/data11_hi.metadata.physics_MinBiasOverlay.filter.txt
#mv HI_vtx.txt HI_files/data11_hi.metadata.physics_MinBiasOverlay.vtx.txt
#dq2-put -L CERN-PROD_SCRATCHDISK -s HI_files group.dataprep.data11_hi.metadata.physics_MinBiasOverlay.test15

