#!/bin/bash
# art-description: Athena-based Rivet run test 
# art-type: build
# art-include: master/AthGeneration
# art-output: *.yoda.gz
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;

cat <<EOF > hepmc.events
HepMC::Version 2.06.11
HepMC::IO_GenEvent-START_EVENT_LISTING
E 0 -1 7.950078896571e+03 1.184039997481e-01 7.763841138914e-03 0 0 1 0 0 0 9 1.628213720795e+02 7.890544115160e-03 2.374571580672e+03 1.000000000000e+00 1.628213720795e+02 1.499999496771e+02 1.628213720795e+02 1.736491475314e+02 1.628213720795e+02
N 9 "Weight" "MEWeight" "WeightNormalisation" "NTrials" "MUR0.5_MUF1_PDF261000" "MUR1_MUF0.5_PDF261000" "MUR1_MUF1_PDF261000" "MUR1_MUF2_PDF261000" "MUR2_MUF1_PDF261000"
U GEV MM
C 1.628213720795e+02 1.628213720795e+02
F 2 -2 1.534004381395e-04 3.066607111596e-01 8.916321492954e+01 2.932005484610e+00 9.963795774716e-03 0 0
V -1 0 0 0 0 0 2 18 0
P 10001 21 9.566679486094e-09 -8.696121886238e-09 4.730183853766e+02 4.730183853767e+02 2.563521975160e-04 11 0 0 -1 0
P 10002 -2 0 0 -2.112559187446e+03 2.112559187446e+03 0 11 0 0 -1 0
P 10003 21 -3.660655267407e-01 5.303090117222e+00 3.503923467891e+02 3.504326660818e+02 2.074367694751e-04 1 0 0 0 0
P 10004 21 -1.912302367621e+00 -1.565265654564e+00 9.783300571621e+00 1.009058605777e+01 1.839071347778e-06 1 0 0 0 0
P 10005 21 -3.345059789036e-02 2.452196877249e-01 2.150341181045e+01 2.150483599398e+01 8.549891031762e-06 1 0 0 0 0
P 10006 -4 9.488764939879e-01 -8.473738612065e+00 7.202305960399e+01 7.253993470762e+01 1.420000000171e+00 1 0 0 0 0
P 10007 2 -9.152986457418e-01 1.786936665098e+00 1.046714095140e+00 2.264182998994e+00 -1.339779305040e-06 1 0 0 0 0
P 10008 -2 -1.624540277937e+00 2.522072124027e+00 -4.041959090288e-02 3.000268764085e+00 -3.754382974690e-06 1 0 0 0 0
P 10009 21 -2.567093921746e+00 -9.548891074080e-01 -2.904689037702e+00 3.992368071240e+00 -8.870885411583e-07 1 0 0 0 0
P 10010 21 -3.688572241228e+00 2.289879266805e+00 6.980961973403e+00 8.220884521108e+00 -3.602014763293e-06 1 0 0 0 0
P 10011 21 -1.392966980455e+00 -5.427392943930e-01 9.831899638784e-01 1.789297475375e+00 -2.021511540665e-06 1 0 0 0 0
P 10012 4 -7.025427264907e+00 1.459963049198e+01 -1.954600479461e+01 2.542771209110e+01 1.419999999999e+00 1 0 0 0 0
P 10013 21 1.703128677078e+00 -1.411585682565e+00 -7.191693427129e+00 7.524205989985e+00 1.192092895508e-07 1 0 0 0 0
P 10014 21 2.017638760353e+00 2.244438987475e-01 -1.880205126992e+00 2.767022325629e+00 1.192092895508e-07 1 0 0 0 0
P 10015 21 7.849144665604e+00 6.560885837257e-01 -1.520321060230e+01 1.712241620885e+01 -3.371747880872e-07 1 0 0 0 0
P 10016 -2 5.857859821719e+00 -2.070389315227e+01 -4.961755747974e+02 4.966418898350e+02 -5.394796609394e-06 1 0 0 0 0
P 10017 21 -6.683178610779e-01 -4.013578096477e-01 -1.253421178782e+01 1.255843151021e+01 2.384185791016e-07 1 0 0 0 0
P 10018 21 5.186393338879e+00 -1.792985274902e+00 -1.011951972955e+02 1.013438771102e+02 1.348699152349e-06 1 0 0 0 0
P 10019 11 3.277813803155e+01 -7.164003915322e-01 -2.434353201235e+02 2.456332116108e+02 5.109985977063e-04 1 0 0 0 0
P 10020 -11 -3.614714409413e+01 8.935494135385e+00 -1.202147260294e+03 1.202723781470e+03 5.115678288037e-04 1 0 0 0 0
HepMC::IO_GenEvent-END_EVENT_LISTING
EOF

mkdir -p 100000
cat <<EOF > 100000/mc.HepMC_test.py
evgenConfig.description = "Simple test JO for Rivet ART"
evgenConfig.keywords = ["Z","SM"]
evgenConfig.contact = ["chris.g@cern.ch"]
evgenConfig.generators += ["HepMCAscii"]
from TruthIO.TruthIOConf import HepMCReadFromFile
genSeq += HepMCReadFromFile()
del testSeq.TestHepMC
EOF

Gen_tf.py --ecmEnergy=13000. --maxEvents=1 --outputYODAFile=test_gen_rivet_athena.yoda.gz \
          --jobConfig=./100000 --inputGeneratorFile=hepmc.events --rivetAnas=MC_GENERIC

echo "art-result: $? generate"

