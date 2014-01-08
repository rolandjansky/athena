!
 SUBROUTINE MUDEMO05
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 CHARACTER(4) :: Atec, Acoo, Asta
 REAL(8)      :: XYZ1(3), XYZ2(3), AssociatedReso, norm
 REAL(8)      :: VecNorm(3)
 INTEGER      :: Ican, Ihit, It, IMdtCsc,I1rst2nd, IC,ISC, Is, IRpcTgc
!
   DO Ican=1,NBCAN
     IF( CHICAN(Ican) > 999.D0 )  CYCLE
     PRINT 1000,Ican
     DO Ihit=1,NZXCAN(Ican)
       It = IZXCAN(Ihit,Ican)
       Call GetTubeEnds( It, IMdtCsc,I1rst2nd,XYZ1,XYZ2,AssociatedReso )
       Atec = ''
       IF( IMdtCsc ==  1 ) Atec = 'Mdt '
       IF( IMdtCsc == -1 ) Atec = 'Csc '
       Acoo = ''
       IF( I1rst2nd ==  1 ) Acoo = '1rst'
       IF( I1rst2nd == -1 ) Acoo = '2nd '
       IC  = KTUBHI(It)/10000
       ISC = (IC+1)/2
       IF(IC == 2*ISC) THEN
         Asta = 'o'//CARISC(ISC)
       ELSE
         Asta = 'i'//CARISC(ISC)
       ENDIF
       XYZ1(1) = XDCCAN(Ihit,Ican)
       XYZ1(2) = YDCCAN(Ihit,Ican)
       XYZ1(3) = ZDCCAN(Ihit,Ican)
       XYZ2(1) = XDCdCAN(Ihit,Ican)
       XYZ2(2) = YDCdCAN(Ihit,Ican)
       XYZ2(3) = ZDCdCAN(Ihit,Ican)
       norm = 1.d0 / SQRT( XYZ2(1)**2 + XYZ2(2)**2 + XYZ2(3)**2 )
       XYZ2(1:3) = norm * XYZ2(1:3)
       PRINT 1100,Ihit,It,Atec,Acoo,Asta,XYZ1(1:3),XYZ2(1:3)
     ENDDO
     DO Ihit=1,NSXCAN(Ican)
       Is = ISXCAN(Ihit,Ican)
       call GetStripEnds( &
        IS, IRpcTgc,I1rst2nd, &
        XYZ1,XYZ2,AssociatedReso , &
        VecNorm &
       )
       Atec = ''
       IF( IRpcTgc ==  1 ) Atec = 'Rpc '
       IF( IRpcTgc == -1 ) Atec = 'Tgc '
       Acoo = ''
       IF( I1rst2nd ==  1 ) Acoo = '1rst'
       IF( I1rst2nd == -1 ) Acoo = '2nd '
       Asta = ''
       XYZ1(1) = XSDCCAN(Ihit,Ican)
       XYZ1(2) = YSDCCAN(Ihit,Ican)
       XYZ1(3) = ZSDCCAN(Ihit,Ican)
       XYZ2(1) = XSDCdCAN(Ihit,Ican)
       XYZ2(2) = YSDCdCAN(Ihit,Ican)
       XYZ2(3) = ZSDCdCAN(Ihit,Ican)
       norm = 1.d0 / SQRT( XYZ2(1)**2 + XYZ2(2)**2 + XYZ2(3)**2 )
       XYZ2(1:3) = norm * XYZ2(1:3)
       PRINT 1200,Ihit,Is,Atec,Acoo,Asta,XYZ1(1:3),XYZ2(1:3)
     ENDDO
   ENDDO
!
1000  FORMAT(/' MUDEMO05 for Ican =',I5)
1100  FORMAT(' Ihit,It,XYZ1-2 =',I3,I5,3(2X,A4),2(F16.4,2F11.4))
1200  FORMAT(' Ihit,Is,XYZ1-2 =',I3,I5,3(2X,A4),2(F16.4,2F11.4))
!
 END SUBROUTINE MUDEMO05
!
