<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">

<unifiedTestConfiguration>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">

      <mailto>brinick.simmons@cern.ch</mailto>
      <refRelease>14.5.0</refRelease>

      <jobList>

         <chain>
            <chainName>SimpleMergeJob</chainName>
            <sequential>
               <parallel>

                  <chainElement>
                      <jobTransform>
                          <doc>MyJob</doc>
                          <jobTransformJobName>SimpleJob1</jobTransformJobName>
                          <jobTransformCmd>echo "1 2 3 4 5 6 7 8 9 10" &gt;&amp; simple1.data;echo "ErrorCode=0 (OK)";</jobTransformCmd>
                          <group>JobTransform</group>
                          <keepFilePattern>simple1.data</keepFilePattern>
                      </jobTransform>

                      <chainfileout>simple1.data</chainfileout>
                  </chainElement>

                  <chainElement>
                       <jobTransform>
                          <doc>MyJob</doc>
                          <jobTransformJobName>SimpleJob2</jobTransformJobName>
                          <jobTransformCmd>echo "11 12 13 14 15 16 17 18 19 20" &gt;&amp; simple2.data;echo "ErrorCode=0 (OK)";</jobTransformCmd>
                          <group>JobTransform</group>
                          <keepFilePattern>simple2.data</keepFilePattern>
                      </jobTransform>

                      <chainfileout>simple2.data</chainfileout>
                  </chainElement>
               </parallel>
               <chainElement>

                  <jobTransform>
                     <doc>MyJob</doc>
                     <jobTransformJobName>SimpleMergeJob0</jobTransformJobName>
                     <jobTransformCmd>cat simple1.data simple2.data &gt;&amp; simple.merged.data;echo "ErrorCode=0 (OK)";</jobTransformCmd>
                     <group>JobTransform</group>

                     <chaindataset_info>
                        <jobTransformData/>
                        <chaindatasetName>simple1.data</chaindatasetName>
                        <dataset_info>
                           <jobTransformData/>
                           <datasetName>$_BASE_$/data/$_TESTNAME_$/simple1.data</datasetName>
                        </dataset_info>
                     </chaindataset_info>

                     <chaindataset_info>
                        <jobTransformData/>
                        <chaindatasetName>simple2.data</chaindatasetName>
                        <dataset_info>
                           <jobTransformData/>
                           <datasetName>$_BASE_$/data/$_TESTNAME_$/simple2.data</datasetName>
                        </dataset_info>
                     </chaindataset_info>
                     <keepFilePattern>simple.merged.data</keepFilePattern>
                  </jobTransform>


               </chainElement>

            </sequential>
         </chain>

       </jobList>
          
       <jobGroups>
          
           <jobGroup name="JobTransform" parent="Transform">

           </jobGroup>          

       </jobGroups>
          
    </rtt>
</unifiedTestConfiguration>
