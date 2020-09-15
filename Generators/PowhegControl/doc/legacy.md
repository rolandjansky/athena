# Legacy documentation

This is documentation of old instructions that are not applicable to ATLAS software releases.
It is unlikely that you will ever need any of this.

[[_TOC_]]

## Generating events in ATLAS releases < 21

These jobOptions should be run using `Generate_tf.py` in the usual way.
A sample Generate_tf command might look like this:

```bash
Generate_tf.py --ecmEnergy 13000 --runNumber 147505 --firstEvent 1 --jobConfig MC15.147505.PowhegDijet_Pythia8_AU2_CT10_muR1muF1.py --randomSeed 1 --outputEVNTFile Powheg.EVNT.root
```

for release 17, `Generate_trf.py` is used instead of `Generate_tf.py`
with slightly different syntax. The equivalent command to the above
would look like this:

```bash
Generate_trf.py ecmEnergy=8000 runNumber=147505 firstEvent=1 jobConfig=MC12.147505.PowhegDijet_Pythia8_AU2_CT10_muR1muF1.py randomSeed=1 evgenJobOpts=MC12JobOpts-00-08-63_v9.tar.gz outputEVNTFile=Powheg.EVNT.root
```

## Getting a specific tag (or the trunk) of PowhegControl from SVN

To use a specific tag of PowhegControl, you will need to check out the
code from SVN and compile it. The following example (in a **clean**
directory) will check

```bash
asetup 19.2.5.30,here # or whichever athena version you prefer
cmt co -r PowhegControl-00-03-00 Generators/PowhegControl # or use HEAD as the version to get the trunk
setupWorkArea.py
cd WorkArea/cmt
cmt br "cmt config; make"
```

If you need a specific version of External/Powheg (unlikely) then you
can check that out too.
