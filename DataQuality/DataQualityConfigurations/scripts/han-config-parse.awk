#!/bin/awk -f
#
# Usage: han-config-parse.awk FILE ...		# check input file(s) for syntax errors
#        han-config-parse.awk DEF=1 FILE ...	# also print out all defined objects
#        han-config-parse.awk REQ=1 FILE ...	# also print out all required objects
#        han-config-parse.awk --lint FILE ...	# also check this script for internal problems

BEGIN {
    errors = 0;					# error count
    nodeType = "";				# dir, output, algorithm, thresholds
    leafType = "";				# hist, compositeAlgorithm, reference, limits, metadata
    nodeLevel = 0;				# level of nesting (all nodes)
    leafLevel = 0;				# level of additional nesting (only leaf nodes, must not exceed 1)
    name[nodeLevel] = "";			# block names of all levels
    DEF = "";					# set this on the command line to print all defined objects
    REQ = "";					# set this on the command line to print all required objects
}

function stderr(message)			# print error message
{
    print message > "/dev/stderr";
}

function error(message)				# print error message, including filename and line number
{
    stderr(FILENAME ":" FNR ": " message);
    ++errors;
}

function quote(message)				# return message in doublequotes
{
    return "\"" message "\"";
}

function clean()				# clean input, remove unnecessary decorations
{
    sub("#.*", "");				# remove comments
    gsub("[[:space:]]+", " ");			# normalise spaces
    sub("^ ", "");				# trim leading spaces
    sub(" $", "");				# trim trailing spaces
    if ($0 == "") next;				# skip empty lines
}

function check()				# check input, accept only well-formed lines
{
    if ($0 ~ /^(dir|hist|output|algorithm|compositeAlgorithm|reference|thresholds|limits|metadata) [[:graph:]]+ {$/) return; # block opening
    if ($0 ~ /^[[:graph:]]+ = [[:graph:]]+$/) return;			# must not contain whitespace
    if ($0 ~ /^(display|description|info) = [[:print:]]+$/) return;		# may contain whitespace
    if ($0 ~ /^Revision = \$Revision: [[:digit:]]* ?\$$/) return;	# SVN keyword
    if ($0 ~ /^URL = \$URL: [[:graph:]]* ?\$$/) return;			# SVN keyword
    if ($0 ~ /^}$/) return;			# block closing
    error("malformed line: " quote($0));	# something else
    next;					# skip this line
}

{						# do this for every line
    clean();
    check();
}

/{$/ {						# block opening: leaf nodes must not be nested
    if (leafLevel) error("already in " quote(leafType " " name[nodeLevel]) ", cannot open " quote($1 " " $2));
    if ($2 ~ /^\//)  error(quote($1 " " $2) " should not contain a leading slash");
    if ($2 ~ /\/$/)  error(quote($1 " " $2) " should not contain a trailing slash");
    if ($2 ~ /\/\//) error(quote($1 " " $2) " should not contain double slashes");
    ++nodeLevel;
}

/^(dir|output|algorithm|thresholds) [[:graph:]]+ {$/ { # block opening: non-leaf node
    if (nodeType != $1) {			# node type can only change at the highest level
        if (nodeLevel != 1) error("still in " quote(nodeType " " name[nodeLevel - 1]) ", cannot open " quote($1 " " $2));
        nodeType = $1;				# accept the new type anyway to avoid additional errors
    }
    parent = name[nodeLevel - 1];		# will be empty at the highest level
    prefix = (parent == "" || parent == "top_level") ? "" : parent "/"; # avoid leading slash, suppress leading "top_level"
    name[nodeLevel] = prefix $2;		# contains the full path in case of non-leaf nodes
}

/^(hist|compositeAlgorithm|reference|limits|metadata) [[:graph:]]+ {$/ { # block opening: leaf node
    ++leafLevel;				# should be 0 or 1, anything greater indicates an error
    leafType = $1;				# only for error reporting, cannot be nested
    name[nodeLevel] = $2;			# only for error reporting, cannot be referenced
}

/^(output|algorithm|reference|thresholds) [[:graph:]]+ {$/ { # block opening: node that can be referenced
    if (DEF) {
        if (name[nodeLevel] != "top_level") print $1 " " name[nodeLevel]; # suppress "top_level"
    }
}

/^(output|algorithm|reference|thresholds) = / {	# reference to another node
    if ($3 ~ /^\//)  error(quote($1 " " $3) " should not contain a leading slash");
    if ($3 ~ /\/$/)  error(quote($1 " " $3) " should not contain a trailing slash");
    if ($3 ~ /\/\//) error(quote($1 " " $3) " should not contain double slashes");
    if (REQ) {
        if ($1 != "reference") {
            print $1 " " $3;
        } else {
            split($3, reference, ";");		# split stream-dependent references, if applicable
            for (i in reference) {
                sub(".*:", "", reference[i]);	# strip away the stream name
                print $1 " " reference[i];	# print each item separately
            }
        }
    }
}

/}$/ {						# block closing
    if (leafLevel) --leafLevel;			# should be 0 or 1, anything greater indicates an error
    if (nodeLevel) --nodeLevel;			# must not become negative, print an error message instead
    else error("trying to close a block that is not open");
}

END {
    if (nodeLevel) error("unclosed block " quote(nodeType " " name[nodeLevel]));
    if (errors) {
        stderr("Found " errors " errors");	# error summary
        close("/dev/stderr");			# we used stderr, close it so lint does not complain
        exit 1;					# any error message implies a failure
    }
}
