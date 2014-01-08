!
!> Demonstration of
!> the use of common for tracks segments
!> \author J.F.Laporte
!
 SUBROUTINE MUDEMO01
 USE M_MB_Digis
 USE M_MB_Reco_Segments
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER :: IBINTR, NUIN, NUAN
 INTEGER :: Ibid1
 REAL(8) :: ProdSca,ProdScaARRAY(500)
 REAL(8) :: ProdScaPara,ProdScaParaARRAY(500)
 REAL(8) :: ProdScaAlig,ProdScaAligARRAY(500)
 REAL(8) :: ProdScaAlSg,ProdScaAlSgARRAY(500)
!
   PRINT 6050,NBINTR
   IF( NBINTR /= 0 ) THEN
     DO IBINTR=1,NBINTR
       do Ibid1=1,NUINTR(IBINTR)+NAINTR(IBINTR)
         ProdSca = &
           XDdINTR(Ibid1,IBINTR) * VEINTR(1,IBINTR) &
         + YDdINTR(Ibid1,IBINTR) * VEINTR(2,IBINTR) &
         + ZDdINTR(Ibid1,IBINTR) * VEINTR(3,IBINTR) 
         ProdSca = ProdSca/ sqrt( &
           XDdINTR(Ibid1,IBINTR)**2 &
         + YDdINTR(Ibid1,IBINTR)**2 &
         + ZDdINTR(Ibid1,IBINTR)**2 )
         ProdSca = ProdSca/ sqrt( &
             VEINTR(1,IBINTR)**2 &
         +   VEINTR(2,IBINTR)**2 &
         +   VEINTR(3,IBINTR)**2 )
         if (Ibid1.le.500) ProdScaARRAY(Ibid1)=ProdSca
         if ( Ibid1 .le. NUINTR(IBINTR) ) then
           ProdScaPara = &
             XDdINTR(Ibid1,IBINTR) * XDdINTR(1,IBINTR) &
           + YDdINTR(Ibid1,IBINTR) * YDdINTR(1,IBINTR) &
           + ZDdINTR(Ibid1,IBINTR) * ZDdINTR(1,IBINTR) 
           ProdScaPara = ProdScaPara/ sqrt( &
             XDdINTR(Ibid1,IBINTR)**2 &
           + YDdINTR(Ibid1,IBINTR)**2 &
           + ZDdINTR(Ibid1,IBINTR)**2 )
           ProdScaPara = ProdScaPara/ sqrt( &
             XDdINTR(1,IBINTR)**2 &
           + YDdINTR(1,IBINTR)**2 &
           + ZDdINTR(1,IBINTR)**2 )
           if (Ibid1.le.500) ProdScaParaARRAY(Ibid1)=ProdScaPara
           ProdScaAlig = &
             XDdINTR(Ibid1,IBINTR) * XDdINTR(1,IBINTR) &
           + YDdINTR(Ibid1,IBINTR) * YDdINTR(1,IBINTR) &
           + ZDdINTR(Ibid1,IBINTR) * ZDdINTR(1,IBINTR) 
           ProdScaAlig = ProdScaAlig/ sqrt( &
             XDdINTR(Ibid1,IBINTR)**2 &
           + YDdINTR(Ibid1,IBINTR)**2 &
           + ZDdINTR(Ibid1,IBINTR)**2 )
           ProdScaAlig = ProdScaAlig/ sqrt( &
             XDdINTR(1,IBINTR)**2 &
           + YDdINTR(1,IBINTR)**2 &
           + ZDdINTR(1,IBINTR)**2 )
           if (Ibid1.le.500) ProdScaAligARRAY(Ibid1)=ProdScaAlig
           ProdScaAlSg = &
             ( PTINTR(1,IBINTR) - XDINTR(Ibid1,IBINTR) ) * XDdINTR(Ibid1,IBINTR) &
           + ( PTINTR(2,IBINTR) - YDINTR(Ibid1,IBINTR) ) * YDdINTR(Ibid1,IBINTR) &
           + ( PTINTR(3,IBINTR) - ZDINTR(Ibid1,IBINTR) ) * ZDdINTR(Ibid1,IBINTR) 
           ProdScaAlSg = ProdScaAlSg/ sqrt( &
             XDdINTR(Ibid1,IBINTR)**2 &
           + YDdINTR(Ibid1,IBINTR)**2 &
           + ZDdINTR(Ibid1,IBINTR)**2 )
           ProdScaAlSg = ProdScaAlSg/ sqrt( &
             ( PTINTR(1,IBINTR) - XDINTR(Ibid1,IBINTR) )**2 &
           + ( PTINTR(2,IBINTR) - YDINTR(Ibid1,IBINTR) )**2 &
           + ( PTINTR(3,IBINTR) - ZDINTR(Ibid1,IBINTR) )**2 )
           ProdScaAlSg = sqrt (abs(1.-ProdScaAlSg**2))
           ProdScaAlSg = ProdScaAlSg* sqrt( &
             ( PTINTR(1,IBINTR) - XDINTR(Ibid1,IBINTR) )**2 &
           + ( PTINTR(2,IBINTR) - YDINTR(Ibid1,IBINTR) )**2 &
           + ( PTINTR(3,IBINTR) - ZDINTR(Ibid1,IBINTR) )**2 )
           ProdScaAlSg = ProdScaAlSg*10000.d0
           if (Ibid1.le.500) ProdScaAlSgARRAY(Ibid1)=ProdScaAlSg
         endif
       enddo
       NUIN = NUINTR(IBINTR)
       PRINT 6052,IBINTR
       PRINT 6054,CARISC(ISCINTR(IBINTR))
       PRINT 6056,CHINTR(IBINTR)
       PRINT 6057,T0INTR(IBINTR)
       PRINT 6058,IKINTR(IBINTR)
       PRINT 6060,PTINTR(1:3,IBINTR)
       PRINT 6062,VEINTR(1:3,IBINTR)
       PRINT 6097,IERINTR(IBINTR)
       IF( IERINTR(IBINTR) /= 0 ) THEN
         PRINT 6094,1.d0/dsqrt(ERINTR(1,IBINTR))*10.0d0, &
                    1.d0/dsqrt(ERINTR(2,IBINTR))*1000.0d0 , &
                    ERINTR(3,IBINTR)/(dsqrt(ERINTR(1,IBINTR))*dsqrt(ERINTR(2,IBINTR)))
         PRINT 6096,1.d0/dsqrt(ERINTR(4,IBINTR))*10000.0d0, &
                    1.d0/dsqrt(ERINTR(5,IBINTR))*1000000.0d0 , &
                    ERINTR(6,IBINTR)/(dsqrt(ERINTR(4,IBINTR))*dsqrt(ERINTR(5,IBINTR)))
       ENDIF
       PRINT 6063,NUIN
       PRINT 6064,       IUINTR(1:NUIN,IBINTR)
       PRINT 6070,ITUBHI(IUINTR(1:NUIN,IBINTR))
       PRINT 6065,JTUBHI(IUINTR(1:NUIN,IBINTR))
       PRINT 6066,RdigINTR(1:NUIN,IBINTR)
       PRINT 6067,RtraINTR(1:NUIN,IBINTR)
       PRINT 6068,RtraINTR(1:NUIN,IBINTR)-RdigINTR(1:NUIN,IBINTR)
       PRINT 7000,ProdScaARRAY(1:NUIN)
       PRINT 7002,ProdScaParaARRAY(1:NUIN)
       PRINT 7004,ProdScaAligARRAY(1:NUIN)
       PRINT 7006,ProdScaAlSgARRAY(1:NUIN)
       WRITE(*,'("     Nb. of holes              :",I5)') NHINTR(IBINTR)
       NUAN = NAINTR(IBINTR)
       PRINT 6080,NUAN
       PRINT 6081,       IUINTR(NUIN+1:NUIN+NUAN,IBINTR)
       PRINT 6082,IBANHI(IUINTR(NUIN+1:NUIN+NUAN,IBINTR))
       PRINT 6083,JBANHI(IUINTR(NUIN+1:NUIN+NUAN,IBINTR))
       PRINT 7000,ProdScaARRAY(NUIN+1:NUIN+NUAN)
     ENDDO
   ENDIF
!
6050 FORMAT(/' Content of Module  M_MB_Reco_Segments  : ',/,' There are ',   &
              I5,' Track segments defined by a Point and a Vector :')
6052 FORMAT(' **> Segment track nb.         :',I5)
6054 FORMAT('     In station                :  ',A3)
6056 FORMAT('     Quality factor            :',F12.5)
6057 FORMAT('     Fitted T0                 :',F12.5)
6058 FORMAT('     ID s of multilayers       :',I7)
6060 FORMAT('     Coordinates of the Point  :',3F12.5)
6062 FORMAT('     Coordinates of the Vector :',3F12.5)
6097 FORMAT('     error validity            :',I7)
6094 FORMAT('     error mat (mm/mrad)       :',3F12.5)
6096 FORMAT('               (micron/micrad) :',3F12.5)
6063 FORMAT('     Nb. of tube hits used     :',I5)
6064 FORMAT('     Tube hits numbers         :',12I12)
6070 FORMAT('     Tube hits identifiers     :',12I12)
6065 FORMAT('     Tube hits flags           :',12I12)
6066 FORMAT('     Tube hits radii           :',12F12.4)
6067 FORMAT('     Segment impact parameters :',12F12.4)
6068 FORMAT('     Residuals                 :',12F12.4)
6080 FORMAT('     Nb. of strip hits used    :',I5)
6081 FORMAT('     Strip hits numbers        :',12I12)
6082 FORMAT('     Strip hits identifiers    :',12I12)
6083 FORMAT('     Strip hits flags          :',12I12)
7000 FORMAT('     ProdSca                   :',12F12.8)
7002 FORMAT('     ProdScaPara               :',12F12.8)
7004 FORMAT('     ProdScaAlig               :',12F12.8)
7006 FORMAT('     ProdScaAlSg               :',12F12.1)
!
 END SUBROUTINE MUDEMO01
!
