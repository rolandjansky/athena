//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"

#include <fstream>



class CaloCellDumper: public AthAlgorithm {

 public:
  CaloCellDumper(const std::string& name, ISvcLocator * pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

  virtual StatusCode finalize() override;

private:
  std::ofstream m_outfile;
  SG::ReadHandleKey<CaloCellContainer> m_key{this,"InputContainer","AllCalo","Input CaloCellContainer key"};
  Gaudi::Property<std::string> m_fileName{this,"FileName","CaloCells.txt","Name of the output text file"};
  Gaudi::Property<float> m_eCut{this,"EnergyCut",std::numeric_limits<float>::lowest(),"Energy cut for cell dumping"};
  Gaudi::Property<bool> m_compact{this,"Compact",true,"compact or detailed cell identifer"};

};

