<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

      <mailto>brinick.simmons@cern.ch</mailto>
      <refRelease>14.5.0</refRelease>

      <jobList>

          <jobTransform>
             <doc>7218.singlepart_mu20</doc>
             <jobTransformJobName>7218.singlepart_mu20</jobTransformJobName>
             <jobTransformCmd>
                   csc_evgen_trf.py -t runNumber=7218 firstEvent=1 maxEvents=1000 randomSeed=54298752 jobConfig=CSC.007218.singlepart_mu20.py outputEvgenFile=CSC.007218.singlepart_mu20.evgen.pool.root histogramFile=NONE ntupleFile=NONE inputGeneratorFile=NONE
              </jobTransformCmd>
              <group>JobTransform</group>
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
