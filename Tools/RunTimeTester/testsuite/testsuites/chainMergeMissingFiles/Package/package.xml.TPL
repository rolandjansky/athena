<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

      <mailto>brinick.simmons@cern.ch</mailto>
      <refRelease>14.5.0</refRelease>

      <jobList>
        <chain>
	  <chainName>MissingFileMergeJob</chainName>
	  <sequential>
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
                  <datasetName>/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/14.5.X/root_archive/archive/RPCwBeam_data08_cosmag.0091890.AOD.pool.root</datasetName>

		</dataset_info>
	      </chaindataset_info>
	      
	      <chaindataset_info>
		<jobTransformData/>
		<chaindatasetName>job2.AOD.pool.root</chaindatasetName>
		<dataset_info>
		  <jobTransformData/>
                  <datasetName>/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/14.5.X/root_archive/archive/RPCwBeam_data08_cosmag.0091890.AOD.pool.root</datasetName>
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
