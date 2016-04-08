<?xml version="1.0"?>
<!DOCTYPE unifiedTestConfiguration SYSTEM "file:$_BASE_$/DTD/unifiedTestConfiguration.dtd">
                                                                                                                                                            
<unifiedTestConfiguration>
   <atn/>
   <kv/>
   <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfigurationFile">

      <jobGroups>

         <jobGroup name="Top" parent="">
         
            <keepFilePattern info="Shows all the environmental variables set">env.log</keepFilePattern>

            <keepFilePattern info="cmt show uses command on package cmt dir">cmtShowUses.log</keepFilePattern>

            <keepFilePattern info="ls -altF command on run directory before and after athena command">lsaltF.txt</keepFilePattern>

            <keepFilePattern info="Gives a concise report of any problems encountered, checks failed, etc.">failureReport.html</keepFilePattern>

            <keepFilePattern info="The script submitted to the batch machine">runScript.sh</keepFilePattern>

            <keepFilePattern info="All ROOT macro output from your job goes in here">ROOToutput.log</keepFilePattern>

            <keepFilePattern info="Exceptions raised by your Python script go in here">Python_Script_Output.log</keepFilePattern>

            <keepFilePattern info="WARNING,ERROR and FATAL messages from job log">ERROR.log</keepFilePattern>

            <keepFilePattern info="Output from checkFile.py">*.pool.root.checkFile</keepFilePattern>

            <keepFilePattern info="Marker emitted at the end of the job">JobCompleteMarker.txt</keepFilePattern>

            <keepFilePattern info="">DQWebDisplay.py.stdout</keepFilePattern>

            <keepFilePattern info="">*.php</keepFilePattern>

            <keepFilePattern info="">*dcube*</keepFilePattern>

            <keepFilePattern info="">perfMonDozer_*.png</keepFilePattern>

            <keepFilePattern info="">postprocessing.*.log</keepFilePattern>

            <keepFilePattern info="">RTTtests.db</keepFilePattern>
            <keepFilePattern info="">postProcessorScript.sh</keepFilePattern>
            <keepFilePattern info="">postProcessor.log</keepFilePattern>

            <keepFilePattern info="">DCube-*/*</keepFilePattern>
            <keepFilePattern info="">DCube-*/plots/*.png</keepFilePattern>

            <action position="zzzzzzzzzzz">
               <modulename>RttLibraryTools</modulename>
               <testname>CheckFileRunner</testname>
            </action>

            <action position="zzzzzzzzzzzz" runInAtlasEnv="True">
               <modulename>RttLibraryTools</modulename>
               <testname>PerfMonDozer</testname>
            </action>


         </jobGroup>
      
         <jobGroup name="Athena" parent="Top">
             
              <action>
               <modulename>RttLibraryTools</modulename>
               <testname>FileGrepper</testname>
	       <arg>
                <argname>inputFile</argname>
                <argvalue>*_log</argvalue>
               </arg>
	      <arg>
                <argname>outputFile</argname>
                <argvalue>ERROR.log</argvalue>
              </arg>
	      <arg>
                <argname>searchList</argname>
                <argvalue>^(?! \-\+\-).*(WARN|ERROR|FATAL)</argvalue>
              </arg>
           </action>
         
            <logChecker class="SimpleLogChecker">
               <searchString>INFO Application Manager Finalized successfully</searchString>
               <searchString>RTT determined exit status to be:  0</searchString>
               <vetoString>ERROR Terminating event processing loop due to errors</vetoString>
               <vetoString>FATAL Invalid initial state</vetoString>
               <vetoString>job killed</vetoString>
       
              </logChecker>
         </jobGroup>

         <jobGroup name="AthenaATN" parent="Athena">
             <logChecker class="ChainedJobsLogChecker">
               <searchString>INFO Application Manager Finalized successfully</searchString>
               <searchString>RTT determined exit status to be:  0</searchString>
               <vetoString>ERROR Terminating event processing loop due to errors</vetoString>
               <vetoString>FATAL Invalid initial state</vetoString>
            </logChecker>
         </jobGroup>

         <jobGroup name="ErrorGroup" parent="Top" />
         <jobGroup name="ContainerGroup" parent="Top" />

         <jobGroup name="Transform" parent="Top">
             <logChecker class="SimpleLogChecker"> 
                <searchString>----- JobTransform shell script starts  ---------</searchString>
                <searchString>----- JobTransform shell script end  ---------</searchString>
                <searchString>ErrorCode=0 (OK)</searchString>
             </logChecker>
         </jobGroup>

      </jobGroups>
   </rtt>
</unifiedTestConfiguration>
