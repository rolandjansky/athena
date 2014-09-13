# $Id: CoralDB.pm,v 1.15 2007-11-12 21:33:47 beringer Exp $

=head1 NAME

CoralDB - API to the ATLAS Pixel Detector Connectivity Database (based on CORAL)

=head1 SYNOPSIS

  use CoralDB;
  my $cdb = new CoralDB($database, $user, $password);

=head1 DESCRIPTION

This module provides a read-only object-oriented API to the CoralDB connectivity database.

The current implementation of CoralDB.pm does not use CORAL and instead accesses databases
directly through DBI using SQL statements that are crafted in order to be compatible with
different databases such as Oracle and SQLite. Since this is not guaranteed to work in all cases, and in
order to avoid corrupting any databases, this is a read-only interface. All write access should
happen through the corresponding C++ interface.

Versioning in CoralDB uses 4 different types of tags. Where a parameter $tagType is required by
a method, it must have one of the following values:

- "idTag" refers to versions of the object dictionary.

- "connTag" refers to versions of the connectivity table.

- "dataTag" refers to versions of payload data (CLOB table).

- "aliasTag" refers to versions of the aliases table.

Versions of the connectivity table, payload data and aliases always refer to a given
given tag of the object dictionary. All methods refer to the set of tags that was previously
set specified using I<setTag> (unless of course the tag to be used is specified explicitly as an argument).

=head1 METHODS

=cut



package CoralDB;

use strict;
use DBI;



=head2 $cdb = I<new CoralDB>($database, $user, $pwd)

=over 4

This is the constructor. It takes the database specification in DBI format (e.g. dbi:SQLite:file.db),
user name ($user) and password ($pwd) as arguments.

=back
=cut
sub new {
    my $pkg = shift;
    my ($database,$user,$pwd,$prefix) = @_;
    unless ($prefix) { $prefix = 'CORALDB2'; }
    my $dbh = DBI->connect($database,$user,$pwd, {RaiseError => 1});
    bless { 'pkg' => $pkg,
	    'database' => $database,
	    'user' => $user,
	    'pwd' => $pwd,
	    'dbh' => $dbh,
	    'idTag'    => 'DEFAULT',
	    'connTag'  => 'DEFAULT',
	    'dataTag'  => 'DEFAULT',
	    'aliasTag' => 'DEFAULT',
	    'prefix' => $prefix,
            'tagTables' => { idTag     => "${prefix}_OBJECTDICT_TAGS",
			     connTag   => "${prefix}_CONNECTIONS_TAGS",
			     dataTag   => "${prefix}_CLOBS_TAGS",
			     aliasTag  => "${prefix}_ALIASES_TAGS" }
	    }, $pkg;
}



=head2 $cdb->I<dump>( )

=over 4

Utility method to print the internal variables of the CoralDB object.

=back

=cut
sub dump {
    my $self = shift;
    foreach my $item (sort keys %$self) {
	print "$item = ".($self->{$item} || '')."\n";
    }
}



=head2 $cdb->I<setTag>($tagType, $tag)

=over 4

Define the tag to be used in the following.

=back

=cut
sub setTag {
    my $self = shift;
    my ($tagType,$tag) = @_;
    unless (defined($self->{tagTables}->{$tagType})) { die "ERROR: Illegal tag type $tagType" };
    $self->{$tagType} = $tag;
}



=head2 @listOfTags = $cdb->I<getAllTags>($tagType, $idTag)

=over 4

Get all tags of type $tagType. Unless $tagType is "idTag", the object dictionary to be
used must be given as the 2nd parameter.

=back

=cut
sub getAllTags {
    my $self = shift;
    my ($tagType,$idTag) = @_;
    unless (defined($self->{tagTables}->{$tagType})) { die "ERROR: Illegal tag type $tagType" };
    my $ref;
    if ($tagType eq 'idTag') {
	$ref = $self->{'dbh'}->selectcol_arrayref("select distinct IDTAG from $self->{tagTables}->{$tagType} order by IDTAG");	
    } else {
	if (defined($idTag)) {
	    $ref = $self->{'dbh'}->selectcol_arrayref("select distinct TAG from $self->{tagTables}->{$tagType} where IDTAG = ? order by TAG", {}, ($idTag));
	} else {
	    $ref = $self->{'dbh'}->selectcol_arrayref("select distinct TAG from $self->{tagTables}->{$tagType} order by TAG");
	}
    }
    return defined($ref) ? @$ref : ();
}



=head2 $isLocked = $cdb->I<isLockedTag>($tagType,$idTag,$tag)

=over 4

Determine if a given tag is locked. If $tagType is "idTag", the 3rd parameter is not used.

=back

=cut
sub isLockedTag {
    my $self = shift;
    my ($tagType,$idTag,$tag) = @_;
    my $ref;
    if ($tagType eq 'idTag') {
	$ref = $self->{'dbh'}->selectcol_arrayref("select LOCKED from $self->{tagTables}->{$tagType} where IDTAG = ?", {}, ($idTag));
    } else {
	$ref = $self->{'dbh'}->selectcol_arrayref("select LOCKED from $self->{tagTables}->{$tagType} where IDTAG = ? and TAG = ?", {}, ($idTag,$tag));
    }
    return $$ref[0];
}



=head2 $arrayRef = $cdb->I<getTagHistory>()

=over 4

Get the complete history of current tags. The return value is a reference to an array, whose elements are references
to arrays with the data from the rows of the tag history table. Each row contains the start of the interval of validity
for this entry in the Unix time format (seconds since the epoch), and the object dictionary, connectivity, data and alias
tags.

=back

=cut
sub getTagHistory {
    my $self = shift;
    # NOTE: The first column is the time in seconds since the epoch (Unix time format)
    my $ref = $self->{'dbh'}->selectall_arrayref("select IOV_BEGIN,IDTAG,CONNECTIVITY_TAG,DATA_TAG,ALIAS_TAG from $self->{prefix}_TAG_HISTORY order by IOV_BEGIN");
    return defined($ref) ? $ref : ();
}



=head2 @masterList = $cdb->I<masterList>()

=over 4

Get the master list of objects. These objects are intended to represent the top of the connectivity tree and can be used
as starting points for navigating the connectivity.

=back

=cut
sub masterList {
    my $self = shift;
    my $ref = $self->{'dbh'}->selectcol_arrayref("select ID from $self->{prefix}_MASTERLIST where TAG = ? and IDTAG = ?", {}, ($self->{'connTag'},$self->{'idTag'}));
    return defined($ref) ? @$ref : ();
}



=head2 @objectTypeList = $cdb->I<getAllObjectTypes>()

=over 4

Get a list of all object types.

=back

=cut
sub getAllObjectTypes {
    my $self = shift;
    my $ref = $self->{'dbh'}->selectcol_arrayref("select distinct TYPE from $self->{prefix}_OBJECTDICT where IDTAG = ? order by TYPE", {}, ($self->{'idTag'}));
    return defined($ref) ? @$ref : ();
}



=head2 $type = $cdb->I<objectType>($id)

=over 4

Determine the type of object $id.

=back

=cut
sub objectType {
    my $self = shift;
    my ($id) = @_;
    my $ref = $self->{'dbh'}->selectcol_arrayref("select TYPE from $self->{prefix}_OBJECTDICT where IDTAG = ? and ID = ?", {}, ($self->{'idTag'},$id));
    return $$ref[0];
}



=head2 @objectList = $cdb->I<findObjects>($name, $searchWhere, $onlyType)

=over 4

Find objects with a given name $name. "%" can be used as a wildcard to match any part of the name.
$searchWhere is used to specify whether $name represents the name of the object ("ID"), the name of the type of the
object ("TYPE"), an alias ("ALIAS"), or any name ("ANY NAME"). $onlyType can be used to restrict
the search to objects of type $onlyType if set to something other than "" or "ANY TYPE".

=back

=cut
sub findObjects {
    my $self = shift;
    my ($name,$searchWhere,$onlyType) = @_;
    my $ref = [];
    if ($name eq '') { $name = '%'; }
    my $typeCondition;
    unless ($onlyType eq '' || uc($onlyType) eq 'ANY TYPE') { $typeCondition = "and TYPE = '$onlyType'"; }

    if (uc($searchWhere) eq 'TYPE') {
	$ref = $self->{'dbh'}->selectcol_arrayref("select distinct ID
            from $self->{prefix}_OBJECTDICT
            where IDTAG = ? and TYPE like ? escape '|' $typeCondition
            order by ID", {},
            ($self->{'idTag'},privateSQLQuote($name,'|')));
    }

    if (uc($searchWhere) eq 'ID') {
	$ref = $self->{'dbh'}->selectcol_arrayref("select distinct ID
            from $self->{prefix}_OBJECTDICT
            where IDTAG = ? and ID like ? escape '|' $typeCondition
            order by ID", {},
            ($self->{'idTag'},privateSQLQuote($name,'|')));
    }

    if (uc($searchWhere) eq 'ALIAS') {
	$ref = $self->{'dbh'}->selectcol_arrayref("select distinct A.ID
            from $self->{prefix}_ALIASES A inner join $self->{prefix}_OBJECTDICT B ON (a.ID = b.ID and a.IDTAG = b.IDTAG)
            where A.IDTAG = ? and TAG = ? and ALIAS like ? escape '|' $typeCondition
            order by A.ID", {},
            ($self->{'idTag'},$self->{'aliasTag'},privateSQLQuote($name,'|')));
    }

    if (uc($searchWhere) eq 'ANY NAME') {
	$ref = $self->{'dbh'}->selectcol_arrayref("select distinct A.ID
            from $self->{prefix}_ALIASES A inner join $self->{prefix}_OBJECTDICT B ON (A.ID = B.ID and a.IDTAG = b.IDTAG)
            where  A.IDTAG = ? and TAG = ? and ALIAS like ? escape '|' $typeCondition
            union
            select distinct ID
            from $self->{prefix}_OBJECTDICT
            where IDTAG = ? and ID like ? escape '|' $typeCondition
            order by ID", {},
            ($self->{'idTag'},$self->{'aliasTag'},privateSQLQuote($name,'|'),$self->{'idTag'},privateSQLQuote($name,'|')));
    }

    return @$ref;
}



=head2 $arrayRef = $cdb->I<findAllConnections>($id, $getOutConnections)

=over 4

Get a list of all incoming ($getOutConnections set to false) or outgoing ($getOutConnections set to true) connections
for object with ID $id. The return value is a reference to an array, whose elements are references to connections.
Each connection is represented by an array whose elements are the ID and SLOT of the source objected, following by
the ID and slot of the destination object.

=back

=cut
sub findAllConnections {
    my $self = shift;
    my ($id,$getOutConnections) = @_;
    unless (defined($getOutConnections)) { $getOutConnections = 1; }
    my $resRef;
    if ($getOutConnections) {
	$resRef = $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where ID = ? and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,$self->{'idTag'},$self->{'connTag'}));
    } else {
	$resRef = $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where TOID = ? and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,$self->{'idTag'},$self->{'connTag'}));
    }
    return $resRef;
}



=head2 $arrayRef = $cdb->I<findConnections>($id, $slot, $getOutConnections)

=over 4

Similar to findAllConnections, but only the connections using slot $slot on object $id are returned.
$slot may contain "%" as a wildcard.

=back

=cut
sub findConnections {
    my $self = shift;
    my ($id,$slot,$getOutConnections) = @_;
    my $resRef;
    if ($getOutConnections) {
	$resRef = $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where ID = ? and SLOT like ? escape '|' and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,privateSQLQuote($slot,'|'),$self->{'idTag'},$self->{'connTag'}));
    } else {
	$resRef = $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where TOID = ? and TOSLOT like ? escape '|' and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,privateSQLQuote($slot,'|'),$self->{'idTag'},$self->{'connTag'}));
    }
    return $resRef;
}



=head2  $arrayRef = $cdb->I<findConnectionsByType>($id, $type, $getOutConnections)

=over 4

Similar to findAllConnections, but only the connections to or from objects of type $type are returned.
$type may contain "%" as a wildcard.

=back

=cut
sub findConnectionsByType {
    my $self = shift;
    my ($id,$type,$getOutConnections) = @_;
    my $resRef;
    if ($getOutConnections) {
	$resRef = $self->{'dbh'}->selectall_arrayref("select C.ID,C.SLOT,C.TOID,C.TOSLOT
            from $self->{prefix}_CONNECTIONS C inner join $self->{prefix}_OBJECTDICT O ON (C.TOID = O.ID and C.IDTAG = O.IDTAG)
            where C.ID = ? and TYPE like ? escape '|' and C.IDTAG = ? and TAG = ? order by SLOT", {}, ($id,privateSQLQuote($type,'|'),$self->{'idTag'},$self->{'connTag'}));
    } else {
	$resRef = $self->{'dbh'}->selectall_arrayref("select C.ID,C.SLOT,C.TOID,C.TOSLOT
            from $self->{prefix}_CONNECTIONS C inner join $self->{prefix}_OBJECTDICT O ON (C.ID = O.ID and C.IDTAG = O.IDTAG)
            where C.TOID = ? and TYPE like ? escape '|' and C.IDTAG = ? and TAG = ? order by SLOT", {}, ($id,privateSQLQuote($type,'|'),$self->{'idTag'},$self->{'connTag'}));
    }
    return $resRef;
}



=head2  $arrayRef = $cdb->I<findConnectionsFromSlot>($slot, $toId)

=over 4

Similar to findAllConnections, but only connections from a given outgoing slot $slot to object with ID $toId are returned. $slot may contain "%" as a wildcard.

=back

=cut
sub findConnectionsFromSlot {
    my $self = shift;
    my ($slot,$toId) = @_;
    return $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where SLOT like ? escape '|' and TOID = ? and IDTAG = ? and TAG = ? order by TOSLOT", {}, (privateSQLQuote($slot,'|'),$toId,$self->{'idTag'},$self->{'connTag'}));
}



=head2 $arrayRef = $cdb->I<findConnectionsToSlot>($id, $toSlot)

=over 4

Similar to findAllConnections, but only connections to slot $slot at object with ID $id are returned.
$toSlot may contain "%" as a wildcard.

=back

=cut
sub findConnectionsToSlot {
    my $self = shift;
    my ($id,$toSlot) = @_;
    return $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where ID = ? and TOSLOT like ? escape '|' and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,privateSQLQuote($toSlot,'|'),$self->{'idTag'},$self->{'connTag'}));
}



=head2 $arrayRef = $cdb->I<findSoftLinks>($id, $getOutConnections)

=over 4

Similar to findAllConnections, but only connections representing "soft links" are returned, ie. "hard links"
are excluded. Connections representing hard links are connections that are flagged by the special slot
name "UP" on the destination side. The subset of hard link connections represents a directed acyclic graph.

=back

=cut
sub findSoftLinks {
    my $self = shift;
    my ($id,$getOutConnections) = @_;
    unless (defined($getOutConnections)) { $getOutConnections = 1; }
    my $resRef;
    if ($getOutConnections) {
	$resRef = $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where ID = ? and TOSLOT <> 'UP' and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,$self->{'idTag'},$self->{'connTag'}));
    } else {
	$resRef = $self->{'dbh'}->selectall_arrayref("select ID,SLOT,TOID,TOSLOT from $self->{prefix}_CONNECTIONS where TOID = ? and TOSLOT <> 'UP' and IDTAG = ? and TAG = ? order by SLOT", {}, ($id,$self->{'idTag'},$self->{'connTag'}));
    }
    return $resRef;
}



=head2 @aliasConventionList = $cdb->I<getAllAliasConventions>()

=over 4

Get a list of all the different alias conventions.

=back

=cut
sub getAllAliasConventions {
    my $self = shift;
    my $ref = $self->{'dbh'}->selectcol_arrayref("select distinct CONVENTION from $self->{prefix}_ALIASES where IDTAG = ? and TAG = ? order by CONVENTION", {}, ($self->{'idTag'},$self->{'aliasTag'}));
    return defined($ref) ? @$ref : ();
}



=head2 $arrayRef = $cdb->I<findAliases>($id)

=over 4

Find all aliases for object with ID $id. The return value is a reference to an array, whose elements are references
to aliases. Each alias is represented by an array, whose elements are the alias and the convention.

=back

=cut
sub findAliases {
    my $self = shift;
    my ($id) = @_;
    my $resRef = $self->{'dbh'}->selectall_arrayref("select ALIAS,CONVENTION from $self->{prefix}_ALIASES where ID = ? and IDTAG = ? and TAG = ? order by CONVENTION", {}, ($id,$self->{'idTag'},$self->{'aliasTag'}));
    return $resRef;
}



=head2 $alias = $cdb->I<findAlias>($id, $convention)

=over 4

Find the alias of object $id for convention $convention.

=back

=cut
sub findAlias {
    my $self = shift;
    my ($id,$convention) = @_;
    my $ref = $self->{'dbh'}->selectcol_arrayref("select ALIAS from $self->{prefix}_ALIASES where ID = ? and CONVENTION = ? and IDTAG = ? and TAG = ?", {}, ($id,$convention,$self->{'idTag'},$self->{'aliasTag'}));
    return $$ref[0];
}



=head2 $arrayRef = $cdb->I<findClobs>($id)

=over 4

Retrieve all the CLOBs attached to object with ID $id. The return value is a reference to an array, whose elements are references to
CLOBs. Each CLOB is represented by a list, whose elements are field name of the CLOB, and the CLOB itself.

=back

=cut
sub findClobs {
    my $self = shift;
    my ($id) = @_;
    my $resRef = $self->{'dbh'}->selectall_arrayref("select FIELD,DATA from $self->{prefix}_CLOBS inner join $self->{prefix}_CLOBS_REVS using (ID,REV,IDTAG) where ID = ? and IDTAG = ? and TAG = ? order by FIELD", {}, ($id,$self->{'idTag'},$self->{'dataTag'}));
    return $resRef;
}


sub privateSQLQuote {
    my ($s,$esc) = @_;
    $s =~ s/_/${esc}_/g;   # Quote _
    #print "quoted: $s\n";
    return $s;
}


1;

=head1 AUTHOR

Written by Juerg Beringer, Lawrence Berkeley National Laboratory.

=cut
