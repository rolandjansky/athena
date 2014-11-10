#!/usr/bin/awk -f

function join(a, b)				# print "a/b", but suppress the slash if either is empty
{
    if (a && b) return a "/" b;
    if (a)      return a;
    if (b)      return b;
}

BEGIN {
    level = 0;					# nesting level of braces (scope depth)
    name[level] = "";				# name of "dir" node at this scope
    output[level] = "top_level";		# value of "output" at this scope
    split("", map);				# one way to initialise an empty array
    hist = "";					# name of "hist" node, if any
}

{						# do this always
    sub(/#.*/, "");				# remove comments
    gsub(/[[:space:]]+/, " ");			# normalise whitespace
    sub(/^ /, "");				# trim leading space
    sub(/ $/, "");				# trim trailing space
}

/ {$/ {						# entering a scope block
    ++level;					# going one level deeper
}

/^}$/ {						# leaving a scope block
    if (level == 0) exit;			# something is wrong (should not happen)
    --level;					# going one level higher
    hist = "";					# in case we were in a "hist" node: now we've left it
}

/^dir [[:graph:]]+ {$/ {			# entering a "dir" node
    output[level] = output[level - 1];		# inherited from the outer scope by default
    name[level] = join(name[level - 1], $2);	# full path inside the input file
}

/^hist [[:graph:]]+ {$/ {			# entering a "hist" node
    sub(/all_in_dir/, ".*");			# not perfect, but at least a hint
    hist = $2;					# a "hist" node may contain a dedicated "output" value
    output[level] = output[level - 1];		# inherited from the outer scope by default
    sub(/@[[:graph:]]+$/, "", $2);		# remove an additional "@" modifier, if it exists
    name[level] = join(name[level - 1], $2);	# full path inside the input file
    map[name[level]] = join(output[level], hist); # may be overwritten by a dedicated "output" value
}

/^output = / {					# specification of an "output" value
    output[level] = $3;				# in a "dir" node: pass it to enclosed blocks
    if (hist) map[name[level]] = join(output[level], hist); # in a "hist" node: apply it immediately
}						# works only if it appears in front of nested "dir" nodes

END {
    for (i in map) printf("%s\t%s\n", i, map[i]); # dump all saved pairs (unsorted)
}
