*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      INTEGER FUNCTION JTROUV(TT,NDEB,NFIN,AA)
*
*>> Input  : TT    = ordered real array
*>>          AA    = one real number
*>> Output : JTROUV , such as TT(JTROUV-1) < AA < TT(JTROUV)
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 24/09/97
*
      IMPLICIT NONE
*
      INTEGER NDEB, NFIN
      REAL    TT(*), AA
      INTEGER ILO,IME,IHI
*
      ILO = NDEB
      IME = (NDEB+NFIN) / 2
      IHI = NFIN
10    IF( AA.LT.TT(IME) ) THEN
        IF( IME-ILO.LE.1 ) THEN
          JTROUV = IME
          RETURN
        ELSE
          IHI = IME
        ENDIF
      ELSE
        IF( IHI-IME.LE.1 ) THEN
          JTROUV = IHI
          RETURN
        ELSE
          ILO = IME
        ENDIF
      ENDIF
      IME = (IHI+ILO) / 2
      GO TO 10
*
      END
