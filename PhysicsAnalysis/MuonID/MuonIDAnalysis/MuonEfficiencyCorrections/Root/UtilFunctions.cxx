/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"

namespace CP{

    std::string RandomString(size_t length) {
        auto randchar = []() -> char {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }
    std::string ReplaceExpInString(std::string str, const std::string &exp, const std::string &rep) {
        size_t ExpPos = str.find(exp);
        if (ExpPos == std::string::npos) return str;
        size_t ExpLen = exp.size();

        str = str.substr(0, ExpPos) + rep + str.substr(ExpPos + ExpLen, std::string::npos);
        if (str.find(exp) != std::string::npos) return ReplaceExpInString(str, exp, rep);
        return str;
    }
   std::string EraseWhiteSpaces(std::string str) {
        str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
        if (str.find(" ") == 0) return EraseWhiteSpaces(str.substr(1, str.size()));
        if (str.size() > 0 && str.find(" ") == str.size() - 1) return EraseWhiteSpaces(str.substr(0, str.size() - 1));
        return str;
    }
    std::string LowerString(const std::string &str) {
        std::string Low;
        std::locale loc;
        for (std::string::size_type i = 0; i < str.length(); ++i) Low += std::tolower(str[i], loc);
        return Low;
    }
    std::unique_ptr<TH1> clone(TH1* H){
        if (!H){
            Warning("clone()", "Nullptr given... Segmentation ahead");
            return std::unique_ptr<TH1>();
        }
        // We do not really care about the name of the histogram at this stage... At only needs 
        // to be cloned
        std::unique_ptr<TH1> ptr ( dynamic_cast<TH1*>(H->Clone(RandomString(10).c_str())));
        ptr->SetDirectory(0);
        return ptr;
    }
    bool isOverflowBin(const TH1 *Histo, int bin) {
        int x(-1), y(-1), z(-1);
        if (Histo == nullptr){
            Warning("isOverflowBin()", "Where is my histogram?");
            return true;
        }
        Histo->GetBinXYZ(bin, x, y, z);
        if (x == 0 || x == Histo->GetXaxis()->GetNbins() + 1) return true;
        if (Histo->GetDimension() >= 2 && (y == 0 || y == Histo->GetYaxis()->GetNbins() + 1)) return true;
        if (Histo->GetDimension() == 3 && (z == 0 || z == Histo->GetZaxis()->GetNbins() + 1)) return true;
        return false;
    }

}
