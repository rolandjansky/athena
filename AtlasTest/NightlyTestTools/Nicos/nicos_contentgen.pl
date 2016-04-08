#!/usr/local/bin/perl
#
#
#
use Env;
use File::Basename;
#@output = ();
my $NICOS_PROJECT_HOME="$NICOS_PROJECT_HOME";
my $NICOS_PROJECT_NAME="$NICOS_PROJECT_NAME";
my $NICOS_PROJECT_RELNAME="$NICOS_PROJECT_RELNAME_COPY";
my $NICOS_DBFILE="$NICOS_DBFILE";
my $NICOS_WORK_AREA="$NICOS_WORK_AREA";
#-> parse arguments
#   print " ---------- $#ARGV $ARGV[0] ";  
    if ($#ARGV != 0) {
    print "nicos_indexgen.pl: one arg req: output file, ";
    exit 1;}

    my $base = basename($NICOS_DBFILE);
    my $NICOS_DBFILE_GEN = "${NICOS_WORK_AREA}/${base}_gen_res";
    my $NICOS_DBFILE_GEN_PREV = "${NICOS_WORK_AREA}/${base}_gen_res_prev";

    my $outfile = "$ARGV[0]";

my %dbf_prev = ();
my %dbf = ();
my @dbf_sam = ();
my @dbf_new = ();
my @dbf_newnew = ();
my $prev_exist=0;

if ( -f "$NICOS_DBFILE_GEN_PREV" ){ 
$prev_exist=1;
open(TTT,"$NICOS_DBFILE_GEN_PREV") 
or die "nicos_contentgen.pl: previous versions files does not exist: ${NICOS_DBFILE_GEN_PREV}";   
while (<TTT>) {
    chomp;
    next if ($_ =~ /^#/ );
             ( $lin1 = $_ ) =~ s/ //g;
             next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
             $lll = join(' ', split(' ', $_));
             @fields = split(" ", $lll);
             $package1=$fields[0];
             $version1=$fields[1];
             $dbf_prev{$package1} = "$version1";
	 }
close(TTT);
}


open(TTT,"$NICOS_DBFILE_GEN")
    or die "nicos_contentgen.pl: couldn't open $NICOS_DBFILE_GEN for reading";
while (<TTT>) {
    chomp;
    next if ($_ =~ /^#/ );
             ( $lin1 = $_ ) =~ s/ //g;
             next if (length($lin1) eq 0);
#   remove trailing leading stretches whitespaces
             $lll = join(' ', split(' ', $_));
             @fields = split(" ", $lll);
             $package=$fields[0];
             $version=$fields[1];
             $dbf{$package}=$version;  
             if ($dbf_prev{$package}){
		 if ($dbf_prev{$package} eq $version){
                 push(@dbf_sam, $package);
                 }
                 else{
		 push(@dbf_new, $package);
                 } 
             } 
             else{
             push(@dbf_newnew, $package);
             }    
             } 
	     close(TTT);

#    print "A @dbf_sam \n";
#    print "B @dbf_new \n";
#    print "C @dbf_newnew \n";

    if ( $prev_exist eq 1 ){
	$phrase1="NEW PACKAGES AND THEIR VERSIONS";
        $phrase2="PACKAGES WITH NEW VERSIONS";
        $phrase3="PACKAGES WITH OLD VERSIONS";
    }
    else{
	$phrase1="PACKAGES AND THEIR VERSIONS (previous release info is not available)";
        $phrase2="&nbsp;";
        $phrase3="&nbsp;";
    } 


%dbf_prev=();


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

NICOS project contents page
</title></head>

<BODY class=body >

<table class=header border=0 cellpadding=5 cellspacing=0 width=\"100%\">
<tr><td bgcolor=\"#CCCFFF\" align=right><font size=\"-1\">
NICOS version $NICOS_VERSION
</font></td></tr>
<tr><td bgcolor=\"#CCCFFF\" align=center>
<br><EM><B><BIG>NICOS (NIghtly COntrol System): list of packages versions
<br><br></EM></B></BIG></td></tr>
<tr><td align=center>
<br><BIG><B>Project: $NICOS_PROJECT_NAME<br></BIG>
<BIG><B>Release: $NICOS_PROJECT_RELNAME<br></BIG>
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
<LI>shows versions of packages for the above indicated release as compared
with previous release (if available) 
</UL>
<P>
<table border=0 cellpadding=5 cellspacing=0 align=center width=\"95%\">
<tbody>
<tr><TH colspan=2 align=center>
${phrase1}
</TH></tr>
";

    foreach $key (@dbf_newnew) {
    $tag=$dbf{$key};
    print WRITEDATA "<tr><td width=\"65%\">$key</td><td width=\"30%\">$tag</td></tr> \n";
    }

print WRITEDATA "
</tbody>
</table>
<P>
<table border=0 cellpadding=5 cellspacing=0 align=center width=\"95%\">
<tbody>
<tr><TH colspan=2 align=center>
${phrase2}
</TH></tr>
";

    foreach $key (@dbf_new){
    $tag=$dbf{$key};
    print WRITEDATA "<tr><td width=\"65%\">$key</td><td width=\"30%\">$tag</td></tr> \n";
    }

print WRITEDATA "
</tbody>
</table>
<P>
<P>
<table border=0 cellpadding=5 cellspacing=0 align=center width=\"95%\">
<tbody>
<tr><TH colspan=2 align=center>
${phrase3}
</TH></tr>
";

    foreach $key (@dbf_sam){
    $tag=$dbf{$key};
    print WRITEDATA "<tr><td width=\"65%\">$key</td><td width=\"30%\">$tag</td></tr> \n";
    }

print WRITEDATA "
</tbody>
</table>
<BR>
<hr align=left width=\"95%\">
<a href=\"index.html\">Back to NICOS project page</a>
<p>
</body>
</html>
";

    close (WRITEDATA);







