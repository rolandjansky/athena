# $Id: CoralDBRuleProcessor.pm,v 1.6 2007-08-27 23:19:42 andr Exp $

=head1 NAME

CoralDBRuleProcessor - A package for parsing connectivity graphs

=head1 SYNOPSIS

  use CoralDBRuleProcessor;
  $p = new CoralDBRuleProcessor($coraldbRef);
  $pp0GeographicalId = $p->process($id,@rule);

=head1 DESCRIPTION

CoralDBRuleProcessor is a package for parsing connectivity graphs using
"rules" as described in ../rules/syntax.html.

=head1 METHODS

=cut



package CoralDBRuleProcessor;

use strict;
use CoralDB;



=head2 $p = I<new> CoralDBRuleProcessor($coraldbRef)

=over 4

This is the constructor. It takes a reference to a CoralDB database object.

=back

=cut
sub new {
    my $pkg = shift;
    my ($db) = @_;
    bless { 'pkg' => $pkg,
	    'db' => $db,
	    'separator' => ' '
	    }, $pkg;
}



=head2 @result = $p->I<process>($id, @rule)

=over 4

This is the only method intended to be called by the user. It applies rule $rule starting at object
$id and returns the result from the rule as an an array of string. The array will contain several items if
the rule branched into several paths while traversing the connectivity graph.

=back

=cut
sub process {
    my $self = shift;
    my ($id,@rule) = @_;
    return $self->doProcess($id,'','','',@rule);
}


sub doProcess {
    my $self = shift;
    my ($id,$slot,$fromId,$fromSlot,@rule) = @_;
    unless (@rule) { return (); }

    my $cmd = shift(@rule);

    # Commands to be implemented
    # P= for payload

    # Command to execute subsequent rule if alias is given instead of ID
    # NOTE: there are different ways how this can be implemented. Right now we
    #       require that there is only a single object who has the given name as
    #       either its ID or one of its aliases. Otherwise an error occurs.
    if (uc($cmd) eq 'UNALIAS') {
	my @objects = $self->{'db'}->findObjects($id,'ANY NAME','ANY TYPE');
	if (scalar(@objects) > 1) {
	    return ("ERROR: Ambiguous alias/id $id matches ".join(",",@objects));
	} else {
	    if ($objects[0]) {
		return $self->doProcess($objects[0],$slot,$fromId,$fromSlot,@rule);
	    } else {
		return ();
	    }
	}
    }

    # Commands to write out information
    if (uc($cmd) eq 'ID') { return $self->makeResult($id, $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'SLOT') { return $self->makeResult($slot, $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'FROMID') { return $self->makeResult($fromId, $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'FROMSLOT') { return $self->makeResult($fromSlot, $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'TYPE') { return $self->makeResult($self->{'db'}->objectType($id), $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'HTMLBOLD') { return $self->makeResult("<B>", $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'HTMLITALIC') { return $self->makeResult("<I>", $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if (uc($cmd) eq 'HTMLRED') { return $self->makeResult("<FONT COLOR=RED>", $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
    if ($cmd =~ /"(.*)"/) { return $self->makeResult("$1", $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }

    # Output/check number of up connections
    if (uc($cmd) eq 'NU' || $cmd =~ /^NU!/i || $cmd =~ /^NU</i || $cmd =~ /^NU>/i ) {
	my $connArray = $self->{'db'}->findConnections($id,'%',0);
	my $n = scalar(@$connArray);
	if (uc($cmd) eq 'NU') { return $self->makeResult("$n", $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
	my $reqN = $';
	if ($n != $reqN && $cmd =~ /^NU!/i) { return ("ERROR: Found $n upward connections, expected $reqN"); }
	if ($n >= $reqN && $cmd =~ /^NU</i) { return ("ERROR: Found $n upward connections, expected < $reqN"); }
	if ($n <= $reqN && $cmd =~ /^NU>/i) { return ("ERROR: Found $n upward connections, expected > $reqN"); }
	return $self->doProcess($id,$slot,$fromId,$fromSlot,@rule);
    }

    # Output/check number of down connections
    if (uc($cmd) eq 'ND' || $cmd =~ /^ND!/i || $cmd =~ /^ND</i || $cmd =~ /^ND>/i ) {
	my $connArray = $self->{'db'}->findConnections($id,'%',1);
	my $n = scalar(@$connArray);
	if (uc($cmd) eq 'ND') { return $self->makeResult("$n", $self->doProcess($id,$slot,$fromId,$fromSlot,@rule)); }
	my $reqN = $';
	if ($n != $reqN && $cmd =~ /^ND!/i) { return ("ERROR: Found $n downward connections, expected $reqN"); }
	if ($n >= $reqN && $cmd =~ /^ND</i) { return ("ERROR: Found $n downward connections, expected < $reqN"); }
	if ($n <= $reqN && $cmd =~ /^ND>/i) { return ("ERROR: Found $n downward connections, expected > $reqN"); }
	return $self->doProcess($id,$slot,$fromId,$fromSlot,@rule);
    }

    # Find a specific alias
    if ($cmd =~ /^A[=!]/i) {
	my $convention = "$'";
	my $alias = $self->{'db'}->findAlias($id,$convention);
	if ($cmd =~ /^A!/i && ! $alias) {
	    return ("ERROR: No alias for convention $convention for $id");
	} else {
	    return $self->makeResult($alias, $self->doProcess($id,$slot,$fromId,$fromSlot,@rule));
	}
    }

    # Go one node up
    if ($cmd =~ /^U[=!]/i) {
	my $slot = "$'";
	my $connArray = $self->{'db'}->findConnections($id,$slot,0);
	my @result;
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[0],$$conn[1],$id,$$conn[3],@rule));
	}
	if ($cmd =~ /^U!/i && !scalar(@$connArray)) {
	    push (@result, "ERROR: No up slot $slot at ID = $id");
	}
	return @result;
    }

    # Go one node up, using the connection from the given slot
    if ($cmd =~ /^UF[=!]/i) {
	my $slot = "$'";
	my $connArray = $self->{'db'}->findConnectionsFromSlot($slot,$id);
	my @result;
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[0],$$conn[1],$id,$$conn[3],@rule));
	}
	if ($cmd =~ /^UF!/i && !scalar(@$connArray)) {
	    push (@result, "ERROR: No connection from slot $slot to ID = $id");
	}
	return @result;
    }

    # Go one node down
    if ($cmd =~ /^D[=!]/i) {
	my $slot = "$'";
	my $connArray = $self->{'db'}->findConnections($id,$slot,1);
	my @result;
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[2],$$conn[3],$id,$$conn[1],@rule));
	}
	if ($cmd =~ /^D!/i && !scalar(@$connArray)) {
	    push (@result, "ERROR: No down slot $slot at ID = $id");
	}
	return @result;
    }

    # Go one node down, using the connection ending at the given slot
    if ($cmd =~ /^DE[=!]/i) {
	my $slot = "$'";
	my $connArray = $self->{'db'}->findConnectionsToSlot($id,$slot);
	my @result;
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[2],$$conn[3],$id,$$conn[1],@rule));
	}
	if ($cmd =~ /^DE!/i && !scalar(@$connArray)) {
	    push (@result, "ERROR: No connection ending at slot $slot from ID = $id");
	}
	return @result;
    }

    # Search for connected objects by type (both up and down nodes)
    if ($cmd =~ /^T[=!]/i) {
	my $type = "$'";
	my @result;
	my $connArray = $self->{'db'}->findConnectionsByType($id,$type,0);
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[0],$$conn[1],$id,$$conn[3],@rule));
	}
	my $connArray2 = $self->{'db'}->findConnectionsByType($id,$type,1);
	foreach my $conn (@$connArray2) {
	    push(@result, $self->doProcess($$conn[2],$$conn[3],$id,$$conn[1],@rule));
	}
	if ($cmd =~ /^T!/i && scalar(@$connArray)+scalar(@$connArray2)==0) {
	    push (@result, "ERROR: No objects of type $type connected at ID = $id");
	}
	return @result;
    }

    # Search for connected objects by type (only up nodes)
    if ($cmd =~ /^TU[=!]/i) {
	my $type = "$'";
	my @result;
	my $connArray = $self->{'db'}->findConnectionsByType($id,$type,0);
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[0],$$conn[1],$id,$$conn[3],@rule));
	}
	if ($cmd =~ /^TU!/i && !scalar(@$connArray)) {
	    push (@result, "ERROR: No objects of type $type connected upwards of ID = $id");
	}
	return @result;
    }

    # Search for connected objects by type (only down nodes)
    if ($cmd =~ /^TD[=!]/i) {
	my $type = "$'";
	my @result;
	my $connArray = $self->{'db'}->findConnectionsByType($id,$type,1);
	foreach my $conn (@$connArray) {
	    push(@result, $self->doProcess($$conn[2],$$conn[3],$id,$$conn[1],@rule));
	}
	if ($cmd =~ /^TD!/i && !scalar(@$connArray)) {
	    push (@result, "ERROR: No objects of type $type connected downwards of ID = $id");
	}
	return @result;
    }

    return ("ERROR: Illegal command $cmd at ID = $id");
}


sub makeResult {
    my $self = shift;
    my ($s,@list) = @_;
    my @result;
    if (@list) {
	foreach my $l (@list) {
	    if ($l) {
		push(@result, join($self->{'separator'}, $s, $l));
	    } else {
		push(@result, $s);
	    }
	}
    } else {
	push(@result, $s);
    }
    return @result;
}


1;

=head1 AUTHOR

Written by Juerg Beringer, Lawrence Berkeley National Laboratory.

=cut
