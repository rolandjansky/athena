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

sub yhyg {
    my $name = shift;
    my $tm = (stat("${NICOS_COMMON_WEBDIR}/$name"))[9];
    return $tm;
}

sub yhyc {
    my $name = shift;
    my $file = "${NICOS_COMMON_WEBDIR}/$name";
    open (FK, "$file");
    my $line = readline(FK);
    $line =~ s/<[^>]*>//gs;
    if (not $line)
    {$line="99999999999";}
#    print "SSS", $line, "\n";
    close (FK);
    return $line;
}

#
use Env;
use File::Basename;
#@output = ();
my $NICOS_VERSION="$NICOS_VERSION";
my $NICOS_WEBDIR="$NICOS_WEBDIR";
my $NICOS_WEBPAGE="$NICOS_WEBPAGE";
my $NICOS_SUFFIX="$NICOS_SUFFIX";
my $NICOS_WORK_AREA="$NICOS_WORK_AREA";
my $NICOS_COMMON_WEBDIR="$NICOS_COMMON_WEBDIR";
my $file_project_title_pf="nicos_project_title";
my $prepage="${NICOS_WORK_AREA}/nicos_prepage";
my $testprepage="${NICOS_WORK_AREA}/nicos_testprepage";
#
#-> parse arguments
#   print " ---------- $#ARGV $ARGV[0] \n";  
    if ($#ARGV != 0) {
    print "nicos_common_indexgen.pl: one arg req: output file, \n";
    exit 1;}

    my $outfile = "$ARGV[0]";

my $ecount=0;
if ( -f ${prepage} ){
open(FL,"<$prepage");
while (<FL>) {
    chomp;
    ( $lin1 = $_ ) =~ s/ //g;
    next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
    $lll = join(' ', split(' ', $_));
    @fields = split(" ", $lll);
    if ( $fields[3] ge 2 ){ $ecount++; }
#    print "FGFGFG $fields[3] $ecount\n";
}
close(FL);
} # if ( -f ${prepage} )

#print "DDDD $ecount\n";
my $testecount=0;
my $testscount=0;
my $testsuccess=100;
if ( -f ${testprepage} ){
open(FL,"<$testprepage");
$scount=0;
while (<FL>) {
    chomp;
    ( $lin1 = $_ ) =~ s/ //g;
    next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
    $lll = join(' ', split(' ', $_));
    @fields = split(" ", $lll);
    if ( $fields[3] ge 2 ){ $testecount++; }
    else { $testscount++; }
}
$testcount = $testscount + $testecount;
if ( $testcount == 0 ){
$testsuccess="N/A";
}
else{
$testsuccess=int(($testscount/$testcount)*100);
}
close(FL);
} # if ( -f ${testprepage} )

$ofl="${NICOS_COMMON_WEBDIR}/nicos_build_failures${NICOS_SUFFIX}";
open(FL,">$ofl");
print FL "$ecount";
close(FL);

$ofl="${NICOS_COMMON_WEBDIR}/nicos_test_success${NICOS_SUFFIX}";
open(FL,">$ofl");
print FL "$testsuccess";
close(FL);
 
opendir(WWD, ${NICOS_COMMON_WEBDIR});
#my @titleli = sort { yhyg($a) <=> yhyg($b) }
my @titleli = sort { yhyc($a) cmp yhyc($b) }
grep { /^${file_project_title_pf}(.+[^~])?$/ }
readdir (WWD);
closedir(WWD);
#my @titlelis = reverse @titleli;
my @titlelis = @titleli;
#print " CC @titlelis \n";

@suffixlis=(); 
foreach $title (@titlelis)
{
    ( $sf = $title ) =~ s!^${file_project_title_pf}!!;
    push(@suffixlis, $sf);
}

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
} </style>
<style> #cellrose20 {
background-color: #FFCCCC;
width: 20%
} </style>
<style> #cellyellow {
background-color: #FFFF99;
} </style>
<style> #cellgreen {
background-color: #66FF33;
} </style>
<style> #cellred {
background-color: #FF9933;
} </style>
<style> #cellcream {
background-color: cornsilk;
} </style>
<head><title>

NICOS common page
</title>
</head>
<BODY class=body >
";

print WRITEDATA "
<table class=header border=0 cellpadding=5 cellspacing=0 width=\"100%\">
<tr><td bgcolor=\"#CCCFFF\" align=right><font size=\"-1\">
NICOS version $NICOS_VERSION
</font></td></tr>
<tr><td bgcolor=\"#CCCFFF\" align=center>
<br><EM><B><BIG>NICOS GLOBAL LIST OF PROJECTS
<br><br></EM></B></BIG></td></tr>

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

my $COMMENT_FILE="${NICOS_COMMON_WEBDIR}/nicos_comment_indexpage";
if ( -f $COMMENT_FILE )
{
    open(TTT,"$COMMENT_FILE");
    while (<TTT>) {    chomp;
                       print WRITEDATA "$_ \n";
                   }
    close(TTT);
}

print WRITEDATA "
<BR>
<P>
<CENTER><TABLE BORDER=1 CELLSPACING=3 CELLPADDING=2 WIDTH=\"90%\">
<TR><TH>Project Title</TH> <TH>NICOS Suffix</TH> <TH>Latest Release</TH> <TH>Status</TH> <TH>Date</TH> <TH>Failed Builds</TH> <TH>Test Success(%)</TH>
</TR>
";
#
our $latest;
our $latest_status;
our $build_failures;
our $test_success;
our $project_title;
our $latest_date;
foreach $sf (@suffixlis)
{
$latest="unavailable";
$latest_status="unknown";
$build_failures="N/A";
$test_success="N/A";
$project_title="N/A";
$latest_date="N/A";
foreach $nm ("project_title", "latest", "latest_status", "build_failures", "test_success", "latest_date")
{
    $filea="${NICOS_COMMON_WEBDIR}/nicos_${nm}${sf}";
    open (FF,"<$filea");
    while(<FF>){
    chomp;
    ${$nm}=$_;
    }
}

my $idbuild="cellgreen";
if ( ${build_failures} > 3 ) { $idbuild="cellyellow";}
if ( ${build_failures} > 9 ) { $idbuild="cellred";}
if ( ${build_failures} eq "" || ${build_failures} eq "N/A" ) 
{ $idbuild="cellcream";}
my $idtest="cellgreen";
if ( ${test_success} < 59 ) { $idtest="cellyellow";}
if ( ${test_success} < 32 ) { $idtest="cellred";}
if ( ${test_success} eq "" || ${test_success} eq "N/A" ) 
{ $idtest="cellcream";}

#print "FGFGFG ${NICOS_WEBPAGE}\n";
print WRITEDATA "
<TR>
<TD ID=cellrose20>${project_title}</TD>
<TD>${sf}</TD>
<TD>${latest}</TD>
<TD ID=cellrose>${latest_status}</TD>
<TD ID=cellrose>${latest_date}</TD>
<TD ID=${idbuild}>${build_failures}</TD>
<TD ID=${idtest}>${test_success}</TD>
</TR>
";
} #foreach $sf (@suffixlis)

print WRITEDATA "
</TABLE></CENTER>
";

print WRITEDATA "
<P>
<UL>
<LI> <b>This page</b> shows the list of nightly projects 
managed by the
<a href=http://www.usatlas.bnl.gov/computing/software/nicos>NICOS system</a>
<LI> Click on the project title to get the project web page with the list of all releases available
<LI> Click on the latest release name to get the summary of build and test results
";

print WRITEDATA"
<BR>
<hr align=left width=\"90%\">
<address><a href=\"mailto:undrus\@bnl.gov\">Send comments or questions about NICOS</a></address>
<p>
</body>
</html> 
";








