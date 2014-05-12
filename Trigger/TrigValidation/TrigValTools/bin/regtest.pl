#!/usr/bin/perl -w
use Getopt::Long;
use File::Basename;
use File::Spec;
use File::Copy;
use File::Path;
use constant TRUE => 1;
use constant FALSE => 0;
use constant UNDEFINED => "undefined";
$prog = basename $0;
sub main();
main();
exit -10;

sub main(){
    command_line();
    $status = regtest();
    exit($status);
}

sub prog_error_exit($$){
    $failkey = 'FAILURE' if (!defined($failkey));
    my ($message,$exitcode) = @_ or die;
    print "$prog: $failkey $message\n";
    exit($exitcode);
}

sub command_line(){
  # parse command arguments and check usage
  $debug = FALSE;
  $inputfile = '';
  $reffile = '';
  $failkey = 'FAILURE';
  $linematch = 'REGTEST';
  my $result = GetOptions ('help' => \$help,
                           'debug!' => \$debug,
                           'failkey=s' => \$failkey,
                           'inputfile=s' => \$inputfile,
                           'reffile=s' => \$reffile,
			   'linematch=s' => \$linematch);
  if ($help || !$result) {
    usage();
    prog_error_exit("usage",-1);
  }

}

# usage message
sub usage(){
    print "
  Usage: $prog [options] 

  Testing tool for comparing marked lines in a log file against a reference

  Options:

  --help                show this information
  --debug               print debug information, for the script maintainer
  --failkey <string>    keyword to be printed to indicate to the test 
                        framework that a test has failed, default $failkey
  --inputfile <string>  specify (athena) log file to take as input
  --reffile <string>    specifiy reference file, with absolute or reletive path
  --linematch <string>  only compare lines which match this string
                        default: REGTEST
                        Note: this is a perl regexp. See man perlre.
                        Example to match a particular algorithm:
                        'TrigJetRec_Cone.+REGTEST'

  Return codes: 0 = success, non-zero = failure of some sort.

  Technical info:

  Lines which match the regular expression

";
}

sub regtest(){
    
    my $newfile = basename($reffile) . ".new";
    
    # extract regression test lines from log file
    open NEW, ">$newfile"
	or die "$prog: error: failed opening $newfile to write: $!\n";
    if (! open LOG, "<$inputfile"){
	print "$prog: error: failed opening $inputfile to read: $!\n";
	print "-> $failkey Aborting this test\n";
	return 2; # go on to next test
    }
    my $lines=0;
    my $result;
    while (<LOG>){
	if (/$linematch/){
	    print NEW;
	    $lines++;
	}
    }
    print "$prog debug: regtest $lines lines matched $linematch in LOG\n" if ($debug);
    close LOG;
    close NEW;
    # check whether any lines matched - if zero it's an error straight away
    # and no point in doing the diff
    if ($lines == 0){
	# print failure keyword here to flag it as an error
	print "=== Alert! $failkey no lines matching $linematch were found in log file\n";
	$result = 1;
    }
# diff the output and the reference
# even if verify is off
    print "$prog: debug: diff -b $newfile $reffile\n" if ($debug);
    my $rc = system("diff -b $newfile $reffile");
    if ($rc == 0){
	print "=== Output is the same as reference\n";
        $result = 0;
    } else {
	# print failure keyword here to flag it as an error
	print "=== Alert! $failkey input file (<) differs from reference (>) \n";
        print "    If this change is understood, to update the reference file please type:\n";
        print "    cp ",File::Spec->rel2abs($newfile)," ",File::Spec->rel2abs($reffile),"\n";
	$result = 1;
    }
    
    print "$prog debug: returning result $result\n" if ($debug);
    return $result;
}
