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
            RpcCalibDBEntry(Identifier gapID, const std::string& etaRec, const std::string& etaDet, const std::string& phiRec1, const std::string& phiRec2, const std::string& phiDet1, const std::string& phiDet2);
            RpcCalibDBEntry(Identifier gapID, std::string_view payload);

	    void getColumns(std::string &recEta, std::string &detEta,std::string &recPhi1,std::string &recPhi2,std::string &detPhi1,std::string &detPhi2) const;  

            ~RpcCalibDBEntry();

	    RpcCalibData * getData(int measPhi, int strip) const ;
	    const Identifier getGapID() const;
	    int getNEtaStrips() const {return m_nRecEta;}
	    int getNPhiStrips() const {return m_nRecPhi1;}

        private:
	    void  initData(std::string etaRec, std::string etaDet, std::string phiRec1, std::string phiRec2, std::string phiDet1, std::string phiDet2);
	    int m_nRecEta, m_nDetEta,m_nRecPhi1,m_nRecPhi2,m_nDetPhi1,m_nDetPhi2;
	    Identifier m_theGap;
	    std::vector<RpcCalibData*> m_thePhiData;
	    std::vector<RpcCalibData*> m_theEtaData;
	    
    };

}

#endif
