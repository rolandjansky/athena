/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//                           jFEXOutputCollection.h  -
//                              -------------------
//     begin                : 09 12 2020
//     email                : varsiha.sothilingam@cern.ch
//  **************************************************************************


#ifndef JFEX_OUTPUT_COLLECTION_H
#define JFEX_OUTPUT_COLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "StoreGate/StoreGateSvc.h"
#include <map>
#include <iostream>
#include <vector>
#include <string>

namespace LVL1 {
class jFEXOutputCollection
{
    public:
        jFEXOutputCollection();
        ~jFEXOutputCollection();
        void clear();
        void addValue_smallRJet(std::string, int);
        void fill_smallRJet();
        void addValue_largeRJet(std::string, int);
        void fill_largeRJet();
        void addValue_tau(std::string, int);
        void fill_tau();
        void addValue_pileup(std::string, int);
        void addValue_pileup(std::string, std::vector<int> );
        void fill_pileup();
        int SRJetSize() const;
        int LRJetSize() const;
        int TauSize() const;
        int PileupSize() const;
        int PileupEtSize() const;
        void setdooutput(bool);
        bool getdooutput() const;
        int get_smallRJet(int,std::string) const;
        int get_largeRJet(int,std::string) const;
        int get_tau(int,std::string) const;
        int get_pileup(int,std::string) const;
        std::vector<int> get_pileup_map(int,std::string) const;
    private:
        bool m_dooutput;
        std::unordered_map<std::string, int> m_values_tem_smallRJet;
        std::vector< std::unique_ptr<std::unordered_map<std::string, int>> > m_allvalues_smallRJet;
        std::unordered_map<std::string, int> m_values_tem_largeRJet;
        std::vector<std::unique_ptr<std::unordered_map<std::string, int>> > m_allvalues_largeRJet;
        std::unordered_map<std::string, int> m_values_tem_tau;
        std::vector<std::unique_ptr<std::unordered_map<std::string, int>> > m_allvalues_tau;
        std::unordered_map<std::string, int> m_values_tem_pileup;
        std::vector< std::unique_ptr<std::unordered_map<std::string, int>> > m_allvalues_pileup;
        std::unordered_map<std::string, std::vector<int> > m_values_tem_pileup_maps;
        std::vector< std::unique_ptr<std::unordered_map<std::string, std::vector<int> >> > m_allvalues_pileup_map;
};
}
CLASS_DEF(LVL1::jFEXOutputCollection, 1317184196 , 1 )
#endif
