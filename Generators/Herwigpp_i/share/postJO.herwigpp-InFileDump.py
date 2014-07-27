## A post-JO fragment for enabling a standalone steering file to
## be written out from Athena Herwig++. Just specify this extra script
## on the athena/Generate_trf command line, e.g.
##   athena myhppjo.py Herwigpp_i/postJO.herwigpp-InFileDump.py
## or
##   Generate_trf.py ... postInclude=Herwigpp_i/postJO.herwigpp-InFileDump.py

## And this is all it does! Pure convenience/referenceability...
topAlg.Herwigpp.InFileDump = "myhpp.in"
