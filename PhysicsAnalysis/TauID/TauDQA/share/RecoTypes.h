
#ifndef RECOTYPES_H_
#define RECOTYPES_H_

#include <map>

#include "TString.h"


class Comp_Tau;
class Comp_TauFromAlg;

namespace RecoTypes {
    
    enum DecayMode {
        t_1p0n,
        t_1p1n,
        t_1pXn,
        t_3p0n,
        t_3pXn,
        t_UnconsideredTrueMode,
        t_OtherTrueMode,
        t_PolarizeModes,
        t_Any1p,
        t_Any3p,
        t_Any,
        t_AllModes,
        t_nTypes
    };
    
    
    inline std::map<RecoTypes::DecayMode, TString> RecoTypeLabelsShort() {
        std::map<RecoTypes::DecayMode, TString> res;
        res[RecoTypes::t_1p0n] = "1p0n";
        res[RecoTypes::t_1p1n] = "1p1n";
        res[RecoTypes::t_1pXn] = "1pXn";
        res[RecoTypes::t_3p0n] = "3p0n";
        res[RecoTypes::t_3pXn] = "3pXn";
        res[RecoTypes::t_UnconsideredTrueMode]   = "UnconsTrueMode";
        res[RecoTypes::t_OtherTrueMode] = "Unkown";
        res[RecoTypes::t_PolarizeModes] = "AllImpModes";
        res[RecoTypes::t_Any1p] = "1p";
        res[RecoTypes::t_Any3p] = "3p";
        res[RecoTypes::t_Any]   = "1p3p";
        res[RecoTypes::t_AllModes] = "AllModes";
        return res;
    }
    
    
    inline std::map<RecoTypes::DecayMode, TString> RecoTypeLabelsLatex() {
        std::map<RecoTypes::DecayMode, TString> res;
        res[RecoTypes::t_1p0n] = "1p0n";
        res[RecoTypes::t_1p1n] = "1p1n";
        res[RecoTypes::t_1pXn] = "1pXn";
        res[RecoTypes::t_3p0n] = "3p0n";
        res[RecoTypes::t_3pXn] = "3pXn";
        res[RecoTypes::t_UnconsideredTrueMode]   = "Unconsidered true mode";
        res[RecoTypes::t_OtherTrueMode] = "Unkown";
        res[RecoTypes::t_PolarizeModes] = "1p0n, 1p1n or 3p0n";
        res[RecoTypes::t_Any1p] = "1p";
        res[RecoTypes::t_Any3p] = "3p";
        res[RecoTypes::t_Any]   = "1p or 3p";        
        res[RecoTypes::t_AllModes] = "All Modes";
        return res;
    }
    
    
    inline TString          GetLabelShort(DecayMode recType) {
        return RecoTypeLabelsShort()[recType];
    }
    
    inline TString          GetLabelLatex(DecayMode recType) {
        return RecoTypeLabelsLatex()[recType];
    }
    
    inline RecoTypes::DecayMode GetDecayModeInteger(TString shortLabel) {
        if(shortLabel == "1p0n") return RecoTypes::t_1p0n;
        if(shortLabel == "1p1n") return RecoTypes::t_1p1n;
        if(shortLabel == "1pXn") return RecoTypes::t_1pXn;
        if(shortLabel == "3p0n") return RecoTypes::t_3p0n;
        if(shortLabel == "3pXn") return RecoTypes::t_3pXn;
        if(shortLabel == "UnconsTrueMode")   return RecoTypes::t_UnconsideredTrueMode;
        if(shortLabel == "AllImpModes")   return RecoTypes::t_PolarizeModes;
        if(shortLabel == "Any1p") return RecoTypes::t_Any1p;
        if(shortLabel == "Any3p") return RecoTypes::t_Any3p;
        if(shortLabel == "AnyMode") return RecoTypes::t_Any;
        if(shortLabel == "AllModes")   return RecoTypes::t_AllModes;
        return RecoTypes::t_OtherTrueMode;
    }
    
    inline RecoTypes::DecayMode  GetDecayMode(int nChrg, int nNeut) {
        if(nChrg == 1 && nNeut == 0) return RecoTypes::t_1p0n;
        if(nChrg == 1 && nNeut == 1) return RecoTypes::t_1p1n;
        if(nChrg == 1 && nNeut >= 2) return RecoTypes::t_1pXn;
        if(nChrg == 3 && nNeut == 0) return RecoTypes::t_3p0n;
        if(nChrg == 3 && nNeut >= 1) return RecoTypes::t_3pXn;
        if(nChrg >= 5)               return RecoTypes::t_UnconsideredTrueMode;
        return RecoTypes::t_OtherTrueMode;
    }
    
    inline bool                 IsValidMode(RecoTypes::DecayMode mode) {
        if(mode == RecoTypes::t_1p0n) return true;
        if(mode == RecoTypes::t_1p1n) return true;
        if(mode == RecoTypes::t_1pXn) return true;
        if(mode == RecoTypes::t_3p0n) return true;
        if(mode == RecoTypes::t_3pXn) return true;
        return false;
    }
    
    //in cxx file
//     RecoTypes::DecayMode GetTrueMode(Comp_Tau* trueTau);
//     RecoTypes::DecayMode GetRecoMode(Comp_TauFromAlg* recoTau);
    
}

#endif













