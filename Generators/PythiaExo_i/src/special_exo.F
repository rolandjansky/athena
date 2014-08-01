C____________________________________________________________________________________________________________________________
C AUTHOR LMijovic
C (place-holder for) subroutine(s) for invoking/screaning special options for processes resisting in Pythia_Exo package;
C idea: the modification some process introduces should not necessarily be picked by each process residing in the package
C the special subroutines can be used in order to enable picking the modifications a-la-carte, from jO.
C____________________________________________________________________________________________________________________________

C----------------------------------------------------------------------------------------------------------------------------
C IMODE 1:
C In Pysgex.F:
C - for W',Z',qq/gg -> G* [ISUB 141,142,391,392]:
C -- take out Breit-Wigner dependence +
C -- suppress low mass events from parton luminosities
C -- Pysgex.F modif. introduced by Oliver Stelzer-Chilton & Daniel Hayden, Exotics group
C----------------------------------------------------------------------------------------------------------------------------
C IMODE 2:
C In pyofsh.F and pyscat.F:
C - for  qqbar (gg) -> QQbar [ISUB 81 (82)]:     
C -- pyofsh.F : ISUB 81,82: set top partner to 3000006, spectrum to be filled via SLHA
C -- pyscat.F : account for qqbar (gg) -> QQbar vs qqbar (gg) -> QbarQ 
C -- modif. adopted from PythiaTop_i by Vikash Chavda, top group
C----------------------------------------------------------------------------------------------------------------------------
      SUBROUTINE SPECIX(IMODE)

      IMPLICIT INTEGER(I-N)

      COMMON/SPSGEX/ISSP
      COMMON/SPTMDF/ISTMDF
      SAVE /SPSGEX/,/SPTMDF/

      IF (IMODE .EQ. 1) THEN
         ISSP=1
      ELSE
         ISSP=0
      ENDIF

      IF (IMODE .EQ. 2) THEN
         ISTMDF=1
      ELSE 
         ISTMDF=0
      ENDIF
      
      RETURN
      
      END
C-----------------------------------------------------------------------------------------------------------------------------
