__device__ void correctPhi(float& a) {

  if(a>(float)M_PI) 
    a=a-(float)M_PI-(float)M_PI; 
  else {
    if(a<-(float)M_PI) 
      a=a+(float)M_PI+(float)M_PI;
  }
}

__device__ void correctTheta(float& a) {
  if(a<0)  a+=(float)M_PI;
  if(a>(float)M_PI) a-=(float)M_PI;
}


__device__ int translateIndex(int i, int j) {
  return (j<i) ? j+i*(i+1)/2 : i+j*(j+1)/2;
}

__device__ void applyMaterialEffects(EXTENDED_TRACK_STATE_TYPE_2& ets, float& radLength, float g[3][3], float* gV, bool shift) {

  float sigmaMS,ms2,invSin,sinThetaLoc,rl,lV[2];

  lV[0]=g[0][0]*gV[0]+g[0][1]*gV[1]+g[0][2]*gV[2];
  lV[1]=g[1][0]*gV[0]+g[1][1]*gV[1]+g[1][2]*gV[2];

  sinThetaLoc=sqrt(lV[0]*lV[0]+lV[1]*lV[1]);
  rl=radLength/fabs(g[2][0]*gV[0]+g[2][1]*gV[1]+g[2][2]*gV[2]);
  sigmaMS=13.6f*fabs(0.001f*ets.m_par[4])*sqrt(rl)*(1+0.038f*log(rl));
  ms2=sigmaMS*sigmaMS;invSin=1/sinThetaLoc;
  
  if(!shift) {
    ets.m_cov[5]+=ms2*invSin*invSin;
    ets.m_cov[9]+=ms2;
    ets.m_cov[8]+=ms2*invSin;
  }
  else {
    ets.m_cov[35]+=ms2*invSin*invSin;
    ets.m_cov[44]+=ms2;
    ets.m_cov[43]+=ms2*invSin;
  }
}

__device__ void invertM(float m[3][3], float g[3][3]) {

  g[0][0]= m[1][1]*m[2][2]-m[1][2]*m[2][1];
  g[1][0]=-m[1][0]*m[2][2]+m[1][2]*m[2][0];
  g[2][0]= m[1][0]*m[2][1]-m[1][1]*m[2][0];
  g[0][1]=-m[0][1]*m[2][2]+m[0][2]*m[2][1];
  g[1][1]= m[0][0]*m[2][2]-m[0][2]*m[2][0];
  g[2][1]=-m[0][0]*m[2][1]+m[0][1]*m[2][0];
  g[0][2]= m[0][1]*m[1][2]-m[0][2]*m[1][1];
  g[1][2]=-m[0][0]*m[1][2]+m[0][2]*m[1][0];
  g[2][2]= m[0][0]*m[1][1]-m[0][1]*m[1][0];

}
__device__ void updateTrackState(EXTENDED_TRACK_STATE_TYPE_2& ets, int& hitType, float2& meas, float4& measCov,
				 float& chi2, int& ndof) {
  if(hitType==1) {//pixel
    float B[10][2], K[10][2];
    float resid[2];

    int base=0;

    for(int ii=0;ii<10;ii++) {
      base+=ii;
      B[ii][0]=ets.m_cov[base];
      B[ii][1]=ets.m_cov[base+1];
    }

    resid[0]=meas.x-ets.m_par[0];resid[1]=meas.y-ets.m_par[1];
    float W[2][2], D[2][2];
    float det;
    W[0][0]=ets.m_cov[0]+measCov.x;
    W[1][0]=W[0][1]=ets.m_cov[1]+measCov.y;
    W[1][1]=ets.m_cov[2]+measCov.w;
    det=W[0][0]*W[1][1]-W[0][1]*W[0][1];det=1/det;
    D[0][0]=W[1][1]*det;D[0][1]=-W[0][1]*det;
    D[1][0]=D[0][1];D[1][1]=W[0][0]*det;
    chi2+=resid[0]*resid[0]*D[0][0]+2*resid[0]*resid[1]*D[0][1]+
      resid[1]*resid[1]*D[1][1];
    for(int ii=0;ii<10;ii++)
      for(int jj=0;jj<2;jj++)
	K[ii][jj]=B[ii][0]*D[0][jj]+B[ii][1]*D[1][jj];
    for(int ii=0;ii<10;ii++)
      ets.m_par[ii]=ets.m_par[ii]+K[ii][0]*resid[0]+K[ii][1]*resid[1];

    for(int jj=0;jj<10;jj++) {
      int base = jj*(jj+1)/2;
      for(int ii=0;ii<=jj;ii++) {
	ets.m_cov[ii+base]+=-K[ii][0]*B[jj][0]-K[ii][1]*B[jj][1];
      }
    }
    ndof+=2;
  }
  if(hitType==2) {// SCT barrel
    float B[10], K[10];
    float D, resid;
    int base=0;
    for(int ii=0;ii<10;ii++) {
      base+=ii;B[ii]=ets.m_cov[base];
    }
    D=1/(ets.m_cov[0]+measCov.x);
    resid=meas.x-ets.m_par[0];
    chi2+=resid*resid*D;
    for(int ii=0;ii<10;ii++) K[ii]=D*B[ii];
    for(int ii=0;ii<10;ii++) 
      ets.m_par[ii]=ets.m_par[ii]+K[ii]*resid;
    for(int jj=0;jj<10;jj++) {
      base = jj*(jj+1)/2;
      for(int ii=0;ii<=jj;ii++) {
	ets.m_cov[ii+base]+=-K[ii]*B[jj];
      }
    }
    ndof+=1;
  }
  if(hitType==3) {//endcap SCT
    //doUpdate=false;
    float corr;
    float H[2],B[10],K[10];
    corr=1/(ets.m_par[1]+meas.y);
    H[0]=meas.y*corr;
    H[1]=-ets.m_par[0]*meas.y*corr*corr;

    int base=0;

    for(int ii=0;ii<10;ii++) {
      base+=ii;B[ii]=H[0]*ets.m_cov[base]+H[1]*ets.m_cov[base+1];
    }

    corr=1+ets.m_par[1]/meas.y;
    float D=1/(B[0]*H[0]+B[1]*H[1]+measCov.x*corr*corr);
    float resid=meas.x-ets.m_par[0]*meas.y/(ets.m_par[1]+meas.y);
    chi2+=resid*resid*D;
    for(int ii=0;ii<10;ii++) K[ii]=D*B[ii];
    for(int ii=0;ii<10;ii++) 
      ets.m_par[ii]=ets.m_par[ii]+K[ii]*resid;
    for(int jj=0;jj<10;jj++) {
      base = jj*(jj+1)/2;
      for(int ii=0;ii<=jj;ii++) {
	ets.m_cov[ii+base]+=-K[ii]*B[jj];
      }
    }
    ndof+=1;
  }
  correctPhi(ets.m_par[2]);
  correctPhi(ets.m_par[7]);
  correctTheta(ets.m_par[3]);
  correctTheta(ets.m_par[8]);
}

__global__ void KalmanFilterGPU(INPUT_TRACK_INFO_TYPE* d_In, 
				OUTPUT_TRACK_INFO_TYPE *d_Out, 
				DETECTOR_SURFACE_TYPE* d_Geo,
				HIT_INFO_TYPE* d_Hit,
				int N){

  __const__ float C=0.00002999975;
  __const__ float minStep=30.0;
  __const__ float B0z = 20.84;

  __shared__ JACOBIAN_TYPE JArr[BLOCK_SIZE];
  __shared__  EXTENDED_TRACK_STATE_TYPE_2 etsArray[BLOCK_SIZE];

 // __shared__ float Jm[7][7];

  int idx = blockDim.x * blockIdx.x + threadIdx.x;
  if (idx < N) {
    
    float chi2=0.0;
    int ndof=-5;
    int status=1;

    //EXTENDED_TRACK_STATE_TYPE_2 ets;
    //JACOBIAN_TYPE J;

    EXTENDED_TRACK_STATE_TYPE_2& ets = etsArray[threadIdx.x];
    JACOBIAN_TYPE& J = JArr[threadIdx.x];

    //

    float4 ip1 = (*d_In).m_parSet1[idx];

    //1. read first surface

    float m[3][3], gV[3], gP[3];
    float c[3],gB[3],sint,
      sinf,cost,cosf,a,b,cc,sl,CQ;
    float JG[5][7];
    float Jm[7][7];
    float J0[7][5];
    int hitType;
    float2 meas;
    float4 measCov;

    int geoIdx = idx;
    c[0]=(*d_Geo).m_rowX[geoIdx].w;c[1]=(*d_Geo).m_rowY[geoIdx].w;c[2]=(*d_Geo).m_rowZ[geoIdx].w;
    m[0][0]=(*d_Geo).m_rowX[geoIdx].x;m[0][1]=(*d_Geo).m_rowX[geoIdx].y;m[0][2]=(*d_Geo).m_rowX[geoIdx].z;
    m[1][0]=(*d_Geo).m_rowY[geoIdx].x;m[1][1]=(*d_Geo).m_rowY[geoIdx].y;m[1][2]=(*d_Geo).m_rowY[geoIdx].z;
    m[2][0]=(*d_Geo).m_rowZ[geoIdx].x;m[2][1]=(*d_Geo).m_rowZ[geoIdx].y;m[2][2]=(*d_Geo).m_rowZ[geoIdx].z;
    float g[3][3];
    invertM(m,g);
		
    ets.m_par[0]=ets.m_par[5]=ip1.x;
    ets.m_par[1]=ets.m_par[6]=ip1.y;
    ets.m_par[2]=ets.m_par[7]=ip1.z;
    ets.m_par[3]=ets.m_par[8]=ip1.w;
    ip1=(*d_In).m_parSet2[idx];
    ets.m_par[4]=ets.m_par[9]=ip1.x*1000.0f;

    memset(&ets.m_cov[0],0,sizeof(ets.m_cov));
    ets.m_cov[0]=ets.m_cov[20]=ets.m_cov[15]=4.0f;
    ets.m_cov[2]=ets.m_cov[27]=ets.m_cov[22]=20.0f;
    ets.m_cov[5]=ets.m_cov[35]=ets.m_cov[30]=0.01f;
    ets.m_cov[9]=ets.m_cov[44]=ets.m_cov[39]=0.01f;
    ets.m_cov[14]=ets.m_cov[54]=ets.m_cov[49]=1.0f;

   
    
    hitType = (*d_Hit).m_hitType[geoIdx];
    meas = (*d_Hit).m_meas[geoIdx];
    measCov = (*d_Hit).m_cov[geoIdx];

    ip1=(*d_Geo).m_parSet[geoIdx];

    for(int iter = 1;iter<=(*d_In).m_nHits[idx];iter++) {

      //A1. update using previous (iter-1) hit

      updateTrackState(ets,hitType,meas,measCov,chi2,ndof);

      if(iter==(*d_In).m_nHits[idx]) break;

      //A2. local->global, J0
      
      sint=sin(ets.m_par[3]);
      cosf=cos(ets.m_par[2]);
      sinf=sin(ets.m_par[2]);
      cost=cos(ets.m_par[3]);

      gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*ets.m_par[4];
      memset(&J0[0][0],0,sizeof(J0));

      gP[0]=c[0]+m[0][0]*ets.m_par[0]+m[0][1]*ets.m_par[1];
      gP[1]=c[1]+m[1][0]*ets.m_par[0]+m[1][1]*ets.m_par[1];
      gP[2]=c[2]+m[2][0]*ets.m_par[0]+m[2][1]*ets.m_par[1];

      J0[0][0]=m[0][0];J0[0][1]=m[0][1];
      J0[1][0]=m[1][0];J0[1][1]=m[1][1];
      J0[2][0]=m[2][0];J0[2][1]=m[2][1];
      J0[3][2]=-sinf*sint;J0[3][3]=cosf*cost;
      J0[4][2]= cosf*sint;J0[4][3]=sinf*cost;
      J0[5][3]=-sint;J0[6][4]=1;		

      //A3. read next surface

      geoIdx = iter*N+idx;
      c[0]=(*d_Geo).m_rowX[geoIdx].w;c[1]=(*d_Geo).m_rowY[geoIdx].w;c[2]=(*d_Geo).m_rowZ[geoIdx].w;

      m[0][0]=(*d_Geo).m_rowX[geoIdx].x;m[0][1]=(*d_Geo).m_rowX[geoIdx].y;m[0][2]=(*d_Geo).m_rowX[geoIdx].z;
      m[1][0]=(*d_Geo).m_rowY[geoIdx].x;m[1][1]=(*d_Geo).m_rowY[geoIdx].y;m[1][2]=(*d_Geo).m_rowY[geoIdx].z;
      m[2][0]=(*d_Geo).m_rowZ[geoIdx].x;m[2][1]=(*d_Geo).m_rowZ[geoIdx].y;m[2][2]=(*d_Geo).m_rowZ[geoIdx].z;
      float4 ip2=(*d_Geo).m_parSet[geoIdx];
      float radLength=ip2.w;

      invertM(m,g);
      
      //A4. global->global extrapolation, Jm

      float D[4];
      D[0]=m[0][2];
      D[1]=m[1][2];
      D[2]=m[2][2];
      D[3]=-D[0]*c[0]-D[1]*c[1]-D[2]*c[2];
			
      gB[0]=(ip1.x+ip2.x)/2;
      gB[1]=(ip1.y+ip2.y)/2;
      gB[2]=(ip1.z+ip2.z)/2;

      ip1=ip2;
			
      cc=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
      b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
      a=CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+gB[1]*(D[2]*gV[0]-D[0]*gV[2])+gB[2]*(D[0]*gV[1]-D[1]*gV[0]))/2;
      float descr=b*b-4*a*cc;
      if(descr<0) {
	status=0;
	continue;
      }
			
      sl=-cc/b;sl=sl*(1-a*sl/b);// expected path

      int nStepMax;
      if(fabs(sl)<minStep) nStepMax=1;
      else {
	nStepMax=(int)(fabs(sl)/minStep)+1;
      }
      if((nStepMax<0)||(nStepMax>1000)){
	status=0;
	continue;
      }
      float Av=sl*CQ;
      float Ac=sl*Av/2;
      float C2=sl*sl*C/2;
			
      float DVx=gV[1]*gB[2]-gV[2]*gB[1];
      float DVy=gV[2]*gB[0]-gV[0]*gB[2];
      float DVz=gV[0]*gB[1]-gV[1]*gB[0];

      float V[3];
      V[0]=gV[0]+Av*DVx;V[1]=gV[1]+Av*DVy;V[2]=gV[2]+Av*DVz;
			
     	
      memset(&Jm[0][0],0,sizeof(Jm));
      float coeff[3], dadVx,dadVy,dadVz,dadQ,dsdx,dsdy,dsdz,dsdVx,dsdVy,dsdVz,dsdQ;
      coeff[0]=-cc*cc/(b*b*b);coeff[1]=cc*(1+3*cc*a/(b*b))/(b*b);coeff[2]=-(1+2*cc*a/(b*b))/b;
      dadVx=CQ*(-D[1]*gB[2]+D[2]*gB[1])/2;
      dadVy=CQ*( D[0]*gB[2]-D[2]*gB[0])/2;
      dadVz=CQ*(-D[0]*gB[1]+D[1]*gB[0])/2;
      dadQ=C*(D[0]*DVx+D[1]*DVy+D[2]*DVz)/2;
      dsdx=coeff[2]*D[0];
      dsdy=coeff[2]*D[1];
      dsdz=coeff[2]*D[2];
      dsdVx=coeff[0]*dadVx+coeff[1]*D[0];
      dsdVy=coeff[0]*dadVy+coeff[1]*D[1];
      dsdVz=coeff[0]*dadVz+coeff[1]*D[2];
      dsdQ=coeff[0]*dadQ;
      
      Jm[0][0]=1 +V[0]*dsdx;
      Jm[0][1]=    V[0]*dsdy;
      Jm[0][2]=    V[0]*dsdz;
      
      Jm[0][3]= sl+V[0]*dsdVx;
      Jm[0][4]=    V[0]*dsdVy+Ac*gB[2];
      Jm[0][5]=    V[0]*dsdVz-Ac*gB[1];
      Jm[0][6]=    V[0]*dsdQ+C2*DVx;
      
      Jm[1][0]=    V[1]*dsdx;
      Jm[1][1]=1 +V[1]*dsdy;
      Jm[1][2]=    V[1]*dsdz;
      
      Jm[1][3]=    V[1]*dsdVx-Ac*gB[2];
      Jm[1][4]= sl+V[1]*dsdVy;
      Jm[1][5]=    V[1]*dsdVz+Ac*gB[0];
      Jm[1][6]=    V[1]*dsdQ+C2*DVy;
      
      Jm[2][0]=    V[2]*dsdx;
      Jm[2][1]=    V[2]*dsdy;
      Jm[2][2]=1 +V[2]*dsdz;
      Jm[2][3]=    V[2]*dsdVx+Ac*gB[1];
      Jm[2][4]=    V[2]*dsdVy-Ac*gB[0];
      Jm[2][5]= sl+V[2]*dsdVz;
      Jm[2][6]=    V[2]*dsdQ+C2*DVz;
			
      Jm[3][0]=dsdx*CQ*DVx;
      Jm[3][1]=dsdy*CQ*DVx;
      Jm[3][2]=dsdz*CQ*DVx;
			
      Jm[3][3]=1 +dsdVx*CQ*DVx;
      Jm[3][4]=CQ*(dsdVy*DVx+sl*gB[2]);
      Jm[3][5]=CQ*(dsdVz*DVx-sl*gB[1]);
      
      Jm[3][6]=(CQ*dsdQ+C*sl)*DVx;
      
      Jm[4][0]=dsdx*CQ*DVy;
      Jm[4][1]=dsdy*CQ*DVy;
      Jm[4][2]=dsdz*CQ*DVy;
      
      Jm[4][3]=CQ*(dsdVx*DVy-sl*gB[2]);
      Jm[4][4]=1 +dsdVy*CQ*DVy;
      Jm[4][5]=CQ*(dsdVz*DVy+sl*gB[0]);
      
      Jm[4][6]=(CQ*dsdQ+C*sl)*DVy;
      
      Jm[5][0]=dsdx*CQ*DVz;
      Jm[5][1]=dsdy*CQ*DVz;
      Jm[5][2]=dsdz*CQ*DVz;
      Jm[5][3]=CQ*(dsdVx*DVz+sl*gB[1]);
      Jm[5][4]=CQ*(dsdVy*DVz-sl*gB[0]);
      Jm[5][5]=1 +dsdVz*CQ*DVz;
      Jm[5][6]=(CQ*dsdQ+C*sl)*DVz;
			
      Jm[6][6]=1.0f;
      
      a = -V[1]/(V[0]*V[0]+V[1]*V[1]);
      b =  V[0]/(V[0]*V[0]+V[1]*V[1]);
      cc = -1/sqrt(1-V[2]*V[2]);

      memset(&JG[0][0],0,sizeof(JG));
      
      for(int ii=0;ii<7;ii++) {
	JG[0][ii]=g[0][0]*Jm[0][ii]+g[0][1]*Jm[1][ii]+g[0][2]*Jm[2][ii];
	JG[1][ii]=g[1][0]*Jm[0][ii]+g[1][1]*Jm[1][ii]+g[1][2]*Jm[2][ii];
	JG[2][ii]=a*Jm[3][ii]+b*Jm[4][ii];
	JG[3][ii]=cc*Jm[5][ii];
	JG[4][ii]=Jm[6][ii];
      }

      memset(&J.m_J[0][0],0,sizeof(JACOBIAN_TYPE));
      for(int ii=0;ii<5;ii++) 
	for(int jj=0;jj<5;jj++) {
	  for(int kk=0;kk<7;kk++) J.m_J[ii][jj]+=JG[ii][kk]*J0[kk][jj];
	}
      
      // J - jacobian of the full transformation: local-global-local
      
      for(int jj=0;jj<5;jj++) {
	for(int ii=0;ii<5;ii++) {
	  JG[ii][jj]=0.0;
	  for(int kk=0;kk<5;kk++) 
	    JG[ii][jj]+=J.m_J[ii][kk]*ets.m_cov[translateIndex(kk,jj)];
	}
      }

      for(int jj=0;jj<5;jj++) {
	int base = jj*(jj+1)/2;
	for(int ii=0;ii<=jj;ii++) {
	  ets.m_cov[ii+base]=0.0;
	  for(int kk=0;kk<5;kk++) ets.m_cov[ii+base]+=JG[ii][kk]*J.m_J[jj][kk];
	}
      }

      memset(&JG[0][0],0,sizeof(JG));
      for(int ii=0;ii<5;ii++) 
	for(int jj=0;jj<5;jj++) {
	  for(int kk=0;kk<5;kk++) JG[ii][jj]+=J.m_J[ii][kk]*ets.m_cov[translateIndex(kk,jj+5)];
	}

      for(int jj=0;jj<5;jj++) {
	int base = (jj+5)*(jj+6)/2;
	for(int ii=0;ii<=jj;ii++) {
	  ets.m_cov[ii+base]=JG[ii][jj];
	}
      }

      int nStep=nStepMax;

      // Track parameter extrapolation
			
      while(nStep>0) {
	cc=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
	b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
	a=0.5f*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+gB[1]*(D[2]*gV[0]-D[0]*gV[2])+gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
	sl=-cc/b;
	sl=sl*(1-a*sl/b);
	float ds=sl/nStep;
	//path+=ds;
	Av=ds*CQ;
	Ac=0.5f*ds*Av;
	DVx=gV[1]*gB[2]-gV[2]*gB[1];
	DVy=gV[2]*gB[0]-gV[0]*gB[2];
	DVz=gV[0]*gB[1]-gV[1]*gB[0];
	gP[0]=gP[0]+gV[0]*ds+Ac*DVx;gP[1]=gP[1]+gV[1]*ds+Ac*DVy;gP[2]=gP[2]+gV[2]*ds+Ac*DVz;
	gV[0]=gV[0]+Av*DVx;gV[1]=gV[1]+Av*DVy;gV[2]=gV[2]+Av*DVz;
	nStep--;
      }

      a=gP[0]-c[0];b=gP[1]-c[1];cc=gP[2]-c[2];
      gP[0]=g[0][0]*a+g[0][1]*b+g[0][2]*cc;
      gP[1]=g[1][0]*a+g[1][1]*b+g[1][2]*cc;
      gP[2]=g[2][0]*a+g[2][1]*b+g[2][2]*cc;
      
      ets.m_par[0]=gP[0];ets.m_par[1]=gP[1];
      ets.m_par[2]= atan2f(gV[1],gV[0]);
      if(fabs(gV[2])>1) {
	status = 0;
	continue;
      }
      ets.m_par[3]=acosf(gV[2]);

      //A5. material correction
      applyMaterialEffects(ets,radLength,g,gV,false);

      //A6. read next hit

      hitType = (*d_Hit).m_hitType[geoIdx];
      meas = (*d_Hit).m_meas[geoIdx];
      measCov = (*d_Hit).m_cov[geoIdx];
    }
    
    // extrapolating track parameters back to perigee

    //B1. reading first plane
		
    geoIdx = idx;
    c[0]=(*d_Geo).m_rowX[geoIdx].w;c[1]=(*d_Geo).m_rowY[geoIdx].w;c[2]=(*d_Geo).m_rowZ[geoIdx].w;
    m[0][0]=(*d_Geo).m_rowX[geoIdx].x;m[0][1]=(*d_Geo).m_rowX[geoIdx].y;m[0][2]=(*d_Geo).m_rowX[geoIdx].z;
    m[1][0]=(*d_Geo).m_rowY[geoIdx].x;m[1][1]=(*d_Geo).m_rowY[geoIdx].y;m[1][2]=(*d_Geo).m_rowY[geoIdx].z;
    m[2][0]=(*d_Geo).m_rowZ[geoIdx].x;m[2][1]=(*d_Geo).m_rowZ[geoIdx].y;m[2][2]=(*d_Geo).m_rowZ[geoIdx].z;
    
    invertM(m,g);
    
    ip1=(*d_Geo).m_parSet[geoIdx];
    float radLength=ip1.w;
    
    //B2. material correction
  
    sint=sin(ets.m_par[8]);
    cosf=cos(ets.m_par[7]);
    sinf=sin(ets.m_par[7]);
    cost=cos(ets.m_par[8]);
    
    gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;

    applyMaterialEffects(ets,radLength,g,gV,true);
    
    //B3. transform to global
		
    float dVxdf,dVydf,dVxdt,dVydt,dsdx0,dsdy0,dsdf0,dsdt0,dVzdt;

    gB[0]=ip1.x/2;
    gB[1]=ip1.y/2;
    gB[2]=(ip1.z+B0z)/2;
    gP[0]=c[0]+m[0][0]*ets.m_par[5]+m[0][1]*ets.m_par[6];
    gP[1]=c[1]+m[1][0]*ets.m_par[5]+m[1][1]*ets.m_par[6];
    gP[2]=c[2]+m[2][0]*ets.m_par[5]+m[2][1]*ets.m_par[6];
		
    CQ=C*ets.m_par[9];
    a=3*CQ*gV[2]*(gB[0]*gV[1]-gB[1]*gV[0])/2;
    b=gV[0]*gV[0]+gV[1]*gV[1]+CQ*(gP[0]*(gV[1]*gB[2]-gV[2]*gB[1])+
				  gP[1]*(gV[2]*gB[0]-gV[0]*gB[2]));
    cc=gP[0]*gV[0]+gP[1]*gV[1];
    sl=-cc/b;
    sl=sl*(1-sl*a/b);
    float s2Q=sl*sl*CQ/2;
    gP[0]+=gV[0]*sl+s2Q*(gV[1]*gB[2]-gV[2]*gB[1]);
    gP[1]+=gV[1]*sl+s2Q*(gV[2]*gB[0]-gV[0]*gB[2]);
    gP[2]+=gV[2]*sl+s2Q*(gV[0]*gB[1]-gV[1]*gB[0]);
    a=gV[0]+sl*CQ*(gV[1]*gB[2]-gV[2]*gB[1]);
    b=gV[1]+sl*CQ*(gV[2]*gB[0]-gV[0]*gB[2]);
    cc=gV[2]+sl*CQ*(gV[0]*gB[1]-gV[1]*gB[0]);
    gV[0]=a;gV[1]=b;gV[2]=cc;

    dVxdf=-sint*sinf;
    dVxdt= cost*cosf;
    dVydf= sint*cosf;
    dVydt= cost*sinf;
    dVzdt= -sint;
    
    dsdx0=-(m[0][0]*cosf+m[1][0]*sinf)/sint;
    dsdy0=-(m[0][1]*cosf+m[1][1]*sinf)/sint;
    dsdf0= (gP[0]*sinf-gP[1]*cosf)/sint;
    dsdt0= (gP[0]*cosf+gP[1]*sinf)*cost/(sint*sint);

    float d0=sqrt(gP[0]*gP[0]+gP[1]*gP[1]);
    float delta=gV[0]*gP[1]-gV[1]*gP[0];
    if(delta<0) d0=-d0;	
    
    memset(&J.m_J[0][0],0,sizeof(JACOBIAN_TYPE));
    
    J.m_J[4][4]=1;
    J.m_J[3][3]=1;
    J.m_J[2][0]=-CQ*gB[2]*dsdx0;
    J.m_J[2][1]=-CQ*gB[2]*dsdy0;
    J.m_J[2][2]=1-CQ*gB[2]*dsdf0;
    J.m_J[2][3]=-CQ*gB[2]*dsdt0;
    J.m_J[2][4]=-C*gB[2]*sl;
    
    J.m_J[1][0]=m[2][0]+dsdx0*gV[2];
    J.m_J[1][1]=m[2][1]+dsdy0*gV[2];
    J.m_J[1][2]=dsdf0*gV[2];
    J.m_J[1][3]=dsdt0*gV[2]+sl*dVzdt;
    
    
    J.m_J[0][0]=(gP[0]*(m[0][0]+dsdx0*gV[0])+gP[1]*(m[1][0]+dsdx0*gV[1]))/d0;
    J.m_J[0][1]=(gP[0]*(m[0][1]+dsdy0*gV[0])+gP[1]*(m[1][1]+dsdy0*gV[1]))/d0;
    J.m_J[0][2]=(gP[0]*(dVxdf*sl+dsdf0*gV[0])+gP[1]*(dVydf*sl+dsdf0*gV[1]))/d0;
    J.m_J[0][3]=(gP[0]*(dVxdt*sl+dsdt0*gV[0])+gP[1]*(dVydt*sl+dsdt0*gV[1]))/d0;

    for(int ii=0;ii<5;ii++)
      for(int jj=0;jj<5;jj++) {
	JG[ii][jj]=0.0;
	for(int kk=0;kk<5;kk++) JG[ii][jj]+=J.m_J[ii][kk]*ets.m_cov[translateIndex(kk+5,jj+5)];
      }

    for(int jj=0;jj<5;jj++) {
      int base = 5+(jj+5)*(jj+6)/2;
      for(int ii=0;ii<=jj;ii++) {
	ets.m_cov[ii+base]=0.0;
	for(int kk=0;kk<5;kk++) ets.m_cov[ii+base]+=JG[ii][kk]*J.m_J[jj][kk];
      }
    }

    for(int ii=0;ii<5;ii++)
      for(int jj=0;jj<5;jj++) {
	JG[ii][jj]=0.0;
	for(int kk=0;kk<5;kk++) JG[ii][jj]+=J.m_J[ii][kk]*ets.m_cov[translateIndex(kk+5,jj)];
      }

    for(int ii=0;ii<5;ii++)
      for(int jj=0;jj<5;jj++) {
	ets.m_cov[translateIndex(ii,jj+5)]=JG[ii][jj];
      }

    
    ets.m_par[5]=d0;
    ets.m_par[6]=gP[2];
    ets.m_par[7]=atan2f(gV[1],gV[0]);
    ets.m_par[8]=acosf(gV[2]);
    ets.m_par[4]=ets.m_par[4]/1000; //GeV->MeV
    ets.m_par[9]=ets.m_par[9]/1000;
    (*d_Out).m_status[idx] = status;
    (*d_Out).m_chi2[idx]=chi2;
    (*d_Out).m_ndof[idx]=ndof;
    
    (*d_Out).m_par0[idx]=ets.m_par[5];
    (*d_Out).m_par1[idx]=ets.m_par[6];
    (*d_Out).m_par2[idx]=ets.m_par[7];
    (*d_Out).m_par3[idx]=ets.m_par[8];
    (*d_Out).m_par4[idx]=ets.m_par[9];

    (*d_Out).m_cov00[idx]=ets.m_cov[20];
    (*d_Out).m_cov01[idx]=ets.m_cov[26];
    (*d_Out).m_cov02[idx]=ets.m_cov[33];
    (*d_Out).m_cov03[idx]=ets.m_cov[41];
    (*d_Out).m_cov04[idx]=ets.m_cov[50]/1000;
    
    (*d_Out).m_cov11[idx]=ets.m_cov[27];
    (*d_Out).m_cov12[idx]=ets.m_cov[34];
    (*d_Out).m_cov13[idx]=ets.m_cov[42];
    (*d_Out).m_cov14[idx]=ets.m_cov[51]/1000;
    
    (*d_Out).m_cov22[idx]=ets.m_cov[35];
    (*d_Out).m_cov23[idx]=ets.m_cov[43];
    (*d_Out).m_cov24[idx]=ets.m_cov[52]/1000;
    
    (*d_Out).m_cov33[idx]=ets.m_cov[44];
    (*d_Out).m_cov34[idx]=ets.m_cov[53]/1000;
    
    (*d_Out).m_cov44[idx]=ets.m_cov[54]/1000000;
 
  }
}
