Flavor Tagging Discriminants
============================

This package contains the relatively trivial code that atlas uses to
transform the raw outputs from vertex finding into flavor tagging
outputs. It is meant as "stand-alone" code: it should be usable in
both Athena and AnalysisBase.

This code is meant to run in all actively developed ATLAS projects. This means that the following features should _not_ be used:

   - Read / Write Handles: These are not supported in 21.2, and thus
     won't work in analysis code. Revisit in mid 2021.

   - Any C++ features beyond C++11: Upgrade physics will use
     [LCG_88][lcg88] for the foreseeable future. This release ships
     with GCC 6.2, which predates C++17. Revisit in 2024.

[lcg88]: http://lcginfo.cern.ch/release/88/

Package Overview
----------------

There are several user-level tools here:

   - `BTagJetAugmenter`: adds jet-wise flavor-tagging inputs.

   - `BTagAugmenterTool`: ASG Tool interface around `BTagJetAugmenter`.

   - `BTagMuonAugmenter`: Class for adding muon information for the
     soft muon tagger.

   - `BTagMuonAugmenterTool`: ASG Tool wrapper around
     `BTagMuonAugmenter`.

   - `BTagTrackAugmenter`: add track-wise inputs. These include signed
      impact parameters and track grades. Note that the signed impact
      parameters **should not be added** in derivations: they are
      specific to one jet and if tracks are used in multiple jet
      collections they will be invalid!

   - `DL2`: low-level implementation of the DL2 tagger. Allows
     lower-level manipulation, for example reading in the
     configuration from a stream rather than a file.

   - `DL2HighLevel`: higher-level implementation of the DL2
     tagger. Takes only one argument to construct: the path to the
     saved NN file. This code uses a fair number of hardcoded regular
     expressions to determine properties of the inputs. It also uses
     `PathResolver` to find the input file.

   - `DL2HighLevelTools`: Utilities for `DL2HighLevel`. The intention
     is to keep the files included in the `DL2.h` header minimal.

   - `DL2Tool`: ASG Tool interface around `DL2HighLevel`.

   - `FlipTagEnums`: List of enums to keep track of the tag flip
     configuration. These change some selection and inputs in in
     DL2. Specifically:

       - `NEGATIVE_IP_ONLY`: Save only the negative IP tracks and flip
         the sign of z0 and d0. Also use the standard Flip/Neg version
         of the other taggers.

       - `FLIP_SIGN`: Save all IP, flip sign of z0 and d0. Use
         Flip/Neg versions of other taggers (as above).


### Hbb Tagging ###

Some components of Hbb tagging also live here. These include:

   - `HbbTag`: Similar to DL2, but for large-R jets. Uses high-level
     flavor tagging inputs. It also uses `HbbGraphConfig` and
     `HbbConstants`.

   - `HbbTagTool`: ASG Tool interface around `HbbTag`.

   - `VRJetOverlapDecorator`: Adds decorations which quantify the
     degree of overlap between VR subjets. The validity of flavor
     tagging calibrations depends on this overlap being small. Two
     numbers in particular are added:

      - `relativeDeltaRToVRJet`: smallest value of `dR / jet_radius`,
        considering this jet paired with every other jet in the
        event. Here `dR` is the angular separation between the two
        jets, while `jet_radius` is the smallest the of the two jet
        radii.

      - `deltaRToVRJet`: corresponds to the `dR` to whatever jet has
        the smallest _relative_ delta R. This isn't necessarily the
        smallest delta R.

     A smaller value in either of these corresponds to a larger
     overlap. In general we recommend removing any jet where
     `relativeDeltaRToVRJet < 1.0`.

   - `VRJetOverlapDecoratorTool`: ASG interface around the class
     above.

   - `HbbSelector`: Class to retrieve the Hbb discriminant as a single
     variable. Provides a method `getHbbScore(const xAOD::Jet&)` which
     returns a double.


### Other Files ###

There are also several tools that you _probably_ don't have to touch:

 - `customGetter`: DL2 relies on some information that isn't stored in
   accessors that we can get with a string (i.e. `pt`, `eta`,
   ...). These are defined in `customGetter`.



Inputs and Outputs from DL2
===========================

The neural networks we use in DL2 are saved as JSON files in the
[ATLAS groupdata area][gd]. They follow the naming scheme

```
[dev/]BTagging/<timestamp>/<tagger>/<jet-collection>/<tagger-specific-name>.json
```

where the `dev/` is for taggers which are in development (i.e. should
not be run in a release).

Format of the NN files
----------------------

Being JSON, these files should be quite easy to inspect. The most
useful fields to inspect are:

   - `inputs`: The list of simple (see below) input nodes for the
     NN. Each node has a `name` and a list of `variables`. The name
     for a node specifying b-tagging inputs should be
     `b_tagging`. Each variable also includes three fields: a `name`,
     an `offset` and a `scale`. These names should either match a
     `customGetter` or refer to something in the EDM. The inputs will
     be transformed according to `(x + offset) * scale` before being
     fed to the NN.
   - `input_sequences`: These are a type of non-simple inputs, in this
     case a variable length array of tracks. The node name is parsed
     according to rules in `DL2HighLevel.cxx` to determine the sort
     order and track selection. The rest of the information is
     structured identically to the simple inputs.
   - `outputs`: This object specifies the naming convention for DL2
     outputs. Each object has a key corresponding to a tagger name, a
     list of `labels`, and a `node_index`. The `node_index` is
     internal to the underlying neural network. The outputs are
     decorated to the b-tagging object, with names like
     `<key>_<label>` and one decoration per label. In other words, an
     entry like

        ```
        "nn": { "labels": ["pb", "pu"], ...}
        ```

     will produce two decorations on the b-tagging object: `nn_pb` and
     `nn_pu`. All outputs are of type `float`.


Inspecting files with jq
------------------------

I would recommend taking a look at [`jq`][j] to parse these files,
since they can get quite large.

For example, to print the list of outputs for a file, find it in the
groupdata area and run:

```
curl path/to/model.json | jq '.outputs'
```

to get the list of input sequence variables, you can use something
like

```
curl path/to/model.json | jq '.input_sequences[].variables[].name'
```

[gd]: https://atlas-groupdata.web.cern.ch/atlas-groupdata/
[j]: https://stedolan.github.io/jq/manual/
