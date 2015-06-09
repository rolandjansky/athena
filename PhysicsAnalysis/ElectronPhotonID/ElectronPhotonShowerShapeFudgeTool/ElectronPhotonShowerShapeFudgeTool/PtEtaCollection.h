/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PtEtaCollection_h
#define PtEtaCollection_h

#include <iostream>

template <class T>
class PtEtaCollection
{

 public:

  PtEtaCollection();

  PtEtaCollection(unsigned int PtBins, 
		  unsigned int EtaBins,		  
		  double PtArray[],
		  double EtaArray[],
		  T* PayloadArray,
		  T  DefaultPayload);


  virtual ~PtEtaCollection();

  virtual void Initialize(unsigned int PtBins, 
			  unsigned int EtaBins,
			  double PtArray[], 
			  double EtaArray[], 
			  T* PayloadArray,
			  T  DefaultPayload);

  void Set(double pt, double eta, T Payload);
  T Get(double pt, double eta);

  unsigned int GetPtBins()      { return m_PtBins; }
  unsigned int GetEtaBins()     { return m_EtaBins; }
  unsigned int GetPayloadBins() { return m_PtBins*m_EtaBins; }
  double* GetPtArray()  { return m_PtArray; }
  double* GetEtaArray() { return m_EtaArray; }
  T* GetPayloadArray()  { return m_PayloadArray; }

 protected:

  unsigned int m_PtBins;
  unsigned int m_EtaBins;
  double* m_PtArray;
  double* m_EtaArray;
  T* m_PayloadArray;
  T m_DefaultPayload;

  int PtBin (double Pt);
  int EtaBin(double Eta);

};

#include "PtEtaCollection.icc"

#endif
