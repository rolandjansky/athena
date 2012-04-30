/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This header contain all strings defining cells in tilecal
// Here we also define the tree "tile"

TTree *tile = (TTree*)file->Get("CALO/169");

// Drawers 
   TCut a0 ="EtaCells[66]>=0 && EtaCells[69]>=0 && EtaCells[71]>=0 && EtaCells[74]>=0 && EtaCells[76]>=0 &&\
             EtaCells[79]>=0 && EtaCells[81]>=0 && EtaCells[84]>=0 && EtaCells[86]>=0 && EtaCells[88]>=0 &&\
             EtaCells[67]>=0 && EtaCells[70]>=0 && EtaCells[72]>=0 && EtaCells[75]>=0 && EtaCells[77]>=0 &&\
             EtaCells[80]>=0 && EtaCells[82]>=0 && EtaCells[85]>=0 && EtaCells[87]>=0 &&\
             EtaCells[68]>=0 && EtaCells[73]>=0 && EtaCells[78]>=0 && EtaCells[83]";
   TCut e0 ="EtaCells[71]<0 && EtaCells[73]<0 && EtaCells[76]<0 && EtaCells[78]<0 && EtaCells[80]<0 && \
             EtaCells[69]<0 && EtaCells[72]<0 && EtaCells[74]<0 && EtaCells[77]<0 && EtaCells[79]<0 && \
             EtaCells[70]<0 && EtaCells[75]";  
   
   TCut a1 ="EtaCells[89]>=0 && EtaCells[92]>=0 && EtaCells[94]>=0 && EtaCells[97]>=0 && EtaCells[99]>=0 && \
             EtaCells[102]>=0 && EtaCells[104]>=0 && EtaCells[107]>=0 && EtaCells[109]>=0 && EtaCells[111]>=0 && \
             EtaCells[90]>=0 && EtaCells[93]>=0 && EtaCells[95]>=0 && EtaCells[98]>=0 && EtaCells[100]>=0 && \
             EtaCells[103]>=0 && EtaCells[105]>=0 && EtaCells[108]>=0 && EtaCells[110]>=0 && \
             EtaCells[91]>=0 && EtaCells[96]>=0 && EtaCells[101]>=0 && EtaCells[106]";  
   TCut e1 ="ECells[83]+ECells[85]+ECells[88]+ECells[90]+ECells[92]+\
             ECells[81]+ECells[84]+ECells[86]+ECells[89]+ECells[91]+\
             ECells[82]+ECells[87]";	     
   
   TCut a2 ="EtaCells[112]>=0 && EtaCells[115]>=0 && EtaCells[117]>=0 && EtaCells[120]>=0 && EtaCells[122]>=0 && \
             EtaCells[125]>=0 && EtaCells[127]>=0 && EtaCells[130]>=0 && EtaCells[132]>=0 && EtaCells[134]>=0 && \
             EtaCells[113]>=0 && EtaCells[116]>=0 && EtaCells[118]>=0 && EtaCells[121]>=0 && EtaCells[123]>=0 && \
             EtaCells[126]>=0 && EtaCells[128]>=0 && EtaCells[131]>=0 && EtaCells[133]>=0 && \
             EtaCells[114]>=0 && EtaCells[119]>=0 && EtaCells[124]>=0 && EtaCells[129]";        
   TCut e2 ="EtaCells[95]<0 && EtaCells[97]<0 && EtaCells[100]<0 && EtaCells[102]<0 && EtaCells[104]<0 && \
             EtaCells[93]<0 && EtaCells[96]<0 && EtaCells[98]<0 && EtaCells[101]<0 && EtaCells[103]<0 && \
             EtaCells[94]<0 && EtaCells[99]";


   
   TString A0="ECells[66]+ECells[69]+ECells[71]+ECells[74]+ECells[76]+\
               ECells[79]+ECells[81]+ECells[84]+ECells[86]+ECells[88]+\
               ECells[67]+ECells[70]+ECells[72]+ECells[75]+ECells[77]+\
               ECells[80]+ECells[82]+ECells[85]+ECells[87]+\
               ECells[68]+ECells[73]+ECells[78]+ECells[83]";      
   TString C0="ECells[0]+ECells[2]+ECells[4]+ECells[7]+ECells[9]+\
               ECells[12]+ECells[14]+ECells[17]+ECells[19]+ECells[21]+\
               ECells[1]+ECells[3]+ECells[5]+ECells[8]+ECells[10]+\
               ECells[13]+ECells[15]+ECells[18]+ECells[20]+\
               ECells[6]+ECells[11]+ECells[16]+ECells[68]";   
   TString E0="ECells[71]+ECells[73]+ECells[76]+ECells[78]+ECells[80]+\
               ECells[69]+ECells[72]+ECells[74]+ECells[77]+ECells[79]+\
               ECells[70]+ECells[75]";
   
   TString A1="ECells[89]+ECells[92]+ECells[94]+ECells[97]+ECells[99]+\
               ECells[102]+ECells[104]+ECells[107]+ECells[109]+ECells[111]+\
               ECells[90]+ECells[93]+ECells[95]+ECells[98]+ECells[100]+\
               ECells[103]+ECells[105]+ECells[108]+ECells[110]+\
               ECells[91]+ECells[96]+ECells[101]+ECells[106]";         
   TString C1="ECells[22]+ECells[24]+ECells[26]+ECells[29]+ECells[31]+\
               ECells[34]+ECells[36]+ECells[39]+ECells[41]+ECells[43]+\
               ECells[23]+ECells[25]+ECells[27]+ECells[30]+ECells[32]+\
               ECells[35]+ECells[37]+ECells[40]+ECells[42]+\
               ECells[28]+ECells[33]+ECells[38]+ECells[91]";
   TString E1="ECells[83]+ECells[85]+ECells[88]+ECells[90]+ECells[92]+\
               ECells[81]+ECells[84]+ECells[86]+ECells[89]+ECells[91]+\
               ECells[82]+ECells[87]";	       	       
      
   TString A2="ECells[112]+ECells[115]+ECells[117]+ECells[120]+ECells[122]+\
               ECells[125]+ECells[127]+ECells[130]+ECells[132]+ECells[134]+\
               ECells[113]+ECells[116]+ECells[118]+ECells[121]+ECells[123]+\
               ECells[126]+ECells[128]+ECells[131]+ECells[133]+\
               ECells[114]+ECells[119]+ECells[124]+ECells[129]";        
   TString C2="ECells[44]+ECells[46]+ECells[48]+ECells[51]+ECells[53]+\
               ECells[56]+ECells[58]+ECells[61]+ECells[63]+ECells[65]+\
               ECells[45]+ECells[47]+ECells[49]+ECells[52]+ECells[54]+\
               ECells[57]+ECells[59]+ECells[62]+ECells[64]+\
               ECells[50]+ECells[55]+ECells[60]+ECells[114]";
   TString E2="ECells[95]+ECells[97]+ECells[100]+ECells[102]+ECells[104]+\
               ECells[93]+ECells[96]+ECells[98]+ECells[101]+ECells[103]+\
               ECells[94]+ECells[99]";	     

// Towers
TString T_10="ECells[111]";
TString T_9= "ECells[109]+ECells[110]";
TString T_8= "ECells[107]+ECells[108]";
TString T_7= "ECells[104]+ECells[105]+ECells[106]";
TString T_6= "ECells[102]+ECells[103]+ECells[106]";
TString T_5= "ECells[99]+ECells[100]+ECells[101]";
TString T_4= "ECells[97]+ECells[98]+ECells[101]";
TString T_3= "ECells[94]+ECells[95]+ECells[96]";
TString T_2= "ECells[92]+ECells[93]+ECells[96]";
TString T_1= "ECells[89]+ECells[90]+ECells[91]";
TString T1 = "ECells[22]+ECells[23]+ECells[91]";
TString T2 = "ECells[24]+ECells[25]+ECells[28]";
TString T3 = "ECells[26]+ECells[27]+ECells[28]";
TString T4 = "ECells[29]+ECells[30]+ECells[33]";
TString T5 = "ECells[31]+ECells[32]+ECells[33]";
TString T6 = "ECells[34]+ECells[35]+ECells[38]";
TString T7 = "ECells[36]+ECells[37]+ECells[38]";
TString T8 = "ECells[39]+ECells[40]";
TString T9 = "ECells[41]+ECells[42]";
TString T10= "ECells[43]+ECells[82]";
TString T11= "ECells[81]+ECells[82]";
TString T12= "ECells[83]+ECells[84]+ECells[82]+ECells[87]";
TString T13= "ECells[85]+ECells[86]+ECells[87]";
TString T14= "ECells[88]+ECells[89]+ECells[87]";
TString T15= "ECells[90]+ECells[91]";
TString T16= "ECells[92]+ECells[91]";
