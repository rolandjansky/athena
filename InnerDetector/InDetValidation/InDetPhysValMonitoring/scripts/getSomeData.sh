#!/usr/bin/bash
setupATLAS
localSetupRucioClients
voms-proxy-init -voms atlas
pushd /tmp/$USER
#rucio download --nrandom 1 valid3:valid3.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.ESD.e3099_s2578_r6603_tid05297574_00
rucio download valid3:ESD.05297574._000080.pool.root.1
#Itk
#rucio download mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.DAOD_IDTRKVALID.e2176_s2897_s2901_r8209
rm ~/run/AOD.pool.root
ln -s $PWD/`ls */*` ~/run/AOD.pool.root
rm ~/run/*.xml
popd


