#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Sherpa tttautau
# art-type: grid
# art-output: EVNT.root
# art-output: MyOutput.root
# art-output: MyOutput.yoda.gz
# art-output: dcube
# art-html: dcube
# art-output: mc.*.py

mkdir 100200
cd 100200
cat > mc.Sh_TtllTest.py <<EOF
include("Sherpa_i/Base_Fragment.py")
include("Sherpa_i/NNPDF30NNLO.py")

evgenConfig.description = "Sherpa tttautau@LO"
evgenConfig.keywords = ["top", "SM", "multilepton"]
evgenConfig.contact = [ "atlas-generators-sherpa@cern.ch", "frank.siegert@cern.ch" ]
evgenConfig.nEventsPerJob = 10

genSeq.Sherpa_i.RunCard="""
(run){
  # ME setup
  ME_SIGNAL_GENERATOR Comix;
  INTEGRATION_ERROR=0.2;
  FINISH_OPTIMIZATION=Off;
  CORE_SCALE VAR{H_TM2/4}
  EXCLUSIVE_CLUSTER_MODE 1;

  # top/W decays
  HARD_DECAYS On;
  HARD_SPIN_CORRELATIONS 1;
  SOFT_SPIN_CORRELATIONS 1;
  SPECIAL_TAU_SPIN_CORRELATIONS=1
  STABLE[24] 0;
  STABLE[6] 0;
}(run)

(processes){
  Process 93 93 -> 6 -6 15 -15;
  Order (*,2);
  End process;
}(processes)

(selector){
  Mass 15 -15  5 E_CMS;
}(selector)
"""

genSeq.Sherpa_i.Parameters += [ "WIDTH[6]=0.0", "WIDTH[24]=0.0", "LOG_FILE="]
genSeq.Sherpa_i.Parameters += [ "EW_SCHEME=3", "GF=1.166397e-5" ]
EOF

cd ..

export TRF_ECHO=True;

Gen_tf.py --ecmEnergy=13000. --maxEvents=10000 --firstEvent=-1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=100200 --outputYODAFile=MyOutput.yoda.gz --postInclude=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/MC_JO/MC_test_03_Zjet.py 
echo "art-result: $? Gen_tf"

asetup 22.6.1,AthGeneration
source setupRivet

python /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/rootconvert.py MyOutput.yoda.gz

echo "art-result: $? convert"

dcubeName="Sherpa"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Sherpa_i/test_00_tttautau/test_00_config.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Sherpa_i/test_00_tttautau/test_00_hists.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName MyOutput.root $dcubeXml $dcubeRef

echo  "art-result: $? Dcube"