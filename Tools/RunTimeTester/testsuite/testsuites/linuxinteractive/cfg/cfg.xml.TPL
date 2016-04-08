<?xml version="1.0"?>

<!DOCTYPE rttconfig SYSTEM "file:$_BASE_$/DTD/RTT.dtd">

<rttconfig>
   <mode>LinuxInteractive</mode>
   <release>14.5.0</release>
   <refRelease>14.0.10</refRelease>
   <cmtConfig>i686-slc4-gcc34-opt</cmtConfig>
   <branch>dev</branch>
   <runType><releaseRun/></runType>
   <topProject>AtlasProduction</topProject>
   <distArea>/afs/cern.ch/atlas/software/builds</distArea>
   <workBasePath>$_BASE_$/testsuites/$_TESTNAME_$/Work</workBasePath>
   <resultsBasePath>$_BASE_$/testsuites/$_TESTNAME_$/Results</resultsBasePath>
   <resultsBaseURL>$_BASE_$/testsuites/$_TESTNAME_$/Results</resultsBaseURL>
   <site>CERNslc4</site>
   <dataSetCatalog>$_BASE_$/xml/rttDataSetCatalog.xml</dataSetCatalog>
   <jobGroupConfig>$_BASE_$/xml/TopLevelJobGroup.xml</jobGroupConfig>
   <dCubeCfgFile>$_BASE_$/DCubeClient-00-00-21/python/DCubeForRTTCfg.xml</dCubeCfgFile>

   <confFileSrc>

      <localConfFiles>
         <userHomeCmtDir>$_BASE_$/testsuites/$_TESTNAME_$/cmtHome</userHomeCmtDir>
         <testProjectCmtDir>$_BASE_$/testsuites/$_TESTNAME_$/TestProject/cmt</testProjectCmtDir>
      </localConfFiles>

   </confFileSrc>
      
   <packageAccept>AthExHelloWorld</packageAccept>
      
   <libToolsLoc>$_LIB_TOOLS_FROM_ENV_$</libToolsLoc>
      
   <releaseType>project</releaseType>
   
   <disableMailTo />
   <devRun />
      
   <chainStoreBasePath>$_BASE_$/testsuites/$_TESTNAME_$/ChainStore</chainStoreBasePath>
      
</rttconfig>
