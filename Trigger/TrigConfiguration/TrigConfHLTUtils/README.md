# TrigConfHLTUtils

Package to host the `string2hash`, `hash2string`, `hashes2file` and `file2hashes` helper functions.

In Run 2 these were hosted in the `Trigger/TriConfiguration/TrigConfHLTData` package along with Run 2 specific trigger element helper functions.

For Run 3, the deprecated trigger element helper functions are moved under a new `HLTTEUtils` class, the hashing helper functions remain under the `HLTUtils` class and move into this package.

This package cannot have any Gaudi dependencies.

