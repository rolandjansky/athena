<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<?php include("https://gartoni.web.cern.ch/gartoni/menu.php"); ?>
<div id="center">
    <?php
      $path = $path . $_SERVER['REQUEST_URI'];
      $files = glob( "./" . "*" );
      $afspath = getcwd();
      print "<h2><b>Contents of $afspath</b></h2>";
      //print "<table class=\"sortable\" id=\"sortabletable\">";
      print "<table>";
      print "<tr><th><img src=\"https://gartoni.web.cern.ch/gartoni/images/blank.gif\" alt=\"Folder\" height=18></th><th><b>File Name</b></th><th><b>Size</b></th><th><b>Last Modification</b></th></tr>";
      print "<tr><td><img src=\"https://gartoni.web.cern.ch/gartoni/images/Folder_icon.png\" alt=\"Folder\" height=18></td><td><a href=\"$path$file..\">Parent Directory</a></td><td></td><td></td></tr>";
      foreach( $files as $file ) {
        $file = str_replace( "./", "", $file );
        if ( $file != "index.php" ) {
          print "<tr>";
          if( filetype( $file ) == "dir" ) { 
            print "<td><img src=\"https://gartoni.web.cern.ch/gartoni/images/Folder_icon.png\" alt=\"Folder\" height=18></td>";
          }
          else if( isImage( $file ) ) {
            print "<td><img src=\"https://gartoni.web.cern.ch/gartoni/images/preview_icon.jpeg\" alt=\"Image File\" height=18></td>";
          }
          else if( isTextFile( $file ) ) {
            print "<td><img src=\"https://gartoni.web.cern.ch/gartoni/images/text_icon.png\" alt=\"Text File\" height=18></td>";
          }
          else {
            print "<td><img src=\"https://gartoni.web.cern.ch/gartoni/images/blank.gif\" alt=\"File\" height=18></td>";
          }
          // Link
          print "<td><a href=\"$path$file\">$file</a></td>";
          // Size
          $fileSize = filesize( $file );
          print "<td>" . formatBytes( $fileSize ) . "</td>";
          // Type
          //print "<td>" . filetype( $file ) . "</td>";
          // Last Modification
          print"<td>" . date( "M j, Y - g:i a", filemtime( $file ) ) . "</td>";
          // Last Access 
          //print"<td>" . date( "M j, Y - g:i a", fileatime( $file ) ) . "</td>";
          print "</tr>";
        }
      }
      print "</table>";
      function formatBytes( $size, $precision = 2 ) {
        $base = log( $size ) / log( 1024 );
        $suffixes = array( '', 'k', 'M', 'G', 'T' );   
        if( floor( $base ) > 1 ) $precision = 1;
        else $precision = 0;
        return round( pow( 1024, $base - floor( $base ) ), $precision ) . $suffixes[ floor( $base ) ];
      }
      function isImage( $string ) {
        $extensions = array( '.png', '.pdf', '.eps', '.gif', '.tiff', '.jpeg', '.jpg' );
        foreach( $extensions as $extension ) {
          $pos = strpos( $string, $extension );
          if( $pos == true ) {
            return true;
          }
        }
        return false;
      }
      function isTextFile( $string ) {
        $extensions = array( '.html', '.php', '.py', '.sh', '.cpp', '.h', '.C', '.txt', '.cxx' );
        foreach( $extensions as $extension ) {
          $pos = strpos( $string, $extension );
          if( $pos == true ) {
            return true;
          }
        }
        return false;
      }
    ?>
</div>
<div id="footer">
	<p></p>
</div>
</body>
</html>
