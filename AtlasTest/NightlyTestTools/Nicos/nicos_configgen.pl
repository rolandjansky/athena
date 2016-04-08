#!/usr/local/bin/perl
#
#
#
use Env;

my $NICOS_PROJECT_HOME="$NICOS_PROJECT_HOME";
my $NICOS_PROJECT_NAME="$NICOS_PROJECT_NAME";
my $NICOS_PROJECT_RELNAME="$NICOS_PROJECT_RELNAME";
my $NICOS_CONFIG_AREA="$NICOS_CONFIG_AREA";
my $cachefile="${NICOS_CONFIG_AREA}/nicos_cache";

#-> parse arguments
#   print " ---------- $#ARGV $ARGV[0] ";  
    if ($#ARGV != 0) {
    print "nicos_configgen.pl: one arg req: output file, ";
    exit 1;}

    my $outfile = "$ARGV[0]";

    open(WRITEDATA,">$outfile");

    print WRITEDATA "
<html>
<style> table.header {
  background: #99CCFF;
  color: #CC0000;
} </style>

<style> body {
  color: black;
  link: navy;
  vlink: maroon;
  alink: tomato;
  background: cornsilk;
  } </style>

<head><title>

NICOS project configuration page
</title></head>

<BODY class=body >

<table class=header border=0 cellpadding=5 cellspacing=0 width=\"100%\">
<tr><td bgcolor=\"#CCCFFF\" align=right><font size=\"-1\">
NICOS version $NICOS_VERSION
</font></td></tr>
<tr><td bgcolor=\"#CCCFFF\" align=center>
<br><EM><B><BIG>NICOS (NIghtly COntrol System) configuration page
<br><br></EM></B></BIG></td></tr>
<tr><td align=center>
<br><BIG><B>Project: $NICOS_PROJECT_NAME<br></BIG>
<font size=\"-1\"> 
Location: $NICOS_PROJECT_HOME
</font><br>
</td></tr>

<tr class=light><td colspan=2 align=right colspan=1><font size=\"-1\">
<script language=\"JavaScript\">
    <!-- Hide script from old browsers
    document.write(\"Last modified \"+ document.lastModified)
    // end of script -->
    </script>
</font></td></tr></table>

<!--------------------------- Contents --------------------------->
<p>
<b>This page</b> 
<UL>
<LI>shows the configuration parameters of the Project for each stage of 
the 
<a href=http://www.usatlas.bnl.gov/computing/software/nicos>NICOS</a> 
building process.
<LI>provides a link for editing the configuration of the Project 
</UL>
<P>
<PRE>
";

open(FILE,"<$cachefile");
$nrec = 0;
while (<FILE>) {
    $nrec++;
    chomp;
    s/</&lt;/;
    s/>/&gt;/;
    $nbgt = $_;
    $bgth = $nbgt;
    $bgth =~ s/ //g;
    if ( length($bgth) != 0 ){ 
	print WRITEDATA "$nbgt \n";}
}
close FILE;

print WRITEDATA "
</PRE>
<BR>
<P>
<CENTER><FORM>
<INPUT Type='button' Value='Edit Project Configuration' style='background:#99CCFF' onClick=\"confirm('web configuration editing is not available in NICOS ${NICOS_VERSION} \\n please manually edit file nicos_cache in NICOS home area')\" >
</FORM></CENTER>

<BR>
<hr align=left width=\"90%\">
<a href=\"index.html\">Back to NICOS project page</a>
<p>
</body>
</html>

";








