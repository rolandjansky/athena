#
# simple macro to trim a large histogram file looping over different folders
#
# ccuenca 25032011
#


INFILENAME=r0000178044_lEoR_ATLAS_MDA-HistogrammingHLT_HistogrammingHLT.root
INSTEM=/HistogrammingHLT/Top-LVL2-L2-Segment-1-Gatherer/EXPERT/
OUTFILENAME=test.root
OUTSTEM=/EXPERT/
DIR=T2CaloTau_Tau

for DIR in T2CaloTau_Tau T2IDTau_Tau T2TauFinal_Tau
do
   echo DIR = $DIR
   root -l -q 'updateHists.C("'$INFILENAME'","'$INSTEM$DIR'","'$OUTFILENAME'","'$OUTSTEM$DIR'")'
done

for BASE in T2CaloTauHypo_ T2IDTauHypo_ T2TauHypo_
do
   for ITEM in tauNoCut tau16i_medium tau16i_loose tau20i_medium1 tau29i_medium1 tau100_medium
   do
     echo DIR = $DIR
     DIR=$BASE$ITEM
     root -l -q 'updateHists.C("'$INFILENAME'","'$INSTEM$DIR'","'$OUTFILENAME'","'$OUTSTEM$DIR'")'
   done
done



INSTEM=/HistogrammingHLT/Top-EF-EBEF-Segment/EXPERT/

for DIR in TrigTauRecMerged_Tau
do
   echo DIR = $DIR
   root -l -q 'updateHists.C("'$INFILENAME'","'$INSTEM$DIR'","'$OUTFILENAME'","'$OUTSTEM$DIR'")'
done

for BASE in EFTauHypo_ EFTauInvHypo_
do
   for ITEM in tauNoCut tau16i_medium tau16i_loose tau20i_medium1 tau29i_medium1 tau100_medium
   do
     echo DIR = $DIR
     DIR=$BASE$ITEM
     root -l -q 'updateHists.C("'$INFILENAME'","'$INSTEM$DIR'","'$OUTFILENAME'","'$OUTSTEM$DIR'")'
   done
done



