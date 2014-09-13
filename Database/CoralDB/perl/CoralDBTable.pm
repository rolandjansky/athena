# $Id: CoralDBTable.pm,v 1.5 2007-08-27 23:19:42 andr Exp $
#
#	Author:		J. Beringer
#
#	Purpose:	A Perl library for producing tables from a CoralDB connectivity
#                       database. The table contents are determined using rules as
#                       implemented in CoralDBRuleProcess.pm.
#
=head1 NAME

CoralDBTable - A package for producing tables from a CoralDB connectivity database

=head1 SYNOPSIS

  use CoralDBTable;
  $t = new CoralDBTable($coraldbRef);
  $t->loadRules("rules/MODULE.rules");
  $t->loadTable("rules/MODULE-DEFAULT.table");
  $t->print(@idList);

=head1 DESCRIPTION

CoralDBTable is a package for producing tables from a CoralDB connectivity
database. The table contents are determined using rules as
implemented in CoralDBRuleProcess.pm.

Currently only HTML-formatted tables are supported.

=head1 METHODS

=cut



package CoralDBTable;

use strict;
use CoralDB;
use CoralDBRuleProcessor;


my %defaultRules = (
		    'Object ID'                 => 'ID',
		    'Object TYPE'               => 'TYPE',
		    'Parent ID'                 => 'U=UP ID',
		    'Parent SLOT'               => 'U=UP SLOT',
		    'Parent TYPE'               => 'U=UP TYPE',
		    'Nodes with forward links'  => 'U=% ID',
		    'Nodes with backward links' => 'D=% ID',
		    );



=head2 $t = I<new> CoralDBTable($coraldbRef)

=over 4

This is the constructor. It takes a reference to a CoralDB database object.

=back

=cut
sub new {
    my $pkg = shift;
    my ($db) = @_;
    my $p = new CoralDBRuleProcessor($db);

    bless { 'pkg' => $pkg,
	    'db' => $db,
	    'p' => $p,
	    'nColumns' => 0,
	    'columnTitle' => [ ],
	    'columnRule' => [ ],
	    'objectLinkCallBack' => undef,
	    'rules' => \%defaultRules
	    }, $pkg;
}



=head2 $nColumns = $t->I<getNColumns>()

=over 4

Get the number of columns in the table.

=back

=cut
sub getNColumns {
    my $self = shift;
    return $self->{'nColumns'};
}



=head2 $t->I<setColumnTitle>($i, $title)

=over 4

Set the title of column $i.

=back

=cut
sub setColumnTitle {
    my $self = shift;
    my ($i,$title) = @_;
    $self->{'columnTitle'}->[$i] = $title;
    if ($i >= $self->{'nColumns'}) { $self->{'nColumns'} = $i+1; }
}



=head2 $columnTitle = $t->I<getColumnTitle>($i)

=over 4

Get the title of column $i.

=back

=cut
sub getColumnTitle {
    my $self = shift;
    my ($i) = @_;
    return $self->{'columnTitle'}->[$i];
}



=head2 $t->I<setColumnRule>($i, $rule)

=over 4

Define the rule to be executed to determine the contents of cells in column $i.

=back

=cut
sub setColumnRule {
    my $self = shift;
    my ($i,$rule) = @_;
    $self->{'columnRule'}->[$i] = $rule;
    if ($i >= $self->{'nColumns'}) { $self->{'nColumns'} = $i+1; }
}



=head2 $rule = $t->I<getColumnRule>($i)

=over 4

Get the rule used for column $i.

=back

=cut
sub getColumnRule {
    my $self = shift;
    my ($i) = @_;
    return $self->{'columnRule'}->[$i];
}



=head2 $t->I<setObjectLinkCallBack>($functionRef)

=over 4

Set the name of a callback function that will be called with the ID of an object and should
return a string containing the text to be written out for that object. This
could e.g. be a HTML link ("<a href=...>...</a>").

=back

=cut
sub setObjectLinkCallBack {
    my $self = shift;
    my ($f) = @_;
    $self->{'objectLinkCallBack'} = $f;
}



=head2 $hashRef = $t->I<getRuleNames>()

=over 4

Get a reference to a hash that contains the definition of all predefined named rules.

=back

=cut
sub getRuleNames {
    my $self = shift;
    my $ruleRef = $self->{'rules'};
    return sort keys %$ruleRef;
}



=head2 $rule = $t->I<getRule>($ruleName)

=over 4

Get the definition of a rule.

=back

=cut
sub getRule {
    my $self = shift;
    my ($ruleName) = @_;
    my $rule = $self->{'rules'}->{$ruleName};
    return $rule ? $rule : $ruleName;
}



=head2 $isRuleName = $t->I<isRuleName>($ruleName)

=over 4

Returns true if a rule named $ruleName is defined.

=back

=cut
sub isRuleName {
    my $self = shift;
    my ($ruleName) = (@_);
    return defined($self->{'rules'}->{$ruleName});
}



=head2 $nRules = $t->I<loadRules>($fileName)

=over 4

Load rule definitions from a file. Returns the number of rules loaded. Any previously defined rules are erased.

=back

=cut
sub loadRules {
    my $self = shift;
    my ($fileName) = (@_);
    if (open(RULES,$fileName)) {
	my %rules;
	LINE: while (<RULES>) {
	    chomp;
	    next LINE if ( /^#/ );
	    next LINE if ( /^$/ );
	    my $line = $_;
	    my @rule = split("\t", $line);
	    die "ERROR: Bad rule '$line' in file $fileName" unless (@rule ==2);
	    $rules{$rule[0]} = $rule[1];
	}
	close(RULES);
	$self->{'rules'} = \%rules;
	return scalar(keys %rules);
    } else {
	return 0;
    }
}



=head2 $nColumns = $t->I<loadTable>($fileName)

=over 4

Load table definition from a file. Returns the number of columns defined in the table.

=back

=cut
sub loadTable {
    my $self = shift;
    my ($fileName) = (@_);
    if (open(TABLE,$fileName)) {
	$self->{'nColumns'} = 0;
	$self->{'columnTitle'} = [ ];
	$self->{'columnRule'} = [ ];
	my $iCol = 0;
	LINE: while (<TABLE>) {
	    chomp;
	    next LINE if ( /^#/ );
	    next LINE if ( /^$/ );
	    my $line = $_;
	    my @colDef = split("\t", $line);
	    if (@colDef == 1) { push @colDef, $colDef[0]; }  # If only one column, assume rule and column title are the same
	    die "ERROR: Bad table column definition '$line' in file $fileName" unless (@colDef ==2);
	    $self->setColumnTitle($iCol,$colDef[0]);
	    $self->setColumnRule($iCol,$colDef[1]);
	    $iCol++;
	}
	close(TABLE);
	return $self->{'nColumns'};
    } else {
	return 0;
    }
}



=head2 $t->I<print>(@idList)

=over 4

Generate and print a table for the list of objects whose ID is passed in @idList.

=back

=cut
sub print {
    my $self = shift;
    my (@idList) = @_;
    my $colTitRef = $self->{'columnTitle'};
    my $colRuleRef = $self->{'columnRule'};

    print "<TABLE BORDER=1 CELLPADDING=4>\n";

    print "<TR>\n";
    for (my $i=0; $i<$self->{'nColumns'}; $i++) {
	print "<TD><B>$$colTitRef[$i]</B></TD>";
    }
    print "</TR>\n";

    foreach my $id (@idList) {
	print "<TR>\n";
	for (my $i=0; $i<$self->{'nColumns'}; $i++) {
	    my $rule = $self->getRule($$colRuleRef[$i]);
	    my @rule = split(/\s+/, $rule);
	    my @result = $self->{'p'}->process($id,@rule);
	    my $value = join("<BR>", @result);
	    # NOTE: At present only the output of rules that produce a single result can
	    #       be linked properly. To prevent an ID rule to create a link, simply
	    #       add a SPACE at the end. IDs with white space can currently not be linked,
	    #       because I cannot distinguish between an ID with whitespace and several IDs
	    #       separated by space.
	    if ($rule =~/ID$/ && @result == 1 && ! (@result[0] =~ /ERROR/) && scalar(split(/\s+/,$value))==1 ) {
		print "<TD>".$self->objectLink($value)."</TD>";
	    } else {
		print "<TD>$value</TD>";
	    }
	}
	print "</TR>\n";
    }
    print "</TABLE>\n";
}



sub objectLink {
    my $self = shift;
    my ($id) = @_;
    my $f = $self->{'objectLinkCallBack'};
    if ($f) {
	return $f->($id);
    } else {
	return $id;
    }
}


1;

=head1 BUGS

This package should be generalized to produce not only tables in HTML format, but to support other
formats such as plain text or LaTeX tables as well.

=head1 AUTHOR

Written by Juerg Beringer, Lawrence Berkeley National Laboratory.

=cut
