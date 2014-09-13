#!/usr/bin/perl -w
#
# $Id: coraldbtable.cgi,v 1.3 2007-08-27 23:19:57 andr Exp $
#
# coraldbtable.cgi is a simple-minded CGI script for creating
# spreadsheet-like tables from a CoralDB connectivity database as
# used by the ATLAS pixel detector.
#
# Written in May 2007 by Juerg Beringer
#
use strict;
use CGI qw(:standard escapeHTML);
use CGI::Carp qw(fatalsToBrowser);
use CoralDB;

my $version = "CoralDB Tables V1.0";

my $defaultTag = '';
my $url = url();
my $browserurl = $ENV{CORALDBBROWSERURL} || 'coraldbbrowser.cgi';

my $bgColor = '#F0F0F0';

my $nTableColumns = param("ncols") || 5;
my @defaultColTitle = ('Name of primary object', 'ID', 'TYPE');
my @defaultColRule = ('NAME', 'ID', 'TYPE');

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

my @allAliasConventions = $db->getAllAliasConventions();

print header();
print start_html(-title =>'CoralDB Table Maker', -BGCOLOR => 'white');
print h1({-ALIGN => "center"}, "CoralDB Table Maker");
print start_form();

#debugParams();
if ($idTag) {
    printSearchForm();
    printTableView();
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
        "<TD></TD>",
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
    print "<TR>",
        "<TD>List of objects to be included into the table (you can use IDs or aliases, but the specified name must be unique)</TD>",
        "<TD>", textarea( -name => "idlist",
			  -rows => 10,
			  -columns => 70 ), "</TD>",
        "</TR>";
    print "</TABLE>\n";
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


sub printTableView {
    use CoralDBTable;
    my $t = new CoralDBTable($db);
    $t->setObjectLinkCallBack(\&objectLink);   # Amazingly, objectLink as a call-back function still sees our global variables ...
    for (my $i=0; $i<$nTableColumns; $i++) {
	my $title = param("coltitle$i");
	unless (defined($title)) { $title = $defaultColTitle[$i] || ''; }
	$t->setColumnTitle($i,$title);
	my $rule = param("urule$i");
	unless (defined($rule)) { $rule = $defaultColRule[$i] || ''; }
	if (uc($rule) eq 'NAME') {
	    $rule = 'ID';
	} else {
	    unless ($rule =~ /^UNALIAS/i ) { $rule = "UNALIAS $rule"; }
	}
	$t->setColumnRule($i,$rule);
    }
    print br();
    my @idList = split(/[\s,]+/, param('idlist') || '');
    printStatus("<B>Found ".scalar(@idList)." primary object(s) to be included into table</B>");
    printTableHints();
    printRuleForm($t);
    $t->print(@idList);
}


sub printTableHints {
    print <<END
<UL>
<LI>Please use the form below to define the contents of your table.
    Each object specified above will result in one row in the table.
<LI>The contents of each cell in the table is derived by applying the specified "rule" to traverse the connectivity graph,
    starting at the primary object for that particular row (<A HREF="javascript:;" onClick="window.open('rules/syntax.html','syntax','scrollbars=yes,width=600,height=700');">see syntax and examples</A>).
</UL>
END
}


sub printRuleForm {
    my ($table) = @_;
    print "<TABLE BORDER=0 WIDTH=100%>\n";
    print "<TR height=10></TR>";
    print "<TR>",
        "<TD>Number of columns </TD>",
        "<TD>",textfield( -name => "ncols",
			  -size => 3,
			  -maxlength => 3,
			  -default => $nTableColumns),"</TD>",
        "</TR>";
    print "<TR height=10></TR>";
    for (my $i=0; $i<$nTableColumns; $i++) {
	print "<TR>";
	print "<TD>Define column $i:</TD>";
	print "<TD>Title = ", textfield( -name => "coltitle$i",
				       -size => 30,
				       -maxlength => 255,
				       -default => $defaultColTitle[$i] || '');
        print ", Rule = ", textfield( -name => "urule$i",
				      -size => 40,
				      -maxlength => 255,
				      -default => $defaultColRule[$i] || '');
        print "</TD>";
	print "</TR>\n";
    }
    print "</TABLE>\n";
    print "<TABLE WIDTH=95%><TR><TD ALIGN=RIGHT>",
           submit(-name => "update", -value => "UPDATE TABLE"),
           "</TD></TR></TABLE>\n";
}
