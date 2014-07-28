/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuLUT.h"
#include <stdio.h>
#include <stdlib.h>             // for exit()


///////////////////////////////////////////////////////////////////////////////

std::string MuLUT::s_lut_file = "lut.data";
MuLUT* MuLUT::s_instance = 0;

///////////////////////////////////////////////////////////////////////////////
#ifdef LVL2_STANDALONE

const MuLUT*
MuLUT::instance(void)
{
    if (! s_instance) {
        s_instance = new MuLUT;
    }

    return s_instance;
}

#endif
///////////////////////////////////////////////////////////////////////////////


MuLUT::MuLUT(void)
{
    // create and check the Look-Up Table
    // using private auxiliary functions
    t2murlut();
}

MuLUT::~MuLUT(void)
{
    // probably nothing to do, since m_lut is on the stack
}

void
MuLUT::t2murlut(void)
{
    int s,c,i,j,k,nlut;
    int charge,chamber;
    float A0,A1,etamin,etamax,phimin,phimax;
    FILE *file;
    
    std::string tag = "_rad";
    m_haveLUTforRadius = (s_lut_file.find(tag)!=std::string::npos)? true: false;
    
    //  printf("Inizio la pulizia delle tabelle di look up\n");
    for(s=0;s<4;s++) for(c=0;c<2;c++) for(i=0;i<50;i++) for(j=0;j<50;j++)
        for(k=0;k<2;k++) m_lut.table[s][c][i][j][k] = 0.;
    //  printf("Apro il file lut_test01.out\n");
    if((file = fopen(s_lut_file.c_str(),"r"))==0) {
        printf("\nT2muini error: cannot open LUT file!\n");
        exit(1);
    }

    for(nlut=0;nlut<8;++nlut) {
      //  printf("Tento di leggere la prima linea\n"); 
      //  fscanf(file,"%*4c%d%*8c%d%*14c%d",&pt,&charge,&chamber);
      fscanf(file,"%d%d",&charge,&chamber);
      //  printf("\n La roba letta e`\n");
      //  printf("Charge = %d,  Cham. type = %d\n",charge,chamber);
      fscanf(file,"%f%f%f%f%d%d",&etamin,&etamax,&phimin,&phimax,
             &m_lut.NbinEta[chamber],&m_lut.NbinPhi[chamber]);
      m_lut.EtaMin[chamber] = etamin;
      m_lut.EtaMax[chamber] = etamax;
      m_lut.PhiMin[chamber] = phimin;
      m_lut.PhiMax[chamber] = phimax;
      m_lut.PhiStep[chamber] = (phimax-phimin)/m_lut.NbinPhi[chamber];
      m_lut.EtaStep[chamber] = (etamax-etamin)/m_lut.NbinEta[chamber];
  
      for(i=0;i<m_lut.NbinEta[chamber];i++) {
          for(j=0;j<m_lut.NbinPhi[chamber];j++) {
              fscanf(file,"%f%f",&A1,&A0);
              m_lut.table[chamber][charge][i][j][0] = A1;
              m_lut.table[chamber][charge][i][j][1] = A0; 
          }
      }
  
    //  printf("A1=%f   A0=%f\n",m_lut.table[chamber][charge][1][3][0],
    //                           m_lut.table[chamber][charge][1][3][1]);
    }	
    
    /*   
    //  printf("Tento di leggere la prima linea\n"); 
    //  fscanf(file,"%*4c%d%*8c%d%*14c%d",&pt,&charge,&chamber);
    fscanf(file,"%d%d",&charge,&chamber);
    //  printf("\n La roba letta e`\n");
    //  printf("Charge = %d,  Cham. type = %d\n",charge,chamber);
    fscanf(file,"%f%f%f%f%d%d",&etamin,&etamax,&phimin,&phimax,
           &m_lut.NbinEta[chamber],&m_lut.NbinPhi[chamber]);
    m_lut.EtaMin[chamber] = etamin;
    m_lut.EtaMax[chamber] = etamax;
    m_lut.PhiMin[chamber] = phimin;
    m_lut.PhiMax[chamber] = phimax;
    m_lut.PhiStep[chamber] = (phimax-phimin)/m_lut.NbinPhi[chamber];
    m_lut.EtaStep[chamber] = (etamax-etamin)/m_lut.NbinEta[chamber];
  
    for(i=0;i<m_lut.NbinEta[chamber];i++) {
        for(j=0;j<m_lut.NbinPhi[chamber];j++) {
            fscanf(file,"%f%f",&A1,&A0);
            m_lut.table[chamber][charge][i][j][0] = A1;
            m_lut.table[chamber][charge][i][j][1] = A0; 
        }
    }  
  
    //  printf("A1=%f   A0=%f\n",m_lut.table[chamber][charge][1][3][0],
    //                           m_lut.table[chamber][charge][1][3][1]);

    //  printf("Tento di leggere la prima linea\n"); 
    //  fscanf(file,"%*4c%d%*8c%d%*14c%d",&pt,&charge,&chamber);
    fscanf(file,"%d%d",&charge,&chamber);
    //  printf("\n La roba letta e`\n");
    //  printf("Charge = %d,  Cham. type = %d\n",charge,chamber);
    fscanf(file,"%f%f%f%f%d%d",&etamin,&etamax,&phimin,&phimax,
           &m_lut.NbinEta[chamber],&m_lut.NbinPhi[chamber]);
    m_lut.EtaMin[chamber] = etamin;
    m_lut.EtaMax[chamber] = etamax;
    m_lut.PhiMin[chamber] = phimin;
    m_lut.PhiMax[chamber] = phimax;
    m_lut.PhiStep[chamber] = (phimax-phimin)/m_lut.NbinPhi[chamber];
    m_lut.EtaStep[chamber] = (etamax-etamin)/m_lut.NbinEta[chamber];
  
    for(i=0;i<m_lut.NbinEta[chamber];i++) {
        for(j=0;j<m_lut.NbinPhi[chamber];j++) {
            fscanf(file,"%f%f",&A1,&A0);
            m_lut.table[chamber][charge][i][j][0] = A1;
            m_lut.table[chamber][charge][i][j][1] = A0; 
        }
    }  
  
    //  printf("A1=%f   A0=%f\n",m_lut.table[chamber][charge][1][3][0],
    //                           m_lut.table[chamber][charge][1][3][1]);

    //  printf("Tento di leggere la prima linea\n"); 
    //  fscanf(file,"%*4c%d%*8c%d%*14c%d",&pt,&charge,&chamber);
    fscanf(file,"%d%d",&charge,&chamber);
    //  printf("\n La roba letta e`\n");
    //  printf("Charge = %d,  Cham. type = %d\n",charge,chamber);
    fscanf(file,"%f%f%f%f%d%d",&etamin,&etamax,&phimin,&phimax,
           &m_lut.NbinEta[chamber],&m_lut.NbinPhi[chamber]);
    m_lut.EtaMin[chamber] = etamin;
    m_lut.EtaMax[chamber] = etamax;
    m_lut.PhiMin[chamber] = phimin;
    m_lut.PhiMax[chamber] = phimax;
    m_lut.PhiStep[chamber] = (phimax-phimin)/m_lut.NbinPhi[chamber];
    m_lut.EtaStep[chamber] = (etamax-etamin)/m_lut.NbinEta[chamber];
  
    for(i=0;i<m_lut.NbinEta[chamber];i++) {
        for(j=0;j<m_lut.NbinPhi[chamber];j++) {
            fscanf(file,"%f%f",&A1,&A0);
            m_lut.table[chamber][charge][i][j][0] = A1;
            m_lut.table[chamber][charge][i][j][1] = A0; 
        }
    }  
  
    //  printf("A1=%f   A0=%f\n",m_lut.table[chamber][charge][1][3][0],
    //                           m_lut.table[chamber][charge][1][3][1]);
   
    */
    
    fclose(file);
}

