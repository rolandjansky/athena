      SUBROUTINE PYTAUD(ITAU,IORIG,KFORIG,NDECAY)

C...  created: 28.04.2009 by vadym.zhuravlov@cern.ch
C     decay of tau to 3 muons, uniform distribution
C     in phase space (mandelstam s,t,u)
      
C...  Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYCOMP
C...  Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYJETS/,/PYDAT1/

      LOGICAL PASSED
      INTEGER COUNTER
      DOUBLE PRECISION A(2)
      DOUBLE PRECISION E(3), PMU(3), PX(3), PY(3), PZ(3)
      DIMENSION DP(4)

      
      TAUMASS = PMAS(PYCOMP(15),1) 
      EMUMASS = PMAS(PYCOMP(13),1)
      
C      PRINT *, "TAUMASS = ", TAUMASS, "MUMASS = ", EMUMASS

C     fill NDECAY, K, mass, px and py     
      NDECAY = 3
      DO I=N+1, N+NDECAY
         P(I, 5) = EMUMASS
      ENDDO

      IF (K(ITAU, 2).GT. 0) THEN 
         K(N+1, 2) = 13
         K(N+2, 2) = 13
         K(N+3, 2) = -13
      ELSE
         K(N+1, 2) = -13
         K(N+2, 2) = -13
         K(N+3, 2) = 13
      ENDIF
      
      TAUMASS2 = TAUMASS*TAUMASS
      EMUMASS2 = EMUMASS*EMUMASS
      
C     generate two random numbers 
      COUNTER = 0
      PASSED = .FALSE.
      DO WHILE (.NOT. PASSED) 
         A(1) = PYR(0)
         A(2) = PYR(0)
C     sort numbers
         IF (A(1)>A(2)) THEN
            TMP = A(1)
            A(1) = A(2)
            A(2) = TMP
         ENDIF
         
C         PRINT *, 'GENERATED NUMBERS: ', A(1), A(2)
         
C     calculated Mandelstam variables
         S = (3*EMUMASS2+TAUMASS2)*A(1)
         T = (3*EMUMASS2+TAUMASS2)*(A(2)-A(1))
         U = (3*EMUMASS2+TAUMASS2)*(1-A(2))
         
C         PRINT *, 'MANDELSTAM VARIABLES: ', S, T, U 
         
C     calculate muon energy
         E(1) = (TAUMASS2+EMUMASS2-S)/(2*TAUMASS)
         E(2) = (TAUMASS2+EMUMASS2-T)/(2*TAUMASS)
         E(3) = (TAUMASS2+EMUMASS2-U)/(2*TAUMASS)
         
C         PRINT *, 'TOTAL MUON ENERGY BEFORE BUST: ', E(1)+E(2)+E(3)
         
         IF (E(1).LT.EMUMASS.OR.E(2).LT.EMUMASS.OR.E(3).LT.EMUMASS) THEN 
            PASSED = .FALSE.
         ELSE 
            PASSED = .TRUE.
         ENDIF
         
         DO I=1,3
            PMU(I)=SQRT(E(I)**2-EMUMASS2)
         ENDDO

C         PRINT *, "MUON MOMENTA: ", PMU
         
         IF (PASSED
     &        .AND.PMU(1)+PMU(2).GT.PMU(3)
     &        .AND.PMU(3)+PMU(1).GT.PMU(2) 
     &        .AND.PMU(2)+PMU(3).GT.PMU(1)) THEN 
            PASSED = .TRUE.
         ELSE 
            PASSED = .FALSE.
         ENDIF
      
         COUNTER = COUNTER+1
         IF (COUNTER.GT.1000) THEN 
            PRINT *, "ERROR IN PYTAUD: FAILED TO GENERATE MUON MOMENTA"
            RETURN
         ENDIF
      ENDDO
      
      PX(1) =  - PMU(1)
      PX(2) = (PMU(1)**2+PMU(2)**2-PMU(3)**2)/(2*PMU(1))
      PX(3) = (PMU(1)**2-PMU(2)**2+PMU(3)**2)/(2*PMU(1))
      
      PY(1) = 0.0
      PY(2) = -SQRT(PMU(2)**2-PX(2)**2)
      PY(3) =  SQRT(PMU(3)**2-PX(3)**2)
    
      PZ(1) =0.0
      PZ(2) =0.0
      PZ(3) =0.0

C      PRINT *, "BEFORE rotation 3MU INV.MASS = ", 
C     &     SQRT((E(1)+E(2)+E(3))**2 
C     &     - (PX(1)+PX(2)+PX(3))**2
C     &     - (PY(1)+PY(2)+PY(3))**2
C     &     - (PZ(1)+PZ(2)+PZ(3))**2)      

      
C      PRINT *, "Before rotation in decay plane: sum px = ", 
C     &     PX(1)+PX(2)+PX(3),
C     &     " sum py = ", PX(1)+PX(2)+PX(3),
C     &     " inv. masses: "
C      DO I=1,3
C         PRINT *, SQRT(E(I)**2-PX(I)**2-PY(I)**2)
C      ENDDO
      
      CALL ROTATE(PX, PY)

C      PRINT *, "After rotation in decay plane: sum px = ", 
C     &     PX(1)+PX(2)+PX(3),
C     &     "sum py = ", PX(1)+PX(2)+PX(3),
C     &     "inv. masses: "
C      DO I=1,3
C         PRINT *, SQRT(E(I)**2-PX(I)**2-PY(I)**2)
C      ENDDO
    
C      PRINT *, "BEFORE BOOST 3MU INV.MASS = ", 
C     &     SQRT((E(1)+E(2)+E(3))**2 
C     &     - (PX(1)+PX(2)+PX(3))**2
C     &     - (PY(1)+PY(2)+PY(3))**2
C     &     - (PZ(1)+PZ(2)+PZ(3))**2)      

C     boost to tau rest frame
      BEX = P(ITAU,1)/P(ITAU,4)
      BEY = P(ITAU,2)/P(ITAU,4)
      BEZ = P(ITAU,3)/P(ITAU,4)
      
      IF(BEX**2+BEY**2+BEZ**2.GT.1D-20) THEN
         DBX=BEX
         DBY=BEY
         DBZ=BEZ
         DB=SQRT(DBX**2+DBY**2+DBZ**2)
         DGA=1D0/SQRT(1D0-DB**2)
C     LOOP OVER 3 MUONS
         DO 160 I=1,3
            DP(1) = PX(I)
            DP(2) = PY(I)
            DP(3) = 0.0
            DP(4) = E(I)
            
            DBP=DBX*DP(1)+DBY*DP(2)+DBZ*DP(3)
            DGABP=DGA*(DGA*DBP/(1D0+DGA)+DP(4))
            PX(I)=DP(1)+DGABP*DBX
            PY(I)=DP(2)+DGABP*DBY
            PZ(I)=DP(3)+DGABP*DBZ
            E(I)=DGA*(DP(4)+DBP)
 160     CONTINUE
      ENDIF
C     STORE MOMENTA
C      PRINT *, " PX = ", PX
C      PRINT *, " PY = ", PY
C      PRINT *, " PZ = ", PZ
C      PRINT *, " 3MU INV.MASS = ", 
C     &     SQRT((E(1)+E(2)+E(3))**2 
C     &     - (PX(1)+PX(2)+PX(3))**2
C     &     - (PY(1)+PY(2)+PY(3))**2
C     &     - (PZ(1)+PZ(2)+PZ(3))**2)      
      
      DO I = 1, 3
         P(N+I, 1) = PX(I)
         P(N+I, 2) = PY(I)
         P(N+I, 3) = PZ(I)
         P(N+I, 4) = E(I)
         P(N+I, 5) = EMUMASS
      ENDDO
      
      RETURN
      END

      SUBROUTINE ROTATE(PX, PY)
c     USED BY PYTAUD      
c     ROTATION OF THE TAU DECAY PRODUCTS BY ARBITRARY ANGLE IN DECAY PLANE
C...  Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      
      DATA PI/3.141592654D0/
      DIMENSION PX(3), PY(3)
      PHI = PI*PYR(0)

      DO I=1,3
         TMP = PX(I)
         PX(I) = SIN(PHI)*TMP+COS(PHI)*PY(I)
         PY(I) = COS(PHI)*TMP-SIN(PHI)*PY(I)
      ENDDO

      RETURN
      END
