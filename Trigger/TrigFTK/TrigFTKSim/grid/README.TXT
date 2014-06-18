Set of back-end scripts used to submit FTK jobs on the grid
See job submission templates in scripts/grid.sh

Please use provided pbook.py instead of pbook to retry FTK jobs!

TECHNICAL NOTE:
FTK uses a modified version of PandaRun executable - prunftk.py
This means that upstream changes need to be merged back from time to time.
For convenience, the last version of panda-client from which prunftk.py
was forked is specified in _FORKED_FROM variable in prunftk.py
