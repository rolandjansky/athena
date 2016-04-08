C     Protos common block for ProtosLHEF_i
C     Patricia Fernandez (Jan 2014)

      INTEGER lun,IHRD
      COMMON /PRdata/ lun,IHRD
      REAL*8 PTbmax,IMATCH
      COMMON /PRmatch/ PTbmax,IMATCH
      INTEGER ISIGNWT
      COMMON /PRinfo/ ISIGNWT

      INTEGER i,ii,in
      COMMON /iter/ in
      CHARACTER(100) string(20)
      COMMON /CHAR/  string

      REAL*8 mtau,mb,mt
      REAL*8 MW,MZ,MH
      REAL*8 mL,mQ,MWp,MZp
      COMMON /MPAR1/ mtau,mb,mt,mL,mQ
      COMMON /MPAR2/ MW,MZ,MH,MWp,MZp

      REAL*8 ET,Pz,mf
      COMMON /EPAR/ ET,Pz,mf

      REAL*8 Gt,GE,GN
      REAL*8 mX,GX
      REAL*8 mD,GD
      REAL*8 GQ,Vmix
      COMMON /PPAR/ Gt,GE,GN,mX,GX,mD,GD,GQ,Vmix

      INTEGER idum1,idum2
      COMMON /DPAR/ idum1,idum2