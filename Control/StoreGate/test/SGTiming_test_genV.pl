#!/usr/bin/perl

my @chars = ("A" .. "Z");

$NTYPES = 20;
$NKEYS = 5;
$NITER = 100;

$DO_OBJGEN = 1;

if ($ENV{"LCG_PLATFORM"} =~ /opt/) {
    print "opt build - not doing assert\n";
    $DO_ASSERT = 0;
} else {
    print "dbg build - doing assert\n";
    $DO_ASSERT = 1;
}

$SEED = 12345;
srand( $SEED );

$n = 0;

while ($n < $NTYPES) {

    $s = randString(6);

    $obn = "SGTObj_$s";

    if (!defined $objs{$obn}) {
        $objs{$obn} = $s;
        $n ++;
    } else {
        print "$s is repeat\n";
        $nn ++;
    }
}

print "Generated $n classes\n";


if ($DO_OBJGEN) {
    open(OUT, ">SGTiming_test_objs.h");
    
    print OUT "#ifndef STOREGATE_SGTEST_OBJS_H\n";
    print OUT "#define STOREGATE_SGTEST_OBJS_H 1\n";
    
    foreach $c (sort keys %objs) {
        $X = createClass($c);
        print OUT "$X\n";
    }
    
    print OUT "#include \"AthenaKernel/CLASS_DEF.h\"\n";
    
    print "generating $n CLIDs: ";
    $i = 1;
    foreach $c (sort keys %objs) {
        print "$i ";
        $CLID = `clid -m $c`;
        print OUT $CLID;
        $i++;
    }
    print "\n";
    
    print OUT "#endif\n";
    
    close(OUT);
}

open (OUT, ">SGTiming_test_def.inc");
print OUT "static const unsigned int NTYPES { $NTYPES };\n";
print OUT "static const unsigned int NKEYS { $NKEYS };\n";
print OUT "static const unsigned int NITER { $NITER };\n";
close (OUT);    

open (OUT, ">SGTiming_test_ptr.inc");
print OUT "   vector<string> Keys;\n";
print OUT "   for (size_t i=0; i<$NKEYS; ++i)\n";
print OUT "       Keys.push_back( \"key_\" + to_string(i) );\n";
foreach $c (sort keys %objs) {
    $s = $objs{$c};

    print OUT "   std::vector<" . $c . "*> VEC_" . $s . ";\n";
    print OUT "   std::vector<const " . $c . "*> CVEC_" . $s . ";\n";
    print OUT "   CVEC_$s.resize($NKEYS);\n";
    print OUT "   for (size_t i=0; i<$NKEYS; ++i)\n";
    print OUT "       VEC_$s.push_back( new $c(i) );\n";

}
close (OUT);


open (OUT, ">SGTiming_test_rec.inc");
print OUT "   for (size_t i=0; i<$NKEYS; ++i) {\n";
foreach $c (sort keys %objs) {
    $s = $objs{$c};

    if ($DO_ASSERT) {
        print OUT "      assert(pSG->record(VEC_" . $s. "[i], Keys[i]).isSuccess());\n";
    } else {
        print OUT "      pSG->record(VEC_" . $s. "[i], Keys[i]).ignore();\n";
    }
}
print OUT "   }\n";
close (OUT);


open (OUT, ">SGTiming_test_ret.inc");
print OUT "   for (size_t i=0; i<$NKEYS; ++i) {\n";
foreach $c (sort keys %objs) {
    $s = $objs{$c};

    if ($DO_ASSERT) {
        print OUT "      assert(pSG->retrieve(CVEC_". $s ."[i], Keys[i]).isSuccess());\n";
    } else {
        print OUT "      pSG->retrieve(CVEC_". $s ."[i], Keys[i]).ignore();\n";
    }
}
print OUT "   }\n";
close (OUT);

open (OUT, ">SGTiming_test_chk.inc");
print OUT "   for (int i=0; i<$NKEYS; ++i) {\n";
foreach $c (sort keys %objs) {
    $s = $objs{$c};
    if ($DO_ASSERT) {
        print OUT "      assert (CVEC_" . $s . "[i]->val() == i);\n";
    } else {
        print OUT "     if (CVEC_" . $s . "[i]->val() != i) { cerr <<  \"ERROR with val check obj $c key \" << Keys[i] << endl; }\n";
    }
}
print OUT "   }\n";
close (OUT);

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

sub randString {
    my ($l,@chars) = @_ if @_;
    my ($s);

    $l = 8 unless $l;
    @chars = ("A" .. "Z") unless @chars;

    $s .= $chars [ rand @chars ] for 1 .. $l;

    return $s;
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

sub createClass {
    my ($n) = @_ if @_;
    my ($X);

    if (!defined $n) {
        print "ERROR: need a class name\n";
        exit(1);
    }

    $X  = "class $n {\n";
    $X .= "public:\n";
    $X .= "   $n(){};\n";
    $X .= "   $n(int i):m_i(i){};\n";
    $X .= "   virtual ~$n(){};\n";
    $X .= "   void val(const int& i) { m_i = i; }\n";
    $X .= "   int  val() const { return m_i; }\n";
    $X .= "private:\n";
    $X .= "   int m_i{0};\n";
    $X .= "};\n";

    return $X;
}

