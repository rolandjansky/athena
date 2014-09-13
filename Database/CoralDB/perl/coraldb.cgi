#!/usr/bin/perl -w
#
# $Id: coraldb.cgi,v 1.19 2007-08-28 00:08:29 jb Exp $
#
# coraldb.cgi is a simple-minded CGI script to give an overview of
# a CoralDB connectivity database as used by the ATLAS pixel detector.
#
# Written in October 2006 by Juerg Beringer
#
use strict;
use CGI qw(:standard escapeHTML);
use CGI::Carp qw(fatalsToBrowser);
use CoralDB;

my $version = "CoralDB Overview V1.0";

my $browserurl = $ENV{CORALDBBROWSERURL} || 'coraldbbrowser.cgi';
my $tableurl = $ENV{CORALDBTABLEURL} || 'coraldbtable.cgi';
my $stdtablesurl = $ENV{CORALDBSTDTABLESURL} || 'coraldbstdtables.cgi';

my $url = url();
my $bgColor = '#F0F0F0';

my $db = new CoralDB($ENV{CORALDB},$ENV{CORALDBUSER},$ENV{CORALDBPWD},$ENV{CORALDBPREFIX});

print header();
print start_html(-title =>'CoralDB Overview', -BGCOLOR => 'white');
print h1({-ALIGN => "center"}, "CoralDB Overview");
print start_form();

printDatabase();
printTags();
printTagHistory();
printDocs();
printFooter();
print end_form();
print end_html(), "\n";

exit 0;


sub printDatabase {
    #printHeader("Database");
    print "<TABLE BORDER='0' WIDTH='80%'>\n";
    print "<TR>",
        "<TD>Database</TD>",
        "<TD>connection = $ENV{CORALDB}<BR>prefix = $db->{prefix}<BR>user = $ENV{CORALDBUSER}</TD>",
        "<TD></TD>",
        "</TR>";
    print "<TR height=10></TR>";

    print "<TR>",
        "<TD><a href=$browserurl>Browser</a></TD>",
        "<TD>Browse database contents and generate tables of selected objects</TD>",
        "</TR>";
    print "<TR>",
        "<TD><a href=$stdtablesurl>Connectivity tables</a></TD>",
        "<TD>Generate a number of standard connectivity tables</TD>",
        "</TR>";
    print "<TR>",
        "<TD><a href=$tableurl>Table Maker</a></TD>",
        "<TD>Generate tables for arbitrary collections of objects</TD>",
        "</TR>";
    print "</TABLE>\n";
}


sub printTags {
    printHeader("Tags");
    print "<TABLE BORDER='1' WIDTH='80%'>\n";
    print "<TR>";
    foreach my $h ('ID Tag', 'Connectivity Tag(s)', 'Data Tag(s)', 'Alias Tag(s)') {
	print "<TD><B>$h</B></TD>";
    }
    print "</TR>";
    my @allIdTags = $db->getAllTags('idTag');
    foreach my $idTag (@allIdTags) {
	print "<TR>";
	my $locked1 = $db->isLockedTag('idTag',$idTag) ? ' <FONT COLOR=RED>*</FONT>' : '';
	print "<TD valign='top'><a href=$browserurl?idTag=$idTag>$idTag</a>$locked1</TD>";
	foreach my $type ('connTag', 'dataTag', 'aliasTag') {
	    print "<TD valign='top'>";
	    my @tags = $db->getAllTags($type, $idTag);
	    foreach my $tag (@tags) {
		my $locked2 = $db->isLockedTag($type,$idTag,$tag) ? ' <FONT COLOR=RED>*</FONT>' : '';
		print $tag,$locked2,br();
	    }
	    print "</TD>";
	}
	print "</TR>\n";
    }
    print "</TABLE>\n";
    #print p();
    print "<FONT COLOR=RED>*</FONT> indicates that a tag is locked";
}


sub printTagHistory {
    printHeader("Tag History");
    print "<TABLE BORDER='1' WIDTH='80%'>\n";
    print "<TR>";
    foreach my $h ('Valid Since', 'ID Tag', 'Connectivity Tag', 'Data Tag', 'Alias Tag', '') {
	print "<TD><B>$h</B></TD>";
    }
    print "</TR>";
    my $ref = $db->getTagHistory();
    foreach my $rowRef (@$ref) {
	print "<TR>";
	print "<TD>".localtime($$rowRef[0])."</TD>";
	for (my $i=1; $i<5; $i++) {
	    print "<TD>$$rowRef[$i]</TD>";
	}
	print "<TD><a href=$browserurl?idTag=$$rowRef[1]&connTag=$$rowRef[2]&dataTag=$$rowRef[3]&aliasTag=$$rowRef[4]>browse</a></TD>";
	print "<TR>";
    }
    print "</TABLE>\n";
}


sub printDocs {
    printHeader("Documentation");
    print <<EOF
<ul>
<li><a href="https://twiki.cern.ch/twiki/bin/view/Atlas/CoralDB">CoralDB Wiki page</a>
<li><a href="https://twiki.cern.ch/twiki/pub/Atlas/CoralDB/CoralDB.pdf">ATLAS internal note on CoralDB (draft)</a>
</ul>
EOF
}


sub printFooter {
    print "\n";
    print p();
    print "<TABLE BORDER='0' CELLPADDING='2' CELLSPACING='0' COLS=2 BGCOLOR='navy' HEIGHT='20' WIDTH='100%'><TR>\n";
    print "<TD ALIGN='left'><FONT COLOR='#FFFBF0'><B>Questions or comments? Please contact ";
    print "<A HREF='mailto:juerg.beringer\@cern.ch'><FONT COLOR='#FFFBF0'>Juerg Beringer</A></B></TD>\n";
    print "<TD ALIGN='right'><FONT COLOR='#FFFBF0'><B>$version, ",scalar localtime(time()),"</B></TD>\n";
    print "</TR>\n";
    print "</TABLE>\n";
}


sub printStatus {
    my ($left,$right) = @_;
    print p();
    print "<TABLE BORDER='0' CELLPADDING='2' CELLSPACING='0' BGCOLOR=$bgColor HEIGHT='20' WIDTH='100%'><TR>\n";
    print "<TD ALIGN='left'><BIG>$left</TD>\n";
    # print "<TD ALIGN='right'><BIG>$right</TD>\n";
    print "</TR></TABLE>\n";
    print p();
}


sub printHeader {
    my ($left,$right) = @_;
    print p();
    print "<TABLE BORDER='0' CELLPADDING='2' CELLSPACING='0' COLS=2 BGCOLOR='navy' HEIGHT='20' WIDTH='100%'><TR>\n";
    print "<TD ALIGN='left'><FONT COLOR='#FFFBF0'><B><BIG>$left</B></TD>\n";
    print "<TD ALIGN='right'><FONT COLOR='#FFFBF0'><B><BIG>$right</B></TD>\n";
    print "</TR></TABLE>\n";
    print p();
}
