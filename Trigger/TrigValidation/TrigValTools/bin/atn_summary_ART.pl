#!/usr/bin/perl -w
use strict;
use File::Basename;
use Date::Manip::Date;

# Produce summary table for trigtest.pl tests in same dir or first parameter to script
my $output="index.php";

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
    foreach $ev qw(AtlasProject AtlasVersion NICOS_GIT_RELBRANCH NICOS_PROJECT_RELNAME_COPY){
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
    my $project = getEnv('AtlasProject',getEnv('NICOS_ATLAS_PROJECT',''));
    my $gitbranch = getEnv('NICOS_GIT_RELBRANCH','');
    my $release = getEnv('AtlasVersion','');
    opendir(DIRHANDLE, $testdir) || die "Cannot opendir $testdir: $!";
    # modification to run on ART jobs (ATR-17644i)
    foreach my $upname (sort readdir(DIRHANDLE)) {
      if (-d $upname and $upname !~ /^\./){
        print "found updir: $upname ... \n";
        # $upname=$testdir."/".$upname;
        opendir(SUBDIRHANDLE, $upname) || die "Cannot opendir $upname: $!";
        foreach my $name (sort readdir(SUBDIRHANDLE)) {
            next if $name eq ".";
            next if $name eq "..";
            $name=$upname."/".$name;
            if (-d $name){
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
        closedir(SUBDIRHANDLE);
      }
    }
    closedir(DIRHANDLE);
    
    open HTMLOUT, "> $output" or die "failed to write to $output: $!";
    print HTMLOUT "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">
<html>\n<head>\n<script type=\"text/javascript\" src=\"http://atlas-project-trigger-release-validation.web.cern.ch/atlas-project-trigger-release-validation/www/js/sorttable.js\"></script>\n<script type=\"text/javascript\" src=\"http://atlas-project-trigger-release-validation.web.cern.ch/atlas-project-trigger-release-validation/www/js/suitehighlight.min.js\"></script>\n<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" >
<title>Trigger ART results for $project,$gitbranch,$release</title>\n
<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>
<script type=\"text/javascript\">

    google.charts.load('current', {packages: ['corechart', 'line']});
    google.charts.setOnLoadCallback(drawCrosshairs17000);
    google.charts.setOnLoadCallback(drawCrosshairs9000);
    google.charts.setOnLoadCallback(drawCrosshairs17000Data);
    google.charts.setOnLoadCallback(drawCrosshairs9000Data);

    function drawCrosshairs17000() {
                var data = new google.visualization.DataTable();

                data.addColumn('string', 'Nightly');

                <?php
                        
                        \$nightlies = glob(dirname(__FILE__) . '/../../*');
                        \$nightlies = array_values(\$nightlies);
                        \$content = file_get_contents(dirname(__FILE__).'/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_17000/ATLASros2rob2018.py');
                        //echo \$content;
                        preg_match_all('/ROS-.[A-Z]+-.[A-Z]+-.[0-9]+/i',\$content,\$ROSes);
                        \$ROSes = array_values(\$ROSes);
                        foreach(\$ROSes as \$ros){
                                \$ros = array_values(\$ros);
                                foreach(\$ros as \$ros1){
                                        echo \"                data.addColumn('number', '\$ros1'); \\n\";
                                }
                        }

                        echo \"                data.addRows([\";
                        foreach(\$nightlies as \$nightly){
                                \$content = file_get_contents(\$nightly.'/TrigP1Test/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_17000/ROStest.reference.new');
                                //echo \$content.'\\n';                           
                                echo \"                ['\".basename(\$nightly).\"',\";
                                foreach(\$ROSes as \$ros){
                                        \$ros = array_values(\$ros);
                                        foreach(\$ros as \$ros1){
                                                //echo \$ros1.'\\n';
                                                \$matched = preg_match_all('/'.\$ros1.'.[ \\t]+[|].[ \\t]+.[0-9]+.[ \t]+[|](.[.0-9]+)/', \$content,\$rates);
                                                if(\$matched) echo \$rates[1][0].\",\";
                                                else echo \"0.,\";
                                                //print_r(\$rates);
                                        }
                                }
                                echo \"],\";
                        }
                        echo \"                ]);\";
                ?>

        var options = {
                title: 'ROS request rates in rejected events for lumi 1.7e34',
                hAxis: {
                        title: 'Nightly'
                },
                        vAxis: {
                        title: 'ROS request rate'
                },
                //colors: ['#a52714', '#097138'],
                crosshair: {
                        color: '#000',
                        trigger: 'selection'
                }
        };

        var chart = new google.visualization.LineChart(document.getElementById('chart_div17000'));

        chart.draw(data, options);

    }

    function drawCrosshairs17000Data() {
                var data = new google.visualization.DataTable();

                data.addColumn('string', 'Nightly');

                <?php
                        
                        \$nightlies = glob(dirname(__FILE__) . '/../../*');
                        \$nightlies = array_values(\$nightlies);
                        \$content = file_get_contents(dirname(__FILE__).'/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_17000/ATLASros2rob2018.py');
                        //echo \$content;
                        preg_match_all('/ROS-.[A-Z]+-.[A-Z]+-.[0-9]+/i',\$content,\$ROSes);
                        \$ROSes = array_values(\$ROSes);
                        foreach(\$ROSes as \$ros){
                                \$ros = array_values(\$ros);
                                foreach(\$ros as \$ros1){
                                        echo \"                data.addColumn('number', '\$ros1'); \\n\";
                                }
                        }

                        echo \"                data.addRows([\";
                        foreach(\$nightlies as \$nightly){
                                \$content = file_get_contents(\$nightly.'/TrigP1Test/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_17000/ROStest.reference.new');
                                //echo \$content.'\\n';                           
                                echo \"                ['\".basename(\$nightly).\"',\";
                                foreach(\$ROSes as \$ros){
                                        \$ros = array_values(\$ros);
                                        foreach(\$ros as \$ros1){
                                                //echo \$ros1.'\\n';
						\$matched = preg_match_all('/'.\$ros1.'.[ \\t]+[|].[ \\t]+.[0-9]+.[ \t]+[|](.[.0-9]+).[ \\t]+,(.[.0-9]+).[ \\t]+,.[ \\t]+(.[.0-9]+)/', \$content, \$data);
                                                if(\$matched) echo \$data[3][0].\",\";
                                                else echo \"0.,\";
                                                //print_r(\$data);
                                        }
                                }
                                echo \"],\";
                        }
                        echo \"                ]);\";
                ?>

        var options = {
                title: 'ROS request data volume (in words) in rejected events for lumi 1.7e34',
                hAxis: {
                        title: 'Nightly'
                },
                        vAxis: {
                        title: 'ROS request data volume (words)'
                },
                //colors: ['#a52714', '#097138'],
                crosshair: {
                        color: '#000',
                        trigger: 'selection'
                }
        };

        var chart = new google.visualization.LineChart(document.getElementById('chart_div17000Data'));

        chart.draw(data, options);

    }


    
    function drawCrosshairs9000() {
                var data = new google.visualization.DataTable();

                data.addColumn('string', 'Nightly');

                <?php
                        
                        \$nightlies = glob(dirname(__FILE__) . '/../../*');
                        \$nightlies = array_values(\$nightlies);
                        \$content = file_get_contents(dirname(__FILE__).'/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_9000/ATLASros2rob2018.py');
                        //echo \$content;
                        preg_match_all('/ROS-.[A-Z]+-.[A-Z]+-.[0-9]+/i',\$content,\$ROSes);
                        \$ROSes = array_values(\$ROSes);
                        foreach(\$ROSes as \$ros){
                                \$ros = array_values(\$ros);
                                foreach(\$ros as \$ros1){
                                        echo \"                data.addColumn('number', '\$ros1'); \\n\";
                                }
                        }

                        echo \"                data.addRows([\";
                        foreach(\$nightlies as \$nightly){
                                \$content = file_get_contents(\$nightly.'/TrigP1Test/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_9000/ROStest.reference.new');
                                //echo \$content.'\\n';                           
                                echo \"                ['\".basename(\$nightly).\"',\";
                                foreach(\$ROSes as \$ros){
                                        \$ros = array_values(\$ros);
                                        foreach(\$ros as \$ros1){
                                                //echo \$ros1.'\\n';
                                                \$matched = preg_match_all('/'.\$ros1.'.[ \\t]+[|].[ \\t]+.[0-9]+.[ \t]+[|](.[.0-9]+)/', \$content,\$rates);
                                                if(\$matched) echo \$rates[1][0].\",\";
                                                else echo \"0.,\";
                                                //print_r(\$rates);
                                        }
                                }
                                echo \"],\";
                        }
                        echo \"                ]);\";
                ?>

        var options = {
                title: 'ROS request rates in rejected events for lumi 0.9e34',
                hAxis: {
                        title: 'Nightly'
                },
                        vAxis: {
                        title: 'ROS request rate'
                },
                //colors: ['#a52714', '#097138'],
                crosshair: {
                        color: '#000',
                        trigger: 'selection'
                }
        };

        var chart = new google.visualization.LineChart(document.getElementById('chart_div9000'));

        chart.draw(data, options);

    }

    function drawCrosshairs9000Data() {
                var data = new google.visualization.DataTable();

                data.addColumn('string', 'Nightly');

                <?php
                        
                        \$nightlies = glob(dirname(__FILE__) . '/../../*');
                        \$nightlies = array_values(\$nightlies);
                        \$content = file_get_contents(dirname(__FILE__).'/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_9000/ATLASros2rob2018.py');
                        //echo \$content;
                        preg_match_all('/ROS-.[A-Z]+-.[A-Z]+-.[0-9]+/i',\$content,\$ROSes);
                        \$ROSes = array_values(\$ROSes);
                        foreach(\$ROSes as \$ros){
                                \$ros = array_values(\$ros);
                                foreach(\$ros as \$ros1){
                                        echo \"                data.addColumn('number', '\$ros1'); \\n\";
                                }
                        }

                        echo \"                data.addRows([\";
                        foreach(\$nightlies as \$nightly){
                                \$content = file_get_contents(\$nightly.'/TrigP1Test/test_HLT_physicsV7_menu_ART_and_ROSsim_build/HLT_physicsV7_ROSsim_9000/ROStest.reference.new');
                                //echo \$content.'\\n';                           
                                echo \"                ['\".basename(\$nightly).\"',\";
                                foreach(\$ROSes as \$ros){
                                        \$ros = array_values(\$ros);
                                        foreach(\$ros as \$ros1){
                                                //echo \$ros1.'\\n';
                                                \$matched = preg_match_all('/'.\$ros1.'.[ \\t]+[|].[ \\t]+.[0-9]+.[ \t]+[|](.[.0-9]+).[ \\t]+,(.[.0-9]+).[ \\t]+,.[ \\t]+(.[.0-9]+)/', \$content, \$data);
                                                if(\$matched) echo \$data[3][0].\",\";
                                                else echo \"0.,\";
                                                //print_r(\$data);
                                        }
                                }
                                echo \"],\";
                        }
                        echo \"                ]);\";
                ?>

        var options = {
                title: 'ROS request data volume (in words) in rejected events for lumi 0.9e34',
                hAxis: {
                        title: 'Nightly'
                },
                        vAxis: {
                        title: 'ROS request data volume (in words)'
                },
                //colors: ['#a52714', '#097138'],
                crosshair: {
                        color: '#000',
                        trigger: 'selection'
                }
        };

        var chart = new google.visualization.LineChart(document.getElementById('chart_div9000Data'));

        chart.draw(data, options);

    }


</script>
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
    for (var j=1; j < cells.length; j++) {   // skip name
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
    document.getElementById('nDiffs').innerHTML = '<a href=\"https://gitlab.cern.ch/atlas/athena/compare/nightly%2F21.1%2F'+document.DiffForm.rel.value+'...nightly%2F21.1%2F'+thisRelease+'\">'+diffs+' differences! click here for GitLab diff</a>';
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
    iframe.src = url.replace(/[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9][0-9][0-9]/, diffRelease);
  }
  else {
    highlightDiffs(true);  // remove all highlighting
  }
}

function setRelease() {
  var url = window.document.location.href;
  thisRelease = url.match(/[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9][0-9][0-9]/);
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
<h1>Trigger ART test results summary</h1>
<form name=\"DiffForm\">
";

  
    my $now=qx(date);
    my $testWWWPage=getTestWWWPage();
    my $nicosWWWPage=dirname($testWWWPage);

    print HTMLOUT "<p>Last updated $now. Contact: Daniele.Zanzi @ cern.ch</p>\n";

#	my $atn_relno = $atn_rel;
#	$atn_relno =~ s/rel_//;
#	print HTMLOUT "<p><a href=\"".$testWWWPage."\">Nightly test</a>: $project,$gitbranch,<b>$release</b> ($atn_platform)";
#	print HTMLOUT "<script src=\"".$nicosWWWPage."/build_failures_".$atn_relno.".js\" language=\"JavaScript\"></script>\n";
#	print HTMLOUT "<script type=\"text/javascript\">showBuildFailures(failures_".$atn_relno."(),"."\"".$nicosWWWPage."/nicos_buildsummary_".$atn_relno.".html\")</script>";
#
    my $test_suite = gettestpkgdir(); # this is a basename of pwd (expected to be e.g. TrigP1Test)
    print HTMLOUT "<p>Nightly test: $project, $gitbranch, $release\n\n";
    print HTMLOUT "&emsp;Check other builds: <select name=\"select_other_builds\" onchange=\"location = this.value;\">\n";
    print HTMLOUT "<option value=\"\" selected=\"selected\">-----</option>\n";
    print HTMLOUT "<?php\n";
    print HTMLOUT "             \$nightlies = glob(dirname(__FILE__) . '/../../*');\n";
    print HTMLOUT "             \$nightlies = array_reverse(\$nightlies);\n";
    print HTMLOUT "             \$current_page_link = \"\$_SERVER[REQUEST_URI]\";\n";
    print HTMLOUT "             // protect against multiple slashes in a row\n";
    print HTMLOUT "             \$current_page_link = preg_replace('#/+#','/',\$current_page_link);\n";
    print HTMLOUT "             \$full_link = (isset(\$_SERVER['HTTPS']) ? \"https\" : \"http\") . \"://\$_SERVER[HTTP_HOST]\" . \$current_page_link;\n";
    print HTMLOUT "             foreach(\$nightlies as \$nightly){\n";
    print HTMLOUT "                 \$nightly = basename(\$nightly);\n";
    print HTMLOUT "                 echo \"<option value='\" . \$full_link . \"../../\" . \$nightly . \"/$test_suite/\" . \"'>\".\$nightly.\"</option>\";\n";
    print HTMLOUT "    }\n";
    print HTMLOUT "?>\n";
    print HTMLOUT "</select></p>\n";

    print HTMLOUT "<p>Check differences: <select name=\"rel\" size=\"1\" onchange=\"loadPage()\">\n";
    print HTMLOUT "<option value=\"\" selected=\"selected\">-----</option>\n";
    print HTMLOUT "<?php\n";
    print HTMLOUT "             \$nightlies = glob(dirname(__FILE__) . '/../../*');\n";
    print HTMLOUT "             \$nightlies = array_reverse(\$nightlies);\n";
    print HTMLOUT "             \$current_page_link = \"\$_SERVER[REQUEST_URI]\";\n";
    print HTMLOUT "             // protect against multiple slashes in a row\n";
    print HTMLOUT "             \$current_page_link = preg_replace('#/+#','/',\$current_page_link);\n";
    print HTMLOUT "             \$full_link = (isset(\$_SERVER['HTTPS']) ? \"https\" : \"http\") . \"://\$_SERVER[HTTP_HOST]\" . \$current_page_link;\n";
    print HTMLOUT "             foreach(\$nightlies as \$nightly){\n";
    print HTMLOUT "                 \$nightly = basename(\$nightly);\n";
    print HTMLOUT "                 echo \"<option value='\".\$nightly.\"'>\".\$nightly.\"</option>\";\n";
    print HTMLOUT "    }\n";
    print HTMLOUT "?>\n";
    print HTMLOUT "</select> <span id='nDiffs' style='font-weight:bold'></span>
</form><script type=\"text/javascript\">setRelease();</script></p>";


    print HTMLOUT "\n<table class=\"sortable\" id=\"ATNResults\" border=1><thead><tr>
<th title=\"Click to sort by test suite\">Test name</th> 
<th>Overall Result</th> 
<th title=\"exit code of Athena\">Athena exit</th> 
<th title=\"presence of any error messages in log\">Error Msgs</th> 
<th title=\"number of warning messages in log\">Warn Msgs</th> 
<th>Reg. tests (incl. WARNING)</th> 
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
                  $postrc .= gethistnumber($name);
                } else {                  
                  $postrc = getposttests($name);
                  $postrc .= gethistnumber($name);
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
	print HTMLOUT "<tr class=\"hi\"><td $order title=\"$doc{$name}\">$thename</td> <td>$atn_script</td> <td>$athena_exit</td> <td>$error</td> <td>$warn</td> <td style=\"white-space:nowrap\">$reg_tests</td> <td style=\"white-space:nowrap\">$rootcomp</td> <td>$checkcount</td> <td><a type=\"text/plain\" href=\"$name/$timerout\">$timer</a></td> <td>$exitcode/$maskedcode</td> <td>$postrc</td> <td><a href=\"$name\">dir</a></td><td>$logfile</td></tr>\n";
    }
    print HTMLOUT "</tbody></table>";
    if (-e "$atn_timeline"){
      print HTMLOUT "<font face=\"monospace\"><br>";
      my @lines = `tail -3 *atn_timeline.log`;
      print HTMLOUT join('<br>',@lines);
      print HTMLOUT "<a href=\"atn_timeline.png\"> [timeline plot]</a>";
      print HTMLOUT "</font>";
    }
    print HTMLOUT '<iframe onload="highlightDiffs(false)" id="DiffFrame" style="visibility:hidden;display:none;"></iframe>';
    print HTMLOUT '<div id="chart_div17000" style="float: left; width: 50%; height: 500px"></div>';
    print HTMLOUT '<div id="chart_div9000"  style="float: right; width: 50%; height: 500px"></div>';
    print HTMLOUT '<div id="chart_div17000Data" style="float: left; width: 50%; height: 500px"></div>';
    print HTMLOUT '<div id="chart_div9000Data"  style="float: right; width: 50%; height: 500px"></div>';
    print HTMLOUT '</body></html>';
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

sub gethistnumber(){
  my ($testname) = @_;
  my $text;
  open POSTRC, "<$testname/post_test_checkHist.log" or return "";
  while (<POSTRC>){
    chomp;
    if ($_ =~ /Total histograms/)
    {    
         $text = $_;
         return "<font size=\"-2\">$text</font>";
     }
  }
  return "No hist number found";
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
