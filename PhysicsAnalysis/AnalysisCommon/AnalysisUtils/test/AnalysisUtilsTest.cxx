/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <time.h>
#include <cstdlib>
#include <vector>
#include <iostream>

#include "AnalysisUtils/AnalysisMisc.h"
#include "AnalysisUtils/AnalysisSelection.h"
#include "AnalysisUtils/AnalysisCombination.h"
#include "AnalysisUtils/AnalysisPermutation.h"

typedef std::vector<int *>    MyCollI_t;
typedef std::vector<long *>   MyCollL_t;
typedef std::vector<float *>  MyCollF_t;
typedef std::vector<double *> MyCollD_t;

// simple particle class
struct SimpleParticle
{
  int index() const {return m_index;}
  double pt() const {return m_pt;}
  void set(const int index, const double pt) {m_index=index; m_pt=pt;}
  
private:
  int m_index;
  double m_pt;
};


namespace AUTest {
  struct MyAction0
  {
    typedef PARLIST_1 (MyCollI_t::value_type) Param;
    void action(Param *param)
    {
      MyCollI_t::value_type par = param->par<0>();
      
      std::cout << "MyAction0 : " << *par << std::endl;
    }
  };
  
  struct MyAction1
  {
    typedef PARLIST_2 (MyCollI_t::value_type, MyCollF_t::value_type) Param;
    void action(Param *param)
    {
      MyCollI_t::value_type par0 = param->par<0>();
      MyCollF_t::value_type par1 = param->par<1>();
      
      std::cout << "MyAction1 : " << *par0 << " " << *par1 << std::endl;
    }
  };

  struct MyAction2 {
    
    typedef PARLIST_3 (MyCollI_t::value_type, MyCollF_t::value_type, MyCollD_t::value_type) Param;
    void action(Param *param)
    {
      MyCollI_t::value_type par0 = param->par<0>();
      MyCollF_t::value_type par1 = param->par<1>();
      MyCollD_t::value_type par2 = param->par<2>();
      
      std::cout << "MyAction2 : " << *par0 << " " << *par1 << " " << *par2 << std::endl;
    }
  };

  struct MyAction3
  {
    typedef PARLIST_4 (MyCollI_t::value_type, MyCollF_t::value_type, MyCollD_t::value_type, MyCollL_t::value_type) Param;
    void action(Param *param)
    {
      MyCollI_t::value_type par0 = param->par<0>();
      MyCollF_t::value_type par1 = param->par<1>();
      MyCollD_t::value_type par2 = param->par<2>();
      MyCollL_t::value_type par3 = param->par<3>();
      
      std::cout << "MyAction3 : " << *par0 << " " << *par1 << " " << *par2 << " " << *par3 << std::endl;
    }
  };

  struct MyCriteria
  {
    bool isAccepted(MyCollI_t::value_type e)
    {
      if (((*e) % 3) == 0) return true;
      return false;
    }
  };

}; // end of namespace AUTest

class CombTest 
{
public:
  CombTest();
  ~CombTest() {}

  void doAction(int cut);
  static bool criteria(CombTest *self, const std::vector<int> &out);

private:
  int m_cut;
  std::vector<int> m_coll;

  friend class CriteriaObj;
  friend bool criteria2(CombTest *self, const std::vector<int> &out);
};

class CriteriaObj
{
public:
  CriteriaObj (CombTest *combTest, const std::vector<int> &out) {}
  ~CriteriaObj () {}

  bool operator () (CombTest *combTest, const std::vector<int> &out)
  {
    if ((out[0] % combTest->m_cut) == 1) return true;
    return false;
  }    
};
  
CombTest::CombTest()
{
  for (int i=0; i<10; ++i)
    m_coll.push_back(i);
}
  
void CombTest::doAction (int cut)
{
  m_cut = cut;

  std::cout << "Comb+Select1" << std::endl;
  AnalysisUtils::Combination<std::vector<int> > comb(&m_coll,3);
  std::vector<int> out;
  while (comb.goodOnes(this,out,CombTest::criteria))
    std::cout << out[0] << " " << out[1] << " " << out[2] << std::endl;

  std::cout << "Comb+Select2" << std::endl;
  AnalysisUtils::Combination<std::vector<int> > comb2(&m_coll,4);
  while (comb2.goodOnes(this,out,criteria2))
    std::cout << out[0] << " " << out[1] << " " << out[2] << " " << out[3] << std::endl;

  std::cout << "Comb+Select3" << std::endl;
  AnalysisUtils::Combination<std::vector<int> > comb3(&m_coll,2);
  std::vector<int> rest;
  CriteriaObj criteriaObj(this,out);
  while (comb3.goodOnes(this,out,rest,criteriaObj))
    std::cout << out[0] << " " << out[1] << " " << out[2] << std::endl;
}

bool CombTest::criteria(CombTest *self, const std::vector<int> &out)
{
  if (out[0] > self->m_cut) return true;
  return false;
}

bool criteria2(CombTest *self, const std::vector<int> &out)
{
  if ((out[1] % self->m_cut) == 0) return true;
  return false;
}

int main()
{
  const int nEle = 5;

  MyCollI_t v;
  for (int i=0; i<nEle; ++i)
    {
      int *tmp = new int(i*i);
      v.push_back(tmp);
    }

  MyCollF_t w;
  for (int i=0; i<nEle+5; ++i)
    {
      float *tmp = new float(i*0.5);
      w.push_back(tmp);
    }

  MyCollD_t x;
  for (int i=0; i<nEle+3; ++i)
    {
      double *tmp = new double(i*0.3);
      x.push_back(tmp);
    }

  MyCollL_t y;
  for (int i=0; i<nEle+3; ++i)
    {
      long *tmp = new long(i+5);
      y.push_back(tmp);
    }

  ///////////// test for Combination
  std::cout << "Combination Test 1" << std::endl;

  const int nEleCom = 4;
  AnalysisUtils::Combination<MyCollI_t> comb(&v, nEleCom);

  MyCollI_t out;

  int iComb=0;
  while (comb.get(out))
    {
      ++iComb;
      for (int i=0; i<nEleCom; ++i)
	std::cout << *(out[i]) << " ";
      std::cout << std::endl;
    }

  std::cout << "Length:" << v.size() << " N:" << nEleCom << " Total:" << iComb << std::endl;

  std::cout << "Combination Test 2" << std::endl;
  comb.reset();
  MyCollI_t remain;
  while (comb.get(out,remain))
    {
      ++iComb;
      for (int i=0; i<nEleCom; ++i)
	std::cout << *(out[i]) << " ";
      std::cout << std::endl;
      for (unsigned int i=0; i<remain.size(); ++i)
	std::cout << *(remain[i]) << " ";
      std::cout << std::endl;
    }

  ///////////// test for Selection

  MyCollI_t selV;
  AnalysisUtils::Selection<AUTest::MyCriteria> selection;
  selection.getObjects(&v,selV);
  MyCollI_t::const_iterator it  = selV.begin();
  MyCollI_t::const_iterator itE = selV.end();
  for (; it!=itE; ++it)
    std::cout << "Sel: " << **it << std::endl;

  ///////////// test for Sort

  srand(static_cast<unsigned>(time(NULL)));
  typedef std::vector<SimpleParticle *> SPcoll_t;
  SPcoll_t spContainer;
  for (int i=0; i<10; ++i)
    {
      SimpleParticle *e = new SimpleParticle;
      e->set(i, rand()%100);
      spContainer.push_back(e);
    }
  AnalysisUtils::Sort::pT(&spContainer);
  SPcoll_t::const_iterator itS =spContainer.begin();
  SPcoll_t::const_iterator itSe=spContainer.end();
  for (; itS!=itSe; ++itS)
    std::cout << "Sort: " << (*itS)->index() << " " << (*itS)->pt() << std::endl;    


  ///////////// test for Permutation
  std::cout << "---Permutation---" << std::endl;

  int nElePer = 3;
  AnalysisUtils::Permutation<MyCollI_t> perm(&v,nElePer);

  int iPer=0;
  while (perm.get(out))
    {
      for (int i=0; i<nElePer; ++i)
	std::cout << *(out[i]) << " ";
      std::cout << std::endl;
      ++iPer;
    }
  std::cout << " Total:" << iPer << std::endl;

  CombTest combTest;
  combTest.doAction(3);

  return 0;
}
