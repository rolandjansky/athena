#!/usr/bin/perl -w
#
# $Id: coraldbstdtables.cgi,v 1.2 2007-08-27 23:19:57 andr Exp $
#
# coraldbstdtables.cgi is a simple-minded CGI script for creating
# spreadsheet-like tables from a CoralDB connectivity database as
# used by the ATLAS pixel detector.
#
# Written in July 2007 by Juerg Beringer
#
use strict;
use CGI qw(:standard escapeHTML);
use CGI::Carp qw(fatalsToBrowser);
use File::Basename;
use CoralDB;

my $version = "CoralDB Standard Connectivity Tables V1.0";

my $defaultTag = '';
my $url = url();
my $browserurl = $ENV{CORALDBBROWSERURL} || 'coraldbbrowser.cgi';

my $bgColor = '#F0F0F0';

my $nTableColumns = param("ncols") || 5;

my $db = new CoralDB($ENV{CORALDB},$ENV{CORALDBUSER},$ENV{CORALDBPWD},$ENV{CORALDBPREFIX});

my @allIdTags = $db->getAllTags('idTag');
my $idTag = param("idTag") || '';
$db->setTag('idTag',$idTag);

my @allConnTags = $db->getAllTags('connTag', $idTag);
my @allDataTags = $db->getAllTags('dataTag', $idTag);
my @allAliasTags = $db->getAllTags('aliasTag', $idTag);
my $connTag = param("connTag") || $allConnTags[0];
my $dataTag = param("dataTag") || $allDataTags[0];
my $aliasTag = param("aliasTag") || $allAliasTags[0];
$db->setTag('connTag',$connTag);
$db->setTag('dataTag',$dataTag);
$db->setTag('aliasTag',$aliasTag);

my $table = param("table");

print header();
print start_html(-title =>'CoralDB Standard Connectivity Tables', -BGCOLOR => 'white');
print h1({-ALIGN => "center"}, "CoralDB Standard Connectivity Tables");
print start_form();

#debugParams();
if ($idTag) {
    printSearchForm();
    printTable();
} else {
    printIdTagForm();
}
printFooter();
print end_form();
print end_html(), "\n";

exit 0;


sub debugParams {
    print h2("DEBUG: Parameters");
    print "URL = $url\n";
    my $name;
    foreach $name ( param() ) {
        print p($name, ' = ', param($name));
    }
}


sub objectLink {
    my ($id) = @_;
    return "<A HREF=$browserurl?what=$id&where=any+name&idTag=$idTag&connTag=$connTag&dataTag=$dataTag&aliasTag=$aliasTag>$id</A>";
}


sub printIdTagForm {
    print "<TABLE BORDER='0' WIDTH='80%'>\n";
    print "<TR>",
        "<TD>Database</TD>",
        "<TD>connection = $ENV{CORALDB}, user = $ENV{CORALDBUSER}</TD>",
        "<TD></TD>",
        "</TR>";
    print "<TR>",
        "<TD>Use ID tag</TD>",
        "<TD>", popup_menu( -name => "idTag",
			    -values => @allIdTags>0 ? \@allIdTags : [$defaultTag],
			    -default => $idTag,
			    -override => 1), "</TD>",
        "</TR>";
    print "</TABLE>\n";
    print "<TABLE WIDTH=95%><TR><TD ALIGN=RIGHT>",
           submit(-name => "find", -value => "    FIND    "),
           "</TD></TR></TABLE>\n";
}


sub printSearchForm {
    print "<TABLE BORDER='0' WIDTH='80%'>\n";
    print "<TR>",
        "<TD>Database</TD>",
        "<TD>connection = $ENV{CORALDB}, user = $ENV{CORALDBUSER}</TD>",
        "</TR>";
    print "<TR>",
        "<TD>ID tag</TD>",
        "<TD>$idTag   (<a href=$url>restart with a different ID tag)</TD>",
        "</TR>";
    print hidden("idTag",$idTag);

    print "<TR height=10></TR>";

    print "<TR>",
        "<TD>Use connectivity tag</TD>",
        "<TD>", popup_menu( -name => "connTag",
			    -values => @allConnTags>0 ? \@allConnTags : [$defaultTag],
			    -default => $connTag,
			    -override => 1), "</TD>",
        "</TR>";
    print "<TR>",
        "<TD>Use data tag</TD>",
        "<TD>", popup_menu( -name => "dataTag",
			    -values => @allDataTags>0 ? \@allDataTags : [$defaultTag],
			    -default => $dataTag,
			    -override => 1), "</TD>",
        "</TR>";
    print "<TR>",
        "<TD>Use aliases tag</TD>",
        "<TD>", popup_menu( -name => "aliasTag",
			    -values => @allAliasTags>0 ? \@allAliasTags : [$defaultTag],
			    -default => $aliasTag,
			    -override => 1), "</TD>",
        "</TR>";
    print "<TR height=10></TR>";

    my @tables;
    foreach my $t (glob("rules/*.table")) {
	my ($name, $dirname, $extname) = fileparse($t,".table");
	push(@tables, $name);
    }
    print "<TR>",
          "<TD>Use table </TD>",
          "<TD>", popup_menu( -name => "table",
			      -values => [@tables],
			      -default => 'PP0-DEFAULT',
			      -override => 0 ) ,"</TD>",
          "</TR>";

    print "</TABLE>\n";

    print "<TABLE WIDTH=70%><TR><TD ALIGN=RIGHT>",
           submit(-name => "update", -value => "GENERATE TABLE"),
           "</TD></TR></TABLE>\n";
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
    print "<TD ALIGN='right'><BIG>$right</TD>\n";
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


sub printTable {
    if ($table) {

	use CoralDBTable;
	my $t = new CoralDBTable($db);
	$t->setObjectLinkCallBack(\&objectLink);   # Amazingly, objectLink as a call-back function still sees our global variables ...

	$table =~ /-[^-]*$/;   # object type in table name is everything before the last dash (-)
	my $type = $`;
	my @idList = $db->findObjects($type,"TYPE");

	print br();
	printStatus("<B>Table $table</B>", "Found ".scalar(@idList)." objects of type $type");

	$t->loadRules("rules/$type.rules");
	$t->loadTable("rules/$table.table");
	$t->print(@idList);
    }
}
