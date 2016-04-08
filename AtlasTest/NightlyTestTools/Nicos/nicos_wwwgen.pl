#!/usr/local/bin/perl
#
#
#
sub header{
    my @arr = split("_",$rel_name);
    my $lastel = $#arr;
    my $relnum=$arr[$lastel];
    my $relnum_suffix = "_$relnum";
    if ( $relnum eq "" ) { $relnum_suffix = ""; }
    if ( $relnum eq "RELEASE" ) { $relnum_suffix = ""; }
    if ( $#arr eq 0 ) { $relnum_suffix = ""; }
    my $cont_word="build";
    my $item_word="packages";
    if ($order eq 22) { $cont_word="test";}
    if ($order eq 22) { $item_word="tests";}
print"
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
  font-family: Verdana, Arial, Helvetica, sans-serif;
  font-size: 10pt;
} </style>

<style> #cellrose {
background-color: #FFCCCC;
} </style>

<style> #cellsalad {
background-color: #00FF33;
} </style>

<head><title>

  nicos webpage with ${cont_word} results
</title>

<SCRIPT SRC=\"status_test_pass${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"status_test_fail${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"status_unit_pass${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
<SCRIPT SRC=\"status_unit_fail${relnum_suffix}.js\" language=\"JavaScript\"></SCRIPT>
</head>

<BODY class=body >
<FONT COLOR=\"#008080\"><FONT SIZE=-1></FONT></FONT>

<table class=header border=0 cellpadding=5 cellspacing=0 width=\"100%\">
";

if (${cont_word} eq "test"){
print "
<tr><td bgcolor=\"#CCCFFF\" align=right>
<form>
<input type=button value=\"Close Window\" onClick=\"window.close();\" style=\"background: red; font-weight:bold\">
</form>
</td></tr>
";
}

print "
<tr><td bgcolor=\"#CCCFFF\" align=center>
<br><EM><B><BIG>NICOS (NIghtly COntrol System) ${cont_word} results
<br><br></EM></B></BIG></td>
<tr><td align=center>
<br><BIG><B>Project: $NICOS_PROJECT_NAME
<br>
";

if ( $NICOS_INC_BUILD eq "yes" ){
print "Release: $rel_name &nbsp; -- &nbsp; Build incrementally on: ${NICOS_HOSTNAME}\n";
}
else{
print "Release: $rel_name &nbsp; -- &nbsp; Build on: ${NICOS_HOSTNAME}\n";
}

print "
<br>
</BIG><font size=\"-1\">\n";
    if ( $NICOS_TITLE_COMMENT ne "" ){
    print "$NICOS_TITLE_COMMENT <br>\n";
    }

if ( $NICOS_INC_BUILD eq "yes" ){
    print "Work area for incrementals: ${rel_loc} <br>\n";
}
else{
    print "Location: ${rel_loc} <br>\n";
}

if ( $NICOS_COPY_HOME ne "" && $NICOS_COPY_HOME ne $NICOS_PROJECT_HOME && $NICOS_COPY_BUILD_TYPE ne "void" ){
    print "Releases are copied to: ${NICOS_COPY_HOME} <br>\n";
}

print"
Highlighted ${item_word} have problems,
click on names to see <a href=\"$dir_name\">logfiles</a></font></B><br>
</td></tr>

<tr><td align=right><font size=\"-1\"> &nbsp; </font></td></tr>
<tr class=light><td bgcolor=\"#CCCFFF\" align=right colspan=1><font size=\"-1\">
<script language=\"JavaScript\">
    <!-- Hide script from old browsers
    document.write(\"Last modified \"+ document.lastModified)
    // end of script -->
    </script>
</font></td></tr></table><BR>

<table border=0 cellpadding=5 cellspacing=0 align=center width=\"90%\">
<tbody>
";

if (${cont_word} ne "test"){
print "
<tr bgcolor=\"99CCCC\"><TH colspan=2 align=center >
Integration+Unit tests results</th>
<th bgcolor=\"99CCCC\" colspan=2 align=center >
<form>
<input type=button style=\"background: cornsilk; font-weight:bold\"
value=\"Click for details\"
onClick=\"myRef = window.open('nicos_testsummary${relnum_suffix}.html','testwin',
'left=20,top=20,width=900,height=700,resizable=1,scrollbars,toolbar,menubar');
myRef.focus()\">
</form>
</th>
</tr>";
}
else{
print "
<tr bgcolor=\"99CCCC\"><TH colspan=4 align=center >
Integration+Unit tests results</th>
</tr>";
}

print"
<tr bgcolor=\"99CCCC\">
<th align=center >
passed:</th>
<th ID=cellsalad> 
<SCRIPT>document.write(status_tp${relnum_suffix}())</SCRIPT>
+
<SCRIPT>document.write(status_up${relnum_suffix}())</SCRIPT>
</th>
<th align=center >
failed:</th>
<th ID=cellrose>
<SCRIPT>document.write(status_tf${relnum_suffix}())</SCRIPT>
+
<SCRIPT>document.write(status_uf${relnum_suffix}())</SCRIPT>
</th>
</tr>";

my $NICOS_COMMENT_TESTING_FILE="${NICOS_WEBDIR}/nicos_comment_testing";
if ( -f $NICOS_COMMENT_TESTING_FILE )
{
    print"
    <tr bgcolor=\"99CCCC\">
    <th colspan=4 align=center>";
    open(TTT,"$NICOS_COMMENT_TESTING_FILE");
    while (<TTT>) {    chomp;
		       print "$_ \n";
		   }
    close(TTT);
    print"
    </th>
    </tr>";
}

print"
</tbody>
</table>
<BR><BR>
";

print"
<table border=0 cellpadding=5 cellspacing=0 align=center width=\"90%\">
<tbody>
";
if (${cont_word} eq "test"){
print "
<tr><td><B>Package with Int. Test</B></td><td><B>Test Name</B></td><td><B>Test Suite</B></td><td><B>Test Result</B></td></tr>
";
}
exit;}
#
#
#
sub interim_test_header{
print "
<tr><td bgcolor=\"#99CCCC\">&nbsp;</td><td bgcolor=\"#99CCCC\">&nbsp;</td><td bgcolor=\"#99CCCC\">&nbsp;</td><td bgcolor=\"#99CCCC\">&nbsp;</td></tr>
<tr><td><B>Package with Unit Test</B></td><td><B>Test Name</B></td><td><B>Test Suite</B></td><td><B>Test Result</B></td></tr>
";
exit;}
#
#
#
sub interim{
    my @arr = split("_",$rel_name);
    my $lastel = $#arr;
    my $relnum=$arr[$lastel];
    my $relnum_suffix = "_$relnum";
    if ( $relnum eq "" ) { $relnum_suffix = ""; }
    if ( $relnum eq "RELEASE" ) { $relnum_suffix = ""; }
    if ( $#arr eq 0 ) { $relnum_suffix = ""; }
print"
</table><BR>
";

print"
<table border=0 cellpadding=5 cellspacing=0 align=center width=\"90%\">
<tbody>
<tr bgcolor=\"99CCCC\"><align=center >
<th colspan=3>Build results for individual packages. Sorted by:</th>
</tr>";

if($order eq 1) { 
print"
<tr><align=center >
<th bgcolor=\"CCCCFF\"><A href=\"nicos_buildsummary${relnum_suffix}.html\">packages names, failures first</A></th>
<th bgcolor=\"99CCCC\">build order</th>
<th bgcolor=\"CCCCFF\"><A href=\"nicos_buildsummary2${relnum_suffix}.html\">containers names</A></th>
</tr>";
}
elsif($order eq 2) {
print"
<tr><align=center >
<th bgcolor=\"CCCCFF\"><A href=\"nicos_buildsummary${relnum_suffix}.html\">packages names, failures first</A></th>
<th bgcolor=\"CCCCFF\"><A href=\"nicos_buildsummary1${relnum_suffix}.html\">build order</A></th>
<th bgcolor=\"99CCCC\">containers names</th>
</tr>";
}
else{
print"
<tr><align=center >
<th bgcolor=\"99CCCC\">packages names, failures first</th>
<th bgcolor=\"CCCCFF\"><A href=\"nicos_buildsummary1${relnum_suffix}.html\">build order</A></th>
<th bgcolor=\"CCCCFF\"><A href=\"nicos_buildsummary2${relnum_suffix}.html\">containers names</A></th>
</tr>";
}

print"
</table>
<BR>
";

print"<table class=header border=10 cellpadding=5 cellspacing=0 align=center>";

my $TestTitle = "Test";
if ( $NICOS_TITLE_TESTS ne "" )
{ $TestTitle = $NICOS_TITLE_TESTS; } 

my $QATitle = "QA Test";
if ( $NICOS_TITLE_QA ne "" )
{ $QATitle = $NICOS_TITLE_QA; }



print"
</table>
<!--------------------------- Contents --------------------------->

<table cellspacing=10%>
<TR>
<TD><B>Package Name</B></TD> <TD><B>Container</B></TD> <TD><B>Build</B></TD> 
<TD><B>${QATitle}</B></TD> <TD><B>${TestTitle}</B></TD> <TD><B>Manager(s)</B></TD></TR>
";
exit;}
#
#
#
sub final{
print"


</table>
</body>
</html>
";
exit;}
#
#
#
use Env;
my $color = "";
my $clr1 = "";
my $color1 = "";
my $package_name = "";
my $a_names = "";
my $NICOS_TITLE_COMMENT="$NICOS_TITLE_COMMENT";
my $NICOS_TITLE_TESTS="$NICOS_TITLE_TESTS";
my $NICOS_TITLE_QA="$NICOS_TITLE_QA";
my $NICOS_HOSTNAME="$NICOS_HOSTNAME";
my $NICOS_COPY_HOME="$NICOS_COPY_HOME";
my $NICOS_COPY_BUILD_TYPE="$NICOS_COPY_BUILD_TYPE";
my $NICOS_INC_BUILD="$NICOS_INC_BUILD";
my $NICOS_PROJECT_NAME="$NICOS_PROJECT_NAME";
@output = ();

#-> parse arguments
    if ($#ARGV < 0) {exit 1;}
    while ($#ARGV>=0) {
	$_ = $ARGV[0];
	/^-[g]+$/ && do {
	    shift;
	    $package_name = $ARGV[0];
	    shift;
	    $container_name = $ARGV[0];
            shift;
            $dir_log = $ARGV[0];
	    shift;
	    $rec_log = $ARGV[0];
            shift;
            $prob = $ARGV[0];
            shift;
            $qdir_log = $ARGV[0];
            shift;
            $qrec_log = $ARGV[0];
            shift;
            $qprob = $ARGV[0];
            shift;
            $tdir_log = $ARGV[0];
            shift;
            $trec_log = $ARGV[0];
            shift;
            $tprob = $ARGV[0];
            for (my $i = 1; $i <= $#ARGV; $i++) 
            {
            $a_names=$a_names . $ARGV[$i] . " "
            }
	    $color="green";
	    goto end;
	};
        /^-[a]+$/ && do {
            shift;
            $test_dir = $ARGV[0];
            shift;
            $test_name = $ARGV[0];
            shift;
            $test_suite = $ARGV[0];
            shift;
            $dir_log = $ARGV[0];
            shift;
            $rec_log = $ARGV[0];
            shift;
            $prob = $ARGV[0];
            for (my $i = 1; $i <= $#ARGV; $i++)
            {
            $a_names=$a_names . $ARGV[$i] . " "
            }
            $color="green";
#            print "FFFF $test_name $dir_log\n";
            goto end2;
        };
	/^-[h]+$/ && do {
	    shift;
	    $order = $ARGV[0];
	    shift;
	    $proj_name = $ARGV[0];
	    shift;
	    $rel_name = $ARGV[0];
	    shift;
            $rel_loc = $ARGV[0];
            shift;
 	    $dir_name = $ARGV[0];
         &header;
        };
        /^-[u]+$/ && do {
            shift;
	    $order = $ARGV[0];
	    &interim_test_header;
        };
	/^-[i]+$/ && do {
	    shift;
	    $order = $ARGV[0];
	    shift;
	    $rel_name = $ARGV[0];
	    shift;
	    $dir_name = $ARGV[0];
         &interim;
        };
	/^-[f]+$/ && do {&final;};
	last;
end:
	shift;
    }

$red_bg="bgcolor=\"#F0C060\"";
$rb_bg="bgcolor=\"#EEB4B4\"";
#$y_bg="bgcolor=\"#FFFFAA\"";
$y_bg="bgcolor=\"#CCCCFF\"";
$hilight="";

if( $prob >= 2 ){
    $image="<IMG SRC=\"cross_red.gif\" HEIGHT=16 WIDTH=20>";
} 
elsif ( $prob == 1 ){
    $image="<IMG src=yl_ball.gif width=20 height=20>"; 
}
else{
    $image="<IMG SRC=\"tick.gif\" HEIGHT=15 WIDTH=15>";
}
if( $tprob >= 2 || $qprob >= 2 ){
    $hilight="$rb_bg";
}
if( $prob >= 2 ){
    $hilight="$red_bg";
}
if( $hilight eq "" && ( $prob >= 1 || $tprob >= 1 || $qprob >= 1 )){
    $hilight="$y_bg";
}  

push(@output,"<tr ${hilight}><td>"); 
push(@output,"<a href=\"$dir_log/$rec_log\">");
push(@output,"$package_name");
push(@output,"</a>");
push(@output,"<td>");
push(@output,"$container_name");
push(@output,"</td>");
push(@output,"</td>\n");
push(@output,"<td>");
push(@output,"${image}"); 
if($qrec_log eq "0" || $qrec_log eq "N/A")
   {
   push(@output,"</td><td>N/A</td>\n");   
   }
else
   {
   if($qprob eq "2")
      {
      push(@output,"</td><td><a href=\"$qdir_log/$qrec_log\">FAIL</a></td>\n");
      }
   if($qprob eq "1")
      {
      push(@output,"</td><td><a href=\"$qdir_log/$qrec_log\">WARN</a></td>\n");
      }
   if($qprob ne "1" && $qprob ne "2")
      {
      push(@output,"</td><td><a href=\"$qdir_log/$qrec_log\">PASS</a></td>\n");
      }
    }

if($trec_log eq "0" || $trec_log eq "N/A")
   {
   push(@output,"</td><td>N/A</td>\n");
   }
else
   {
   if($tprob eq "2")
      {
      push(@output,"</td><td><a href=\"$tdir_log/$trec_log\">FAIL</a></td>\n");
      }
   if($tprob eq "1")
      {
      push(@output,"</td><td><a href=\"$tdir_log/$trec_log\">WARN</a></td>\n");
      }
   if($tprob ne "1" && $tprob ne "2")
      {
      push(@output,"</td><td><a href=\"$tdir_log/$trec_log\">PASS</a></td>\n");
      }
   }

if ( $a_names eq "" )
{push(@output,"</td><td>N/A</td>\n");}
else
{push(@output,"</td><td>$a_names</td></tr>\n");}
print @output;

exit;

end2:

$red_bg="bgcolor=\"#F0C060\"";
$rb_bg="bgcolor=\"#EEB4B4\"";
#$y_bg="bgcolor=\"#FFFFAA\"";
$y_bg="bgcolor=\"#CCCCFF\"";
$hilight="";
#print "SSS $dir_log/$rec_log $test_name \n";

if( $prob >= 2 ){
    $image="<IMG SRC=\"cross_red.gif\" HEIGHT=16 WIDTH=20>";
    $hilight="$red_bg";
}
elsif ( $prob == 1 ){
    $image="<IMG src=yl_ball.gif width=20 height=20>";
    $hilight="$y_bg";
}
else{
    $image="<IMG SRC=\"tick.gif\" HEIGHT=15 WIDTH=15>";
}

{ push(@output,"<tr ${hilight}><td>"); }
push(@output,"$test_dir");
push(@output,"</td>\n");
push(@output,"<td>");
push(@output,"<a href=\"$dir_log/$rec_log\">");
push(@output,"$test_name");
push(@output,"</a>");
push(@output,"</td>\n");
push(@output,"<td>");
push(@output,"$test_suite");
push(@output,"</td><td>");
{ push(@output,"${image}"); }
push(@output,"</td></tr>\n");
print @output;










