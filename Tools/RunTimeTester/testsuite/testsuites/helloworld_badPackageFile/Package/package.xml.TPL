<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

      <mailto>brinick.simmons@cern.ch</mailto>
      <refRelease>14.5.0</refRelease>

      <jobList>

          <athena>
              <doc>blah</doc>
              <doclink>http://cern.ch</doclink>
              <options>AthExHelloWorld/HelloWorldOptions.py</options>
              <package>Control/AthenaExamples/AthExHelloWorld</package>
              <group>HelloWorld</group>
              <queue>short</queue>
          </athena>

      <jobList> <!-- intentionally error: should be </jobList> -->

      <jobGroups>

         <jobGroup name="AthenaHelloWorld" parent="Athena">
         </jobGroup>

    </jobGroups>

   </rtt>
</unifiedTestConfiguration>
