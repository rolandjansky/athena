/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoIData_h__
#define __RoIData_h__
/*
  RoIData.h
*/
#include <string>
#include <vector>
#include "TriggerMenuNtuple/TrigStatus.h"

bool isPassed(const std::string& chain_name, 
	      const std::vector<TrigStatus>& status_vec);
bool isAssociated(const std::string& chain_name, 
		  const std::vector<TrigStatus>& status_vec);

class RoIData {

private:
  static std::string sChainName;

public:
  RoIData(int create=0);
  virtual ~RoIData();

  //
  virtual bool l2exist() const;
  virtual float l2pt() const;
  virtual float l2eta() const;
  virtual float l2phi() const;

  virtual bool efexist() const;
  virtual float efpt() const;
  virtual float efeta() const;
  virtual float efphi() const;
  //

  bool isAssociated(const std::string& cname) const;
  bool isPassed(const std::string& cname) const;
  bool isPassed(const std::string& chain_name, 
		const std::vector<TrigStatus>& status_vec);
  bool isAssociated(const std::string& chain_name, 
		    const std::vector<TrigStatus>& status_vec);
  bool isPassed() const;

  inline const std::vector<std::string>& getAssociatedChains() const;
  inline const std::vector<std::string>& getPassedChains() const;
  
  inline void setAssociatedChains(const std::vector<std::string>& v);
  inline void setPassedChains(const std::vector<std::string>& v);

  static void setChainToCheck(std::string str);
  static std::string getChainToCheck();


protected:
  std::vector<std::string> mAssociatedChains;
  std::vector<std::string> mPassedChains;
};

const std::vector<std::string>& RoIData::getAssociatedChains() const {
  return mAssociatedChains;
}

const std::vector<std::string>& RoIData::getPassedChains() const {
  return mPassedChains;
}

void RoIData::setAssociatedChains(const std::vector<std::string>& v) {
  //  if (mAssociatedChains==0) mAssociatedChains = new std::vector<std::string>();
  mAssociatedChains = v;
}

void RoIData::setPassedChains(const std::vector<std::string>& v) {
  //  if (mPassedChains==0) mPassedChains = new std::vector<std::string>();
  mPassedChains = v;
}

#endif // __RoIData_h__
