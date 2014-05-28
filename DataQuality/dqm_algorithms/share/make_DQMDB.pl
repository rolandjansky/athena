#!/usr/local/bin/perl -w
use strict;
use Getopt::Long;
use Cwd;

my $int=0;
my $outputfile = "";
my $rootfile = "";
my $inputfile = "";
my $Region="";
my $createrootdump;
my $first=1;
my $c;
my $index;
my $dir;
my $createnewvalues=1;
my $algorithm;
my %opt = ();
my $optparam = ();
my $optred = ();
my $optgreen = ();
&GetOptions(\%opt, "param=f%",  "green=f%", "red=f%", 'c=i',\$createrootdump,'o=s', \$outputfile,'i=s',\$inputfile, 'f=s',\$rootfile, 's=i',\$createnewvalues, 'd=s', \$dir, 'a=s', \$algorithm, 'r=s', \$Region );

$optgreen=$opt{"green"};
$optred=$opt{"red"};
$optparam=$opt{"param"};
my $gcount += scalar keys %$optgreen;
my $rcount += scalar keys %$optred;
my $pcount += scalar keys %$optparam;

open(G,">dump_$outputfile") || die "ups";
$c=-e "inspect.C"  ;
if (!$c){
print "need file inspect.C\n";
exit 0;
}

if ($inputfile eq ""){
	$inputfile="root_dump.txt";
}

system "which root";
if ($createrootdump){
    print "creating root file dump\n";
    system "root -b $rootfile -q inspect.C |sort > $inputfile";
};
if (!($createrootdump) && !(-e $inputfile) ){
    print "creating root file\n";
    system "root -b $rootfile -q inspect.C |sort > $inputfile";
}
open(F,"<".$inputfile) || die "cannot open $inputfile";
while(<F>){
    chomp $_;
    if ($_=~/$rootfile:($dir\/.*)/ || $_=~/$rootfile:(\/$dir\/.*)/ || $_=~/$rootfile:($dir.*)/){
	print "$_\n";
	++$int;
	if ($createnewvalues) {
		$index=$int;
	}else {
		$index=1;
	}
       	  print G "\<obj class=\"DQParameter\" id=\"Param_$int\"\> \n";
	  print G "\<attr name=\"InputDataSource\" type=\"string\"\>\"$1\"\<\/attr\> \n";
	  print G "\<attr name=\"Weight\" type=\"float\"\>1\<\/attr\> \n";
	  print G "\<rel name=\"Algorithm\">\"DQAlgorithm\" \"$algorithm\"\<\/rel\> \n";
	  print G "\<attr name=\"Action\" type=\"string\"\>\"\"\<\/attr\> \n";
          if ($pcount != 0 ){
 	    print G "\<rel name=\"AlgorithmParameters\" num=\"$pcount\"\> \n";
	    while ( my ($pkey, $pvalue) = each(%$optparam) ) {
 	       print G "	  \"DQAlgorithmParameter\" \"param-$pkey-$index\" \n";
            }
	    print G "\<\/rel\> \n";
	    print G "\<rel name=\"References\" num=\"0\"\> <\/rel\>\n";
	  }else {
	    print G "\<rel name=\"AlgorithmParameters\" num=\"0\"\> \n";
            print G "\<\/rel\> \n";
            print G "\<rel name=\"References\" num=\"0\"\> <\/rel\>\n";
          }
          if ($gcount != 0 ){ 
	    print G "\<rel name=\"GreenThresholds\" num=\"$gcount\"\>\n";
	    while ( my ($pkey, $pvalue) = each(%$optgreen) ) {
	       print G "	\"DQThreshold\" \"GreenTh-$pkey-$index\" \n";
            }
	    print G "\<\/rel\>\n";
	  }else {
	   print G "\<rel name=\"GreenThresholds\" num=\"0\"\>\n";
           print G "\<\/rel\>\n";
	  }

	  if ($rcount != 0 ){ 
            print G "\<rel name=\"RedThresholds\" num=\"$rcount\"\>\n";
	    while ( my ($pkey, $pvalue) = each(%$optred) ) {
	       print G "	\"DQThreshold\" \"RedTh-$pkey-$index\" \n";
            }
		print G "\<\/rel\>\n";
          }else { 
	    print G "\<rel name=\"RedThresholds\" num=\"0\"\>\n";
            print G "\<\/rel\>\n";
          }
	print G "\<\/obj\> \n";
	print G "\n \n";
	
	if ($createnewvalues || $first) {
	   if ($gcount !=0 ){
              while ( my ($pkey, $pvalue) = each(%$optgreen) ) {
	        print G" \<obj class\=\"DQThreshold\" id\=\"GreenTh\-$pkey\-$int\"\> \n";
	        print G" \<attr name=\"Name\" type=\"string\"\>\"$pkey\"\<\/attr\> \n";
	        print G" \<attr name=\"Value\" type\=\"double\"\>$pvalue\<\/attr\> \n";
	        print G "\<\/obj\> \n";
	        print G "\n \n";
	      } 
           }
	   if ($rcount !=0 ){
              while ( my ($pkey, $pvalue) = each(%$optred) ) {
	        print G" \<obj class\=\"DQThreshold\" id\=\"RedTh\-$pkey\-$int\"\> \n";
	        print G" \<attr name=\"Name\" type=\"string\"\>\"$pkey\"\<\/attr\> \n";
	        print G" \<attr name=\"Value\" type\=\"double\"\>$pvalue\<\/attr\> \n";
	        print G "\<\/obj\> \n";
	        print G "\n \n";
              }
           }

	   if ($pcount !=0 ){
              while ( my ($pkey, $pvalue) = each(%$optparam) ) {
	       print G "\<obj class=\"DQAlgorithmParameter\" id\=\"param\-$pkey\-$int\"\> \n";
	       print G "\<attr name\=\"Name\" type\=\"string\"\>\"$pkey\"\<\/attr\> \n";
	       print G "\<attr name\=\"Value\" type\=\"double\"\ num\=\"1\">$pvalue\<\/attr\> \n";
	       print G "\<\/obj\> \n";
	       print G "\n \n";
              }
            }
	      $first=0;
	    
	}
	
	
	
    }
}		  
close(F);
       if ($Region ne "" ) {
	print G "\<obj class=\"DQRegion\" id=\"$Region\"\> \n";
	print G "\<attr name=\"InputDataSource\" type=\"string\">\"\"\<\/attr\> \n";
	print G "\<attr name=\"Weight\" type=\"float\"\>1\<\/attr\> \n";
	print G "\<attr name=\"Action\" type=\"string\"\>\"\"\<\/attr\> \n";
	print G "\<rel name=\"Algorithm\"\>\"DQAlgorithm\" \"SimpleSummary\"\<\/rel\>\n";
	print G "\<rel name=\"AlgorithmParameters\" num=\"0\"\>\<\/rel\>\n";
	print G "\<rel name=\"References\" num=\"0\"\>\<\/rel\>\n";
	print G "\<rel name=\"GreenThresholds\" num=\"0\"\>\<\/rel\>\n";
	print G "\<rel name=\"RedThresholds\" num=\"0\">\<\/rel\> \n";
	print G "\<rel name=\"DQRegions\" num=\"0\">\<\/rel\> \n";
	print G "\<rel name=\"DQParameters\" num=\"$int\"\> \n";
	for (my $i=1 ; $i < ($int+1) ; $i++) {
		print G "\"DQParameter\"  \"Param_$i\" \n";
        }
        print G "\<\/rel\>\n";
	print G "\<\/obj\>\n";
      }
print G "</oks-data>\n";

close(G);
system "cat oks-xml-header.txt dump_$outputfile > $outputfile";
if ($createrootdump == 1){ 
print "*****DQM DB file $outputfile created from root file $rootfile*****\n";
}else {
print "*****DQM DB file $outputfile created from input file $inputfile*****\n";
}
if ($Region ne "" ){
print "*****This file has a Region called $Region*****\n";
}
print "*****This file has $int Parameters*****\n";
