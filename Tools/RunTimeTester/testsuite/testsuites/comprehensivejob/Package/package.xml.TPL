<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

      <mailto>brinick.simmons@cern.ch</mailto>
      <refRelease>14.5.0</refRelease>

      <jobList>

          <jobTransform>
             <doc>comprehensive job - test all RTT features</doc>
	     <jobTransformJobName>ComprehensiveJob1</jobTransformJobName>
             <jobTransformCmd>echo "1 2 3 4 5 6 7 8 9 10" &gt;&amp; comprehensiveJob.data;echo "ErrorCode=0 (OK)";</jobTransformCmd>
             <group>JobTransform</group>
             <keepFilePattern>comprehensiveJob.data</keepFilePattern>
             <keepFilePattern>nonexistant.data</keepFilePattern>
          </jobTransform>

       </jobList>
          
       <jobGroups>
          <jobGroup name="JobTransform" parent="Transform"/>
       </jobGroups>
          
    </rtt>
</unifiedTestConfiguration>
