c global variables
      integer maxNumSubProc
      parameter( maxNumSubProc = 1000 )

c then the local clone of HEPEUP
      integer LMAXNUP
      parameter ( LMAXNUP = 20 )
      integer LNUP, LIDPRUP, LISTUP, LIDUP, LMOTHUP, LICOLUP, LSPINUP
      integer colorprocess, decayprocess, numEntry
      logical isFromCP
      common /localHEPEUP/ LNUP(maxNumSubProc), LIDPRUP(maxNumSubProc), 
     &               LISTUP(LMAXNUP,maxNumSubProc), 
     &               LIDUP(LMAXNUP,maxNumSubProc), 
     &               LMOTHUP(2,LMAXNUP,maxNumSubProc),
     &               LSPINUP(LMAXNUP,maxNumSubProc),
     &               LICOLUP(2,LMAXNUP,maxNumSubProc),
     &               colorprocess,decayprocess,
     &               numEntry(maxNumSubProc), isFromCP(maxNumSubProc) 


c and local variables which are needed to fill the run common block HEPRUP
      double precision sigtot, sdev, totalweight
      integer numcolor,numdecay
      common /localHEPRUP/ sigtot, sdev, totalweight,
     &                     numcolor, numdecay




