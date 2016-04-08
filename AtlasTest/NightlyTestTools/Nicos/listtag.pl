#!/usr/local/bin/perl 

#-> usage
sub usage {
    print "\n";
    print <<"End";
listtag [-t] [-d cvsroot] module
  -t: trace
  -d cvsroot: specify CVSROOT
  module: name of module or top directory under \$CVSROOT. 

  example: listtag bbsim                (local query)
           rsh greenjay listtag bbsim   (remote query)
End
    exit;
}

#-> cat a file
sub cat {
	open (tmp,"<$_[2]") || &die (__FILE__,__LINE__, "failed");
	while (<tmp>) {print "$_";}
	close (tmp);
	1;
}	

#-> extract: extract a portion of the file	
#   first argument: file name
#   2nd: starting symbol
#   3rd: ending symbol  
#   4th: output file 
sub extract {
	local ($match);
	open (tmp,"<$_[0]") || &die (__FILE__,__LINE__, "failed");
	open (tmp2,">$_[3]") || &die (__FILE__,__LINE__, "failed");
	while (<tmp>)
	{
	    /^$_[1].*$/ && do {
		$match=1; 
#		chop($_);
		if ("$_" ne "symbols;\n") {print (tmp2 "$_")}
		next};
	    /^$_[2].*$/ && do {$match=0; last};	    
	    if ($match) {print (tmp2 "$_")};
	}
	close (tmp);
	close (tmp2);
	1;
}

#-> check list of variables, make sure they exist
sub checklist {	
	local($var, $file, $line);
	$file=$_[0];
	$line=$_[1];
	shift;
	shift;
	foreach $var (@_)
	{    
	    if (eval "\$$var eq \"\"") 
	    {
	    	print "\$$var not defined, stopped at $file line $line.\n"; 
		&exit(__FILE__,__LINE__,1);
	    }
	}
}

#-> die: cleanup before exit
sub die {
    print "$_[2]\n";
    unlink ("$repotmp");
    exit 1;
}

#-> expand alias
#   1st argument: an alias defined in modules file
sub expand_alias {
    local ($cvsdir);
    local ($alias);
    $cvsdir=$_[0];
    if ($trace) {print "-> \$alias=\$alias_$cvsdir\n";}
    eval "\$alias=\$alias_$cvsdir";
    eval "\$define=\$define_$cvsdir";
    if ("$alias" ne "") {
	if ($trace || $found) {print "-> alias of $cvsdir is $alias ...\n";}
	if ("$alias" ne "$cvsdir") {$cvsdir=&expand_alias("$alias");}
    } elsif ("$define" ne "") {
	if ($trace || $found) {print "-> $cvsdir is directory $define\n";}
	if ("$define" ne "$cvsdir") {$cvsdir=&expand_alias("$define");}
    }
    return $cvsdir;
}

#-> cvsdir: find directory of a module in repository
#           in case user provides a module name, not directory name
sub cvsdirs {
	local ($found, @argv);
	$found=0;
	$mod=$_[2];
	open (tmp, "${cvsroot}/CVSROOT/modules");
	while (<tmp>)
	{
	    chop($_);	
	    split(' ',$_); 
#	    if ($_[0] eq $mod) 
	    if (substr($_[0],0,1) ne "#") {
		$Mod=$_[0];
		shift;
		$hasalias=0;
loop:		while (@_)
		{
		    $_=$_[0];	
		    /^-[l]$/ && do 
			{	       
			    shift;
			    next loop;
			};
		    /^-[diotu]$/ && do 
			{	       
			    shift;
			    shift;
			    next loop;
			};
		    /^-[a]$/ && do 
			{	       
			    shift;
		    	    $Cvsdir=$_[0];
			    if ($trace || "$Mod" eq "$mod") {print "-> alias of $Mod is $Cvsdir...\n";}
			    eval "\"\$alias_$Mod\" = \"$Cvsdir\"";
			    $hasalias=1;
			    if ($Mod eq $mod) {
				$found=1;
				$Cvsdir=&expand_alias($Cvsdir);
				$cvsdir=$Cvsdir;
		    	        &die (__FILE__,__LINE__, "1cannot find $cvsroot/$Cvsdir according to 'modules' file") if (!-d "$cvsroot/$Cvsdir");
				last loop;
			    };
			    shift;
			    next loop;
			};
		    /.*/ && do
			{	
		    	    $Cvsdir=$_[0];
#                            print " 999900 \"$Cvsdir\"\n";
			    if ($trace || "$Mod" eq "$mod") {print "-> $Mod is defined as $Cvsdir\n";}
#                            print " 999999 \"$Cvsdir\"\n";
			    eval "\"\$define_$Mod\" = \"$Cvsdir\"";
 			    if ($Mod eq $mod) {
				$found=1;
#                                print " fff111 $Cvsdir\n";
				$Cvsdir=&expand_alias($Cvsdir);
#                                print " fff222 $Cvsdir\n";
				if ($Cvsdir eq "") {$Cvsdir=$_[0];}
				&die (__FILE__,__LINE__, "2cannot find \$CVSROOT/$Cvsdir according to \$CVSROOT/modules") if (!-d "$cvsroot/$Cvsdir");
				$cvsdir=$Cvsdir;
				last loop;
			    }
			    shift;
			    last loop;
			};
		};
		if ($found) {close (tmp);}
	    }
	}	
	close (tmp);
	if (!$found) {$cvsdir=$mod};
	&die (__FILE__,__LINE__, "cannot find '$mod' in $cvsroot") if (!-d "$cvsroot/$cvsdir");
}

#-> dodir: look for ,v files in all subdirs
sub dodir {
    local($dir,$nlink) = @_;
    local($dev,$ino,$mode,$subcount);
# At the top level we need to find nlink ourselves
    ($dev, $ino, $mode, $nlink) = stat('$dir') unless $nlink;
# Get the list of files in the current directory
    opendir(tmp,$dir);
#  || &die (__FILE__,__LINE__, " Can't open $dir");
    local(@allfiles) = readdir(tmp);
#    print " dodir @allfiles \n";
    closedir(tmp);
     
 my $iPCK = -1;
 my $icnt = 0;
 for   (@allfiles) 
 {
     if( $_ eq "PACKAGE,v" ) { $iPCK = $icnt;}
     $icnt = $icnt +1;
 }
    if ( $iPCK != -1 )
 { 
 my $zeroel = $allfiles[0];
 $allfiles[0] =  $allfiles[$iPCK];
 $allfiles[$iPCK] = $zeroel;
 }

    if ($nlink == 2) {    # no subdirs
lab1: for   (@allfiles) {  
        next if $_ eq '.';        
        next if $_ eq '..';        
                    if (substr($_,-2) eq ",v" ) {
		    $firstfile="$dir/$_";
		    $found=1;
		    last lab1;
		    }
    }}
    else {   # there is subdirs
	$subcount = $nlink -2;
lab2: for   (@allfiles) {  
        next if $_ eq '.';        
        next if $_ eq '..';        
        next if $_ eq 'Attic';        
        next if $_ eq 'Template';        
        next if $_ eq 'Instantiate';        
        next if $_ eq 'Doc';        
        next if $_ eq 'Docs';        
        next if $_ eq 'Test';        
        $name = "$dir/$_";
#        print " name $name \n";
        next if $subcount == 0; # seen all subdirs
        ($dev, $ino, $mode, $nlink) = lstat($_);

	if (! -d name && substr($_,-2) eq ",v")	{
#        print " fileq $name $_ \n";	
	$firstfile="$name";
	$found=1;
	}

        last if ($found);
        next if -d name; 

# recursion if directory
        
#        print " in dir  $_ \n";
        &dodir($name,$nlink);
        --$subcount;
    }}
}


#-> firstfile: find first ,v file in CVS module
sub firstfile {
	local ($dir, $found, $file);
	$mod=$_[2];
#	if ($hasfirstfile) {return 1;}
#        print " cvsdirs \n";
	&cvsdirs(__FILE__,__LINE__,$mod);
#        print " checklist\n ";
	&checklist (__FILE__, __LINE__, cvsroot, cvsdir);
	$dirmain="${cvsroot}/${cvsdir}";
        &dodir("$dirmain");

	if ($found) {
	    return 1;
	    $hasfirstfile=1;
	}
	else {
	    &die (__FILE__,__LINE__, "cannot find the first file in $dir");
	    return 0;
	}
}

#-> gettagrepo: get tag section in repository
sub gettagrepo {
	$mod=$_[0];
#        print " ddddd $mod \n";
# search tag in repo
	&firstfile(__FILE__,__LINE__,$mod); 
	$time=time;
#        print " ddddd \n";
    	if ($repotmp ne "" && -s $repotmp) { unlink ($repotmp)|| &die (__FILE__,__LINE__, "failed in gettagrepo") };
	&extract($firstfile,"symbols","locks",$repotmp);	
}

#-> listtag: list tags in CVS repository
#   1st argument: file name
#   2nd argument: line number
#   3rd argument: name of a CVS top-directory or module
sub listtag {
	local ($str, $file);
	$module=$_[2];
#        print "mmmmm  $module\n";
	&gettagrepo($module);
#        print "rrrrr \n";
	$file=$repotmp;
#        print "rrrrr  $file\n";
	if (1) 
	{
	    if (-s $file) {
		print "tags in $firstfile:\n";
		&cat(__FILE__,__LINE__, $file);
	    } else {
		print "no tag found in $firstfile\n";
	    }
	}
	else 
	{ 
	    &grep (__FILE__,__LINE__, ".*v([0-9]*)\-([0-9]*)", $file);
#	    else {&catr(__FILE__,__LINE__, $file, ":"," :")};
	}

#	if ($repotmp ne "" && -s $repotmp) {&catr(__FILE__,__LINE__,$repotmp,":"," :")};
}


#-> main
    $cvsroot=$ENV{'CVSROOT'};
    $ppid=getppid(); 
    $repotmp="/tmp/listtag.$ppid";

#-> parse arguments
    if ($#ARGV < 0) {&usage;}
    while ($#ARGV>=0) {
	$_ = $ARGV[0];
	/^-[d]+$/ && do {
	    shift;
	    $cvsroot = $ARGV[0];
	    if ("$cvsroot" eq "") {&usage()};
	    goto end;
	};
	/^-[t]+$/ && do {
	    while ($_) {
		$_=substr($_,1);
		/^t.*$/ && do {$trace=1; next};
	    };
	    goto end;
	};
	/^-.*$/ && do {&usage;};
	last;
end:
	shift;
    }

    @list = @ARGV;
    if ($#ARGV<0) {&usage()}
    if ("$cvsroot" eq "") {print "define $CVSROOT or use -d option\n"}
    foreach $item (@list) {
#        print " ------ $item\n";  
	&listtag(__FILE__,__LINE__, "$item");
    }
    unlink ("$repotmp");
    exit;
