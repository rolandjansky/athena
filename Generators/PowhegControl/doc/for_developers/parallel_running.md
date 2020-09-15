# Parallelizing Powheg runs with many random seeds at each stage

Note: this is not possible in V1.

Running the code proceeds in distinct stages. These stages and the possibility to parallelize them is discussed in section 5 ("Parallel runs") of V2-paper.pdf in the `POWHEG-BOX-V2/Docs/` directory. Here is a quick summary.

* parallelstage 1: importance sampling integration grids are generated. This stage can be iterated to refine the grids. The appropriate flag in the input file related to that iteration is "xgriditeration". When running in a parallel mode, one or two such iterations is sufficient. Files pertaining to the output of these stages carry either `xg1`, for the first xgriditeration, or `xg2` for the second xgriditeration in the name.
* parallelstage 2: importance sampling grids from the parallel runs in parallelstage 1 are first combined to yield a single optimised sampling grid. Then an upper bound for the so-called POWHEG Btilde function is searched for. This is needed as the initial "underlying Born" kinematics are generated according to this function. Files pertaining to the output of this stage generally carry an `st2` in the name.
* parallelstage 3: output from parallelstage 2 is combined to give an optimal upper bound for the Btilde function. Then an upper bound is found for the POWHEG Sudakov form factor ("Real/Born"). Related output files carry `st3` in the name.
* parallelstage 4: events are generated and output in `.lhe` event files. Also, depending on the nature of the input file, additional weights will accompany each event corresponding to e.g. scale variations. The default input file in the current directory is set up so that a 7 point renormalization and factorization scale variation is performed. The formatting of these weights and their meaning is obvious from looking at the end of the POWHEG input file / header of the LHE event files.
