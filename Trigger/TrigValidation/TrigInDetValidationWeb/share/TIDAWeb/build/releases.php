<!-- emacs: this is -*- html -*- -->
<!-- <h2>other releases</h2> -->
<?php 

print("<table>\n");
tableTitle("", "&nbsp;other releases" );
print("<tr><td>\n<ul>\n");


 $index_page="TrigInDetValidation/$job/TIDAWeb";

$releases = array( 
 "17.3.X.Y-VAL/build/i686-slc5-gcc43-opt/offline", 
 "19.0.X.Y-VAL/build/x86_64-slc6-gcc47-opt/p1hlt",
 "19.0.X.Y-VAL2/build/x86_64-slc6-gcc47-opt/p1hlt",
 "19.1.1.Y-VAL/build/x86_64-slc6-gcc47-opt/offline",
 "19.1.1.Y-VAL/build/x86_64-slc6-gcc47-opt/offline",
 "19.1.1.X.Y-VAL/build/x86_64-slc6-gcc47-opt/p1hlt",
 "dev/build/x86_64-slc6-gcc47-opt/offline",
 "devval/build/x86_64-slc6-gcc47-opt/offline",
 "mig8/build/x86_64-slc6-gcc47-opt/offline" );


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
