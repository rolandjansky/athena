<?php
/**
  * @file rainbowWarrior.php
  * @author Krzysztof.Ciba@NOSPAMagh.edu.pl
  * @date July 31, 2007
  * @brief log file coloriser
  *
  * This is a part of DCube package - offline monitoring of root histograms.
  * Copyleft (C) 2007  Krzysztof Ciba
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program; if not, write to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
  *
  * If something goes wrong during run of dcube, please read again paragraph 
  * about WARRANTY...
  */


ob_start ("ob_gzhandler");
header("Content-type: text/html; charset: UTF-8");
header("Cache-Control: must-revalidate");
$offset = 60 * 60  * 12;
$ExpStr = "Expires: " . 
  gmdate("D, d M Y H:i:s",
	 time() + $offset) . " GMT";
header($ExpStr);

set_time_limit(300);

  /**
   * @class rainbow
   * @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
   * @brief <i>Rainbow Warrior</i>, log file coloriser  
   * 
   */
class rainbow {

  var $ln = 1;

  var $lcount = array( "VERBOSE"  => 0, 
		       "DEBUG"    => 0, 
		       "INFO"     => 0, 
		       "WARNING"  => 0, 
		       "ERROR"    => 0, 
		       "FATAL"    => 0, 
		       "CRITICAL" => 0, 
		       "DEFAULT"  => 0 ); 


  function rainbow( $log ) {

    echo  "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" ".
      "\"http://www.w3.org/TR/1999/REC-html401-19991224/strict.dtd\">\n\n";
    echo "<html>\n";

    $this->header();

    $this->body( $log );

    echo "</html>\n";

  }
 
  function header( ) {

    echo "<head>\n";

    echo "<title>RainbowWarrior LogBook</title>\n";
    echo "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n";
    echo "<meta name=\"description\" content=\"displaying log files as html\">\n";
    echo "<meta name=\"keywords\" content=\"Gaudi Athena log file colour\">\n";
    echo "<meta http-equiv=\"content-language\" content=\"en\">\n";
    echo "<meta name=\"generator\" content=\"GNU Emacs 21.3.1\">\n";
    echo "<meta name=\"author\" content=\"Krzysztof Daniel Ciba\">\n";

    echo "<style>\n";
    echo "<!--\n";

    echo "body { background-color: #e6e6e6; font-family: Courier, serif; font-size: 12pt; ";
    echo " font-weight: normal; height: 100%; }\n";
    echo "span, div   { width: 100%;  }\n";
    echo "a { color: #0033ff; }\n";
    echo "a:visited {color: #ff0000;  }\n";
    echo "div.log     { padding-bottom: 95px; }\n";
    echo "div.bottom  { font-weight: bold; height: 90px; color: #ffffff; background-color: #003399; ";
    echo "position: fixed; bottom: 0px; left: 0px; text-align: center; }\n"; 
    echo ".v { color: #999999; white-space: pre; }\n";
    echo ".d { color: #333333; white-space: pre; }\n";
    echo ".i { color: #000033; font-weight: bold; white-space: pre; }\n";
    echo ".w { color: #ff6600; font-weight: bold; white-space: pre; }\n";
    echo ".e { color: #cc0000; font-weight: bold; white-space: pre; }\n"; 
    echo ".f { color: #990000; font-weight: bold; white-space: pre; }\n";
    echo ".c { text-align: right; }\n";
    echo "-->\n";
    echo "</style>\n";

    echo "</head>\n";    
  }

  function body( $log ) {

    $handle = fopen( $log, "r" );
   
    if ( ! $handle ) { 
      
      die( "Cannot open and read file " . $log . "\n" );
      
    } else {
    
      echo "<div class=\"log\">\n";
      echo "<table>\n";
      while ( ! feof( $handle ) ) {

	$buffer = fgets( $handle ); 
	$buffer = htmlentities(rtrim( $buffer ));

	if ( $buffer != "" ) {
	  echo "<tr><td class=\"c\"><a href=\"#".$this->ln."\" name=\"".$this->ln."\">"
	    .$this->ln++."</a>&nbsp;</td><td>";
	  if ( ereg( "VERBOSE", $buffer ) )  { 
	    echo "<span class=\"v\">".$buffer."</span></td></tr>\n";
	    $this->lcount["VERBOSE"]++;
	  } else 
	    if ( ereg( "DEBUG", $buffer ) ) {
	      echo "<span class=\"d\">".$buffer."</span></td></tr>\n";
	      $this->lcount["DEBUG"]++;
	    } else 
	      if ( ereg( "INFO", $buffer ) ) { 
		echo "<span class=\"i\">".$buffer."</span></td></tr>\n";
		$this->lcount["INFO"]++;
	      } else 
		if ( ereg( "WARNING", $buffer ) ) {
		  echo "<span class=\"w\">".$buffer."</span></td></tr>\n";
		  $this->lcount["WARNING"]++;
		} else 
		  if ( ereg( "ERROR", $buffer ) ) {
		    echo "<span class=\"e\">".$buffer."</span></td></tr>\n";
		    $this->lcount["ERROR"]++;
		  } else 
		    if ( ereg("FATAL|CRITICAL|Traceback", $buffer ) ) { 
		      echo "<span class=\"f\">".$buffer."</span></td></tr>\n";
		      $this->lcount["FATAL"]++;
		    } else {
		      echo "<span>".$buffer."</span></td></tr>\n";
		      $this->lcount["DEFAULT"]++;
		    } 
	}
      }	
      echo "</table>\n";
      echo "</div>\n";


      echo "<div class=\"bottom\">";
      echo "<br>Number of lines with:<br>";
      echo "<span class=\"v\">VERBOSE = </span>".$this->lcount["VERBOSE"]." | \n";
      echo "<span class=\"d\">DEBUG = </span>".$this->lcount["DEBUG"]."| \n";
      echo "<span class=\"i\">INFO = </span>".$this->lcount["INFO"]." | \n";
      echo "<span class=\"w\">WARNING = </span>".$this->lcount["WARNING"]." | \n";
      echo "<span class=\"e\">ERROR = </span>".$this->lcount["ERROR"]." | \n";
      echo "<span class=\"f\">FATAL/CRITICAL/Traceback = </span>".$this->lcount["FATAL"]." | \n";
      echo "<span class=\"v\">OTHERS = </span>".$this->lcount["DEFAULT"]."<br> \n";
      echo "<a href=\"".$log."\" target=\"_blank\" style=\"color: #ffcc00\" >click here to download log";
      echo " file as plain text</a><br>\n";

      echo "</div>\n";
   }
    
    fclose( $handle );
  }
  
}


?>