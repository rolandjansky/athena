# $Id: CoralDBGraph.pm,v 1.4 2007-08-27 23:19:42 andr Exp $

=head1 NAME

CoralDBGraph - A package to create connectivity graphs from a CoralDB database

=head1 SYNOPSIS

  use CoralDBGraph;
  $g = new CoralDBGraph>($coraldbRef, width => 7.5, height => 10, layout => 'dot', node => {fontsize => '10'});
  $g->showNode($id, color => 'red');
  $g->showAncestors($id,1);
  $g->showDaughters($id,1);
  $g->writeFile('graph.gif');

=head1 DESCRIPTION

This module allows to generate connectivity graphs from a CoralDB database using the Perl module
GraphViz for rendering the graph.

=head1 METHODS

=cut



package CoralDBGraph;

use strict;
use GraphViz;
use CoralDB;



=head2 $g = I<new CoralDBGraph>($coraldbRef)

=over 4

This is the constructor. It takes a reference to a CoralDB database object plus optional configuration
parameters to be passed to the GraphViz constructor.

=back

=cut
sub new {
    my $pkg = shift;
    my $db = shift;
    my $gr = GraphViz->new(@_);
    bless { 'pkg' => $pkg,
	    'db' => $db,
	    'gr' => $gr,
	    'visited' => {},
	    'edges' => {},
	    'loopEdgeColor' => 'black',   # set to a different color to highlight where loops are encountered
	    'loopCounter' => 0,
	    'convention' => 'ID',
	    }, $pkg;
}



=head2 $g->I<showAncestors>($toId, $showAll)

=over 4

Add all ancestors of object $toId to the graph. If $showAll is false, only parent nodes (in the sense
of "hard links") are shown. If $showAll is set true, "soft links" are also included. An optional third
parameter $nested should never be set by the user.

=back

=cut
sub showAncestors {
    my $self = shift;
    my ($toId,$showAll,$nested) = @_;
    unless ($nested) {$self->{'visited'} = {}; }
    #print "ShowAncestors for $toId\n";
    my $resRef;
    if ($showAll) {
	$resRef = $self->{'db'}->findAllConnections($toId,0);
    } else {
	$resRef = $self->{'db'}->findConnections($toId,"UP",0);
    }
    $self->{'visited'}->{$toId} = 1;
    if (@$resRef>0) {
	my $rowRef;
	foreach $rowRef (@$resRef) {
	    my $id = $$rowRef[0];
	    if ($self->{'visited'}->{$id}) {
		#print "Found LOOP at $id\n";
		$self->{'loopCounter'}++;
		$self->showEdge($id,$$rowRef[1],$toId,$$rowRef[3],color => $self->{'loopEdgeColor'});
	    } else {
		$self->showNode($id);
		$self->showEdge($id,$$rowRef[1],$toId,$$rowRef[3]);
		$self->showAncestors($id,$showAll,1);
	    }
	}
    }
    #print "End of ShowAncestors for $toId\n";
}


=head2 $g->I<showDaughters>($id, $showAll)

=over 4

Add all daughters of object $id to the graph. If $showAll is false, only child nodes (in the sense
of "hard links") are shown. If $showAll is set true, "soft links" are also included. An optional third
parameter $nested should never be set by the user.

=back

=cut
sub showDaughters {
    my $self = shift;
    my ($id,$showAll,$nested) = @_;
    my $resRef;
    unless ($nested) { $self->{'visited'} = {}; }
    #print "ShowDaughters for $id\n";
    if ($showAll) {
	$resRef = $self->{'db'}->findAllConnections($id);
    } else {
	$resRef = $self->{'db'}->findConnectionsToSlot($id,"UP");
    }
    $self->{'visited'}->{$id} = 1;
    if (@$resRef>0) {
	my $rowRef;
	foreach $rowRef (@$resRef) {
	    my $toId = $$rowRef[2];
	    if ($self->{'visited'}->{$toId}) {
		#print "Found LOOP at $id\n";
		$self->{'loopCounter'}++;
		$self->showEdge($id,$$rowRef[1],$toId,$$rowRef[3],color => $self->{'loopEdgeColor'});
	    } else {
		$self->showNode($toId);
		$self->showEdge($id,$$rowRef[1],$toId,$$rowRef[3]);
		$self->showDaughters($toId,$showAll,1);
	    }
	}
    }
    #print "End of ShowDaughters for $id\n";
}


=head2 $g->I<showNode>($id, %attr)

=over 4

Add a node for object $id to the graph. The optional attributes specified to %attr are
passed on to GraphViz.

=back

=cut
sub showNode {
    my $self = shift;
    my ($id, %attr) = @_;
    #print "... Node: $id\n";
    my $type = $self->{'db'}->objectType($id) || 'no type';;
    my $name = $id;
    my $convention = $self->{'convention'};
    if ($convention ne "ID") {
	$name = $self->{'db'}->findAlias($id,$convention);
	unless ($name) { $name = "ID=$id"; }
    }
    $self->{'gr'}->add_node($id, label => "$name\n($type)", %attr);
}


=head2 $g->I<showEdge>($id, $slot, $toId, $toSlot, %attr)

=over 4

Add an edge for the specified connection to the graph. The optional attributes specified to %attr are
passed on to GraphViz.

=back

=cut
sub showEdge {
    my $self = shift;
    my ($id,$slot,$toId,$toSlot,%attr) = @_;
    #print "... Edge: $id $slot --> $toId $toSlot\n";
    my $edgeId = "$id-$slot-$toId-$toSlot";
    if (! $self->{'edges'}->{$edgeId}) {
	$self->{'edges'}->{$edgeId} = 1;
	my $style = 'dashed';
	my $toLabel = $toSlot;
	if ($toSlot eq 'UP') {
	    $style = 'filled';
	    $toLabel = '';
	}
	$self->{'gr'}->add_edge($id => $toId, style => $style,
				taillabel => $slot, headlabel => $toLabel, labelfontsize => 7, %attr);
    }
}


=head2 $g->I<writeFile>($fileName)

=over 4

Write graph to file with name $fileName.

=back

=cut
sub writeFile {
    my $self = shift;
    my ($outfile) = @_;
    if ($outfile =~ /\.gif$/) {
	$self->{'gr'}->as_gif($outfile);
	return;
    }
    if ($outfile =~ /\.jpg$/) {
	$self->{'gr'}->as_jpeg($outfile);
	return;
    }
    if ($outfile =~ /\.png$/) {
	$self->{'gr'}->as_png($outfile);
	return;
    }
    if ($outfile =~ /\.ps$/) {
	$self->{'gr'}->as_ps($outfile);
	return;
    }
    die "ERROR: Unknown output file type of file $outfile\n";
}


=head2 $image = $g->I<getOutput>($type)

=over 4

Return the image data of the graph as a string in the format specified by $type. Possible values are
"gif", "jpg", "png", and "ps".

=back

=cut
sub getOutput {
    my $self = shift;
    my ($type) = @_;
    if ($type eq "gif") {
	return $self->{'gr'}->as_gif();
    }
    if ($type eq "jpg") {
	return $self->{'gr'}->as_jpeg();
    }
    if ($type eq "png") {
	return $self->{'gr'}->as_png();
    }
    if ($type eq "ps") {
	return $self->{'gr'}->as_ps();
    }
    die "ERROR: Unknown output file type: $type\n";
}


1;

=head1 AUTHOR

Written by Juerg Beringer, Lawrence Berkeley National Laboratory.

=cut
