#!/usr/bin/perl
#
# A.Sbrizzi
#
# A script to format FLUKA HL-LHC beam gas files from Anton Lechner format into Roderick Bruce format
#
use strict;

die ("Usage: $0 <input file> <output prefix> <number of events>\n")
    unless ($#ARGV==2);

my $input_file     = $ARGV[0];
my $output_prefix  = $ARGV[1];
my $num_events     = $ARGV[2];
my $file_counter   = 1; 
my $event_counter  = 0;

my $event_number   = 0;
my $particle_gen   = 1;
my $particle_type  = 0;
my $stat_weight    = 0;
my $x_position     = 0;
my $y_position     = 0;
my $x_cosine       = 0;
my $y_cosine       = 0;
my $t_start        = -1;
my $total_energy   = 0;
my $kinetic_energy = 0;
my $t_primary      = 0;
my $x_primary      = 0;
my $y_primary      = 0;
my $z_primary      = 0;

die ("Cannot open $input_file for reading\n")
    unless open(INPUTFILE, "< $input_file");

my $output_file = sprintf("$output_prefix%05d.events", $file_counter);
die ("Cannot open $output_file for writing\n")
    unless open(OUTPUTFILE, '>', $output_file);

my $last_event = -10;

while (my $line = <INPUTFILE>) {
    
    # Remove the new line character
    chomp $line;
    
    # If the string is not refined, skip this line.
    if (defined $line) {
	
	# Split the string, using one or more spaces to denote the splitting.
	my @frags = split(/\s+/,$line);

	$event_number   = $frags[1];
	$particle_type  = $frags[2];
	$stat_weight    = $frags[4];
	$x_position     = $frags[5];
	$y_position     = $frags[6];
	$x_cosine       = $frags[7];
	$y_cosine       = $frags[8];
	$total_energy   = $frags[9];
	$kinetic_energy = $frags[10];
	$t_primary      = $frags[11];
	$x_primary      = $frags[12];
	$y_primary      = $frags[13];
	$z_primary      = $frags[14];

	if ($event_number != $last_event) {

	    $event_counter = $event_counter + 1;

	    if ($event_counter > $num_events) {
		
		close(OUTPUTFILE);
		
		$file_counter++;
		$event_counter = 1;

		$output_file = sprintf("$output_prefix%05d.events", $file_counter);

		die ("Cannot open $output_file for writing\n")
		    unless open(OUTPUTFILE, '>', $output_file);
	    }
	}

	$last_event = $event_number;
    }

    printf OUTPUTFILE "%10d %3d %6d %27.17e %27.17e %27.17e %27.17e %27.17e %27.17e %3d %27.17e %27.17e %27.17e %27.17e %27.17e \n", 
    $event_counter, $particle_gen, $particle_type, $kinetic_energy, $stat_weight, $x_position, $y_position, $x_cosine, $y_cosine, $t_start, $total_energy, $x_primary, $y_primary, $z_primary, $t_primary;
}

close(INPUTFILE);
close(OUTPUTFILE);
