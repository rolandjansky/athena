#include "QatDataAnalysis/Table.h"
#include "QatDataAnalysis/HistogramManager.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "CLHEP/GenericFunctions/Parameter.hh"
#include <string>
void record(HistogramManager *output, const MinuitMinimizer & minimizer) {
  Table *param      = output->newTable("PARAMETERS");
  Table *index      = output->newTable("INDICES");
  Table *covariance = output->newTable("COVARIANCE");
  Table *FCNmin     = output->newTable("FCNMin");
 
  FCNmin->add("FCNmin",minimizer.getFunctionValue());
  FCNmin->add("Status",minimizer.getStatus());    
  FCNmin->capture();
  for (unsigned int i=0;i<minimizer.getNumParameters();i++) {
    const Genfun::Parameter *p = minimizer.getParameter(i);
   
    index->add(p->getName(),i);
    param->add("Index",i);
    param->add("Value",p->getValue());
    param->add("Min",p->getLowerLimit());
    param->add("Max",p->getUpperLimit());
    param->capture();
    for (unsigned int j=0;j<minimizer.getNumParameters();j++ ) {
      const Genfun::Parameter *q = minimizer.getParameter(j);
      covariance->add("I",i);
      covariance->add("J",j);
      covariance->add("CIJ", minimizer.getError(p,q));
      covariance->capture();
    }
  }
  index->capture();
}

MinuitMeasurement getMeasurement(const HistogramManager *input, const std::string & parName) {
  MinuitMeasurement measurement;
  measurement.value=0;
  measurement.error=0;
  const Table *indices      = input->findTable("INDICES");
  const Table *parameters   = input->findTable("PARAMETERS");
  const Table *covariance   = input->findTable("COVARIANCE");
  
  unsigned int    index=0;
  indices->read(0,parName,index);
  double          value=0;
  parameters->read(index,"Value", value);
  unsigned int I(0),J(0);
  double CIJ=0;
  for (unsigned int i=0;i<covariance->numTuples();i++) {
    covariance->read(i,"I", I);
    covariance->read(i,"J", J);
    if (I==index && J==index) {
      covariance->read(i,"CIJ", CIJ);
      measurement.value=value;
      measurement.error=sqrt(CIJ);
      break;
    }
  }
  
  return measurement;
}
