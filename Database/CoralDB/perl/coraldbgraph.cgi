#!/usr/bin/perl -w
#
# $Id: coraldbgraph.cgi,v 1.5 2007-08-27 23:19:57 andr Exp $
#
# coraldbgraph.cgi is cgi script to generate connectivity graphs from
# a CoralDB database.
#
# Written in January 2007 by Juerg Beringer
#
use strict;
use CGI qw(:standard escapeHTML);
use CGI::Carp qw(fatalsToBrowser);
use CoralDB;
use CoralDBGraph;

my $id = param('id');
my $type = param('type') || 'gif';
my $convention = param('convention') || 'ID';
unless (defined($id)) {
	print header();
	print h2("ERROR: No parameter id found\n");
	exit 0;
}

my $db = new CoralDB($ENV{CORALDB},$ENV{CORALDBUSER},$ENV{CORALDBPWD},$ENV{CORALDBPREFIX});
my $defaultTag = 'DEFAULT';
my $idTag = param("idTag") || $defaultTag;
my $connTag = param("connTag") || $defaultTag;
my $dataTag = param("dataTag") || $defaultTag;
my $aliasTag = param("aliasTag") || $defaultTag;
$db->setTag('idTag',$idTag);
$db->setTag('connTag',$connTag);
$db->setTag('dataTag',$dataTag);
$db->setTag('aliasTag',$aliasTag);

my $graph;
if ($type eq 'ps') {
    $graph = new CoralDBGraph($db, width => 7.5, height => 10, layout => 'dot', node => {fontsize => '10'});
} else {
    $graph = new CoralDBGraph($db, layout => 'dot', node => {fontsize => '10'});
}
$graph->{'convention'} = $convention;
$graph->showNode($id, color => 'red');
$graph->showAncestors($id,1);
$graph->showDaughters($id,1);

print header( -type => "image/$type" );
print $graph->getOutput($type);
