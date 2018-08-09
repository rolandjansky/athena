#ifndef FTKDataMatrixFromMCAlgo_h
#define FTKDataMatrixFromMCAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigFTKSim/FTKLogging.h"
#include "TrigFTKBankGen/FTKGhostHitCalculator.h"

#include <TFile.h>
#include <string>

class FTKDataMatrixFromMCAlgo : public AthAlgorithm, public FTKLogger {
public:
   FTKDataMatrixFromMCAlgo(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~FTKDataMatrixFromMCAlgo ();
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
 protected:
   virtual void PostMessage(void); // divert FTKLogger messages to Athena
   std::string m_MCmatrixFileName;
   std::string m_DATAmatrixFileName;
   std::string m_debugFileName;
   std::string m_MCmodulePositionFileName;
   std::string m_DATAmodulePositionFileName;
   std::string m_pmap_path;
   std::string m_rmap_path;
   int m_HWMODEID;
   int m_tower;
   int m_invertIBLphiMatrix;
   int m_invertIBLphiData;
   std::string m_modulelut_path;
   double m_matrixVtxX,m_matrixVtxY,m_matrixVtxZ;
   double m_dataVtxX,m_dataVtxY,m_dataVtxZ;
   
   FTKRegionMap *m_rmap;
   FTKGhostHitCalculator *m_MCmodulePositionCalculator;
   FTKGhostHitCalculator *m_DATAmodulePositionCalculator;
   TFile *m_MCmatrixFile; // input: FTK matrix file for MC geometry
   TFile *m_DATAmatrixFile; // output: FTK matrix file for data geometry
   TFile *m_debugFile;
};

#endif
