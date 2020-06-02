/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_StripVmmMappingTool.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <string>
#include <tuple>

//returns ADDC, MMFE, VMM
int electronics::elec(int stripNumber, std::string tech, int off_stationEta, int chMax)
{

        off_stationEta = std::abs(off_stationEta);

        int MMFE_total = off_stationEta == 1 ? 10 : 6;
        int MMFE_size = chMax/MMFE_total;
        double MMFE_num = (double)stripNumber/MMFE_size;
//MMFE
        int MMFE = 1+(int)MMFE_num;
        MMFE = MMFE + (off_stationEta == 1 ? 0 : 10);
//ADDC
        int ADDC = MMFE;

        int firstStripInMMFE = (int)MMFE_num*MMFE_size;
        int stripNumInMMFE = stripNumber - firstStripInMMFE;
        int VMM_size = 64;
        double VMM_num = (double)stripNumInMMFE/VMM_size;
//VMM
        int VMM = 1+(int)VMM_num;

if      (tech[0]=='A') return ADDC;
else if (tech[0]=='M') return MMFE;
else if (tech[0]=='V') return VMM;

MsgStream log(Athena::getMessageSvc(), "electronics");
log<<MSG::WARNING<<"elec() - MMStripVmmMappingTool: something went wrong"<<endmsg;

return 0;

}
