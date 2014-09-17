#! /usr/bin/perl

# RootFileMergeTool.plx
# MuonDQA Utilities
#
# Created by Austin Basye on 6/23/11.
# Copyright 2011 by UIUC ATLAS Group. All rights reserved.

use warnings;
use File::Basename;

my $NOW = `date +"%m%d%y.%H%M"`;
chomp($NOW);

print "\n";
print "***************************************************\n";
print "* RootFileMergeTool.plx                           *\n";
print "* MuDQA Utilities Packages                        *\n";
print "* Source by: Austin Basye                         *\n";
print "* Date: $NOW                               *\n";
print "* USAGE: NewFilename -d [Directory] -l [List.txt] *\n";
print "***************************************************\n";


#ROOT Check
#$ROOTSYS = `echo $ROOTSYS`;
#chomp($ROOTSYS);
#if($ROOTSYS eq '' ){
#
#  print "ERROR: ROOT not configured, ROOTSYS=$ROOTSYS, Aborting...\n";
#  return;
#
#}


print "Output located in RootMerge.out\n";


#run options
my $DIR      = "unknown";
my $LIST     = "unknown";
my $nFilesPerMerge = 10;
my $SPLIT    = "unknown";
my $MergedFN = $ARGV[0];
my $Log      = "RootMerge.out";

print "Output File: $MergedFN\n";





#############################################
# Parse Arguments
#############################################
foreach $ARG (1 .. (scalar @ARGV-1)){
	if($ARGV[$ARG] eq "-d"){#Define Config file
		
		$DIR = $ARGV[$ARG+1];
	}
	
	if($ARGV[$ARG] eq "-l"){#Define DS [filename=file_src]
		
		$LIST = $ARGV[$ARG+1];
		
	}
	
	if($ARGV[$ARG] eq "-npm"){#scp to lxplus
		
		$nFilesPerMerge = $ARGV[$ARG+1];
		
	}
	
	if($ARGV[$ARG] eq "-split"){#migrate
		
		$SPLIT = $ARGV[$ARG+1];
		
	}
}

#############################################
# Get Files to be merged
#############################################
@FILES = ();

unless($DIR eq "unknown"){
  
  if(-d $DIR){
    
    $FileNames="";
    $FileNames=`find $DIR -name "*.root*" | sort`;
    
    @FILES = split(/\s/, $FileNames);
    
    #Remove Duplicates
    @FILES = unique(@FILES);
    
    
	}else{
    
    print "Directory Not Found: $DIR\n";
    return 1;
  }
}
unless($LIST eq "unknown"){
  
  if(-e $LIST){
    
    open(LINES,$LIST);
    
    @LINES_im = <LINES>;
    
    foreach $line (@LINES_im){
    
      chomp($line);
      
      if($line =~ m/(.*)\.root(.*)/){
      
        push @FILES, $line;
      
      } 
      
    
    }
    
    close LINES;
    
    #Remove Duplicates
    @FILES = unique(@FILES);
    
    
	}else{
    
    print "List Not Found: $LIST\n";
    return 1;
  }
}


$nFiles = scalar(@FILES);

print "There were $nFiles root files found.\n\n";



#############################################
# Merge Files
#############################################

my $FileIndex     = 0;
my $TMPFile_Index = 0;
my @FilesToBeMerged = ();


foreach $F (@FILES) {
  
  $FileIndex++;
  push @FilesToBeMerged, $F;
  
  if (scalar(@FilesToBeMerged) == $nFilesPerMerge || $FileIndex == scalar(@FILES) ){
    
    #merge
    $TMPFile_Index++;
    
    $From = $FileIndex-scalar(@FilesToBeMerged);
    
    print "Merging Files $From-$FileIndex...\n";
    
    $cmd = MergeWithHaddToTMP(@FilesToBeMerged);
    
    $OldTMPFile = $TMPFile_Index - 1;
    
    system("$cmd &> TMP_$Log");
    
    system("cat TMP_$Log >> $Log");
    
    unless($OldTMPFile == 0){
      
      system("hadd -f TMP_$TMPFile_Index.root TMP_$OldTMPFile.root TMP.root >& /dev/null");
      system("rm TMP_$OldTMPFile.root");
      
    }else{
      
      system("hadd -f TMP_$TMPFile_Index.root TMP.root >& /dev/null");
    
    }
    

    @FilesToBeMerged = ();
    
  }
  
}

print "\n All files merged to: $MergedFN \n\n";

system("hadd -f $MergedFN TMP_$TMPFile_Index.root >& /dev/null");
system("rm TMP_$TMPFile_Index.root > /dev/null");
system("rm TMP.root > /dev/null");
system("cat TMP_$Log >> $Log");
system("rm TMP_$Log");

unless($SPLIT eq "unknown"){
print "Splitting $MergedFN:\n";

  if(-e $SPLIT){
    system("root -l -q -b \"$SPLIT(\\\"$MergedFN\\\")\"");
  }else{
    print "ERROR: Unable to split, $SPLIT not found!\n\n";
  }
}






#############################################
# Helper Subs
#############################################

sub MergeWithHaddToTMP {
  
  my $cmd = "hadd -f TMP.root ";
  
  foreach my $a (@_) {
    $cmd .= "$a ";
  }
  
  return $cmd;
}


#Determine Unique Set
sub unique {
  my %seen = ();
  my @r = ();
  foreach my $a (@_) {
    unless ($seen{$a}) {
      push @r, $a;
      $seen{$a} = 1;
    }
  }
  return @r;
}




