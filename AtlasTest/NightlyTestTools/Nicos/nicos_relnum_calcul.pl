#!/usr/local/bin/perl
use Env;
use Cwd;
use File::Basename;
use File::stat;

sub nicos_mtime {
    my $file = shift(@_);
    @modtime = localtime((stat($file))[9]);
    @curtime = localtime(time());
    $mtim=$modtime[7]+1;
    $ctim=$curtime[7]+1;
    $diftim=$ctim-$mtim;
    if($diftim < 0 ) {$diftim=$diftim + 366;}
    return $diftim;
}


    if ($#ARGV != 0) {
    print "nicos_relnum_calcul: one arg req: lifetime parameter \n";
    exit 1;}

    my $NICOS_HOUR = "$NICOS_HOUR";
    my $NICOS_DAY_OFFSET = "$NICOS_DAY_OFFSET";
    my $NICOS_PROJECT_HOME = "$NICOS_PROJECT_HOME";
    my $NICOS_PROJECT_RELNAME_GEN = "$NICOS_PROJECT_RELNAME_GEN";
    my $lifetime = $ARGV[0];
    my $relnum=0;
    my $lastsym="";
    
    if ( substr($lifetime,-1) =~ /[a-zA-Z]/ ) {
    $lastsym = substr($lifetime,-1,1,"");
    }
  
    if ( $lifetime !~ /\d+$/ ){
    print "Error in lifetime format: $lifetime\n";
    exit 1;     
    }
    
    if ( $lastsym eq "c" or $lastsym eq "C" ){

    opendir(DD, ${NICOS_PROJECT_HOME});
    @allfiles = readdir DD;
    closedir DD;
  
    @list = grep /^${NICOS_PROJECT_RELNAME_GEN}.+$/, @allfiles;
    $relnumb_max=-1;
    @nmbrs = ();
    $relnumb_maxlife=-1; 
    
    $maxlife=-1;

    foreach $listfile (@list){
        ($lin1 = $listfile) =~ s/ //g;
        next if (length($lin1) eq 0);
        next if $listfile =~ /^\.\.?$/;
        next if $listfile =~ /~$/;
        @relnumb_ar=split(/_/,$listfile);
        if ($#relnumb_ar >= 0){
        $relnumb=$relnumb_ar[$#relnumb_ar];
        if ($relnumb =~ /\d+$/){
        push(@nmbrs,$relnumb);  
        $path_to_file="${NICOS_PROJECT_HOME}/${listfile}";  
        $file_mtime=nicos_mtime($path_to_file);
#        print "$relnumb $file_mtime\n";  
        $relnumb_maxlife=$relnumb if ($file_mtime > $maxlife);
        $maxlife=$file_mtime if ($file_mtime > $maxlife);
        $relnumb_max=$relnumb if ($relnumb > $relnumb_max);
        }
        }
        }

#print "lifetime $lifetime \n";
#print "relnumb_max $relnumb_max \n";
#print "relnumb_maxlife $relnumb_maxlife \n";
#print "maxlife $maxlife \n";
       
    if ( $relnumb_max < ($lifetime - 1) ){
    $relnumb=$relnumb_max + 1;
#    print "relnumba $relnumb \n";
    print "$relnumb";
    exit; 
    } 
    else{
    $i_cur=0;
    for ($i=0; $i <= $relnumb_maxlife; $i++){
        $i_cur=$i;
        $itm_cur=-1;
	foreach $itm (@nmbrs){
	$itm_cur=$itm;
        last if ( $itm == $i );
	}
    last if ($i ne $itm_cur); 
    }
    $relnumb=$i_cur;
#    print "relnumbb $relnumb \n"; 
    print "$relnumb";
    exit;
    } 

} # if ( $lastsym eq "c" or $lastsym eq "C" )
else {
@curtime = localtime(time());
$timenum=$curtime[6]; #weekday
$timenum=$curtime[7] if ($lifetime ne 7); # day of year
  if ($lifetime ne 7 && $NICOS_DAY_OFFSET !=""){
  $timenum=$timenum + $NICOS_DAY_OFFSET;  
  }
    if ( $NICOS_HOUR =~ /\d+$/ ) {
    $hr = $NICOS_HOUR;
    $hour = $curtime[2];
        if ( $hour >= $hr ){
             if ( $timenum == 6 && $lifetime == 7 ) {$timenum=0;} 
             else { $timenum++;}
        } 
    }
if ( $lifetime == 0 ){
$relnum="";
}
elsif ( $lifetime == 1 ){
$relnum=0;
}
else{
$relnum = $timenum % $lifetime;
} 
#print " normal lifetime $lifetime \n";
#print " normal relnum $relnum \n";
print "$relnum";
exit;
}
