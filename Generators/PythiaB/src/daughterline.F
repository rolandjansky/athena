c
c Funkciya vidayot nomer stroki docherney chastici if OK
c Inache ona vidayot 0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function niklenedote(NKF,LM)

c.......Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)      

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM,i,count
      integer niklenedote
 
      count=0
      niklenedote=0
      
      do i=k(LM,4),k(LM,5)
         if(k(i,2).eq.NKF) then       
            niklenedote=i
            count=count+1
         endif
      enddo
      
      if(count.EQ.0) niklenedote=0
      
c      print *,'     '
c      print *,' NKF =', NKF, '   LM =', LM
c      print *,' doteline =', niklenedote
      
      return
      end



c
c Funkciya vidayot pT docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzptdote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
      double precision zzzptdote

      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzptdote=-1000000.D0
      else
         zzzptdote=dsqrt(p(myline,1)**2+p(myline,2)**2)
      endif
      
      return
      end

c
c Funkciya vidayot p_x docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzpxdote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
      double precision zzzpxdote

      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzpxdote=-1000000.D0
      else
         zzzpxdote=p(myline,1)
      endif
      
      return
      end


c
c Funkciya vidayot p_y docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzpydote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
      double precision zzzpydote
      
      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzpydote=-1000000.D0
      else
         zzzpydote=p(myline,2)
      endif
      
      return
      end

c
c Funkciya vidayot p_z docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzpzdote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
      double precision zzzpzdot
      
      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzpzdote=-1000000.D0
      else
         zzzpzdote=p(myline,3)
      endif
      
      return
      end


c
c Funkciya vidayot E docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzedote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
      double precision zzzedote
      
      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzedote=-1000000.D0
      else
         zzzedote=p(myline,4)
      endif
      
      return
      end


c
c Funkciya vidayot pseudorapidity docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzprapdote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
      double precision theta, npz, npt
      double precision zzzprapdote
      
      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzprapdote=-1000000.D0
      else
         npz=zzzpzdote(NKF,LM)
	 npt=zzzptdote(NKF,LM)
         theta=DATAN(npt/npz)
         zzzprapdote=-DLOG(MAX(0.0001,DABS(DTAN(theta/2.D0))))
      endif
      
      print *,'     '
      print *,' doteline =', myline 
      print *,'       pz =', npz
      print *,'       pt =', npt
      print *,'      eta =', zzzprapdote 
      
      return
      end
      
c
c Funkciya vidayot ugol phi docherney chastici if OK
c Inache ona vidayot -1000000.D0
c
c NKF - KF kod iskomoy docherney chastici
c LM - nomer stroki materinskoy chastici
c
      function zzzphidote(NKF,LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N) 
      
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer NKF,LM, myline
c      double precision phi, npx, npy
      double precision npx, npy
      double precision zzzphidote
      
      myline=niklenedote(NKF,LM)
      if(myline.EQ.0) then
         zzzphidote=-1000000.D0
      else
         npx=zzzpxdote(NKF,LM)
	 npy=zzzpydote(NKF,LM)
         zzzphidote=DATAN(npy/npx)
      endif    
      
c      print *,'     '
c      print *,' doteline =', myline 
c      print *,'       px =', npx
c      print *,'       py =', npy
c      print *,'      phi =', zzzphidote      
            
      return
      end           


c
c Funkciya vidayot dlinu probega chastici if OK
c Inache ona vidayot -1000000.D0
c
c LM - nomer stroki chastici
c
c (x,y,z) in mm!!!!!
      function zzzLprobeg(LM)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer LM, line1, line2
      double precision zzzLprobeg
      
      line1=k(LM,4)
      line2=k(LM,5) 
      
      if(line1.EQ.0) then
         zzzLprobeg=-1000000.D0
         return
      endif
      
      if(line2.EQ.0) then
         zzzLprobeg=-1000000.D0
         return
      endif
      
      zzzLprobeg=DSQRT((V(line1,1)-V(LM,1))**2+
     #                 (V(line1,2)-V(LM,2))**2+
     #                 (V(line1,3)-V(LM,3))**2)
      	     
      return
      end
      
      
      
      
c
c Funkciya vidayot effektivnuyu massu dvuh chastic iz
c strok LM1 and LM2.
c
c Inache ona vidayot -1000000.D0
c
      function zzzmeff(LM1, LM2)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      integer LM1, LM2, line1, line2
      double precision px, py, pz, e
      double precision zzzmeff
      
      line1=LM1
      line2=LM2 
      
      if(line1.EQ.0) then
         zzzmeff=-1000000.D0
         return
      endif
      
      if(line2.EQ.0) then
         zzzmeff=-1000000.D0
         return
      endif
      
       e=P(line1,4)+P(line2,4)
      px=P(line1,1)+P(line2,1)
      py=P(line1,2)+P(line2,2)
      pz=P(line1,3)+P(line2,3)
      
      zzzmeff=DSQRT(e**2-px**2-py**2-pz**2)
      	     
      return
      end     
