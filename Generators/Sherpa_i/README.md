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
* The weights can have an unusual normalization, such that numbers on the order of 10e7 are expected. But this will not affect distributions, as the overall factor will always be divided out through the sum of weights when scaling distributions to the luminosity/cross section.
* For samples with N different enrichment factors you will see a smooth weight distribution with N bumps, due to additional internal (MENLOPS) weights.
## Electroweak corrections
* Approximate NLO EW corrections are included in newer Sherpa setups as on-the-fly weights using the electroweak virtual approximation. 
The electroweak and QCD components can be combined using an additive or a multiplicative prescription 
(cf. section 6 of [arXiv:1412.5157](https://arxiv.org/abs/1412.5157)), the difference of the two being
an estimate of the uncertainty on the size of the approximate electroweak correction. 
The corresponding weights using the additive prescription will have a name pattern similar to `MUR1_MUF1_PDF303200_ASSEW` 
(`ASSEW` _clearly_ being short-hand for associated electroweak contribution), whereas the corresponding weights using the
multiplicative prescription will follow a pattern similar to `MUR1_MUF1_PDF303200_MULTIASSEW`.
* The weights correspond to the final cross-section, i.e. in practice they should be used to replace the nominal weight, 
similar to the usual scale and PDF variations (see also this
[twiki](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgSystematicUncertaintyRecipes#On_the_fly_systematic_variations)).
* As of Sherpa 2.2.11 a third approach, which exponentiates the electroweak correction, will be available as an additional on-the-fly weight
  * additive approach: `(B + VI_QCD + VI_EW) * PS = B * (1 + delta_QCD + delta_EW) * PS`
  * multiplicative approach: `(B + VI_QCD) * (1 + delta_EW) * PS = B * (1 + delta_QCD) * (1 + delta_EW) * PS`
  * exponentiated approach: `(B + VI_QCD) * exp(delta_EW) * PS = B * (1+delta_QCD) * exp(delta_EW) * PS`
* Note that for each approach additional weights might exist with `LO1` or `LO1LO2` attached: These are are the same as the EW virtual correction 
plus - if available - subleading Born corrections, which are formally part of the NNLO EW correction. 
Their effect is typically negligible compared to the dominant electroweak virtual correction 
but they might still be interesting for phenomenological studies. 
(These weights are added automatically. For processes where the corresponding libraries for these subleading Born terms don't exist yet, 
the weights will be identical to the virtual-only weights.)
* Note that the usual on-the-fly weights for scale and PDF variations will be with respect to the QCD-only nominal and that if
you choose a QCD+EW weight as the new nominal, you should strictly speaking evaluate the other generator systematics with respect
to the QCD-only nominal and transfer the relative uncertainty onto the new nominal.
## QCD scale variation weights
* As of Sherpa 2.2.1, variations of the renormalisation and factorisation scales in the matrix elements 
are provided as on-the-fly weights. The variations are encoded in the weight names e.g. as `MUR1_MUF1_PDF261000`
for the trivial (1,1) variation, `MUR2_MUF1_PDF261000` for a renormalisation scale up-variation by a factor 2
or `MUR1_MUF0.5_PDF261000` for a factorisation scale down-variation by a factor 2 for instance. 
The PDF is also specified via its [LHAPDF ID](https://lhapdf.hepforge.org/pdfsets.html).
* As of Sherpa 2.2.10, this has been extended to include coherent variations of the renormalisation and factorisation scales
in the parton shower. The weight names have been extended to encode this e.g. as 
`MUR2_MUF1_PDF261000_PSMUR2_PSMUF1` for a renormalisation scale up-variation by a factor 2 in both ME and PS or 
`MUR1_MUF0.5_PDF261000_PSMUR1_PSMUF0.5` for a factorisation scale down-variation by a factor 2 in both ME and PS, 
while the trivial (1,1) variation is still labelled as `MUR1_MUF1_PDF261000`.
* In case the reweighting of the parton-shower is found to be unstable, an additional set of multiweights is provided
where the parton-shower scales are _not_ reweighted, which is equivalent to the behaviour in the older Sherpa versions.
For technical reasons, these weights are simply prefixed with `ME_ONLY_` but the rest of the weight name is the same.
This means that e.g. `ME_ONLY_MUR2_MUF1_PDF261000_PSMUR2_PSMUF1` is an up-variation of the renormalisation scale *in the matrix element
only*, as indicated by the prefix and despite the `PSMUR2` in the remainder of the string.
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

## Sherpa-Pythia8 Hadronization interface

Sherpa allows to switch between their in-house cluster hadronisation model (used by default) and a Lund string model taken from Pythia8 (and Pythia6) to allow for systematic studies of the hadronisation uncertainties without changing the underlying perturbative physics.
How to enable and use the cluster model with standalone Sherpa can be seen in the ([Sherpa documentation](https://sherpa-team.gitlab.io/sherpa/master/manual/parameters/hadronization.html#id21)).
For use within athena a base fragment has been prepared that can be included directly. (Pythia8_Hadronization.py)
It is important to note that adjusting **ANY** other Pythia8 settings via the PYTHIA8 block overrides the settings in the base fragment.
So they have to be included again in ones own settings even if one wishes not to explicitely change their values.
Another caveat is that the interface does not work with every athena release. Instead the Pythia8 version that is in the release as to be the same version that Sherpa was compiled against.
Currently the only working release that fulfills this criteria is 21.6.82 with Sherpa version 3.0.0alpha1.py245 and Pythia 8.245.