/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#  include "TTreeFormulaManager.h"

#include "APRTreeFormula.h"
#include "POOLCore/Exception.h"

#include "TBranchElement.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFormLeafInfo.h"
#include "TMethodCall.h"
#include "TEntryList.h"

#include "TCutG.h"
#include "TRandom.h"
#include "TAxis.h"
#include "TMath.h"

#include <cstring>
#include <cmath>
#include <sstream>
#include <cassert>
//#include <iostream>
using namespace std;

APRTreeFormula::~APRTreeFormula() {
   delete[] m_aprConst;
}


void    APRTreeFormula::readConstants()
{
   // we may need more space than fNconst in case Tformula discarded some constants as duplicates due to loss of precision
   // using "oper" is index to constants
   m_aprConst = new LDouble_t[fNoper];
   
   for (Int_t i=0; i<fNoper ; ++i) {
      const Int_t oper = GetOper()[i];
      if( (oper >> kTFOperShift) == kConstant ) {
         // copy the value as read by TFormula
         m_aprConst[i] = fConst[oper & kTFOperMask];
         // check for large integers and re-read them with bigger precision
         bool exit = false;
         const char* numstr = fExpr[i];
         for( size_t x=0; x<strlen(numstr); x++) {
            if( !std::strchr("0123456789abcdefABCDEFxX", numstr[x]) ) {
               // not a clean integer
               exit = true;
               break;
            }
         }            
         if( strlen(numstr) < 3 || exit ) {
            // only interested in really big ints
            continue;
         }
         if( numstr[0] == '0' && ( std::toupper(numstr[1]) == 'X' ) ) {
            // hex.  use sscanf - could not make streamstring read hex (???) MN
            unsigned long long lval(0);
            sscanf(numstr, "%80llx", &lval);
            m_aprConst[i] = lval;
         } else {
            std::stringstream ss(numstr);
            ss >> m_aprConst[i]; 
         }
      }
   }
}
            
namespace {
   typedef APRTreeFormula::LDouble_t LDouble_t;
   
   LDouble_t Summing(TTreeFormula *sum) {
      Int_t len = sum->GetNdata();
      LDouble_t res = 0;
      for (int i=0; i<len; ++i) res += sum->EvalInstance(i);
      return res;
   }
   LDouble_t FindMin(TTreeFormula *arr) {
      Int_t len = arr->GetNdata();
      LDouble_t res = 0;
      if (len) {
         res = arr->EvalInstance(0);
         for (int i=1; i<len; ++i) {
            LDouble_t val = arr->EvalInstance(i);
            if (val < res) {
               res = val;
            }
         }
      }
      return res;
   }
   LDouble_t FindMax(TTreeFormula *arr) {
      Int_t len = arr->GetNdata();
      LDouble_t res = 0;
      if (len) {
         res = arr->EvalInstance(0);
         for (int i=1; i<len; ++i) {
            LDouble_t val = arr->EvalInstance(i);
            if (val > res) {
               res = val;
            }
         }
      }
      return res;
   }
   LDouble_t FindMin(TTreeFormula *arr, TTreeFormula *condition) {
      Int_t len = arr->GetNdata();
      LDouble_t res = 0;
      if (len) {
         int i = 0;
         LDouble_t condval;
         do {
            condval = condition->EvalInstance(i);
            ++i;
         } while (!condval && i<len);
         if (i==len) {
            return 0;
         }
         if (i!=1) {
            // Insure the loading of the branch.
            arr->EvalInstance(0);
         }
         // Now we know that i>0 && i<len and cond==true
         res = arr->EvalInstance(i-1);
         for (; i<len; ++i) {
            condval = condition->EvalInstance(i);
            if (condval) {
               LDouble_t val = arr->EvalInstance(i);
               if (val < res) {
                  res = val;
               }
            }
         }
      }
      return res;
   }
   LDouble_t FindMax(TTreeFormula *arr, TTreeFormula *condition) {
      Int_t len = arr->GetNdata();
      LDouble_t res = 0;
      if (len) {
         int i = 0;
         LDouble_t condval;
         do {
            condval = condition->EvalInstance(i);
            ++i;
         } while (!condval && i<len);
         if (i==len) {
            return 0;
         }
         if (i!=1) {
            // Insure the loading of the branch.
            arr->EvalInstance(0);
         }
         // Now we know that i>0 && i<len and cond==true
         res = arr->EvalInstance(i-1);
         for (; i<len; ++i) {
            condval = condition->EvalInstance(i);
            if (condval) {
               LDouble_t val = arr->EvalInstance(i);
               if (val > res) {
                  res = val;
               }
            }
         }
      }
      return res;
   }
}



#define TT_EVAL_INIT                                                                            \
   TLeaf *leaf = (TLeaf*)fLeaves.UncheckedAt(0);                                                \
                                                                                                \
   const Int_t real_instance = GetRealInstance(instance,0);                                     \
                                                                                                \
   if (instance==0) fNeedLoading = kTRUE;                                                       \
   if (real_instance>fNdata[0]) return 0;                                                       \
                                                                                                \
   /* Since the only operation in this formula is reading this branch,                          \
      we are guaranteed that this function is first called with instance==0 and                 \
      hence we are guaranteed that the branch is always properly read */                        \
                                                                                                \
   if (fNeedLoading) {                                                                          \
      fNeedLoading = kFALSE;                                                                    \
      TBranch *br = leaf->GetBranch();                                                          \
      Long64_t tentry = br->GetTree()->GetReadEntry();                                          \
      R__LoadBranch(br,tentry,fQuickLoad);                                                      \
   }                                                                                            \
                                                                                                \
   if (fAxis) {                                                                                 \
      char * label;                                                                             \
      /* This portion is a duplicate (for speed reason) of the code                             \
         located  in the main for loop at "a tree string" (and in EvalStringInstance) */        \
      if (fLookupType[0]==kDirect) {                                                            \
         label = (char*)leaf->GetValuePointer();                                                \
      } else {                                                                                  \
         label = (char*)GetLeafInfo(0)->GetValuePointer(leaf,instance);                         \
      }                                                                                         \
      Int_t bin = fAxis->FindBin(label);                                                        \
      return bin-0.5;                                                                           \
   }

#define TREE_EVAL_INIT                                                                          \
   const Int_t real_instance = GetRealInstance(instance,0);                                     \
                                                                                                \
   if (real_instance>fNdata[0]) return 0;                                                       \
                                                                                                \
   if (fAxis) {                                                                                 \
      char * label;                                                                             \
      /* This portion is a duplicate (for speed reason) of the code                             \
         located  in the main for loop at "a tree string" (and in EvalStringInstance) */        \
      label = (char*)GetLeafInfo(0)->GetValuePointer((TLeaf*)0x0,instance);                     \
      Int_t bin = fAxis->FindBin(label);                                                        \
      return bin-0.5;                                                                           \
   }

#define TT_EVAL_INIT_LOOP                                                                       \
   TLeaf *leaf = (TLeaf*)fLeaves.UncheckedAt(code);                                             \
                                                                                                \
   /* Now let calculate what physical instance we really need.  */                              \
   const Int_t real_instance = GetRealInstance(instance,code);                                  \
                                                                                                \
   if (willLoad) {                                                                              \
      TBranch *branch = (TBranch*)fBranches.UncheckedAt(code);                                  \
      if (branch) {                                                                             \
         Long64_t treeEntry = branch->GetTree()->GetReadEntry();                                \
         R__LoadBranch(branch,treeEntry,fQuickLoad);                                            \
      } else if (fDidBooleanOptimization) {                                                     \
         branch = leaf->GetBranch();                                                            \
         Long64_t treeEntry = branch->GetTree()->GetReadEntry();                                \
         if (branch->GetReadEntry() != treeEntry) branch->GetEntry( treeEntry );                \
      }                                                                                         \
   } else {                                                                                     \
      /* In the cases where we are behind (i.e. right of) a potential boolean optimization      \
         this tree variable reading may have not been executed with instance==0 which would     \
         result in the branch being potentially not read in. */                                 \
      if (fDidBooleanOptimization) {                                                            \
         TBranch *br = leaf->GetBranch();                                                       \
         Long64_t treeEntry = br->GetTree()->GetReadEntry();                                    \
         if (br->GetReadEntry() != treeEntry) br->GetEntry( treeEntry );                        \
      }                                                                                         \
   }                                                                                            \
   if (real_instance>fNdata[code]) return 0;

#define TREE_EVAL_INIT_LOOP                                                                     \
   /* Now let calculate what physical instance we really need.  */                              \
   const Int_t real_instance = GetRealInstance(instance,code);                                  \
                                                                                                \
   if (real_instance>fNdata[code]) return 0;



inline static void R__LoadBranch(TBranch* br, Long64_t entry, Bool_t quickLoad)
{
   if (!quickLoad || (br->GetReadEntry() != entry)) {
      br->GetEntry(entry);
   }
}



Double_t APRTreeFormula::EvalInstance(Int_t instance, const char *stringStackArg[])
{
   if (TestBit(kMissingLeaf)) return 0;

   if (fNoper == 1 && fNcodes > 0) {
      switch (fLookupType[0]) {
         case kDirect:     {
            TT_EVAL_INIT;
            LDouble_t result = leaf->GetValue(real_instance);
            return result;
         }
         case kMethod:     {
            TT_EVAL_INIT;
            ((TFormLeafInfo*)fDataMembers.UncheckedAt(0))->SetBranch(leaf->GetBranch());
            return GetValueFromMethod(0,leaf);
         }
         case kDataMember: {
            TT_EVAL_INIT;
            ((TFormLeafInfo*)fDataMembers.UncheckedAt(0))->SetBranch(leaf->GetBranch());
            return ((TFormLeafInfo*)fDataMembers.UncheckedAt(0))->GetValue(leaf,real_instance);
         }
         case kTreeMember: {
            TREE_EVAL_INIT;
            return ((TFormLeafInfo*)fDataMembers.UncheckedAt(0))->GetValue((TLeaf*)0x0,real_instance);
         }
         case kIndexOfEntry: return (LDouble_t)fTree->GetReadEntry();
         case kIndexOfLocalEntry: return (LDouble_t)fTree->GetTree()->GetReadEntry();
         case kEntries:      return (LDouble_t)fTree->GetEntries();
         case kLength:       return fManager->GetNdata();
         case kLengthFunc:   return ((TTreeFormula*)fAliases.UncheckedAt(0))->GetNdata();
         case kIteration:    return instance;
         case kSum:          return Summing((TTreeFormula*)fAliases.UncheckedAt(0));
         case kMin:          return FindMin((TTreeFormula*)fAliases.UncheckedAt(0));
         case kMax:          return FindMax((TTreeFormula*)fAliases.UncheckedAt(0));
         case kEntryList: {
            TEntryList *elist = (TEntryList*)fExternalCuts.At(0);
            return elist->Contains(fTree->GetTree()->GetReadEntry());
         }
         case -1: break;
      }
      switch (fCodes[0]) {
         case -2: {
            TCutG *gcut = (TCutG*)fExternalCuts.At(0);
            TTreeFormula *fx = (TTreeFormula *)gcut->GetObjectX();
            TTreeFormula *fy = (TTreeFormula *)gcut->GetObjectY();
            LDouble_t xcut = fx->EvalInstance(instance);
            LDouble_t ycut = fy->EvalInstance(instance);
            return gcut->IsInside(xcut,ycut);
         }
         case -1: {
            TCutG *gcut = (TCutG*)fExternalCuts.At(0);
            TTreeFormula *fx = (TTreeFormula *)gcut->GetObjectX();
            return fx->EvalInstance(instance);
         }
         default: return 0;
      }
   }

   LDouble_t tab[kMAXFOUND] = {};
   const Int_t kMAXSTRINGFOUND = 10;
   const char *stringStackLocal[kMAXSTRINGFOUND];
   const char **stringStack = stringStackArg?stringStackArg:stringStackLocal;

   const bool willLoad = (instance==0 || fNeedLoading); fNeedLoading = kFALSE;
   if (willLoad) fDidBooleanOptimization = kFALSE;

   Int_t pos  = 0;
   Int_t pos2 = 0;
   for (Int_t i=0; i<fNoper ; ++i) {

      const Int_t oper = GetOper()[i];
      const Int_t newaction = oper >> kTFOperShift;

      if (newaction<kDefinedVariable) {
         // TFormula operands.

         // one of the most used cases
         // m_aprConst[] is indexed by "i"!!   (see readConstants())
         if (newaction==kConstant) { tab[pos++] = m_aprConst[i]; continue; }
         
         switch(newaction) {
            case kEnd        : return tab[0];
            case kAdd        : pos--; tab[pos-1] += tab[pos]; continue;
            case kSubstract  : pos--; tab[pos-1] -= tab[pos]; continue;
            case kMultiply   : pos--; tab[pos-1] *= tab[pos]; continue;
            case kDivide     : pos--; if (tab[pos] == 0) tab[pos-1] = 0; //  division by 0
                                      else               tab[pos-1] /= tab[pos];
                                      continue;
            case kModulo     : {pos--;
                                Long64_t int1((Long64_t)tab[pos-1]);
                                Long64_t int2((Long64_t)tab[pos]);
                                tab[pos-1] = LDouble_t(int1%int2);
                                continue;}

            case kcos  : tab[pos-1] = TMath::Cos(tab[pos-1]); continue;
            case ksin  : tab[pos-1] = TMath::Sin(tab[pos-1]); continue;
            case ktan  : if (TMath::Cos(tab[pos-1]) == 0) {tab[pos-1] = 0;} // { tangente indeterminee }
                         else tab[pos-1] = TMath::Tan(tab[pos-1]);
                         continue;
            case kacos : if (TMath::Abs((Double_t)tab[pos-1]) > 1) {tab[pos-1] = 0;} //  indetermination
                         else tab[pos-1] = TMath::ACos(tab[pos-1]);
                         continue;
            case kasin : if (TMath::Abs((Double_t)tab[pos-1]) > 1) {tab[pos-1] = 0;} //  indetermination
                         else tab[pos-1] = TMath::ASin(tab[pos-1]);
                         continue;
            case katan : tab[pos-1] = TMath::ATan(tab[pos-1]); continue;
            case kcosh : tab[pos-1] = TMath::CosH(tab[pos-1]); continue;
            case ksinh : tab[pos-1] = TMath::SinH(tab[pos-1]); continue;
            case ktanh : if (TMath::CosH(tab[pos-1]) == 0) {tab[pos-1] = 0;} // { tangente indeterminee }
                         else tab[pos-1] = TMath::TanH(tab[pos-1]);
                         continue;
            case kacosh: if (tab[pos-1] < 1) {tab[pos-1] = 0;} //  indetermination
                         else tab[pos-1] = TMath::ACosH(tab[pos-1]);
                         continue;
            case kasinh: tab[pos-1] = TMath::ASinH(tab[pos-1]); continue;
            case katanh: if (TMath::Abs((Double_t)tab[pos-1]) > 1) {tab[pos-1] = 0;} // indetermination
                     else tab[pos-1] = TMath::ATanH(tab[pos-1]);
                     continue;
            case katan2: pos--; tab[pos-1] = TMath::ATan2(tab[pos-1],tab[pos]); continue;

            case kfmod : pos--; tab[pos-1] = fmod(tab[pos-1],tab[pos]); continue;
            case kpow  : pos--; tab[pos-1] = TMath::Power((Double_t)tab[pos-1],(Double_t)tab[pos]); continue;
            case ksq   : tab[pos-1] = tab[pos-1]*tab[pos-1]; continue;
            case ksqrt : tab[pos-1] = TMath::Sqrt(TMath::Abs((Double_t)tab[pos-1])); continue;

            case kstrstr : pos2 -= 2; pos++;if (strstr(stringStack[pos2],stringStack[pos2+1])) tab[pos-1]=1;
                                        else tab[pos-1]=0;
                                        continue;

            case kmin : pos--; tab[pos-1] = std::min(tab[pos-1],tab[pos]); continue;
            case kmax : pos--; tab[pos-1] = std::max(tab[pos-1],tab[pos]); continue;

            case klog  : if (tab[pos-1] > 0) tab[pos-1] = TMath::Log(tab[pos-1]);
                         else {tab[pos-1] = 0;} //{indetermination }
                          continue;
            case kexp  : { LDouble_t dexp = tab[pos-1];
                           if (dexp < -700) {tab[pos-1] = 0; continue;}
                           if (dexp >  700) {tab[pos-1] = TMath::Exp(700); continue;}
                           tab[pos-1] = TMath::Exp(dexp); continue;
                         }
            case klog10: if (tab[pos-1] > 0) tab[pos-1] = TMath::Log10(tab[pos-1]);
                         else {tab[pos-1] = 0;} //{indetermination }
                         continue;

            case kpi   : pos++; tab[pos-1] = TMath::ACos(-1); continue;

            case kabs  : tab[pos-1] = TMath::Abs((Double_t)tab[pos-1]); continue;
            case ksign : if (tab[pos-1] < 0) tab[pos-1] = -1; else tab[pos-1] = 1; continue;
            case kint  : tab[pos-1] = LDouble_t(Int_t(tab[pos-1])); continue;
            case kSignInv: tab[pos-1] = -1 * tab[pos-1]; continue;
            case krndm : //pos++; tab[pos-1] = gRandom->Rndm(1); continue;
              // Using gRandom is not thread-safe.
              // ATLAS doesn't use this code, so just give an error
              // rather than trying to fix it.
              throw pool::Exception ("Rndm() not supported",
                                     "APRTreeFormula::EvalInstance",
                                     "APRTreeFormula");

            case kAnd  : pos--; if (tab[pos-1]!=0 && tab[pos]!=0) tab[pos-1]=1;
                                else tab[pos-1]=0;
                                continue;
            case kOr   : pos--; if (tab[pos-1]!=0 || tab[pos]!=0) tab[pos-1]=1;
                                else tab[pos-1]=0;
                                continue;

            case kEqual      : pos--; tab[pos-1] = (tab[pos-1] == tab[pos]) ? 1 : 0; continue;
            case kNotEqual   : pos--; tab[pos-1] = (tab[pos-1] != tab[pos]) ? 1 : 0; continue;
            case kLess       : pos--; tab[pos-1] = (tab[pos-1] <  tab[pos]) ? 1 : 0; continue;
            case kGreater    : pos--; tab[pos-1] = (tab[pos-1] >  tab[pos]) ? 1 : 0; continue;
            case kLessThan   : pos--; tab[pos-1] = (tab[pos-1] <= tab[pos]) ? 1 : 0; continue;
            case kGreaterThan: pos--; tab[pos-1] = (tab[pos-1] >= tab[pos]) ? 1 : 0; continue;
            case kNot        :        tab[pos-1] = (tab[pos-1] !=        0) ? 0 : 1; continue;

            case kStringEqual : pos2 -= 2; pos++; if (!strcmp(stringStack[pos2+1],stringStack[pos2])) tab[pos-1]=1;
                                                  else tab[pos-1]=0;
                                                  continue;
            case kStringNotEqual: pos2 -= 2; pos++;if (strcmp(stringStack[pos2+1],stringStack[pos2])) tab[pos-1]=1;
                                                   else tab[pos-1]=0;
                                                   continue;

            case kBitAnd    : pos--; tab[pos-1]= ((ULong64_t) tab[pos-1]) & ((ULong64_t) tab[pos]); continue;
            case kBitOr     : pos--; tab[pos-1]= ((ULong64_t) tab[pos-1]) | ((ULong64_t) tab[pos]); continue;
            case kLeftShift : pos--; tab[pos-1]= ((ULong64_t) tab[pos-1]) <<((Long64_t) tab[pos]); continue;
            case kRightShift: pos--; tab[pos-1]= ((ULong64_t) tab[pos-1]) >>((Long64_t) tab[pos]); continue;

            case kJump   : i = (oper & kTFOperMask); continue;
            case kJumpIf : pos--; if (!tab[pos]) i = (oper & kTFOperMask); continue;

            case kStringConst: {
               // String
               pos2++; stringStack[pos2-1] = (char*)fExpr[i].Data();
               continue;
            }

            case kBoolOptimize: {
               // boolean operation optimizer

               int param = (oper & kTFOperMask);
               Bool_t skip = kFALSE;
               int op = param % 10; // 1 is && , 2 is ||

               if (op == 1 && (!tab[pos-1]) ) {
                  // &&: skip the right part if the left part is already false

                  skip = kTRUE;

                  // Preserve the existing behavior (i.e. the result of a&&b is
                  // either 0 or 1)
                  tab[pos-1] = 0;

               } else if (op == 2 && tab[pos-1] ) {
                  // ||: skip the right part if the left part is already true

                  skip = kTRUE;

                  // Preserve the existing behavior (i.e. the result of a||b is
                  // either 0 or 1)
                  tab[pos-1] = 1;
               }

               if (skip) {
                  int toskip = param / 10;
                  i += toskip;
                  if (willLoad) fDidBooleanOptimization = kTRUE;
              }
               continue;
            }

            case kFunctionCall: {
               // an external function call

               int param = (oper & kTFOperMask);
               int fno   = param / 1000;
               int nargs = param % 1000;

               // Retrieve the function
               TMethodCall *method = (TMethodCall*)fFunctions.At(fno);

               // Set the arguments
               method->ResetParam();
               if (nargs) {
                  UInt_t argloc = pos-nargs;
                  for(Int_t j=0;j<nargs;j++,argloc++,pos--) {
                     method->SetParam( (Double_t)tab[argloc] );
                  }
               }
               pos++;
               Double_t ret = 0;
               method->Execute(ret);
               tab[pos-1] = ret; // check for the correct conversion!

               continue;
            }

//         case kParameter:    { pos++; tab[pos-1] = fParams[(oper & kTFOperMask)]; continue; }
         }

      } else {
         // TTreeFormula operands.

         // a tree variable (the most used case).

         if (newaction == kDefinedVariable) {

            const Int_t code = (oper & kTFOperMask);
            const Int_t lookupType = fLookupType[code];

            switch (lookupType) {
               case kIndexOfEntry: tab[pos++] = (LDouble_t)fTree->GetReadEntry(); continue;
               case kIndexOfLocalEntry: tab[pos++] = (LDouble_t)fTree->GetTree()->GetReadEntry(); continue;
               case kEntries:      tab[pos++] = (LDouble_t)fTree->GetEntries(); continue;
               case kLength:       tab[pos++] = fManager->GetNdata(); continue;
               case kLengthFunc:   tab[pos++] = ((TTreeFormula*)fAliases.UncheckedAt(i))->GetNdata(); continue;
               case kIteration:    tab[pos++] = instance; continue;
               case kSum:          tab[pos++] = Summing((TTreeFormula*)fAliases.UncheckedAt(i)); continue;
               case kMin:          tab[pos++] = FindMin((TTreeFormula*)fAliases.UncheckedAt(i)); continue;
               case kMax:          tab[pos++] = FindMax((TTreeFormula*)fAliases.UncheckedAt(i)); continue;
               case kDirect:     { TT_EVAL_INIT_LOOP;
                      if( !strcmp(leaf->GetTypeName(), "ULong64_t") ) {
                         tab[pos++]= *( (ULong64_t*)leaf->GetValuePointer() + real_instance);
                      } else if( !strcmp(leaf->GetTypeName(), "Long64_t") ) {
                         tab[pos++]= *( (Long64_t*)leaf->GetValuePointer() + real_instance);
                      } else {
                         tab[pos++] = leaf->GetValue(real_instance);
                      }
                      continue; }
               case kMethod:     { TT_EVAL_INIT_LOOP; tab[pos++] = GetValueFromMethod(code,leaf); continue; }
               case kDataMember: { TT_EVAL_INIT_LOOP; tab[pos++] = ((TFormLeafInfo*)fDataMembers.UncheckedAt(code))->
                                          GetValue(leaf,real_instance); continue; }
               case kTreeMember: { TREE_EVAL_INIT_LOOP; tab[pos++] = ((TFormLeafInfo*)fDataMembers.UncheckedAt(code))->
                                          GetValue((TLeaf*)0x0,real_instance); continue; }
               case kEntryList: { TEntryList *elist = (TEntryList*)fExternalCuts.At(code);
                  tab[pos++] = elist->Contains(fTree->GetReadEntry());
                  continue;}
               case -1: break;
               default: tab[pos++] = 0; continue;
            }
            switch (fCodes[code]) {
               case -2: {
                  TCutG *gcut = (TCutG*)fExternalCuts.At(code);
                  TTreeFormula *fx = (TTreeFormula *)gcut->GetObjectX();
                  TTreeFormula *fy = (TTreeFormula *)gcut->GetObjectY();
                  LDouble_t xcut = fx->EvalInstance(instance);
                  LDouble_t ycut = fy->EvalInstance(instance);
                  tab[pos++] = gcut->IsInside(xcut,ycut);
                  continue;
               }
               case -1: {
                  TCutG *gcut = (TCutG*)fExternalCuts.At(code);
                  TTreeFormula *fx = (TTreeFormula *)gcut->GetObjectX();
                  tab[pos++] = fx->EvalInstance(instance);
                  continue;
               }
               default: {
                  tab[pos++] = 0;
                  continue;
               }
            }
         }
         switch(newaction) {

            // a TTree Variable Alias (i.e. a sub-TTreeFormula)
            case kAlias: {
               int aliasN = i;
               TTreeFormula *subform = static_cast<TTreeFormula*>(fAliases.UncheckedAt(aliasN));
               assert(subform);

               LDouble_t param = subform->EvalInstance(instance);

               tab[pos] = param; pos++;
               continue;
            }
            // a TTree Variable Alias String (i.e. a sub-TTreeFormula)
            case kAliasString: {
               int aliasN = i;
               TTreeFormula *subform = static_cast<TTreeFormula*>(fAliases.UncheckedAt(aliasN));
               assert(subform);

               pos2++;
               stringStack[pos2-1] = subform->EvalStringInstance(instance);
               continue;
            }
            case kMinIf: {
               int alternateN = i;
               TTreeFormula *primary = static_cast<TTreeFormula*>(fAliases.UncheckedAt(alternateN));
               TTreeFormula *condition = static_cast<TTreeFormula*>(fAliases.UncheckedAt(alternateN+1));
               LDouble_t param = FindMin(primary,condition);
               ++i; // skip the place holder for the condition
               tab[pos] = param; pos++;
               continue;
            }
            case kMaxIf: {
               int alternateN = i;
               TTreeFormula *primary = static_cast<TTreeFormula*>(fAliases.UncheckedAt(alternateN));
               TTreeFormula *condition = static_cast<TTreeFormula*>(fAliases.UncheckedAt(alternateN+1));
               LDouble_t param = FindMax(primary,condition);
               ++i; // skip the place holder for the condition
               tab[pos] = param; pos++;
               continue;
            }
               
            // a TTree Variable Alternate (i.e. a sub-TTreeFormula)
            case kAlternate: {
               int alternateN = i;
               TTreeFormula *primary = static_cast<TTreeFormula*>(fAliases.UncheckedAt(alternateN));

               // First check whether we are in range for the primary formula
               if (instance < primary->GetNdata()) {

                  LDouble_t param = primary->EvalInstance(instance);

                  ++i; // skip the alternate value.

                  tab[pos] = param; pos++;
               } else {
                  // The primary is not in rancge, we will calculate the alternate value
                  // via the next operation (which will be a intentional).

                  // kAlias no operations
               }
               continue;
            }
            case kAlternateString: {
               int alternateN = i;
               TTreeFormula *primary = static_cast<TTreeFormula*>(fAliases.UncheckedAt(alternateN));

               // First check whether we are in range for the primary formula
               if (instance < primary->GetNdata()) {

                  pos2++;
                  stringStack[pos2-1] = primary->EvalStringInstance(instance);

                  ++i; // skip the alternate value.

               } else {
                  // The primary is not in rancge, we will calculate the alternate value
                  // via the next operation (which will be a kAlias).

                  // intentional no operations
               }
               continue;
            }

            // a tree string
            case kDefinedString: {
               Int_t string_code = (oper & kTFOperMask);
               TLeaf *leafc = (TLeaf*)fLeaves.UncheckedAt(string_code);

               // Now let calculate what physical instance we really need.
               const Int_t real_instance = GetRealInstance(instance,string_code);

               if (instance==0 || fNeedLoading) {
                  fNeedLoading = kFALSE;
                  TBranch *branch = leafc->GetBranch();
                  Long64_t readentry = branch->GetTree()->GetReadEntry();
                  R__LoadBranch(branch,readentry,fQuickLoad);
               } else {
                  // In the cases where we are behind (i.e. right of) a potential boolean optimization
                  // this tree variable reading may have not been executed with instance==0 which would
                  // result in the branch being potentially not read in.
                  if (fDidBooleanOptimization) {
                     TBranch *br = leafc->GetBranch();
                     Long64_t treeEntry = br->GetTree()->GetReadEntry();
                     R__LoadBranch(br,treeEntry,kTRUE);
                  }
                  if (real_instance>fNdata[string_code]) return 0;
               }
               pos2++;
               if (fLookupType[string_code]==kDirect) {
                  stringStack[pos2-1] = (char*)leafc->GetValuePointer();
               } else {
                  stringStack[pos2-1] = (char*)GetLeafInfo(string_code)->GetValuePointer(leafc,real_instance);
               }
               continue;
            }

         }
      }

      assert(i<fNoper);
   }

   Double_t result = tab[0];
   return result;
}
