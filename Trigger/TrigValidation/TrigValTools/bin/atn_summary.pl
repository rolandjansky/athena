#!/usr/bin/perl -w
use strict;
use File::Basename;

# Produce summary table for trigtest.pl tests in same dir or first parameter to script
my $output="index.html";

# If given, use first argument as output file
if ($#ARGV>=0) {
  $output=$ARGV[0];
}
print "Writing output to $output\n";

# Test procedure.
# 1. Identify nightly to use for test, and the path to the working directory, e.g.
#    17.X.0 rel_1 TrigAnalsysTest:
#    web http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/nightlies/devval/AtlasAnalysis/rel_1/NICOS_area/NICOS_atntest17X0VAL64BS5G4AnlOpt/triganalysistest_testconfiguration_work/
#    afs /afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasAnalysis/rel_1/NICOS_area/NICOS_atntest17X0VAL64BS5G4AnlOpt/triganalysistest_testconfiguration_work/
# 2. Find the output from that nightly in the atn_summary log file, e.g.
#    http://atlas-computing.web.cern.ch/atlas-computing/links/distDirectory/nightlies/developmentWebArea/nicos_web_area17X0VAL64BS5G4AnlOpt/NICOS_TestLog_rel_1/Trigger_TrigValidation_TrigAnalysisTest_36__TrigAnalysisTest_TestConfiguration__atn_xml_summary_table__x.html
#    and cut and paste the environment settings from that:
#    "To test, set the following environment:"
# 3. Make a mock-up of the working directory for the tests 
#   a) create a path that the script will recognise, e.g. from step 1 above, 
#      take this path. You need at least the rel_ part:
#        dir=builds/nightlies/17.X.0/AtlasAnalysis/rel_1/NICOS_area/NICOS_atntest17X064BS5G4AnlOpt/triganalysistest_testconfiguration_work/
#        mkdir -p $dir
#        cd $dir
#   b) Make copies for files and links to test dirs from ATN itself
#    find  /afs/cern.ch/atlas/software/$dir -maxdepth 1 -type d -exec ln -s {} . \;
#    find  /afs/cern.ch/atlas/software/$dir -maxdepth 1 \! -type d -exec cp {} . \;
# 
# 4. Rename the previous index.html to something else e.g. 
#    mv index.html index.html.orig
# 5. Now you can run atn_summary.pl
#    Check there are no warnings or errors of course.
# 6. Compare index.html to index.html.orig, look for expected changes
# 7. Use a web brower to view it, e.g.
#    firefox -no-remote file://`pwd`/index.html

# these lines must match trigtest.pl:
my $regtestout = "regtest.log";
my $htmldiffout = "regtest.html";
my $summaryout = "summary.log";
my $timerout = "timer.log";
my $checklogout = "checklog.log";
my $warnout = "warn.log";
my $rootcompout = "rootcompout.log";
my $checkcountout = "checkcountout.log";
#my $edmcheckout = "edmcheckout.log";
my $postcmdout = "postcmd.log";
my $posttestrc = "post_test.exitcodes";
my $docout = "doc.txt";
my $athenalogglob = "*_{tail,test}.log{,.gz}"; # add .gz
my $atn_timeline = "atn_timeline.png";
my $atn_order = "atn_timeline_order.txt";
my $no_of_nightlies=7;
my %no_of_nightlies_exceptions = ('TrigMC',2);
my %ERROR = (
          OK => 0,
          SCRIPT_MISC => 1,
	  POST_TEST_BAD_EXIT => 2,
	  ROOTCOMP_MISMATCH => 4,
          ATHENA_BAD_EXIT => 8,
          CHECKCOUNTS_FAILED => 16,
          ATHENA_ERROR_IN_LOG => 32,
          ATHENA_REGTEST_FAILED => 64,
	  ATN_TIME_LIMIT => 128,
          PRECONDITION_FAILED_SO_NOT_RUN => 256
          );

my $atn_rel = "unkown";
my $atn_platform = "unkown";
# use to fake env for testing - uncomment to enable
my $testcwd;
#my $testcwd = "/afs/cern.ch/atlas/software/builds/nightlies/17.X.0/AtlasAnalysis/rel_1/NICOS_area/NICOS_atntest17X064BS5G4AnlOpt/triganalysistest_testconfiguration_work/";  


sub main();
main();
exit;

sub main(){
    # for debugging:
    system("printenv > atn_summary_printenv.log");
    print "\nTo test, set the following environment. If unset, results may not reproduce those within ATN. Read this script for more details on testing procedure.\n";
    my $ev;
    foreach $ev qw(AtlasProject NICOS_NIGHTLY_NAME NICOS_ATLAS_PROJECT NICOS_ATLAS_RELEASE NICOS_PROJECT_RELNAME_COPY){
        print "export " . $ev . "=" . getEnv($ev,"") . "\n";
    }
    print "\n";
    my $context = getcontext();
    my $testdir='.';
    my @testnames=();
    my %exitcodes=();
    my %maskedcodes=();
    my %doc=();
    my %timer=();
    my %order = getorder();
    opendir(DIRHANDLE, $testdir) || die "Cannot opendir $testdir: $!";
    foreach my $name (sort readdir(DIRHANDLE)) {
	if (-d $name and $name !~ /^\./){
	    print "found dir: $name ... ";
	    push @testnames, $name;
	    if (-f "$name/$summaryout"){
		print "found summary file\n";
		open SUMMARYIN, "<$name/$summaryout";
		while (<SUMMARYIN>){
		    if (/exitcode = (\d+)/){
			$exitcodes{$name} = $1;
		    }
		    elsif (/maskedcode = (\d+)/){
			$maskedcodes{$name} = $1;
		    }
		}
		close SUMMARYIN;
		if (-f "$name/$timerout"){
		    print "found timer file\n";
		    open TIMERIN, "<$name/$timerout";
		    while (<TIMERIN>){
			my ($wall,$user,$system)=split;
			$timer{$name}=$wall;
		    }
		    close TIMERIN;
		} 
		else {
		    print "no timer file\n";
		}
	    } else {
		print "no summary file\n";
	    }
	    $doc{$name}="";
	    if (-f "$name/$docout"){
		print "found doc file\n";
		open DOCIN, "<$name/$docout";
		while (<DOCIN>){
		    chomp;
		    $doc{$name}.=$_;
		}
		close DOCIN;
	    } 
	    else {
		print "no doc file\n";
	    }

	}
    }
    closedir(DIRHANDLE);
    
    open HTMLOUT, "> $output" or die "failed to write to $output: $!";
    print HTMLOUT "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">
<html>\n<head>\n<script type=\"text/javascript\" src=\"http://atlas-project-trigger-release-validation.web.cern.ch/atlas-project-trigger-release-validation/www/js/sorttable.js\"></script>\n<script type=\"text/javascript\" src=\"http://atlas-project-trigger-release-validation.web.cern.ch/atlas-project-trigger-release-validation/www/js/suitehighlight.min.js\"></script>\n<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" >
<title>Trigger ATN results for ".getEnv("AtlasProject","").",".getEnv("NICOS_NIGHTLY_NAME","").",$atn_rel</title>\n
<script type=\"text/javascript\">

var thisRelease = null;
var loaded = false;

function highlightDiffs(clear) {
  var table1 = document.getElementById('ATNResults');
  var table2 = null;
  if (!clear) {
    try {
      table2 = document.getElementById('DiffFrame').contentWindow.document.getElementById('ATNResults');
    }
    catch (e) {}
  }

  var diffs = 0;
  for (var i=1; i < table1.rows.length; i++) {
    var cells = table1.rows[i].cells;
    for (var j=2; j < cells.length; j++) {   // skip name and wiki column
      if (j==9) continue;   // skip timing
      if (!table2 || cells[j].textContent == table2.rows[i].cells[j].textContent) {
        cells[j].style.backgroundColor = '';
        cells[j].title = '';
      }
      else {
        cells[j].style.backgroundColor = '#FFCCCC';
        cells[j].title = table2.rows[i].cells[j].textContent || table2.rows[i].cells[j].innerText;
        diffs++;
      }
    }
  }
  setDiffCount(diffs);
}

function setDiffCount(diffs) {
  if (diffs>0) {        
    document.getElementById('nDiffs').textContent = diffs+' differences';
  }
  else {
    document.getElementById('nDiffs').textContent = '';
  }
}

function loadPage() {
  var diffRelease = document.DiffForm.rel.value;
  var iframe = document.getElementById('DiffFrame');  
  if (diffRelease != thisRelease) {
    // Load second summary page in hidden iframe
    var url = window.document.location.href;
    iframe.src = url.replace(/rel_[0-6]/, diffRelease);
  }
  else {
    highlightDiffs(true);  // remove all highlighting
  }
}

function setRelease() {
  var url = window.document.location.href;
  thisRelease = url.match(/rel_[0-6]/);
  if (thisRelease) {
    thisRelease = thisRelease[0];
    document.DiffForm.rel.value = thisRelease;
  }
  return true;
}

function popup(mylink, windowname) {
  if (! window.focus) return true;
  var href;
  if (typeof(mylink) == 'string')
    href=mylink;
  else
    href=mylink.href;
  window.open(href, windowname, 'width=1000,height=300,toolbar=yes,resizable=yes,scrollbars=yes');
  return false;
}

function showBuildFailures(failures,link) {
  if (failures!='0') {
    document.write('<a href=\"'+link+'\" style=\"color:Red\">Build failures: '+failures+'</a>');
  }
}

</script>
</head>
<style type=\"text/css\">
  tr.hi:hover {background-color:#E6E6E6;}
</style>
<body onload=\"highlightSuite({id:'ATNResults'});setDiffCount(0);\">
<h1>Trigger ATN test results summary</h1>
<form name=\"DiffForm\">
";

  
    my $date=qx(date);
    my $testWWWPage=getTestWWWPage();
    my $nicosWWWPage=dirname($testWWWPage);
    #print ">>> nicosWWWPage=$nicosWWWPage\ntestWWWPage=$testWWWPage\n";

    print HTMLOUT "<p>Last updated $date</p>\n";

    if ($context){
        # supported projects: Offline + AtlasHLT for bugfix, P1HLT + Tier0 for caches. Better to use case logic but this will do for now.
        my $project="AtlasOffline";
	if (getEnv('NICOS_ATLAS_PROJECT','') =~ /AtlasHLT|AtlasP1HLT|AtlasCAFHLT|AtlasTier0|AtlasProduction|TrigMC|AtlasTestHLT/){
	    $project=getEnv('NICOS_ATLAS_PROJECT','');
	}
	print "NICOS_ATLAS_PROJECT=getEnv('NICOS_ATLAS_PROJECT','') project=$project\n";

	my $atn_relno = $atn_rel;
	$atn_relno =~ s/rel_//;
	print HTMLOUT "<p><a href=\"".$testWWWPage."\">Nightly test</a>: ".getEnv("AtlasProject","").",".getEnv("NICOS_NIGHTLY_NAME","").",<b>$atn_rel</b> ($atn_platform)";
	print HTMLOUT "<script src=\"".$nicosWWWPage."/build_failures_".$atn_relno.".js\" language=\"JavaScript\"></script>\n";
	print HTMLOUT "<script type=\"text/javascript\">showBuildFailures(failures_".$atn_relno."(),"."\"".$nicosWWWPage."/nicos_buildsummary_".$atn_relno.".html\")</script>";
	
	print HTMLOUT "<br>Other nightlies: ";
        if (defined $no_of_nightlies_exceptions{$project}){
            $no_of_nightlies=$no_of_nightlies_exceptions{$project};
            print "$project exceptionally has $no_of_nightlies nightly builds\n";
        } else {
            print "$project normally has $no_of_nightlies nightly builds\n";
        }
	for (my $i=0; $i<$no_of_nightlies; $i++){
	    if ("rel_$i" eq $atn_rel){
		print HTMLOUT " $i ";
	    } else {
		my $p=getrelpath();
		$p =~ s/rel_\d/rel_$i/;
		$p =~ s/rel_nightly/rel_$i/;
		print HTMLOUT " <a href=\"$p\">$i</a> ";
	    }
	}

	# useful link to nightly change notes page
	my $release=getEnv('NICOS_ATLAS_RELEASE','');
	my $val="";
	print "NICOS_NIGHTLY_NAME=getEnv('NICOS_NIGHTLY_NAME','')\n";
        if (getEnv('NICOS_NIGHTLY_NAME','') =~ /-VAL/){
	    $val="-val";
	}
	#my $cnurl="http://cern.ch/atlas-project-trigger-release-validation/www/ReleaseNotes/test/TestReleaseDiff-$release-$atn_rel$val.html";
        my $cnurl="http://atlas-project-trigger-release-validation.web.cern.ch/atlas-project-trigger-release-validation/www/ReleaseNotes/ChangeNotes$project-$release-$atn_rel$val.html";
	print "cnurl=$cnurl\n";
        
	print HTMLOUT "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<a href=\"$cnurl\">nightly tag diffs</a>
&nbsp;&nbsp;&nbsp;&nbsp;Diff results: <select name=\"rel\" size=\"1\" onchange=\"loadPage()\">
<option value='rel_0'>rel_0</option>
<option value='rel_1'>rel_1</option>
<option value='rel_2'>rel_2</option>
<option value='rel_3'>rel_3</option>
<option value='rel_4'>rel_4</option>
<option value='rel_5'>rel_5</option>
<option value='rel_6'>rel_6</option>
</select> <span id='nDiffs' style='font-weight:bold'></span>
</form><script type=\"text/javascript\">setRelease();</script></p>";
    }

    #my $cmt = $ENV{'CMTPATH'};   
    #my $nightlykey =  'builds/nightlies/';
    #my $keylength  = length $nightlykey;
    #my $relstart  = rindex($cmt,$nightlykey) ;
    #if( $relstart > -1 ) {
    #  $relstart =  $relstart + $keylength;
    #  #  $nightly should be dev,devval, bugfix, val, ect.
    #  $nightly = substr $cmt, $relstart;
    #  my $nextslash = index $nightly, '/' ;
    #  $nightly = substr $nightly, 0, $nextslash ;
    #}
    my $nightlywiki = getEnv('NICOS_NIGHTLY_NAME','');
    print "Nightly wiki page name postfix before deletes: $nightlywiki\n";
    #remove val from the name
    $nightlywiki =~ s/val//;
    $nightlywiki =~ s/VAL//;
    # remove any . from the nightly name (Wiki doesn't like these)
    $nightlywiki =~ s/\.//g;
    # remove any - from the nightly name (Wiki dosen't like these either)
    $nightlywiki =~ s/-//g;
    # Strip all but first two characters so that the version
    # becomes just '16' rather than 16.X.Y etc. Only do this
    # for versions starting with 16 (or higher) so that older
    # versions remain with the old links
    my $shortver = substr $nightlywiki, 0, 2;
    if ($shortver ge 16) {
	$nightlywiki=$shortver;
    }
    print "Nightly wiki page name using postfix: $nightlywiki\n";
    
#    print HTMLOUT "<p>Shifter: before modifing the wiki test that you are authenticated <A  HREF=\"https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerValidationATNStatusDummy\" onClick=\"return popup(this, 'notes')\">here</A><br>\n";
# doesnt' work    print HTMLOUT <A  HREF=\"https://login.cern.ch/adfs/ls/?wa=wsignout1.0\"> logout </A> \n"; 

#    print HTMLOUT "Update wiki with  problems common to multiple tests <A  HREF=\"https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerValidationATNStatus$nightlywiki#Current_issues_in_nightly_ATN_te\" onClick=\"return popup(this, 'notes')\">here</A> </p>\n";

    print HTMLOUT "<p><b>Shifter:</b> <a href=\"https://savannah.cern.ch/bugs/?group=atlas-trig&func=browse&set=custom&msort=0&custom_sb6[]=101&assigned_to[]=0&date[]=1900-1-1&priority[]=0&resolution_id=3,4,5,8,100,101,378&severity[]=0&status_id[]=1&release[]=&fix_release[]=&date_op[]=%3E&advsrch=0&msort=0&chunksz=150&spamscore=5&report_id=378&sumORdet=&morder=resolution_id<,bug_id>&order=bug_id#results\">Click here</a> to get a list of Savannah bugs flagged as affecting ATLAS nightlies. Please create a new bug report, with \"Affects ATN\" set to true for all issues not listed there.</p>";
    
    print HTMLOUT "\n<table class=\"sortable\" id=\"ATNResults\" border=1><thead><tr>
<th title=\"Click to sort by test suite\">Test name</th> 
<th>Wiki</th>
<th>Overall Result</th> 
<th title=\"exit code of Athena\">Athena exit</th> 
<th title=\"presence of any error messages in log\">Error Msgs</th> 
<th title=\"number of warning messages in log\">Warn Msgs</th> 
<th>Reg. tests</th> 
<th title=\"expert histogram exact comparison to ref\">Root comp</th> 
<th title=\"L2 + EF chain and trigger element counts within tolerance w.r.t. ref\"> Chain, TE cnts </th> 
<th title=\"athena wall time in secs\">Time (s)</th> 
<th title=\"trigtest.pl exit code, bare and masked for ATN\">Exit code</th> 
<th>Post Tests</th> 
<th>Dir. link</th> 
<th>Log link</th></tr></thead><tbody>\n";
    for my $name (@testnames){
	my $atn_script = "-";
	my $atn_timeout = "-";
	my $athena_exit = "-";
	my $error_msgs = "-";
	my $error = "-";
	my $warn = "-";
	my $reg_tests = "-";
	my $rootcomp = "-";
	my $checkcount = "-";
	my $timer = "-";
	my $exitcode = "-";
	my $maskedcode = "-";
        my $postrc = 0;
	my $logfile = "";
	my $order = "";	
	if (defined $exitcodes{$name}){
	    $exitcode = $exitcodes{$name};
	    if (defined $maskedcodes{$name}){
		$maskedcode = $maskedcodes{$name};		
		if ($maskedcodes{$name} != 0){
		    $maskedcode = "<font color=\"red\">$maskedcodes{$name}</font>";		    
		}
	    }
	    if (defined $timer{$name}){
		$timer = $timer{$name};
	    }
	    $atn_script = testcode($exitcodes{$name},"SCRIPT_MISC");
	    my $skipped = testcode($exitcodes{$name},"PRECONDITION_FAILED_SO_NOT_RUN");
	    if ($skipped !~ /OK/){ 
		$atn_script = "<div title=\"Not run because precondition failed\" style=\"color:gray\">SKIP</div>";
		$logfile = "not run because precondition failed";
	    }
	    elsif ($atn_script !~ /OK/){
		$atn_script = "<font color=\"red\">FAIL</font>";
	    }
	    else{
		if ($maskedcodes{$name} != 0){
		    my $errors = errornames($maskedcodes{$name});
		    $atn_script = "<div title=\"Failures: $errors\" style=\"color:red\">ERROR</div>";
		}

		$atn_timeout = testcode($exitcodes{$name},"ATN_TIME_LIMIT");
		$athena_exit = testcode($exitcodes{$name},"ATHENA_BAD_EXIT");
		$rootcomp = testcode($exitcodes{$name},"ROOTCOMP_MISMATCH");
		
		if ($atn_timeout !~ /OK/){
		    $athena_exit="<font color=\"yellow\">TIMEOUT</font>";
		}
		
		# no checklog output file and no failure in exitcode
		$error_msgs = testcode($exitcodes{$name},"ATHENA_ERROR_IN_LOG");
		if (! -f "$name/$checklogout" and $error_msgs =~ /OK/){
		    $error = "<font color=\"gray\">N/A</font>";
		} else {
		    my $error_count = '';
		    if ($error_msgs !~ /OK/){
			$error_count=qx(head -5 $name/$checklogout | grep 'Found messages in');
			if ($error_count =~ /Found messages in.*\(([0-9]*)\).*/){
			    $error_count="($1)";
			}
		    }
		    print "error count: $error_count\n";
		    $error = "<a type=\"text/plain\" href=\"$name/$checklogout\">$error_msgs$error_count</a>";
		}
		    
		if (! -f "$name/$warnout"){
		    $warn = "<font color=\"gray\">N/A</font>";
		} else {
                    my $warn_count=qx(head -5 $name/$warnout | grep 'Found messages in');
		    if ($warn_count =~ /Found messages in.*\(([0-9]*)\).*/){
                      $warn_count = $1;
                      chomp $warn_count;
                    }
                    else {
                      $warn_count = 'N/A'
                    }
		    print "warn count: $warn_count\n";
		    $warn = "<a type=\"text/plain\" href=\"$name/$warnout\">$warn_count</a>";
		}
		if (! -e "$name/$regtestout"){
		    $reg_tests = "<font color=\"gray\">N/A</font>";
		} else {                  
                    my $status = warncode($exitcodes{$name},"ATHENA_REGTEST_FAILED");
                    my $tooltip = "";                    
                    if ($status ne "OK") {
                      my @diffs = `grep REGTEST $name/$regtestout | awk '{print \$2}' | sort -u`;
                      chomp @diffs;
                      if (scalar(@diffs) > 50) {
                        $tooltip = "More than 50 sources of REGTEST differences!";
                      }
                      elsif (scalar(@diffs) > 0) {
                        $tooltip = join(", ",@diffs);
                      }
                    }
		    $reg_tests = "<a type=\"text/plain\" title=\"$tooltip\" href=\"$name/$regtestout\">" . $status . "</a>";
                    if ($status ne "OK" and -e "$name/$htmldiffout"){
                      $reg_tests .= " <a href=\"$name/$htmldiffout\">[ht]</a>"
                    }
		}
		if (! -e "$name/$rootcompout"){
		    $rootcomp = "<font color=\"gray\">N/A</font>";
		} elsif ($rootcomp =~ /FAIL/){
                    my @diffs = `egrep '^Different histograms:' $name/$rootcompout`;
                    chomp @diffs;
		    $rootcomp = "<a type=\"text/plain\" title=\"@diffs\" href=\"$name/$rootcompout\"><font color=\"red\">DIFF</font></a>";
                    if (-e "$name/rootcomp.ps.gz") {
                      $rootcomp .= " <a href=\"$name/rootcomp.ps.gz\">[ps]</a>";
                    }
		} elsif ($rootcomp =~ /OK/){
		    $rootcomp = "<a type=\"text/plain\" href=\"$name/$rootcompout\"><font color=\"green\">MATCH</font></a>";
		}
		if (! -f "$name/$checkcountout"){
		    $checkcount = "<font color=\"gray\">N/A</font>";
		} else {
		    my $checkcount1 = chaincountcheck("$name/$checkcountout");
		    print "check chain counts: $checkcount1\n";
		    $checkcount = "<a type=\"text/plain\" href=\"$name/$checkcountout\">$checkcount1</a>";
		}
		$postrc = 0;
		if (! -e "$name/$posttestrc"){
                  $postrc = "<font color=\"gray\">N/A</font>";
                } elsif (($exitcodes{$name} & $ERROR{"POST_TEST_BAD_EXIT"})==0) {
                  $postrc = "OK";
                } else {                  
                  $postrc = getposttests($name);
		}

	    }
	}
	my @logfiles = glob("$name/$athenalogglob");
	for (@logfiles){
	    my ($lfd, $lflabel) = split('/',$_);
	    $lflabel = "[tail]" if ($lflabel =~ /tail/);
	    $logfile .= "<a type=\"text/plain\" href=\"$_\"><font size=-2>$lflabel</font></a> ";
	}
	if (length($logfile)==0){
	    $logfile = "&nbsp;";
	}
	print "logfile=$logfile\n";
	
	my $thename = $name;
        my $shortname = $name;
	# remove any . from the nightly name (Wiki doesn't like these)
        $shortname =~ s/\.//g;
	#truncate to desired 32 character wiki limit
        $shortname =  substr($shortname,0,32);
	my $logurl;
	if ($context){
	    $logurl = getlogpath($name,$testdir);
	}
	# special case when run by hand, log might be in current dir as testname.log
	if (! defined $logurl and -f "$name.log"){
	    $logurl="$name.log";
	}
	if (defined $logurl){
	    $thename = "<a type=\"text/plain\" href=\"$logurl\">$name</a>";
	    print "found URL: $logurl\n";
	} else {
	    print "not found URL for $name\n";
	}

	if (exists $order{$name}) {
	    $order = "suite=\"$order{$name}[1]\" sorttable_customkey=\"$order{$name}[0]\"";
	}
	print HTMLOUT "<tr class=\"hi\"><td $order title=\"$doc{$name}\">$thename</td> <td><A HREF=\"https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerValidationATNStatus$nightlywiki#$shortname\" onClick=\"return popup(this, 'notes')\">wiki</A></td> <td>$atn_script</td> <td>$athena_exit</td> <td>$error</td> <td>$warn</td> <td style=\"white-space:nowrap\">$reg_tests</td> <td style=\"white-space:nowrap\">$rootcomp</td> <td>$checkcount</td> <td><a type=\"text/plain\" href=\"$name/$timerout\">$timer</a></td> <td>$exitcode/$maskedcode</td> <td>$postrc</td> <td><a href=\"$name\">dir</a></td><td>$logfile</td></tr>\n";
    }
    print HTMLOUT "</tbody></table>";
    if (-e "$atn_timeline"){
      print HTMLOUT "<font face=\"monospace\"><br>";
      my @lines = `tail -3 *atn_timeline.log`;
      print HTMLOUT join('<br>',@lines);
      print HTMLOUT "<a href=\"atn_timeline.png\"> [timeline plot]</a>";
      print HTMLOUT "</font>";
    }
    print HTMLOUT '<iframe onload="highlightDiffs(false)" id="DiffFrame" style="visibility:hidden;display:none;"></iframe></body></html>';
    close HTMLOUT;
    
}

sub chaincountcheck($){
    my ($infile)=@_;
    my $counts="";
    my $nonexist = 0;
    open COUNTIN, "<$infile" or return "-";
    while (<COUNTIN>){
	if (/Fraction inside tolerance\:\s+(\d+)\/(\d+)/){
	    $counts .= "$1/$2 "
	} elsif (/does not exist/){
            $nonexist++;
        }   
    }
    close COUNTIN;
    $counts = "New" if ($counts eq "" &&  $nonexist >  0);
    $counts = "OK" if ($counts eq "");
    print "nonexistant counts: $nonexist\n"; 
    return $counts;
}

sub testcode($$){
    my ($code,$string) = @_;
    my $value=$ERROR{$string};
    return (($code & $value)? "<font color=\"red\">FAIL</font>" : "OK");
}

sub warncode($$){
    my ($code,$string) = @_;
    my $value=$ERROR{$string};
    return (($code & $value)? "<font color=\"orange\">WARN</font>" : "OK");
}

sub getEnv($$){
  my ($name,$default) = @_;
  if (exists $ENV{$name}) {
    return $ENV{$name};
  }
  else {
    return $default;
  }
}

# figure out which nightly this is and where logs are etc.
# this wil only work if run in ATN.
sub getcontext(){
    my $cwd = '';
    chomp($cwd=`pwd`);
    # for testing: 
    if (defined $testcwd){
        print "!WARNING: TEST MODE!";
        $cwd=$testcwd;
    }
    print "--------------------------------------------------------\n";
    print "$cwd\n";
    if ($cwd =~ /NICOS_.*test([\w\d]+)\//){   # NICOS_qmtest (old) or NICOS_atntest (new) directory
	$atn_platform=$1;
	#print "$atn_platform\n";
	#$atn_log_dir="../../NICOS_TestLog$atn_platform";
	#print "atn log dir: $atn_log_dir\n";
	#system("ls $atn_log_dir");
        print "ATN_JOB_LOGFILE=" . getEnv("ATN_JOB_LOGFILE","") . "\n";
	print "ATN_PACKAGE=" . getEnv("ATN_PACKAGE","") . "\n";
	print "--------------------------------------------------------\n";
    }    
    $atn_rel = $ENV{'NICOS_PROJECT_RELNAME_COPY'};
    
    if (defined $atn_platform and defined $atn_rel){
	print "$atn_platform $atn_rel $cwd\n";
    }else{
	print "atn_platform and atn_rel not defined - probably running outside ATN\n";
    }
    return (defined $atn_platform && defined $atn_rel && defined $cwd);
}

sub getlogpath($){
    my ($name,$testdir) = @_;
    my $found;
    opendir(DIRHANDLE, $testdir) || die "Cannot opendir $testdir: $!";
    foreach my $file (sort readdir(DIRHANDLE)) {
	if ($file =~ /__${name}.log/){
	    #print "$name matches $file\n";
	    $found="$file";
	    last;
	}
    }
    return $found;
}

# create a relative URL by going back (../) until rel_N is reached, then forward again to here. This can then be changed to use for other rel_N
sub getrelpath(){
    my $pcwd = '';
    chomp($pcwd=`pwd`);
    # for testing
    if (defined $testcwd){
        $pcwd=$testcwd;
    }
    my @pcwd = split('/',$pcwd);
    my ($i, $n, $up, $down);
    $up="";
    $down="";
    for ($i=$#pcwd; $i>=0; --$i){
	$up.="../";
	$down = $pcwd[$i]. "/" . $down;
	if ($pcwd[$i] =~ /rel_/){
	    last;
	}
    }
    return "$up$down";
}

# find the directory name we are in, e.g. triganalysistest_testconfiguration_work
sub gettestpkgdir(){
    use File::Basename;
    my $cwd = '';
    chomp($cwd=`pwd`);
    if (defined $testcwd){
        $cwd=$testcwd;
    }
    return basename($cwd);
}

sub getposttests(){
  my ($testname) = @_;
  my $text;
  open POSTRC, "<$testname/$posttestrc" or return "";
  while (<POSTRC>){
    chomp;
    (my $name, my $rc) = split();
    if ($rc!=0){
      $text .= "<a type=\"text/plain\" href=\"$testname/post_test_$name.log\"><font color=\"red\">$name</font></a> ";
    }
  }
  close POSTRC;
  return "<font size=\"-2\">$text</font>";
}

sub getorder(){
    my %order = ();
    open ORDER,"<$atn_order" or return %order;
    while (<ORDER>){
	chomp;
	(my $i, my $name, my $suite) = split();
	$order{$name} = [$i,$suite]
    }
    return %order;
}

sub errornames() {
    my ($code) = @_;
    my @errors = ();
    foreach my $key ( keys %ERROR ) {
	if (($code & $ERROR{$key}) != 0) {
	    push(@errors,$key);
	}
    }
    return join(', ', @errors);
}

sub getTestWWWPage() {
    open XML,"ATNOverview.xml" or return "";
    while (<XML>){
	chomp;
	if (/<testWWWPage>(.*)<\/testWWWPage>/){
	    return $1;
	}
    }
    return "";	
}
