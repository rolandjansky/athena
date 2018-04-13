#include "TrigInDetPattRecoTools/DoubletResLookup.h"
#include <stdexcept> // out_of_range exception

DoubletResLookup::DoubletResLookup() {
  // create vectors of fit parameters for fits to z0 residual vs eta:
  // parametrised by layer, barrel or endcap, and inner or outer type doublets
  //
  // NOTE: fits were produced ignoring SCT->pix doublets

  // for doublet midpoint in pixels
  std::vector< std::vector<double> > pix_barrel_inn = { { 0.000000, 0.000000, 0.000000 }, // cannot form an inner doublet with IBL
                                                        { 0.288947, -0.002310, 0.002386 },
							{ 0.221817, 0.000858, 0.000135 },
							{ 0.221914, 0.001357, -0.000363 } };

  std::vector< std::vector<double> > pix_barrel_outr = { { 0.218743, 0.000515, 0.000282 },
							 { 0.225039, -0.001908, 0.001879 },
							 { 0.304582, -0.051218, 0.023582 },
							 { 0.377925, -0.123984, 0.081515 } };

  std::vector< std::vector<double> > pix_endcap_inn = { { 0.218634, 0.011829, -0.000274 },
							{ 0.256454, -0.008510, 0.002615 },
							{ 0.260501, -0.002157, 0.001081 } };
  
  std::vector< std::vector<double> > pix_endcap_outr = { { 4.395863, -1.664610, 0.203655 },
							 { -0.669678, 0.531671, -0.019895 },
							 { -20.236349, 7.944731, -0.709482 } };

  // for doublet midpoint in SCT
  std::vector< std::vector<double> > sct_barrel_inn = { { 0.000000, 0.000000, 0.000000 }, // cannot form an inner doublet with SCT first barrel layer
							{ 4.633091, -0.762706, 0.298495 },
							{ 2.464560, -0.310093, 0.148860 },
							{ 1.523526, -0.137975, 0.060219 } };

  std::vector< std::vector<double> > sct_barrel_outr = { { 1.738054, -0.160983, 0.126355 },
							 { 2.883086, -0.666834, 0.538291 },
							 { 5.322181, -1.063491, 0.745554 },
							 { 0.000000, 0.000000, 0.000000 } }; // cannot form an outer doublet with SCT last barrel layer

  std::vector< std::vector<double> > sct_endcap_inn = { { 8.122619, -8.632954, 3.176801 },
							{ 20.238796, -20.796283, 6.061286 },
							{ 3.352235, -4.197448, 1.915598 },
							{ -5.475171, 2.529484, 0.391650 },
							{ 48.494789, -30.907494, 5.439277 },
							{ 10.625556, -5.933777, 1.180163 },
							{ 76.690219, -34.321548, 4.114075 },
							{ -60.168713, 20.558626, -1.410660 },
							{ 4.975197, 0.627486, 0.062806 } };

  std::vector< std::vector<double> > sct_endcap_outr = { { 29.943720, -13.931958, 1.760239 },
							 { 31.500658, -15.077034, 2.051784 },
							 { 15.195015, -2.631298, 0.027117 },
							 { 116.595790, -46.469904, 4.863254 },
							 { 20.728878, -3.559963, 0.112256 },
							 { 54.644726, -14.439724, 1.111641 },
							 { 52.689573, -13.127867, 1.136984 },
							 { 9.260209, 2.108495, 0.451135 },
							 { 0.000000, 0.000000, 0.000000 } }; // cannot form an outer doublet with SCT last endcap layer

  // create final vectors, organised by new layer numbering scheme, with endcaps split between +ve and -ve side (current doublet residual measurements do not split between +ve and -ve endcaps)
  m_fitPars_inn.insert(m_fitPars_inn.end(), pix_barrel_inn.begin(), pix_barrel_inn.end());
  m_fitPars_inn.insert(m_fitPars_inn.end(), sct_barrel_inn.begin(), sct_barrel_inn.end());
  m_fitPars_inn.insert(m_fitPars_inn.end(), pix_endcap_inn.begin(), pix_endcap_inn.end());
  m_fitPars_inn.insert(m_fitPars_inn.end(), sct_endcap_inn.begin(), sct_endcap_inn.end());
  m_fitPars_inn.insert(m_fitPars_inn.end(), pix_endcap_inn.begin(), pix_endcap_inn.end());
  m_fitPars_inn.insert(m_fitPars_inn.end(), sct_endcap_inn.begin(), sct_endcap_inn.end());

  m_fitPars_outr.insert(m_fitPars_outr.end(), pix_barrel_outr.begin(), pix_barrel_outr.end());
  m_fitPars_outr.insert(m_fitPars_outr.end(), sct_barrel_outr.begin(), sct_barrel_outr.end());
  m_fitPars_outr.insert(m_fitPars_outr.end(), pix_endcap_outr.begin(), pix_endcap_outr.end());
  m_fitPars_outr.insert(m_fitPars_outr.end(), sct_endcap_outr.begin(), sct_endcap_outr.end());
  m_fitPars_outr.insert(m_fitPars_outr.end(), pix_endcap_outr.begin(), pix_endcap_outr.end());
  m_fitPars_outr.insert(m_fitPars_outr.end(), sct_endcap_outr.begin(), sct_endcap_outr.end());
}

double DoubletResLookup::getRes(unsigned int layer, DoubletType type, double eta) {
  if (type == DoubletType::inner) {
    std::vector<double>* fitPars;
    
    try {
      fitPars = &m_fitPars_inn.at(layer);
    } catch (const std::out_of_range& outOfRange) {
      return 0.;
    }
    
    return ( fitPars->at(0) + fitPars->at(1)*(eta*eta) + fitPars->at(2)*(eta*eta*eta*eta) );
  }
  
  else if (type == DoubletType::outer) {
    std::vector<double>* fitPars;
    
    try {
      fitPars = &m_fitPars_outr.at(layer);
    } catch (const std::out_of_range& outOfRange) {
      return 0.;
    }
    
    return ( fitPars->at(0) + fitPars->at(1)*(eta*eta) + fitPars->at(2)*(eta*eta*eta*eta) );
  }

  return 0.;
}
