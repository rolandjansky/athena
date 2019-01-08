#!/usr/bin/perl
#
# W. H. Bell
#
# A script to split FLUKA beam halo files from Roderick Bruce into
# smaller files.
#
use strict;

die ("Usage: $0 <input file> <output prefix> <number of events>\n")
  unless ($#ARGV==2);

my $input_file = $ARGV[0];
my $output_prefix = $ARGV[1];
my $num_events = $ARGV[2];
my $file_counter = 1; 
my $event_counter = 0;

die ("Cannot open $input_file for reading\n")
    unless open(INPUTFILE, "< $input_file");

my $output_file = sprintf("$output_prefix%05d.events",$file_counter);
die ("Cannot open $output_file for writing\n")
   unless open(OUTPUTFILE, '>', $output_file);

my $event_column = 1; # The position of the event number in the input file columns

my $last_event = -10;
while (my $line = <INPUTFILE>) {

    # Remove the new line character
    chomp $line;
    
    # If the string is not refined, skip this line.
    if(defined $line) {
	
	# Split the string, using one or more spaces to denote the
	# splitting.
	my @frags = split(/\s+/,$line);
	
	# Check if this run number is the same as the last run number
	if($frags[$event_column] != $last_event) {
	    
	    # Increament the number of runs read.
	    $event_counter++;
	    
	    # If enough runs have been written close this output file and
	    # open a new one.
	    if($event_counter >= $num_events) {
		close(OUTPUTFILE);
		
		$file_counter++;
		$event_counter = 0;
		
		$output_file = sprintf("$output_prefix%05d.events",$file_counter);
		die ("Cannot open $output_file for writing\n")
		    unless open(OUTPUTFILE, '>', $output_file);
	    }
	    $last_event = $frags[$event_column];
	}
	
	# Write this line to the output file
	print OUTPUTFILE "$line\n";
    }  
}

close(INPUTFILE);
close(OUTPUTFILE);
