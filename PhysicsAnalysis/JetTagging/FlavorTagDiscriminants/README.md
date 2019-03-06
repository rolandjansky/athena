Flavor Tagging Discriminants
============================

This package contains the relatively trivial code that atlas uses to
transform the raw outputs from vertex finding into flavor tagging
outputs. It is meant as "stand-alone" code: it should be usable in
both Athena and AnalysisBase.

Package Overview
----------------

There are several user-level tools here:

   - `BTagJetAugmenter`: adds jet-wise flavor-tagging inputs.

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
     expressions to determine properties of the inputs.

   - `DL2Tool`: ASG Tool interface around `DL2HighLevel`. Uses
     PathResolver to find the input calibration file.

### Other Files ###

There are also several tools that you _probably_ don't have to touch:

 - `customGetter`: DL2 relies on some information that isn't stored in
   accessors that we can get with a string (i.e. `pt`, `eta`,
   ...). These are defined in `customGetter`.
