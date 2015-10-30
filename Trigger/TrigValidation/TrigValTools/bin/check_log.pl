#!/usr/bin/perl -w
# check log file for ERROR or FATAL messages
# ignore those listed as "known"
use Getopt::Long;
use File::Basename;
use constant TRUE => 1;
use constant FALSE => 0;
$prog = basename $0;
sub main();
main();
exit 0;

sub main(){
  parse_options(); # command line options
  parse_config(); # config file
  scan_logfile();
  print_results();
}

sub parse_options(){
  # default config file:
  #$configfile = '../Testing/check_log.conf';
  $configfile = 'check_log.conf';
  $showexcludestats = FALSE;
  $help = FALSE;
  $warnings = FALSE;
  $errors = TRUE;
  my $result = GetOptions ('help' => \$help,
			   'showexcludestats!' => \$showexcludestats,
			   'config=s' => \$configfile,
                           'warnings!' => \$warnings,
                           'errors!' => \$errors,
			  );
  if (!$result){
    usage();
    exit 1;
  }

  # log file should be present as remaining argument
  if (! defined $ARGV[0]){
    print "$prog: error: logfile argument not given\n";
    usage();
    exit -1;
  }
  $logfile=$ARGV[0];
  if (! -r $logfile){
    print "$prog: error: logfile $logfile does not exist\n";
    exit -2;
  }
  if (not $errors and not $warnings){
    print "$prog: error: at least one of errors and warnings must be enabled\n";
    exit -4;
  }
}


# read config file
sub parse_config(){
  @ignore = ();
  # if specified config file does not exist, try get_files. If still not there, fail.
  if (! -r $configfile) {
      system("get_files -data -symlink $configfile");
      if (! -r $configfile){ 
	  print "$prog: Error: config file $configfile not found. Specify a config file including the path with --config <file>\n";
	  exit -1;
      }
  }
  open CONFIG, "<$configfile"
    or die "$prog: error: failed opening $configfile: $!\n";
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
    # look for ignore lines:
    if ($line =~ /^ignore\s+(.+)$/){
      my $ignore_pattern = $1;
      $ignore_pattern =~ s/^\'//; # strip quote at start
      $ignore_pattern =~ s/\'$//; # strip quote at end
      push @ignore, $ignore_pattern;
    }
  }
  close CONFIG;
}

sub scan_logfile(){
  # initialise counters for ignored lines
  @ignore_counter = ();
  for ($i=0; $i<=$#ignore; $i++){
    $ignore_counter[$i] = 0;
  }

  @patterns=();
  if ($errors) {
      push @patterns, "^ERROR | ERROR | FATAL |CRITICAL |ABORT_CHAIN ";
      push @patterns, "^Exception\:|Traceback|Shortened traceback|stack trace|^Algorithm stack\:|\.IncludeError|inconsistent use of tabs and spaces in indentation|glibc detected"
      }
  if ($warnings) {
      push @patterns, "WARNING ";
  }
  
  $msgLevels = join("|", @patterns);

  # open log file
  open LOG, "<$logfile" or die "can't open $logfile:$!";
  @errors = ();
  while (<LOG>){
    # skip python code from athena -b
    next if (/^ \-\+\- /);
    # skip stupid list of all symbols defined in python
    next if (/^\[\'ALL\'\,/);
    # match pattern declared above for warnings or errors
    if (/($msgLevels)/){
      #print "LINE: $_";
      $line = $_;
      $ignore = FALSE;
      for ($i=0; $i<=$#ignore; $i++){
	#print "VETO? $ignore[$i]\n";
	if ($line =~ /$ignore[$i]/){
	  #print "VETO!\n";
	  $ignore_counter[$i]++;
	  $ignore = TRUE;
	}
      }
      if (!$ignore){
	push @errors, $line;

	# get the full traceback for the tags Traceback and Shortened traceback
	if (/(Traceback|Shortened traceback|^Algorithm stack\:)/){
	    # two space at the start of the line
	    while (($line = <LOG>) =~ /^  /) {
		push @errors, $line;
	    }
	    # another line, already read in
	    push @errors, $line;
	}
      }
    }
  }
}

sub print_results(){
  print "$prog: checking for <<$msgLevels>> in log.\n";
  print "Found messages in $logfile (" . ($#errors+1) . "):\n";
  for (@errors){
    print;
  }
  if ($#errors<0){
    print "None\n";
  }
  if ($showexcludestats){
    print "The following messages were ignored:";
    $printed_something = FALSE;
    for ($i=0; $i<=$#ignore; $i++){
      if ($ignore_counter[$i] > 0){
	printf "\n%3dx %s", $ignore_counter[$i], $ignore[$i];
	$printed_something = TRUE;
      }
    }
    print " none" if (! $printed_something);
    print "\n";
  }
  $pwd=`pwd`;
  chomp $pwd;
  # make a URL by substituting the lxbuild local disk path for the web 
  # server URL if it does not match the lxbuild local disk path then it 
  # will be left as a file path instead. 
  my $logfileURL = "$pwd/$logfile";
  $logfileURL =~ s%/build/atnight/localbuilds/nightlies%http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/nightlies%;
  if ($#errors>=0){
    print "FAILURE : error/fatal found in log file - see $logfileURL\nNB replace rel_0 with actual nightly in this URL.\n";
    exit -10;
  }
}

sub usage(){
  print "
  Usage: $prog [options] <logfile>

  Tool to check for error messages in a log file. By default ERROR, FATAL
  and CRITICAL messages are considered. The config file may be used to
  provide patterns of lines to exclude from this check - known false positives.

  Options:

  --help               show this message
  --config <file>      specify config file (default is check_log.conf)
                       will be got from DATAPATH if it does not exist
  --showexcludestats   print a summary table of the number of times
                       each of the exclude patterns was matched (default false)
  --warnings           check in addition for WARNING messages (default false)
  --errors             check errors (default true)

To negate something that is default true, prefix the option with no, 
e.g. --noerrors.

Note that at least one of errors and warnings must be true, otherwise 
there is nothing to search for.

"

}
