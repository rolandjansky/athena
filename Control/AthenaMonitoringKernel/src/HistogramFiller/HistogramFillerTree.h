/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerTree_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerTree_h

#include "TTree.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include <boost/algorithm/string.hpp>

namespace Monitored {
  template <typename T> void scalarFillerFunc(TBranch* branch, const IMonitoredVariable& var);
  template <> void scalarFillerFunc<std::string>(TBranch* branch, const IMonitoredVariable& var);
  template <typename T> void vectorFillerFunc(TBranch* branch, const IMonitoredVariable& var);
  template <> void vectorFillerFunc<std::string>(TBranch* branch, const IMonitoredVariable& var);

  /**
   * @brief Filler for TTrees
   */
  class HistogramFillerTree : public HistogramFiller {
  public:
    HistogramFillerTree(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {
        parseDefinition();
    }

    virtual HistogramFillerTree* clone() const override {
      return new HistogramFillerTree( *this );
    }

    virtual unsigned fill() const override {
      // handling of the cutmask
      auto cutMaskValuePair = getCutMaskFunc();
      if (cutMaskValuePair.first == 0) { return 0; }
      if (ATH_UNLIKELY(cutMaskValuePair.first > 1)) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree");
        log << MSG::WARNING << "HistogramFillerTree (" << m_histDef->alias 
                            << ") does not support more than a single entry being filled at a time\n"
                            << "so a cut mask with > 1 entry doesn't make sense. Using first entry only." << endmsg;
        if (! cutMaskValuePair.second(0)) { return 0; }
      }

      if (ATH_UNLIKELY(m_monVariables.size() != m_branchDefs.size())) {
        MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree");
        log << MSG::ERROR << "Mismatch of passed variables and expected variables for " << m_histDef->alias 
                          << "(" << m_monVariables.size() << ", " << m_branchDefs.size() << ")" << endmsg;
        return 0;
      }

      auto cutMaskAccessor = cutMaskValuePair.second;

      auto tree = this->histogram<TTree>();
      if (tree->GetListOfBranches()->GetEntries() == 0) {
        createBranches(tree);
      }
      auto branchList = tree->GetListOfBranches();
      // following logic allows us to skip branches that were badly-defined
      size_t idx = 0, idxgood = 0;
      for (const auto& brdef : m_branchDefs) {
        if (ATH_UNLIKELY(brdef.second == "IGNORE")) {
          ++idx; continue;
        }
        TBranch* branch = static_cast<TBranch*>(branchList->At(idxgood));
        m_fillerFunctions[idx](branch, m_monVariables[idx].get());
        ++idx; ++idxgood;
      }
      for (Int_t i = 0; i < branchList->GetEntries(); ++i) {
        

      }
      tree->SetEntries(tree->GetEntries() + 1);
      return 1;
    }

  private:
    std::vector<std::pair<std::string, std::string>> m_branchDefs;
    std::vector<std::function<void(TBranch*, const IMonitoredVariable&)>> m_fillerFunctions;

    void parseDefinition() {
      std::vector<std::string> tokenized;
      boost::split(tokenized, m_histDef->treeDef, [](char c){ return c ==':'; });
      for (const auto& token : tokenized) {
        auto ipart = token.find("/");
        if (ipart == std::string::npos) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree");
          log << MSG::ERROR << "Tree " << m_histDef->alias << ": Badly formed variable definition " << token 
              << "; skipping" << endmsg;
          continue;
        }
        auto branch = token.substr(0, ipart);
        auto type = token.substr(ipart+1);
        std::function<void(TBranch*, const IMonitoredVariable&)> fillerFunc;
        // scalar
        if (type == "B") fillerFunc = scalarFillerFunc<Char_t>;
        else if (type == "b") fillerFunc = scalarFillerFunc<UChar_t>;
        else if (type == "S") fillerFunc = scalarFillerFunc<Short_t>;
        else if (type == "s") fillerFunc = scalarFillerFunc<UShort_t>;
        else if (type == "I") fillerFunc = scalarFillerFunc<Int_t>;
        else if (type == "i") fillerFunc = scalarFillerFunc<UInt_t>;
        else if (type == "F") fillerFunc = scalarFillerFunc<Float_t>;
        else if (type == "f") fillerFunc = scalarFillerFunc<Float16_t>;
        else if (type == "D") fillerFunc = scalarFillerFunc<Double_t>;
        else if (type == "d") fillerFunc = scalarFillerFunc<Double32_t>;
        else if (type == "L") fillerFunc = scalarFillerFunc<Long64_t>;
        else if (type == "l") fillerFunc = scalarFillerFunc<ULong64_t>;
        else if (type == "O") fillerFunc = scalarFillerFunc<Bool_t>;
        else if (type == "string") fillerFunc = scalarFillerFunc<std::string>;
        else if (type == "vector<char>") fillerFunc = vectorFillerFunc<char>;
        else if (type == "vector<unsigned char>") fillerFunc = vectorFillerFunc<unsigned char>;
        else if (type == "vector<int>") fillerFunc = vectorFillerFunc<int>;
        else if (type == "vector<unsigned int>") fillerFunc = vectorFillerFunc<unsigned int>;
        else if (type == "vector<float>") fillerFunc = vectorFillerFunc<float>;
        else if (type == "vector<double>") fillerFunc = vectorFillerFunc<double>;
        else if (type == "vector<long>") fillerFunc = vectorFillerFunc<long>;
        else if (type == "vector<unsigned long>") fillerFunc = vectorFillerFunc<unsigned long>;
        else if (type == "vector<string>") fillerFunc = vectorFillerFunc<std::string>;
        else if (type == "C") {
          MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree");
          log << MSG::ERROR << "Tree " << m_histDef->alias << ": Branch type \"C\" not supported for branch" << branch
                            << "; please use \"string\"" << endmsg;
          type = "IGNORE";
        } else {
          MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree");
          log << MSG::ERROR << "Tree " << m_histDef->alias << ": Unrecognized branch type " << type << " for branch " << branch
                            << "; ignoring branch " << endmsg;
          type = "IGNORE";
        }
        m_branchDefs.emplace_back(branch, type);
        m_fillerFunctions.push_back(fillerFunc);
      }
    }

    template <typename T>
    void branchHelper(TTree* tree, const std::pair<std::string, std::string>& brdef) const {
      std::vector<T> dummy;
      std::vector<T>* dummyptr = &dummy;
      // we only need dummy object to exist through the end of this method
      tree->Branch(brdef.first.c_str(), &dummyptr);
    }

    void createBranches(TTree* tree) const {
      for (const auto& brdef : m_branchDefs) {
        if (brdef.second == "vector<char>") branchHelper<char>(tree, brdef);
        else if (brdef.second == "vector<unsigned char>") branchHelper<unsigned char>(tree, brdef);
        else if (brdef.second == "vector<int>") branchHelper<int>(tree, brdef);
        else if (brdef.second == "vector<unsigned int>") branchHelper<unsigned int>(tree, brdef);
        else if (brdef.second == "vector<float>") branchHelper<float>(tree, brdef);
        else if (brdef.second == "vector<double>") branchHelper<double>(tree, brdef);
        else if (brdef.second == "vector<long>") branchHelper<long>(tree, brdef);
        else if (brdef.second == "vector<unsigned long>") branchHelper<unsigned long>(tree, brdef);
        else if (brdef.second == "vector<string>") branchHelper<std::string>(tree, brdef);
        else if (brdef.second == "string") {
            std::string dummy; tree->Branch(brdef.first.c_str(), &dummy);
        } else if (brdef.second != "IGNORE") {
          tree->Branch(brdef.first.c_str(), (void*) nullptr, (brdef.first+"/"+brdef.second).c_str());
        }
      }
    }
  };

  // helper functions for filling branches
  template <typename T>
  void scalarFillerFunc(TBranch* branch, const IMonitoredVariable& var) {
    if (ATH_UNLIKELY(var.size() == 0)) {
      MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree"); 
      log << MSG::WARNING << "Tree " << branch->GetTree()->GetName() << ": Empty value passed to scalar branch fill for" 
                          << branch->GetName() << endmsg;
      return; 
    }
    T tofill = var.get(0);
    T* ptr = &tofill;
    branch->SetAddress(ptr);
    branch->Fill();
  }

  // specialization for string
  template <>
  void scalarFillerFunc<std::string>(TBranch* branch, const IMonitoredVariable& var) {
    if (ATH_UNLIKELY(var.size() == 0)) { 
      MsgStream log(Athena::getMessageSvc(), "HistogramFillerTree"); 
      log << MSG::WARNING << "Tree " << branch->GetTree()->GetName() << ": Empty value passed to scalar branch fill for" 
                          << branch->GetName() << endmsg;
      return; 
    }
    std::string tofill{var.getString(0)};
    branch->SetObject(&tofill);
    //*static_cast<std::string*>(branch->GetObject()) = tofill;
    branch->Fill();
  }

  template <typename T>
  void vectorFillerFunc(TBranch* branch, const IMonitoredVariable& var) {
    std::vector<T> tofill;
    tofill.reserve(var.size());
    for (size_t i = 0; i < var.size(); i++) {
      tofill.push_back(var.get(i));
    }
    std::vector<T>* tofillptr = &tofill;
    branch->SetAddress(&tofillptr);
    branch->Fill();
  }

  // specialization for string
  template <>
  void vectorFillerFunc<std::string>(TBranch* branch, const IMonitoredVariable& var) {
    std::vector<std::string> tofill;
    tofill.reserve(var.size());
    for (size_t i = 0; i < var.size(); i++) {
      tofill.push_back(var.getString(i));
    }
    auto* tofillptr = &tofill;
    branch->SetAddress(&tofillptr);
    branch->Fill();
  }
}

#endif // AthenaMonitoringKernel_HistogramFiller_HistogramFillerTree_h
