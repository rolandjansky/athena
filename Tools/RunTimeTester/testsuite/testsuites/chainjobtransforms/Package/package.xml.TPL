<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
  <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

    <mailto>brinick.simmons@cern.ch</mailto>
    <refRelease>14.5.0</refRelease>

    <jobList>

      <chain>
	<chainName>TwoParallelThenMergeJob</chainName>
	<sequential>
	  <parallel>

	    <chainElement>
	      <jobTransform>
		<doc>ParallelJob1</doc>
		<jobTransformJobName>ParallelJob1</jobTransformJobName>
		<jobTransformCmd>
		  Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=-1 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preInclude=RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py outputESDFile=myESD.pool.root outputAODFile=job1.AOD.pool.root outputMuonCalibNtp=muonCalib.root HIST=myMergedMonitoring.root RAW_IDPROJCOMM=IDPROJCOMM._0001.data DPD_PIXELCOMM=PIXELCOMM.pool.root DPD_SCTCOMM=SCTCOMM.pool.root DPD_IDCOMM=IDCOMM.pool.root DPD_IDPROJCOMM=IDPROJCOMM.pool.root DPD_CALOCOMM=CALOCOMM.pool.root DPD_EMCLUSTCOMM=EMCLUSTCOMM.pool.root DPD_EGAMTAUCOMM=EGamTauComm.pool.root DPD_RPCCOMM=RPCCOMM.pool.root DPD_TGCCOMM=TGCCOMM.pool.root DPD_TILECOMM=TILECOMM.pool.root postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py --ignoreunknown
		</jobTransformCmd>
		<group>JobTransform</group>

		<dataset_info>
		  <jobTransformData />
		  <datasetName>/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data</datasetName>
		</dataset_info>

	      </jobTransform>

	      <chainfileout>job1.AOD.pool.root</chainfileout>
	    </chainElement>

	    <chainElement>

	      <jobTransform>
		<doc>ParallelJob2</doc>
		<jobTransformJobName>ParallelJob2</jobTransformJobName>
		<jobTransformCmd>
		  Reco_trf.py inputBSFile=/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data maxEvents=-1 trigStream=IDCosmic autoConfiguration=FieldAndGeo,BeamType,ConditionsTag preInclude=RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py outputESDFile=myESD.pool.root outputAODFile=job2.AOD.pool.root outputMuonCalibNtp=muonCalib.root HIST=myMergedMonitoring.root RAW_IDPROJCOMM=IDPROJCOMM._0001.data DPD_PIXELCOMM=PIXELCOMM.pool.root DPD_SCTCOMM=SCTCOMM.pool.root DPD_IDCOMM=IDCOMM.pool.root DPD_IDPROJCOMM=IDPROJCOMM.pool.root DPD_CALOCOMM=CALOCOMM.pool.root DPD_EMCLUSTCOMM=EMCLUSTCOMM.pool.root DPD_EGAMTAUCOMM=EGamTauComm.pool.root DPD_RPCCOMM=RPCCOMM.pool.root DPD_TGCCOMM=TGCCOMM.pool.root DPD_TILECOMM=TILECOMM.pool.root postInclude=RecJobTransforms/reducedRegionSelectorPostConfig.py --ignoreunknown
		</jobTransformCmd>
		<group>JobTransform</group>

		<dataset_info>
		  <jobTransformData />
		  <datasetName>/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data</datasetName>
		</dataset_info>

	      </jobTransform>

	      <chainfileout>job2.AOD.pool.root</chainfileout>
	    </chainElement>
	    
	  </parallel>

	  <chainElement>

	    <jobTransform>
	      <doc>MergeJob</doc>
	      <jobTransformJobName>MergeJob</jobTransformJobName>
	      <jobTransformCmd>
		MergePool_trf.py maxEvents=-1 inputAODFile=job1.AOD.pool.root,job2.AOD.pool.root geometryVersion=ATLAS-GEO-03-00-00 outputAODFile=MergedAOD.pool.root preInclude=RecExCommission/MinimalCommissioningSetup.py preExec='rec.doEgamma.set_Value_and_Lock(False)' --ignoreunknown
	      </jobTransformCmd>

	      <group>JobTransform</group>
	      
	      <chaindataset_info>
		<jobTransformData/>
		<chaindatasetName>job1.AOD.pool.root</chaindatasetName>
		<dataset_info>
		  <jobTransformData/>
		  <datasetName>/castor/cern.ch/user/s/seuster/JetRec_RTT/J5/calib0.005014.J5_pythia_jetjet.digit.RDO.v12003101_tid003440._00006.pool.root.2</datasetName>
		</dataset_info>
	      </chaindataset_info>
	      
	      <chaindataset_info>
		<jobTransformData/>
		<chaindatasetName>job2.AOD.pool.root</chaindatasetName>
		<dataset_info>
		  <jobTransformData/>
		  <datasetName>/castor/cern.ch/user/s/seuster/JetRec_RTT/J5/calib0.005014.J5_pythia_jetjet.digit.RDO.v12003101_tid003440._00006.pool.root.2</datasetName>
		</dataset_info>
	      </chaindataset_info>
  
	    </jobTransform>
	    
	  </chainElement>
	</sequential>
      </chain>

    </jobList>
          
    <jobGroups>
      <jobGroup name="JobTransform" parent="Transform"></jobGroup>     
    </jobGroups>
       
  </rtt>
</unifiedTestConfiguration>
