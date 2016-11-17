<!-- emacs: this is -*- html -*- -->
<!-- <h2>other releases</h2> -->
<?php 

print("<table>\n");
tableTitle("", "&nbsp;other releases" );
print("<tr><td>\n<ul>\n");


 $index_page="TrigInDetValidation/$job/TIDAWeb";

$releases = array( 
 "20.11.X.Y-VAL/build/x86_64-slc6-gcc49-opt/p1hlt",
 "20.11.X.Y.Z-VAL/build/x86_64-slc6-gcc49-opt/AtlasCAFHLT",
 "20.7.X-VAL/build/x86_64-slc6-gcc49-opt/offline",
 "20.7.X.Y-VAL/build/x86_64-slc6-gcc49-opt/offline",
 "21.0.X-VAL/build/x86_64-slc6-gcc49-opt/offline",
 "21.0.X/build/x86_64-slc6-gcc49-opt/offline",
 "dev/build/x86_64-slc6-gcc49-opt/offline",
 "devval/build/x86_64-slc6-gcc49-opt/offline" );


for ( $i=0 ; $i<count($releases) ; $i++ ) { 
   $proj  = basename($releases[$i]);
   $rtags = explode("/",$releases[$i]);
   $bran  = $rtags[0];
   $display_project = "AtlasProduction"; 
   if ( $proj == "p1hlt" ) $display_project = "AtlasP1HLT";
   print( "<li><a href=\"$release_dir/$releases[$i]/$index_page\"><span>$bran</span></a> $display_project </li>\n");
}

?>

</ul>
</td></tr>
</table>
