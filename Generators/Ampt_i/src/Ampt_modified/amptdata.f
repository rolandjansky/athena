      BLOCK DATA AMPTDATA

c from amptsub.f
      PARAMETER (AMU= 0.9383,nxymax=10001)
      double precision dpcoal,drcoal,ecritl
      INTEGER ZTA, ZPR

      common  /gg/      dx,dy,dz,dpx,dpy,dpz
      save gg
      common  /zz/      zta,zpr
      SAVE /zz/
      COMMON  /RUN/     NUM
      SAVE /RUN/
      common/input1/ MASSPR,MASSTA,ISEED,IAVOID,DT
      SAVE /input1/
      COMMON /INPUT2/ ILAB, MANYB, NTMAX, ICOLL, INSYS, IPOT, MODE,
     &   IMOMEN, NFREQ, ICFLOW, ICRHO, ICOU, KPOTEN, KMUL
      SAVE /INPUT2/
      COMMON /INPUT3/ PLAB, ELAB, ZEROPT, B0, BI, BM, DENCUT, CYCBOX
      SAVE /INPUT3/
      common /imulst/ iperts
      SAVE /imulst/
      common /coal/dpcoal,drcoal,ecritl
      save coal
      common/anim/nevent,isoft,isflag,izpc
      save anim
      common /para7/ ioscar,nsmbbbar,nsmmeson
      save para
      common/embed/iembed,nsembd,pxqembd,pyqembd,xembd,yembd,
     1     psembd,tmaxembd,phidecomp
      save embed
      common/xyembed/nxyjet,xyjet(nxymax,2)
      save xyembed

c from art1f.f
      PARAMETER     (MAXSTR=150001,MAXR=1,
     1               AKA=0.498,etaM=0.5475)
      PARAMETER     (MAXX   =   20,  MAXZ  =    24)
      PARAMETER     (ISUM   =   1001,  IGAM  =    1100)
      parameter     (MX=4,MY=4,MZ=8,MPX=4,MPY=4,mpz=10,mpzp=10)

      INTEGER   OUTPAR
      COMMON  /AA/      R(3,MAXSTR)
      SAVE /AA/
      COMMON  /BB/      P(3,MAXSTR)
      SAVE /BB/
      COMMON  /CC/      E(MAXSTR)
      SAVE /CC/
      COMMON  /DD/      RHO(-MAXX:MAXX,-MAXX:MAXX,-MAXZ:MAXZ),
     &                     RHOP(-MAXX:MAXX,-MAXX:MAXX,-MAXZ:MAXZ),
     &                     RHON(-MAXX:MAXX,-MAXX:MAXX,-MAXZ:MAXZ)
      SAVE /DD/
      COMMON  /EE/      ID(MAXSTR),LB(MAXSTR)
      SAVE /EE/
      COMMON  /HH/  PROPER(MAXSTR)
      SAVE /HH/
      common  /ff/f(-mx:mx,-my:my,-mz:mz,-mpx:mpx,-mpy:mpy,-mpz:mpzp)
      SAVE /ff/
      COMMON  /INPUT/ NSTAR,NDIRCT,DIR
      SAVE /INPUT/
      COMMON  /PP/      PRHO(-20:20,-24:24)
      save /pp/
      COMMON  /QQ/      PHRHO(-MAXZ:MAXZ,-24:24)
      save /qq/
      COMMON  /RR/      MASSR(0:MAXR)
      SAVE /RR/
      common  /ss/      inout(20)
      SAVE /ss/
      COMMON  /KKK/     TKAON(7),EKAON(7,0:2000)
      SAVE /KKK/
      COMMON  /KAON/    AK(3,50,36),SPECK(50,36,7),MF
      SAVE /KAON/
      COMMON/TABLE/ xarray(0:1000),earray(0:1000)
      SAVE /TABLE/
      COMMON  /DDpi/    piRHO(-MAXX:MAXX,-MAXX:MAXX,-MAXZ:MAXZ)
      SAVE /DDpi/
      common  /tt/  PEL(-maxx:maxx,-maxx:maxx,-maxz:maxz)
     &,rxy(-maxx:maxx,-maxx:maxx,-maxz:maxz)
      SAVE /tt/
      COMMON /ARPRNT/ ARPAR1(100), IAPAR2(50), ARINT1(100), IAINT2(50)
      SAVE /ARPRNT/
      COMMON /ARERCP/PRO1(MAXSTR, MAXR)
      SAVE /ARERCP/
      COMMON /ARERC1/MULTI1(MAXR)
      SAVE /ARERC1/
      COMMON /ARPRC1/ITYP1(MAXSTR, MAXR),
     &     GX1(MAXSTR, MAXR), GY1(MAXSTR, MAXR), GZ1(MAXSTR, MAXR),
     &     FT1(MAXSTR, MAXR),
     &     PX1(MAXSTR, MAXR), PY1(MAXSTR, MAXR), PZ1(MAXSTR, MAXR),
     &     EE1(MAXSTR, MAXR), XM1(MAXSTR, MAXR)
      SAVE /ARPRC1/
      common/resdcy/NSAV,iksdcy
      SAVE /resdcy/
      COMMON/RNDF77/NSEED
      SAVE /RNDF77/

c from linana.f

      COMMON/hbt/lblast(MAXSTR),xlast(4,MAXSTR),plast(4,MAXSTR),nlast
      SAVE /hbt/
      common /lastt/itimeh,bimp
      SAVE /lastt/
      COMMON/tdecay/tfdcy(MAXSTR),tfdpi(MAXSTR,MAXR),tft(MAXSTR)
      SAVE /tdecay/
      COMMON /AREVT/ IAEVT, IARUN, MISS
      SAVE /AREVT/
      common/snn/efrm,npart1,npart2,epsiPz,epsiPt,PZPROJ,PZTARG
      SAVE /snn/
      COMMON/HJGLBR/NELT,NINTHJ,NELP,NINP
      SAVE /HJGLBR/
      COMMON/FTMAX/ftsv(MAXSTR),ftsvt(MAXSTR, MAXR)
      save /FTMAX/
      COMMON /dpert/dpertt(MAXSTR,MAXR),dpertp(MAXSTR),dplast(MAXSTR),
     1     dpdcy(MAXSTR),dpdpi(MAXSTR,MAXR),dpt(MAXSTR, MAXR),
     2     dpp1(MAXSTR,MAXR),dppion(MAXSTR,MAXR)
      save /dpert/
clin-12/14/03:
      COMMON/HPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
      save /HPARNT/
      common /para8/ idpert,npertd,idxsec
      save /para8/
clin-2/2012:
      common /phiHJ/iphirp,phiRP
      SAVE   /phiHJ/


      END

