# Experimental/additional PowhegControl features

This is documentation for experimenting around, with less support from the ATLAS
experts.

[[_TOC_]]

## Testing unsupported versions of processes with PowhegControl

If you want to test a process through PowhegControl, it is sometimes
possible to have it installed on afs, after which one can manually point
PowhegControl to this location. Firstly, check out the `External/Powheg`
and `Generators/PowhegControl` packages as described earlier.

Then one has to change the `POWHEGPATH` environment variable to point to
the location of the unofficial installation:

```bash
POWHEGPATH=/afs/direct_path_to_installed_process
```

One can then run the job as usual, and the version pointed to by
`POWHEGPATH` will be used.

## Testing private Powheg run cards with PowhegControl

For debugging, it may occasionally be useful to use the PowhegControl
machinery to generate events according to a privately provided run card.
If you do this

- we won't provide support
- you won't be able to request central production of your events

However, for private testing, this can be enabled by replacing the call
to

```py
PowhegConfig.generate()
```

with

```py
PowhegConfig.generate(use_external_run_card=True)
```


The job can then be run as usual, with all settings from PowhegControl
ignored in favour of settings from the private run card.

Note that using external run cards does not seem to work when generating
events in *multicore* mode due to PowhegControl interfering. Please
contact the PowhegControl maintainers if you need this, we may be able to
provide it.

## Generating Powheg run cards with PowhegControl

For debugging, it may occasionally be useful to use the PowhegControl
machinery to produce run cards which can be tested by non-ATLAS experts
(eg. the process authors). This can be enabled by replacing the call:

```py
PowhegConfig.generate()
```

with

```py
PowhegConfig.generate(create_run_card_only=True)
```

The job should be run as usual, but will terminate after the run card
has been produced.
