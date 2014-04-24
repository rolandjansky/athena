#!/usr/bin/perl
# ==================================================================================================================================#
#                                                                                                                                   #
#    reindex -r <rel> -f <file> -d <DOXYTAGPATH> -a <AFSDOXYTAGPATH>                                                                #
#    Unix script to reindex all doxygen search.idx indexes in order to create a singular searchable index file.                     #
#    The script uses search.idx files already created by doxygen in the AFSDOXYTAGPATH.                                             #
#                                                                                                                                   #
#    Options:                                                                                                                       #
#     -r|--release <rel> Nightly or build release.                                                                                  #
#     -f|--file Name of the new index file.                                                                                         #
#     -d|--doxpath DOXYTAGPATH     Http location where the doxygen is located.                                                      #
#          By default for nightly it is                                                                                             #
#                  http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDevDirectory/AtlasOffline/<rel>/InstallArea/doc/ #
#          for build releases it is                                                                                                 #
#                 http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/AtlasOffline/<rel>/InstallArea/doc/       #
#     -a|--afspath AFSDOXYTAGPATH  AFS location where the doxygen is located.                                                       #
#          By default for nightly it is                                                                                             #
#                /afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/<rel>/InstallArea/doc/                               #
#          for build releases it is                                                                                                 #
#               /afs/cern.ch/atlas/software/builds/AtlasOffline/<rel>/InstallArea/doc/                                              #
#     -t|--tgtpath TARGETPATH  AFS location where the globsearch.idx and globalDoxySearch.php will be copied to                     #
#     -v|-verbose  Print verbose messages while running.                                                                            #
#                                                                                                                                   #
#####################################################################################################################################
use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;
#use Devel::Size qw(size total_size);

sub show_help {
    my $script = shift;
    my $message = "Usage: $script -r <rel> [-f <file> -d <DOXYTAGPATH> -a <AFSDOXYTAGPATH>]\n".
                  "  Options:\n".
                  "    -r|--release rel Nightly or build release.\n".
                  "    -f|--file    Name of the new index file.\n".
                  "    -d|--doxpath DOXYTAGPATH     Http location where the doxygen is located.\n".
                  "    -a|--afspath AFSDOXYTAGPATH  AFS location where the doxygen is located.\n".
                  "    -t|-tgtpath  TARGETPATH      AFS location where the globsearch.idx and globalDoxySearch.php files will be copied to.\n".
                  "    -h|-?|-help  This message\n".
                  "    -v|-verbose  Print verbose messages while running.";
    return $message;
}

sub readInt
{
    my $file = shift;
    my $c1 = getc($file); my $c2 = getc($file);
    my $c3 = getc($file); my $c4 = getc($file);
    if (defined($c1) && defined($c2) && defined($c3) && defined($c4)){
        my $b1 = ord($c1); my $b2 = ord($c2);
        my $b3 = ord($c3); my $b4 = ord($c4);
        return ($b1<<24)|($b2<<16)|($b3<<8)|$b4;
    }
    else{
        return 0;
    }
}

sub readString
{
    my $file = shift;
    my $result="";
    while (ord(my $c=getc($file)) != 0){ $result.=$c};
    return $result;
}

sub writeInt
{
    my $file = shift;
    my $index = shift;
    print $file chr(($index)>>24);
    print $file chr((($index)>>16)&0xff);
    print $file chr((($index)>>8)&0xff);
    print $file chr(($index)&0xff);
}

sub writeString
{
    my $file = shift;
    my $s = shift;
    
    print $file "$s\0";
}

sub computeIndex
{
  # Fast string hashing
  my $word = shift;
  my $retVal = -1;
  # Simple hashing that allows for substring search
  if (length($word)<2) {return (-1)};
  # high char of the index
  my $hi = ord(substr($word,0,1));
  if ($hi==0) {return (-1)};
  # low char of the index
  my $lo = ord(substr($word,1,1));
  if ($lo==0) {return (-1)};
  # return index
  return ($hi*256+$lo);
}

sub addUrlIndex
{
    my $idx = shift;
    my $urls = shift;
    my $hiPriority = shift;
    my $freq = shift;
    
}

###################### MAIN #####################

my $script=$0;
my $rel;
my $outfilename   = "globsearch.idx";
my $DOXYTAGPATH;    # Path to the HTML dir containing Doxygen docs
my $AFSDOXYTAGPATH; # Path to the AFS  dir containing Doxygen docs
my $verbose = 0;
my $man = 0;
my $help = 0;
my $TARGETPATH;
my $result = GetOptions ("r|release=s"=> \$rel,           # string
                         "f|file=s"   => \$outfilename,   # string
                         "d|doxpath=s"=> \$DOXYTAGPATH,   # string
                         "a|afspath=s"=> \$AFSDOXYTAGPATH,# string
                         "t|tgtpath=s"=> \$TARGETPATH,    # string
                         "v|verbose"  => \$verbose,       # flag
                         "h|?|help"   => \$help);         # flag, help

pod2usage(show_help($0)) if $help;
pod2usage("$0: No rel defined\n" . show_help($0)) if ( !defined($rel) );

print "$script begin: " , `date`;
if ($rel =~ /^rel_/) 
{ #location of the nightly
  print "rel=$rel. Setting nightly locations\n";
  $DOXYTAGPATH="http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDevDirectory/AtlasOffline/$rel/InstallArea/doc/" unless (defined($DOXYTAGPATH));
  $AFSDOXYTAGPATH="/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/$rel/InstallArea/doc/" unless (defined($AFSDOXYTAGPATH));
}
else
{ #location of the release
  print "rel=$rel. Setting release locations\n";
  $DOXYTAGPATH="http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/AtlasOffline/$rel/InstallArea/doc/" unless (defined($DOXYTAGPATH));
  $AFSDOXYTAGPATH="/afs/cern.ch/atlas/software/builds/AtlasOffline/$rel/InstallArea/doc/" unless (defined($AFSDOXYTAGPATH));
}

if ( ! -d "$AFSDOXYTAGPATH" ){
        print "Directory $AFSDOXYTAGPATH does not exist. Exitting...\n";
        exit 1;
}

my @infilenameArr=`find $AFSDOXYTAGPATH -maxdepth 4 -iname "search.idx"`;
chomp(@infilenameArr);

my $count;
my $numIndexEntries=256*256;
my $buffer = "";
my $word = "";
my %cols = ( r => "\033[1;31m",
	     g => "\033[1;32m",
	     b => "\033[1;34m",
	     y => "\033[1;33m",
	     p => "\033[1;35m",
	     nc => "\033[0;m",
	   );
my @globWordList;
# globWordlist is an array of hashes
# index: hash index of the word
# globWordList[i] --> word1 --> url1word1 --> name
#                                         --> freqhi
#                           --> url2word1 --> name
#                                         --> freqhi
#                 --> word2 --> url1word2 --> name
#                                         --> freqhi
#                           --> url2word2 --> name
#                                         --> freqhi
# globWordList[i] = { word => { urls => [0] = $name,
#                                       [1] = $freqhi, 
# 				        },
# 		              },
# 		    };

my $doxyHeader="DOXS";
my $fail;
$count = 0;
for my $infilename (@infilenameArr)
{# Begin loop over infiles
    my $fileTime = `date`;
    chomp $fileTime;
    my $meminfo = `cat /proc/meminfo`;
    my %mem;
    ($mem{MemFree})  = $meminfo=~/MemFree:\s*(\d+)/;
    ($mem{MemTotal}) = $meminfo=~/MemTotal:\s*(\d+)/;
    ($mem{SwapFree}) = $meminfo=~/SwapFree:\s*(\d+)/;
    ($mem{SwapTotal}) = $meminfo=~/SwapTotal:\s*(\d+)/;
    my $memLine = "Mem: $mem{MemFree}/$mem{MemTotal}, Swap: $mem{SwapFree}/$mem{SwapTotal}";
#    my $memInfo = `cat /proc/meminfo | perl -ne '/MemFree:\ *([0-9]+\ kB)/ && print "$1\n"; /SwapFree:\ *([0-9]+\ kB)/ && print "$1\n"; ' | paste - -`;
    print "$fileTime, $memLine: Opening input file: $infilename\n";
    next if (-z $infilename); #If the file is empty, skip it

    open(my $infile, "<", $infilename) || die("Can't open infile: $!");
    my $fileHeader = readString($infile);
    next unless ($fileHeader eq $doxyHeader); #If the file is not a doxy index, skip it.
    binmode($infile);

#    (my $path) = $infilename=~/\.\/(.*)\//; #Returns basename of infilename
    (my $path) = $infilename =~ /${AFSDOXYTAGPATH}[\/]?(.*)\//;	#Returns the package path relative to the AFSDOXYTAGPATH root.
    my @fileWordList; #wordList beloning to this file only
    ###################### READ FILE #####################
    if ($verbose) {print "################ WORDINDEX ##################\n";}
    for (my $index = 0; $index<$numIndexEntries; $index++)
    {# Begin loop over num of index entries
        $fail = seek($infile, $index*4+4, 0);
        my $idx = readInt($infile);

        my %fileWords; #Wrapper for @words, adding the $index (hash) these words belong to.
        if ($idx)
        { # Begin if ($idx)
            if ($verbose) {printf("%spos=%d (0x%0x) idx[%d]=%d (0x%0x)%s\n", $cols{r},($index*4+4),($index*4+4),$index,$idx,$idx,$cols{nc});}
            $fail = seek($infile,$idx,0);
            my $w = readString($infile);
            
            my @words; #Array of words belonging to the same hash ($index)
            while ($w)
            {#Begin loop over indexed words
                my $statidx = readInt($infile);
                my %word = ( "word"    => $w,
                             "statidx" => $statidx,
                             "docs"    => [],
                           );
                push (@words, \%word);
		
                if ($verbose) {printf ("word=%s statidx=%d (0x%0x)\n", $w, $statidx, $statidx);}
                $w = readString($infile);
            }#End loop over indexed words
            %fileWords = ( "index" => $index,
                           "words" => [ @words ],
                         );
        } # End if ($idx)
        push (@fileWordList, \%fileWords);
    } # End loop over num of index entries
    if ($verbose) {print "################ WORDLIST ##################\n";}
    my $hashid=-1;
    foreach my $fileWords (@fileWordList)
    {#Begin loop over all word hashes in the fileWordList
        $hashid++;
        my $index = $fileWords->{"index"};
        my $words = $fileWords->{"words"};
        next unless (defined ($words));
        foreach my $word (@$words)
        {#Begin loop over all words beloning to this hash
            my $statidx = $word->{"statidx"};
            $fail = seek($infile,$statidx,0);
            my $filepos = tell($infile);
            if ($verbose) {printf("%sReading statidx: %d (0x%0x)   ", $cols{g},$filepos, $filepos);}
            
            my $numDocs = readInt($infile);
            if ($verbose) {printf("numDocs: %d (0x%0x)%s\n", $numDocs, $numDocs, $cols{nc});}
            exit unless ($numDocs < 50000);
            my @docInfo;
            for (my $iDoc=0;$iDoc<$numDocs;$iDoc++)
            {
                my $docidx=readInt($infile); 
                my $freqhi=readInt($infile);
                my $freq = $freqhi>>1;
                my $hi = $freqhi&1;
                $docInfo[$iDoc]={"docidx"  => $docidx,
                                 "freq"    => $freq,
                                 "hi"      => $hi,
				};
            }#End loop over all words beloning to this hash
            for (my $iDoc=0;$iDoc<$numDocs;$iDoc++)
            {#Begin loop over an indexed word's documents
                $fail         = seek($infile,$docInfo[$iDoc]{"docidx"}, 0);
                my $name    = readString($infile);
                my $url     = readString($infile);
                my $wordKey = $word->{word};
                my $urlKey  = "$path/$url";
# 		print "$wordKey $docInfo[$iDoc]{docidx} $docInfo[$iDoc]{freq} $urlKey \n";
# 		if (defined ($globWordList[$index]->{$wordKey}->{$urlKey}->[1])) {
# 		  print "index=$index, wordKey=$wordKey urlKey=$urlKey EXISTS! org freq=",$globWordList[$index]->{$wordKey}->{$urlKey}->[1]>>1," ";
# 		  print "add freq ", $docInfo[$iDoc]{freq}, "\n";
# 		}
		$globWordList[$index]->{$wordKey}->{$urlKey}->[0]  = $name;
		$globWordList[$index]->{$wordKey}->{$urlKey}->[1] += $docInfo[$iDoc]{freq}<<1;
		$globWordList[$index]->{$wordKey}->{$urlKey}->[1] |= $docInfo[$iDoc]{hi};
		if ($verbose){
		  print "name = $name | ";
		  print "URL = $urlKey | ";
		  print "freq = ", $globWordList[$index]->{$wordKey}->{$urlKey}->[1]>>1 ," | ";
		  print "\n";
		}
            }#End loop over an indexed word's documents
        }#End loop over all words beloning to this hash
    }#End loop over all word hashes in the fileWordList
    close $infile;
} #End loop over infiles

# my  $struct_size = size(\@globWordList);
# my  $total_size = total_size(\@globWordList);
# print "Structure Size=$struct_size\nTotal Size=$total_size\n"; 
# Create sorted searchIndex:
###################### WRITE TO FILE #####################
my $indexListBegin = 4;	# Beginning of the index list in the output file
my $wordListBegin = $indexListBegin + 4*$numIndexEntries; # Beginning of the word list in the output file
my $docListBegin;       # Beginning of the doc list in the output file 
my $urlListBegin;       # Beginning of the url list in the output file

my @wordListOffsets;
my $wordListOffset = 0;

# first +second pass: compute the size of the wordlist and the wordlist index
if ($verbose) {print "################ CALCULATING OFFSETS ##################\n";}
for (my $index = 0; $index<$numIndexEntries; $index++){# Begin loop over num of index entries
    my $globWords = $globWordList[$index];
    if (defined ($globWords)){
      $wordListOffsets[$index] = $wordListBegin + $wordListOffset;
      if ($verbose) { print "wordListOffsets[$index]=$cols{r}$wordListOffsets[$index]$cols{nc}\n   ";}
      foreach my $word (keys %$globWords) { #Begin loop over all words belonging to this hash
	my $ws = length($word)+1+4; #word + offset to url info array + zero list terminator
	$wordListOffset += $ws;
	if ($verbose) { print "$word ";}
      } # End loop over all words belonging to this hash
      if ($verbose) { print "\n";}
      $wordListOffset += 1; #zero list terminator
    } else {
      $wordListOffsets[$index]=0;
    }
} # End loop over num of index entries

my $size    = $wordListBegin + $wordListOffset;
my $padding = $size;
$size = ($size+3)&~3; # round up to 4 byte boundary
$padding = $size - $padding;

$docListBegin = $size;
my @docOffsets;
my $docOffset = 0;
$count = 0;
# third pass: compute offset to stats info for each word
for (my $index = 0; $index<$numIndexEntries; $index++)
{# Begin loop over num of index entries
    my $globWords = $globWordList[$index];
    next unless (defined ($globWords));
    foreach my $word (keys %$globWords)
    {#Begin loop over all words beloning to this hash
      my $nDocs = scalar keys %{$globWords->{$word}};
      $docOffsets[$count++] = $docListBegin + $docOffset;
      if ($verbose) { print ("globWordList[$index][?]: word = $word  nDocs = $nDocs docOffsets[",$count-1,"] = $cols{g}$docOffsets[$count-1]$cols{nc}\n");}
      $docOffset += 4+$nDocs*8; # count + (url_index,freq) per url
    }#End loop over all words beloning to this hash
} # End loop over num of index entries

$urlListBegin = $docListBegin + $docOffset;
my @urlOffsets;
my $urlOffset = 0;
$count=0;
# fourth pass: compute offset for urls:
for (my $index = 0; $index<$numIndexEntries; $index++)
{# Begin loop over num of index entries
    my $globWords = $globWordList[$index];
    next unless (defined ($globWords));
    foreach my $word (keys %$globWords)
    {#Begin loop over all words beloning to this hash
      my $nDocs = scalar keys %{$globWords->{$word}};
      my $urls = $globWords->{$word};
      foreach my $url (keys %$urls) {
	my $name = $urls->{$url}->[0];
	$urlOffsets[$count++]   = $urlListBegin + $urlOffset;
	if ($verbose) { print "word=$word calc urlOffsets[",$count-1,"]=$urlOffsets[$count-1] url=$url\n";}
	$urlOffset += length($url)+1+
	              length($name)+1;
      }
    }#End loop over all words beloning to this hash
} # End loop over num of index entries

#Begin writing:
#Write header:
if ($verbose) {print "################ WRITING FILE ##################\n";}
open(my $outfile, "+>", $outfilename) || die("Can't open outfile: $!");
binmode($outfile);
print $outfile $doxyHeader;
#Write index
if ($verbose) { print "$cols{y}******* Writing index ******* $cols{nc}\n";}
for (my $index = 0; $index<$numIndexEntries; $index++)
{# Begin loop over num of index entries
  if ($verbose) { 
    my $pos = tell ($outfile);
    print "$cols{p}$pos$cols{nc}, write wordListOffsets[$index]=$wordListOffsets[$index]\n" if ($wordListOffsets[$index]>0);
  }
  writeInt($outfile, $wordListOffsets[$index]);
} # End loop over num of index entries

#Write word lists
if ($verbose) { print "$cols{y}******* Writing wordList ******* $cols{nc}\n";}
$count  = 0;
for (my $index = 0; $index<$numIndexEntries; $index++){# Begin loop over num of index entries
    my $globWords = $globWordList[$index];
    next unless (defined ($globWords));
    if ($verbose) { 
      my $pos = tell($outfile);
      print "$cols{p}$pos$cols{nc} write words:\n";
    }
    foreach my $word (keys %$globWords)
    {#Begin loop over all words beloning to this hash
      if ($verbose) {print "    $word, docOffsets[$count]=$docOffsets[$count]\n";}
      writeString($outfile, $word);
      writeInt($outfile,$docOffsets[$count++]);
    }#End loop over all words beloning to this hash
    print $outfile "\0";
} # End loop over num of index entries

#Write extra padding bytes:
for (my $index=0;$index<$padding;$index++) { print $outfile "\0";}
#Write word statistics:
if ($verbose) { print "$cols{y}******* Writing docList ******* $cols{nc}\n";}
$count = 0;
for (my $index = 0; $index<$numIndexEntries; $index++)
{# Begin loop over num of index entries
    my $globWords = $globWordList[$index];
    next unless (defined ($globWords));
    foreach my $word (keys %$globWords)
    {# Begin loop over all words beloning to this hash
      my $nDocs = scalar keys %{$globWords->{$word}};
      if ($verbose) {
	my $pos = tell ($outfile);
	print "$cols{p}$pos$cols{nc} write nDocs=$nDocs for word $cols{y}$word$cols{nc}\n";
      }
      writeInt($outfile,$nDocs);
      my $urls = $globWords->{$word};
      foreach my $url (keys %$urls) {# Begin loop over urls
	my $name      = $urls->{$url}->[0];
	my $freq      = $urls->{$url}->[1]>>1;
	my $hi        = $urls->{$url}->[1]&1;
#        my $freqhi    = ((($freq)<<1) | $hi);
        my $freqhi    = $urls->{$url}->[1];
	if ($verbose) {print ("   url=$cols{b}$url$cols{nc}, urlOffset=$urlOffsets[$count], freq=$cols{r}$freq$cols{nc}, hi=$cols{g}$hi$cols{nc}, freqhi=$freqhi\n");}
	writeInt($outfile, $urlOffsets[$count++]);
	writeInt($outfile, $freqhi);
      }# End loop over urls
    }# End loop over all words beloning to this hash
} # End loop over num of index entries

#Write URLs:
if ($verbose) { print "$cols{y}******* Writing URLs ******* $cols{nc}\n";}
for (my $index = 0; $index<$numIndexEntries; $index++)
{# Begin loop over num of index entries
    my $globWords = $globWordList[$index];
    next unless (defined ($globWords));
    foreach my $word (keys %$globWords)
    {#Begin loop over all words beloning to this hash
      my $urls = $globWords->{$word};
      foreach my $url (keys %$urls) {
	my $name = $urls->{$url}->[0];
	if ($verbose) {
	  my $pos = tell($outfile);
	  print ("$cols{p}$pos$cols{nc} writing url=$url, name=$name\n");
	}
	writeString($outfile, $name);
	writeString($outfile, $url);
      }
    }#End loop over all words beloning to this hash
} # End loop over num of index entries

close $outfile;
print ("File \"$outfilename\" closed.\n");
if (defined $TARGETPATH) {
  chomp(my $cur_path = `pwd`);
  print ("Moving \"$outfilename\" from $cur_path to $TARGETPATH... ");
  $fail=`mv $outfilename $TARGETPATH`;
  unless ($fail) {print "Success.\n";}
  else           {print "Failure.\n";}
  (my $path_to_script) = $0 =~ /(.*)\//;
  print ("Copying \"globalDoxySearch.php\" from $path_to_script to $TARGETPATH... ");
  $fail=`cp $path_to_script/globalDoxySearch.php $TARGETPATH`;
  unless ($fail) {print "Success.\n";}
  else           {print "Failure.\n";}
}
print "$script end: " , `date`;

