Below is an example SS file with comments.

Version 9
! PIX: 3mm SS, so we use 328/(20mm/3mm)=328/6.6=50
! SCT: 5mm SS, so we use 768/(60mm/5mm)=768/12=64

! description of the columns
! pix=1, sct=0
! barrel=0 endcap=1
! layer number
! SS size along r-phi
! number of r-phi pixels/strips in a module (constant)
! SS size along eta (pixel only)
! number of eta pixels in a module (constant)

!these are 3 pixel layers in the barrel
1 0 0 15 328 36 144
1 0 1 15 328 36 144
1 0 2 15 328 36 144

!these are 8 SCT layers in the barrel (4 stereo layers)
0 0 0 16 768
0 0 1 16 768
0 0 2 16 768
0 0 3 16 768
0 0 4 16 768
0 0 5 16 768
0 0 6 16 768
0 0 7 16 768

!these are 3 pixel endcap disks
1 1 0 15 328 36 144
1 1 1 15 328 36 144
1 1 2 15 328 36 144

!these are the SCT disks
0 1 0 16 768
0 1 1 16 768
0 1 2 16 768
0 1 3 16 768
0 1 4 16 768
0 1 5 16 768
0 1 6 16 768
0 1 7 16 768
0 1 8 16 768
0 1 9 16 768
0 1 10 16 768
0 1 11 16 768
0 1 12 16 768
0 1 13 16 768
0 1 14 16 768
0 1 15 16 768
0 1 16 16 768
0 1 17 16 768
0 1 18 16 768
0 1 19 16 768
0 1 20 16 768
0 1 21 16 768
0 1 22 16 768
0 1 23 16 768
0 1 24 16 768
0 1 25 16 768
0 1 26 16 768
0 1 27 16 768
0 1 28 16 768
0 1 29 16 768
0 1 30 16 768
0 1 31 16 768
0 1 32 16 768
0 1 33 16 768
