#!/bin/zsh

MAX_FILES_E=61
MAX_FILES_J=40
MAX_FILES_SE=61
MAX_FILES_H=61
MAX_FILES_JPSI=61

#MAX_FILES_E=5
#MAX_FILES_J=5
#MAX_FILES_SE=5
#MAX_FILES_H=5

MAX_FILES_MTPT_E=5
MAX_FILES_MTPT_SE=5
MAX_FILES_MTPT_J=5
MAX_FILES_MTPT_H=5

ROOTDATA="/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/dferreir"

cat >TrigEgammaValidation_TestConfiguration.xml <<EOF
<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
<unifiedTestConfiguration>

  <atn />
  <kv />

   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">
      <rttContactPerson>Denis Damazio (damazio@mail.cern.ch)</rttContactPerson>
      <rttContactPerson>Danilo Enoque Ferreira de Lima (daniloefl@gmail.com)</rttContactPerson>

<!--      <mailto>damazio@mail.cern.ch, Danilo.Enoque.Ferreira.De.Lima@cern.ch</mailto> -->
      <mailto>damazio@mail.cern.ch, Danilo.Enoque.Ferreira.De.Lima@cern.ch, andreza@cern.ch</mailto>
      <refRelease>12.0.4</refRelease>

      <jobList>
EOF

cat >DataSetCatalog.xml <<EOF
<?xml version="1.0"?>

<dsns>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00001.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00001.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00002.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00002.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00003.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00003.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00004.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00004.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00005.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00005.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00006.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00006.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00007.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00007.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00008.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00008.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00009.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00009.pool.root</physical>
  </dsn>

  <dsn>
    <logical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00010.pool.root</logical>
    <physical>/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005200.T1_McAtNlo_Jimmy/digit/v12000003/mc12.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000003._00010.pool.root</physical>
  </dsn>

EOF

if (( $MAX_FILES_SE != 0 ))
then

  k=0
  # FOR SINGLE ELECTRONS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_off_se">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_off_se.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/` ; do
    if (( $k >= $MAX_FILES_SE )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</physical>
  </dsn>
EOF
  done
#  for i in `nsls $ROOTDATA/BSs-2008/misal1_csc11.007003.singlepart_e_Et25.digit.BS.v12003101_tid003505/` ; do
#    if (( $k >= $MAX_FILES_SE )) ; then
#      break
#    fi
#    k=$k+1
#    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
#	       <dataset_info>
#	       <bytestream/>
#               <datasetName>$ROOTDATA/BSs-2008/misal1_csc11.007003.singlepart_e_Et25.digit.BS.v12003101_tid003505/$i</datasetName>
#	       </dataset_info>
#EOF
#    cat >>DataSetCatalog.xml <<EOF
#  <dsn>
#    <logical>$ROOTDATA/BSs-2008/misal1_csc11.007003.singlepart_e_Et25.digit.BS.v12003101_tid003505/$i</logical>
#    <physical>$ROOTDATA/BSs-2008/misal1_csc11.007003.singlepart_e_Et25.digit.BS.v12003101_tid003505/$i</physical>
#  </dsn>
#EOF
#  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         </athena>
EOF
fi

if (( $MAX_FILES_SE != 0 ))
then

  k=0
  # FOR SINGLE ELECTRONS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_se">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_se.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/` ; do
    if (( $k >= $MAX_FILES_SE )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</physical>
  </dsn>
EOF

  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi

if (( $MAX_FILES_J != 0 ))
then

  k=0
  # FOR JETS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_off_j">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_off_j.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

#  for i in `nsls $ROOTDATA/BSs-2008/misal1_mc12.005802.JF17_pythia_jet_filter.digit.BS.v12003105_tid004254/` ; do
#    if (( $k >= $MAX_FILES_J )) ; then
#      break
#    fi
#    k=$k+1
#    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
#	       <dataset_info>
#	       <bytestream/>
#               <datasetName>$ROOTDATA/BSs-2008/misal1_mc12.005802.JF17_pythia_jet_filter.digit.BS.v12003105_tid004254/$i</datasetName>
#	       </dataset_info>
#EOF
#    cat >>DataSetCatalog.xml <<EOF
#  <dsn>
#    <logical>$ROOTDATA/BSs-2008/misal1_mc12.005802.JF17_pythia_jet_filter.digit.BS.v12003105_tid004254/$i</logical>
#    <physical>$ROOTDATA/BSs-2008/misal1_mc12.005802.JF17_pythia_jet_filter.digit.BS.v12003105_tid004254/$i</physical>
#  </dsn>
#EOF
#  done

  for i in `nsls $ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/` ; do
    if (( $k >= $MAX_FILES_J )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/$i</logical>
    <physical>$ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/$i</physical>
  </dsn>
EOF
  done

  cat >>TrigEgammaValidation_TestConfiguration.xml<<EOF
         </athena>
EOF
fi


if (( $MAX_FILES_J != 0 ))
then

  k=0
  # FOR JETS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_j">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_j.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/` ; do
    if (( $k >= $MAX_FILES_J )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/$i</logical>
    <physical>$ROOTDATA/RDOs/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/$i</physical>
  </dsn>
EOF
  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         </athena>
EOF
fi

#cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
#<!--
#EOF

if (( $MAX_FILES_H != 0 ))
then

  k=0
  # FOR H->gg
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_off_h">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_off_h.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/` ; do
    if (( $k >= $MAX_FILES_H )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/$i</logical>
    <physical>$ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/$i</physical>
  </dsn>
EOF
  done
#  for i in `nsls $ROOTDATA/BSs-2008/mc11.003047.H3_120_gamgam.digit.BS.v11000301/` ; do
#    if (( $k >= $MAX_FILES_H )) ; then
#      break
#    fi
#    k=$k+1
#    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
#	       <dataset_info>
#	       <bytestream/>
#               <datasetName>$ROOTDATA/BSs-2008/mc11.003047.H3_120_gamgam.digit.BS.v11000301/$i</datasetName>
#	       </dataset_info>
#EOF
#    cat >>DataSetCatalog.xml <<EOF
#  <dsn>
#    <logical>$ROOTDATA/BSs-2008/mc11.003047.H3_120_gamgam.digit.BS.v11000301/$i</logical>
#    <physical>$ROOTDATA/BSs-2008/mc11.003047.H3_120_gamgam.digit.BS.v11000301/$i</physical>
#  </dsn>
#EOF
#  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi

if (( $MAX_FILES_H != 0 ))
then

  k=0
  # FOR H->gg
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_h">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_h.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/` ; do
    if (( $k >= $MAX_FILES_H )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/$i</logical>
    <physical>$ROOTDATA/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/$i</physical>
  </dsn>
EOF
  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         </athena>
EOF
fi

if (( $MAX_FILES_E != 0 ))
then
  k=0
  # FOR Z->ee
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_off_e">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_off_e.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

#  for i in `nsls $ROOTDATA/BSs-2008/mc12.005144.PythiaZee.digit.BS.v12000003/` ; do
#    if (( $k >= $MAX_FILES_E )) ; then
#      break
#    fi
#    k=$k+1
#    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
#	       <dataset_info>
#	         <bytestream/>
#                 <datasetName>$ROOTDATA/BSs-2008/mc12.005144.PythiaZee.digit.BS.v12000003/$i</datasetName>
#	       </dataset_info>
#EOF
#    cat >>DataSetCatalog.xml <<EOF
#  <dsn>
#    <logical>$ROOTDATA/BSs-2008/mc12.005144.PythiaZee.digit.BS.v12000003/$i</logical>
#    <physical>$ROOTDATA/BSs-2008/mc12.005144.PythiaZee.digit.BS.v12000003/$i</physical>
#  </dsn>
#EOF
#  done
  for i in `nsls $ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/` ; do
    if (( $k >= $MAX_FILES_E )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	         <dc2/>
                 <datasetName>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</physical>
  </dsn>
EOF
  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         </athena>
EOF
fi

if (( $MAX_FILES_E != 0 ))
then

  k=0
  # FOR Z->ee
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_e">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_e.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/` ; do
    if (( $k >= $MAX_FILES_E )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	         <dc2/>
                 <datasetName>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</physical>
  </dsn>
EOF
  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         </athena>
EOF
fi

if (( $MAX_FILES_SE != 0 ))
then

  k=0
  # FOR ANDREZA'S SINGLE ELECTRONS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_off_Nav_se">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_off_Nav_se.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewNavValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/` ; do
    if (( $k >= $MAX_FILES_SE )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</physical>
  </dsn>
EOF

  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi

if (( $MAX_FILES_SE != 0 ))
then

  k=0
  # FOR ANDREZA'S SINGLE ELECTRONS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_truth_Nav_se">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_truth_Nav_se.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewNavValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/` ; do
    if (( $k >= $MAX_FILES_SE )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</physical>
  </dsn>
EOF

  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi


if (( $MAX_FILES_E != 0 ))
then

  k=0
  # FOR ANDREZA'S Z->ee
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_Nav_Zee">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_Nav_Zee.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewNavValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/` ; do
    if (( $k >= $MAX_FILES_SE )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/$i</physical>
  </dsn>
EOF

  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi

if (( $MAX_FILES_JPSI != 0 ))
then

  k=0
  # FOR JPSI -> e + e
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_jpsi">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_jpsi.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.005751.Pythia_directJpsie3e3.digit.RDO.e325_s404_d117/` ; do
    if (( $k >= $MAX_FILES_JPSI )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.005751.Pythia_directJpsie3e3.digit.RDO.e325_s404_d117/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.005751.Pythia_directJpsie3e3.digit.RDO.e325_s404_d117/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.005751.Pythia_directJpsie3e3.digit.RDO.e325_s404_d117/$i</physical>
  </dsn>
EOF
  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi

if (( $MAX_FILES_SE != 0 ))
then

  k=0
  # FOR SINGLE ELECTRONS
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
         <athena userJobId="TrigEgammaValidation_RTT_testOptions_ringer_se">
            <doc></doc>
            <options>TrigEgammaValidation/TrigEgammaValidation_RTT_testOptions_ringer_se.py</options>
            <package>Trigger/TrigValidation/TrigEgammaValidation</package>
            <group>TrigEgammaNewRingerValidation</group>
            <queue>long</queue>
EOF

  for i in `nsls $ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/` ; do
    if (( $k >= $MAX_FILES_SE )) ; then
      break
    fi
    k=$k+1
    cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
	       <dataset_info>
	       <dc2/>
               <datasetName>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</datasetName>
	       </dataset_info>
EOF
    cat >>DataSetCatalog.xml <<EOF
  <dsn>
    <logical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</logical>
    <physical>$ROOTDATA/RDOs/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/$i</physical>
  </dsn>
EOF

  done
  cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
          </athena>
EOF
fi

#cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
#-->
#EOF

# END IT!
cat >>DataSetCatalog.xml <<EOF
</dsns>
EOF

cat >>TrigEgammaValidation_TestConfiguration.xml <<EOF
      </jobList>
      
      <jobGroups>

         <jobGroup name="AthenaTrigEgammaNewValidation" parent="Athena">
            <keepFilePattern>*.root</keepFilePattern>

            <keepFilePattern>*.txt</keepFilePattern>

            <keepFilePattern>*.eps</keepFilePattern>
            <keepFilePattern>*.gif</keepFilePattern>
            <keepFilePattern>*.xml</keepFilePattern>
            <keepFilePattern>*.php</keepFilePattern>
            <keepFilePattern>*.css</keepFilePattern>
	    
	    <keepFilePattern>generalJobOption.py</keepFilePattern>
            <keepFilePattern>out.perfmon.*</keepFilePattern>
            <keepFilePattern>ntuple.pmon.gz</keepFilePattern>
	    <keepFilePattern>*.root</keepFilePattern>
            <keepFilePattern>PerfMonProcessing.C</keepFilePattern>
            <keepFilePattern>file.txt</keepFilePattern>

            <auxFilePattern>generalJobOption.py</auxFilePattern>
            <auxFilePattern>ProcessValidation.C</auxFilePattern>
            <auxFilePattern>CKolmogorov.C</auxFilePattern>
            <auxFilePattern>CompareFilesKolmogorov.C</auxFilePattern>
            <auxFilePattern>PerfMonProcessing.C</auxFilePattern>

            <auxFilePattern>didItFail.py</auxFilePattern>

            <auxFilePattern>reference_e.root</auxFilePattern>
            <auxFilePattern>reference_j.root</auxFilePattern>
            <auxFilePattern>reference_se.root</auxFilePattern>
            <auxFilePattern>reference_h.root</auxFilePattern>
            <auxFilePattern>reference_off_e.root</auxFilePattern>
            <auxFilePattern>reference_off_j.root</auxFilePattern>
            <auxFilePattern>reference_off_se.root</auxFilePattern>
            <auxFilePattern>reference_off_h.root</auxFilePattern>

            <auxFilePattern>summary.php</auxFilePattern>
            <auxFilePattern>select.php</auxFilePattern>
            <auxFilePattern>plots.php</auxFilePattern>
            <auxFilePattern>kolmogorov.php</auxFilePattern>
            <auxFilePattern>table_example.php</auxFilePattern>
            <auxFilePattern>table.php</auxFilePattern>
            <auxFilePattern>perfmon.php</auxFilePattern>

            <auxFilePattern>example.xml</auxFilePattern>
            <auxFilePattern>content.xml</auxFilePattern>

            <auxFilePattern>style.css</auxFilePattern>

            <action position="a">
              <modulename>RttLibraryTools</modulename>
              <testname>PerfMonRunner</testname>
              <arg>
                <argname>fileName</argname>
                <argvalue>ntuple.pmon.gz</argvalue>
              </arg>
            </action>

            <action position="b">
               <modulename>RttLibraryTools</modulename>
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>ProcessValidation.C</argvalue>
               </arg>
            </action>

            <action position="c">
               <modulename>RttLibraryTools</modulename> 
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>CKolmogorov.C</argvalue>
               </arg>
            </action>

            <action position="d">
               <modulename>RttLibraryTools</modulename> 
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>PerfMonProcessing.C</argvalue>
               </arg>
            </action>

            <test position="e">
               <modulename>didItFail</modulename>
               <testname>didItFail</testname>
            </test>

         </jobGroup>

         <jobGroup name="AthenaTrigEgammaNewNavValidation" parent="Athena">
            <keepFilePattern>*.root</keepFilePattern>

            <keepFilePattern>*.txt</keepFilePattern>

            <keepFilePattern>*.eps</keepFilePattern>
            <keepFilePattern>*.gif</keepFilePattern>
            <keepFilePattern>*.xml</keepFilePattern>
            <keepFilePattern>*.php</keepFilePattern>
            <keepFilePattern>*.css</keepFilePattern>
            <keepFilePattern>generalJobOption.py</keepFilePattern>
            <keepFilePattern>convertToGif.C</keepFilePattern>
            <keepFilePattern>out.perfmon.*</keepFilePattern>
            <keepFilePattern>ntuple.pmon.gz</keepFilePattern>
            <keepFilePattern>PerfMonProcessing.C</keepFilePattern>

            <auxFilePattern>generalJobOption.py</auxFilePattern>
            <auxFilePattern>convertToGif.C</auxFilePattern>
            <auxFilePattern>CKolmogorov.C</auxFilePattern>
            <auxFilePattern>CompareFilesKolmogorov.C</auxFilePattern>
            <auxFilePattern>PerfMonProcessing.C</auxFilePattern>

            <auxFilePattern>didItFail.py</auxFilePattern>

            <auxFilePattern>reference_e.root</auxFilePattern>
            <auxFilePattern>reference_j.root</auxFilePattern>
            <auxFilePattern>reference_se.root</auxFilePattern>
            <auxFilePattern>reference_h.root</auxFilePattern>
            <auxFilePattern>reference_off_e.root</auxFilePattern>
            <auxFilePattern>reference_off_j.root</auxFilePattern>
            <auxFilePattern>reference_off_se.root</auxFilePattern>
            <auxFilePattern>reference_off_h.root</auxFilePattern>

            <auxFilePattern>summary.php</auxFilePattern>
            <auxFilePattern>select.php</auxFilePattern>
            <auxFilePattern>plots.php</auxFilePattern>
            <auxFilePattern>kolmogorov.php</auxFilePattern>
            <auxFilePattern>table_example.php</auxFilePattern>
            <auxFilePattern>table.php</auxFilePattern>
            <auxFilePattern>perfmon.php</auxFilePattern>

            <auxFilePattern>example.xml</auxFilePattern>
            <auxFilePattern>content.xml</auxFilePattern>

            <auxFilePattern>style.css</auxFilePattern>

            <action position="a">
               <modulename>RttLibraryTools</modulename>
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>convertToGif.C</argvalue>
               </arg>
            </action>
	    
            <action position="b">
               <modulename>RttLibraryTools</modulename> 
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>CKolmogorov.C</argvalue>
               </arg>
            </action>
	    
            <test position="c">
               <modulename>didItFail</modulename>
               <testname>didItFail</testname>
            </test>

         </jobGroup>

         <jobGroup name="AthenaTrigEgammaNewRingerValidation" parent="Athena">
            <keepFilePattern>*.root</keepFilePattern>

            <keepFilePattern>*.txt</keepFilePattern>

            <keepFilePattern>*.gif</keepFilePattern>
            <keepFilePattern>*.xml</keepFilePattern>
            <keepFilePattern>*.php</keepFilePattern>
            <keepFilePattern>*.css</keepFilePattern>
	    
	    <keepFilePattern>generalJobOption.py</keepFilePattern>
            <keepFilePattern>out.perfmon.*</keepFilePattern>
            <keepFilePattern>ntuple.pmon.gz</keepFilePattern>
	    <keepFilePattern>*.root</keepFilePattern>
            <keepFilePattern>PerfMonProcessing.C</keepFilePattern>
            <keepFilePattern>file.txt</keepFilePattern>

            <auxFilePattern>generalJobOption.py</auxFilePattern>
            <auxFilePattern>PostRinger.C</auxFilePattern>
            <auxFilePattern>PostComparison.C</auxFilePattern>
            <auxFilePattern>CKolmogorov.C</auxFilePattern>
            <auxFilePattern>CompareFilesKolmogorov.C</auxFilePattern>
            <auxFilePattern>PerfMonProcessing.C</auxFilePattern>

            <auxFilePattern>didItFail.py</auxFilePattern>

            <auxFilePattern>summary.php</auxFilePattern>
            <auxFilePattern>select.php</auxFilePattern>
            <auxFilePattern>plots.php</auxFilePattern>
            <auxFilePattern>kolmogorov.php</auxFilePattern>
            <auxFilePattern>table_example.php</auxFilePattern>
            <auxFilePattern>table.php</auxFilePattern>
            <auxFilePattern>perfmon.php</auxFilePattern>

            <auxFilePattern>example.xml</auxFilePattern>
            <auxFilePattern>content.xml</auxFilePattern>

            <auxFilePattern>style.css</auxFilePattern>

            <action position="a">
              <modulename>RttLibraryTools</modulename>
              <testname>PerfMonRunner</testname>
              <arg>
                <argname>fileName</argname>
                <argvalue>ntuple.pmon.gz</argvalue>
              </arg>
            </action>

            <action position="b">
               <modulename>RttLibraryTools</modulename>
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>PostRinger.C</argvalue>
               </arg>
            </action>

            <action position="c">
               <modulename>RttLibraryTools</modulename> 
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>PostComparison.C</argvalue>
               </arg>
            </action>

            <action position="d">
               <modulename>RttLibraryTools</modulename> 
               <testname>ROOTMacroRunner</testname>
               <arg>
                  <argname>macro</argname>
                  <argvalue>PerfMonProcessing.C</argvalue>
               </arg>
            </action>

            <test position="e">
               <modulename>didItFail</modulename>
               <testname>didItFail</testname>
            </test>

         </jobGroup>
      </jobGroups>
   </rtt>

</unifiedTestConfiguration>

EOF

mv TrigEgammaValidation_TestConfiguration.xml ../test/
mv DataSetCatalog.xml ../../../../

