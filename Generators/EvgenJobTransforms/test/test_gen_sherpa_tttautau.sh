#!/bin/bash
# art-description: Generation test Sherpa ttll without inputs 
# art-type: build
# art-include: master/AthGeneration
# art-output: *.root
# art-output: log.generate
# art-output: mc.*.py

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

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=. --maxEvents=10 \
    --outputEVNTFile=test_sherpa_tttautau.EVNT.pool.root \

echo "art-result: $? generate"
