/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RegionSelector/RegSelectorMapElement.h"

#include <stdio.h>
#include <iostream>
// using namespace std;

RegSelectorMapElement::RegSelectorMapElement(){
  m_position = 0;
  m_number = 0;
}

int RegSelectorMapElement::layerDiskPosition(void){
  return m_position;
}

int RegSelectorMapElement::layerDiskNumber(void){
  return m_number;
}
std::vector<IdentifierHash> RegSelectorMapElement::hashId() const {
  return m_hashId;
}

void RegSelectorMapElement::insertElementInVector(double etaminIn, double etamaxIn, double phiminIn,
						  double phimaxIn, IdentifierHash hashIdIn,
						  uint32_t robIdIn){
  if(etaminIn > etamaxIn){
      m_etamin.push_back(etamaxIn);
      m_etamax.push_back(etaminIn);
  }
  else{
    m_etamin.push_back(etaminIn);
    m_etamax.push_back(etamaxIn);
  }

  m_phimin.push_back(phiminIn);
  m_phimax.push_back(phimaxIn);
  m_hashId.push_back(hashIdIn);
  m_robId.push_back(robIdIn);
}

void RegSelectorMapElement::insertNumberElement(int numberIn){
  m_number = numberIn;
}

void RegSelectorMapElement::insertPositionElement(int positionIn){
  m_position = positionIn;
}

void RegSelectorMapElement::findHash(double &etaminIn, double &etamaxIn,double &phiminIn,
				     double &phimaxIn, std::set<IdentifierHash> &outset){
  int vecsize = m_etamin.size();
  double phiTmpMin = phiminElem();
  double phiTmpMax = phimaxElem();
    for(int i= 0; i < vecsize; i++){
    if( (etaminIn <= m_etamax[i]) && (etamaxIn >= m_etamin[i]) ){
      if( m_phimin[i] < m_phimax[i]){
	if( (phiminIn <= m_phimax[i]) && (phimaxIn >= m_phimin[i]) )
	  outset.insert(m_hashId[i]);
      }
      else{
	if( ((phiminIn <= m_phimax[i]) && (phimaxIn >= phiTmpMin )) ||
	    ((phiminIn <= phiTmpMax )  && (phimaxIn >= m_phimin[i])) )
	  outset.insert(m_hashId[i]);
      }
    }
  }
}

void RegSelectorMapElement::findRobId(double &etaminIn, double &etamaxIn, double &phiminIn,
					  double &phimaxIn, std::set<uint32_t> &outset){
  int vecsize = m_etamin.size();
  double phiTmpMin = phiminElem();
  double phiTmpMax = phimaxElem();
  for(int i= 0; i < vecsize; i++){
    if( (etaminIn <= m_etamax[i]) && (etamaxIn >= m_etamin[i]) ){
      if( m_phimin[i] < m_phimax[i]){
	if( (phiminIn <= m_phimax[i]) && (phimaxIn >= m_phimin[i]) )
           outset.insert( m_robId[i]);
      }
      else{
	if( ((phiminIn <= m_phimax[i]) && (phimaxIn >= phiTmpMin )) ||
	    ((phiminIn <= phiTmpMax )  && (phimaxIn >= m_phimin[i])) )
	    outset.insert( m_robId[i]);
      }
    }
  }
}

double RegSelectorMapElement::etaMin( const IdentifierHash hashId ){
  return m_etamin[ hashId ];
}

double RegSelectorMapElement::etaMax( IdentifierHash hashId ){
  return m_etamax[ hashId ];
}

double RegSelectorMapElement::phiMin( IdentifierHash hashId ){
  return m_phimin[ hashId ];
}

double RegSelectorMapElement::phiMax( IdentifierHash hashId ){
  return m_phimax[ hashId ];
}

double RegSelectorMapElement::etaminElem(){
  return m_etaminValue;
}

double RegSelectorMapElement::etamaxElem(){
  return m_etamaxValue;
}

double RegSelectorMapElement::phiminElem(){
  if(m_phiminValue < -M_PI/2)
    m_phiminValue = -M_PI;
  else
    m_phiminValue = 0;
  return m_phiminValue;
}

double RegSelectorMapElement::phimaxElem(){
  if(m_phimaxValue > (3*M_PI)/2)
    m_phimaxValue = 2*M_PI;
  else
    m_phimaxValue = M_PI;
  return m_phimaxValue;
}

void RegSelectorMapElement::findMaxMinElem(void){
  m_etaminValue = 1000; m_etamaxValue = -1000;
  m_phiminValue = 1000; m_phimaxValue = -1000;
  int vecsize = m_etamin.size();
  for(int i = 0; i < vecsize; i++){
    if( m_etamin[i] < m_etaminValue )
      m_etaminValue = m_etamin[i];
    if( m_etamax[i] > m_etamaxValue )
      m_etamaxValue = m_etamax[i];
    if( m_phimin[i] < m_phiminValue )
      m_phiminValue = m_phimin[i];
    if( m_phimax[i] > m_phimaxValue )
      m_phimaxValue = m_phimax[i];
  }
}

void RegSelectorMapElement::additem(const IdentifierHash hashId, const double etaMin,
 				    const double etaMax, const double phiMin, const double phiMax,
                                    const int layerDiskNumber, const int layerDiskPosition,
			            const uint32_t robId){

  insertElementInVector( etaMin, etaMax, phiMin, phiMax, hashId, robId);
  insertNumberElement( layerDiskNumber );
  insertPositionElement( layerDiskPosition );
}

void RegSelectorMapElement::selection(double &etaminIn, double &etamaxIn, 
				double &phiminIn, double &phimaxIn,
				 std::set<IdentifierHash> &outset){
  if(phiminIn > phimaxIn){
    double tmp = phimaxElem();
    findHash(etaminIn, etamaxIn, phiminIn, tmp, outset);
    tmp = phiminElem();
    findHash(etaminIn, etamaxIn, tmp, phimaxIn, outset);
  }
  else{
    findHash(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
  }
}

void RegSelectorMapElement::selectionRobIdUint(double &etaminIn, double &etamaxIn, 
							double &phiminIn, double &phimaxIn, 
					      std::set<uint32_t> &outset){
  if(phiminIn > phimaxIn){
    double tmp = phimaxElem();
    findRobId(etaminIn, etamaxIn, phiminIn, tmp, outset);
    tmp = phiminElem();
    findRobId(etaminIn, etamaxIn, tmp, phimaxIn, outset);
  }
  else{
    findRobId(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
  }
}
