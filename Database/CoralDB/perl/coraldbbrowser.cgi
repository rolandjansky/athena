#!/usr/bin/perl -w
#
# $Id: coraldbbrowser.cgi,v 1.2 2007-08-27 23:19:57 andr Exp $
#
# coraldb.cgi is a simple-minded CGI script for browsing a CoralDB
# connectivity database as used by the ATLAS pixel detector.
#
# Written in October 2006 by Juerg Beringer
#
use strict;
use CGI qw(:standard escapeHTML);
use CGI::Carp qw(fatalsToBrowser);
use CoralDB;

my $version = "CoralDB Browser V2.1";

my $graphurl = $ENV{CORALDBGRAPHURL} || 'coraldbgraph.cgi';

my $defaultTag = '';
my $url = url();
my $searchString = param("what");
my $searchWhere = param("where");
my $searchOnlyType = param("type");
my $bgColor = '#F0F0F0';

my $convention = param("convention") || 'ID';
my $dograph = param("dograph") || 0;
my $dotable = param("dotable") || 0;
my $predeftable = param("predeftable");

my $nTableColumns = param("ncols") || 3;
my @defaultColTitle = ('ID');
my @defaultColRule = ('ID');

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

my @allObjectTypes = $db->getAllObjectTypes();
my @allAliasConventions = $db->getAllAliasConventions();

print header();
print start_html(-title =>'CoralDB Browser', -BGCOLOR => 'white');
print h1({-ALIGN => "center"}, "CoralDB Browser");
print start_form();

#debugParams();
if ($idTag) {
    printSearchForm();
    if ($dotable) {
	printTableView();
    } else {
	printSearchResult();
	printMasterList();
    }
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


sub desiredName {
    my ($id) = @_;
    my $name = $id;
    if ($convention ne "ID") {
	$name = $db->findAlias($id,$convention);
	unless ($name) { $name = "ID=$id"; }
    }
    return $name;
}


sub objectLink {
    my ($id) = @_;
    return "<A HREF=$url?what=$id&where=any+name&convention=$convention&idTag=$idTag&connTag=$connTag&dataTag=$dataTag&aliasTag=$aliasTag&dograph=$dograph>".desiredName($id)."</A>";
}


sub getAncestor {
    my ($id) = @_;
    my $resRef = $db->findConnections($id,'UP',0);
    my $colRef = $$resRef[0];
    return $$colRef[0];
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
        "<TD>Search only objects of type</TD>",
        "<TD>", popup_menu( -name => "type",
			    -values => ['ANY TYPE', @allObjectTypes],
			    -default => '',
			    -override => 0), 
        "</TD>",
        "</TR>";
    print "<TR>",
        "<TD>Search using</TD>",
        "<TD>", popup_menu( -name => "where",
			    -values => ["ID", "ALIAS", "ANY NAME", "TYPE"],
			    -default => "ANY NAME"), "</TD>",
        "</TR>";
    print "<TR>",
        "<TD>Search for</TD>",
        "<TD>",textfield( -name => "what",
			  -size => 30,
			  -maxlength => 256),
        " <BR>(case-sensitive, use % as wildcard character)","</TD>",
        "</TR>";

    print "<TR height=10></TR>";

    print "<TR>",
        "<TD>Use table view (instead of browser view)?</TD>",
        "<TD>",checkbox("dotable",$dotable,"1",""),"</TD>",
        "</TR>";
    print "<TR>",
        "<TD>Show inline connectivity graph?</TD>",
        "<TD>",checkbox("dograph",$dograph,"1",""),"</TD>",
        "</TR>";
    print "<TR>",
        "<TD>Display results using naming convention</TD>",
        "<TD>", popup_menu( -name => "convention",
			    -values => ["ID", @allAliasConventions],
			    -default => "ID"), "</TD>",
        "</TR>";
    print "</TABLE>\n";
    unless ($dotable) {
	print "<TABLE WIDTH=95%><TR><TD ALIGN=RIGHT>",
	      submit(-name => "find", -value => "    FIND    "),
	      "</TD></TR></TABLE>\n";
    }
}


sub printSearchResult {
    print br();
    printConvention();
    my @idList = $db->findObjects($searchString,$searchWhere,$searchOnlyType);
    if (@idList > 1) {
	printStatus("<B>Found ".scalar(@idList)." objects with the following IDs:</B>");
	printObjectList(@idList);
    } else {
	if (@idList == 1) {
	    my $id = $idList[0];
	    my $objType = $db->objectType($id);
	    printStatus("<B>".desiredName($id)."</B> is of type <B>$objType</B>");
	    printAliases($id);
	    printHeader("Connections for ".desiredName($id));
	    print "<TABLE BORDER=0 COLS=2 WIDTH=100%'>\n";
	    print "<TR><TD><B>Ancestors</B></TD><TD><B>Outgoing connections<B></TD></TR>";
	    print "<TR>";
	    print "<TD VALIGN=top BGCOLOR=$bgColor>";
	    printAncestors($id);
	    print "</TD>";
	    print "<TD BGCOLOR=$bgColor>";
	    printOutConnections($id);
	    print "</TD>";
	    print "</TR>";
	    print "</TABLE>";
	    print p();
	    print "<B>Other incoming connections (soft links)</B>";
	    printInConnections($id);
	    printGraph($id);
	    printData("$id");
	} else {
	    printStatus("No objects found");
	}
    }
}


sub printAliases {
    my ($id) = @_;
    my $resRef = $db->findAliases($id);
    unless (defined($resRef)) { return; }
    if (@$resRef>0) {
	print "<TABLE BORDER=0 COLS=2>";
	print "<TR>",
	    "<TD><B>ID:</TD>",
	    "<TD><B>$id</TD>",
	    "</TR>";
	foreach my $rowRef (@$resRef) {
	    my $convention = $$rowRef[1];
	    my $alias = $$rowRef[0];
	    my $aliasString = $alias;
	    if ($convention eq 'PDBSERIAL') {
		$aliasString = "<a href=http://atlas1.ge.infn.it/pixelprod/ItemOverview.php?Item=20210020$alias>$alias</a>";
	    }
	    print "<TR>",
	    "<TD>$convention: </TD>",
	    "<TD>$aliasString</TD>",
	    "</TR>";
	}
	print "</TABLE>\n";
    } else {
	print desiredName($id)." has no aliases";
    }
    print p();
}


sub printAncestors {
    my ($id) = @_;
    my @ancestors = ($id);
    my $a = getAncestor($id);
    while ($a) {
	push (@ancestors,$a);
	$a = getAncestor($a);
    }
    if (@ancestors>1) {
	while (@ancestors) {
	    print objectLink(pop(@ancestors));
	    if (@ancestors) {
		print " --> ";
	    }
	}
    } else {
	print desiredName($id)." has no ancestors",br();
    }
    print p();
}


sub printOutConnections {
    my ($id) = @_;
    my $resRef = $db->findAllConnections($id,1);
    if (@$resRef>0) {
	print "<TABLE BORDER=0 COLS=3>";
	my $rowRef;
	foreach $rowRef (@$resRef) {
	    print "<TR>",
	    "<TD>slot $$rowRef[1]</TD>",
	    "<TD WIDTH='40'>--></TD>",
	    "<TD>",objectLink($$rowRef[2])," slot $$rowRef[3]</TD>",
	    "</TR>";
	}
	print "</TABLE>";
    } else {
	print "No outgoing connections";
    }
}


sub printInConnections {
    my ($id) = @_;
    my $resRef = $db->findSoftLinks($id,0);
    if (@$resRef>0) {
	print "<TABLE COLS=4 BGCOLOR=$bgColor>";
	my $rowRef;
	foreach $rowRef (@$resRef) {
	    print "<TR>",
	    "<TD>",objectLink($$rowRef[0]),"</TD",
	    "<TD>slot $$rowRef[1]</TD>",
	    "<TD WIDTH='40'>--></TD>",
	    "<TD>slot $$rowRef[3]</TD>",
	    "</TR>";
	}
	print "</TABLE>";
    } else {
	print "<TABLE COLS=4 BGCOLOR=$bgColor>";
	print "<TR><TD>No other incoming connections</TD></TR>";
	print "</TABLE>";
    }
}


sub printGraph {
    my ($id) = @_;
    printHeader("Connectivity graph for ".desiredName($id));
    print("<A HREF=$graphurl?id=$id&type=ps&convention=$convention&idTag=$idTag&connTag=$connTag&dataTag=$dataTag&aliasTag=$aliasTag>Download PostScript file</A");
    if ($dograph) {
	print "<CENTER><IMG SRC=$graphurl?id=$id&type=gif&type=ps&convention=$convention&idTag=$idTag&connTag=$connTag&dataTag=$dataTag&aliasTag=$aliasTag></CENTER>\n";
    }
}


sub printData {
    my ($id) = @_;
    printHeader("Payload Data for ".desiredName($id));
    my $resRef = $db->findClobs($id);
    if (@$resRef>0) {
	print "<TABLE BORDER=1 COLS=2>";
	my $rowRef;
	foreach $rowRef (@$resRef) {
	    print "<TR>",
	    "<TD>$$rowRef[0]</TD>",
	    "<TD>$$rowRef[1]</TD>",
	    "</TR>";
	}
	print "</TABLE>";
    } else {
	print "No data fields found";
    }
}


sub printMasterList {
    printHeader("Objects in master list");
    my @masterList = $db->masterList();
    if (@masterList) {
	printObjectList(@masterList);
    } else {
	print "Master table contains no objects for this tag.";
    }
}



sub printObjectList {
    my (@idList) = @_;
    #print "<TABLE BORDER=2>\n";
    my $id;
    foreach $id (@idList) {
	# print "<TR><TD>",$obj,p(),"</TD></TR>\n";
	print objectLink($id)," ";
    }
    #print "</TABLE>\n";
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


sub printConvention {
    if ($convention ne 'ID') {
	if ($dotable) {
	    print "<CENTER><FONT COLOR='RED'>PLEASE NOTE: In table view, only identifiers appearing in links are shown using convention $convention (or are prefixed by 'ID=' if no such alias is available).</FONT></CENTER>\n";
	} else {
	    print "<CENTER><FONT COLOR='RED'>PLEASE NOTE: Unless prefixed by 'ID=', all identifiers below are shown using convention $convention.</FONT></CENTER>\n";
	}
    }
}


sub printTableView {
    use CoralDBTable;
    my $t = new CoralDBTable($db);
    $t->setObjectLinkCallBack(\&objectLink);   # Amazingly, objectLink as a call-back function still sees our global variables ...
    my @preDefinedTables = ('');
    if ($searchOnlyType ne 'ANY TYPE') {
	my $ruleDefFile = "rules/$searchOnlyType.rules";
	$t->loadRules($ruleDefFile);
	use File::Basename;
	foreach my $t (glob("rules/$searchOnlyType-*.table")) {
	    my ($name, $dirname, $extname) = fileparse($t,".table");
	    push(@preDefinedTables, $name);
	}
    }
    if ($predeftable) {
	$t->loadTable("rules/$predeftable.table");
	$nTableColumns = $t->getNColumns();
	param("ncols",$nTableColumns);
	for (my $i=0; $i<$nTableColumns; $i++) {
	    param("coltitle$i",$t->getColumnTitle($i));
	    my $rule = $t->getColumnRule($i);
	    if ($t->isRuleName($rule)) {
		param("rule$i",$rule);
	    } else {
		param("urule$i",$rule);
	    }
	}
    } else {
	for (my $i=0; $i<$nTableColumns; $i++) {
	    my $title = param("coltitle$i") || $defaultColTitle[$i] || '';
	    $t->setColumnTitle($i,$title);
	    my $rule = param("rule$i") ||  param("urule$i") || $defaultColRule[$i] || '';
	    $t->setColumnRule($i,$rule);
	}
    }

    print br();
    printConvention();
    my @idList = $db->findObjects($searchString,$searchWhere,$searchOnlyType);
    printStatus("<B>Found ".scalar(@idList)." primary object(s) to be included into table</B>");
    printTableHints();
    printRuleForm($t,@preDefinedTables);
    $t->print(@idList);
}


sub printTableHints {
    print <<END
<UL>
<LI>Please use the form below to define the contents of your table.
    Each object selected by the query above will result in one row in the table.
<LI>The contents of each cell in the table is derived by applying a "rule" to traverse the connectivity graph.
<LI>For each column, choose either a predefined rule from the pulldown menu or define your own rule
    (<A HREF="javascript:;" onClick="window.open('rules/syntax.html','syntax','scrollbars=yes,width=600,height=700');">see syntax and examples</A>).
<LI>To use predefined rules for a specific type of objects, narrow your search to a single type of object
    by using "Search only objects of type" above.
</UL>
END
}


sub printRuleForm {
    my ($table,@preDefinedTables) = @_;
    print "<TABLE BORDER=0 WIDTH=100%>\n";

    print "<TR height=10></TR>";
    print "<TR>",
        "<TD>Start with a predefined table </TD>",
        "<TD>", popup_menu( -name => "predeftable",
			    -values => [@preDefinedTables],
			    -default => '',
			    -override => 1 ) ," (choosing a non-blank value here will override the fields below)</TD>",
        "</TR>";
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
	print "<TD>Title ", textfield( -name => "coltitle$i",
				       -size => 20,
				       -maxlength => 255,
				       -default => $defaultColTitle[$i] || '');
        print ", Rule ", popup_menu( -name => "rule$i",
				   -values => ['', $table->getRuleNames()],
				   -default => $defaultColRule[$i]);
	print ", or leave blank and define your own ", textfield( -name => "urule$i",
				    -size => 20,
				    -maxlength => 255),' ';
        print "</TD>";
	print "</TR>\n";
    }
    print "<TR height=10></TR>";
    print "</TABLE>\n";
    print "<TABLE WIDTH=95%><TR><TD ALIGN=RIGHT>",
           submit(-name => "update", -value => "UPDATE TABLE"),
           "</TD></TR></TABLE>\n";
}
