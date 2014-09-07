<!-- emacs: this is -*- html -*- -->

<?php


// Get plot roots
$dir = $time_dir;
if (is_dir($dir)) {
  $files = scandir($dir);
  foreach ($files as &$file) $file = "$file";
  $plotroots = findPdfPngFileRoots($files);
}


// Generate table
if (count($plotroots) !== 0) {

  echo "<table>\n";
  tableTitle("Latencies", "Latencies", "");

  for ($i = 0, $numplots = count($plotroots); $i <= $numplots; $i += 2) {
    if ($i !== $numplots-1) tableRow(       "$time_dir", $plotroots[$i], "",  $plotroots[$i+1], "" );  
    else                    tableRowSingle( "$time_dir", $plotroots[$i], "" );  
  }
  echo "</table>\n";

}
else {
  print( "<big>No latency plots produced today :(</big>" );
}
?>
