#!/usr/bin/perl -w
# Script to manage Trigger tests
# $Id: trigtest.pl,v 1.16 2009-05-20 15:53:22 sgeorge Exp $

use Getopt::Long;
use File::Basename;
use File::Spec;
use File::Copy;
use File::Path;
use constant TRUE => 1;
use constant FALSE => 0;
use constant UNDEFINED => "undefined";
$prog = basename $0;
$config = {};
$fullregtest = '<fullregtest>';
$customregtest = '<customregtest>';
# set some output file names
$regtestout = "regtest.log";
$htmldiffout = "regtest.html";
$summaryout = "summary.log";
$timerout = "timer.log";
$checklogout = "checklog.log";
$warnout = "warn.log";
$rootcompout = "rootcompout.log";
$checkcountout = "checkcountout.log";
$checkmergeout = "checkmergeout.log";
$edmcheckout = "edmcheckout.log";
$postcmdout = "postcmd.log";
$posttestrc = "post_test.exitcodes";
$docout = "doc.txt";

# define error codes - must match atn_summary.pl
%ERROR = (
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
# max bit: 128. Bash takes return codes 0-256.
# Larger numbers are taken mod 256 so effectively masked off. Can be used for internal signalling only.

# mask off those codes above which you do not want to flag in the exit code
# i.e. so ATN does not interpret them as the test failing. 
# bit positions marked 1 will be used, 0 will not be used.
# SCRIPT_MISC is on the right, ATN_TIME_LIMIT on the left
# This default mask can be modified via the 'extra_failure_codes' test directive
# only "shout" if we see/have: ATHENA_BAD_EXIT => 8 decimal 8 in bit is 0b00001000 thus 
$default_exitmask = 0b00001001;

# Exit mask of the last run test. Not nice but the easiest way to implement
# this on top of all the other ugly things in this script.
$exitmask = $default_exitmask;
    
# command to use for rootcomp
$rootcomp_cmd = "rootcomp.py --pdf --noRoot";
$checkcount_cmd = "trigtest_checkcounts.sh";
$checkmerge_cmd = "trigtest_check_merge.sh";
sub main();
main();
exit -10;
####

sub main(){
    # trap errors from here on
    $SIG{'INT'} = 'CLEANUP_INT';
    $SIG{'TERM'} = 'CLEANUP_TERM';
    $t0=time(); # timer
    $failures = 0; # count number of tests which fail
    $statuscode = 0; # combination of errors from last test
    command_line();
    parse_config();
    if ($verify){
	print_config();
    }
    exit if ($verifyconfig);
    check_tests();
    do_tests();
    results_summary();
    timer($t0);
    if ($statuscode != 0){
	prog_error_exit("at end",@statuscodes);
    }
    exit 0;
    # exit($statuscode); # status code of last test
    # exit($failures); # number of tests which fail
}

sub CLEANUP_INT {
    print "\n\n$prog: Caught Interrupt (^C), probably ATN time exceeded, finish up and exit\n";
    push @statuscodes, 'ATN_TIME_LIMIT';
    timer($t0);
    if ($statuscode != 0){
	prog_error_exit("at end",@statuscodes);
    }
    exit(1); # never reaches this line in principle
}

sub CLEANUP_TERM {
    print "\n\n$prog: Caught TERM signal, finish up and exit\n";
    push @statuscodes, 'ATN_TIME_LIMIT';
    timer($t0);
    if ($statuscode != 0){
	prog_error_exit("at end",@statuscodes);
    }
    exit(1); # never reaches this line in principle
}

sub prog_error_exit($$){
    my ($message,@error_strings) = @_ or die;
    my $exitcode_unmasked = process_error_codes(@error_strings);
    # for ATN purposes, mask less significant codes
    my $exitcode = $exitcode_unmasked & $exitmask;
    if ($exitcode!=0){
	printf "$prog: $failkey $message (unmasked=%#.8b exitcode=%#.8b)\n", $exitcode_unmasked, $exitcode;
    } else {
	printf "$prog: $warnkey $message (unmasked=%#.8b exitcode=%#.8b)\n", $exitcode_unmasked, $exitcode;
    }
    exit($exitcode);

}

sub process_error_codes(@) {
    my (@error_strings) = @_;
    my $exitcode=0;
    # | sort | uniq
    my %saw;
    undef %saw;
    @saw{@error_strings} = ();
    my @error_strings_uniq = sort keys %saw;
    print "$prog: debug: all statuscodes: @error_strings\n" if ($debug);
    print "$prog: debug: uniq statuscodes: @error_strings_uniq\n" if ($debug);
    # convert strings, keep numbers as they are, add together
    print "=== These errors occured: ";
    open SUMMARY, ">$summaryout";
    for (@error_strings_uniq){
	my $code;
	if (/^\d+$/){
	    # it's already a number
	    $code = $_;
	} else {
	    # translate string
	    $code = $ERROR{$_};
	}
	$exitcode += $code;
	if ($code != 0){ # print all except ok.
	    print "$_ ($code) ";
	    print SUMMARY "$_ ($code)\n";
	}
    }
    print "\n";
    my $maskedcode = $exitcode & $exitmask;
    print "$prog:process_error_codes() debug exitcode = $exitcode maskedcode = $maskedcode\n" if ($debug);
    print SUMMARY " exitcode = $exitcode\n";
    print SUMMARY " maskedcode = $maskedcode\n";
    close SUMMARY;
    return $exitcode;
}

# command line arguments
sub command_line(){
  # parse command arguments and check usage
  $debug = FALSE;
  $verify = FALSE;
  $verifyconfig = FALSE;
  $configfile = 'trigtest.conf'; # default
  $failkey = 'FAILURE';
  $warnkey = 'WARNING';
  $showsummary = FALSE;
  @tests = ();
  $run_dir = '../run'; # working directory, relative to cmt dir
  $clear_dir = FALSE;
  print "=== Command to repeat this test:\n  $prog @ARGV\n";
  my $result = GetOptions ('help' => \$help,
                           'debug!' => \$debug,
                           'verify!' => \$verify,
                           'verifyconfig!' => \$verifyconfig,
                           'showsummary' => \$showsummary,
                           'failkey=s' => \$failkey,
                           'warnkey=s' => \$warnkey,
                           'config=s' => \$configfile,
			               'test=s' => \@tests,
			               'rundir=s' => \$run_dir,
			               'cleardir!' => \$clear_dir,
			               'package=s' => \$testpackagefq);
  if ($help || !$result) {
      usage();
      prog_error_exit("usage",'SCRIPT_MISC');
  }
  $verify = TRUE if ($verifyconfig);
  if (defined $testpackagefq){
      print "\n$prog info: note that the --package option is deprecated and will be removed in a \nfuture release. It no longer does anything.\n";
  }
}

# usage message
sub usage(){
    print "
  Usage: $prog [options] 

  Testing tool for managing Trigger software tests

  Options:

  --help                show this information
  --debug               print debug information, for the script maintainer
  --verify              verify config file and show what would be done
                        for the tests but don't run athena (see tip below)
  --verifyconfig        just verify config file and stop
  --showsummary         print a short summary table of tests results at the end
  --failkey <string>    keyword to be printed to indicate to the test 
                        framework that a test has failed, default $failkey
  --failkey <string>    keyword to be printed to indicate a problem, but not
                        seen as failure by test framework, default $warnkey
  --config <file>       specify config file, default trigtest.conf
                        tries with get_files from DATAPATH if it doesn't exist 
  --rundir <dir>        specify directory in which to run tests, default ../run
  --cleardir            remove/clear rundir before running (FALSE by default)
  --package <string>    (deprecated, does nothing, will be removed in future release)
  --test <test name|id> specify test to run, see below

  A test name or id can optionally be specified after the --test
  option to run only that test. By default, all tests will be run in
  the order they are defined in the config file. The verify option
  will list all the available tests that are defined in the config
  file. The --test option may be repeated, and tests will be run in the
  order the come on the command line.

  See the sample config file for test configuration information.

  It is expected that this tool be run from the cmt or run directory
  under TriggerTest, after the necessary cmt environment has been set
  up.  If you changed any files in TriggerTest or checked out a
  version different from the release you are using, then you need to
  gmake before running these tests. The --package option can be used
  to change this default location to another package for reference
  files and the check_log config file.

  Output from $prog will always be preceeded by its name (for error
  and debug messages) or '===' for the few messages you get without
  debug. All other messages come from other programs and commands it
  has run.

  Tip: after running a test for the first time, run again in verify
  mode and the same log files will be re-used. This is a good way to
  quickly test the config file, regression tests, and for script
  development.

  The exit code refers only to the last test - be sure to read the 
  output if you run multiple tests!
  The exit code can be any combination of these values:
";
    my $errorcode;
    foreach $error_name (sort { $ERROR{$a} <=> $ERROR{$b} } keys %ERROR) {
	printf "      %20s %4d\n", $error_name, $ERROR{$error_name};
    }
}


# If using a file for stdin, 
# try to find it with get_files
# then check it is there
# and return with < pre-pended,
# or return empty string if failed
sub getStdinFile($){
    my ($stdinfile) = @_;
    $stdinfile =~ s/^\s+$//; # kill file names that are only spaces
    $stdinfile = $stdinfile;
    if (length($stdinfile)>0){
	if (! -e "$stdinfile" ){
	    print "$prog debug: $stdinfile does not exist, so copying it with get_files\n" if ($debug);
	    systemcall("get_files -data $stdinfile");
	} else {
	    print "$prog debug: $stdinfile already exists; not copying it\n" if ($debug);
	}
	if (! -e $stdinfile){
	    prog_error_exit ("stdin defined as '$stdinfile' but this is not found", 'SCRIPT_MISC');
	    $stdinfile = "";
	} else {
	    $stdinfile = "< $stdinfile";
	}
    } else {
	$stdinfile = "";
    }
    return $stdinfile;
}

# parse config file
sub parse_config(){

    # if specified config file does not exist, try get_files. If still not there, fail.
    print "$prog debug: config file is specified as $configfile\n" if ($debug);
    if (! -r $configfile) {
	print "$prog debug: $configfile does not exist, so try copying it with get_files\n" if ($debug);
	systemcall("get_files -data -symlink -remove $configfile",TRUE);
	if (! -r $configfile){ 
	    prog_error_exit ("config file $configfile not found. Specify a config file with the option --config <filename>", 'SCRIPT_MISC');
	}
    }
    open CONFIG, "<$configfile" 
	or prog_error_exit ("failed opening $configfile: $!\n", 'SCRIPT_MISC');
    print "\n$prog debug: start of config file\n" if ($debug);
    my $first = TRUE;
    my $this_test = {};
    my $current_test_id = '';
    while (<CONFIG>){
	chomp;
	# strip out comments
	$endofline = index $_, '#';
	if ($endofline == -1){
	    $endofline = length;
	}
	$line = substr($_, 0, $endofline);
	# strip leading white space
	$line =~ s/^\s+//;
	# skip empty lines and lines with only spaces
	next if (length $line == 0);
	next if (/^\s+$/);
	#print "$prog debug: config $. >$line<\n" if ($debug);
	# test keyword
	if ($line =~ /^test/){
	    if ($line =~ /^test\s+(\w+)/){
		my $id = $1;
		print "$prog debug: config id = >$id<\n" if ($debug);
		if ($current_test_id eq $id or exists $config{$id}){
		    prog_error_exit ("test id $id is not unique",'SCRIPT_MISC');
		}
		# save old test config
		if (! $first){
		    $config{$current_test_id} = $this_test;
		    push @test_order, $current_test_id;
		}
		# new test config
		$first = FALSE;
		$current_test_id = $id;
		$this_test = {};
		# defaults
		$this_test->{'name'} = "test_$id";
		$this_test->{'disabled'} = FALSE;
		$this_test->{'checklog'} = FALSE;
#		$this_test->{'checklog_opts'} = "--config check_log.conf --showexcludestats";
		$this_test->{'checklog_opts'} = "--showexcludestats";
		$this_test->{'datafile'} = UNDEFINED;
		$this_test->{'datafilelink'} = 'ZEBRA.P';
		$this_test->{'joboptions'} = UNDEFINED;
		$this_test->{'pre_condition'} = ();
		$this_test->{'pre_command'} = ();
		$this_test->{'post_command'} = ();
        $this_test->{'post_test'} = ();
		$this_test->{'athena_env'} = 'export STAGE_SVCCLASS=atlascerngroupdisk;export STAGE_HOST=castoratlast3';
		$this_test->{'athena_args'} = UNDEFINED;
		$this_test->{'athena_cmd'} = 'athena.py -b';
		$this_test->{'exitmask'} = $default_exitmask;
		$this_test->{'stdinfile'} = '';
		$this_test->{'regtest'} = ();
        $this_test->{'filterlog'} = '';
		$this_test->{'compresslog'} = FALSE;
		$this_test->{'rootcomp'} = FALSE;
		$this_test->{'rootcomp_cmd'} = "";
		$this_test->{'rootcomp_file1'} = "";
		$this_test->{'rootcomp_file2'} = "";
        $this_test->{'checkcount'} = FALSE;
        $this_test->{'checkcount_file'} = "";
        $this_test->{'checkcount_tolerance'} = 1;
        $this_test->{'checkcount_level'} = "BOTH";
        $this_test->{'checkmerge'} = FALSE;
        $this_test->{'checkmerge_file'} = "";
        $this_test->{'checkmerge_reffile'} = "";
        $this_test->{'checkmerge_tolerance'} = 1;
        $this_test->{'emdcheck'} = ();
		$this_test->{'doc'} = "";
		next;
	    }
	    else {
		print "$prog: Error: config line $. incorrect, test definition invalid - ignoring line\n";
		next;
	    }
	}
	if ($line =~ /^name\s+([^\s]+)/){
	    $this_test->{'name'} = $1;
	    print "$prog debug: config test name >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^datafile\s+([^\s]+)\s+([^\s]+)/){
	    $this_test->{'datafile'} = $1;
	    $this_test->{'datafilelink'} = $2;
	    print "$prog debug: config datafile >$1< link >$2<\n" if ($debug);
	    next;
	}
	if ($line =~ /^datafile\s+([^\s]+)/){
	    $this_test->{'datafile'} = $1;
	    print "$prog debug: config datafile >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^joboptions\s+([^\s]+)/){
	    $this_test->{'joboptions'} = $1;
	    print "$prog debug: config joboptions >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^pre_condition\s+(.+)$/){
	    my $pre_condition = $1;
	    push @{$this_test->{'pre_condition'}}, $pre_condition;
	    print "$prog debug: config pre_condition >$pre_condition<\n" if ($debug);
	    next;
	}
	if ($line =~ /^pre_command\s+(.+)$/){
	    my $pre_command = $1;
	    push @{$this_test->{'pre_command'}}, $pre_command;
	    print "$prog debug: config pre_command >$pre_command<\n" if ($debug);
	    next;
	}
	if ($line =~ /^post_command\s+(.+)$/){
	    my $post_command = $1;
	    push @{$this_test->{'post_command'}}, $post_command;
	    print "$prog debug: config post_command >$post_command<\n" if ($debug);
	    next;
	}
        if ($line =~ /^post_test\s+([^\s]+)\s+(.+)$/){
	    my $post_test = {'name' => $1, 'cmd' => $2};
	    push @{$this_test->{'post_test'}}, $post_test;            
	    print "$prog debug: config post_test >$1< >$2<\n" if ($debug);
	    next;
	}
	if ($line =~ /^athena_args\s+(.+)$/){
	    $this_test->{'athena_args'} = $1;
	    print "$prog debug: config athena_args >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^athena_env\s+(.+)$/){
	    $this_test->{'athena_env'} = $1;
	    print "$prog debug: config athena_env >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^athena_cmd\s+(.+)$/){
	    $this_test->{'athena_cmd'} = $1;
	    print "$prog debug: config athena_cmd >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^extra_failure_codes\s+(.+)$/){
	    $this_test->{'exitmask'} = $default_exitmask;	    
	    my @codes = split('\s',$1);
	    for (@codes) {
		if (exists $ERROR{$_}) {
		    $this_test->{'exitmask'} |= $ERROR{$_};
		}
		else {
		    my @tmp = keys %ERROR;
		    prog_error_exit ("Exit code '$_' unknown. Allowed values are: @tmp",'SCRIPT_MISC');
		}
	    }	    	    
	    print "$prog debug: config extra_failure_codes >$1<\n" if ($debug);
	    next;
	}	
	if ($line =~ /^stdinfile\s+(.+)$/){
	    $this_test->{'stdinfile'} = $1;
	    print "$prog debug: config stdinfile >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^disable/){
	    $this_test->{'disabled'} = TRUE;
	    print "$prog debug: config disabled >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    next;
	}
	if ($line =~ /^checklog\s+(.+)$/){
	    $this_test->{'checklog'} = TRUE;
	    $this_test->{'checklog_opts'} = $1;
	    print "$prog debug: config checklog >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    print "$prog debug: config checklog_opts >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^checklog\s*$/){
	    $this_test->{'checklog'} = TRUE;
	    print "$prog debug: config checklog >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    next;
	}
        if ($line =~ /^filterlog\s+(.+)$/){
	    $this_test->{'filterlog'} = $1;
	    print "$prog debug: config filterlog >$1<\n" if ($debug);
	    next;
	}
	if ($line =~ /^compresslog/){
	    $this_test->{'compresslog'} = TRUE;
	    print "$prog debug: config compresslog >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    next;
	}
	if ($line =~ /^regtest\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)/){
	    my $regtest = { 'algoname' => $1, 'match' => $2, 'reffile' => $3};
	    push @{$this_test->{'regtest'}}, $regtest;
	    print "$prog debug: config regtest >$1< >$2< >$3<\n" if ($debug);
	    next;
	}
	if ($line =~ /^edmcheck\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)/){
	    my $edmcheck = { 'algoname' => $1, 'edmfile' => $2, 'edmreffile' => $3};
	    push @{$this_test->{'edmcheck'}}, $edmcheck;
	    print "$prog debug: config edmcheck >$1< >$2< >$3<\n" if ($debug);
	    next;
	}
	if ($line =~ /^fullregtest\s+([^\s]+)\s+([^\s]+)/){
	    my $regtest = { 'algoname' => $fullregtest, 'match' => $1, 'reffile' => $2};
	    push @{$this_test->{'regtest'}}, $regtest;
	    print "$prog debug: config fullregtest >$1< >$2<\n" if ($debug);
	    next;
	}
        if ($line =~ /^customregtest\s+(.+)$/){
            my $regtest = { 'algoname' => $customregtest, 'match' => $1, 'reffile' => ''};
	    push @{$this_test->{'regtest'}}, $regtest;
	    print "$prog debug: config customregtest >$1<\n" if ($debug);
	    next;
	}
	#check merge
	if ($line =~ /^checkmerge\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)/){
	    $this_test->{'checkmerge'} = TRUE;
	    $this_test->{'checkmerge_file'} = $1;
            $this_test->{'checkmerge_reffile'}= $2;
            $this_test->{'checkmerge_tolerance'}= $3;
	    print "$prog debug: config checkmerge >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    print "$prog debug: config checkmerge_file >$1<\n" if ($debug);
	    print "$prog debug: config checkmerge_reffile >$2<\n" if ($debug);
	    print "$prog debug: config checkmerge_tolerance >$3<\n" if ($debug);
	    next;
	}
        #  check for 3rd argument (L2, EF or BOTH)
	if ($line =~ /^checkcount\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)/){
	    $this_test->{'checkcount'} = TRUE;
	    $this_test->{'checkcount_file'} = $1;
            $this_test->{'checkcount_tolerance'}=$2;
            $this_test->{'checkcount_level'}=$3;
	    print "$prog debug: config checkcount >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    print "$prog debug: config checkcount_file >$1<\n" if ($debug);
	    print "$prog debug: config checkcount_tolerance >$2<\n" if ($debug);
	    print "$prog debug: config checkcount_level >$3<\n" if ($debug);
	    next;
	}
        # only 2 arguments, assume BOTH
	if ($line =~ /^checkcount\s+([^\s]+)\s+([^\s]+)/){
	    $this_test->{'checkcount'} = TRUE;
	    $this_test->{'checkcount_file'} = $1;
            $this_test->{'checkcount_tolerance'}=$2;
	    print "$prog debug: config checkcount >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    print "$prog debug: config checkcount_file >$1<\n" if ($debug);
	    print "$prog debug: config checkcount_tolerance >$2<\n" if ($debug);
	    next;
	}
        # optional 2nd argument
	if ($line =~ /^rootcomp\s+([^\s]+)\s*([^\s]*)$/){
	    $this_test->{'rootcomp'} = TRUE;
	    $this_test->{'rootcomp_cmd'} = $rootcomp_cmd;
            if ($2 eq "") {
              $this_test->{'rootcomp_file1'} = "expert-monitoring.root";
              $this_test->{'rootcomp_file2'} = $1;
            }
            else {
              $this_test->{'rootcomp_file1'} = $1;
              $this_test->{'rootcomp_file2'} = $2;
            }
	    print "$prog debug: config rootcomp >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
	    print "$prog debug: config rootcomp_file1 >$this_test->{'rootcomp_file1'}<\n" if ($debug);            
	    print "$prog debug: config rootcomp_file2 >$this_test->{'rootcomp_file2'}<\n" if ($debug);
	    next;
	}
    # old style                                                                                                                                                                
    if ($line =~ /^customrootcomp\s+([^\s]+)\s+([^\s]+)$/){
        $this_test->{'rootcomp'} = TRUE;
        $this_test->{'rootcomp_cmd'} = $1;
        $this_test->{'rootcomp_file2'} = $2;
        print "$prog debug: config custom rootcomp >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
        print "$prog debug: config custom rootcomp_cmd >$1<\n" if ($debug);
        print "$prog debug: config custom rootcomp_file2 >$2<\n" if ($debug);
        next;
    }
    # optional 3rd argument                                                                                                                                                    
    if ($line =~ /^customrootcomp\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)$/){
        $this_test->{'rootcomp'} = TRUE;
        $this_test->{'rootcomp_cmd'} = $1;
        $this_test->{'rootcomp_file1'} = $2;
        $this_test->{'rootcomp_file2'} = $3;
        print "$prog debug: config custom rootcomp >" . ( $_ ? "true" : "false") . "<\n" if ($debug);
        print "$prog debug: config custom rootcomp_cmd >$1<\n" if ($debug);
        print "$prog debug: config custom rootcomp_file1 >$2<\n" if ($debug);
        print "$prog debug: config custom rootcomp_file2 >$3<\n" if ($debug);
        next;
    }
	if ($line =~ /^doc\s+(.+)$/){
	    $this_test->{'doc'} = $1;
	    print "$prog debug: config doc >$1<\n" if ($debug);
	    next;
	}
	else {
	    print "$prog: Warning: config line $. not recognised, ignoring it: $line\n";
	    next;
	}
    }
    close CONFIG;
    print "$prog debug: end of config file\n\n" if ($debug);
    # save last test config
    if (! $first){
	$config{$current_test_id} = $this_test;
	push @test_order, $current_test_id;
    }

}

# display details of the configured tests
sub print_config(){
    print "=== Test Configuration\n";
    foreach (keys %config){
	print "test: $_\n";
	print "  name '" . $config{$_}->{'name'} . "' \n";
	print "  doc '" . $config{$_}->{'doc'} . "'\n";
	print "  disabled '" . ( $config{$_}->{'disabled'} ? "true" : "false") . "'\n";
	print "  checklog '" . ( $config{$_}->{'checklog'} ? "true" : "false") . "'\n";
	print "  checklog_opts '" . $config{$_}->{'checklog_opts'} . "'\n";
        print "  filterlog '" . $config{$_}->{'filterlog'} . "'\n";
	print "  compresslog '" . ( $config{$_}->{'compresslog'} ? "true" : "false") . "'\n";
	print "  rootcomp '" . ( $config{$_}->{'rootcomp'} ? "true" : "false") . "'\n";
	print "  rootcomp_cmd '" . $config{$_}->{'rootcomp_cmd'} . "'\n";
	print "  rootcomp_file1 '" . $config{$_}->{'rootcomp_file1'} . "'\n";        
	print "  rootcomp_file2 '" . $config{$_}->{'rootcomp_file2'} . "'\n";
	print "  checkcount '" . ( $config{$_}->{'checkcount'} ? "true" : "false") . "'\n";
	print "  checkcount_file '" . $config{$_}->{'checkcount_file'} . "'\n";
	print "  checkcount_tolerance '" . $config{$_}->{'checkcount_tolerance'} . "'\n";
	print "  checkcount_level '" . $config{$_}->{'checkcount_level'} . "'\n";
	print "  checkmerge '" . ( $config{$_}->{'checkcount'} ? "true" : "false") . "'\n";
	print "  checkmerge_file '" . $config{$_}->{'checkcount_file'} . "'\n";
	print "  checkmerge_tolerance '" . $config{$_}->{'checkcount_tolerance'} . "'\n";
	print "  checkmerge_reffile '" . $config{$_}->{'checkcount_file'} . "'\n";
	print "  datafile '" . $config{$_}->{'datafile'} . "'\n";
	print "  datafilelink '" . $config{$_}->{'datafilelink'} . "'\n";
	print "  joboptions '" . $config{$_}->{'joboptions'} . "'\n";
	for (@{$config{$_}->{'pre_condition'}}){
	    print "  pre_condition $_'\n";
	}
	for (@{$config{$_}->{'pre_command'}}){
	    print "  pre_command $_'\n";
	}
	for (@{$config{$_}->{'post_command'}}){
	    print "  post_command $_'\n";
	}
	for (@{$config{$_}->{'post_test'}}){
	    print "  post_test $_'\n";
	}        
        print "  athena_env'"  . $config{$_}->{'athena_env'} . "'\n";
	print "  athena_cmd '" . $config{$_}->{'athena_cmd'} . "'\n";
	print "  athena_args '" . $config{$_}->{'athena_args'} . "'\n";
	printf "  exitmask %#.8b \n", $config{$_}->{'exitmask'};
	print "  stdinfile '" . $config{$_}->{'stdinfile'} . "'\n";
	if ($#{$config{$_}->{'regtest'}} == -1){
	    print "  (no regtests defined)\n";
	}
	for (@{$config{$_}->{'regtest'}}){
	    print "  regtest: algorithm name '" . $_->{'algoname'} . "'\n";
	    print "           match message pattern '" . $_->{'match'} . "'\n";
	    print "           reference file '" . $_->{'reffile'} . "'\n";
	}
	if ($#{$config{$_}->{'edmcheck'}} == -1){
	    print "  (no edmtests defined)\n";
	}
	for (@{$config{$_}->{'edmcheck'}}){
	    print "  edmcheck: algorithm name  '" . $_->{'algoname'} . "'\n";
	    print "           edmfile          '" . $_->{'edmfile'} . "'\n";
	    print "           reference file   '" . $_->{'edmreffile'} . "'\n";
	}
    }
}

# check that tests specified on command line exist
# replace names by ids
sub check_tests(){

    # build map of tests by name
    my $test_id_byname = {};
    foreach (keys %config){
	my $name = $config{$_}->{'name'};
	if (exists $test_id_byname{$name}){
	    print "$prog: error: test name $name is not unique\n";
	} else {
	    $test_id_byname{$name} = $_;
	}
    }   
    
    # check that tests specified on command line exist
    # replace names by ids
    for ( my $i=0; $i<=$#tests; $i++ ){
	if ( exists $config{$tests[$i]} ){
	    next;
	} elsif ( exists $test_id_byname{$tests[$i]} ) {
	    $tests[$i] = $test_id_byname{$tests[$i]};
	    next;
	} else {
	    prog_error_exit ("test name/id $tests[$i] is not defined in the config file", 'SCRIPT_MISC');
	}
    }

    # if no tests are specified, do them all in the order they were read in.
    if ($#tests == -1){
	@tests = @test_order;
    }
}


# loop over tests, skip those that are disabled
sub do_tests(){
    my $first = TRUE;
    for (@tests){
	my $id = $_;	
	@statuscodes = ( 'OK' ); # reset for status code of latest test
	$exitmask = $config{$id}->{'exitmask'};	# exitmask of latest test
	#print "-------------------------------------------------------\n";
	print "=== Test $id: " . $config{$id}->{'name'};
	if ($config{$id}->{'disabled'}) {
	    print " is disabled\n";
	} else {
	    print "\n";
	    if ($first){
		first_test_prep();
		$first = FALSE;
	    }
 	    run_test($id);
	    if ($#statuscodes>0){
		$failures++;
	    }
	    $statuscode = process_error_codes(@statuscodes);
	    $testresults{$id}=$statuscode;	    
	}
	print "--------------------------------------------------------------------------------\n";
    }
}

# Timer subroutine
# given an initial time t0 in seconds since the epoch (i.e. as given by perl time function), calculates and prints the total elapsed (aka wallclock) time, user time and system time. Do not expect these to add up, e.g. if the system is busy with other users or you have to wait for castor.
# NB if called multiple times, the last call will overwrite previous output files.
sub timer($){
    my ($t0) = @_;
    my $t1=time();
    my ($tuser,$tsystem,$tcuser,$tcsystem) = times;
    printf( "=== Timer: total elapsed %d s, total user %6.2f s, total system %6.2f s\n", 
	    $t1-$t0, $tuser+$tcuser, $tsystem+$tcsystem);
    open TIMER, ">$timerout";
    printf TIMER "%d %f %f\n",$t1-$t0, $tuser+$tcuser, $tsystem+$tcsystem;
    close TIMER;
    
}

sub results_summary(){
    return if (!$showsummary);
    print "=== Test results summary\n";
    printf "  %3s %-20s %-10s\n", "Id", "Test Name", "Result";
    for (@tests){
	my $id = $_;
	printf "  %3s %-20s ", $id, $config{$id}->{'name'};
	if ($config{$id}->{'disabled'}){
	    print "(disabled)\n";
	    next;
	}
	if ($testresults{$id} == 0){
	    print "passed";
	} else {
	    print "failed ($testresults{$id})";
	}
	print "\n";
    }
}

# preparations before first test is run
sub first_test_prep {

    # create run dir if it doesn't already exist
    if ( $clear_dir ) {
        system "rm -fr $run_dir";
    }
    if ( ! -d $run_dir ){
	mkpath $run_dir 
	    #or die "$prog: error: failed mkpath $run_dir: $!\n";
    }
    chdir $run_dir or prog_error_exit( "failed chdir $run_dir: $!\n", 'SCRIPT_MISC');

    # get log file from env
    my $ajl = $ENV{'ATN_JOB_LOGFILE'};
    if (defined($ajl)){
	system "echo $ajl > atn_job_logfile_name";
    }

    # run script to get relevant file
    my $id = $_;
    my $jobopts = $config{$id}->{'joboptions'};
    print "$prog debug: job options:$jobopts\n" if ($debug);
}

# run a test
sub run_test($){
    my ($id) = @_;
    my $name = $config{$id}->{'name'};

    # prepare file names
    my $jobopts = $config{$id}->{'joboptions'};
    my $logfile = "atn_test.log";
    my $logfiletail = "atn_tail.log";
    my $local_jobopts = "";
    print "$prog debug: job options:$jobopts\n" if ($debug);
    systemcall("echo \"$config{$id}->{'doc'}\" > $docout");
    if ( $jobopts  ne 'None' and $jobopts ne UNDEFINED ){
        my @suffixlist = ( '.py', '.txt' );
        my ($filename,$path,$suffix) = fileparse($jobopts,@suffixlist);
        $local_jobopts = $filename . "_test" . $suffix;
        $logfile = "$filename" . "_test.log";
        $logfiletail = "$filename" . "_tail.log";
        print "$prog debug: $jobopts  $local_jobopts $logfile\n" if ($debug);
    
    # get copy of joboptions file to customise for this job, 
    # if it's not already there
    # allow for the case of no jobotions (AthanMT)
        if (! -e "$filename$suffix" ){
	    print "$prog debug: $filename$suffix does not exist, so copying it with get_files\n" if ($debug);
	    systemcall("get_files -jo $jobopts");
        }
        if (-e "$filename$suffix"){ # check: will not be there in verify mode
	    copy "$filename$suffix", "$local_jobopts"
	        or prog_error_exit( "failed copying $filename$suffix to $local_jobopts: $!\n", 'SCRIPT_MISC');
        }
        # check permissions on local joboptions file before trying to append to it
        if (! -w $local_jobopts){
	    chmod 0644, $local_jobopts
	         or prog_error_exit( "failed to set write permission for $local_jobopts: $!\n", 'SCRIPT_MISC');
        }
     

        # append joboptions to set algorithm output level to 2, 
        # for algorithms to be regression tested.
        open JOBOPTS, ">> $local_jobopts" 
	    or prog_error_exit ("failed opening $local_jobopts: $!\n", 'SCRIPT_MISC');
        print JOBOPTS "\n";
#    for (@{$config{$id}->{'regtest'}}){
#	my $algoname = $_->{'algoname'};
	# require some alphanumeric chars for a valid algoname
#	if ($algoname =~ /\w/){
#	    print "$prog debug: appending to joboptions file: $algoname.Outputlevel = 2\n"  if ($debug);
#	    print JOBOPTS "Algorithm(\"$algoname\").Outputlevel = DEBUG\n";
#	print JOBOPTS "MessageSvc.debugLimit = 2000\n";
#	    print JOBOPTS "MessageSvc.defaultLimit = 2000\n";
#	} else {
#	    print "$prog warning: not setting output level for algorithm \"$algoname\" since it doesn't look like a valid name (no alphanumeric chars)\n";
#	}
#    }
        close JOBOPTS;
    }
    # run any pre-condition - they must all succeeed or else the job won't run
    my $pre_condition; 
    my $precondrc=TRUE;
    for $pre_condition (@{$config{$id}->{'pre_condition'}}){
	my $precondrc1=systemcall("$pre_condition");
	print "$prog debug: pre_condition \"$pre_condition\" returned $precondrc1\n" if ($debug);
	if ($precondrc1!=0){
	    print "$prog info: pre_condition \"$pre_condition\" failed (exit code $precondrc1)\n";
	    $precondrc=FALSE;
	}
    }
    if ($precondrc){
	print "$prog debug: pre-conditions met\n" if ($debug);
    } else {
	print "$prog info: pre-conditions failed so test cannot be run. This is not a test failure.\n";
	push @statuscodes, 'PRECONDITION_FAILED_SO_NOT_RUN';
	return;
    }

    # run any pre-command
    my $pre_command; 
    for $pre_command (@{$config{$id}->{'pre_command'}}){
	systemcall("$pre_command");
    }

    # link data file if specified
    my $datafile = $config{$id}->{'datafile'};
    my $datafilelink = $config{$id}->{'datafilelink'};
    if ($datafile ne UNDEFINED){
	# expand environment variables in datafile
	while ($datafile =~ /\$\{(.+)\}/){ 
	    my $var = $1;
	    if (exists $ENV{$var}){ 
		my $sub=$ENV{$var};
		$datafile =~ s/\$\{$var\}/$sub/;
		print "$prog debug: expanding env var '$var' in datafile yields: $datafile\n" if ($debug);
	    } else {
		print "$prog: Warning: env var '$var' used in datafile definition is undefined, replacing with blank\n";
		$datafile =~ s/\$\{$1\}//;
	    }
	}

	print "$prog debug: linking data file to $datafilelink: $datafile\n"  
	    if ($debug);
	if (-e $datafilelink or -l $datafilelink){
	    print "$prog debug: first removing $datafilelink\n" if ($debug);
	    unlink $datafilelink
		or die "$prog: error: failed to delete $datafilelink: $!\n";
	}
	symlink $datafile, $datafilelink
	    or die "$prog: error: failed to symlink $datafilelink: $!\n";
    }

    # run athena
    my $athena_env = $config{$id}->{'athena_env'};
    my $athena_cmd = $config{$id}->{'athena_cmd'};
    my $athena_args = $config{$id}->{'athena_args'};
    my $filter_cmd = $config{$id}->{'filterlog'};
    $filter_cmd = " 2>&1 | " . $filter_cmd if (length($filter_cmd)>0);
    $athena_args = "" if ($athena_args eq UNDEFINED);
    my $stdinfile = getStdinFile($config{$id}->{'stdinfile'});
    print "=== Athena env '$athena_env' \n";
    print "=== Running '$athena_cmd $athena_args $local_jobopts $stdinfile $filter_cmd'\n";
    # the stderr redirection needs to be exactly at this place
    my $athenarc = systemcall("$athena_env ; $athena_cmd $athena_args $local_jobopts $stdinfile $filter_cmd > $logfile 2>&1");

    # the way to recognise a timeout
    $timeout = FALSE;
    if (! open LOG, "tail -600 $logfile|"){
	print "$prog: error: failed opening $logfile to read: $!\n";
	print "-> $failkey Aborting this test\n";
	# will happen later: return; # go on to next test
    }
    while (<LOG>){
	if (/CRITICAL stopped by user interrupt/ or
        /ERROR Keyboard interruption caught/ or
	    /^Signal handler: Killing [0-9]+ with 15/){
	    $timeout = TRUE;
	    last;
	}

    }
    close LOG;

    print "$prog: debug: timeout=$timeout " . ($timeout?"true":"false") . " athenarc=$athenarc \n" if ($debug);

    # check athena return code
    #print "-------------------------------------------------------\n";
    if ($athenarc == 0 and not $timeout){
	print "=== $name OK: Athena exited normally\n";
        print "art-result: 0 athena.$name\n";
    } elsif ($timeout){
    # or $athenarc == 35584 -> sometimes this exit code is just a seg fault
	print "=== $name $warnkey: Athena exceeded time limit and was killed\n";
        print "art-result: 1 athena.$name\n";
	push @statuscodes, 'ATN_TIME_LIMIT';
    } else {
	print "=== $name $failkey : Athena exited abnormally! Exit code: $athenarc\n";
        print "art-result: 2 athena.$name\n";
	push @statuscodes, 'ATHENA_BAD_EXIT';
    }

	
    # run any post-commands
    unlink "$postcmdout" if (-f "$postcmdout");
#    if 
    if (defined $config{$id}->{'post_command'} 
	and   @{$config{$id}->{'post_command'}}>0) {
      my $post_command;
      @postrc=();

      for $post_command (@{$config{$id}->{'post_command'}}){
        # All post command share the same log file
	push @postrc, systemcall("($post_command) >> $postcmdout 2>&1");
      }
      systemcall("cat $postcmdout");
      print "$prog debug: post_command #commands: ".@postrc."  return codes: @postrc \n" if ($debug);
    }

    #my %reldata = release_metadata();
    my $nightly = '';
    #if ( exists $reldata{'nightly name'} ) {
    #  $nightly = $reldata{'nightly name'};
    #  print "nightly name: ".$nightly."\n";
    #} 
    if( defined $ENV{'AtlasBuildBranch'} ) {
      $nightly = $ENV{'AtlasBuildBranch'}; 
      #print "AtlasBuildBranch: ".$nightly."\n";
    } elsif ( defined $ENV{'gitlabTargetBranch'} ) {  #fix for ATR-19383
      $nightly = $ENV{'gitlabTargetBranch'}; 
      #print "gitlabTargetBranch: ".$nightly."\n";
    } else {
      #print "nightly unknown\n";
      $nightly = 'UNKNOWN';
    }
    # modification for ART (ATR-17618)
    # $nightly = $nightly . "/latest";
    print "$prog: looking for histograms and references for nightly $nightly \n";

    # run any post-tests    
    unlink "$posttestrc" if (-f "$posttestrc");
    if ( defined(@{$config{$id}->{'post_test'}}) ) {
      my $post_test;
      my $total_rc = 0;
      @postrc=();
      open (POSTTEST, '>', $posttestrc);
      for $post_test (@{$config{$id}->{'post_test'}}){
        # Each post test has its own log file
        my $post_test_log = "post_test_$post_test->{'name'}.log";
        # Replace '$logfile' with the actual file name
        my $post_test_cmd = $post_test->{'cmd'};
        $post_test_cmd =~ s/\$logfile/$logfile/;
	# If there is a comparison to a reference file, make sure that the correct path is used
        $post_test_cmd =~ s/latest/$nightly/g;
        my $rc = systemcall("($post_test_cmd) > $post_test_log 2>&1");
        $total_rc += $rc;
        # Save return codes in separate text file
        print POSTTEST "$post_test->{'name'} $rc\n";
      }
      close POSTTEST;
      if ($total_rc>0) {
        push @statuscodes, 'POST_TEST_BAD_EXIT';
      }
    }

    my $pwd = `pwd`;
    chomp $pwd;
    # make a URL by substituting the lxbuild local disk path for the web 
    # server URL if it does not match the lxbuild local disk path then it 
    # will be left as a file path instead. 
    # NB gives wrong address for incremental nightlies because the build is always done in atlrel_0 then copied to altrel_1-5 as appropriate
    my $logfileURL = $logfile;
    $logfileURL =~ s%/pool/build/atnight/localbuilds/nightlies%http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/nightlies%;
    print "=== Log file: " . $pwd . "/$logfileURL\nNB replace rel_0 with actual nightly in this URL.\n";
    if (! -e $logfile or -z $logfile){
	print "=== Error: log file does not exist or is empty, skipping the rest of this test\n";
	return;
    }
    # get data file LFN/PFN from log file + pool file catalogue
    if (! open LOG, "<$logfile"){
	print "$prog: error: failed opening $logfile to read: $!\n";
	print "-> $warnkey Aborting this test\n";
	return; # go on to next test
    }
    my $dataLFN="unknown";
    my $dataPFN="";
    my $eventCounter=0;
    while (my $line = <LOG>){
      if ($line =~ /InputCollections\s+\= \['(.*)'\]/){  # athena
	$dataLFN=$1;
      }
      elsif ($line =~ /event stream from file list\s+\['(.*)'\]/){  # athenaHLT
	$dataLFN=$1;
      }
      # count events for athena job
      ++$eventCounter if ($line =~ /AthenaEventLoopMgr\s+INFO\s+===>>>  done processing event/);
      # handle athenaHLT case
      if ($line =~ /Py:athenaHLT\s+INFO\s+Processed ([0-9]+) events/){
        $eventCounter=$1;
      }
    }
    close LOG;

    print "$prog debug: dataLFN=$dataLFN\n" if ($debug);
    if ($dataLFN ne "unknown"){
	my $dataLFNstripped = $dataLFN;
	$dataLFNstripped =~ s/^LFN\://;
	my $cmdout1 = qx(FClistPFN -u xmlcatalog_file:PoolFileCatalog.xml -l $dataLFNstripped);
	chomp $cmdout1;
	if (length($cmdout1)>0){
	    $dataPFN = $cmdout1;
	}
	print "$prog debug: dataLFNstripped=$dataLFNstripped dataPFN=$dataPFN\n" if ($debug);
	print "$prog debug: cmdout1=$cmdout1\n" if ($debug);
    }
    print "=== Data file: $dataLFN   $dataPFN\n";

    # write how many events processed
    print "=== Events processed: $eventCounter\n";

    # check log file for likely errors and known false positives
    if ($config{$id}->{'checklog'}){
      my $checklog_opts =  $config{$id}->{'checklog_opts'};
      # ERRORs
      my $logrc = systemcall("check_log.pl $checklog_opts $logfile > $checklogout 2>&1");
      systemcall("cat $checklogout");
      if ($logrc != 0){
	print "=== $name $failkey : problem detected in log file\n";
        print "art-result: 1 $name.CheckLog\n";
	push @statuscodes, 'ATHENA_ERROR_IN_LOG';
      } else {
        print "art-result: 0 $name.CheckLog\n";
      }
      # WARNINGs
      $rc = systemcall("check_log.pl $checklog_opts --noerrors --warnings $logfile > $warnout 2>&1");
      # Ignore rc.
    }

    #make short file with last 600 lines only
    systemcall("tail -600 $logfile > $logfiletail");
 

    # rootcomp
    if ($config{$id}->{'rootcomp'}){
      	my $rootcomp_file1 =  $config{$id}->{'rootcomp_file1'};
        my $rootcomp_file2 =  $config{$id}->{'rootcomp_file2'};
        if( $nightly ne "" ) {
           # allow for custom tests with two files
           $rootcomp_file2  =~ s/latest/$nightly/g ;
	}
        $rootcomp_file1 = resolveSymlinks($rootcomp_file1);
        $rootcomp_file2 = resolveSymlinks($rootcomp_file2);
    
    if (-e $rootcomp_file1) {
        if (-e $rootcomp_file2) {
	my $rc=systemcall("$config{$id}->{'rootcomp_cmd'} $rootcomp_file1 $rootcomp_file2 > $rootcompout 2>&1", TRUE);
 	if ($rc != 0){
	    print "=== $name WARNING: monitoring histogram mismatch detected by rootcomp\n";
            print "art-result: 1 $name.RootComp\n";
	    push @statuscodes, 'ROOTCOMP_MISMATCH';
	} else {
	    print "=== $name: rootcomp: monitoring histograms match. \n";
            print "art-result: 0 $name.RootComp\n";
	}
        }
        else {
            print "=== Alert: $rootcomp_file2 does not exist \n";
            print "art-result: 2 $name.RootComp\n";
        }
    }
        else {
            print "=== Alert: $rootcomp_file1 does not exist \n";
            print "art-result: 2 $name.RootComp\n";
        }
	#systemcall("cat $rootcompout");
        if (-e $rootcompout) {
	systemcall("grep -A 5 '^Summary' $rootcompout");
        }
      }

    # checkcount
    if ($config{$id}->{'checkcount'}){
	my $checkcount_file =  $config{$id}->{'checkcount_file'};
        my $checkcount_tolerance = $config{$id}->{'checkcount_tolerance'};
        my $checkcount_level = $config{$id}->{'checkcount_level'};
        if( $nightly ne "" ) {
           $checkcount_file  =~ s/latest/$nightly/ ;
	}
        $checkcount_file = resolveSymlinks($checkcount_file);
	print "$prog: checking trigger and TE counts \n";
        print "=== Running $checkcount_cmd $checkcount_tolerance $checkcount_file $checkcount_level > $checkcountout 2>&1 \n";
	my $rc=systemcall("$checkcount_cmd $checkcount_tolerance $checkcount_file $checkcount_level > $checkcountout 2>&1", TRUE);
 	if ($rc != 0){
	    print "=== $name WARNING: trigger count mismatch detected by checkcount\nSee $checkcountout for details.\n";
            print "art-result: 1 $name.CheckCounts\n";
	    push @statuscodes, 'CHECKCOUNTS_FAILED';
	} else {
	    print "=== $name: countcheck: trigger counts match. \n";
            print "art-result: 0 $name.CheckCounts\n";
	}
	#systemcall("cat $rootcompout");
        #Grep for FAILURE here so that bad runs will be flagged as such  
	systemcall("grep 'FAILURE' $checkcountout");
	systemcall("grep 'test warning' $checkcountout");
	systemcall("grep 'tolerance' $checkcountout");
    }

 # checkmerge
    if ($config{$id}->{'checkmerge'}){
	my $checkmerge_file =  $config{$id}->{'checkmerge_file'};
        my $checkmerge_tolerance = $config{$id}->{'checkmerge_tolerance'};
        my $checkmerge_reffile = $config{$id}->{'checkmerge_reffile'};
        if( $nightly ne "" ) {
           $checkmerge_file  =~ s/latest/$nightly/ ;
	}
        $checkmerge_file = resolveSymlinks($checkmerge_file);
	print "$prog: checking trigger histograms for merged system \n";
        print "=== Running $checkmerge_cmd $checkmerge_file $checkmerge_reffile $checkmerge_tolerance > $checkmergeout 2>&1 \n";
	my $rc=systemcall("$checkmerge_cmd $checkmerge_file $checkmerge_reffile $checkmerge_tolerance > $checkmergeout 2>&1", TRUE);

 	if ($rc != 0){
	    print "=== $name WARNING: Errors detected by checkmerge\nSee $checkmergeout for details.\n";
            print "art-result: 1 $name.CheckMerge\n";  
	    push @statuscodes, 'POST_TEST_BAD_EXIT';
	} else {
	    print "=== $name: mergecheck: the test passed successfully. \n";
            print "art-result: 0 $name.CheckMerge\n";
	}
	systemcall("grep 'ERROR' $checkmergeout");
	systemcall("grep 'WARNING' $checkmergeout");
    }
    
   # EDM tests
    my $emdcheckresult=TRUE;
  EDMCHECK:
    for (@{$config{$id}->{'edmcheck'}}){
        # truncate algorithm name for 30 characters
	my $algoname = $_->{'algoname'};
	my $truncalgoname = substr($algoname, 0, 30);
	my $edmfile = $_->{'edmfile'};
	my $edmreffile = $_->{'edmreffile'};
	# if file exist for this sepecfic nightly use it, otherwise
	# use default
        if( $nightly ne "" ) {
	  $newedmreffile =  $edmreffile;
	  # substitute  e.g. devval/latest for latest
	  $newedmreffile =~ s/latest/$nightly/ ;
	  # check if the reference file for this nightly exists
	  if ( -e $edmreffile ) {
	      $edmreffile = $newedmreffile;
	      print "New reference file name $edmreffile \n";
	  }
	}
        $edmreffile = resolveSymlinks($edmreffile);
	my $rc=systemcall("$truncalgoname -f $edmfile -r $edmreffile >>$edmcheckout 2>&1", TRUE);
        if ($rc != 0){
	    print "=== $name WARNING: trigger edmcheck mismatch detected by $truncalgoname\nSee $edmcheckout for details.\n";
            print "=== $name if differences are understood cp $edmfile  $edmreffile \n";
            print "art-result: 1 $name.EdmCheck\n"; 
	} else {
	    print "=== $name: edmcheck: files matched \n";
            print "art-result: 0 $name.EdmCheck\n";  
	}
    }

    # regression tests
    my $regtestresult=TRUE;
  REGTEST:
    for (@{$config{$id}->{'regtest'}}){
        # truncate algorithm name for 15 characters
	my $algoname = $_->{'algoname'};
	my $truncalgoname = substr($algoname, 0, 30);
	my $match = $_->{'match'};
	my $reffile = $_->{'reffile'};
	my $newfile = $reffile . ".new";
        # reference file caser
        if( substr($newfile, 0, 5) eq "/tmp/" ){ 
	  # ref file is in /tmp, eg. from another tst (due to tmp filling up, avoid this)
	  $newfile = substr($newfile, 5);
	} elsif ( substr($newfile,0,3) eq "../"){
	  # reference is to another test -- other test must be first in test/xxx_TestConfigruation.xml
	  my $lslash = rindex $newfile, '/';
          $newfile = substr($newfile, $lslash+1 );
          # in this case ref-file for an existing test will end in .new
          $reffile = $reffile . ".new";
	}  elsif      ( substr($newfile, 0, 7) eq "/cvmfs/" ){
          # should be usual case: ref file is in cvmfs for ART (was /afs/ for ATN)  
          my $lslash = rindex $newfile, '/';
          $newfile = substr($newfile, $lslash+1 );
          if( $nightly ne "" ) {
	    $newreffile =  $reffile;
	    # substitute  e.g. devval/latest for latest
	    $newreffile =~ s/latest/$nightly/ ;
	    # check if the reference file for this nightly exists
	    if ( -e $newreffile ) {
	      $reffile = $newreffile;
	      print "New reference file name $reffile \n";
	    }
	  }
	}
	print "$prog debug: truncated algoname $truncalgoname\n" if ($debug);
	# extract regression test lines for this algorithm from log file
	open NEW, ">$newfile"
	    or die "$prog: error: failed opening $newfile to write: $!\nart-result: 2 $name.Regtest\n";
	if (! open LOG, "<$logfile"){
	    print "$prog: error: failed opening $logfile to read: $!\n";
	    print "-> $warnkey Aborting this test\nart-result: 2 $name.Regtest\n";
	    return FALSE; # go on to next test
	}
	my $lines=0;
	if ($algoname eq $fullregtest){
	    while (<LOG>){
		if (/$match/){
		    print NEW;
		    $lines++;
		}
	    }
	    print "$prog debug: fullregtest $lines lines matched $match in LOG\n" if ($debug);
        } elsif ($algoname eq $customregtest){
          $rc = systemcall("$match >> $regtestout", TRUE);
          $regtestresult = ( $rc==0 ? TRUE : FALSE );
          next REGTEST;
        }
        else {
	    while (<LOG>){
		if (/$truncalgoname.*$match/){
		    print NEW;
		    $lines++;
		}
	    }
	    print "$prog debug: regtest $lines lines matched $truncalgoname $match in LOG\n" if ($debug);
	}

	close LOG;
	close NEW;
	# check whether any lines matched - if zero it's an error straight away
	# and no point in doing the diff
	if ($lines == 0){
	    # print failure keyword here to flag it as an error
	    print "=== Alert! $warnkey no lines matching $match were found for $truncalgoname in log file\n";
            print "art-result: 2 $name.Regtest\n";
	    $regtestresult = FALSE;
	    next REGTEST;
	}
	# diff the output and the reference
	# even if verify is off
	my $rc = 0;
        $reffile = resolveSymlinks($reffile);
        print  "old/reference file $reffile\n";
        print  "new/test      file $newfile\n";
        print  "< old/reference\n> new/test\n";
	print substr($reffile, 0, 4);
# identify location of ref file
	if( substr($reffile, 0, 4) eq "/tmp" ){
	  print "=== This test looks for reference in /tmp directory\n";
	} elsif( substr($reffile, 0, 3) eq "../" ){
	  print "=== This test looks for reference in another directory\n";
        } elsif (substr($reffile, 0, 1) eq "/" ){
	  print "=== This test looks for reference in /cvmfs or /afs or /localhome directory\n";
  	} else {
	  print "=== This test looks for reference from svn\n";
	}
        if (-e $reffile) {
            #            print "old/reference file exists!\n";
	systemcall("echo -e 'total lines in old/reference:' `wc -l $reffile` >>$regtestout",TRUE); 
	systemcall("echo -e 'total lines in new/test     :' `wc -l $newfile` >>$regtestout",TRUE); 
	systemcall("echo -e '< old/reference\n> new/test' >> $regtestout", TRUE);
	$rc = systemcall("diff $reffile $newfile", TRUE);
	$rc = systemcall("diff $reffile $newfile >> $regtestout", TRUE);

	# Do HTML diff if differences are small enough
	$difflines = `wc -l $regtestout | cut -d' ' -f1`;
	if ($difflines < 1000) {
	    systemcall("htmldiff.py -w 80 $reffile $newfile > $htmldiffout 2>&1", TRUE);
	}
	if ($rc == 0){
	    print "=== Output is the same as reference for $algoname\n";
            print "art-result: 0 $name.Regtest\n";
	} else {
	    # print failure keyword here to flag it as an error
	    print "=== Alert! $warnkey Output differs from reference for $algoname\n";
	    print "    If this change is understood, please update the reference file";
            if(substr($reffile, 0, 6) eq "/cvmfs" ){
		print "by typing:\n    cp $newfile $reffile\n";
	    } else {
		print ".\n";
	    }
            print "art-result: 1 $name.Regtest\n";
	    $regtestresult = FALSE;
	    next REGTEST;
	}
        }
        else {
          print "=== Alert! old/reference file: $reffile does not exist - check if this is a new release!\n";  
          print "art-result: 2 $name.Regtest\n";
        }
    }
    if (!$regtestresult){
	push @statuscodes, 'ATHENA_REGTEST_FAILED';
    }

    # compress log file
    if ($config{$id}->{'compresslog'}){
	systemcall("gzip -9 $logfile");
    }

#    print "$prog debug: returning result $result (" 
#	. ($result ? "true" : "false") . ")\n"      if ($debug);
    return;
}


# wrap system calls to provide for verify and debug modes.
# second, optional argument overrides verify mode if TRUE
sub systemcall($;$){
    my $override_verify = FALSE;
    my $command;
    ($command, $override_verify) = @_;
    
    my $rc = 0;
    if ($verify or $debug){
	print "$prog: command: $command\n"; 
    }
    if (!$verify or $override_verify){
        $rc = system ($command); 
    }
    $rc = ($rc >> 8);  # see http://perldoc.perl.org/functions/system.html
    if ($debug){
	print "$prog: return code " . $rc . "\n";
    }
    return $rc;
}

# Resolve all symlinks in given path
sub resolveSymlinks() {
  my ($path) = @_;
  return $path if (length $path == 0);
  my $respath = `readlink -f $path`;
  if ($? != 0) {
      return $path;    # failure: return original path
  }
  chomp $respath;
  return $respath;
}

# Read ReleaseData file and return dictionary with content
sub release_metadata() {
    my %data;
    my @dir = grep(/\/$ENV{'AtlasProject'}\//, split(':',$ENV{'LD_LIBRARY_PATH'}));
    open RELDATA, "$dir[0]/../ReleaseData" or print "=== Alert! Cannot open ReleaseData file\n";
    #print "Release metadata:\n";
    while (<RELDATA>) {
        chomp;
        my @kv = split(':');
        #print $kv[0].": ".$kv[1]."\n";
        $data{$kv[0]} = $kv[1];
    }
    return %data;
}
