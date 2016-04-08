#!/usr/local/bin/perl
#
#
sub thth {
    my $name = shift;
    my @arr = split("_",$name);
    my $lastel = $#arr;
    return $arr[$lastel];
}

sub yhyh {
    my $name = shift;
    my $tm = (stat("${NICOS_WEBDIR}/$name"))[9];
    return $tm;
}

#
use Env;
use File::Basename;
#@output = ();
my $NICOS_INC_BUILD="$NICOS_INC_BUILD";
my $NICOS_PROJECT_HOME="$NICOS_PROJECT_HOME";
my $NICOS_COPY_HOME="$NICOS_COPY_HOME";
my $NICOS_COPY_BUILD_TYPE="$NICOS_COPY_BUILD_TYPE";
my $releases_loc;
$releases_loc=$NICOS_PROJECT_HOME;
if ( $NICOS_INC_BUILD eq "yes" ) 
{$releases_loc=$NICOS_COPY_HOME;}
my $NICOS_PROJECT_NAME="$NICOS_PROJECT_NAME";
my $NICOS_PROJECT_RELNAME="$NICOS_PROJECT_RELNAME_COPY";
my $NICOS_PROJECT_RELNAME_GEN="$NICOS_PROJECT_RELNAME_GEN";
my $NICOS_PROJECT_RELNUMB="$NICOS_PROJECT_RELNUMB_COPY";
my $NICOS_VERSION_ADM="$NICOS_VERSION_ADM";
my $NICOS_TITLE_COMMENT="$NICOS_TITLE_COMMENT";
my $NICOS_WEBDIR="$NICOS_WEBDIR";
my $NICOS_WEBPAGE_MAIN="$NICOS_WEBPAGE_MAIN";
my $NICOS_WEBPAGE="$NICOS_WEBPAGE";
my $NICOS_WEBPAGE_DEPENDENT="$NICOS_WEBPAGE_DEPENDENT";
my $NICOS_DOMINANT_PROJECTS="$NICOS_DOMINANT_PROJECTS";
$NICOS_DOMINANT_PROJECTS =~ s/\s+/ /g;
$NICOS_DOMINANT_PROJECTS =~ s/^\s+//;
$NICOS_DOMINANT_PROJECTS =~ s/\s+$//;
my $NICOS_DOMINANT_PARALLEL="$NICOS_DOMINANT_PARALLEL";
$NICOS_DOMINANT_PARALLEL =~ s/\s+/ /g;
$NICOS_DOMINANT_PARALLEL =~ s/^\s+//;
$NICOS_DOMINANT_PARALLEL =~ s/\s+$//;

#-> parse arguments
#   print " ---------- $#ARGV $ARGV[0] ";  
    if ($#ARGV != 0) {
    print "nicos_indexgen.pl: one arg req: output file, ";
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

<style> #cellrose { 
background-color: #FFCCCC; 
}
</style>

<head><title>

NICOS project page
</title>
";
my $buildsummary_suffix="_\\d+";
if ( $NICOS_PROJECT_RELNUMB eq "" )
{ $buildsummary_suffix="";}

opendir(WWWS, ${NICOS_WEBDIR});
my @wwwli = sort { yhyh($a) <=> yhyh($b) }
grep { /^nicos_buildsummary${buildsummary_suffix}\.html$/ }
readdir (WWWS);
closedir(WWWS);
my @wwwlis = reverse @wwwli;

my @filelist = ();
if ( "${NICOS_PROJECT_RELNAME_GEN}" ne "WORK_RELEASE" && "${NICOS_PROJECT_RELNAME_GEN}" ne "" )
{ 
opendir(RELEASES, ${releases_loc});
@filelist = sort { thth($a) <=> thth($b) }
       grep { /^${NICOS_PROJECT_RELNAME_GEN}${buildsummary_suffix}/ }
       readdir (RELEASES);
closedir(RELEASES);
}
else
{
@filelist = ("WORK_RELEASE");
}

foreach $wwww (@wwwlis)
{
    my @arw = split("_",$wwww);
    my $lastem = $#arw;
    my $rm=$arw[$lastem];
    my @art = split(/\./,$rm);
    my $rn = $art[0];

foreach $ppp (@filelist)
{
    my @arr = split("_",$ppp);
    my $lastel = $#arr;
    my $relnum=$arr[$lastel];
    my $relnum_suffix = "_$relnum";
    if ( $relnum eq "" ) { $relnum_suffix = ""; }
    if ( $relnum eq "RELEASE" ) { $relnum_suffix = ""; }
    if ( $#arr eq 0 ) { $relnum_suffix = ""; }
    if ( $NICOS_PROJECT_RELNUMB ne "" )
    {
    if ( $relnum == $rn )
{
print WRITEDATA "
<SCRIPT SRC=\"date_rel${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"status_rel${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"copy_status_rel${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
";
}
    }
    else
    {
print WRITEDATA "
<SCRIPT SRC=\"date_rel${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"status_rel${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"copy_status_rel${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
";
    }
}
}

print WRITEDATA "
</head>

<BODY class=body >

<table class=header border=0 cellpadding=5 cellspacing=0 width=\"100%\">
<tr><td bgcolor=\"#CCCFFF\" align=right><font size=\"-1\">
NICOS version $NICOS_VERSION
</font></td></tr>
<tr><td bgcolor=\"#CCCFFF\" align=center>
<br><EM><B><BIG>NICOS (NIghtly COntrol System) project page
<br><br></EM></B></BIG></td></tr>
<tr><td align=center>
<br><BIG><B>Project: $NICOS_PROJECT_NAME <br></BIG>
<font size=\"-1\">\n";
    if ( $NICOS_TITLE_COMMENT ne "" ){
    print WRITEDATA "$NICOS_TITLE_COMMENT <br>\n";
    }
print WRITEDATA " 
Location: ${NICOS_PROJECT_HOME} <br>\n";

if ( $NICOS_COPY_HOME ne "" && $NICOS_COPY_HOME ne $NICOS_PROJECT_HOME && $NICOS_COPY_BUILD_TYPE && "void" ){
print WRITEDATA "
Releases are copied to: ${NICOS_COPY_HOME} \n";
}

print WRITEDATA " 
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
";

my $NICOS_COMMENT_FILE="${NICOS_WEBDIR}/nicos_comment_indexpage";

if ( -f $NICOS_COMMENT_FILE )
{
    open(TTT,"$NICOS_COMMENT_FILE");
while (<TTT>) {    chomp;
    print WRITEDATA "$_ \n";
}
    close(TTT);
}

print WRITEDATA "
<P>
<CENTER><TABLE BORDER=1 CELLSPACING=3 CELLPADDING=2 >
<TR><TH>release name</TH> <TH>status</TH> <TH>date</TH> <TH>copy status</TH> <TH>list of tags</TH>
</TR>
";

opendir(WWWS, ${NICOS_WEBDIR});
my @wwwli = sort { yhyh($a) <=> yhyh($b) }
               grep { /^nicos_buildsummary${buildsummary_suffix}.html$/ }
               readdir (WWWS);
closedir(WWWS);
my @wwwlis = reverse @wwwli;

my @filelis = ();
if ( "${NICOS_PROJECT_RELNAME_GEN}" ne "WORK_RELEASE" && "${NICOS_PROJECT_RELNAME_GEN}" ne "" )
{                                      
opendir(RELEASES, ${releases_loc});
@filelis = sort { thth($a) <=> thth($b) }
              grep { /^${NICOS_PROJECT_RELNAME_GEN}${buildsummary_suffix}/ }
              readdir (RELEASES);
closedir(RELEASES);
}
else
{
@filelis="WORK_RELEASE";
}

#print "XXXT nicos_buildsummary${buildsummary_suffix}\n";
#print "XXX1 @wwwlis \n";
#print "XXX2 @filelis \n";

foreach $wwww (@wwwlis)
{
    my @arw = split("_",$wwww);
    my $lastem = $#arw;
    my $rm=$arw[$lastem];
    my @art = split(/\./,$rm);
    my $rn = $art[0];

foreach $ppp (@filelis)
{
    my @arr = split("_",$ppp);
    my $lastel = $#arr;
    my $relnum=$arr[$lastel];
    my $relnum_suffix = "_$relnum";
    if ( $relnum eq "" ) { $relnum_suffix = ""; }
    if ( $relnum eq "RELEASE" ) { $relnum_suffix = ""; } 
    if ( $#arr eq 0 ) { $relnum_suffix = ""; }
    if ( $NICOS_PROJECT_RELNUMB ne "" )
    { 
    if ( $relnum == $rn )
{
print WRITEDATA "
<TR>
<TD> <a href=\"nicos_buildsummary${relnum_suffix}.html\"><b>${ppp}</b></a> \</TD>
<TD ID=cellrose> <SCRIPT>document.write(status${relnum_suffix}())</SCRIPT> </TD>
<TD ID=cellrose> <SCRIPT>document.write(dddd${relnum_suffix}())</SCRIPT></TD>
<TD ID=cellrose> <SCRIPT>document.write(copy_status${relnum_suffix}())</SCRIPT></TD>
<TD><a href=\"nicos_content${relnum_suffix}.html\">list of tags</a></TD>
</TR>
";
}
    }
    else # if ( $NICOS_PROJECT_RELNUMB ne ""
    {
print WRITEDATA "
<TR>
<TD> <a href=\"nicos_buildsummary${relnum_suffix}.html\"><b>${ppp}</b></a> \</TD>
<TD ID=cellrose> <SCRIPT>document.write(status${relnum_suffix}())</SCRIPT> </TD>
<TD ID=cellrose> <SCRIPT>document.write(dddd${relnum_suffix}())</SCRIPT></TD>
<TD ID=cellrose> <SCRIPT>document.write(copy_status${relnum_suffix}())</SCRIPT></TD>
<TD><a href=\"nicos_content${relnum_suffix}.html\">list of tags</a></TD>
</TR>
";
    }
}
}

print WRITEDATA "
</TABLE></CENTER>
";

if ( $NICOS_COMMON_WEBPAGE ne "" )
{
print WRITEDATA "
<P>
<b> <a href=\"${NICOS_COMMON_WEBPAGE}\"> 
The summary of all available projects</a>.
</b>
";
}

if ( $NICOS_WEBPAGE_MAIN ne "" || $NICOS_DOMINANT_PROJECTS ne "" || $NICOS_DOMINANT_PARALLEL ne "" )
{
print WRITEDATA "
<P>
<b> This project is dependent on other projects: 
";
}

$base_main="";
if ( $NICOS_WEBPAGE_MAIN ne "" )
{
$base_main=basename(${NICOS_WEBPAGE_MAIN});
print WRITEDATA 
"&nbsp;&nbsp;<a href=\"${NICOS_WEBPAGE_MAIN}\">main</a>&nbsp;&nbsp;";
}

if ( $NICOS_DOMINANT_PROJECTS ne "" )
{
@fields=split(/ /,$NICOS_DOMINANT_PROJECTS);
for ($m=0; $m <= $#fields; $m++){
$aaaa=dirname(${NICOS_WEBPAGE});
$bbbb="${aaaa}/nicos_web_area${fields[$m]}";
if ( "$base_main" ne "nicos_web_area${fields[$m]}" ){
print WRITEDATA
"&nbsp;&nbsp;<a href=\"${bbbb}\">${fields[$m]}</a>&nbsp;&nbsp;";        
}
}
} # if ( $NICOS_DOMINANT_PROJECTS ne "" )

if ( $NICOS_DOMINANT_PARALLEL ne "" )
{
@fields=split(/ /,$NICOS_DOMINANT_PARALLEL);
for ($m=0; $m <= $#fields; $m++){
$aaaa=dirname(${NICOS_WEBPAGE});
$bbbb="${aaaa}/nicos_web_area${fields[$m]}";
if ( $base_main ne "nicos_web_area${fields[$m]}" ){
print WRITEDATA
"&nbsp;&nbsp;<a href=\"${bbbb}\">${fields[$m]} (parallel)</a>&nbsp;&nbsp;";
}
}
} # if ( $NICOS_DOMINANT_PARALLEL ne "" )

if ( $NICOS_WEBPAGE_MAIN ne "" || $NICOS_DOMINANT_PROJECTS ne "" || $NICOS_DOMINANT_PARALLEL ne "" )
{
print WRITEDATA "
<BR><BR>
<p>
";
}
 
if ( $NICOS_WEBPAGE_DEPENDENT ne "" )
{
print WRITEDATA "
<P>
<b> This project has the dependent project with
<a href=\"${NICOS_WEBPAGE_DEPENDENT}\"> this web page </a>.
</b>
";
}

print WRITEDATA "
<BR><BR>
<P>
<b>This page</b>
<UL>
<LI>shows the status of nightly builds of $NICOS_PROJECT_NAME project
managed by the
<a href=http://www.usatlas.bnl.gov/computing/software/nicos>NICOS system</a>
<LI>provides a link to the Project Configuration Page
";

if ( $NICOS_VERSION_ADM ne "NONE" && $NICOS_VERSION_ADM ne "" )
{
print WRITEDATA "
<LI>provides a link to the Version Administrator Page (this page may not work
with older browsers, such as Netscape 4)
";
}

print WRITEDATA "
</UL>
<p><b>The table above</b>
<UL>
<LI> contains the links to pages with build summaries and
version contents of nightly releases
<LI> informs about the date and status of builds
(could be completed or in progress)
<LI> shows all releases in the cycle of nightly builds.
The lifetime of releases (in days) is equal to number of
releases in the table.
</UL>
<BR>
";

print WRITEDATA "
<P>

<BR>

<CENTER><FORM METHOD=\"GET\" ACTION=\"nicos_configuration.html\">
<INPUT Type='submit' Value='View Project Configuration' style='background:#99CCFF'>
</FORM></CENTER>
";

if ( $NICOS_VERSION_ADM ne "NONE" && $NICOS_VERSION_ADM ne "" )
{ 
print WRITEDATA"
<CENTER><FORM METHOD=\"GET\" ACTION=\"${NICOS_VERSION_ADM}\">
<INPUT Type='submit' Value='Open Version Admnistrator' style='background:\#99CCFF'>
</FORM></CENTER>
";
}

print WRITEDATA"
<BR>
<hr align=left width=\"90%\">
<address><a href=\"mailto:undrus\@bnl.gov\">Send comments or questions about NICOS</a></address>
<p>
</body>
</html> 
";








