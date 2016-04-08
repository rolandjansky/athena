<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
  <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

    <mailto>brinick.simmons@cern.ch</mailto>
    <refRelease>14.5.0</refRelease>

    <jobList>

      <jobTransform>
	<doc>5224.AlpgenJimmyWenuNp3</doc>
	<jobTransformJobName>5224.AlpgenJimmyWenuNp3</jobTransformJobName>
	<jobTransformCmd>
	  csc_evgen_trf.py -t runNumber=5224 firstEvent=1 maxEvents=50 randomSeed=54298752 jobConfig=CSC.005224.AlpgenJimmyWenuNp3.py outputEvgenFile=CSC.005224.AlpgenJimmyWenuNp3.evgen.pool.root histogramFile=NONE ntupleFile=NONE inputGeneratorFile=/afs/cern.ch/atlas/offline/ProdData/13.0.X/alpgen.005224.WenuNp3._00023.tar.gz
	</jobTransformCmd>
	<group>JobTransform</group>
	<dataset_info>
	  <jobTransformData />
	  <datasetName>/afs/cern.ch/atlas/offline/ProdData/13.0.X/alpgen.005224.WenuNp3._00023.tar.gz</datasetName>
	</dataset_info>
      </jobTransform>

    </jobList>
       
    <jobGroups>
          
      <jobGroup name="JobTransform" parent="Transform">
	<keepFilePattern>*.root</keepFilePattern>
	<keepFilePattern>*.txt</keepFilePattern>

	<test>
	  <modulename>RttLibraryTools</modulename>
	  <testname>FileGrepper</testname>
	  <arg>
	    <argname>inputFile</argname>
	    <argvalue>*_log</argvalue>
	  </arg>
	  <arg>
	    <argname>outputFile</argname>
	    <argvalue>RTT_search_results.txt</argvalue>
	  </arg>
	  <arg>
	    <argname>searchList</argname>
	    <argvalue>ErrorCode=0</argvalue>
	  </arg>
	</test>
	
      </jobGroup>
      
    </jobGroups>
    
  </rtt>
</unifiedTestConfiguration>
