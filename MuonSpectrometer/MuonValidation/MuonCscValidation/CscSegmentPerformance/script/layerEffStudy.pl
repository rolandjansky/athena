#!/usr/local/bin/perl

# usage   this rootFileName....

use File::Basename;

$rootFileName = $ARGV[0];
($fileBaseName, $Name, $fileExtension)
    = fileparse($rootFileName, ('\.root') );

$jobName = $fileBaseName;

print "$jobName\n";

$dirName = $jobName.'/figures';
system "mkdir -p $dirName";

$command = 'root -b -q macroEffFake.C\(\"'.$jobName.'\"\)';
print "$command\n";
system($command);

chmod(0775, $jobName);
chmod(0775, $dirName);

$gifFileName = $dirName."/sector$i\_measphi0.gif";
for ($i=0; $i<32; ++$i) {
    chmod(0744, $gifFileName);
}

# file to make...
$htmlName = $jobName."/".$jobName.".html";
print "html created and name is $htmlName \n";
open(PRINTOUT, ">$htmlName") or die "Can't open file: $!\n";

#header...
print PRINTOUT "<html><head> \n";
print PRINTOUT "<title>Graph</title> \n";
print PRINTOUT "</head><body><h1 align=\"center\"><u>Graph</u></h1>\n";
print PRINTOUT "<PRE>\n";
    
    
    for ($i=0; $i<32; ++$i){
        $sector = getIdxToSector($i);
        print PRINTOUT "<a href=\"figures/sector$i\_measphi0.gif\">sector $sector</a>\n";
    }

print PRINTOUT "</PRE>\n";
print PRINTOUT "</body></html>";

$command = "tar zcf ".$jobName.".gz ".$jobName;
print "$command\n";
system("$command");

system "scp -oPort=555 $jobName.gz wpark\@boson.physics.sc.edu:/Volumes/BosonHD2/Users/wpark/Sites/CSC/";



sub getIdxToSector {
    return ($_[0]<16) ? $_[0]+1 : -1*($_[0] -15);
    
}
