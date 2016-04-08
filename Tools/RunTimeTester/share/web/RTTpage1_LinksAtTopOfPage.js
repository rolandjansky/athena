
/*********************/
/***** Base URLs *****/
/*********************/

uclHEP    = 'http://www.hep.ucl.ac.uk/atlas/AtlasTesting/';
cernRTT   = 'http://cern.ch/atlasrtt/';
atlasComp = 'http://atlas-computing.web.cern.ch/atlas-computing/';

/*********************/
/***** Style CSS *****/
/*********************/

styleClass  = '<style>';
styleClass += 'a.pageLink{font-family:arial;font-size:10pt;color:#777777;text-decoration:underline}';
styleClass += 'a.pageLink:hover{font-family:arial;font-size:10pt;color:black;text-decoration:none}';
styleClass += '</style>';

/*********************/
/***** The links *****/
/*********************/

link00 = '<a class="pageLink" href="page0.php">All RTT results</a>';
link0 = '<a class="pageLink" href="weeklyTable/weekly.php">Summary of the week</a>';
link1 = '<a class="pageLink" href="https://savannah.cern.ch/bugs/?group=atlas-infra">Report RTT bugs</a>';
link2 = '<a class="pageLink" href="' + uclHEP + '">RTT documentation</a>';
link3 = '<a class="pageLink" href="' + atlasComp + 'projects/releases/releases.php">Release Status</a>';
link4 = '<a class="pageLink" href="' + atlasComp +'links/distDirectory/nightlies/global/">NICOS</a>';
link5 = '<a class="pageLink" href="' + cernRTT + 'DatasetManager.report">Dataset Manager Report</a>';
link6 = '<a class="pageLink" href="' + uclHEP + '../RTTmonolithic/RTTpage1.html">Old RTT monolithic results</a>';
link7 = '<a class="pageLink" href="' + uclHEP + 'DTD/unifiedTestConfiguration.dtd">Package DTD</a>';
link8 = '<a class="pageLink" href="' + uclHEP + 'DTD/RTT.dtd">Top level DTD</a>';


document.write(
   styleClass
 + '<div align="center">'
 + link00 + ' | '
 + link0 + ' | '
 + link1 + ' | '
 + link2 + ' | '
 + link3 + ' | '
 + link4 + ' | '
 + link5 + ' | '
 + link6 + ' | '
 + link7 + ' | '
 + link8
 + '</div><br />' 
);
