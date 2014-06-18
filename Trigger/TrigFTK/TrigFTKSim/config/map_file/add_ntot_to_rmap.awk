#!/usr/bin/awk -f

BEGIN {
  # initialize arrays

  # barrel
  maxmodphi[0]=22;
  maxmodphi[1]=38;
  maxmodphi[2]=52;
  maxmodphi[3]=32;
  maxmodphi[4]=32;
  maxmodphi[5]=40;
  maxmodphi[6]=40;
  maxmodphi[7]=48;
  maxmodphi[8]=48;
  maxmodphi[9]=56;
  maxmodphi[10]=56;

  # endcap pix
  maxmodphi[11]=48;
  maxmodphi[12]=48;
  maxmodphi[13]=48;
  # endcap sct
  maxmodphi[14]=52;
  maxmodphi[15]=52;
  maxmodphi[16]=40;
  maxmodphi[17]=40;
  maxmodphi[18]=52;
  maxmodphi[19]=52;
  maxmodphi[20]=40;
  maxmodphi[21]=40;
  maxmodphi[22]=52;
  maxmodphi[23]=52;
  maxmodphi[24]=40;
  maxmodphi[25]=40;
  maxmodphi[26]=52;
  maxmodphi[27]=52;
  maxmodphi[28]=40;
  maxmodphi[29]=40;
  maxmodphi[30]=52;
  maxmodphi[31]=52;
  maxmodphi[32]=40;
  maxmodphi[33]=40;
  maxmodphi[34]=52;
  maxmodphi[35]=52;
  maxmodphi[36]=40;
  maxmodphi[37]=40;
  maxmodphi[38]=52;
  maxmodphi[39]=52;
  maxmodphi[40]=40;
  maxmodphi[41]=40;
  maxmodphi[42]=52;
  maxmodphi[43]=52;
  maxmodphi[44]=40;
  maxmodphi[45]=40;
  maxmodphi[46]=52;
  maxmodphi[47]=52;
  
  # barrel
  maxmodeta[0]=13;
  maxmodeta[1]=13;
  maxmodeta[2]=13;
  maxmodeta[3]=13;
  maxmodeta[4]=13;
  maxmodeta[5]=13;
  maxmodeta[6]=13;
  maxmodeta[7]=13;
  maxmodeta[8]=13;
  maxmodeta[9]=13;
  maxmodeta[10]=13;
  # endcap pix
  maxmodeta[11]=1;
  maxmodeta[12]=1;
  maxmodeta[13]=1;
  # endcap SCT
  maxmodeta[14]=1;
  maxmodeta[15]=1;
  maxmodeta[16]=1;
  maxmodeta[17]=1;
  maxmodeta[18]=1;
  maxmodeta[19]=1;
  maxmodeta[20]=2;
  maxmodeta[21]=2;
  maxmodeta[22]=1;
  maxmodeta[23]=1;
  maxmodeta[24]=2;
  maxmodeta[25]=2;
  maxmodeta[26]=1;
  maxmodeta[27]=1;
  maxmodeta[28]=2;
  maxmodeta[29]=2;
  maxmodeta[30]=1;
  maxmodeta[31]=1;
  maxmodeta[32]=2;
  maxmodeta[33]=2;
  maxmodeta[34]=1;
  maxmodeta[35]=1;
  maxmodeta[36]=2;
  maxmodeta[37]=2;
  maxmodeta[38]=1;
  maxmodeta[39]=1;
  maxmodeta[40]=1;
  maxmodeta[41]=1;
  maxmodeta[42]=1;
  maxmodeta[43]=1;
  maxmodeta[44]=1;
  maxmodeta[45]=1;
  maxmodeta[46]=1;
  maxmodeta[47]=1;

  started=0;
}

NF==1 {
  started = 1;
  icur=0;
}

started==1 && NF>1 {
  #1 0     0       19      21      0       6
  printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",$1,$2,$3,$4,$5,maxmodphi[icur] ,$6,$7,maxmodeta[icur]);
  icur++;
}

(started==0 || NF<=1) {
  print $0;
}
