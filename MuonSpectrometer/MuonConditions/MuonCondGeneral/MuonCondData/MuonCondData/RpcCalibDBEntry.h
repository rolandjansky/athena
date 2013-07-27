/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCALIBDBENTRY_H
#define RPCCALIBDBENTRY_H

#include <string>
#include <iostream>
#include "MuonCondData/RpcCalibData.h"
#include "Identifier/Identifier.h"
//#include <sstream>

namespace MuonCalib{

    /**
      @class RpcCalibDBEntry

      @author andrea.di.simone@cern.ch

      @brief representation of a db entry in rpc cond db.

      RPC calib db entry
     */

    class RpcCalibDBEntry
    {
        public:
            RpcCalibDBEntry(){};
            RpcCalibDBEntry(Identifier gapID, std::string etaRec, std::string etaDet, std::string phiRec1, std::string phiRec2, std::string phiDet1, std::string phiDet2);
            RpcCalibDBEntry(Identifier gapID, std::string payload);

	    void getColumns(std::string &recEta, std::string &detEta,std::string &recPhi1,std::string &recPhi2,std::string &detPhi1,std::string &detPhi2) const;  

            ~RpcCalibDBEntry();

	    RpcCalibData * getData(int measPhi, int strip) const ;
	    const Identifier getGapID() const;
	    int getNEtaStrips() const {return nRecEta;}
	    int getNPhiStrips() const {return nRecPhi1;}

        private:
	    void  initData(std::string etaRec, std::string etaDet, std::string phiRec1, std::string phiRec2, std::string phiDet1, std::string phiDet2);
	    int nRecEta, nDetEta,nRecPhi1,nRecPhi2,nDetPhi1,nDetPhi2;
	    Identifier theGap;
	    std::vector<RpcCalibData*> m_thePhiData;
	    std::vector<RpcCalibData*> m_theEtaData;
	    
    };

}

#endif
