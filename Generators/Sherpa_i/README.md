[[_TOC_]]

# Introduction

Sherpa ([Reference](http://arxiv.org/abs/0811.4622)) is a multi-purpose Monte Carlo with special emphasis on matrix-element/parton-shower merging. The high-multiplicity parton-level matrix elements used in Sherpa are usually obtained from the internal matrix-element generator Comix ([Reference](http://arxiv.org/abs/0808.3674)). Parton showering is conducted by Csshower++ ([Reference](http://arxiv.org/abs/0709.1027)), which is based on Catani-Seymour subtraction terms. The combination of the matrix elements with this parton shower is accomplished according to an improved CKKW procedure ([Reference 1](http://arxiv.org/abs/hep-ph/0109231) , [Reference 2](http://arxiv.org/abs/0903.1219)) which in some recent samples is even extended to NLO accuracy using the MEPS@NLO method ([Reference](http://arxiv.org/abs/1207.5030)). The hadronisation of the shower configuration is performed by the internal fragmentation module AHADIC ([reference](http://arxiv.org/abs/arXiv:hep-ph/0311085)) and QED FSR radiation from signal leptons and hadron decays is done in a YFS-based approach ([Reference](http://arxiv.org/abs/0810.5071)).

As a **general introduction to Sherpa**, you may find the recording of Marek's presentation at a recent [CMS Sherpa tutorial](https://indico.cern.ch/event/555805/) useful.

If you are interested in Sherpa samples or need support in their generation, please subscribe+post to the [atlas-generators-sherpa@cern.ch](mailto:atlas-generators-sherpa@cern.ch) egroup, an archive of which can be found [here](https://groups.cern.ch/group/atlas-generators-sherpa/default.aspx). Historical information about Sherpa in ATLAS not relevant for the latest release can also be found in [SherpaForAtlas](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SherpaForAtlas).

# Using Sherpa within Athena

The recommended way of getting Sherpa events into Athena is to use the on-the-fly interface which generates Sherpa events within Athena.

## Job Options

Event generation jobs in ATLAS are steered using jobOption files and run using the `Gen_tf.py` transform as described in [PmgMcSoftware#Production_transforms_and_job_op](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgMcSoftware#Production_transforms_and_job_op).

A simple example JO for running Sherpa on-the-fly within Athena can be found in the [Sherpa_i interface](https://gitlab.cern.ch/atlas/athena/blob/21.6/Generators/Sherpa_i/share/example/simple/). Please see the documentation in that directory for more details on how to write Sherpa job option files.

## Running the event generation

Such a JO lives in a directory (e.g. `./athena/Generators/Sherpa_i/share/example/simple/mc.Sh_Example.py`) and can be used to generate events with Gen_tf:
```
Gen_tf.py --ecmEnergy=13000.0 --randomSeed=1234 --jobConfig=./athena/Generators/Sherpa_i/share/example/simple --outputEVNTFile=tmp.EVNT.root --maxEvents=10
```
as described in [PmgMcSoftware#Production_transforms_and_job_op](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgMcSoftware#Production_transforms_and_job_op).

In some Sherpa setups, this might stop with a message like:
```
generate 16:59:26 Sherpa_i            ERROR Have to compile Amegic libraries
generate 16:59:26 Sherpa_i            ERROR You probably want to run ./makelibs
```
In this case you have to execute the freshly created `./makelibs` in the same working directory and then restart the `Gen_tf` command as above.

## Using input tarballs to shorten the initialisation step

It will take a few minutes to initialise the generator, e.g. performing the phase space integrations over the Z+n jet processes. The initialisation results will be written out to `Process/` and `Results.db`.

The time taken by this initialisation step depends very much on the complexity of the process and can easily take in the order of a day for complicated processes. To avoid the initialisation step in case you are doing multiple evgen runs, `Sherpa_i` allows the usage of an input file which stores process libraries or integration results. This is beam-dependent and can be provided in the setup directory as e.g. `simple/mc_13TeV.Sh_Example.GRID.tar.gz`. The Gen_tf.py transform will take care of unpacking the tarball and thus its contents will be available for the generator (interface) in the working directory at run time.
```
tar czf /path/to/999999/mc_13TeV.Sh_Example.GRID.tar.gz Process/ Results.db
Gen_tf.py --ecmEnergy=13000.0 --randomSeed=1234 --jobConfig=./athena/Generators/Sherpa_i/share/example/simple --outputEVNTFile=tmp.EVNT.root --maxEvents=10
## this run should start generating events more quickly than before
```

Official Sherpa JOs and input tarballs have DSIDs in the 7xxxxx range and can be found in `/cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/700xxx` etc or browsed in [GitLab](https://gitlab.cern.ch/atlas-physics/pmg/mcjoboptions/tree/master).

Notes for running with input tarball:
* The tarball can contain custom libraries or non-standard PDF sets, should you choose to use those.
* You can specify the `maxEvents=1` argument to Gen_tf if you only want to create the `Process/` and `Results.db` directories for the input tarball and not generate any events (but we recommend using the `sherpaTarCreator` script described below to automatically create input tarballs).
* This is also possible for [central production](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/McSampleRequestProcedure), in which case not only the JO has to be uploaded but also that tarball during the [sample request procedure](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/McSampleRequestProcedure).

## Preparing input tarballs with sherpaTarCreator

A tool to automatically generate the input tarball for a given Sherpa JO is provided within the Sherpa_i interface and is available as `sherpaTarCreator.py` after asetup. Documentation can be found in [git](https://gitlab.cern.ch/atlas/athena/-/tree/21.6/Generators/Sherpa_i/python/sherpaTarCreator/).

If you want to create tarballs for an older release (<=21.6.48) in which the sherpaTarCreator did not exist yet, you may obtain a standalone version of the tool from [[https://gitlab.cern.ch/atlas-physics/pmg/tools/sherpaTarCreator/]] (the documentation in git above still applies).

# Some practical information for users

## MC event weight
* The weights can have an unusual normalization, such that numbers at the order of 10e7 are expected. But this will not affect distributions, as the overall factor will always be divided out through the sum of weights when scaling distributions to the luminosity/cross section.
* For samples with N different enrichment factors you will see a smooth weight distribution with N bumps, due to additional internal (MENLOPS) weights.
## Sherpa event record
* Starting from 1.4.0, Sherpa follows the [current HepMC convention](http://lcgapp.cern.ch/project/simu/HepMC/206/HepMC2_user_manual.pdf). Final state particles still have status=1 but status 2 was replaced by multiple status codes: 2 for "physical" intermediate states (hadron resonances), 11 for non-physical intermediate states (e.g. parton shower evolution), 3 for particles in the hard scattering matrix element, and 4 for incoming beams. An example: If in a Z-&gt;ee sample you need the electron/positron before QEDFSR radiation, you need to select status==11
* For NLO processes the status 3 particles from the hard scattering matrix element will contain the one additional parton emission which is matched between ME and shower. An additional level of original particles before that emission is being made available as separate status 20 particles for events where they would be different from status 3 particles. So if you want to select parton-level event kinematics, you should always use status 20 if available in the event and otherwise status 3.
* Note that Sherpa does not output any heuristic intermediate particles (propagators) for the hard scattering process. For example, if you are generating pp -&gt; e+ nue e- nuebar then you will not find any intermediate bosons (e.g. pp -&gt; W+[e+ nue] W-[e- nuebar]) in the event record. This is due to the fact that Sherpa generates all diagrams pertaining to that final state and the corresponding interference, such that such an identification is not physical. In this example, the simulation also includes diagrams like pp -&gt; Z[e+e-] Z[nue nuebar] or singly-resonant production at the diagram level, so it would not make sense to identify a W+W- in the event record (=squared ME level).
* Sherpa versions &lt;=2.1.1 contain "loops" in the event record, representing the particles in the "Parton shower" vertex in this picture: [record_named.pdf](%ATTACHURL%/record_named.pdf) They are straightened out using `fixSeq.FixHepMC.LoopsByBarcode = True` in Athena. Sherpa &gt;=2.2.0 on the other hand does not need this, because it has an internal mechanism, which cuts out the parts of the event that are inside the "Parton shower" blob, to avoid loops (enabled by `HEPMC_TREE_LIKE=1` in the base fragment). These cut-outs are then added as separate disconnected parts of the event, so that they are still available for the identification of the hard scattering process.

## Preparing tarballs using different .db files

When a JO is composed of different processes, one way to build an integration tarball is to split the JO into individual ones for each single process block and at the end combine the different Results.db files into one single file which will be contained in the tarball.
To combine the .db files, one can use the [MergeDb.sh](https://twiki.cern.ch/twiki/pub/AtlasProtected/SherpaForAtlas/MergeDb.sh) combined with [Db2Dir](https://twiki.cern.ch/twiki/pub/AtlasProtected/SherpaForAtlas/Db2Dir) and [Dir2Db](https://twiki.cern.ch/twiki/pub/AtlasProtected/SherpaForAtlas/Dir2Db) script files. This script will merge different input .db files and merge them into one single output .db file.
```
./MergeDb.sh file1.db file2.db ... output.db
```
The script Db2Dir and Dir2Db should be in the same directory and be executable.