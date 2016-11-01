Which jO files to use?
-----------------------

1) The file with the suffix _AutoConfig is more up to date because it uses the AutoConfig feature. 
PLEASE use this one

2) However, VrtSecInclusive_jobOptions_Rel15.py works and was used upto 15.5.2 and in 15.6.1. Also 
worked in 15.6.8.7. Had to use it because some files didn't have the correct AutoConfig data.

### Added 2016-07-30 by Christian Ohm and Jordi Duarte Campderros

VrtSecInclusive_DV_postInclude.py to be used with Reco_tf.py on output from large-radius tracking
for finding displaced vertices from new long-lived particles (SUSY and Exotics WGs). Can be used in 
jobs with either DRAW_RPVLL and DESD_RPVLL input transparently (only e2d for RAW jobs though).