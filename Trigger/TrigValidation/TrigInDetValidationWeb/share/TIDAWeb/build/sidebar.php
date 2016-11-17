<!-- emacs: this is -*- html -*- --> 
<!-- sidebar -->
<!-- <td width="40%" cellpadding="0px" border="0" frame="void" rules="none" > -->
<td>
<div id="sidebar">
  <ul>
     <li>
       <p />
       <h2>RTT overview</h2>
       <br />  
       <ul>
         <li><a href="index.php"><span> TIDValidation overview for this nightly</span></a></li>
         <?php print ( "<li><a href=\"$job_base\"><span> RTT file view page for this job </span></a></li>\n" ); 
         //	 <li style="margin-bottom:-3em">&nbsp;</li>
	 checkPlotType();      
	 print( "<li><span>\n" );
         for ( $i=0 ; $i<7 ; $i++ ) {
            if ( "rel_$i" != "$release" ) { 
              print ( "<a href=\"$rtt_base/rel_$i/$branch/build/$arch/$project/$package/$job/TIDAWeb/$page\">rel_$i</a>\n");
            }
            else {
              print ( "rel_$i\n");
            }
         }     
         print ( "</span></li>\n" );
	 print ( "<li style=\"margin-bottom:-3em\">&nbsp;</li>" );
         print ( "<li><a href=\"$base_page\"><span>All RTT jobs for <b>$branch-$release</b></span></a></li>\n" ); 
	 print ( "<li><a href=\"$big_base_page\"><span>All RTT jobs for all <b>$branch</b> nightlies</span></a></li>\n" );
         ?> 
         <li><a href="https://atlas-rtt.cern.ch/index.php?q=%28packagename=TrigInDetValidation;branch=*;verbosity=vv;%29"><span>All ID trigger RTT results</span></a></li> 
	 <!--<li><a href="https://atlas-rtt.cern.ch/index.php"><span>RTT home page</span></a></li>-->
       </ul>  
       
       <h2>Inner Detector RTT jobs</h2>
       <br/>  
       <ul>
        <!-- print all the different tests from this release -->
        <?php 
	
          $job_types = array( "bjet_", "el_", "mu_", "tau_", "all_", "beamspot_", "minBias", "cosmic" );

          $dirs  = scandir("../../");
//        $plots = scandir("$plot_directory");

          foreach ($job_types as &$job_type) { 
	   
            foreach ($dirs as &$dir) { 

	      if ( strpos($dir,$job_type) != true  ) continue;
//	      if ( !(strpos($dir,'run2') == true ) && strpos($dir,'cosmic') != true  && ( strpos($dir,'minBias') != true ) ) continue;  
//	      if ( !(strpos($dir,'run2') == true && strpos($dir,'_pu') == true ) && strpos($dir,'cosmic') != true  && strpos($dir,'minBias') != true  ) continue;
	      if (   strpos($dir,'run1') == true   ) continue; 
	      if (   strpos($dir,'merge') == true   ) continue; 

  	      if (   strpos($dir,'el_single_e_7-80_pu40') == true  && !( strpos($dir,'IBL') == true || strpos($dir,'large') == true ) ) continue;
//	      if (   strpos($dir,'Jpsi') == true   ) continue;  
 	      if (   strpos($dir,'single_e_5_run2') == true ) continue;
   
	      if ( $dir != "." && $dir != ".." ) {  
//               $long_dir      = "$package_dir/$dir/TIDAWeb";
                 $short_jobname = str_replace( "TrigInDetValidation_", "", $dir );

                 if ( file_exists( "../../$dir/TIDAWeb" ) ) { 
//	            print( "\t\t<li><a href=\"$long_dir/$page\"><span> TIDV  $short_jobname</span></a> </li>\n" );
	            print( "\t\t<li><a href=\"../../$dir/TIDAWeb/$page\"><span> TIDV  $short_jobname</span></a> </li>\n" );
                 }
                 else {
                    print( "\t\t<li><span> TIDV  $short_jobname</span></li>\n" );
                 } 
              }
	    }
            
            if ( strpos($job_type,"cosmic") != true ) print( "\t<li style=\"margin-bottom:-3em\">&nbsp;</li>\n"); //print("\t<br/>\n");

          }
 
	  ?>
       </ul>

    </li>
 
  </ul>
</div>
</td>
<!-- end #sidebar -->
