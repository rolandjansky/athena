!
 LOGICAL FUNCTION IOKSSS(IC,LS,IGEOM, ZZtrue,TTtrue,SStrue,VZtrue,VTtrue,VSTtrue)
 USE M_MB_Digis
 USE M_MB_SSBAN
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IC, LS, IGEOM
 REAL(8), INTENT(IN) :: ZZtrue, TTtrue, SStrue, VZtrue, VTtrue, VSTtrue
 INTEGER :: L, IS, ICODE, ICT, IP, Itrans, IRPC
 INTEGER :: JTYP, JFF, JZZ, JOB, JSPLI, JSL, JSZ, JSTRI, Ier
 REAL(8) :: VStrue, COSF, SINF, AXX, AYY, AZZ, AVX, AVY, AVZ
 REAL(8) :: BSS, BZZ, BTT, BVS, BVZ, BVT
 REAL(8) :: SS, ZZ, TT, WZ, WT, VZT, VST
 REAL(8) :: Z, S, T, TOLZ, TOLS
 REAL(8) :: SZT(3,4)
!
! return .true. if the segment determined with the S position of a given strip
! is indeed in the active region of that strip
!
   VStrue  = VSTtrue * VTtrue
   COSF = COSCHAV(IGEOM)
   SINF = SINCHAV(IGEOM)
   CALL MuboSZTtoXYZ( (IC<=NCOUBA), SINF,COSF, SStrue,ZZtrue,TTtrue, AXX,AYY,AZZ)
   CALL MuboSZTtoXYZ( (IC<=NCOUBA), SINF,COSF, VStrue,VZtrue,VTtrue, AVX,AVY,AVZ)
   IOKSSS = .false.
   DO L=1,NSTRBA
     IS = ISBAN(L,LS)
     IF( IS <= 0 )  EXIT
     ICODE = KBANHI(IS) / 10
     ICT   = ICODE / 100
     IP    = ICODE - ICT*100
     IRPC  = IRPTGC(ICT)
! intersection of segment with layer in local (ideal) frame
     COSF  = COSTRI(ICT)
     SINF  = SINTRI(ICT)
     CALL MuboXYZtoSZT( (ICT<=NTRIBA), SINF,COSF, AXX,AYY,AZZ, BSS,BZZ,BTT)
     CALL MuboXYZtoSZT( (ICT<=NTRIBA), SINF,COSF, AVX,AVY,AVZ, BVS,BVZ,BVT)
     Itrans = ItransTRI(ICT)     
     CALL SZTWU_Ideal_from_True(Itrans, BSS,BZZ,BTT,BVS,BVZ,BVT, SS,ZZ,TT,WZ,WT,VZT,VST)
     T = TCETRI(ICT) + SIGN( 1.0d0, TCETRI(ICT) ) * TBAND(Irpc,Ip)
     Z = ZZ + (T-TT)*VZT
     S = SS + (T-TT)*VST
     TOLZ = 0.2d0 + 0.01d0*ABS(T-TT)
     TOLS = 1.0d0
! geometry of the strip 
     CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB)
     JSZ   = 1
     IF( IP > N1BAND(IRPC) ) JSZ = 2
     JSL   = IP - (JSZ-1)*N1BAND(IRPC)
     JSTRI = ABS(LBANHI(IS)) / 10
     JSPLI = MOD( ABS(LBANHI(IS)) , 10 )
     IF( ICT <= NTRIBA ) THEN
       CALL FGetIdealStripPositionFromRPCindex(JTYP,JFF,JZZ,JOB,JSPLI,JSL,JSZ,JSTRI,SZT,Ier)
     ELSE
       CALL FGetIdealChannelPositionFromTGCind(JTYP,JFF,JZZ,JOB,JSPLI,JSL,JSZ,JSTRI,SZT,Ier)
     ENDIF
     SZT(:,:) = 0.1d0 * SZT(:,:)  ! Amdc unit is mm
!
! S strips have SZT(4) positionned as    4 3   ^ Z axis          or    3 4
!                                        1 2   |  -> S axis            2 1
!
     IF( Z < ( SZT(2,2)*(S-SZT(1,1)) + SZT(2,1)*(SZT(1,2)-S) ) / (SZT(1,2)-SZT(1,1)) - TOLZ )   CYCLE
     IF( Z > ( SZT(2,4)*(S-SZT(1,3)) + SZT(2,3)*(SZT(1,4)-S) ) / (SZT(1,4)-SZT(1,3)) + TOLZ )   CYCLE
     IF( SZT(1,2) > SZT(1,1) ) THEN
       IF( S < ( SZT(1,4)*(Z-SZT(2,1)) + SZT(1,1)*(SZT(2,4)-Z) ) / (SZT(2,4)-SZT(2,1)) - TOLS ) CYCLE
       IF( S > ( SZT(1,3)*(Z-SZT(2,2)) + SZT(1,2)*(SZT(2,3)-Z) ) / (SZT(2,3)-SZT(2,2)) + TOLS ) CYCLE
     ELSE
       IF( S > ( SZT(1,4)*(Z-SZT(2,1)) + SZT(1,1)*(SZT(2,4)-Z) ) / (SZT(2,4)-SZT(2,1)) + TOLS ) CYCLE
       IF( S < ( SZT(1,3)*(Z-SZT(2,2)) + SZT(1,2)*(SZT(2,3)-Z) ) / (SZT(2,3)-SZT(2,2)) - TOLS ) CYCLE
     ENDIF
     IOKSSS = .true.
     EXIT
   ENDDO
!
 END FUNCTION IOKSSS
!
