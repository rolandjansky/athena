#!/usr/local/bin/perl
my $pick="";
my $lengthmin=30000;
my $i=0;

for (@ARGV) 
{
if( length($_) < $lengthmin )
{$lengthmin=length($_); $pick=$_;}
$i=$i+1;
}
print "$pick";
exit;
