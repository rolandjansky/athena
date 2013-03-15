      implicit none

c index ordering is always z,r,phi

      integer nzmax,nrmax,nfmax,nz,nr,nf,i,j,k,n,
     +  mmz,mmr,mrf,izp,irp,ifp
      real twopi,bz,br,bf,zgp,rgp,fgp,zsinv,rsinv,fsinv

      parameter ( nzmax=300, nrmax=25, nfmax=64 )
      parameter ( mmz=6000, mmr=1200 ,mrf=6300 )

      common/bsol /zgp(2,nzmax),rgp(2,nrmax),fgp(2,nfmax),
     +             zsinv(2,nzmax),rsinv(2,nrmax),fsinv(2,nfmax),twopi,
     +             bz(2,nzmax,nrmax,nfmax),
     +             br(2,nzmax,nrmax,nfmax),
     +             bf(2,nzmax,nrmax,nfmax),
     +             izp(2,mmz),irp(2,mmr),ifp(2,mrf),nz(2),nr(2),nf(2)

      save
