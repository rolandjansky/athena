!
!> \author M.Virchaux
!
 SUBROUTINE REDUCS2(Ibeg,Iend,Rmesu0,Rmesu1)
 USE M_MB_Control
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER :: Ibeg, Iend
 REAL(8) :: Rmesu0, Rmesu1
 INTEGER :: I, Ishift
 REAL(8) :: X0TOT, T2TOT, B2TOT, TBTOT, DRmesu
 REAL(8) :: YI, Delta, Selta, Y1def, Y2def, Y1, Y2, Frac1, Frac2
 REAL(8) :: T01, T02, XI1, XI2, TI1, TI2, RY1, RY2, RYM, AY1, AY2, BY1, BY2
!
   IF( Iend-Ibeg < 2 ) RETURN
!
#if !defined(ATLAS_NOPRINTMU)
   IF(MBPri >= 3) PRINT 7770,Ibeg,Iend,Rmesu0,Rmesu1,RRMASC(Ibeg),RRMASC(Iend)
7770  FORMAT(' In REDUCS2 : Ibeg,Iend,Rmesu0-1,RRMASC(.) =',2I5,4F12.4)
#endif
   DRmesu = Rmesu1 - Rmesu0
!
!>> Find the resulting <Theta**2>, <Beta**2> and <Theta*Beta>
!>> (and also the total amount of X0's)
   X0TOT = 0.D0
   T2TOT = 0.D0  ! A
   B2TOT = 0.D0  ! B
   TBTOT = 0.D0  ! C
   DO I=Ibeg,Iend
     TI2 = X0MASC(I) * ( 1.D0 + 0.038D0*LOG(X0MASC(I)) )**2
     YI  = (Rmesu1-RRMASC(I)) / DRmesu
     X0TOT = X0TOT + X0MASC(I)
     T2TOT = T2TOT +         TI2
     B2TOT = B2TOT + YI**2 * TI2
     TBTOT = TBTOT +   YI  * TI2
   ENDDO
!
!>> Find the locations (Y1 and Y2) of  2  m.s. radii simulating the
!>> (Iend-Ibeg+1) ones above and, possibly, change the repartition of
!>> the X0's from the default one (50%,50%) when Y1 or Y2 is outside
!>> the range [0,1]  (i.e. RY1 or RY2 outside the range [Rmesu0,Rmesu1])
   Delta = T2TOT*B2TOT - TBTOT**2
   Selta = SQRT( Delta )
   Y1def = ( TBTOT + Selta ) / T2TOT
   Y2def = ( TBTOT - Selta ) / T2TOT
   IF( Y1def > 1.D0 ) THEN
     Y1 = 0.99D0
     Frac1 = 1.D0 / ( 1.D0 + (T2TOT*Y1-TBTOT)**2/Delta )
   ELSEIF( Y2def < 0.D0 ) THEN
     Y2 = 0.01D0
     Frac1 = 1.D0 / ( 1.D0 + Delta/(TBTOT-T2TOT*Y2)**2 )
   ELSE
     Frac1 = 0.5D0
   ENDIF
   Frac2 = 1.D0 - Frac1
   Y1 = ( TBTOT + SQRT(Frac2/Frac1)*Selta ) / T2TOT
   Y2 = ( TBTOT - SQRT(Frac1/Frac2)*Selta ) / T2TOT
#if !defined(ATLAS_NOPRINTMU)
   IF(MBPri >= 3) PRINT 7771,Y1def,Y2def,Frac1,Frac2,Y1,Y2
7771  FORMAT('            : Y1def,Y2def,Frac1,Frac2,Y1,Y2 =',6F12.6)
#endif
!
!>> find the amount of X0's coresponding to mean scat. angles = (Frac1 and Frac2)*<Theta**2>
   T01 = Frac1 * T2TOT
   T02 = Frac2 * T2TOT
#if !defined(ATLAS_NOPRINTMU)
   IF(MBPri >= 3) PRINT 7772,T01,T02
7772   FORMAT('            :           T01,T02 =',35X,2F15.9)
#endif
   XI1 = Frac1 * X0TOT
   XI2 = Frac2 * X0TOT
   TI1 = T01
   TI2 = T02
   DO I=1,50
     XI1 = T01 - TI1 + XI1
     XI2 = T02 - TI2 + XI2
     TI1 = XI1 * ( 1.D0 + 0.038D0*LOG(XI1) )**2
     TI2 = XI2 * ( 1.D0 + 0.038D0*LOG(XI2) )**2
#if !defined(ATLAS_NOPRINTMU)
     IF(MBPri >= 3) PRINT 7773,I,XI1,XI2,TI1,TI2
7773 FORMAT('            : i,XI1,XI2,TI1,TI2 =',I5,4F15.9)
#endif
     IF( ABS(TI1-T01) < 0.001D0 .AND. ABS(TI2-T02) < 0.001D0 ) EXIT
   ENDDO
!
!>> Reduce the (Iend-Ibeg+1) radii of multiple scattering and energy losses
!>> to  two  !!!
   RY1 = Rmesu1 - DRmesu * Y1
   RY2 = Rmesu1 - DRmesu * Y2
   RYM = (RY1+RY2) / 2.D0
   AY1 = 1.D0
   BY1 = 0.D0
   AY2 = 1.D0
   BY2 = 0.D0
   DO I=Ibeg,Iend
     IF( RRMASC(I) < RYM ) THEN
       BY1 = ALMASC(I)*BY1 + BLMASC(I)
       AY1 = ALMASC(I)*AY1
     ELSE
       BY2 = ALMASC(I)*BY2 + BLMASC(I)
       AY2 = ALMASC(I)*AY2
     ENDIF
   ENDDO
   RRMASC(Ibeg  ) = RY1
   X0MASC(Ibeg  ) = XI1
   ALMASC(Ibeg  ) = AY1
   BLMASC(Ibeg  ) = BY1
   RRMASC(Ibeg+1) = RY2
   X0MASC(Ibeg+1) = XI2
   ALMASC(Ibeg+1) = AY2
   BLMASC(Ibeg+1) = BY2
   Ishift = Iend-Ibeg-1
   DO I=Iend+1,NBMASC
     RRMASC(I-Ishift) = RRMASC(I)
     X0MASC(I-Ishift) = X0MASC(I)
     ALMASC(I-Ishift) = ALMASC(I)
     BLMASC(I-Ishift) = BLMASC(I)
   ENDDO
   NBMASC = NBMASC - Ishift
#if !defined(ATLAS_NOPRINTMU)
   IF( MBPri >= 3 ) PRINT 7774,RY1,XI1,AY1,BY1,RY2,XI2,AY2,BY2
7774  FORMAT('       =====> RY1,XI1,AY1,BY1 =',F12.4,3F12.6,   &
            /'       =====> RY2,XI2,AY2,BY2 =',F12.4,3F12.6)
#endif
!
 END SUBROUTINE REDUCS2
!
