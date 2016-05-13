/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define FudgeMCTool_cxx

#include "ElectronPhotonShowerShapeFudgeTool/FudgeMCTool.h"
#include "TMath.h"

static const double GeV = 1.e+3;

using namespace IDVAR;

	//#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

void FudgeMCTool::FudgeShowers( double  pt     ,
							   double  eta2   ,
							   double& rhad1  ,
							   double& rhad   ,
							   double& e277   ,
							   double& reta   ,
							   double& rphi   ,
							   double& weta2  ,
							   double& f1     ,
							   double& fside  ,
							   double& wtot   ,
							   double& w1     ,
							   double& deltae ,
							   double& eratio ,
							   int     isConv   ,
							   int     preselection)
{
	if(preselection<0) preselection = m_preselection;
	this->LoadFFs(isConv,preselection);

		//fudge showers
	rhad1  = Fudge_Rhad1(rhad1, pt, eta2);
	rhad   = Fudge_Rhad(rhad, pt, eta2);
	e277   = Fudge_E277(e277, pt, eta2);
	reta   = Fudge_Reta(reta, pt, eta2);
	rphi   = Fudge_Rphi(rphi, pt, eta2);
	weta2  = Fudge_Weta2(weta2, pt, eta2);
	f1     = Fudge_F1(f1, pt, eta2);
	deltae = Fudge_DE(deltae, pt, eta2);
	wtot   = Fudge_Wtot(wtot, pt, eta2);
	fside  = Fudge_Fside(fside, pt, eta2);
	w1     = Fudge_W1(w1, pt, eta2);
	eratio = Fudge_Eratio(eratio, pt, eta2);
}


void FudgeMCTool::FudgeShowers( float  pt     ,
							   float  eta2   ,
							   float& rhad1  ,
							   float& rhad   ,
							   float& e277   ,
							   float& reta   ,
							   float& rphi   ,
							   float& weta2  ,
							   float& f1     ,
							   float& fside  ,
							   float& wtot   ,
							   float& w1     ,
							   float& deltae ,
							   float& eratio ,
							   int     isConv   ,
							   int     preselection)
{
	if(preselection<0) preselection = m_preselection;
	this->LoadFFs(isConv,preselection);

		//fudge showers
	rhad1  = Fudge_Rhad1(rhad1, pt, eta2);
	rhad   = Fudge_Rhad(rhad, pt, eta2);
	e277   = Fudge_E277(e277, pt, eta2);
	reta   = Fudge_Reta(reta, pt, eta2);
	rphi   = Fudge_Rphi(rphi, pt, eta2);
	weta2  = Fudge_Weta2(weta2, pt, eta2);
	f1     = Fudge_F1(f1, pt, eta2);
	deltae = Fudge_DE(deltae, pt, eta2);
	wtot   = Fudge_Wtot(wtot, pt, eta2);
	fside  = Fudge_Fside(fside, pt, eta2);
	w1     = Fudge_W1(w1, pt, eta2);
	eratio = Fudge_Eratio(eratio, pt, eta2);
}

void FudgeMCTool::FudgeShowers(  std::vector<float> clE,
                                 std::vector<float> eta2   ,
                                 std::vector<float>& rhad1  ,
                                 std::vector<float>& rhad   ,
                                 std::vector<float>& e277   ,
                                 std::vector<float>& reta   ,
                                 std::vector<float>& rphi   ,
                                 std::vector<float>& weta2  ,
                                 std::vector<float>& f1     ,
                                 std::vector<float>& fside  ,
                                 std::vector<float>& wtot   ,
                                 std::vector<float>& w1     ,
                                 std::vector<float>& deltae ,
                                 std::vector<float>& eratio ,
                                 std::vector<int> isConv  ,
                                 int    preselection)
{
	if(preselection<0) preselection = m_preselection;

   // Assume all vectors have the same size
   for (unsigned int i = 0; i < clE.size(); ++i)
      FudgeShowers(clE[i]/TMath::CosH(eta2[i]),
         eta2  [i],
         rhad1 [i],
         rhad  [i],
         e277  [i],
         reta  [i],
         rphi  [i],
         weta2 [i],
         f1    [i],
         fside [i],
         wtot  [i],
         w1    [i],
         deltae[i] ,
         eratio[i] ,
         isConv[i],
         preselection);
}


	//get Fudge Factors for currently loaded values
double FudgeMCTool::GetFF_Rhad1(){
	return GetFF_Rhad1( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Rhad(){
	return GetFF_Rhad( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_E277(){
	return GetFF_E277( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Reta(){
	return GetFF_Reta( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Rphi(){
	return GetFF_Rphi( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Weta2(){
	return GetFF_Weta2( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_F1(){
	return GetFF_F1( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_DE(){
	return GetFF_DE( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Eratio(){
	return GetFF_Eratio( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Fside(){
	return GetFF_Fside( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_Wtot(){
	return GetFF_Wtot( m_pt, m_eta2 );
}

double FudgeMCTool::GetFF_W1(){
	return GetFF_W1( m_pt, m_eta2 );
}

	//get Fudge Factors errors for currently loaded values
double FudgeMCTool::GetFFerr_Rhad1(){
	return GetFFerr_Rhad1( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Rhad(){
	return GetFFerr_Rhad( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_E277(){
	return GetFFerr_E277( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Reta(){
	return GetFFerr_Reta( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Rphi(){
	return GetFFerr_Rphi( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Weta2(){
	return GetFFerr_Weta2( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_F1(){
	return GetFFerr_F1( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_DE(){
	return GetFFerr_DE( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Eratio(){
	return GetFFerr_Eratio( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Fside(){
	return GetFFerr_Fside( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_Wtot(){
	return GetFFerr_Wtot( m_pt, m_eta2 );
}

double FudgeMCTool::GetFFerr_W1(){
	return GetFFerr_W1( m_pt, m_eta2 );
}

	//get Fudge Factors for new settings
double FudgeMCTool::GetFF_Rhad1( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Rhad1( pt, eta2 );
}

double FudgeMCTool::GetFF_Rhad( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Rhad( pt, eta2 );
}

double FudgeMCTool::GetFF_E277( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_E277( pt, eta2 );
}

double FudgeMCTool::GetFF_Reta( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Reta( pt, eta2 );
}

double FudgeMCTool::GetFF_Rphi( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Rphi( pt, eta2 );
}

double FudgeMCTool::GetFF_Weta2( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Weta2( pt, eta2 );
}

double FudgeMCTool::GetFF_F1( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_F1( pt, eta2 );
}

double FudgeMCTool::GetFF_DE( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_DE( pt, eta2 );
}

double FudgeMCTool::GetFF_Eratio( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Eratio( pt, eta2 );
}

double FudgeMCTool::GetFF_Fside( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Fside( pt, eta2 );
}

double FudgeMCTool::GetFF_Wtot( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_Wtot( pt, eta2 );
}

double FudgeMCTool::GetFF_W1( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFF_W1( pt, eta2 );
}

	//get Fudge Factors errors for new settings
double FudgeMCTool::GetFFerr_Rhad1( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Rhad1( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Rhad( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Rhad( pt, eta2 );
}

double FudgeMCTool::GetFFerr_E277( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_E277( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Reta( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Reta( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Rphi( double pt, double eta2, int conv, int preselection){
	if((preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Rphi( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Weta2( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Weta2( pt, eta2 );
}

double FudgeMCTool::GetFFerr_F1( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_F1( pt, eta2 );
}

double FudgeMCTool::GetFFerr_DE( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_DE( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Eratio( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Eratio( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Fside( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Fside( pt, eta2 );
}

double FudgeMCTool::GetFFerr_Wtot( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_Wtot( pt, eta2 );
}

double FudgeMCTool::GetFFerr_W1( double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return GetFFerr_W1( pt, eta2 );
}

	//fudge a single shower shape
double FudgeMCTool::Fudge_Rhad1( double rhad1, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Rhad1( rhad1, pt, eta2 );
}

double FudgeMCTool::Fudge_Rhad( double rhad, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Rhad( rhad, pt, eta2 );
}

double FudgeMCTool::Fudge_Reta( double reta, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Reta( reta, pt, eta2 );
}

double FudgeMCTool::Fudge_E277( double e277, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_E277( e277, pt, eta2 );
}

double FudgeMCTool::Fudge_Rphi( double rphi, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Rphi( rphi, pt, eta2 );
}

double FudgeMCTool::Fudge_Weta2( double weta2, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Weta2( weta2, pt, eta2 );
}

double FudgeMCTool::Fudge_F1( double f1, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_F1( f1, pt, eta2 );
}

double FudgeMCTool::Fudge_DE( double deltae, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_DE( deltae, pt, eta2 );
}

double FudgeMCTool::Fudge_Eratio( double eratio, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Eratio( eratio, pt, eta2 );
}

double FudgeMCTool::Fudge_Fside( double fside, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Fside( fside, pt, eta2 );
}

double FudgeMCTool::Fudge_Wtot( double wtot, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_Wtot( wtot, pt, eta2 );
}

double FudgeMCTool::Fudge_W1( double w1, double pt, double eta2, int conv, int preselection){
	if( (preselection>=0 && preselection != m_preselection) || (conv>=0 && conv!=m_conv))
		this->LoadFFs(conv, preselection);
	return Fudge_W1( w1, pt, eta2 );
}

	//Load FFs
void FudgeMCTool::LoadFFsDefault()
{
	this->LoadFFs(m_conv, m_preselection);
}

void FudgeMCTool::LoadFFs(int isConv, int preselection)
{

		//FFs obtained according to last id re-optimization (i.e. tight(loose) tune 5(2) in PhotonIDTool)
		//rel16 Data periods A-I
		//JF17,35,70,140,240 MC10
		//'Iso' = EtCone40_corrected < 3GeV

	static double pt_DVs_0[15] = {15000,20000,25000,30000,35000,40000,45000,55000,70000,
		85000,100000,125000,150000,200000,400000};// in MeV
	static double eta_DVs_0[5] = { 0.6, 1.37, 1.52, 1.81, 2.37}; // eta binning according to SMDP inclusive analysis


		//redefine conversion flag *NEW*
	m_conv = isConv;

	if (preselection<0) //define flag for switch downhere *NEW*
		preselection   = m_preselection;
	else		//redefine current configuration *NEW*
		m_preselection = preselection;

	switch (preselection) {

		case 0:  // RobustTight+Iso preselection

				//converted photons

				static double ff_c_rhad1_0[15][5]    = {{-0.00201901, -0.00313722, 0.0 , -0.00296045, -0.000667818},
				{-0.00194984, 0.00073684, 0.0 , 0.000114076, -0.000551676},
				{-0.000735819, -0.00103701, 0.0 , -0.000403063, -0.00112865},
				{-0.000205322, -0.000577202, 0.0 , 0.00029276, -0.000669067},
				{-2.01465e-05, -3.32503e-05, 0.0 , 0.000226236, -0.000515795},
				{-0.000242492, 0.000636269, 0.0 , 2.44312e-05, -4.17088e-05},
				{-0.000768584, -0.000491206, 0.0 , -0.00186764, 0.00067537},
				{-0.000245252, -0.000177353, 0.0 , -0.000531317, -0.000276207},
				{-0.00035814, -2.25012e-05, 0.0 , 0.000344019, 0.000143763},
				{-0.00042429, -0.000571946, 0.0 , -3.83843e-05, 5.27865e-05},
				{-1.32549e-05, 0.000183842, 0.0 , 0.000288124, 3.70078e-05},
				{0.000192393, -0.000189087, 0.0 , 0.000198758, -2.05191e-05},
				{0.000581023, -0.000160059, 0.0 , -0.000881137, 8.0988e-05},
				{-0.00031188, 0.000213898, 0.0 , 0.000679157, -1.82621e-06},
				{-0.000387427, -0.000109134, 0.0 , -0.000376867, 4.52041e-07}		};


				static double fferr_c_rhad1_0[15][5]    = {{0.00103176, 0.00163946, 0.0 , 0.000791676, 0.000635162, },
				{0.00183577, 0.000896634, 0.0 , 0.00185998, 0.00121603, },
				{0.00024689, 0.00043341, 0.0 , 0.000371571, 0.000233989, },
				{0.00028676, 0.000353547, 0.0 , 0.000496861, 0.000302753, },
				{0.000340068, 0.000359242, 0.0 , 0.000693998, 0.000329259, },
				{0.000565694, 0.000328729, 0.0 , 0.00080998, 0.000464987, },
				{0.00026204, 0.000373919, 0.0 , 0.0011114, 0.000602772, },
				{0.000179762, 0.000188531, 0.0 , 0.00035961, 0.000186171, },
				{0.000221073, 0.000200906, 0.0 , 0.00038286, 0.00021588, },
				{0.000382053, 0.000624342, 0.0 , 0.000768646, 0.000337005, },
				{0.000172699, 0.000178854, 0.0 , 0.000389863, 0.000207309, },
				{0.00018305, 0.000235443, 0.0 , 0.000375293, 0.000261447, },
				{0.000172714, 0.000251359, 0.0 , 0.000970002, 0.000315525, },
				{0.000279577, 0.000167228, 0.0 , 0.000549967, 0.000292164, },
				{0.000150386, 7.27626e-05, 0.0 , 0.000296482, 0.000213175, }		};


				static double ff_c_rhad_0[15][5]    = {{0, -0.00267811, 0.0 , 0, 0},
				{0, 0.000597254, 0.0 , 0, 0},
				{0, -0.000320961, 0.0 , 0, 0},
				{0, -0.000704423, 0.0 , 0, 0},
				{0, -2.06614e-06, 0.0 , 0, 0},
				{0, -0.000114451, 0.0 , 0, 0},
				{0, -0.000322536, 0.0 , 0, 0},
				{0, -0.000401715, 0.0 , 0, 0},
				{0, -0.00046355, 0.0 , 0, 0},
				{0, 0.000255787, 0.0 , 0, 0},
				{0, -0.000272332, 0.0 , 0, 0},
				{0, -9.40651e-05, 0.0 , 0, 0},
				{0, -0.000103012, 0.0 , 0, 0},
				{0, -0.000152187, 0.0 , 0, 0},
				{0, -5.4725e-06, 0.0 , 0, 0}		};


				static double fferr_c_rhad_0[15][5]    = {{0, 0.00051383, 0.0 , 0, 0, },
				{0, 0.00121605, 0.0 , 0, 0, },
				{0, 0.000145232, 0.0 , 0, 0, },
				{0, 0.000251349, 0.0 , 0, 0, },
				{0, 0.000237684, 0.0 , 0, 0, },
				{0, 0.000266255, 0.0 , 0, 0, },
				{0, 0.000419039, 0.0 , 0, 0, },
				{0, 0.000183116, 0.0 , 0, 0, },
				{0, 0.000186425, 0.0 , 0, 0, },
				{0, 0.000173638, 0.0 , 0, 0, },
				{0, 0.000202543, 0.0 , 0, 0, },
				{0, 0.000118862, 0.0 , 0, 0, },
				{0, 0.000132471, 0.0 , 0, 0, },
				{0, 0.00010412, 0.0 , 0, 0, },
				{0, 5.54677e-05, 0.0 , 0, 0, }		};


				static double ff_c_e277_0[15][5]    = {{983.238, 1229.59, 0.0 , 3135.74, 4146.14},
				{-322.823, 1387.42, 0.0 , 1108.92, -425.992},
				{-25.0986, 195.973, 0.0 , 292.918, 390.047},
				{81.1914, 388.422, 0.0 , 181.961, -109.445},
				{-95.0898, -168.545, 0.0 , 1185.17, -264.789},
				{-287.219, 629.426, 0.0 , -875.207, -568.484},
				{917.502, 2498.71, 0.0 , -2801.07, -1516.09},
				{-346.707, 1055.13, 0.0 , -2523.62, -1829.48},
				{-160.855, 1785.84, 0.0 , -506.812, -1501.34},
				{-2401.08, 1768.79, 0.0 , -5566.27, -10223.9},
				{-1305.64, 3228.77, 0.0 , -8841.39, -4451.44},
				{1222.73, 2102.55, 0.0 , -5067.33, -14701.2},
				{-7178.87, 4936.97, 0.0 , 23258.8, 290754},
				{7066.77, -2289.48, 0.0 , 25922.1, 0},
				{-37169.3, -11012.8, 0.0 , 11963.5, 0}		};


				static double fferr_c_e277_0[15][5]    = {{120.225, 76.6262, 0.0 , 122.911, 219.582, },
				{599.252, 460.914, 0.0 , 1549.79, 1685.2, },
				{119.927, 123.738, 0.0 , 366.034, 395.393, },
				{187.903, 224.875, 0.0 , 711.477, 763.533, },
				{312.838, 412.172, 0.0 , 1357.94, 1103.68, },
				{379.886, 572.68, 0.0 , 2025.91, 1660.41, },
				{537.662, 899.029, 0.0 , 2762.42, 2810.67, },
				{376.662, 462.842, 0.0 , 1767.99, 1586.05, },
				{552.718, 764.483, 0.0 , 2444.53, 2487.72, },
				{1145.48, 1469.2, 0.0 , 4994.09, 4438.99, },
				{1381.74, 1729.11, 0.0 , 6936.52, 3880.66, },
				{1406.9, 3032.4, 0.0 , 9854.05, 4191.19, },
				{2415.59, 4470.42, 0.0 , 13041.5, 2382.44, },
				{3441.18, 6722.46, 0.0 , 13403.3, 0, },
				{4967.09, 5577.8, 0.0 , 14232.4, 0, }		};


				static double ff_c_reta_0[15][5]    = {{-0.00669569, -0.00769955, 0.0 , -0.00895137, -0.00684947},
				{-0.00174075, -0.00746357, 0.0 , -0.0166336, -0.00982338},
				{-0.00353235, -0.00747305, 0.0 , -0.0117531, -0.0117093},
				{-0.00364298, -0.00746787, 0.0 , -0.0112496, -0.0109245},
				{-0.00413704, -0.00848514, 0.0 , -0.0111741, -0.0114245},
				{-0.00530422, -0.00725818, 0.0 , -0.0099262, -0.0121607},
				{-0.00473034, -0.00634456, 0.0 , -0.0110427, -0.0106177},
				{-0.00484556, -0.00805265, 0.0 , -0.0117719, -0.0108848},
				{-0.00581801, -0.00769514, 0.0 , -0.0118123, -0.0112793},
				{-0.00519371, -0.00655347, 0.0 , -0.012536, -0.00999516},
				{-0.00561816, -0.00748789, 0.0 , -0.010067, -0.0102658},
				{-0.00502104, -0.00831085, 0.0 , -0.0123059, -0.00896555},
				{-0.00542802, -0.00866467, 0.0 , -0.00962681, -0.0102351},
				{-0.00526196, -0.00694442, 0.0 , -0.0131562, -0.011188},
				{-0.00471318, -0.00819421, 0.0 , -0.0103003, -0.0108964}		};


				static double fferr_c_reta_0[15][5]    = {{0.00203414, 0.00088326, 0.0 , 0.000847716, 0.000574521, },
				{0.00366793, 0.00208656, 0.0 , 0.00252125, 0.00173709, },
				{0.000573258, 0.000330829, 0.0 , 0.000456619, 0.000348472, },
				{0.000695493, 0.000440371, 0.0 , 0.000711924, 0.00050603, },
				{0.000800246, 0.00058535, 0.0 , 0.00104904, 0.000650668, },
				{0.0010917, 0.000715373, 0.0 , 0.00138879, 0.000945077, },
				{0.00124055, 0.000936936, 0.0 , 0.0017919, 0.000987423, },
				{0.000521494, 0.00039266, 0.0 , 0.000691502, 0.000544822, },
				{0.00059825, 0.000439268, 0.0 , 0.000895143, 0.000689021, },
				{0.000976323, 0.000796868, 0.0 , 0.00112248, 0.00123851, },
				{0.00077899, 0.000680998, 0.0 , 0.00107095, 0.000945613, },
				{0.00095655, 0.000742974, 0.0 , 0.00127056, 0.00124089, },
				{0.00120022, 0.000772366, 0.0 , 0.00167379, 0.00188265, },
				{0.000888741, 0.000917726, 0.0 , 0.00182499, 0.00188752, },
				{0.000659408, 0.000784536, 0.0 , 0.00119454, 0.00159891, }		};


				static double ff_c_rphi_0[15][5]    = {{-0.0121642, -0.0133714, 0.0 , -0.00845546, -0.00878078},
				{0.017596, -0.0106719, 0.0 , 0.00174105, -0.0188595},
				{-0.00716972, -0.00748992, 0.0 , -0.0168079, -0.0122809},
				{-0.0162054, -0.0134668, 0.0 , -0.0168058, -0.00932646},
				{-0.0171915, -0.0192338, 0.0 , -0.0128222, -0.011477},
				{0.00106925, -0.0128191, 0.0 , -0.00775439, -0.00240481},
				{0.00514418, -0.00667948, 0.0 , 0.00543797, -0.00909787},
				{-0.00416613, -0.00252634, 0.0 , 0.000512362, -0.00419998},
				{-0.00517762, -0.0029577, 0.0 , -0.00333422, -0.00731343},
				{-0.00432593, -0.00832689, 0.0 , -0.00759047, -0.00619233},
				{-0.0014897, -0.00353181, 0.0 , -0.000588059, -0.0065943},
				{-0.0050059, -0.00498551, 0.0 , -0.00305718, -0.00489056},
				{-0.0118812, -0.0031141, 0.0 , -0.00793666, -0.00806504},
				{-0.00520885, -0.00514621, 0.0 , -0.00869012, -0.00566143},
				{-0.0043537, -0.0062288, 0.0 , -0.00662583, -0.00726759}		};


				static double fferr_c_rphi_0[15][5]    = {{0.00449247, 0.00173156, 0.0 , 0.00192578, 0.00162946, },
				{0.0280056, 0.0106167, 0.0 , 0.0157169, 0.00943754, },
				{0.00357805, 0.00187352, 0.0 , 0.00232485, 0.0016503, },
				{0.00504079, 0.00253807, 0.0 , 0.0032725, 0.00210281, },
				{0.00714157, 0.00326615, 0.0 , 0.00429448, 0.0020223, },
				{0.00852127, 0.00432726, 0.0 , 0.00580422, 0.00349559, },
				{0.0100544, 0.00500652, 0.0 , 0.00680075, 0.00260472, },
				{0.00398804, 0.00229927, 0.0 , 0.00321634, 0.00131172, },
				{0.00303024, 0.00237425, 0.0 , 0.00267085, 0.00116956, },
				{0.00418036, 0.00204212, 0.0 , 0.00208454, 0.00226893, },
				{0.00384561, 0.00222793, 0.0 , 0.0046508, 0.00131023, },
				{0.00217538, 0.00212415, 0.0 , 0.00230496, 0.0013869, },
				{0.00207891, 0.00282755, 0.0 , 0.00188341, 0.00225156, },
				{0.00172663, 0.00312126, 0.0 , 0.00321313, 0.00178642, },
				{0.00120243, 0.00126862, 0.0 , 0.00178394, 0.00154664, }		};


				static double ff_c_weta2_0[15][5]    = {{0.00037854, 0.000379791, 0.0 , 0.000464386, 0.000313411},
				{0.000210977, 0.000174891, 0.0 , 0.000781563, 0.000421401},
				{0.000252594, 0.000338228, 0.0 , 0.000562963, 0.000555791},
				{0.000265466, 0.000301982, 0.0 , 0.000595583, 0.000484822},
				{0.000210277, 0.000270099, 0.0 , 0.000450018, 0.000456692},
				{0.000288446, 0.000288441, 0.0 , 0.000569489, 0.000521109},
				{0.000147069, 0.000142789, 0.0 , 0.00055636, 0.000399848},
				{0.000216867, 0.000310902, 0.0 , 0.000599699, 0.000436417},
				{0.00018279, 0.000247099, 0.0 , 0.000612498, 0.000541396},
				{0.00037759, 0.000271818, 0.0 , 0.000662521, 0.000433222},
				{0.000312335, 0.000336043, 0.0 , 0.000446449, 0.00040307},
				{0.00017615, 0.000372329, 0.0 , 0.000591191, 0.000286262},
				{0.000235946, 0.000338168, 0.0 , 0.000585613, 0.000495125},
				{0.000188489, 0.000287789, 0.0 , 0.000735307, 0.000523293},
				{0.0002956, 0.000299294, 0.0 , 0.000784857, 0.000530182}		};


				static double fferr_c_weta2_0[15][5]    = {{0.000108914, 3.83486e-05, 0.0 , 3.93362e-05, 2.6447e-05, },
				{0.000315404, 0.000122915, 0.0 , 0.000152201, 9.41789e-05, },
				{2.93282e-05, 1.72517e-05, 0.0 , 2.78756e-05, 2.303e-05, },
				{3.51324e-05, 2.61018e-05, 0.0 , 3.79274e-05, 3.45866e-05, },
				{4.7606e-05, 3.49831e-05, 0.0 , 7.05949e-05, 4.57544e-05, },
				{5.24081e-05, 4.42093e-05, 0.0 , 6.65515e-05, 6.37349e-05, },
				{7.67572e-05, 5.48759e-05, 0.0 , 9.87625e-05, 8.04225e-05, },
				{3.10081e-05, 2.5589e-05, 0.0 , 4.05224e-05, 3.61132e-05, },
				{3.59009e-05, 3.39039e-05, 0.0 , 5.11891e-05, 5.00761e-05, },
				{5.10322e-05, 4.15915e-05, 0.0 , 9.18777e-05, 8.48523e-05, },
				{4.75078e-05, 4.76227e-05, 0.0 , 8.67937e-05, 7.82118e-05, },
				{5.04131e-05, 5.31084e-05, 0.0 , 0.000108347, 8.43908e-05, },
				{9.55141e-05, 7.01742e-05, 0.0 , 0.000122697, 0.000144444, },
				{6.20522e-05, 6.4247e-05, 0.0 , 0.000143766, 0.000152077, },
				{4.46393e-05, 5.03732e-05, 0.0 , 0.00011446, 0.000127187, }		};


				static double ff_c_f1_0[15][5]    = {{-0.00283736, -0.00224003, 0.0 , 0.00280136, -0.0261427},
				{0.0237384, -0.0185746, 0.0 , 0.0187699, 0.00487632},
				{-0.00264636, 0.00041911, 0.0 , 0.0107647, 0.0179561},
				{-0.00842622, -0.000411332, 0.0 , 0.0122931, 0.0104484},
				{-0.00500602, 0.00365391, 0.0 , 0.00845629, 0.0104308},
				{0.00369114, -0.00115415, 0.0 , 0.0084424, 0.0176243},
				{-0.018076, -0.0182971, 0.0 , 0.0109882, 0.00579551},
				{0.00177297, -0.00206545, 0.0 , 0.00920016, 0.00998327},
				{0.00047645, -0.00727859, 0.0 , 0.00264901, 0.0164422},
				{0.0138007, -0.0147898, 0.0 , 0.0120805, 0.0170017},
				{0.0109786, -0.00591448, 0.0 , 0.0199934, 0.00841133},
				{-0.0105802, -0.00639421, 0.0 , 0.0266825, 0.00982489},
				{0.0347409, -0.00691761, 0.0 , -0.00753433, 0.00640576},
				{-0.0169988, -0.00481574, 0.0 , 0.0451113, 0.0142405},
				{0.00960271, 0.0193844, 0.0 , 0.0109586, 0.0081856}		};


				static double fferr_c_f1_0[15][5]    = {{0.00641471, 0.00254115, 0.0 , 0.00282883, 0.00293986, },
				{0.0365052, 0.0108067, 0.0 , 0.0143442, 0.0113363, },
				{0.00382968, 0.0020265, 0.0 , 0.00263497, 0.00251219, },
				{0.00480578, 0.00306091, 0.0 , 0.00399328, 0.00397472, },
				{0.00686401, 0.00462951, 0.0 , 0.00565747, 0.0053819, },
				{0.00775656, 0.00617447, 0.0 , 0.00791032, 0.00620097, },
				{0.0108446, 0.00756743, 0.0 , 0.00944674, 0.00811273, },
				{0.00518142, 0.00336358, 0.0 , 0.00531947, 0.00390064, },
				{0.00634059, 0.004482, 0.0 , 0.00584753, 0.00490345, },
				{0.00976269, 0.00649539, 0.0 , 0.0097156, 0.00919306, },
				{0.0093383, 0.00640718, 0.0 , 0.0107045, 0.00748049, },
				{0.00951918, 0.0080179, 0.0 , 0.0137986, 0.00896775, },
				{0.018353, 0.0104704, 0.0 , 0.0202819, 0.012952, },
				{0.0126276, 0.0115627, 0.0 , 0.0261085, 0.0130777, },
				{0.00860782, 0.00752338, 0.0 , 0.0142694, 0.013026, }		};


				static double ff_c_fside_0[15][5]    = {{0.0117931, 0.027173, 0.0 , 0.034304, 0.0198009},
				{0.028909, -0.0017229, 0.0 , 0.0410852, 0.0196639},
				{0.00384039, 0.0146541, 0.0 , 0.035002, 0.0170748},
				{0.00542456, 0.00863776, 0.0 , 0.0317358, 0.0152968},
				{0.00298539, 0.0127148, 0.0 , 0.0331915, 0.0202067},
				{0.0088587, 0.0109771, 0.0 , 0.025674, 0.0154183},
				{0.00129274, 0.00555524, 0.0 , 0.0253471, 0.0199585},
				{0.00442399, 0.0100391, 0.0 , 0.033429, 0.0209125},
				{0.00439371, 0.00958657, 0.0 , 0.0368572, 0.0197004},
				{0.00706698, 0.00738993, 0.0 , 0.0445012, 0.0241665},
				{0.00534436, 0.0219429, 0.0 , 0.0253268, 0.0237611},
				{0.01553, 0.0180061, 0.0 , 0.0289601, 0.0269872},
				{0.0231747, 0.0162185, 0.0 , 0.0348702, 0.0300372},
				{0.00722642, 0.0221267, 0.0 , 0.0572183, 0.0293473},
				{0.0068334, 0.0179781, 0.0 , 0.0371346, 0.0245385}		};


				static double fferr_c_fside_0[15][5]    = {{0.00402627, 0.00304474, 0.0 , 0.00294836, 0.00131989, },
				{0.0137158, 0.0094353, 0.0 , 0.0183462, 0.0039102, },
				{0.00185586, 0.00202187, 0.0 , 0.00308246, 0.000946752, },
				{0.00250346, 0.00300067, 0.0 , 0.00512457, 0.00147749, },
				{0.00312019, 0.0044333, 0.0 , 0.00774748, 0.00195244, },
				{0.00419066, 0.0057585, 0.0 , 0.0101657, 0.0026559, },
				{0.00492168, 0.00775995, 0.0 , 0.0131985, 0.00321452, },
				{0.00245321, 0.00324213, 0.0 , 0.00578356, 0.00160592, },
				{0.00337258, 0.00395173, 0.0 , 0.00595459, 0.00202552, },
				{0.00482976, 0.00632314, 0.0 , 0.0110115, 0.00330076, },
				{0.00434531, 0.00555473, 0.0 , 0.0121566, 0.00303574, },
				{0.00382447, 0.00686795, 0.0 , 0.0145342, 0.00417607, },
				{0.00806898, 0.00836, 0.0 , 0.0120577, 0.00400352, },
				{0.00471457, 0.00792554, 0.0 , 0.0193428, 0.00645432, },
				{0.00378255, 0.00611014, 0.0 , 0.00981957, 0.00628949, }		};


				static double ff_c_wtot_0[15][5]    = {{0.101794, 0.0630041, 0.0 , 0.114027, 0.0939065},
				{0.139312, 0.090718, 0.0 , 0.114822, 0.0859762},
				{0.0794425, 0.0568368, 0.0 , 0.0891588, 0.0556554},
				{0.0938549, 0.035424, 0.0 , 0.116628, 0.0627699},
				{0.070709, 0.0537314, 0.0 , 0.100024, 0.098972},
				{0.10349, 0.0468297, 0.0 , 0.116405, 0.071943},
				{0.0726359, 0.0763798, 0.0 , 0.111077, 0.0843556},
				{0.0817552, 0.0655446, 0.0 , 0.113709, 0.0887531},
				{0.0795497, 0.0452924, 0.0 , 0.103624, 0.0760713},
				{0.0956447, 0.0699735, 0.0 , 0.143626, 0.118032},
				{0.0834334, 0.13243, 0.0 , 0.0925939, 0.115555},
				{0.0975579, 0.100871, 0.0 , 0.139713, 0.163578},
				{0.126424, 0.0927033, 0.0 , 0.129371, 0.132881},
				{0.109142, 0.0861337, 0.0 , 0.132405, 0.135559},
				{0.0911722, 0.0781298, 0.0 , 0.143042, 0.0677646}		};


				static double fferr_c_wtot_0[15][5]    = {{0.0311168, 0.0149986, 0.0 , 0.0133245, 0.00799189, },
				{0.0653826, 0.0363188, 0.0 , 0.0634442, 0.0234924, },
				{0.0103999, 0.00713036, 0.0 , 0.00960851, 0.00620763, },
				{0.0139088, 0.0101043, 0.0 , 0.015615, 0.00966886, },
				{0.017804, 0.0133134, 0.0 , 0.0205962, 0.0121622, },
				{0.0204458, 0.0197092, 0.0 , 0.0254941, 0.0158558, },
				{0.0254205, 0.0269573, 0.0 , 0.0339122, 0.0212615, },
				{0.0115735, 0.0113006, 0.0 , 0.0167688, 0.0111753, },
				{0.0144778, 0.0141887, 0.0 , 0.0183137, 0.013706, },
				{0.0250281, 0.0227677, 0.0 , 0.0379727, 0.0234641, },
				{0.0191699, 0.0234551, 0.0 , 0.0351792, 0.0217587, },
				{0.0162341, 0.0263711, 0.0 , 0.0350468, 0.030118, },
				{0.0324693, 0.0357352, 0.0 , 0.041506, 0.0376177, },
				{0.021822, 0.0381793, 0.0 , 0.0808082, 0.0497097, },
				{0.0158651, 0.0264171, 0.0 , 0.0377322, 0.0454252, }		};


				static double ff_c_w1_0[15][5]    = {{0.0130479, 0.0190681, 0.0 , 0.0134295, 0.0282562},
				{0.00659782, 0.00236171, 0.0 , 0.00397986, 0.0333766},
				{-0.00135899, 0.00608802, 0.0 , 0.00794172, 0.0313606},
				{-0.00235909, 0.00468439, 0.0 , 0.00685555, 0.0260294},
				{0.000489295, 0.0131845, 0.0 , 0.00683147, 0.0287299},
				{0.00790083, 0.00291884, 0.0 , -0.000164509, 0.0262943},
				{-0.00159639, 0.00410116, 0.0 , 0.00628883, 0.0186517},
				{-0.00233752, 0.00591838, 0.0 , 0.00788301, 0.0291774},
				{0.00030148, 0.0045144, 0.0 , 0.00433415, 0.0271631},
				{0.0102981, 0.00575161, 0.0 , 0.00885594, 0.0357977},
				{0.000942647, 0.0118216, 0.0 , 0.00417745, 0.024285},
				{0.00635159, 0.00714421, 0.0 , 0.00759715, 0.0338098},
				{0.00972402, 0.0077405, 0.0 , 0.0100828, 0.0291659},
				{-0.00532937, 0.0107045, 0.0 , 0.0287367, 0.0424647},
				{-0.000653267, 0.011613, 0.0 , 0.0198708, 0.0295559}		};


				static double fferr_c_w1_0[15][5]    = {{0.0036671, 0.00200489, 0.0 , 0.0018071, 0.00165194, },
				{0.013772, 0.00635643, 0.0 , 0.0112146, 0.00599916, },
				{0.00170189, 0.00120666, 0.0 , 0.00168798, 0.0013629, },
				{0.00227578, 0.00168524, 0.0 , 0.00264206, 0.00212602, },
				{0.00321468, 0.00288634, 0.0 , 0.00357687, 0.00293128, },
				{0.00332861, 0.00332228, 0.0 , 0.00424986, 0.00388685, },
				{0.00357018, 0.00432623, 0.0 , 0.0061601, 0.00551187, },
				{0.00225931, 0.00197914, 0.0 , 0.00301753, 0.00243289, },
				{0.00267423, 0.00246292, 0.0 , 0.00338393, 0.00284849, },
				{0.00415338, 0.00360016, 0.0 , 0.00481465, 0.0048927, },
				{0.00347044, 0.00359132, 0.0 , 0.00713989, 0.00507683, },
				{0.00363172, 0.00443551, 0.0 , 0.00677243, 0.00573468, },
				{0.00843942, 0.0063613, 0.0 , 0.00818696, 0.00920481, },
				{0.00477768, 0.00656149, 0.0 , 0.0118262, 0.0082087, },
				{0.00384602, 0.00444002, 0.0 , 0.00628305, 0.00663361, }		};


				static double ff_c_deltae_0[15][5]    = {{-1.81888, -2.00029, 0.0 , -2.83976, -2.26994},
				{2.76943, -0.047224, 0.0 , -0.73307, -0.189499},
				{-1.97039, -1.64385, 0.0 , -1.08803, -4.18694},
				{1.60711, -1.8798, 0.0 , 0.0157242, -2.00836},
				{-0.917854, 0.77734, 0.0 , -1.56894, -3.05904},
				{-0.81616, 0.741528, 0.0 , -3.63797, -2.76846},
				{-2.43999, 1.75037, 0.0 , 0.469044, 0.198624},
				{-1.60837, -2.51659, 0.0 , 0.37788, -1.8405},
				{-2.47035, -0.852955, 0.0 , -0.730568, -1.83205},
				{-5.94399, -0.37619, 0.0 , 4.49932, -1.5727},
				{2.29242, -2.9033, 0.0 , -1.30704, -1.84146},
				{-2.85843, 0.251028, 0.0 , -11.243, -0.707695},
				{-2.08364, -4.84029, 0.0 , 7.05597, -3.26838},
				{1.75637, -1.40112, 0.0 , 8.42484, -3.73875},
				{0.848095, -0.990803, 0.0 , 7.94258, -6.39232}		};


				static double fferr_c_deltae_0[15][5]    = {{1.01103, 0.481089, 0.0 , 0.820577, 0.56055, },
				{5.75925, 2.20241, 0.0 , 6.20305, 2.70446, },
				{0.876233, 0.522648, 0.0 , 1.06662, 0.729066, },
				{1.05802, 0.805273, 0.0 , 1.65891, 0.98932, },
				{1.83047, 1.03861, 0.0 , 2.72972, 1.54566, },
				{1.96391, 1.24773, 0.0 , 3.09241, 1.90327, },
				{2.70863, 2.02313, 0.0 , 3.67598, 2.18537, },
				{1.52152, 0.888505, 0.0 , 2.13293, 1.35734, },
				{1.60559, 1.15667, 0.0 , 2.49683, 1.59146, },
				{3.08029, 1.86991, 0.0 , 4.18426, 3.36326, },
				{2.34333, 1.89033, 0.0 , 4.41167, 2.77623, },
				{3.30546, 2.13153, 0.0 , 5.40905, 4.11952, },
				{5.66516, 4.16179, 0.0 , 7.22957, 4.16186, },
				{5.24577, 3.83515, 0.0 , 5.71073, 4.5267, },
				{3.07996, 2.77761, 0.0 , 5.25998, 4.61251, }		};


				static double ff_c_eratio_0[15][5]    = {{-0.000185132, -0.00194544, 0.0 , 0.00307775, 0.000129342},
				{-0.00228506, -0.0120177, 0.0 , -0.00361657, -0.00227308},
				{-0.00250059, -0.00158411, 0.0 , -0.00189799, 0.000683546},
				{-0.00283235, -0.0035013, 0.0 , -0.00502694, -0.000247478},
				{-0.00201809, -0.00393951, 0.0 , -0.00810677, -0.00147033},
				{-0.000759959, 0.00258738, 0.0 , -0.00838912, -0.000495672},
				{-0.00214422, 0.000211298, 0.0 , -0.00650656, -0.00118202},
				{-0.000347614, -0.000622451, 0.0 , -0.00437433, -0.000978649},
				{-0.00266755, -0.00343215, 0.0 , -0.00182676, -0.000411808},
				{-0.00107312, -0.00374258, 0.0 , -0.00563174, 0.000515282},
				{-0.0018639, -0.00178629, 0.0 , -0.00389701, -0.00138408},
				{-0.00062871, -0.00240278, 0.0 , -0.00100315, -0.00114465},
				{0.00106055, -0.00445509, 0.0 , -0.00351161, -0.00122416},
				{-0.00363231, -0.00458574, 0.0 , -0.00446445, -0.000830114},
				{-0.00236303, -0.00106966, 0.0 , -0.00162917, 0.00082314}		};


				static double fferr_c_eratio_0[15][5]    = {{0.00122212, 0.00134505, 0.0 , 0.0021632, 0.000543174, },
				{0.00553001, 0.00468496, 0.0 , 0.00746248, 0.00134363, },
				{0.000873251, 0.00102731, 0.0 , 0.00202056, 0.000314493, },
				{0.00118727, 0.00142623, 0.0 , 0.00242344, 0.000418664, },
				{0.00142645, 0.00182086, 0.0 , 0.00269218, 0.000508046, },
				{0.00172553, 0.00270336, 0.0 , 0.00247072, 0.000671051, },
				{0.00239112, 0.00304382, 0.0 , 0.00190024, 0.000864821, },
				{0.00105535, 0.00127491, 0.0 , 0.00149412, 0.000472513, },
				{0.00109593, 0.00142005, 0.0 , 0.00166891, 0.000474832, },
				{0.00150479, 0.00210279, 0.0 , 0.00148783, 0.000884517, },
				{0.00116829, 0.00224704, 0.0 , 0.00167614, 0.000589085, },
				{0.00194375, 0.00204001, 0.0 , 0.00238835, 0.000880939, },
				{0.00180863, 0.00298594, 0.0 , 0.00226873, 0.000999901, },
				{0.00209197, 0.00207317, 0.0 , 0.00131764, 0.00108527, },
				{0.001246, 0.00162866, 0.0 , 0.00113184, 0.000867444, }		};

				//unconverted photons

				static double ff_u_rhad1_0[15][5]    = {{-0.0034745, -0.00235099, 0.0 , -0.000155916, -0.000737883},
				{0.000744989, -0.00110784, 0.0 , -0.000556218, 0.00254812},
				{-0.000564042, -0.000485175, 0.0 , -0.000188405, -0.000521},
				{-0.000569941, 0.00011893, 0.0 , -0.00149687, -0.000730097},
				{-0.000341317, -0.000545282, 0.0 , -0.00139224, -0.000389023},
				{-0.000318478, -0.000568973, 0.0 , 0.000856707, -0.000390534},
				{-0.000411879, 0.000213057, 0.0 , 9.53348e-06, -0.000898713},
				{-0.000493028, -6.81262e-05, 0.0 , -0.000145628, -0.000108469},
				{-0.00032398, -0.000576657, 0.0 , 0.000773878, -0.000120155},
				{-0.000295906, -0.000212512, 0.0 , -0.00149979, -0.000379009},
				{-0.00029821, -0.000264573, 0.0 , -0.000388181, 0.000173547},
				{6.31384e-05, 0.000119036, 0.0 , -8.76216e-05, -0.000455548},
				{-0.000633782, -1.18738e-05, 0.0 , 0.000197221, 3.18905e-05},
				{-0.00047793, -0.000129307, 0.0 , -0.00109484, -0.000167764},
				{-0.0004274, 0.000127285, 0.0 , -0.000293432, -0.00025635}		};


				static double fferr_u_rhad1_0[15][5]    = {{0.000335328, 0.000525684, 0.0 , 0.000789336, 0.000428592, },
				{0.000651199, 0.000997868, 0.0 , 0.00168433, 0.000837623, },
				{0.000129737, 0.00018943, 0.0 , 0.000375763, 0.000195553, },
				{0.000181414, 0.000183928, 0.0 , 0.000560332, 0.000310391, },
				{0.000199884, 0.000328912, 0.0 , 0.000681844, 0.000360211, },
				{0.000221769, 0.000496635, 0.0 , 0.000831241, 0.000337506, },
				{0.000311155, 0.000270231, 0.0 , 0.000810708, 0.00066463, },
				{0.000154656, 0.000179112, 0.0 , 0.000454989, 0.000198378, },
				{0.000188669, 0.00037057, 0.0 , 0.000288565, 0.000222832, },
				{0.000191815, 0.000199762, 0.0 , 0.000754792, 0.00033256, },
				{0.000189556, 0.000170938, 0.0 , 0.000534543, 0.000262468, },
				{0.000143883, 0.000126214, 0.0 , 0.000749023, 0.000361342, },
				{0.000382597, 0.000163364, 0.0 , 0.000669588, 0.00031807, },
				{0.000277102, 0.000415423, 0.0 , 0.0011741, 0.000350805, },
				{0.00016787, 0.000136082, 0.0 , 0.000476111, 0.000290159, }		};


				static double ff_u_rhad_0[15][5]    = {{0, -0.00276564, 0.0 , 0, 0},
				{0, -0.00163934, 0.0 , 0, 0},
				{0, -0.000121347, 0.0 , 0, 0},
				{0, -0.00046459, 0.0 , 0, 0},
				{0, -0.000452925, 0.0 , 0, 0},
				{0, -0.000471185, 0.0 , 0, 0},
				{0, -0.000465611, 0.0 , 0, 0},
				{0, -5.76166e-05, 0.0 , 0, 0},
				{0, -0.0001852, 0.0 , 0, 0},
				{0, 0.000279547, 0.0 , 0, 0},
				{0, 1.44355e-05, 0.0 , 0, 0},
				{0, 0.000102757, 0.0 , 0, 0},
				{0, -8.34829e-05, 0.0 , 0, 0},
				{0, -2.23047e-05, 0.0 , 0, 0},
				{0, 5.36031e-06, 0.0 , 0, 0}		};


				static double fferr_u_rhad_0[15][5]    = {{0, 0.000402161, 0.0 , 0, 0, },
				{0, 0.000935684, 0.0 , 0, 0, },
				{0, 0.000135772, 0.0 , 0, 0, },
				{0, 0.000227771, 0.0 , 0, 0, },
				{0, 0.000196654, 0.0 , 0, 0, },
				{0, 0.00031721, 0.0 , 0, 0, },
				{0, 0.00046739, 0.0 , 0, 0, },
				{0, 0.000128714, 0.0 , 0, 0, },
				{0, 0.000188175, 0.0 , 0, 0, },
				{0, 0.000138272, 0.0 , 0, 0, },
				{0, 9.41798e-05, 0.0 , 0, 0, },
				{0, 8.06156e-05, 0.0 , 0, 0, },
				{0, 0.000113067, 0.0 , 0, 0, },
				{0, 0.00011779, 0.0 , 0, 0, },
				{0, 8.46184e-05, 0.0 , 0, 0, }		};


				static double ff_u_e277_0[15][5]    = {{383.674, 834.607, 0.0 , 1801.68, 2417.9},
				{-106.945, 1600.4, 0.0 , 1578.15, 1526.21},
				{244.391, 1068.46, 0.0 , 1157.76, 1440.59},
				{425.203, 1024.02, 0.0 , -23.375, 2288.88},
				{310.352, 631.141, 0.0 , 1871.47, 2008.77},
				{440.285, 1113.95, 0.0 , 1894.77, -1535.41},
				{198.871, -463.273, 0.0 , -7692.29, -1407.67},
				{79.7227, 533.355, 0.0 , 60.6719, -3502.28},
				{-153.34, 1583.26, 0.0 , 1582.92, 1331.8},
				{612.473, 2587.91, 0.0 , -6556.98, 2220.34},
				{79.375, 3368.24, 0.0 , -3091.33, -3052.22},
				{-2137.77, -775.297, 0.0 , -7672.19, -9905.75},
				{459.828, -1955.98, 0.0 , 19103.3, 0},
				{-2737.77, -9666.62, 0.0 , -31616.5, 0},
				{-30426, -12005.8, 0.0 , 0, 0}		};


				static double fferr_u_e277_0[15][5]    = {{34.1137, 50.3862, 0.0 , 130.946, 139.614, },
				{289.204, 630.319, 0.0 , 1392.71, 1427.01, },
				{71.7332, 125.842, 0.0 , 438.332, 416.781, },
				{123.581, 241.947, 0.0 , 832.081, 812.67, },
				{196.094, 420.303, 0.0 , 1525.54, 1289.17, },
				{334.752, 641.21, 0.0 , 2398.77, 2083.63, },
				{497.225, 911.407, 0.0 , 3022.47, 3190.71, },
				{239.435, 478.502, 0.0 , 1898.26, 1813.12, },
				{394.463, 827.943, 0.0 , 2910.44, 2496.09, },
				{667.093, 1549.79, 0.0 , 5711.67, 3711.84, },
				{822.676, 1810.98, 0.0 , 5823.99, 3856.02, },
				{1270.21, 2286.06, 0.0 , 6995.17, 2716.26, },
				{1719.67, 4995.92, 0.0 , 19067.6, 0, },
				{2667.57, 6076.17, 0.0 , 8190.71, 0, },
				{3516.01, 4159.79, 0.0 , 0, 0, }		};


				static double ff_u_reta_0[15][5]    = {{-0.00272906, -0.00425076, 0.0 , -0.00690031, -0.00712144},
				{-0.00160658, -0.00646037, 0.0 , -0.0126932, -0.00702232},
				{-0.00301051, -0.00584674, 0.0 , -0.00873816, -0.00881225},
				{-0.00361222, -0.00639462, 0.0 , -0.00913733, -0.00964051},
				{-0.0035826, -0.00625426, 0.0 , -0.00889099, -0.00927162},
				{-0.00410444, -0.00644976, 0.0 , -0.00908726, -0.00776595},
				{-0.00406504, -0.00658149, 0.0 , -0.0120273, -0.0103697},
				{-0.00451887, -0.00636172, 0.0 , -0.00935656, -0.00985867},
				{-0.00396472, -0.00639236, 0.0 , -0.0102593, -0.0102628},
				{-0.00486279, -0.00657123, 0.0 , -0.0119166, -0.0110399},
				{-0.00452906, -0.00666696, 0.0 , -0.0094521, -0.011026},
				{-0.00385511, -0.00727516, 0.0 , -0.0121187, -0.0124425},
				{-0.00508034, -0.00726122, 0.0 , -0.00803238, -0.0104172},
				{-0.00420606, -0.00763917, 0.0 , -0.0100555, -0.0108979},
				{-0.00425392, -0.00849926, 0.0 , -0.0097068, -0.0122566}		};


				static double fferr_u_reta_0[15][5]    = {{0.00056699, 0.000486066, 0.0 , 0.000676972, 0.000339919, },
				{0.00113827, 0.00138504, 0.0 , 0.00175604, 0.00124807, },
				{0.000241194, 0.000222125, 0.0 , 0.000398101, 0.000304909, },
				{0.000318422, 0.000317227, 0.0 , 0.0005676, 0.000455705, },
				{0.000444494, 0.000437979, 0.0 , 0.000839433, 0.000627607, },
				{0.000602565, 0.000555471, 0.0 , 0.00104785, 0.000878989, },
				{0.000639277, 0.000637127, 0.0 , 0.000945585, 0.00112262, },
				{0.00028696, 0.000286697, 0.0 , 0.000602824, 0.000448162, },
				{0.000362961, 0.000355855, 0.0 , 0.000727835, 0.0005875, },
				{0.000437334, 0.000459862, 0.0 , 0.00111657, 0.000956496, },
				{0.00047933, 0.000501479, 0.0 , 0.000974979, 0.00104565, },
				{0.000421413, 0.000474889, 0.0 , 0.000969606, 0.00107766, },
				{0.000622771, 0.000584736, 0.0 , 0.00226727, 0.00147723, },
				{0.000581201, 0.000811736, 0.0 , 0.0012719, 0.00165942, },
				{0.000397042, 0.000563672, 0.0 , 0.00120967, 0.00141991, }		};


				static double ff_u_rphi_0[15][5]    = {{-0.00200909, -0.00403404, 0.0 , -0.00507373, -0.00525671},
				{-0.00201243, -0.00616097, 0.0 , -0.00609922, -0.00440168},
				{-0.00237793, -0.0028159, 0.0 , -0.00341868, -0.00532329},
				{-0.00206345, -0.00341254, 0.0 , -0.00386977, -0.00654685},
				{-0.00268978, -0.00464666, 0.0 , -0.00416374, -0.00615591},
				{-0.00268376, -0.00476509, 0.0 , -0.00356293, -0.00537473},
				{-0.00286388, -0.00404447, 0.0 , -0.0064078, -0.00680584},
				{-0.00366938, -0.00407338, 0.0 , -0.00383461, -0.00616884},
				{-0.00320297, -0.00500405, 0.0 , -0.00445044, -0.00634241},
				{-0.00250787, -0.00538844, 0.0 , -0.00688708, -0.00632727},
				{-0.00282139, -0.00553232, 0.0 , -0.0043481, -0.00685996},
				{-0.00221699, -0.00461417, 0.0 , -0.00475806, -0.00831842},
				{-0.00275171, -0.00548542, 0.0 , -0.00148606, -0.00460804},
				{-0.00322992, -0.00409353, 0.0 , -0.00644088, -0.0065648},
				{-0.00399965, -0.0054211, 0.0 , -0.00453937, -0.00842947}		};


				static double fferr_u_rphi_0[15][5]    = {{0.00048802, 0.000745826, 0.0 , 0.00060477, 0.000261426, },
				{0.00107874, 0.00179158, 0.0 , 0.00187717, 0.00102703, },
				{0.000214133, 0.000465483, 0.0 , 0.000442323, 0.000260927, },
				{0.000278903, 0.000595405, 0.0 , 0.000667065, 0.00040288, },
				{0.000385138, 0.000651477, 0.0 , 0.0010033, 0.000549966, },
				{0.000538019, 0.000897851, 0.0 , 0.00130826, 0.000770595, },
				{0.000569498, 0.0013737, 0.0 , 0.00140283, 0.000926296, },
				{0.000247373, 0.000472171, 0.0 , 0.000699195, 0.000428027, },
				{0.00030708, 0.000467572, 0.0 , 0.000896661, 0.000532475, },
				{0.000485971, 0.000622211, 0.0 , 0.00130477, 0.000776424, },
				{0.000441994, 0.000706585, 0.0 , 0.000991732, 0.000901856, },
				{0.000465174, 0.000544801, 0.0 , 0.00107114, 0.000887261, },
				{0.000601583, 0.000888738, 0.0 , 0.00272047, 0.00152956, },
				{0.000510977, 0.000950054, 0.0 , 0.00124813, 0.00130969, },
				{0.000472037, 0.000593746, 0.0 , 0.00116724, 0.00135768, }		};


				static double ff_u_weta2_0[15][5]    = {{0.000256944, 0.000287889, 0.0 , 0.000370672, 0.000337016},
				{0.000191036, 0.000223578, 0.0 , 0.000507684, 0.00027054},
				{0.000208458, 0.000261066, 0.0 , 0.000483393, 0.000373533},
				{0.00019185, 0.000271417, 0.0 , 0.000504465, 0.000455286},
				{0.000209056, 0.00026055, 0.0 , 0.000512904, 0.000405461},
				{0.000261803, 0.000315732, 0.0 , 0.000553702, 0.000325901},
				{0.000211469, 0.000294873, 0.0 , 0.000636603, 0.000461037},
				{0.000237744, 0.000250532, 0.0 , 0.00053748, 0.000378497},
				{0.000255191, 0.000301466, 0.0 , 0.000539606, 0.000457522},
				{0.000199503, 0.000297825, 0.0 , 0.000719218, 0.00045057},
				{0.000243892, 0.000325569, 0.0 , 0.000574205, 0.000462417},
				{0.000200487, 0.000263379, 0.0 , 0.000706555, 0.000644976},
				{0.000188447, 0.000315718, 0.0 , 0.000409806, 0.000444856},
				{0.000239731, 0.00031675, 0.0 , 0.000614931, 0.000463183},
				{0.000308163, 0.000370061, 0.0 , 0.000584001, 0.0005279}		};


				static double fferr_u_weta2_0[15][5]    = {{2.86233e-05, 2.03147e-05, 0.0 , 2.55723e-05, 1.59351e-05, },
				{6.24783e-05, 6.78576e-05, 0.0 , 0.000102752, 7.27623e-05, },
				{1.27143e-05, 1.25087e-05, 0.0 , 2.20675e-05, 2.0988e-05, },
				{1.74821e-05, 1.78725e-05, 0.0 , 3.30564e-05, 3.31789e-05, },
				{2.33937e-05, 2.53998e-05, 0.0 , 5.7366e-05, 4.33155e-05, },
				{3.42186e-05, 3.12135e-05, 0.0 , 7.02692e-05, 5.93014e-05, },
				{3.82508e-05, 3.73051e-05, 0.0 , 7.29875e-05, 8.11068e-05, },
				{1.58588e-05, 1.76024e-05, 0.0 , 3.97939e-05, 3.22571e-05, },
				{1.99203e-05, 2.11822e-05, 0.0 , 5.2077e-05, 4.61265e-05, },
				{3.09212e-05, 2.94173e-05, 0.0 , 6.9296e-05, 7.5829e-05, },
				{2.69762e-05, 2.84835e-05, 0.0 , 7.2868e-05, 7.71369e-05, },
				{2.58185e-05, 2.95173e-05, 0.0 , 8.07609e-05, 8.96653e-05, },
				{4.5466e-05, 4.70311e-05, 0.0 , 9.36097e-05, 0.000109581, },
				{3.66397e-05, 4.74827e-05, 0.0 , 0.000102381, 0.000138778, },
				{2.67139e-05, 3.57317e-05, 0.0 , 6.56102e-05, 0.000112815, }		};


				static double ff_u_f1_0[15][5]    = {{-0.0131755, -0.00923616, 0.0 , -0.0161323, -0.0407467},
				{-0.0238507, -0.0347714, 0.0 , 0.01296, -0.032716},
				{-0.0115775, -0.01094, 0.0 , -0.00555271, -0.0118446},
				{-0.0112526, -0.0116678, 0.0 , 0.000586748, -0.00613321},
				{-0.0106624, -0.00728919, 0.0 , -0.00679323, -0.00866227},
				{-0.013451, -0.00666972, 0.0 , -0.00128117, -0.0114961},
				{-0.00808619, 0.00240634, 0.0 , 0.0407445, 0.00746675},
				{-0.00402491, -0.0100703, 0.0 , -0.000815719, 0.00362211},
				{0.00113527, -0.0146121, 0.0 , 0.00190702, -0.00321773},
				{-0.00370705, -0.0111584, 0.0 , 0.0317247, 0.000742704},
				{-0.00523086, -0.00801635, 0.0 , 0.00309137, 0.00695418},
				{-0.00138007, 0.00133362, 0.0 , 0.0310667, 0.0266611},
				{-0.00310862, -0.00374267, 0.0 , -0.0336001, -0.0171966},
				{-0.00286631, -0.00186023, 0.0 , 0.0257831, 0.012504},
				{0.0143795, 0.0125523, 0.0 , -0.00529353, 0.019855}		};


				static double fferr_u_f1_0[15][5]    = {{0.00265725, 0.00202228, 0.0 , 0.00289171, 0.00225609, },
				{0.0125232, 0.0115334, 0.0 , 0.0166793, 0.0109863, },
				{0.002545, 0.00208591, 0.0 , 0.00369829, 0.00289457, },
				{0.00355159, 0.00326872, 0.0 , 0.00604933, 0.00419218, },
				{0.0050171, 0.00465475, 0.0 , 0.00913379, 0.00583029, },
				{0.00742029, 0.00600509, 0.0 , 0.0118232, 0.00803833, },
				{0.00894437, 0.00749609, 0.0 , 0.0154068, 0.010635, },
				{0.00368999, 0.00333992, 0.0 , 0.00769132, 0.00469617, },
				{0.00470218, 0.00427995, 0.0 , 0.00858667, 0.00603176, },
				{0.00730249, 0.00649601, 0.0 , 0.0158299, 0.00810659, },
				{0.00682323, 0.00643103, 0.0 , 0.013176, 0.00952799, },
				{0.00688767, 0.00608388, 0.0 , 0.0156892, 0.00799089, },
				{0.00924441, 0.0110844, 0.0 , 0.0238869, 0.0104322, },
				{0.0086091, 0.00988593, 0.0 , 0.0224885, 0.0134254, },
				{0.00599958, 0.00613788, 0.0 , 0.0135026, 0.0116243, }		};


				static double ff_u_fside_0[15][5]    = {{0.0126567, 0.0174359, 0.0 , 0.0265573, 0.0136302},
				{-0.000348568, 0.0133792, 0.0 , 0.0432564, 0.0139614},
				{0.00270849, 0.00356826, 0.0 , 0.0183387, 0.0116096},
				{-9.67681e-05, 0.00547104, 0.0 , 0.0286636, 0.0132331},
				{0.00295904, 0.00361353, 0.0 , 0.0133213, 0.0111157},
				{0.00300035, 0.00599517, 0.0 , 0.0262514, 0.0158569},
				{0.00520264, 0.0180119, 0.0 , 0.0279263, 0.0148633},
				{0.0024308, 0.00622112, 0.0 , 0.0236876, 0.016973},
				{0.00244296, 0.00837383, 0.0 , 0.0248954, 0.0166508},
				{0.006321, 0.0138649, 0.0 , 0.0426916, 0.0180359},
				{0.00147688, 0.0175143, 0.0 , 0.0284578, 0.0150501},
				{0.00137943, 0.00443557, 0.0 , 0.0444934, 0.0187515},
				{0.00256756, 0.0180782, 0.0 , 0.0204745, 0.0148399},
				{0.00348061, 0.00790867, 0.0 , 0.0420924, 0.0185277},
				{0.00523445, 0.0180659, 0.0 , 0.0228496, 0.0186584}		};


				static double fferr_u_fside_0[15][5]    = {{0.00118564, 0.00146468, 0.0 , 0.00236676, 0.000740257, },
				{0.0039044, 0.00825869, 0.0 , 0.0132266, 0.00290875, },
				{0.00106179, 0.00156828, 0.0 , 0.00340245, 0.000778269, },
				{0.00157261, 0.00236488, 0.0 , 0.00485069, 0.0012602, },
				{0.00209727, 0.00348393, 0.0 , 0.00780555, 0.00160851, },
				{0.00291736, 0.00424559, 0.0 , 0.00970641, 0.00216558, },
				{0.00359546, 0.00500118, 0.0 , 0.0138768, 0.00311742, },
				{0.00159818, 0.00241069, 0.0 , 0.00675791, 0.00144988, },
				{0.00201098, 0.00266745, 0.0 , 0.00706308, 0.00169091, },
				{0.00294539, 0.00354179, 0.0 , 0.0102116, 0.00298638, },
				{0.00276571, 0.00366504, 0.0 , 0.0080282, 0.0026234, },
				{0.00278793, 0.00397182, 0.0 , 0.00914537, 0.00333541, },
				{0.0038439, 0.00560334, 0.0 , 0.0153157, 0.00476715, },
				{0.00318695, 0.00532691, 0.0 , 0.0106543, 0.00565249, },
				{0.0026377, 0.00426211, 0.0 , 0.00685385, 0.00463401, }		};


				static double ff_u_wtot_0[15][5]    = {{0.0498329, 0.0771685, 0.0 , 0.0843751, 0.071798},
				{0.131487, 0.0721399, 0.0 , 0.24562, 0.10951},
				{0.0670028, 0.0541846, 0.0 , 0.101668, 0.0689237},
				{0.0411838, 0.058804, 0.0 , 0.114743, 0.0692112},
				{0.0581766, 0.0537953, 0.0 , 0.0805992, 0.0647044},
				{0.0615263, 0.0775042, 0.0 , 0.103154, 0.094525},
				{0.081843, 0.0903391, 0.0 , 0.128242, 0.0836093},
				{0.0645174, 0.0610784, 0.0 , 0.109357, 0.103824},
				{0.0737532, 0.0779748, 0.0 , 0.126693, 0.0881898},
				{0.0633897, 0.0907177, 0.0 , 0.115411, 0.0935789},
				{0.0779537, 0.101344, 0.0 , 0.163666, 0.0842601},
				{0.0329025, 0.074824, 0.0 , 0.1487, 0.0790747},
				{0.0668129, 0.043818, 0.0 , 0.137709, 0.047681},
				{0.0775421, 0.0804719, 0.0 , 0.182331, 0.10549},
				{0.0290289, 0.0671611, 0.0 , 0.146889, 0.0904267}		};


				static double fferr_u_wtot_0[15][5]    = {{0.0109561, 0.00872764, 0.0 , 0.0118836, 0.00543019, },
				{0.0357304, 0.0367431, 0.0 , 0.0582209, 0.0212879, },
				{0.00767306, 0.0066856, 0.0 , 0.0109103, 0.00591162, },
				{0.00992732, 0.00971953, 0.0 , 0.0162325, 0.00933045, },
				{0.0129004, 0.0128827, 0.0 , 0.024323, 0.0125469, },
				{0.0182705, 0.0189983, 0.0 , 0.0347042, 0.0158022, },
				{0.0232735, 0.0218319, 0.0 , 0.0413642, 0.0182952, },
				{0.00859568, 0.00974597, 0.0 , 0.0198864, 0.00999362, },
				{0.012094, 0.0133886, 0.0 , 0.0222025, 0.013189, },
				{0.0167023, 0.0172322, 0.0 , 0.0346115, 0.0253143, },
				{0.0158597, 0.0178011, 0.0 , 0.0288662, 0.0253641, },
				{0.0171742, 0.0190314, 0.0 , 0.0404626, 0.025257, },
				{0.0224247, 0.0283379, 0.0 , 0.0445854, 0.0385841, },
				{0.0207507, 0.0288053, 0.0 , 0.0351135, 0.0562322, },
				{0.015741, 0.0208077, 0.0 , 0.0325392, 0.0434768, }		};


				static double ff_u_w1_0[15][5]    = {{0.0127671, 0.014631, 0.0 , 0.0128657, 0.0270206},
				{-0.00489974, 0.00257605, 0.0 , 0.0222117, 0.024223},
				{-0.00442821, 0.00241244, 0.0 , 0.000848114, 0.0211806},
				{-0.0048973, 0.00140035, 0.0 , 0.00873113, 0.0248568},
				{-0.0053885, 0.00415397, 0.0 , 0.00263399, 0.0182945},
				{-0.0073694, 0.00505847, 0.0 , 0.00466305, 0.0262889},
				{-0.00189924, 0.00950712, 0.0 , 0.0151696, 0.0291376},
				{-0.0033204, 0.000760972, 0.0 , 0.00311208, 0.0244524},
				{-0.0028246, 0.00222456, 0.0 , 0.0103042, 0.022359},
				{-0.00358999, 0.00769144, 0.0 , 0.0114753, 0.0228748},
				{-0.00386173, 0.00959212, 0.0 , 0.0106937, 0.0234746},
				{-0.00261867, 0.00629479, 0.0 , 0.0235783, 0.0228831},
				{-0.00486869, 0.00743222, 0.0 , -0.00228995, 0.0249979},
				{-0.00506151, 0.00427204, 0.0 , 0.0168156, 0.0373782},
				{0.000977159, 0.0146682, 0.0 , 0.0106578, 0.0250539}		};


				static double fferr_u_w1_0[15][5]    = {{0.00124294, 0.00114051, 0.0 , 0.00160197, 0.00112997, },
				{0.00341691, 0.00489875, 0.0 , 0.00792577, 0.00534202, },
				{0.000971469, 0.00107945, 0.0 , 0.00182584, 0.00133037, },
				{0.00138554, 0.00166176, 0.0 , 0.00291215, 0.00191138, },
				{0.0020997, 0.00244733, 0.0 , 0.00456373, 0.00267271, },
				{0.00283735, 0.00295246, 0.0 , 0.00647572, 0.00343484, },
				{0.00353605, 0.00410807, 0.0 , 0.00715126, 0.00435641, },
				{0.00146326, 0.00170555, 0.0 , 0.00346052, 0.00211122, },
				{0.00193219, 0.00206032, 0.0 , 0.00408984, 0.00243911, },
				{0.00275141, 0.00284453, 0.0 , 0.00817626, 0.0039474, },
				{0.00310898, 0.00306746, 0.0 , 0.00584748, 0.00453688, },
				{0.00319409, 0.00321458, 0.0 , 0.0077272, 0.00542755, },
				{0.00419259, 0.00522646, 0.0 , 0.00903935, 0.00654796, },
				{0.00360392, 0.00516277, 0.0 , 0.00819605, 0.00685457, },
				{0.0028106, 0.00335465, 0.0 , 0.00727795, 0.00626777, }		};


				static double ff_u_deltae_0[15][5]    = {{-1.30449, -1.00222, 0.0 , -1.59133, -3.62886},
				{-0.23493, -1.32058, 0.0 , 3.54696, -2.99889},
				{-1.27765, -0.703552, 0.0 , -2.36687, -3.05545},
				{-0.176476, -0.584356, 0.0 , -1.43178, -2.71582},
				{-1.34034, -1.28671, 0.0 , -2.08207, -4.33499},
				{-0.149544, -0.882389, 0.0 , -2.5738, -3.48497},
				{-2.67961, 0.0925331, 0.0 , 7.35751, -2.13843},
				{-0.772747, -1.28329, 0.0 , 1.47847, -4.38502},
				{1.10107, -2.49722, 0.0 , 2.08765, 3.59121},
				{-0.769932, 0.0701084, 0.0 , 4.37979, 2.03495},
				{-2.46688, -2.11826, 0.0 , 0.381495, 0.427664},
				{-0.234692, 1.46091, 0.0 , 4.3073, 0.403223},
				{-3.90747, -1.27537, 0.0 , -0.218199, 2.38044},
				{-3.2891, -4.53504, 0.0 , 2.86215, -2.25544},
				{-4.16369, -1.22514, 0.0 , 1.75497, -0.544491}		};


				static double fferr_u_deltae_0[15][5]    = {{0.327689, 0.250787, 0.0 , 0.648063, 0.458633, },
				{2.00004, 2.11158, 0.0 , 2.91293, 3.54458, },
				{0.450528, 0.370306, 0.0 , 0.877913, 0.760247, },
				{0.610814, 0.578408, 0.0 , 1.40245, 1.13719, },
				{0.857786, 0.867308, 0.0 , 2.03047, 1.79617, },
				{1.25017, 1.11844, 0.0 , 2.84186, 2.39759, },
				{1.61456, 1.36333, 0.0 , 2.29949, 2.68553, },
				{0.694722, 0.655504, 0.0 , 1.62789, 1.66232, },
				{0.889708, 0.918838, 0.0 , 1.878, 1.53031, },
				{1.35159, 1.1916, 0.0 , 2.97554, 2.81083, },
				{1.57846, 1.32976, 0.0 , 2.62211, 2.4662, },
				{1.57627, 1.20833, 0.0 , 3.28338, 3.01404, },
				{2.14823, 2.12309, 0.0 , 6.5401, 3.55884, },
				{2.19813, 2.61619, 0.0 , 5.27536, 5.278, },
				{1.4977, 1.61041, 0.0 , 4.00211, 4.97346, }		};


				static double ff_u_eratio_0[15][5]    = {{-0.000644982, -0.00350606, 0.0 , -0.00245005, -0.00670296},
				{-0.0122138, -0.00847065, 0.0 , -0.00238681, -0.00461286},
				{0.000162542, -0.00277948, 0.0 , -0.00190389, -0.00065589},
				{-0.000835359, -0.00262159, 0.0 , -0.00260037, -0.00241715},
				{-0.00272757, -0.00316167, 0.0 , -0.00321382, -0.00104648},
				{0.00186175, -0.000618517, 0.0 , -0.0019238, -0.00098139},
				{-4.92334e-05, -0.00539839, 0.0 , 0.0080086, -0.000290573},
				{-0.00305659, -0.00247222, 0.0 , -0.00425649, -0.000798047},
				{-0.000387907, -0.00461513, 0.0 , -0.00407076, -0.00119966},
				{-0.00300056, -0.0026955, 0.0 , 0.00338644, -0.00131685},
				{0.000100076, -0.00288099, 0.0 , -0.00587022, 0.000413954},
				{0.000944674, -0.00299257, 0.0 , -0.0028283, -0.000978589},
				{-0.00329077, 0.000515223, 0.0 , -0.00586456, -0.00372207},
				{-0.00396132, -0.00559902, 0.0 , 0.0022347, -0.000730693},
				{-0.00125211, -0.00362003, 0.0 , -0.00477821, 0.00119066}		};


				static double fferr_u_eratio_0[15][5]    = {{0.0013756, 0.000662379, 0.0 , 0.000765882, 0.0008078, },
				{0.00476868, 0.00392349, 0.0 , 0.00345721, 0.00298526, },
				{0.00113319, 0.000705665, 0.0 , 0.000841582, 0.000730682, },
				{0.00156516, 0.00104845, 0.0 , 0.00111462, 0.000726393, },
				{0.00181003, 0.00130195, 0.0 , 0.00170271, 0.00120706, },
				{0.00345635, 0.00181097, 0.0 , 0.00241534, 0.00225695, },
				{0.0033171, 0.00199396, 0.0 , 0.00400857, 0.00170122, },
				{0.00119615, 0.00098659, 0.0 , 0.000992589, 0.00102476, },
				{0.00166432, 0.00111911, 0.0 , 0.000887061, 0.00162946, },
				{0.00176041, 0.00195237, 0.0 , 0.0040569, 0.00126972, },
				{0.00320426, 0.00150357, 0.0 , 0.00121378, 0.00150626, },
				{0.00260371, 0.001513, 0.0 , 0.00212284, 0.00125011, },
				{0.00316905, 0.00343727, 0.0 , 0.00176385, 0.00128691, },
				{0.00236765, 0.00236584, 0.0 , 0.00241588, 0.00254231, },
				{0.00289893, 0.00201559, 0.0 , 0.00242859, 0.00213439, }		};


				if(isConv){
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad1_0[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad_0[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_e277_0[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_reta_0[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rphi_0[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_weta2_0[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_f1_0[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_fside_0[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_wtot_0[0][0] ,  0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_w1_0[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_deltae_0[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_eratio_0[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad1_0[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad_0[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_e277_0[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_reta_0[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rphi_0[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_weta2_0[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_f1_0[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_fside_0[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_wtot_0[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_w1_0[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_deltae_0[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_eratio_0[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad1_0[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad_0[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_e277_0[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_reta_0[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rphi_0[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_weta2_0[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_f1_0[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_fside_0[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_wtot_0[0][0],   0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_w1_0[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_deltae_0[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_eratio_0[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad1_0[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad_0[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_e277_0[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_reta_0[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rphi_0[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_weta2_0[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_f1_0[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_fside_0[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_wtot_0[0][0],   0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_w1_0[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_deltae_0[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_eratio_0[0][0], 0. );
				}

				break;

		case 1:  // RobustLoose+Iso preselection

				//converted photons

				static double ff_c_rhad1_1[15][5]    = {{-0.00234993, -0.00118102, 0.0 , -0.00216891, -0.000404502},
				{-0.0011417, 0.00118392, 0.0 , -0.00342647, -0.00165709},
				{-0.000928833, -0.00101438, 0.0 , -0.000648535, -0.00179858},
				{-0.000633918, -0.000832669, 0.0 , 0.000144757, -0.00119894},
				{-0.00143738, -0.000584924, 0.0 , 0.000467088, -0.000490138},
				{-0.000452512, 3.31925e-05, 0.0 , 0.000256902, -0.000362037},
				{-0.000821324, -0.000939407, 0.0 , -0.00197309, 3.90213e-06},
				{-0.000877852, -0.000287947, 0.0 , -0.000926281, -0.000604274},
				{-0.000494828, -0.000277327, 0.0 , -0.000509674, -0.000213587},
				{-0.00033792, -0.000361671, 0.0 , 0.000134017, -7.11135e-05},
				{-0.000208757, 0.00015781, 0.0 , 0.000395934, -0.000109529},
				{0.000178214, -5.93531e-05, 0.0 , 0.00023968, -8.4821e-05},
				{0.00065754, -5.769e-05, 0.0 , -0.000128174, 7.2122e-05},
				{-0.000318661, 0.000226644, 0.0 , 0.000620059, 6.60659e-05},
				{-0.000426226, -9.399e-05, 0.0 , -0.000369756, -9.55523e-05}		};


				static double fferr_c_rhad1_1[15][5]    = {{0.000350125, 0.000591778, 0.0 , 0.00056887, 0.000377684, },
				{0.00110829, 0.00057847, 0.0 , 0.00206978, 0.0010232, },
				{0.000150786, 0.000239638, 0.0 , 0.000267149, 0.000172149, },
				{0.000190395, 0.000374535, 0.0 , 0.000352763, 0.000252096, },
				{0.00036759, 0.000465524, 0.0 , 0.000494592, 0.000320817, },
				{0.000405435, 0.000323504, 0.0 , 0.000710148, 0.000411392, },
				{0.000405118, 0.00038561, 0.0 , 0.000821176, 0.000535625, },
				{0.000237717, 0.000178325, 0.0 , 0.000351303, 0.000204411, },
				{0.000240691, 0.000268587, 0.0 , 0.000433741, 0.000287855, },
				{0.000290367, 0.000486283, 0.0 , 0.000642578, 0.000275242, },
				{0.000268233, 0.00017637, 0.0 , 0.000388809, 0.000210421, },
				{0.000278101, 0.000216996, 0.0 , 0.000362193, 0.000226254, },
				{0.000195365, 0.000254649, 0.0 , 0.000876035, 0.000297141, },
				{0.00026102, 0.000153405, 0.0 , 0.000510087, 0.000264767, },
				{0.00014143, 7.20699e-05, 0.0 , 0.000285987, 0.000206833, }		};


				static double ff_c_rhad_1[15][5]    = {{0, -0.00247857, 0.0 , 0, 0},
				{0, -0.000301357, 0.0 , 0, 0},
				{0, -0.000615386, 0.0 , 0, 0},
				{0, -0.000503023, 0.0 , 0, 0},
				{0, -0.000543643, 0.0 , 0, 0},
				{0, -0.000245572, 0.0 , 0, 0},
				{0, -0.000306696, 0.0 , 0, 0},
				{0, -0.000576898, 0.0 , 0, 0},
				{0, -0.000368448, 0.0 , 0, 0},
				{0, 0.000293194, 0.0 , 0, 0},
				{0, -0.000339616, 0.0 , 0, 0},
				{0, -0.00010762, 0.0 , 0, 0},
				{0, -8.49382e-05, 0.0 , 0, 0},
				{0, -0.000101179, 0.0 , 0, 0},
				{0, -3.32672e-05, 0.0 , 0, 0}		};


				static double fferr_c_rhad_1[15][5]    = {{0, 0.000293822, 0.0 , 0, 0, },
				{0, 0.00100532, 0.0 , 0, 0, },
				{0, 0.000112182, 0.0 , 0, 0, },
				{0, 0.000172127, 0.0 , 0, 0, },
				{0, 0.000252393, 0.0 , 0, 0, },
				{0, 0.000215395, 0.0 , 0, 0, },
				{0, 0.000313729, 0.0 , 0, 0, },
				{0, 0.000158016, 0.0 , 0, 0, },
				{0, 0.000154053, 0.0 , 0, 0, },
				{0, 0.000153297, 0.0 , 0, 0, },
				{0, 0.000207767, 0.0 , 0, 0, },
				{0, 0.000109955, 0.0 , 0, 0, },
				{0, 0.000120753, 0.0 , 0, 0, },
				{0, 0.000111426, 0.0 , 0, 0, },
				{0, 5.64834e-05, 0.0 , 0, 0, }		};


				static double ff_c_e277_1[15][5]    = {{524.218, 903.677, 0.0 , 3018.76, 2984.32},
				{63.8701, 751.508, 0.0 , -1087.61, -288.008},
				{-63.6553, 250.584, 0.0 , -134.551, -3.30469},
				{-87.7266, 357.891, 0.0 , -797.133, -20.6094},
				{-164.312, -276.316, 0.0 , -163.285, -818.391},
				{-290.057, 680.27, 0.0 , 121.539, -995.992},
				{673.125, 1191.03, 0.0 , -532.734, -269.586},
				{-270.512, 913.289, 0.0 , -4068.84, -618.25},
				{97.1953, 1727.64, 0.0 , -1203.56, -1545.92},
				{-2293.88, 1871.47, 0.0 , -5980.91, -9548.03},
				{-1317.02, 1949.93, 0.0 , -3709.78, -4583.94},
				{261.141, -91.5625, 0.0 , -6722.58, -15242.2},
				{-5722.43, 1593.48, 0.0 , 20675.2, 290754},
				{5159.26, -3288.31, 0.0 , 23753.9, 0},
				{-34965.1, -10252.1, 0.0 , 11963.5, 0}		};


				static double fferr_c_e277_1[15][5]    = {{31.311, 39.4664, 0.0 , 87.9091, 117.877, },
				{336.353, 380.847, 0.0 , 1197.28, 1335.04, },
				{68.1638, 84.4384, 0.0 , 256.097, 255.848, },
				{119.72, 168.484, 0.0 , 498.355, 521.787, },
				{205.627, 317.651, 0.0 , 957.535, 923.754, },
				{316.156, 461.556, 0.0 , 1628.57, 1472.73, },
				{443.956, 750.798, 0.0 , 2172.44, 2260.68, },
				{285.688, 397.247, 0.0 , 1506.18, 1300.13, },
				{434.394, 670.866, 0.0 , 2180.72, 2130.52, },
				{1083.08, 1314.81, 0.0 , 4378.05, 3660.11, },
				{1197.43, 1683.71, 0.0 , 6422.86, 3345.79, },
				{1257.45, 2727.2, 0.0 , 9655.45, 3833.19, },
				{2738.45, 4398.97, 0.0 , 11945.3, 2382.44, },
				{3344.6, 6677.29, 0.0 , 13576.2, 0, },
				{5264.8, 5509.35, 0.0 , 14232.4, 0, }		};


				static double ff_c_reta_1[15][5]    = {{-0.00538641, -0.0057103, 0.0 , -0.00909007, -0.00699937},
				{0.00155967, -0.00625616, 0.0 , -0.0192043, -0.0100381},
				{-0.00321501, -0.00695843, 0.0 , -0.0102361, -0.0106221},
				{-0.00448048, -0.00749582, 0.0 , -0.0107668, -0.0106843},
				{-0.00297666, -0.00792718, 0.0 , -0.0095644, -0.0106241},
				{-0.00494844, -0.00621957, 0.0 , -0.00879657, -0.0107446},
				{-0.00435162, -0.0058493, 0.0 , -0.00949115, -0.00864422},
				{-0.0044114, -0.00726289, 0.0 , -0.0115922, -0.0103889},
				{-0.0053317, -0.00766134, 0.0 , -0.0110525, -0.00935018},
				{-0.00447267, -0.00583196, 0.0 , -0.0136886, -0.00843555},
				{-0.00585634, -0.00756192, 0.0 , -0.00799286, -0.00868165},
				{-0.0060457, -0.00801176, 0.0 , -0.013319, -0.00757933},
				{-0.00656372, -0.00807738, 0.0 , -0.00956976, -0.00977099},
				{-0.00459039, -0.00699359, 0.0 , -0.0128653, -0.0116824},
				{-0.00537896, -0.00846577, 0.0 , -0.0113829, -0.0112035}		};


				static double fferr_c_reta_1[15][5]    = {{0.000786794, 0.000530167, 0.0 , 0.000672262, 0.000367026, },
				{0.00255936, 0.00181282, 0.0 , 0.00188667, 0.00147864, },
				{0.000368772, 0.000255586, 0.0 , 0.000397168, 0.000239937, },
				{0.00052784, 0.00038498, 0.0 , 0.000632848, 0.000398959, },
				{0.000739352, 0.000597456, 0.0 , 0.0010265, 0.000555072, },
				{0.000955576, 0.000712244, 0.0 , 0.00150016, 0.000848424, },
				{0.00115306, 0.00101146, 0.0 , 0.00185038, 0.00107238, },
				{0.000532154, 0.000418554, 0.0 , 0.000744786, 0.000491725, },
				{0.000622215, 0.00048243, 0.0 , 0.000996474, 0.000720892, },
				{0.00116413, 0.000887087, 0.0 , 0.0010716, 0.00122373, },
				{0.000917857, 0.000748782, 0.0 , 0.0018354, 0.00102491, },
				{0.000887844, 0.000851961, 0.0 , 0.00173651, 0.00129029, },
				{0.00108393, 0.00108641, 0.0 , 0.00142302, 0.00208389, },
				{0.00138386, 0.00092224, 0.0 , 0.0017953, 0.00181395, },
				{0.000742288, 0.000812548, 0.0 , 0.0015308, 0.00157465, }		};


				static double ff_c_rphi_1[15][5]    = {{-0.00864619, -0.0103325, 0.0 , -0.00898463, -0.00644547},
				{-0.00436288, -0.0123215, 0.0 , -0.0186692, -0.0314378},
				{-0.00871873, -0.0109113, 0.0 , -0.0172299, -0.0138555},
				{-0.0136308, -0.0128129, 0.0 , -0.0138916, -0.0077939},
				{-0.0115987, -0.0145387, 0.0 , -0.0114437, -0.00641227},
				{-0.000430584, -0.0086332, 0.0 , -0.00858325, -0.00358772},
				{0.00794953, -0.00904393, 0.0 , 0.0105627, -0.0105039},
				{-0.00249904, -0.00325978, 0.0 , -0.00340384, -0.000954092},
				{-0.000163376, -0.00311047, 0.0 , -0.00163311, -0.00629801},
				{-0.0118222, -0.00904173, 0.0 , -0.0138291, -0.00218415},
				{-0.00101668, -0.00377691, 0.0 , 0.000672042, -0.00641513},
				{-0.0043909, -0.00532758, 0.0 , -0.00423598, -0.000444472},
				{-0.000192881, -0.00260317, 0.0 , -0.00919408, -0.0064286},
				{-0.00710005, -0.00676483, 0.0 , -0.00967497, -0.00632739},
				{-0.00391215, -0.00669712, 0.0 , -0.00846589, -0.00719512}		};


				static double fferr_c_rphi_1[15][5]    = {{0.00171673, 0.00120072, 0.0 , 0.0015923, 0.00121326, },
				{0.0140167, 0.00814409, 0.0 , 0.0109124, 0.00738042, },
				{0.00199183, 0.00126566, 0.0 , 0.00166818, 0.00114189, },
				{0.00298453, 0.00197387, 0.0 , 0.00263825, 0.00178754, },
				{0.00456535, 0.00281656, 0.0 , 0.00351953, 0.00244998, },
				{0.00657449, 0.00360562, 0.0 , 0.00502045, 0.00342616, },
				{0.00757842, 0.00447452, 0.0 , 0.00744236, 0.0030187, },
				{0.00329822, 0.00204709, 0.0 , 0.00290937, 0.00170142, },
				{0.00362356, 0.00228543, 0.0 , 0.00379584, 0.00157912, },
				{0.00372449, 0.0025419, 0.0 , 0.00189742, 0.00301821, },
				{0.00452021, 0.00251121, 0.0 , 0.00458124, 0.00190536, },
				{0.0049269, 0.00221811, 0.0 , 0.00303545, 0.00235341, },
				{0.0136398, 0.00318031, 0.0 , 0.00174301, 0.00255397, },
				{0.00205769, 0.00321476, 0.0 , 0.0029651, 0.00160964, },
				{0.0012717, 0.00128952, 0.0 , 0.0026124, 0.00150409, }		};


				static double ff_c_weta2_1[15][5]    = {{0.000279294, 0.000349549, 0.0 , 0.000455244, 0.000318204},
				{0.000194259, 0.000135149, 0.0 , 0.000778392, 0.000388271},
				{0.000161726, 0.000262588, 0.0 , 0.000453585, 0.000429506},
				{0.000224799, 0.000245473, 0.0 , 0.000453687, 0.000452939},
				{0.000130315, 0.000260654, 0.0 , 0.000328545, 0.000363662},
				{0.0001565, 0.000205476, 0.0 , 0.00047713, 0.000468859},
				{0.000256589, 0.000102306, 0.0 , 0.000494096, 0.00036715},
				{0.000219852, 0.00020927, 0.0 , 0.000501798, 0.000363943},
				{0.000136877, 0.000175553, 0.0 , 0.000621918, 0.000410006},
				{0.000269625, 0.000218972, 0.0 , 0.000741848, 0.00024603},
				{0.00028499, 0.000326023, 0.0 , 0.000458451, 0.000283721},
				{0.000204487, 0.000341714, 0.0 , 0.000711962, 0.000197933},
				{0.000219305, 0.000358567, 0.0 , 0.000621811, 0.000566708},
				{0.000167264, 0.000324361, 0.0 , 0.00076676, 0.00058699},
				{0.000287059, 0.000303933, 0.0 , 0.000743614, 0.000474619}		};


				static double fferr_c_weta2_1[15][5]    = {{3.83943e-05, 2.43999e-05, 0.0 , 3.52867e-05, 1.78806e-05, },
				{0.000164224, 9.79549e-05, 0.0 , 0.000148439, 7.71536e-05, },
				{2.06066e-05, 1.48569e-05, 0.0 , 2.88904e-05, 1.65043e-05, },
				{2.99317e-05, 2.36433e-05, 0.0 , 4.7573e-05, 2.73831e-05, },
				{4.36981e-05, 3.40115e-05, 0.0 , 7.75622e-05, 3.87446e-05, },
				{5.8687e-05, 4.2244e-05, 0.0 , 0.000108236, 5.39957e-05, },
				{7.92613e-05, 6.4434e-05, 0.0 , 0.000115183, 7.44875e-05, },
				{3.41776e-05, 3.00997e-05, 0.0 , 6.51024e-05, 3.63908e-05, },
				{4.69209e-05, 4.10728e-05, 0.0 , 6.99424e-05, 5.26652e-05, },
				{8.68299e-05, 5.52416e-05, 0.0 , 0.000100277, 8.68248e-05, },
				{6.83031e-05, 5.43334e-05, 0.0 , 0.000107297, 8.38761e-05, },
				{6.30114e-05, 6.3763e-05, 0.0 , 0.000103734, 9.29837e-05, },
				{0.000107904, 6.53543e-05, 0.0 , 0.000109907, 0.000138985, },
				{9.89969e-05, 6.90753e-05, 0.0 , 0.000131106, 0.000143503, },
				{5.56969e-05, 4.92667e-05, 0.0 , 0.000114658, 0.000119345, }		};


				static double ff_c_f1_1[15][5]    = {{-0.00154725, 0.00262052, 0.0 , 0.00687477, -0.0122707},
				{0.00200409, -0.0131288, 0.0 , 0.0302318, 0.0168725},
				{0.00188312, -0.000286281, 0.0 , 0.0117246, 0.0156158},
				{-0.000458479, -0.00249359, 0.0 , 0.0124312, 0.0137484},
				{0.000506341, 0.00521508, 0.0 , 0.00998059, 0.0101287},
				{0.00364038, -0.00361314, 0.0 , 0.00679874, 0.0134168},
				{-0.013243, -0.00498752, 0.0 , 0.00516886, 0.00473389},
				{0.00124934, -0.00306499, 0.0 , 0.0171335, 0.00744671},
				{-0.0059137, -0.0062754, 0.0 , 0.00580171, 0.0074278},
				{0.0158924, -0.0128387, 0.0 , 0.0125158, 0.00955473},
				{0.0106595, 0.000333443, 0.0 , 0.0120288, 0.00365728},
				{-0.00700915, 0.00260474, 0.0 , 0.0316016, 0.012626},
				{0.0294737, -0.00189088, 0.0 , -0.00353897, 0.00536895},
				{-0.0147237, -0.00407623, 0.0 , 0.0477722, 0.0116804},
				{0.00838536, 0.018571, 0.0 , 0.0121823, 0.0164491}		};


				static double fferr_c_f1_1[15][5]    = {{0.00299423, 0.00169308, 0.0 , 0.00215147, 0.00220736, },
				{0.0167887, 0.008411, 0.0 , 0.0105516, 0.00960244, },
				{0.00225267, 0.00145542, 0.0 , 0.00171619, 0.00164651, },
				{0.00321054, 0.00240467, 0.0 , 0.00277047, 0.00281533, },
				{0.00449596, 0.00374591, 0.0 , 0.004211, 0.00410228, },
				{0.00592502, 0.00471595, 0.0 , 0.00636167, 0.00537963, },
				{0.00838916, 0.00616365, 0.0 , 0.00717133, 0.00709996, },
				{0.00400086, 0.00302249, 0.0 , 0.00474751, 0.00328037, },
				{0.00486308, 0.00398108, 0.0 , 0.00497844, 0.00421709, },
				{0.00940114, 0.00624167, 0.0 , 0.00894718, 0.00749704, },
				{0.00831088, 0.00632372, 0.0 , 0.00956191, 0.00669208, },
				{0.00807326, 0.00759942, 0.0 , 0.0137584, 0.00811145, },
				{0.0172403, 0.00996378, 0.0 , 0.0185514, 0.012489, },
				{0.0120485, 0.0115082, 0.0 , 0.0235355, 0.0120649, },
				{0.00820527, 0.00735199, 0.0 , 0.0137446, 0.013339, }		};


				static double ff_c_fside_1[15][5]    = {{0.023903, 0.0320908, 0.0 , 0.0400922, 0.0357751},
				{-0.00475904, 0.0107588, 0.0 , 0.0240199, 0.017988},
				{-0.0113783, 0.00976399, 0.0 , 0.0207101, -0.000937343},
				{-0.0102666, -0.00227401, 0.0 , 0.0268284, -0.000315756},
				{-0.0395125, 0.00173271, 0.0 , 0.0151284, 0.005329},
				{0.00169086, -0.0294123, 0.0 , 0.0270198, 0.00966215},
				{-0.0136365, -0.0180376, 0.0 , -0.0157493, 0.0198803},
				{-0.00388497, -0.00532487, 0.0 , 0.0165913, -0.00135969},
				{-0.0177703, -0.00396609, 0.0 , 0.0345558, 0.00773086},
				{-0.00152725, -0.00529468, 0.0 , 0.0567939, -0.00581336},
				{0.00604245, 0.0113741, 0.0 , 0.0177746, 0.0174008},
				{0.0255436, 0.0168223, 0.0 , 0.0323469, -0.0210839},
				{-0.00265835, 0.026273, 0.0 , 0.0677218, 0.0326317},
				{-0.00501591, 0.0385808, 0.0 , 0.0243209, 0.0398009},
				{0.00901495, 0.0235313, 0.0 , 0.0300774, 0.0230585}		};


				static double fferr_c_fside_1[15][5]    = {{0.00326024, 0.00301304, 0.0 , 0.00375208, 0.00247129, },
				{0.0230837, 0.0135406, 0.0 , 0.0252536, 0.019411, },
				{0.00425815, 0.00281204, 0.0 , 0.00450314, 0.00343645, },
				{0.00675945, 0.00497634, 0.0 , 0.00750591, 0.00555554, },
				{0.0111808, 0.00749997, 0.0 , 0.0129655, 0.00802376, },
				{0.0138162, 0.0107567, 0.0 , 0.0162688, 0.00954046, },
				{0.0185989, 0.013518, 0.0 , 0.0217861, 0.0120334, },
				{0.0077528, 0.00581415, 0.0 , 0.00998819, 0.00671205, },
				{0.0115232, 0.00742757, 0.0 , 0.0119978, 0.00848186, },
				{0.0161432, 0.0116976, 0.0 , 0.0172117, 0.0171272, },
				{0.0137976, 0.010355, 0.0 , 0.0178145, 0.0104333, },
				{0.011344, 0.0109641, 0.0 , 0.0229531, 0.0192803, },
				{0.0330894, 0.00914895, 0.0 , 0.016125, 0.0152478, },
				{0.0198965, 0.0100232, 0.0 , 0.0382531, 0.00953428, },
				{0.0150322, 0.00924232, 0.0 , 0.0101815, 0.010842, }		};


				static double ff_c_wtot_1[15][5]    = {{0.0684371, 0.0600868, 0.0 , 0.0961869, 0.140945},
				{0.0765496, 0.0711088, 0.0 , 0.124837, 0.0925324},
				{0.0303316, 0.0437057, 0.0 , 0.075382, 0.0295796},
				{0.0525331, 0.0253913, 0.0 , 0.0863807, 0.0452368},
				{0.00968575, 0.039844, 0.0 , 0.0760274, 0.0787808},
				{0.0622699, 0.035223, 0.0 , 0.0981627, 0.0570318},
				{0.0499595, 0.0535281, 0.0 , 0.0978134, 0.100386},
				{0.0501076, 0.0548747, 0.0 , 0.0983942, 0.056667},
				{0.0741113, 0.0443745, 0.0 , 0.0970557, 0.0564517},
				{0.0940109, 0.0551169, 0.0 , 0.149748, 0.0747569},
				{0.0524057, 0.125446, 0.0 , 0.0897276, 0.0720959},
				{0.0710969, 0.0958152, 0.0 , 0.12146, 0.0382098},
				{0.14338, 0.0750365, 0.0 , 0.219638, 0.139553},
				{0.120761, 0.0887892, 0.0 , 0.13197, 0.126108},
				{0.0841383, 0.0748687, 0.0 , 0.137585, 0.0975785}		};


				static double fferr_c_wtot_1[15][5]    = {{0.0134909, 0.0105556, 0.0 , 0.0115789, 0.00666065, },
				{0.0499547, 0.0321268, 0.0 , 0.0634431, 0.0345221, },
				{0.00830982, 0.00616744, 0.0 , 0.0085118, 0.00671433, },
				{0.0119912, 0.00857805, 0.0 , 0.013379, 0.0108902, },
				{0.0167723, 0.0116516, 0.0 , 0.0191488, 0.0158349, },
				{0.0227349, 0.0178938, 0.0 , 0.0247716, 0.0214462, },
				{0.0271162, 0.0247336, 0.0 , 0.0333857, 0.028503, },
				{0.0126511, 0.0106023, 0.0 , 0.0158783, 0.0135776, },
				{0.0148661, 0.013472, 0.0 , 0.0180816, 0.0184707, },
				{0.0277989, 0.0218956, 0.0 , 0.0376264, 0.0321892, },
				{0.0252159, 0.0227716, 0.0 , 0.0347836, 0.0291055, },
				{0.0251546, 0.0259559, 0.0 , 0.0375554, 0.0461702, },
				{0.0329464, 0.0350457, 0.0 , 0.0706836, 0.0485058, },
				{0.0227715, 0.0380949, 0.0 , 0.0805995, 0.052195, },
				{0.0189709, 0.0263966, 0.0 , 0.0376582, 0.0579815, }		};


				static double ff_c_w1_1[15][5]    = {{0.0123374, 0.0186456, 0.0 , 0.0124431, 0.038825},
				{-0.00611711, 0.00969803, 0.0 , 0.0083285, 0.0447536},
				{-0.000318527, 0.00973207, 0.0 , 0.00645, 0.0489485},
				{-0.000422776, 0.00796646, 0.0 , 0.00879061, 0.043349},
				{-0.00417823, 0.01452, 0.0 , 0.00609642, 0.0329461},
				{0.00382054, -0.00178593, 0.0 , 0.00426286, 0.0311916},
				{-0.00722605, 0.00230128, 0.0 , -0.000566244, 0.0330318},
				{-0.00431627, 0.00700068, 0.0 , 0.00791931, 0.0317311},
				{-0.00883943, 0.00553364, 0.0 , 0.00528222, 0.0313342},
				{0.00645381, 0.00384164, 0.0 , 0.0112221, 0.0285205},
				{-0.00326377, 0.0107284, 0.0 , 0.00289637, 0.026602},
				{0.00568038, 0.0102076, 0.0 , 0.00941753, 0.0311286},
				{0.00115883, 0.00855356, 0.0 , 0.0279847, 0.0398991},
				{0.00333542, 0.0136205, 0.0 , 0.0265607, 0.0407006},
				{0.00394022, 0.013588, 0.0 , 0.0205598, 0.0370596}		};


				static double fferr_c_w1_1[15][5]    = {{0.00170457, 0.00131456, 0.0 , 0.00145472, 0.00126828, },
				{0.00848619, 0.00505472, 0.0 , 0.00817696, 0.00636496, },
				{0.00120438, 0.000912772, 0.0 , 0.00124906, 0.00143356, },
				{0.00180852, 0.00138744, 0.0 , 0.00207501, 0.00230758, },
				{0.00270243, 0.00238958, 0.0 , 0.00293035, 0.00322442, },
				{0.00386354, 0.00299435, 0.0 , 0.00366074, 0.0043151, },
				{0.00512569, 0.00430763, 0.0 , 0.00562178, 0.0054859, },
				{0.0024439, 0.00184195, 0.0 , 0.00289062, 0.00276004, },
				{0.00352977, 0.00230195, 0.0 , 0.00350344, 0.00364551, },
				{0.00573169, 0.00368938, 0.0 , 0.00617152, 0.00613787, },
				{0.00548087, 0.00405683, 0.0 , 0.00632654, 0.0055364, },
				{0.00502876, 0.00417882, 0.0 , 0.00783321, 0.00798858, },
				{0.0129386, 0.00613257, 0.0 , 0.0128731, 0.0102101, },
				{0.00556072, 0.00658338, 0.0 , 0.0109575, 0.00836952, },
				{0.00603174, 0.00488632, 0.0 , 0.00618054, 0.00822485, }		};


				static double ff_c_deltae_1[15][5]    = {{0.687439, -1.77635, 0.0 , -4.90638, -3.77153},
				{-10.3347, 2.89987, 0.0 , -2.50946, 0.309372},
				{-7.52982, -6.78571, 0.0 , -4.13795, -4.56084},
				{-4.58921, -5.59222, 0.0 , 1.4254, -3.54855},
				{-5.09488, -3.18439, 0.0 , -5.20824, -4.87563},
				{-9.60308, -5.30745, 0.0 , 0.407948, -1.10051},
				{-8.33346, -3.11355, 0.0 , 4.14128, 2.13399},
				{-4.31726, -2.41233, 0.0 , -3.12639, -3.44169},
				{1.11285, -2.41021, 0.0 , 1.94948, -0.783493},
				{-14.3433, -3.82943, 0.0 , 0.165615, 0.42721},
				{0.123699, -5.05659, 0.0 , -2.64026, 0.428465},
				{-0.523001, -4.14987, 0.0 , -10.0489, 0.531158},
				{-2.12686, -7.98813, 0.0 , -18.1499, 0.327192},
				{2.53867, -1.03791, 0.0 , 12.3404, -6.27556},
				{0.365921, -2.3632, 0.0 , 7.94258, -9.61931}		};


				static double fferr_c_deltae_1[15][5]    = {{0.781132, 0.57847, 0.0 , 1.01822, 0.669175, },
				{9.8691, 3.45854, 0.0 , 9.18463, 3.44273, },
				{1.48097, 0.924339, 0.0 , 1.49454, 0.867893, },
				{2.11774, 1.44588, 0.0 , 2.1024, 1.32078, },
				{3.04337, 2.05463, 0.0 , 3.86305, 2.00975, },
				{3.67778, 2.68792, 0.0 , 3.49465, 2.3026, },
				{5.29367, 3.30553, 0.0 , 3.57415, 2.90739, },
				{2.11417, 1.19468, 0.0 , 2.9897, 1.68788, },
				{1.50846, 1.69726, 0.0 , 2.50206, 2.03011, },
				{5.41937, 2.76799, 0.0 , 6.01587, 3.79895, },
				{3.75491, 2.69266, 0.0 , 4.91796, 3.35905, },
				{3.11884, 3.57762, 0.0 , 5.46012, 4.4495, },
				{5.65238, 5.55816, 0.0 , 17.1379, 4.39662, },
				{5.34481, 3.87422, 0.0 , 6.28921, 6.99503, },
				{3.11763, 2.76773, 0.0 , 5.25998, 5.96384, }		};


				static double ff_c_eratio_1[15][5]    = {{-0.00307107, -0.00254542, 0.0 , 0.000395954, -0.00518984},
				{-0.00114769, -0.00747424, 0.0 , -0.0460574, -0.000396907},
				{0.023302, 0.0213373, 0.0 , 0.0137317, 0.0224041},
				{0.0140625, 0.0149317, 0.0 , 0.0190254, 0.00326395},
				{0.017943, 0.0149521, 0.0 , 0.0118881, 0.013139},
				{0.0235203, 0.0139801, 0.0 , -0.00980788, 0.00296289},
				{0.00356889, 0.00206912, 0.0 , -0.0163939, 0.00520015},
				{0.0164498, 0.00355327, 0.0 , 0.0136288, 0.0125608},
				{0.00553834, 0.00285488, 0.0 , 0.0180887, 0.00512838},
				{0.0127434, 0.00338924, 0.0 , -0.0107623, 0.00588709},
				{0.00775915, 0.0123346, 0.0 , 0.018393, 0.00769567},
				{0.00224042, 0.0134063, 0.0 , 0.00643224, 0.0196543},
				{-0.0154513, 0.0133664, 0.0 , -0.00851029, 0.00597703},
				{-0.0135042, -0.0107429, 0.0 , -0.0132991, 0.00244945},
				{0.00233674, -0.000748515, 0.0 , -0.01222, -0.00282878}		};


				static double fferr_c_eratio_1[15][5]    = {{0.00270881, 0.00217633, 0.0 , 0.00281294, 0.00174179, },
				{0.0205512, 0.0130196, 0.0 , 0.00692668, 0.00635444, },
				{0.00421009, 0.00266083, 0.0 , 0.0040632, 0.00256103, },
				{0.00595535, 0.00398862, 0.0 , 0.00660982, 0.00300044, },
				{0.00867367, 0.00580872, 0.0 , 0.00905965, 0.00500332, },
				{0.0115067, 0.00687313, 0.0 , 0.0100242, 0.00529972, },
				{0.0109506, 0.00704898, 0.0 , 0.0089722, 0.00726228, },
				{0.00644761, 0.00346087, 0.0 , 0.00767143, 0.00413376, },
				{0.00713606, 0.00467336, 0.0 , 0.010566, 0.00430487, },
				{0.0131867, 0.00688369, 0.0 , 0.00863324, 0.00744071, },
				{0.0126631, 0.00800072, 0.0 , 0.0139806, 0.0065317, },
				{0.0104046, 0.00819763, 0.0 , 0.00970337, 0.0118265, },
				{0.00578537, 0.0115982, 0.0 , 0.00498704, 0.0156624, },
				{0.00595996, 0.00451305, 0.0 , 0.00726222, 0.00533405, },
				{0.00610041, 0.00188142, 0.0 , 0.0169419, 0.00716637, }		};

				//unconverted photons

				static double ff_u_rhad1_1[15][5]    = {{-0.00284358, -0.0028266, 0.0 , -0.00116584, -0.000443354},
				{-0.000729407, -0.000436728, 0.0 , 4.11158e-05, 0.00166723},
				{-0.000936598, -0.000718208, 0.0 , -0.00109112, -0.0010507},
				{-0.00099441, -0.000400825, 0.0 , -0.0013813, -0.00125305},
				{-0.000617604, -0.000822571, 0.0 , -0.000695394, -0.000889971},
				{-0.000544818, -0.000394509, 0.0 , 0.000582306, -0.000374019},
				{-0.00115396, -0.000138731, 0.0 , -4.6535e-05, -0.00144458},
				{-0.000604684, -0.000196503, 0.0 , -0.000304072, -0.000230283},
				{-0.000659275, -0.00113634, 0.0 , 0.000214284, -0.000315155},
				{-0.000737796, -0.000644211, 0.0 , -0.00188375, -0.000267701},
				{-0.000355288, -0.000238148, 0.0 , -9.68054e-05, 0.000378304},
				{0.000119525, 8.09417e-05, 0.0 , -2.54053e-05, -0.000311089},
				{-0.000511569, 6.50776e-05, 0.0 , 0.000460606, -6.66137e-05},
				{-0.000556225, -3.77454e-05, 0.0 , -0.00191041, 0.00016258},
				{-0.000369466, 9.23804e-05, 0.0 , -0.000546576, -0.000234479}		};


				static double fferr_u_rhad1_1[15][5]    = {{0.000143279, 0.000241488, 0.0 , 0.000413243, 0.000257877, },
				{0.000554844, 0.000647771, 0.0 , 0.00148321, 0.000804016, },
				{8.44546e-05, 0.000127916, 0.0 , 0.000289329, 0.000156516, },
				{0.000144401, 0.000197218, 0.0 , 0.000436988, 0.000254682, },
				{0.000172519, 0.000296079, 0.0 , 0.00055804, 0.000359942, },
				{0.000199733, 0.000336594, 0.0 , 0.00074228, 0.000320955, },
				{0.000386122, 0.000342425, 0.0 , 0.000710623, 0.000709578, },
				{0.000132803, 0.000165101, 0.0 , 0.000412095, 0.000223481, },
				{0.000189493, 0.000435639, 0.0 , 0.000336394, 0.000264126, },
				{0.000274493, 0.000439508, 0.0 , 0.000752097, 0.000306286, },
				{0.000186929, 0.000165044, 0.0 , 0.00051557, 0.000242673, },
				{0.000162817, 0.000120552, 0.0 , 0.000684747, 0.000358011, },
				{0.000367039, 0.000156685, 0.0 , 0.000653103, 0.000410771, },
				{0.000346507, 0.000422148, 0.0 , 0.00124931, 0.000371328, },
				{0.000178989, 0.000128947, 0.0 , 0.000507984, 0.000276608, }		};


				static double ff_u_rhad_1[15][5]    = {{0, -0.00334169, 0.0 , 0, 0},
				{0, -4.09576e-05, 0.0 , 0, 0},
				{0, -0.000633676, 0.0 , 0, 0},
				{0, -0.000734589, 0.0 , 0, 0},
				{0, -0.000829465, 0.0 , 0, 0},
				{0, -0.000540386, 0.0 , 0, 0},
				{0, -0.000395338, 0.0 , 0, 0},
				{0, -0.000306429, 0.0 , 0, 0},
				{0, -0.000177648, 0.0 , 0, 0},
				{0, 0.00017379, 0.0 , 0, 0},
				{0, -0.000157632, 0.0 , 0, 0},
				{0, 9.64237e-05, 0.0 , 0, 0},
				{0, -9.70556e-05, 0.0 , 0, 0},
				{0, -4.69866e-05, 0.0 , 0, 0},
				{0, 1.63663e-05, 0.0 , 0, 0}		};


				static double fferr_u_rhad_1[15][5]    = {{0, 0.000181347, 0.0 , 0, 0, },
				{0, 0.000625347, 0.0 , 0, 0, },
				{0, 0.000105323, 0.0 , 0, 0, },
				{0, 0.000172188, 0.0 , 0, 0, },
				{0, 0.0002147, 0.0 , 0, 0, },
				{0, 0.000284151, 0.0 , 0, 0, },
				{0, 0.000359266, 0.0 , 0, 0, },
				{0, 0.000150708, 0.0 , 0, 0, },
				{0, 0.000158318, 0.0 , 0, 0, },
				{0, 0.000178682, 0.0 , 0, 0, },
				{0, 0.000195705, 0.0 , 0, 0, },
				{0, 7.96882e-05, 0.0 , 0, 0, },
				{0, 0.000112454, 0.0 , 0, 0, },
				{0, 0.000128, 0.0 , 0, 0, },
				{0, 8.18665e-05, 0.0 , 0, 0, }		};


				static double ff_u_e277_1[15][5]    = {{339.661, 624.093, 0.0 , 1275, 1627.21},
				{-567.061, 1290.09, 0.0 , 71.5605, 466.652},
				{238.255, 774.244, 0.0 , 1135.61, 750.047},
				{247.795, 658.396, 0.0 , -970.352, 1099.61},
				{226.762, 698.434, 0.0 , 1565.47, 1263.76},
				{262.277, 1176.89, 0.0 , 1355.21, -481.672},
				{-49.4141, 844.906, 0.0 , -7325.34, -1553.39},
				{-87.1875, 696.387, 0.0 , -1853.57, -2881.98},
				{-154.289, 856.688, 0.0 , -2959.61, 103.547},
				{520.055, 2152.17, 0.0 , -5413.03, 2185.67},
				{251.609, 4106.44, 0.0 , -2786.81, -3427.34},
				{-2264.05, 269.875, 0.0 , -2248.84, -10645.1},
				{709.508, -213.297, 0.0 , 9770.64, 0},
				{-3547.55, -9091.09, 0.0 , -39886.4, 0},
				{-29490.6, -10285.9, 0.0 , 0, 0}		};


				static double fferr_u_e277_1[15][5]    = {{12.5072, 19.1808, 0.0 , 56.5728, 74.6614, },
				{221.308, 401.974, 0.0 , 1140.11, 1132.84, },
				{42.4331, 82.2389, 0.0 , 329.979, 285.149, },
				{83.5602, 179.658, 0.0 , 671.549, 572.752, },
				{143.823, 320.654, 0.0 , 1259.45, 997.952, },
				{246.798, 502.797, 0.0 , 1853.76, 1660.6, },
				{374.871, 741.492, 0.0 , 2752.52, 2493.83, },
				{200.795, 422.051, 0.0 , 1645.1, 1601.34, },
				{360.133, 797.416, 0.0 , 2633.42, 2346.04, },
				{615.117, 1456.01, 0.0 , 5541.49, 3414.9, },
				{763.275, 1746.61, 0.0 , 5678.28, 3773.78, },
				{1208.56, 2266.91, 0.0 , 7008.48, 2613.67, },
				{1924.08, 4822.47, 0.0 , 18390.3, 0, },
				{2687.26, 5777.77, 0.0 , 10252.3, 0, },
				{3438.12, 4090.66, 0.0 , 0, 0, }		};


				static double ff_u_reta_1[15][5]    = {{-0.00303793, -0.00473207, 0.0 , -0.00826848, -0.00803423},
				{-0.00503409, -0.00836867, 0.0 , -0.0134533, -0.0110518},
				{-0.00364953, -0.00612307, 0.0 , -0.00896001, -0.0105894},
				{-0.00418288, -0.00687492, 0.0 , -0.010682, -0.0109594},
				{-0.00311118, -0.00705463, 0.0 , -0.00940549, -0.00992298},
				{-0.0043605, -0.00770319, 0.0 , -0.00907528, -0.00906485},
				{-0.00378245, -0.00605333, 0.0 , -0.0124921, -0.00927895},
				{-0.00421619, -0.00639808, 0.0 , -0.00965708, -0.0102981},
				{-0.00399524, -0.00604165, 0.0 , -0.0107061, -0.0101085},
				{-0.00502968, -0.00677693, 0.0 , -0.0120901, -0.0115098},
				{-0.00461596, -0.0067274, 0.0 , -0.0100319, -0.0102591},
				{-0.00383335, -0.00696129, 0.0 , -0.0127588, -0.0119473},
				{-0.00498444, -0.00732028, 0.0 , -0.00804341, -0.0110978},
				{-0.00401318, -0.00793546, 0.0 , -0.0110644, -0.0107288},
				{-0.00429577, -0.00831807, 0.0 , -0.00992024, -0.012184}		};


				static double fferr_u_reta_1[15][5]    = {{0.00029388, 0.000271826, 0.0 , 0.000459893, 0.000236723, },
				{0.00109531, 0.0012184, 0.0 , 0.0022875, 0.001344, },
				{0.000191699, 0.000196719, 0.0 , 0.000409691, 0.000254063, },
				{0.00029195, 0.000316156, 0.0 , 0.000616165, 0.000395221, },
				{0.000434886, 0.000434925, 0.0 , 0.00108108, 0.000585668, },
				{0.000582662, 0.000598213, 0.0 , 0.0012133, 0.0008161, },
				{0.000714876, 0.000802324, 0.0 , 0.00127321, 0.00115959, },
				{0.000315898, 0.00033714, 0.0 , 0.000670268, 0.000484848, },
				{0.000395686, 0.000460621, 0.0 , 0.000911164, 0.000618438, },
				{0.000501889, 0.000519203, 0.0 , 0.00123644, 0.00090148, },
				{0.000574586, 0.00054747, 0.0 , 0.000988599, 0.00110123, },
				{0.000502603, 0.000560243, 0.0 , 0.000974347, 0.00109874, },
				{0.000600594, 0.000606677, 0.0 , 0.00211201, 0.00143331, },
				{0.000633584, 0.000802555, 0.0 , 0.00141627, 0.00165566, },
				{0.000408664, 0.000558461, 0.0 , 0.00121641, 0.00136294, }		};


				static double ff_u_rphi_1[15][5]    = {{0.00068146, -0.00324428, 0.0 , -0.00419062, -0.00610274},
				{-0.0104192, -0.00496334, 0.0 , -0.0143659, -0.0186231},
				{-0.00130427, -0.0022254, 0.0 , -0.00202072, -0.0067721},
				{-0.000751078, -0.0023191, 0.0 , -0.00395101, -0.00756204},
				{-0.00116313, -0.00298136, 0.0 , -0.00403774, -0.00420976},
				{0.000656128, -0.00457132, 0.0 , 8.74996e-05, -0.00590384},
				{-0.00301665, -0.00252694, 0.0 , -0.00612503, -0.00445592},
				{-0.00202358, -0.00293779, 0.0 , -0.00344384, -0.00612229},
				{-0.00377446, -0.0047946, 0.0 , -0.00498122, -0.00505155},
				{0.000690877, -0.00582123, 0.0 , -0.00751323, -0.00715911},
				{-0.00393122, -0.00613755, 0.0 , -0.00458479, -0.00560987},
				{-0.00115108, -0.00388741, 0.0 , -0.00355798, -0.00760603},
				{-0.0025382, -0.00574237, 0.0 , -0.00149977, -0.00499296},
				{-0.000762939, -0.00225383, 0.0 , -0.00715232, -0.00625247},
				{-0.00485635, -0.00520003, 0.0 , -0.00469387, -0.00797141}		};


				static double fferr_u_rphi_1[15][5]    = {{0.00060668, 0.000557751, 0.0 , 0.000854294, 0.000567622, },
				{0.00242161, 0.00353502, 0.0 , 0.0028209, 0.00131915, },
				{0.000547156, 0.000555717, 0.0 , 0.000956515, 0.000705024, },
				{0.000897212, 0.000966257, 0.0 , 0.00162608, 0.00101354, },
				{0.00129392, 0.0013454, 0.0 , 0.00227141, 0.00182802, },
				{0.00227929, 0.00169643, 0.0 , 0.00439188, 0.00168949, },
				{0.00197338, 0.00198614, 0.0 , 0.00289257, 0.00210919, },
				{0.00101391, 0.000883433, 0.0 , 0.00147828, 0.000889948, },
				{0.000936499, 0.000856748, 0.0 , 0.00123664, 0.00143238, },
				{0.00230477, 0.00107306, 0.0 , 0.00148667, 0.0011832, },
				{0.000556625, 0.000759006, 0.0 , 0.000999447, 0.00155019, },
				{0.0013088, 0.000827525, 0.0 , 0.00253212, 0.00117108, },
				{0.0013058, 0.000933278, 0.0 , 0.00251755, 0.00156996, },
				{0.00254747, 0.0019696, 0.0 , 0.00139601, 0.0013063, },
				{0.0011455, 0.000578641, 0.0 , 0.00116625, 0.00132622, }		};


				static double ff_u_weta2_1[15][5]    = {{0.000247213, 0.000349269, 0.0 , 0.000428656, 0.000383694},
				{0.000354492, 0.000383229, 0.0 , 0.000830716, 0.000566308},
				{0.000158496, 0.000218676, 0.0 , 0.000418682, 0.000404171},
				{0.000156181, 0.000244501, 0.0 , 0.000529232, 0.000441993},
				{0.00012584, 0.000218597, 0.0 , 0.000493808, 0.000409132},
				{0.000173436, 0.000301746, 0.0 , 0.000438366, 0.000310271},
				{0.000240498, 0.000247964, 0.0 , 0.000693056, 0.000374262},
				{0.000188134, 0.000221607, 0.0 , 0.000476297, 0.000393956},
				{0.000184465, 0.000285978, 0.0 , 0.00052875, 0.000427058},
				{0.000178035, 0.000307934, 0.0 , 0.000833988, 0.000475354},
				{0.000260865, 0.000303932, 0.0 , 0.000650415, 0.000420355},
				{0.000195489, 0.000236308, 0.0 , 0.000683488, 0.000579618},
				{0.00019078, 0.000301246, 0.0 , 0.000421291, 0.000541713},
				{0.00025134, 0.00026574, 0.0 , 0.000752818, 0.000460996},
				{0.000323255, 0.000339472, 0.0 , 0.000597012, 0.000526992}		};


				static double fferr_u_weta2_1[15][5]    = {{1.42364e-05, 1.22769e-05, 0.0 , 2.23438e-05, 1.11233e-05, },
				{5.82739e-05, 6.67065e-05, 0.0 , 0.000129579, 7.30153e-05, },
				{1.11663e-05, 1.1694e-05, 0.0 , 2.76874e-05, 1.68383e-05, },
				{1.72274e-05, 1.82871e-05, 0.0 , 4.43091e-05, 2.72703e-05, },
				{2.56104e-05, 2.81413e-05, 0.0 , 6.85071e-05, 3.93601e-05, },
				{3.89044e-05, 3.47992e-05, 0.0 , 0.000114014, 5.86889e-05, },
				{4.08967e-05, 4.9398e-05, 0.0 , 9.82712e-05, 7.67881e-05, },
				{2.12706e-05, 2.16837e-05, 0.0 , 5.82007e-05, 3.47074e-05, },
				{2.94981e-05, 2.33959e-05, 0.0 , 8.06597e-05, 4.60028e-05, },
				{4.04921e-05, 3.15393e-05, 0.0 , 7.09301e-05, 7.08384e-05, },
				{2.91551e-05, 3.55288e-05, 0.0 , 7.10366e-05, 7.65379e-05, },
				{3.3054e-05, 3.47554e-05, 0.0 , 9.54915e-05, 8.69309e-05, },
				{4.75715e-05, 6.26717e-05, 0.0 , 8.86041e-05, 0.000114895, },
				{4.02347e-05, 7.0345e-05, 0.0 , 0.000123531, 0.000138697, },
				{3.29738e-05, 3.59574e-05, 0.0 , 6.60206e-05, 0.000108874, }		};


				static double ff_u_f1_1[15][5]    = {{-0.00804952, -0.00495076, 0.0 , -0.00224099, -0.0110609},
				{0.00767335, -0.021263, 0.0 , 0.0172966, 0.0148135},
				{-0.0079101, -0.00507264, 0.0 , -0.00222075, 0.00775559},
				{-0.00456026, -0.00442597, 0.0 , 0.00863138, 0.00891323},
				{-0.00649722, -0.00211607, 0.0 , -0.00919032, 0.00170924},
				{-0.00454852, -0.00142473, 0.0 , -0.00594071, -0.00309345},
				{-9.02414e-05, -0.00240089, 0.0 , 0.0345059, 0.0122588},
				{-0.00239705, -0.00732562, 0.0 , 0.00156075, 0.0119287},
				{-0.00114487, -0.0115174, 0.0 , 0.0153856, -0.0023104},
				{-0.00290999, -0.00689569, 0.0 , 0.0272582, 0.00472388},
				{-0.00700761, -0.0106462, 0.0 , -0.000720039, -0.000287324},
				{7.3716e-05, -0.00306676, 0.0 , 0.0116967, 0.0211433},
				{-0.0058928, -0.00907935, 0.0 , -0.0360462, -0.0131974},
				{-0.0054311, -0.00207376, 0.0 , 0.0383185, 0.00415701},
				{0.0140881, 0.0103389, 0.0 , -0.00184068, 0.0176148}		};


				static double fferr_u_f1_1[15][5]    = {{0.00126978, 0.00106391, 0.0 , 0.00177614, 0.00157404, },
				{0.00904144, 0.0082044, 0.0 , 0.0134487, 0.0102839, },
				{0.00151245, 0.00145549, 0.0 , 0.00252992, 0.00205485, },
				{0.00244029, 0.00252873, 0.0 , 0.0046732, 0.00319431, },
				{0.00365242, 0.00382909, 0.0 , 0.00723873, 0.00471036, },
				{0.00544011, 0.00506788, 0.0 , 0.00969074, 0.00656135, },
				{0.00692898, 0.0070114, 0.0 , 0.0136716, 0.00900474, },
				{0.0031417, 0.00308915, 0.0 , 0.00663114, 0.00422206, },
				{0.00437726, 0.00425639, 0.0 , 0.00843432, 0.00556759, },
				{0.00681953, 0.00632741, 0.0 , 0.0159921, 0.00822927, },
				{0.00647607, 0.00643972, 0.0 , 0.0129435, 0.00987909, },
				{0.00710187, 0.00606732, 0.0 , 0.0151239, 0.00819528, },
				{0.00936768, 0.0110806, 0.0 , 0.022205, 0.0108735, },
				{0.00884373, 0.0100081, 0.0 , 0.02324, 0.0133807, },
				{0.00624501, 0.0063279, 0.0 , 0.0136297, 0.0113944, }		};


				static double ff_u_fside_1[15][5]    = {{0.0155135, 0.0220167, 0.0 , 0.0296753, 0.0321464},
				{0.0105868, 0.0342841, 0.0 , 0.0706255, 0.00640525},
				{-0.0136581, -0.0123671, 0.0 , 0.00641966, -0.00306158},
				{-0.0133554, -0.0154676, 0.0 , 0.0186851, 0.00218543},
				{-0.0300223, -0.0141245, 0.0 , 0.000930965, -0.00197822},
				{-0.0117171, -0.0166041, 0.0 , 0.0183361, -0.00307152},
				{-0.0146641, 0.0037297, 0.0 , 0.0396995, 0.00481403},
				{-0.0173963, -0.00789249, 0.0 , 0.0113315, 0.00647959},
				{-0.00482082, -0.00259236, 0.0 , 0.00678036, 0.022447},
				{-0.00429623, 0.0150503, 0.0 , 0.0223455, 0.0154791},
				{-0.0233557, 0.0161442, 0.0 , 0.0283706, 0.0353783},
				{-0.00931516, 0.0139003, 0.0 , 0.0430865, 0.0140221},
				{-0.00263388, 0.0260362, 0.0 , 0.0275361, -0.0140866},
				{0.00503857, 0.0186764, 0.0 , -0.0232156, 0.0322254},
				{0.0187584, 0.0173351, 0.0 , 0.0106174, 0.0104886}		};


				static double fferr_u_fside_1[15][5]    = {{0.00120919, 0.00136589, 0.0 , 0.00229028, 0.00127677, },
				{0.0123285, 0.0126082, 0.0 , 0.019137, 0.024048, },
				{0.00246418, 0.00249873, 0.0 , 0.0046828, 0.00369611, },
				{0.00414917, 0.00432628, 0.0 , 0.00824321, 0.00542172, },
				{0.00663259, 0.00658155, 0.0 , 0.0135967, 0.00799057, },
				{0.00859107, 0.00918709, 0.0 , 0.0194245, 0.0118434, },
				{0.0116663, 0.0103753, 0.0 , 0.0171588, 0.0116317, },
				{0.00520366, 0.00471549, 0.0 , 0.00970927, 0.00599543, },
				{0.00621052, 0.00536594, 0.0 , 0.0125659, 0.00558025, },
				{0.00893539, 0.00638001, 0.0 , 0.0189733, 0.0122456, },
				{0.0110185, 0.00724637, 0.0 , 0.0139533, 0.00532439, },
				{0.0113048, 0.0061431, 0.0 , 0.0163556, 0.0102629, },
				{0.0135908, 0.00849181, 0.0 , 0.0145144, 0.0292707, },
				{0.012021, 0.00818767, 0.0 , 0.0498279, 0.00694742, },
				{0.0116929, 0.00730994, 0.0 , 0.011052, 0.00679777, }		};


				static double ff_u_wtot_1[15][5]    = {{0.0542138, 0.0630641, 0.0 , 0.0750129, 0.107208},
				{0.0854545, 0.0783963, 0.0 , 0.269181, 0.116679},
				{0.0288438, 0.0431421, 0.0 , 0.07813, 0.04388},
				{0.0148269, 0.036521, 0.0 , 0.0872544, 0.0478377},
				{0.00920808, 0.043797, 0.0 , 0.06265, 0.0483326},
				{0.0144322, 0.0679471, 0.0 , 0.0905296, 0.0833433},
				{0.0212295, 0.0822723, 0.0 , 0.120256, 0.0403233},
				{0.0382875, 0.0543046, 0.0 , 0.0809388, 0.0702887},
				{0.0640522, 0.0613823, 0.0 , 0.115806, 0.0868633},
				{0.0551521, 0.0904077, 0.0 , 0.102067, 0.0973952},
				{0.0557435, 0.102941, 0.0 , 0.161325, 0.11642},
				{0.0232961, 0.0762924, 0.0 , 0.131878, 0.0665249},
				{0.0695817, 0.0446084, 0.0 , 0.140541, -0.000372767},
				{0.077953, 0.0738933, 0.0 , 0.179414, 0.133626},
				{0.0280337, 0.0601866, 0.0 , 0.146889, 0.0807018}		};


				static double fferr_u_wtot_1[15][5]    = {{0.00498308, 0.00469158, 0.0 , 0.00729466, 0.0040993, },
				{0.0313166, 0.0303791, 0.0 , 0.0582982, 0.0376857, },
				{0.00572818, 0.00530261, 0.0 , 0.010099, 0.00711289, },
				{0.00835833, 0.00823867, 0.0 , 0.0152566, 0.01083, },
				{0.0118095, 0.0114923, 0.0 , 0.0235451, 0.0159052, },
				{0.017023, 0.0166608, 0.0 , 0.0331311, 0.0214521, },
				{0.0223903, 0.0202923, 0.0 , 0.0402393, 0.0303348, },
				{0.00880434, 0.00919048, 0.0 , 0.0185636, 0.0151003, },
				{0.0123329, 0.0124618, 0.0 , 0.0230459, 0.0176558, },
				{0.016778, 0.016842, 0.0 , 0.034455, 0.029516, },
				{0.0173897, 0.0176979, 0.0 , 0.0290912, 0.0248156, },
				{0.0191424, 0.0188725, 0.0 , 0.0402621, 0.029053, },
				{0.0225205, 0.0276976, 0.0 , 0.0412131, 0.0569085, },
				{0.0224499, 0.0293177, 0.0 , 0.0348881, 0.0567449, },
				{0.0166941, 0.0206958, 0.0 , 0.0325392, 0.0423401, }		};


				static double ff_u_w1_1[15][5]    = {{0.0105486, 0.0140757, 0.0 , 0.011699, 0.0378897},
				{0.011151, 0.00399363, 0.0 , 0.0284552, 0.0643052},
				{-0.0033834, 0.00539947, 0.0 , 0.00162524, 0.0433353},
				{-0.00433189, 0.00354975, 0.0 , 0.00595397, 0.0374923},
				{-0.00838393, 0.00444543, 0.0 , 0.00326586, 0.0301573},
				{-0.00808519, 0.00611722, 0.0 , 0.00701106, 0.0302147},
				{-0.000296533, 0.00418311, 0.0 , 0.0132251, 0.0293806},
				{-0.00743639, 0.00324494, 0.0 , 0.00245196, 0.0242252},
				{-0.00515205, 0.00134289, 0.0 , 0.00801015, 0.0251266},
				{-0.0053122, 0.00853813, 0.0 , 0.0027194, 0.0196079},
				{-0.00733364, 0.0102187, 0.0 , 0.0120651, 0.0270877},
				{-0.00532991, 0.00725156, 0.0 , 0.0177277, 0.0223824},
				{-0.00219285, 0.00548047, 0.0 , 0.00161952, 0.0121429},
				{-0.00592583, 0.00292569, 0.0 , -0.00109345, 0.0398127},
				{0.00470114, 0.0122934, 0.0 , 0.0094471, 0.0333131}		};


				static double fferr_u_w1_1[15][5]    = {{0.000665544, 0.000666051, 0.0 , 0.00106093, 0.000793212, },
				{0.0034367, 0.00427148, 0.0 , 0.00651533, 0.00719657, },
				{0.000757546, 0.000811069, 0.0 , 0.00147783, 0.00144922, },
				{0.00121692, 0.00137273, 0.0 , 0.00257662, 0.00228503, },
				{0.00200361, 0.0021035, 0.0 , 0.00401507, 0.00346469, },
				{0.0028597, 0.00271424, 0.0 , 0.00587142, 0.00471434, },
				{0.00398378, 0.00401273, 0.0 , 0.00685346, 0.00502075, },
				{0.00174663, 0.00168499, 0.0 , 0.00327418, 0.00306479, },
				{0.00247598, 0.0022111, 0.0 , 0.00458575, 0.00313219, },
				{0.00335131, 0.00307125, 0.0 , 0.0101618, 0.0066476, },
				{0.00354756, 0.00331971, 0.0 , 0.00566678, 0.00587633, },
				{0.00406155, 0.00334173, 0.0 , 0.00863478, 0.00673023, },
				{0.00464428, 0.00609112, 0.0 , 0.00874974, 0.0122021, },
				{0.00495936, 0.00559155, 0.0 , 0.0148626, 0.0070016, },
				{0.0042811, 0.00384543, 0.0 , 0.00749201, 0.00836944, }		};


				static double ff_u_deltae_1[15][5]    = {{-0.913189, -2.05949, 0.0 , -4.79776, -5.07556},
				{10.4679, 1.24322, 0.0 , 13.6182, -6.55383},
				{-4.84641, -6.1808, 0.0 , -3.25845, -3.79482},
				{-4.42576, -4.28334, 0.0 , -1.88213, -3.59899},
				{-6.57843, -3.49266, 0.0 , -3.85539, -4.44414},
				{-6.24129, -5.19426, 0.0 , -7.74202, -1.65102},
				{-4.74307, -4.69457, 0.0 , 6.62707, -7.26402},
				{-2.02354, -3.28544, 0.0 , 0.468592, -6.81869},
				{-0.393068, -4.35247, 0.0 , 1.83028, 2.22272},
				{-3.94222, 0.524851, 0.0 , 7.32029, 1.12509},
				{-1.07912, -2.98111, 0.0 , -2.48396, 2.33716},
				{0.710398, 0.902512, 0.0 , 3.78399, 2.15787},
				{-1.83489, -2.29196, 0.0 , -3.09179, 3.59533},
				{-5.34414, -8.28493, 0.0 , 4.8646, -0.55538},
				{-3.00959, -1.95066, 0.0 , 1.7089, -0.282518}		};


				static double fferr_u_deltae_1[15][5]    = {{0.26981, 0.260574, 0.0 , 0.609633, 0.409524, },
				{3.09205, 3.38745, 0.0 , 3.80329, 4.52667, },
				{0.758957, 0.729211, 0.0 , 1.32049, 0.842205, },
				{1.14657, 1.06295, 0.0 , 1.98044, 1.29965, },
				{1.62548, 1.45209, 0.0 , 2.92206, 1.81002, },
				{2.42416, 2.04958, 0.0 , 5.03059, 2.17033, },
				{2.67691, 2.39345, 0.0 , 3.37588, 3.42405, },
				{1.02742, 1.03038, 0.0 , 2.29238, 1.84036, },
				{1.34802, 1.22527, 0.0 , 2.47478, 1.97475, },
				{2.25045, 1.34654, 0.0 , 2.92235, 3.42087, },
				{1.57554, 1.66207, 0.0 , 3.91304, 2.38706, },
				{1.75047, 1.39656, 0.0 , 3.98715, 2.85094, },
				{2.12432, 2.56837, 0.0 , 8.16954, 3.35658, },
				{2.7307, 3.7607, 0.0 , 5.27935, 5.40392, },
				{1.79718, 1.57872, 0.0 , 3.98445, 4.81752, }		};


				static double ff_u_eratio_1[15][5]    = {{-0.00200909, -0.00330174, 0.0 , -0.00102842, -0.00149381},
				{-0.0335343, -0.0273034, 0.0 , -0.0506559, -0.0115845},
				{0.0201253, 0.0174572, 0.0 , 0.0143047, 0.00956243},
				{0.0129388, 0.0120704, 0.0 , 0.00441796, 0.00509423},
				{0.0163425, 0.0131615, 0.0 , 0.00991684, 0.00729245},
				{0.0181673, 0.00818235, 0.0 , 0.0251261, -0.00209594},
				{0.00855649, -0.000451744, 0.0 , 0.00579733, 0.0167575},
				{-0.00073576, 0.00566173, 0.0 , 0.00712639, 0.00649625},
				{0.0033356, 0.00321251, 0.0 , 0.0101345, -0.00344568},
				{-0.000772417, -0.0018388, 0.0 , -0.00328785, -0.00586849},
				{-0.00143987, -0.00241333, 0.0 , -0.011542, -0.00611836},
				{0.00064683, -0.0070101, 0.0 , 0.00176275, 0.00869679},
				{-0.0071649, -0.00825155, 0.0 , -0.00809437, -0.00679457},
				{-0.00620246, -0.0120607, 0.0 , -0.000706553, -0.00957245},
				{0.000347137, -0.00589263, 0.0 , -0.00477821, 0.00582457}		};


				static double fferr_u_eratio_1[15][5]    = {{0.000951619, 0.00101875, 0.0 , 0.0017602, 0.00108214, },
				{0.00950609, 0.00853497, 0.0 , 0.00881612, 0.0103592, },
				{0.00225191, 0.00208682, 0.0 , 0.00392987, 0.00248288, },
				{0.00331249, 0.00317732, 0.0 , 0.00537519, 0.00342771, },
				{0.00494049, 0.00447424, 0.0 , 0.0089357, 0.00476217, },
				{0.00651736, 0.00525778, 0.0 , 0.0137842, 0.00566652, },
				{0.00752016, 0.00542731, 0.0 , 0.00911397, 0.00809654, },
				{0.0029881, 0.00301392, 0.0 , 0.00602486, 0.00372128, },
				{0.0039474, 0.00356973, 0.0 , 0.00812744, 0.00370722, },
				{0.00496851, 0.00465623, 0.0 , 0.00470093, 0.00478252, },
				{0.00512769, 0.00460391, 0.0 , 0.00261837, 0.00271086, },
				{0.00515444, 0.00278401, 0.0 , 0.0123674, 0.00864433, },
				{0.00688082, 0.00406088, 0.0 , 0.00216863, 0.00720943, },
				{0.00444185, 0.00619201, 0.0 , 0.00333475, 0.00470056, },
				{0.00442667, 0.00492966, 0.0 , 0.00242859, 0.00425736, }		};


				if(isConv){
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad1_1[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad_1[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_e277_1[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_reta_1[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rphi_1[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_weta2_1[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_f1_1[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_fside_1[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_wtot_1[0][0] ,  0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_w1_1[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_deltae_1[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_eratio_1[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad1_1[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad_1[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_e277_1[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_reta_1[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rphi_1[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_weta2_1[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_f1_1[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_fside_1[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_wtot_1[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_w1_1[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_deltae_1[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_eratio_1[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad1_1[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad_1[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_e277_1[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_reta_1[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rphi_1[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_weta2_1[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_f1_1[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_fside_1[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_wtot_1[0][0],   0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_w1_1[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_deltae_1[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_eratio_1[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad1_1[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad_1[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_e277_1[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_reta_1[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rphi_1[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_weta2_1[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_f1_1[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_fside_1[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_wtot_1[0][0],   0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_w1_1[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_deltae_1[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_eratio_1[0][0], 0. );
				}

				break;

		case 2:  // RobustTightX+Iso preselection

				//converted photons

				static double ff_c_rhad1_2[15][5]    = {{-0.00201901, -0.00313722, 0.0 , -0.00296045, -0.000667818},
				{-0.00194984, 0.00073684, 0.0 , 0.000114076, -0.000551676},
				{-0.000735819, -0.00103701, 0.0 , -0.000403063, -0.00112865},
				{-0.000205322, -0.000577202, 0.0 , 0.00029276, -0.000669067},
				{-2.01465e-05, -3.32503e-05, 0.0 , 0.000226236, -0.000515795},
				{-0.000242492, 0.000636269, 0.0 , 2.44312e-05, -4.17088e-05},
				{-0.000768584, -0.000491206, 0.0 , -0.00186764, 0.00067537},
				{-0.000245252, -0.000177353, 0.0 , -0.000531317, -0.000276207},
				{-0.00035814, -2.25012e-05, 0.0 , 0.000344019, 0.000143763},
				{-0.00042429, -0.000571946, 0.0 , -3.83843e-05, 5.27865e-05},
				{-1.32549e-05, 0.000183842, 0.0 , 0.000288124, 3.70078e-05},
				{0.000192393, -0.000189087, 0.0 , 0.000198758, -2.05191e-05},
				{0.000581023, -0.000160059, 0.0 , -0.000881137, 8.0988e-05},
				{-0.00031188, 0.000213898, 0.0 , 0.000679157, -1.82621e-06},
				{-0.000387427, -0.000109134, 0.0 , -0.000376867, 4.52041e-07}		};


				static double fferr_c_rhad1_2[15][5]    = {{0.00103176, 0.00163946, 0.0 , 0.000791676, 0.000635162, },
				{0.00183577, 0.000896634, 0.0 , 0.00185998, 0.00121603, },
				{0.00024689, 0.00043341, 0.0 , 0.000371571, 0.000233989, },
				{0.00028676, 0.000353547, 0.0 , 0.000496861, 0.000302753, },
				{0.000340068, 0.000359242, 0.0 , 0.000693998, 0.000329259, },
				{0.000565694, 0.000328729, 0.0 , 0.00080998, 0.000464987, },
				{0.00026204, 0.000373919, 0.0 , 0.0011114, 0.000602772, },
				{0.000179762, 0.000188531, 0.0 , 0.00035961, 0.000186171, },
				{0.000221073, 0.000200906, 0.0 , 0.00038286, 0.00021588, },
				{0.000382053, 0.000624342, 0.0 , 0.000768646, 0.000337005, },
				{0.000172699, 0.000178854, 0.0 , 0.000389863, 0.000207309, },
				{0.00018305, 0.000235443, 0.0 , 0.000375293, 0.000261447, },
				{0.000172714, 0.000251359, 0.0 , 0.000970002, 0.000315525, },
				{0.000279577, 0.000167228, 0.0 , 0.000549967, 0.000292164, },
				{0.000150386, 7.27626e-05, 0.0 , 0.000296482, 0.000213175, }		};


				static double ff_c_rhad_2[15][5]    = {{0, -0.00267811, 0.0 , 0, 0},
				{0, 0.000597254, 0.0 , 0, 0},
				{0, -0.000320961, 0.0 , 0, 0},
				{0, -0.000704423, 0.0 , 0, 0},
				{0, -2.06614e-06, 0.0 , 0, 0},
				{0, -0.000114451, 0.0 , 0, 0},
				{0, -0.000322536, 0.0 , 0, 0},
				{0, -0.000401715, 0.0 , 0, 0},
				{0, -0.00046355, 0.0 , 0, 0},
				{0, 0.000255787, 0.0 , 0, 0},
				{0, -0.000272332, 0.0 , 0, 0},
				{0, -9.40651e-05, 0.0 , 0, 0},
				{0, -0.000103012, 0.0 , 0, 0},
				{0, -0.000152187, 0.0 , 0, 0},
				{0, -5.4725e-06, 0.0 , 0, 0}		};


				static double fferr_c_rhad_2[15][5]    = {{0, 0.00051383, 0.0 , 0, 0, },
				{0, 0.00121605, 0.0 , 0, 0, },
				{0, 0.000145232, 0.0 , 0, 0, },
				{0, 0.000251349, 0.0 , 0, 0, },
				{0, 0.000237684, 0.0 , 0, 0, },
				{0, 0.000266255, 0.0 , 0, 0, },
				{0, 0.000419039, 0.0 , 0, 0, },
				{0, 0.000183116, 0.0 , 0, 0, },
				{0, 0.000186425, 0.0 , 0, 0, },
				{0, 0.000173638, 0.0 , 0, 0, },
				{0, 0.000202543, 0.0 , 0, 0, },
				{0, 0.000118862, 0.0 , 0, 0, },
				{0, 0.000132471, 0.0 , 0, 0, },
				{0, 0.00010412, 0.0 , 0, 0, },
				{0, 5.54677e-05, 0.0 , 0, 0, }		};


				static double ff_c_e277_2[15][5]    = {{292.812, 552.564, 0.0 , 1858.92, 1315.15},
				{-1108.89, 606.417, 0.0 , -1355.12, -1395.03},
				{-60.1387, 226.484, 0.0 , 240.883, -201.125},
				{-84.8359, 506.275, 0.0 , -567.82, 322.086},
				{-76.1035, -191.895, 0.0 , 362.145, -376.359},
				{-30.9648, 610.906, 0.0 , 251.418, -495.133},
				{864.992, 1164.83, 0.0 , -386.867, -1029.27},
				{-124.102, 698.242, 0.0 , -2905.01, -1040.05},
				{54.9102, 1913.3, 0.0 , -2607.66, -1012},
				{-1746.42, 2017.86, 0.0 , -6622.99, -9247.95},
				{-1428.04, 2427.29, 0.0 , -4521.28, -4895.81},
				{755.391, -567.312, 0.0 , -7029.61, -12607.9},
				{-4864.26, 1559.73, 0.0 , 20675.2, 289920},
				{6016.52, -3632.5, 0.0 , 23753.9, 284149},
				{-34850.1, -10252.1, 0.0 , 11963.5, 0}		};


				static double fferr_c_e277_2[15][5]    = {{16.9937, 22.1776, 0.0 , 59.3221, 64.9376, },
				{348.757, 367.133, 0.0 , 1119.23, 1203.96, },
				{56.5056, 74.9581, 0.0 , 230.369, 210.4, },
				{102.748, 153.431, 0.0 , 456.265, 423.036, },
				{179.555, 288.185, 0.0 , 912.761, 798.338, },
				{288.933, 438.624, 0.0 , 1563.95, 1309.21, },
				{407.74, 710.795, 0.0 , 2008.11, 1938.43, },
				{264.667, 379.632, 0.0 , 1443.77, 1188.6, },
				{421.581, 648.883, 0.0 , 2096.11, 2012.13, },
				{993.122, 1286.71, 0.0 , 4284.7, 3497.84, },
				{1141.75, 1679.72, 0.0 , 6216.64, 3238.83, },
				{1259.68, 2732.66, 0.0 , 9649.86, 4944.16, },
				{2594.43, 4398, 0.0 , 11945.3, 2285.51, },
				{3220.26, 6675.58, 0.0 , 13576.2, 0, },
				{5177.39, 5509.35, 0.0 , 14232.4, 0, }		};


				static double ff_c_reta_2[15][5]    = {{-0.00688416, -0.00753719, 0.0 , -0.00856042, -0.00870299},
				{-0.00370771, -0.0100653, 0.0 , -0.0213614, -0.0127997},
				{-0.00223345, -0.00784421, 0.0 , -0.0107826, -0.0120487},
				{-0.00275522, -0.0072214, 0.0 , -0.012127, -0.0110034},
				{-0.00328398, -0.00681752, 0.0 , -0.00996149, -0.0112146},
				{-0.00527, -0.0068711, 0.0 , -0.0109803, -0.0127329},
				{-0.0051536, -0.00540483, 0.0 , -0.0117257, -0.00892454},
				{-0.0054307, -0.00787234, 0.0 , -0.0123243, -0.010668},
				{-0.00599879, -0.00709391, 0.0 , -0.0121586, -0.0115734},
				{-0.00534308, -0.00595999, 0.0 , -0.0127543, -0.0100847},
				{-0.00567311, -0.00757915, 0.0 , -0.0101377, -0.00980741},
				{-0.00502104, -0.00720555, 0.0 , -0.0125296, -0.00838041},
				{-0.00542802, -0.00866467, 0.0 , -0.00962681, -0.0108652},
				{-0.00526196, -0.00694442, 0.0 , -0.0131562, -0.011188},
				{-0.00471318, -0.00807583, 0.0 , -0.0103003, -0.0108964}		};


				static double fferr_c_reta_2[15][5]    = {{0.00255111, 0.001083, 0.0 , 0.00101657, 0.000734151, },
				{0.00435858, 0.00257464, 0.0 , 0.00252164, 0.00180277, },
				{0.000878856, 0.000450155, 0.0 , 0.000709125, 0.0004483, },
				{0.00107457, 0.000677016, 0.0 , 0.000808212, 0.00066297, },
				{0.00136119, 0.000959824, 0.0 , 0.00150307, 0.000875462, },
				{0.00152935, 0.00108123, 0.0 , 0.00139023, 0.000972838, },
				{0.00133476, 0.00139002, 0.0 , 0.00179291, 0.00165382, },
				{0.000551478, 0.00047415, 0.0 , 0.000693544, 0.000643001, },
				{0.000640794, 0.000605592, 0.0 , 0.000897329, 0.000690338, },
				{0.000980409, 0.000964076, 0.0 , 0.00112514, 0.00123906, },
				{0.000780771, 0.000682797, 0.0 , 0.00107294, 0.00108746, },
				{0.00095655, 0.00132078, 0.0 , 0.0012792, 0.00140197, },
				{0.00120022, 0.000772366, 0.0 , 0.00167379, 0.00198208, },
				{0.000888741, 0.000917726, 0.0 , 0.00182499, 0.00188752, },
				{0.000659408, 0.000793174, 0.0 , 0.00119454, 0.00159891, }		};


				static double ff_c_rphi_2[15][5]    = {{-0.0121642, -0.0133714, 0.0 , -0.00845546, -0.00878078},
				{0.017596, -0.0106719, 0.0 , 0.00174105, -0.0188595},
				{-0.00716972, -0.00748992, 0.0 , -0.0168079, -0.0122809},
				{-0.0162054, -0.0134668, 0.0 , -0.0168058, -0.00932646},
				{-0.0171915, -0.0192338, 0.0 , -0.0128222, -0.011477},
				{0.00106925, -0.0128191, 0.0 , -0.00775439, -0.00240481},
				{0.00514418, -0.00667948, 0.0 , 0.00543797, -0.00909787},
				{-0.00416613, -0.00252634, 0.0 , 0.000512362, -0.00419998},
				{-0.00517762, -0.0029577, 0.0 , -0.00333422, -0.00731343},
				{-0.00432593, -0.00832689, 0.0 , -0.00759047, -0.00619233},
				{-0.0014897, -0.00353181, 0.0 , -0.000588059, -0.0065943},
				{-0.0050059, -0.00498551, 0.0 , -0.00305718, -0.00489056},
				{-0.0118812, -0.0031141, 0.0 , -0.00793666, -0.00806504},
				{-0.00520885, -0.00514621, 0.0 , -0.00869012, -0.00566143},
				{-0.0043537, -0.0062288, 0.0 , -0.00662583, -0.00726759}		};


				static double fferr_c_rphi_2[15][5]    = {{0.00449247, 0.00173156, 0.0 , 0.00192578, 0.00162946, },
				{0.0280056, 0.0106167, 0.0 , 0.0157169, 0.00943754, },
				{0.00357805, 0.00187352, 0.0 , 0.00232485, 0.0016503, },
				{0.00504079, 0.00253807, 0.0 , 0.0032725, 0.00210281, },
				{0.00714157, 0.00326615, 0.0 , 0.00429448, 0.0020223, },
				{0.00852127, 0.00432726, 0.0 , 0.00580422, 0.00349559, },
				{0.0100544, 0.00500652, 0.0 , 0.00680075, 0.00260472, },
				{0.00398804, 0.00229927, 0.0 , 0.00321634, 0.00131172, },
				{0.00303024, 0.00237425, 0.0 , 0.00267085, 0.00116956, },
				{0.00418036, 0.00204212, 0.0 , 0.00208454, 0.00226893, },
				{0.00384561, 0.00222793, 0.0 , 0.0046508, 0.00131023, },
				{0.00217538, 0.00212415, 0.0 , 0.00230496, 0.0013869, },
				{0.00207891, 0.00282755, 0.0 , 0.00188341, 0.00225156, },
				{0.00172663, 0.00312126, 0.0 , 0.00321313, 0.00178642, },
				{0.00120243, 0.00126862, 0.0 , 0.00178394, 0.00154664, }		};


				static double ff_c_weta2_2[15][5]    = {{0.000230247, 0.000363054, 0.0 , 0.000441111, 0.000344491},
				{0.000235897, 0.000200076, 0.0 , 0.000788011, 0.000411027},
				{0.0002469, 0.000325355, 0.0 , 0.000566152, 0.00055865},
				{0.000257181, 0.000298261, 0.0 , 0.000596602, 0.000483956},
				{0.000213073, 0.000257116, 0.0 , 0.000451512, 0.000454521},
				{0.000252503, 0.000294237, 0.0 , 0.000569489, 0.00052754},
				{0.000147904, 0.000145645, 0.0 , 0.000557142, 0.000407086},
				{0.000205801, 0.000313463, 0.0 , 0.000599699, 0.000431675},
				{0.000183693, 0.000237687, 0.0 , 0.000480846, 0.000546931},
				{0.00037759, 0.000272457, 0.0 , 0.000662521, 0.000436339},
				{0.000312335, 0.000336043, 0.0 , 0.000446449, 0.00040972},
				{0.00017615, 0.000372329, 0.0 , 0.000591191, 0.000311105},
				{0.000235946, 0.000338168, 0.0 , 0.000585613, 0.000531849},
				{0.000188489, 0.000287789, 0.0 , 0.000735307, 0.000523293},
				{0.0002956, 0.000299294, 0.0 , 0.000784857, 0.000530182}		};


				static double fferr_c_weta2_2[15][5]    = {{0.00011927, 4.33028e-05, 0.0 , 4.30803e-05, 2.92286e-05, },
				{0.000315406, 0.000122916, 0.0 , 0.000152202, 0.000100418, },
				{3.09048e-05, 1.86457e-05, 0.0 , 2.78797e-05, 2.35694e-05, },
				{3.83823e-05, 2.73778e-05, 0.0 , 3.79295e-05, 3.59694e-05, },
				{4.76141e-05, 3.69186e-05, 0.0 , 7.05979e-05, 4.65479e-05, },
				{6.60455e-05, 4.42169e-05, 0.0 , 6.65515e-05, 6.37435e-05, },
				{7.67593e-05, 5.48784e-05, 0.0 , 9.87655e-05, 8.04324e-05, },
				{3.31277e-05, 2.55947e-05, 0.0 , 4.05224e-05, 3.7123e-05, },
				{3.59119e-05, 3.57668e-05, 0.0 , 0.000104922, 5.01036e-05, },
				{5.10322e-05, 4.15956e-05, 0.0 , 9.18777e-05, 8.48779e-05, },
				{4.75078e-05, 4.76227e-05, 0.0 , 8.67937e-05, 7.8333e-05, },
				{5.04131e-05, 5.31084e-05, 0.0 , 0.000108347, 8.52157e-05, },
				{9.55141e-05, 7.01742e-05, 0.0 , 0.000122697, 0.000146498, },
				{6.20522e-05, 6.4247e-05, 0.0 , 0.000143766, 0.000152077, },
				{4.46393e-05, 5.03732e-05, 0.0 , 0.00011446, 0.000127187, }		};


				static double ff_c_f1_2[15][5]    = {{-0.00283736, -0.00224003, 0.0 , 0.00277412, -0.0261427},
				{0.0237384, -0.0185746, 0.0 , 0.0187699, 0.00487632},
				{-0.00264636, 0.00041911, 0.0 , 0.0107647, 0.0179561},
				{-0.00842622, -0.000411332, 0.0 , 0.0122931, 0.0104484},
				{-0.00500602, 0.00365391, 0.0 , 0.00845629, 0.0104308},
				{0.00369114, -0.00115415, 0.0 , 0.0084424, 0.0176243},
				{-0.018076, -0.0182971, 0.0 , 0.0109882, 0.00579551},
				{0.00177297, -0.00206545, 0.0 , 0.00920016, 0.00998327},
				{0.00047645, -0.00727859, 0.0 , 0.00264901, 0.0164422},
				{0.0138007, -0.0147898, 0.0 , 0.0120805, 0.0170017},
				{0.0109786, -0.00591448, 0.0 , 0.0199934, 0.00841133},
				{-0.0105802, -0.00639421, 0.0 , 0.0266825, 0.00982489},
				{0.0347409, -0.00691761, 0.0 , -0.00753433, 0.00640576},
				{-0.0169988, -0.00481574, 0.0 , 0.0451113, 0.0142405},
				{0.00960271, 0.0193844, 0.0 , 0.0109586, 0.0081856}		};


				static double fferr_c_f1_2[15][5]    = {{0.00641471, 0.00254115, 0.0 , 0.00282888, 0.00293986, },
				{0.0365052, 0.0108067, 0.0 , 0.0143442, 0.0113363, },
				{0.00382968, 0.0020265, 0.0 , 0.00263497, 0.00251219, },
				{0.00480578, 0.00306091, 0.0 , 0.00399328, 0.00397472, },
				{0.00686401, 0.00462951, 0.0 , 0.00565747, 0.0053819, },
				{0.00775656, 0.00617447, 0.0 , 0.00791032, 0.00620097, },
				{0.0108446, 0.00756743, 0.0 , 0.00944674, 0.00811273, },
				{0.00518142, 0.00336358, 0.0 , 0.00531947, 0.00390064, },
				{0.00634059, 0.004482, 0.0 , 0.00584753, 0.00490345, },
				{0.00976269, 0.00649539, 0.0 , 0.0097156, 0.00919306, },
				{0.0093383, 0.00640718, 0.0 , 0.0107045, 0.00748049, },
				{0.00951918, 0.0080179, 0.0 , 0.0137986, 0.00896775, },
				{0.018353, 0.0104704, 0.0 , 0.0202819, 0.012952, },
				{0.0126276, 0.0115627, 0.0 , 0.0261085, 0.0130777, },
				{0.00860782, 0.00752338, 0.0 , 0.0142694, 0.013026, }		};


				static double ff_c_fside_2[15][5]    = {{0.011715, 0.0348622, 0.0 , 0.0432001, 0.0311946},
				{0.0452981, 0.0124972, 0.0 , 0.0601031, 0.0354572},
				{-0.00196108, 0.0159777, 0.0 , 0.0398524, 0.0136026},
				{-0.00534293, 0.00845167, 0.0 , 0.0366569, 0.013989},
				{-0.0109018, 0.00880212, 0.0 , 0.0257326, 0.0205711},
				{0.0162806, 0.00460494, 0.0 , 0.0346317, 0.0205054},
				{0.00953843, -0.00287235, 0.0 , 0.0302346, 0.0227632},
				{-0.00447661, -0.000304461, 0.0 , 0.0326492, 0.0152594},
				{0.00213353, 0.00704467, 0.0 , 0.036313, 0.0244587},
				{0.0116965, 0.00565162, 0.0 , 0.0515374, 0.0224188},
				{0.0102385, 0.0221342, 0.0 , 0.0284234, 0.02239},
				{0.00424498, 0.0194947, 0.0 , 0.030822, 0.0243359},
				{0.0260855, 0.0172872, 0.0 , 0.037407, 0.0329882},
				{0.00722642, 0.0260985, 0.0 , 0.0572183, 0.0367524},
				{0.0113153, 0.0216102, 0.0 , 0.0371346, 0.0217204}		};


				static double fferr_c_fside_2[15][5]    = {{0.00569592, 0.00366433, 0.0 , 0.00357062, 0.00168098, },
				{0.0161663, 0.0112975, 0.0 , 0.0196104, 0.00501283, },
				{0.00349749, 0.0025999, 0.0 , 0.00416866, 0.00178116, },
				{0.00487447, 0.00379397, 0.0 , 0.00627768, 0.00246956, },
				{0.00707821, 0.00575918, 0.0 , 0.0116302, 0.00379307, },
				{0.00649821, 0.00785693, 0.0 , 0.0112063, 0.00414901, },
				{0.0066585, 0.0100692, 0.0 , 0.0145665, 0.00494868, },
				{0.00537253, 0.00450391, 0.0 , 0.00721389, 0.00325433, },
				{0.0052831, 0.00474372, 0.0 , 0.00877438, 0.00235394, },
				{0.00531822, 0.00810785, 0.0 , 0.0110875, 0.00421358, },
				{0.00453428, 0.00604831, 0.0 , 0.0122301, 0.00504617, },
				{0.00773564, 0.00690537, 0.0 , 0.0145882, 0.00510404, },
				{0.00822147, 0.00842232, 0.0 , 0.0122898, 0.00426479, },
				{0.00471457, 0.0083857, 0.0 , 0.0193428, 0.00778748, },
				{0.00732441, 0.00702407, 0.0 , 0.00981957, 0.00658528, }		};


				static double ff_c_wtot_2[15][5]    = {{0.101794, 0.0630041, 0.0 , 0.114027, 0.0985895},
				{0.139312, 0.090718, 0.0 , 0.114822, 0.0901555},
				{0.0794425, 0.0568368, 0.0 , 0.0891588, 0.0586207},
				{0.0938549, 0.035424, 0.0 , 0.116628, 0.0652831},
				{0.070709, 0.0537314, 0.0 , 0.100024, 0.101862},
				{0.10349, 0.0468297, 0.0 , 0.116405, 0.0746058},
				{0.0726359, 0.0763798, 0.0 , 0.111077, 0.0875564},
				{0.0817552, 0.0655446, 0.0 , 0.113709, 0.0913393},
				{0.0795497, 0.0452924, 0.0 , 0.103624, 0.0780072},
				{0.0956447, 0.0699735, 0.0 , 0.143626, 0.119483},
				{0.0834334, 0.13243, 0.0 , 0.0925939, 0.11751},
				{0.0975579, 0.100871, 0.0 , 0.139713, 0.163974},
				{0.126424, 0.0927033, 0.0 , 0.129371, 0.13882},
				{0.109142, 0.0861337, 0.0 , 0.132405, 0.135559},
				{0.0911722, 0.0781298, 0.0 , 0.143042, 0.0677646}		};


				static double fferr_c_wtot_2[15][5]    = {{0.0311168, 0.0149986, 0.0 , 0.0133245, 0.00822572, },
				{0.0653826, 0.0363188, 0.0 , 0.0634442, 0.0234926, },
				{0.0103999, 0.00713036, 0.0 , 0.00960851, 0.00623714, },
				{0.0139088, 0.0101043, 0.0 , 0.015615, 0.0096754, },
				{0.017804, 0.0133134, 0.0 , 0.0205962, 0.0121633, },
				{0.0204458, 0.0197092, 0.0 , 0.0254941, 0.0158573, },
				{0.0254205, 0.0269573, 0.0 , 0.0339122, 0.0212637, },
				{0.0115735, 0.0113006, 0.0 , 0.0167688, 0.0111774, },
				{0.0144778, 0.0141887, 0.0 , 0.0183137, 0.0137075, },
				{0.0250281, 0.0227677, 0.0 , 0.0379727, 0.0234652, },
				{0.0191699, 0.0234551, 0.0 , 0.0351792, 0.0217797, },
				{0.0162341, 0.0263711, 0.0 , 0.0350468, 0.0301042, },
				{0.0324693, 0.0357352, 0.0 , 0.041506, 0.0380069, },
				{0.021822, 0.0381793, 0.0 , 0.0808082, 0.0497097, },
				{0.0158651, 0.0264171, 0.0 , 0.0377322, 0.0454252, }		};


				static double ff_c_w1_2[15][5]    = {{0.0128839, 0.01839, 0.0 , 0.0130758, 0.0290567},
				{0.0124767, 0.00301945, 0.0 , 0.00475591, 0.0348237},
				{-0.00373882, 0.00550497, 0.0 , 0.00780159, 0.0294747},
				{-0.00545603, 0.00438887, 0.0 , 0.00689518, 0.0244163},
				{0.000517905, 0.0116042, 0.0 , 0.00738806, 0.0228199},
				{0.00608027, 0.00251734, 0.0 , 0.000331819, 0.0293242},
				{-0.00542039, 0.0040254, 0.0 , 0.00672251, 0.0229707},
				{-0.00407773, 0.00543094, 0.0 , 0.00781673, 0.0293118},
				{-0.00284666, 0.00527287, 0.0 , 0.00473022, 0.025208},
				{0.0127997, 0.00531965, 0.0 , 0.00910991, 0.035466},
				{0.0010857, 0.0108997, 0.0 , 0.00417745, 0.0232431},
				{0.00706631, 0.00726277, 0.0 , 0.00759715, 0.0282209},
				{0.0105389, 0.0077405, 0.0 , 0.0100828, 0.029835},
				{-0.00277132, 0.0107045, 0.0 , 0.0287367, 0.0424647},
				{-0.0021525, 0.011613, 0.0 , 0.0198708, 0.0295559}		};


				static double fferr_c_w1_2[15][5]    = {{0.00392463, 0.00206043, 0.0 , 0.00184589, 0.00173818, },
				{0.0137725, 0.00660122, 0.0 , 0.0112147, 0.00694525, },
				{0.00197672, 0.00125125, 0.0 , 0.00172463, 0.00167703, },
				{0.00274839, 0.00175716, 0.0 , 0.00268481, 0.00253391, },
				{0.00381838, 0.00300673, 0.0 , 0.00357755, 0.00378532, },
				{0.0043458, 0.0034661, 0.0 , 0.00425071, 0.0041013, },
				{0.00522925, 0.00455629, 0.0 , 0.00616076, 0.00551564, },
				{0.00268384, 0.00207847, 0.0 , 0.00311052, 0.00265306, },
				{0.00337054, 0.00247961, 0.0 , 0.00338614, 0.00329481, },
				{0.00417568, 0.00368881, 0.0 , 0.00481764, 0.00521601, },
				{0.00407162, 0.00372132, 0.0 , 0.00713989, 0.00523878, },
				{0.00366477, 0.00443648, 0.0 , 0.00677243, 0.00697672, },
				{0.00847327, 0.0063613, 0.0 , 0.00818696, 0.00922037, },
				{0.00507792, 0.00656149, 0.0 , 0.0118262, 0.0082087, },
				{0.00398181, 0.00444002, 0.0 , 0.00628305, 0.00663361, }		};


				static double ff_c_deltae_2[15][5]    = {{-1.81888, -1.88094, 0.0 , -4.09099, -2.1477},
				{2.76943, 0.448654, 0.0 , -6.57757, -1.0393},
				{-1.97039, -2.42941, 0.0 , -2.90784, -5.02063},
				{1.60711, -2.5637, 0.0 , -0.243603, -2.76869},
				{-0.917854, 0.234262, 0.0 , -1.1311, -4.61397},
				{-0.81616, -0.0150146, 0.0 , -0.836857, -1.92984},
				{-2.43999, 0.863594, 0.0 , 2.92246, 0.0969467},
				{-1.60837, -3.03268, 0.0 , -1.61103, -2.02482},
				{-2.47035, -0.36503, 0.0 , 1.28588, -1.75607},
				{-5.94399, -2.16879, 0.0 , -0.591881, -1.52723},
				{2.29242, -2.4861, 0.0 , -2.21316, -2.56436},
				{-2.85843, -0.563078, 0.0 , -10.4005, 1.4691},
				{-2.08364, -8.22071, 0.0 , -18.3064, -0.976725},
				{1.75637, -0.747169, 0.0 , 12.3404, -6.44122},
				{0.848095, -1.90577, 0.0 , 7.94258, -9.50037}		};


				static double fferr_c_deltae_2[15][5]    = {{1.01103, 0.481505, 0.0 , 1.04018, 0.623983, },
				{5.75925, 2.20257, 0.0 , 9.5144, 3.47138, },
				{0.876233, 0.600436, 0.0 , 1.45255, 0.876844, },
				{1.05802, 0.935355, 0.0 , 2.06074, 1.21111, },
				{1.83047, 1.18547, 0.0 , 3.11125, 1.98558, },
				{1.96391, 1.52913, 0.0 , 3.09934, 2.07792, },
				{2.70863, 2.3966, 0.0 , 3.68214, 3.16779, },
				{1.52152, 1.01963, 0.0 , 2.73536, 1.55392, },
				{1.60559, 1.16003, 0.0 , 2.51163, 2.01282, },
				{3.08029, 2.30334, 0.0 , 6.01514, 4.26437, },
				{2.34333, 1.90199, 0.0 , 5.0011, 3.49883, },
				{3.30546, 2.3583, 0.0 , 5.441, 4.18469, },
				{5.66516, 5.75151, 0.0 , 17.1415, 4.38168, },
				{5.24577, 3.88519, 0.0 , 6.28921, 7.0114, },
				{3.07996, 2.82507, 0.0 , 5.25998, 5.985, }		};


				static double ff_c_eratio_2[15][5]    = {{-0.000355899, -0.00103414, 0.0 , 0.00490224, -0.00109965},
				{-0.0187889, -0.00994527, 0.0 , -0.00838834, -0.00241721},
				{0.00505227, 0.00512356, 0.0 , 0.00058496, 0.000849724},
				{0.00107801, 0.00267011, 0.0 , -0.00515032, -0.000794649},
				{0.00494754, 0.00288647, 0.0 , -0.0100142, -0.00163174},
				{0.00209641, 0.00438637, 0.0 , -0.00990254, -0.000645697},
				{-0.000155926, -0.00282419, 0.0 , -0.00763887, -0.00212389},
				{0.000220239, 0.00240636, 0.0 , -0.0046851, -0.00121319},
				{-0.000710607, -0.00162011, 0.0 , -0.00208092, -0.00084269},
				{-0.00154442, -0.00246942, 0.0 , -0.00605094, 0.000157297},
				{-0.000988007, -0.00117594, 0.0 , -0.00389701, -0.00230467},
				{0.00252253, -0.00197375, 0.0 , -0.00100315, -0.00122637},
				{8.75592e-05, 0.00565112, 0.0 , -0.00351161, -0.00316864},
				{-0.00363231, -0.00675505, 0.0 , -0.00446445, -0.00243378},
				{-0.000851154, -0.00106966, 0.0 , -0.00162917, 0.00155419}		};


				static double fferr_c_eratio_2[15][5]    = {{0.0029712, 0.00233533, 0.0 , 0.0027811, 0.00121369, },
				{0.00553559, 0.0110816, 0.0 , 0.00746419, 0.00209764, },
				{0.00277298, 0.002015, 0.0 , 0.00274515, 0.000495977, },
				{0.00334757, 0.00276333, 0.0 , 0.0030432, 0.000523275, },
				{0.00506967, 0.00403813, 0.0 , 0.00270302, 0.0007198, },
				{0.00380149, 0.00399238, 0.0 , 0.00248614, 0.00100591, },
				{0.00480476, 0.00305317, 0.0 , 0.00192018, 0.000869132, },
				{0.001699, 0.00222807, 0.0 , 0.00150068, 0.000585757, },
				{0.00221578, 0.00243802, 0.0 , 0.00167884, 0.000558995, },
				{0.00152694, 0.00295641, 0.0 , 0.00154555, 0.000890959, },
				{0.00186692, 0.00280405, 0.0 , 0.00167614, 0.000681605, },
				{0.00475804, 0.00243962, 0.0 , 0.00238835, 0.000884504, },
				{0.00204955, 0.00799851, 0.0 , 0.00226873, 0.00158972, },
				{0.00209197, 0.00299005, 0.0 , 0.00131764, 0.0019273, },
				{0.00170398, 0.00162866, 0.0 , 0.00113184, 0.00100516, }		};


				//unconverted photons

				static double ff_u_rhad1_2[15][5]    = {{-0.0034745, -0.00235099, 0.0 , -0.000155916, -0.000737883},
				{0.000744989, -0.00110784, 0.0 , -0.000556218, 0.00254812},
				{-0.000564042, -0.000485175, 0.0 , -0.000188405, -0.000521},
				{-0.000569941, 0.00011893, 0.0 , -0.00149687, -0.000730097},
				{-0.000341317, -0.000545282, 0.0 , -0.00139224, -0.000389023},
				{-0.000318478, -0.000568973, 0.0 , 0.000856707, -0.000390534},
				{-0.000411879, 0.000213057, 0.0 , 9.53348e-06, -0.000898713},
				{-0.000493028, -6.81262e-05, 0.0 , -0.000145628, -0.000108469},
				{-0.00032398, -0.000576657, 0.0 , 0.000773878, -0.000120155},
				{-0.000295906, -0.000212512, 0.0 , -0.00149979, -0.000379009},
				{-0.00029821, -0.000264573, 0.0 , -0.000388181, 0.000173547},
				{6.31384e-05, 0.000119036, 0.0 , -8.76216e-05, -0.000455548},
				{-0.000633782, -1.18738e-05, 0.0 , 0.000197221, 3.18905e-05},
				{-0.00047793, -0.000129307, 0.0 , -0.00109484, -0.000167764},
				{-0.0004274, 0.000127285, 0.0 , -0.000293432, -0.00025635}		};


				static double fferr_u_rhad1_2[15][5]    = {{0.000335328, 0.000525684, 0.0 , 0.000789336, 0.000428592, },
				{0.000651199, 0.000997868, 0.0 , 0.00168433, 0.000837623, },
				{0.000129737, 0.00018943, 0.0 , 0.000375763, 0.000195553, },
				{0.000181414, 0.000183928, 0.0 , 0.000560332, 0.000310391, },
				{0.000199884, 0.000328912, 0.0 , 0.000681844, 0.000360211, },
				{0.000221769, 0.000496635, 0.0 , 0.000831241, 0.000337506, },
				{0.000311155, 0.000270231, 0.0 , 0.000810708, 0.00066463, },
				{0.000154656, 0.000179112, 0.0 , 0.000454989, 0.000198378, },
				{0.000188669, 0.00037057, 0.0 , 0.000288565, 0.000222832, },
				{0.000191815, 0.000199762, 0.0 , 0.000754792, 0.00033256, },
				{0.000189556, 0.000170938, 0.0 , 0.000534543, 0.000262468, },
				{0.000143883, 0.000126214, 0.0 , 0.000749023, 0.000361342, },
				{0.000382597, 0.000163364, 0.0 , 0.000669588, 0.00031807, },
				{0.000277102, 0.000415423, 0.0 , 0.0011741, 0.000350805, },
				{0.00016787, 0.000136082, 0.0 , 0.000476111, 0.000290159, }		};


				static double ff_u_rhad_2[15][5]    = {{0, -0.00276564, 0.0 , 0, 0},
				{0, -0.00163934, 0.0 , 0, 0},
				{0, -0.000121347, 0.0 , 0, 0},
				{0, -0.00046459, 0.0 , 0, 0},
				{0, -0.000452925, 0.0 , 0, 0},
				{0, -0.000471185, 0.0 , 0, 0},
				{0, -0.000465611, 0.0 , 0, 0},
				{0, -5.76166e-05, 0.0 , 0, 0},
				{0, -0.0001852, 0.0 , 0, 0},
				{0, 0.000279547, 0.0 , 0, 0},
				{0, 1.44355e-05, 0.0 , 0, 0},
				{0, 0.000102757, 0.0 , 0, 0},
				{0, -8.34829e-05, 0.0 , 0, 0},
				{0, -2.23047e-05, 0.0 , 0, 0},
				{0, 5.36031e-06, 0.0 , 0, 0}		};


				static double fferr_u_rhad_2[15][5]    = {{0, 0.000402161, 0.0 , 0, 0, },
				{0, 0.000935684, 0.0 , 0, 0, },
				{0, 0.000135772, 0.0 , 0, 0, },
				{0, 0.000227771, 0.0 , 0, 0, },
				{0, 0.000196654, 0.0 , 0, 0, },
				{0, 0.00031721, 0.0 , 0, 0, },
				{0, 0.00046739, 0.0 , 0, 0, },
				{0, 0.000128714, 0.0 , 0, 0, },
				{0, 0.000188175, 0.0 , 0, 0, },
				{0, 0.000138272, 0.0 , 0, 0, },
				{0, 9.41798e-05, 0.0 , 0, 0, },
				{0, 8.06156e-05, 0.0 , 0, 0, },
				{0, 0.000113067, 0.0 , 0, 0, },
				{0, 0.00011779, 0.0 , 0, 0, },
				{0, 8.46184e-05, 0.0 , 0, 0, }		};


				static double ff_u_e277_2[15][5]    = {{227.592, 414.993, 0.0 , 911.721, 857.795},
				{-632.365, 861.805, 0.0 , 2141.69, 103.984},
				{308.437, 944.891, 0.0 , 2188.09, 1029.62},
				{322.912, 809.514, 0.0 , -682.496, 1541.77},
				{479.773, 1013.96, 0.0 , 2071.81, 1587.62},
				{543.051, 1527.09, 0.0 , 2921.76, 410.578},
				{125.781, 974.574, 0.0 , -5613.84, -1156.45},
				{42.2227, 1071.64, 0.0 , -1689.87, -1669.39},
				{-52.6211, 1080.58, 0.0 , -2308.45, 619.828},
				{631.414, 2156.22, 0.0 , -5545.95, 1189.97},
				{304.5, 4088.16, 0.0 , -2192.92, -3734},
				{-2381.29, 265.109, 0.0 , 4299.34, -11172.7},
				{494.016, -345.453, 0.0 , 8891.27, 0},
				{-3791.23, -9182.95, 0.0 , -42122, 0},
				{-29389.6, -10264, 0.0 , 0, 0}		};


				static double fferr_u_e277_2[15][5]    = {{6.65367, 11.3252, 0.0 , 40.2963, 45.3708, },
				{199.029, 375.756, 0.0 , 1112.47, 1094.89, },
				{38.0255, 76.4908, 0.0 , 311.854, 256.863, },
				{75.8654, 167.285, 0.0 , 662.5, 508.601, },
				{133.115, 298.647, 0.0 , 1199.68, 902.004, },
				{230.475, 484.216, 0.0 , 1968.63, 1533.76, },
				{348.094, 732.704, 0.0 , 2792.47, 2359.26, },
				{193.797, 410.961, 0.0 , 1603.23, 1495.13, },
				{348.052, 785.225, 0.0 , 2778.72, 2267.85, },
				{612.147, 1446.24, 0.0 , 5542.53, 3325.92, },
				{752.668, 1735.71, 0.0 , 5674.5, 3773.23, },
				{1205.1, 2258.88, 0.0 , 9825.63, 2622.9, },
				{1923.33, 4821.88, 0.0 , 18405.5, 0, },
				{2686.26, 5773.32, 0.0 , 10145.9, 0, },
				{3436.41, 4085.72, 0.0 , 0, 0, }		};


				static double ff_u_reta_2[15][5]    = {{-0.00265682, -0.00500673, 0.0 , -0.00856048, -0.00924462},
				{-0.00552571, -0.00892889, 0.0 , -0.0140256, -0.00918919},
				{-0.00265956, -0.00557208, 0.0 , -0.00954425, -0.00874162},
				{-0.00362194, -0.00612283, 0.0 , -0.0102331, -0.010027},
				{-0.00294858, -0.00626135, 0.0 , -0.0100549, -0.00989372},
				{-0.00378758, -0.00678074, 0.0 , -0.00996017, -0.00870353},
				{-0.00417548, -0.00650567, 0.0 , -0.0128258, -0.0105776},
				{-0.00481367, -0.00669736, 0.0 , -0.00982803, -0.0103061},
				{-0.00402445, -0.00649196, 0.0 , -0.0107806, -0.0106706},
				{-0.0049457, -0.00667453, 0.0 , -0.0121983, -0.0113106},
				{-0.00471473, -0.0067367, 0.0 , -0.00961292, -0.0112275},
				{-0.00394118, -0.00751072, 0.0 , -0.0124717, -0.0124845},
				{-0.00520647, -0.00738502, 0.0 , -0.00803238, -0.0104172},
				{-0.00420606, -0.00785506, 0.0 , -0.0100555, -0.0108979},
				{-0.00416273, -0.00849926, 0.0 , -0.0097068, -0.0122566}		};


				static double fferr_u_reta_2[15][5]    = {{0.000755651, 0.00062498, 0.0 , 0.000898872, 0.00045709, },
				{0.00144258, 0.00185458, 0.0 , 0.00271381, 0.00134411, },
				{0.00039888, 0.000355667, 0.0 , 0.000512671, 0.000416835, },
				{0.000508962, 0.000533487, 0.0 , 0.000680519, 0.000528733, },
				{0.00078582, 0.00068499, 0.0 , 0.000841305, 0.000700729, },
				{0.00099887, 0.000797799, 0.0 , 0.00104947, 0.000879708, },
				{0.000793602, 0.000877833, 0.0 , 0.000947253, 0.00124285, },
				{0.000362034, 0.00036189, 0.0 , 0.000653051, 0.000482854, },
				{0.00044163, 0.000404609, 0.0 , 0.000730276, 0.000588504, },
				{0.000476558, 0.00051666, 0.0 , 0.00111887, 0.000957636, },
				{0.000482184, 0.000577246, 0.0 , 0.000978864, 0.00104797, },
				{0.000423521, 0.000478559, 0.0 , 0.000984212, 0.00107833, },
				{0.000635204, 0.00058847, 0.0 , 0.00226727, 0.00147723, },
				{0.000581201, 0.000825438, 0.0 , 0.0012719, 0.00165942, },
				{0.000407231, 0.000563672, 0.0 , 0.00120967, 0.00141991, }		};


				static double ff_u_rphi_2[15][5]    = {{-0.000305474, -0.00322962, 0.0 , -0.00395846, -0.00456816},
				{-0.00685787, -0.00854397, 0.0 , -0.00804943, -0.00976884},
				{-0.00229454, -0.00269032, 0.0 , -0.00319237, -0.00546974},
				{-0.00089407, -0.00342721, 0.0 , -0.00329864, -0.00725633},
				{-0.00258601, -0.00437403, 0.0 , -0.00493705, -0.00751364},
				{-0.00112391, -0.0052582, 0.0 , -0.00499374, -0.00637454},
				{-0.00315624, -0.00441563, 0.0 , -0.00615793, -0.00464082},
				{-0.00384063, -0.00394541, 0.0 , -0.00155443, -0.00625104},
				{-0.00346184, -0.004641, 0.0 , -0.004807, -0.00647062},
				{-0.000913143, -0.0056088, 0.0 , -0.00701874, -0.00650942},
				{-0.0029633, -0.00547069, 0.0 , -0.0043481, -0.00602138},
				{-0.00236446, -0.00430584, 0.0 , -0.00214875, -0.00762415},
				{-0.00290304, -0.00553507, 0.0 , -0.00148606, -0.00460804},
				{-0.00322992, -0.00409353, 0.0 , -0.00644088, -0.0065648},
				{-0.00394791, -0.0054211, 0.0 , -0.00453937, -0.00842947}		};


				static double fferr_u_rphi_2[15][5]    = {{0.000834532, 0.000843749, 0.0 , 0.00107971, 0.000584992, },
				{0.0013894, 0.00177633, 0.0 , 0.0023022, 0.00103266, },
				{0.000469983, 0.00054774, 0.0 , 0.00070035, 0.000603326, },
				{0.000842643, 0.000780156, 0.0 , 0.00118124, 0.000696193, },
				{0.000853624, 0.000811456, 0.0 , 0.00123909, 0.000609137, },
				{0.00174146, 0.000905765, 0.0 , 0.00131603, 0.000842795, },
				{0.000889942, 0.00137637, 0.0 , 0.00161497, 0.00174034, },
				{0.000363788, 0.000529357, 0.0 , 0.00114749, 0.000562812, },
				{0.000343189, 0.000614778, 0.0 , 0.00089997, 0.000656452, },
				{0.0014213, 0.000623836, 0.0 , 0.00130635, 0.000777536, },
				{0.000444916, 0.000737514, 0.0 , 0.000991732, 0.00137676, },
				{0.00047179, 0.000703012, 0.0 , 0.00292497, 0.00120422, },
				{0.000610828, 0.000889913, 0.0 , 0.00272047, 0.00152956, },
				{0.000510977, 0.000950054, 0.0 , 0.00124813, 0.00130969, },
				{0.00047476, 0.000593746, 0.0 , 0.00116724, 0.00135768, }		};


				static double ff_u_weta2_2[15][5]    = {{0.000294228, 0.000341086, 0.0 , 0.000401815, 0.000346575},
				{0.000182481, 0.000273285, 0.0 , 0.000585307, 0.000285761},
				{0.000224443, 0.000278756, 0.0 , 0.000524672, 0.000374963},
				{0.000200711, 0.000279236, 0.0 , 0.000530393, 0.000455781},
				{0.000206227, 0.000244321, 0.0 , 0.00055398, 0.000396494},
				{0.000268413, 0.000326672, 0.0 , 0.000587464, 0.000325048},
				{0.0002162, 0.000308248, 0.0 , 0.000667414, 0.000465885},
				{0.000236388, 0.000260386, 0.0 , 0.000564201, 0.0003841},
				{0.000256922, 0.000306592, 0.0 , 0.000562698, 0.00045967},
				{0.000199503, 0.000300667, 0.0 , 0.000734823, 0.000454972},
				{0.000244807, 0.000326708, 0.0 , 0.000591032, 0.000420212},
				{0.000200487, 0.000264524, 0.0 , 0.000723513, 0.000654024},
				{0.000188447, 0.000320415, 0.0 , 0.000422063, 0.000454101},
				{0.000239731, 0.00031675, 0.0 , 0.000660912, 0.000474905},
				{0.000308163, 0.000370061, 0.0 , 0.000584001, 0.0005279}		};


				static double fferr_u_weta2_2[15][5]    = {{3.24328e-05, 2.35965e-05, 0.0 , 3.14602e-05, 1.62948e-05, },
				{8.75042e-05, 7.29284e-05, 0.0 , 0.000102783, 7.19725e-05, },
				{1.32316e-05, 1.35892e-05, 0.0 , 2.33253e-05, 2.15936e-05, },
				{1.80649e-05, 1.90049e-05, 0.0 , 3.62549e-05, 3.37127e-05, },
				{2.65071e-05, 3.10375e-05, 0.0 , 5.74273e-05, 4.44632e-05, },
				{3.4228e-05, 3.19607e-05, 0.0 , 7.03254e-05, 5.90065e-05, },
				{3.82561e-05, 3.73143e-05, 0.0 , 7.30379e-05, 8.11102e-05, },
				{1.6224e-05, 1.76216e-05, 0.0 , 3.98902e-05, 3.22666e-05, },
				{1.99363e-05, 2.11997e-05, 0.0 , 5.21967e-05, 4.6058e-05, },
				{3.09212e-05, 2.94502e-05, 0.0 , 6.94342e-05, 7.58436e-05, },
				{2.69905e-05, 2.85047e-05, 0.0 , 7.32069e-05, 8.85064e-05, },
				{2.58185e-05, 2.95367e-05, 0.0 , 8.12773e-05, 8.98749e-05, },
				{4.5466e-05, 4.72529e-05, 0.0 , 9.4331e-05, 0.000109912, },
				{3.66397e-05, 4.74827e-05, 0.0 , 0.000106989, 0.000139112, },
				{2.67139e-05, 3.57317e-05, 0.0 , 6.56102e-05, 0.000112815, }		};


				static double ff_u_f1_2[15][5]    = {{-0.0132794, -0.00923616, 0.0 , -0.0161323, -0.0407871},
				{-0.023862, -0.0347714, 0.0 , 0.01296, -0.0327254},
				{-0.0115819, -0.01094, 0.0 , -0.00555271, -0.0118555},
				{-0.0112605, -0.0116678, 0.0 , 0.000586748, -0.00613321},
				{-0.0106728, -0.00728919, 0.0 , -0.00679323, -0.00867437},
				{-0.013451, -0.00666972, 0.0 , -0.00128117, -0.0114961},
				{-0.00808619, 0.00240634, 0.0 , 0.0407445, 0.00745131},
				{-0.00402491, -0.0100703, 0.0 , -0.000815719, 0.00360702},
				{0.00113527, -0.0146121, 0.0 , 0.00190702, -0.00321773},
				{-0.00370705, -0.0111584, 0.0 , 0.0317247, 0.000742704},
				{-0.00523086, -0.00801635, 0.0 , 0.00309137, 0.00695418},
				{-0.00138007, 0.00133362, 0.0 , 0.0310667, 0.0266611},
				{-0.00310862, -0.00374267, 0.0 , -0.0336001, -0.0171966},
				{-0.00286631, -0.00186023, 0.0 , 0.0257831, 0.012504},
				{0.0143795, 0.0125523, 0.0 , -0.00529353, 0.019855}		};


				static double fferr_u_f1_2[15][5]    = {{0.00265731, 0.00202228, 0.0 , 0.00289171, 0.00225613, },
				{0.0125232, 0.0115334, 0.0 , 0.0166793, 0.0109863, },
				{0.002545, 0.00208591, 0.0 , 0.00369829, 0.00289457, },
				{0.0035516, 0.00326872, 0.0 , 0.00604933, 0.00419218, },
				{0.00501711, 0.00465475, 0.0 , 0.00913379, 0.0058303, },
				{0.00742029, 0.00600509, 0.0 , 0.0118232, 0.00803833, },
				{0.00894437, 0.00749609, 0.0 , 0.0154068, 0.0106351, },
				{0.00368999, 0.00333992, 0.0 , 0.00769132, 0.00469618, },
				{0.00470218, 0.00427995, 0.0 , 0.00858667, 0.00603176, },
				{0.00730249, 0.00649601, 0.0 , 0.0158299, 0.00810659, },
				{0.00682323, 0.00643103, 0.0 , 0.013176, 0.00952799, },
				{0.00688767, 0.00608388, 0.0 , 0.0156892, 0.00799089, },
				{0.00924441, 0.0110844, 0.0 , 0.0238869, 0.0104322, },
				{0.0086091, 0.00988593, 0.0 , 0.0224885, 0.0134254, },
				{0.00599958, 0.00613788, 0.0 , 0.0135026, 0.0116243, }		};


				static double ff_u_fside_2[15][5]    = {{0.0162235, 0.0174589, 0.0 , 0.0268256, 0.0216578},
				{0.00259265, 0.0104385, 0.0 , 0.044022, 0.0305337},
				{-0.00158249, 0.000426412, 0.0 , 0.0182744, 0.00893083},
				{-0.00189069, -0.00188249, 0.0 , 0.0293421, 0.00431734},
				{-0.00348574, -0.00525898, 0.0 , 0.0140821, 0.00833102},
				{-0.00106776, 0.00102168, 0.0 , 0.0267639, 0.0122117},
				{-0.00466348, 0.0116798, 0.0 , 0.0284419, 0.0114806},
				{-0.00659414, -0.000160635, 0.0 , 0.0236876, 0.0170697},
				{0.0017134, -0.00450869, 0.0 , 0.0251574, 0.0184861},
				{0.00137067, 0.0152416, 0.0 , 0.0426916, 0.0228228},
				{-0.0044602, 0.0169, 0.0 , 0.0164292, 0.0149521},
				{-0.000266001, 0.00584145, 0.0 , 0.0444934, 0.0246992},
				{-0.000818029, 0.0200502, 0.0 , 0.0204745, 0.0116238},
				{0.00118721, 0.00483049, 0.0 , 0.0420924, 0.0251576},
				{0.00255132, 0.014254, 0.0 , 0.0228496, 0.0166221}		};


				static double fferr_u_fside_2[15][5]    = {{0.00158194, 0.00177838, 0.0 , 0.00236796, 0.000905116, },
				{0.00589121, 0.0103015, 0.0 , 0.0132274, 0.0030131, },
				{0.00168341, 0.00199811, 0.0 , 0.0034862, 0.00172027, },
				{0.00227633, 0.0031548, 0.0 , 0.00485317, 0.0028754, },
				{0.00356508, 0.004808, 0.0 , 0.00780807, 0.00398273, },
				{0.00466689, 0.00573452, 0.0 , 0.00970827, 0.00550088, },
				{0.00743962, 0.007637, 0.0 , 0.0138784, 0.00680776, },
				{0.00298645, 0.00323838, 0.0 , 0.00675791, 0.0026036, },
				{0.00280129, 0.00426386, 0.0 , 0.00706541, 0.00247708, },
				{0.0045405, 0.00367813, 0.0 , 0.0102116, 0.00313325, },
				{0.00524827, 0.00408013, 0.0 , 0.0142603, 0.00330306, },
				{0.00482272, 0.00407859, 0.0 , 0.00914537, 0.00377039, },
				{0.0059666, 0.00567073, 0.0 , 0.0153157, 0.00785126, },
				{0.00553861, 0.00639028, 0.0 , 0.0106543, 0.00613521, },
				{0.00286525, 0.00448804, 0.0 , 0.00685385, 0.00477706, }		};


				static double ff_u_wtot_2[15][5]    = {{0.0498329, 0.0771685, 0.0 , 0.0843751, 0.0743252},
				{0.131487, 0.0721399, 0.0 , 0.24562, 0.110856},
				{0.0670028, 0.0541846, 0.0 , 0.101668, 0.0682777},
				{0.0411838, 0.058804, 0.0 , 0.114743, 0.0689265},
				{0.0581766, 0.0537953, 0.0 , 0.0805992, 0.0635936},
				{0.0615263, 0.0775042, 0.0 , 0.103154, 0.0951279},
				{0.081843, 0.0903391, 0.0 , 0.128242, 0.0840613},
				{0.0645174, 0.0610784, 0.0 , 0.109357, 0.104109},
				{0.0737532, 0.0779748, 0.0 , 0.126693, 0.0884707},
				{0.0633897, 0.0907177, 0.0 , 0.115411, 0.0942065},
				{0.0779537, 0.101344, 0.0 , 0.163666, 0.0850394},
				{0.0329025, 0.074824, 0.0 , 0.1487, 0.0790747},
				{0.0668129, 0.043818, 0.0 , 0.137709, 0.047681},
				{0.0775421, 0.0804719, 0.0 , 0.182331, 0.10549},
				{0.0290289, 0.0671611, 0.0 , 0.146889, 0.0904267}		};


				static double fferr_u_wtot_2[15][5]    = {{0.0109561, 0.00872764, 0.0 , 0.0118836, 0.0055089, },
				{0.0357304, 0.0367431, 0.0 , 0.0582209, 0.0212888, },
				{0.00767306, 0.0066856, 0.0 , 0.0109103, 0.00597256, },
				{0.00992732, 0.00971953, 0.0 , 0.0162325, 0.00936395, },
				{0.0129004, 0.0128827, 0.0 , 0.024323, 0.0126466, },
				{0.0182705, 0.0189983, 0.0 , 0.0347042, 0.0158032, },
				{0.0232735, 0.0218319, 0.0 , 0.0413642, 0.0182958, },
				{0.00859568, 0.00974597, 0.0 , 0.0198864, 0.00999413, },
				{0.012094, 0.0133886, 0.0 , 0.0222025, 0.0131894, },
				{0.0167023, 0.0172322, 0.0 , 0.0346115, 0.0253158, },
				{0.0158597, 0.0178011, 0.0 , 0.0288662, 0.0253668, },
				{0.0171742, 0.0190314, 0.0 , 0.0404626, 0.025257, },
				{0.0224247, 0.0283379, 0.0 , 0.0445854, 0.0385841, },
				{0.0207507, 0.0288053, 0.0 , 0.0351135, 0.0562322, },
				{0.015741, 0.0208077, 0.0 , 0.0325392, 0.0434768, }		};


				static double ff_u_w1_2[15][5]    = {{0.0123816, 0.0141711, 0.0 , 0.0126642, 0.0288762},
				{-6.17504e-05, 0.000859797, 0.0 , 0.0243251, 0.0281727},
				{-0.00586516, 0.00159836, 0.0 , 0.000966907, 0.0181982},
				{-0.00610119, 0.000533462, 0.0 , 0.00881076, 0.0211331},
				{-0.00812262, 0.00265533, 0.0 , 0.00285578, 0.0161532},
				{-0.00712097, 0.00480664, 0.0 , 0.00572783, 0.0202317},
				{-0.00427246, 0.00967562, 0.0 , 0.0162632, 0.0305361},
				{-0.00407213, 0.00123149, 0.0 , 0.00291854, 0.0187593},
				{-0.00376856, 0.0019235, 0.0 , 0.00921321, 0.0209616},
				{-0.00425434, 0.00787604, 0.0 , 0.0116231, 0.0211454},
				{-0.0100777, 0.0100716, 0.0 , 0.0106937, 0.0239205},
				{-0.0047316, 0.00629479, 0.0 , 0.0235783, 0.0231984},
				{-0.00421047, 0.00743222, 0.0 , -0.00228995, 0.0258575},
				{-0.00431442, 0.00427204, 0.0 , 0.0168156, 0.0373782},
				{0.00028348, 0.0134043, 0.0 , 0.0106578, 0.0333578}		};


				static double fferr_u_w1_2[15][5]    = {{0.00134454, 0.00118668, 0.0 , 0.00165647, 0.00115023, },
				{0.00372668, 0.00542021, 0.0 , 0.00792666, 0.0053436, },
				{0.0011127, 0.00113782, 0.0 , 0.00191753, 0.00157988, },
				{0.00160003, 0.00174713, 0.0 , 0.0030398, 0.00240668, },
				{0.00246778, 0.00258192, 0.0 , 0.00467863, 0.00314111, },
				{0.00319597, 0.00307636, 0.0 , 0.0064771, 0.00455291, },
				{0.00408729, 0.00424628, 0.0 , 0.00715269, 0.00458449, },
				{0.00170872, 0.0017522, 0.0 , 0.00361625, 0.00294014, },
				{0.00226327, 0.00217439, 0.0 , 0.00442407, 0.00283656, },
				{0.00304589, 0.00288688, 0.0 , 0.00817747, 0.00450987, },
				{0.00379235, 0.00307118, 0.0 , 0.00584748, 0.00454187, },
				{0.00363557, 0.00321458, 0.0 , 0.0077272, 0.00543129, },
				{0.00420663, 0.00522646, 0.0 , 0.00903935, 0.00656994, },
				{0.00363573, 0.00516277, 0.0 , 0.00819605, 0.00685457, },
				{0.00283764, 0.00340996, 0.0 , 0.00727795, 0.00828603, }		};


				static double ff_u_deltae_2[15][5]    = {{-1.53641, -1.03752, 0.0 , -1.52409, -4.36474},
				{1.72822, -3.36062, 0.0 , 5.17104, -5.50315},
				{-4.80322, -1.27499, 0.0 , -2.2216, -3.56907},
				{-2.52741, -1.13166, 0.0 , -2.10718, -3.85275},
				{-2.33445, -1.68618, 0.0 , -4.96227, -3.77705},
				{-3.13363, -2.37778, 0.0 , -4.67301, -2.37883},
				{-3.69209, -1.66557, 0.0 , 6.13139, -4.29717},
				{-1.2537, -1.79345, 0.0 , -0.510666, -4.5075},
				{0.77463, -2.23166, 0.0 , 1.15844, 1.61638},
				{0.0201607, -0.401819, 0.0 , 6.84226, 1.06955},
				{-2.54803, -3.42703, 0.0 , -2.61037, 1.61649},
				{0.775471, 0.976572, 0.0 , 3.21291, 1.49463},
				{-3.13636, -2.2345, 0.0 , -2.8115, 3.66275},
				{-3.2891, -5.78311, 0.0 , 6.38987, -2.25544},
				{-3.58897, -1.68037, 0.0 , 1.75497, -0.544491}		};


				static double fferr_u_deltae_2[15][5]    = {{0.359292, 0.256398, 0.0 , 0.703175, 0.534613, },
				{2.73572, 2.84552, 0.0 , 3.94282, 4.30195, },
				{0.801934, 0.46591, 0.0 , 1.10285, 0.897538, },
				{1.06003, 0.694442, 0.0 , 1.75413, 1.40661, },
				{1.31412, 1.01968, 0.0 , 2.85238, 1.87507, },
				{2.04125, 1.46874, 0.0 , 3.64717, 2.40008, },
				{2.2516, 2.04927, 0.0 , 3.3292, 3.62689, },
				{0.895734, 0.773876, 0.0 , 2.2828, 1.80405, },
				{1.02312, 0.92021, 0.0 , 2.5518, 2.11372, },
				{1.35939, 1.41032, 0.0 , 3.00068, 3.70812, },
				{1.69042, 1.67727, 0.0 , 3.98492, 2.49984, },
				{1.60826, 1.37795, 0.0 , 4.10154, 3.05058, },
				{2.18018, 2.60558, 0.0 , 8.18078, 3.66559, },
				{2.19813, 2.99407, 0.0 , 5.64883, 5.278, },
				{1.81521, 1.64123, 0.0 , 4.00211, 4.97346, }		};


				static double ff_u_eratio_2[15][5]    = {{-0.000878811, -0.0059849, 0.0 , -0.00626814, -0.00667316},
				{-0.0133167, -0.0143259, 0.0 , -0.013072, -0.0048452},
				{0.000322163, 0.000208974, 0.0 , -0.00269318, -0.000569522},
				{-0.0011965, -0.00198221, 0.0 , 0.000520408, -0.00257438},
				{-0.00297129, 0.000186622, 0.0 , -0.00777453, -0.00111878},
				{0.00148487, -0.00285226, 0.0 , -0.0064587, -0.00100887},
				{-0.00038898, -0.00736189, 0.0 , 0.00439763, -0.000340104},
				{-0.00313193, -0.00288111, 0.0 , -0.00295377, 0.000566185},
				{-0.000552356, -0.00162637, 0.0 , -0.000569105, -0.00119966},
				{-0.00310206, -0.00449651, 0.0 , 0.00163317, -0.00131685},
				{0.000100076, -0.00579268, 0.0 , -0.00944334, 0.000413954},
				{0.000944674, -0.0042147, 0.0 , -0.00610548, -0.000978589},
				{-0.00329077, -0.000869632, 0.0 , -0.00727892, -0.00372207},
				{-0.00396132, -0.00757492, 0.0 , -0.000184953, -0.000730693},
				{-0.00125211, -0.00492436, 0.0 , -0.00477821, 0.00119066}		};


				static double fferr_u_eratio_2[15][5]    = {{0.0015454, 0.00139504, 0.0 , 0.00196228, 0.0010411, },
				{0.00477065, 0.00611719, 0.0 , 0.00347694, 0.00298577, },
				{0.00118913, 0.00134898, 0.0 , 0.00169325, 0.000757542, },
				{0.00156669, 0.00186665, 0.0 , 0.00309963, 0.000727893, },
				{0.00181119, 0.00255257, 0.0 , 0.001738, 0.00120748, },
				{0.00345781, 0.00241563, 0.0 , 0.0024475, 0.00225711, },
				{0.00331818, 0.00299671, 0.0 , 0.00402597, 0.00170157, },
				{0.00119673, 0.00149163, 0.0 , 0.00246779, 0.00142029, },
				{0.00166592, 0.00229612, 0.0 , 0.00394967, 0.00162946, },
				{0.00176331, 0.0022965, 0.0 , 0.00410003, 0.00126972, },
				{0.00320426, 0.00159449, 0.0 , 0.00164006, 0.00150626, },
				{0.00260371, 0.00213632, 0.0 , 0.00253515, 0.00125011, },
				{0.00316905, 0.0035047, 0.0 , 0.00225433, 0.00128691, },
				{0.00236765, 0.00274149, 0.0 , 0.00340893, 0.00254231, },
				{0.00289893, 0.00264177, 0.0 , 0.00242859, 0.00213439, }		};

				if(isConv){
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad1_2[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad_2[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_e277_2[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_reta_2[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rphi_2[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_weta2_2[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_f1_2[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_fside_2[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_wtot_2[0][0] ,  0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_w1_2[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_deltae_2[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_eratio_2[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad1_2[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad_2[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_e277_2[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_reta_2[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rphi_2[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_weta2_2[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_f1_2[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_fside_2[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_wtot_2[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_w1_2[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_deltae_2[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_eratio_2[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad1_2[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad_2[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_e277_2[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_reta_2[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rphi_2[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_weta2_2[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_f1_2[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_fside_2[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_wtot_2[0][0],   0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_w1_2[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_deltae_2[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_eratio_2[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad1_2[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad_2[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_e277_2[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_reta_2[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rphi_2[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_weta2_2[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_f1_2[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_fside_2[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_wtot_2[0][0],   0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_w1_2[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_deltae_2[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_eratio_2[0][0], 0. );
				}

				break;

		case 3:  // RobustTightY+Iso preselection

				//converted photons

				static double ff_c_rhad1_3[15][5]    = {{-0.00201901, -0.00313722, 0.0 , -0.00296045, -0.000667818},
				{-0.00194984, 0.00073684, 0.0 , 0.000114076, -0.000551676},
				{-0.000735819, -0.00103701, 0.0 , -0.000403063, -0.00112865},
				{-0.000205322, -0.000577202, 0.0 , 0.00029276, -0.000669067},
				{-2.01465e-05, -3.32503e-05, 0.0 , 0.000226236, -0.000515795},
				{-0.000242492, 0.000636269, 0.0 , 2.44312e-05, -4.17088e-05},
				{-0.000768584, -0.000491206, 0.0 , -0.00186764, 0.00067537},
				{-0.000245252, -0.000177353, 0.0 , -0.000531317, -0.000276207},
				{-0.00035814, -2.25012e-05, 0.0 , 0.000344019, 0.000143763},
				{-0.00042429, -0.000571946, 0.0 , -3.83843e-05, 5.27865e-05},
				{-1.32549e-05, 0.000183842, 0.0 , 0.000288124, 3.70078e-05},
				{0.000192393, -0.000189087, 0.0 , 0.000198758, -2.05191e-05},
				{0.000581023, -0.000160059, 0.0 , -0.000881137, 8.0988e-05},
				{-0.00031188, 0.000213898, 0.0 , 0.000679157, -1.82621e-06},
				{-0.000387427, -0.000109134, 0.0 , -0.000376867, 4.52041e-07}		};


				static double fferr_c_rhad1_3[15][5]    = {{0.00103176, 0.00163946, 0.0 , 0.000791676, 0.000635162, },
				{0.00183577, 0.000896634, 0.0 , 0.00185998, 0.00121603, },
				{0.00024689, 0.00043341, 0.0 , 0.000371571, 0.000233989, },
				{0.00028676, 0.000353547, 0.0 , 0.000496861, 0.000302753, },
				{0.000340068, 0.000359242, 0.0 , 0.000693998, 0.000329259, },
				{0.000565694, 0.000328729, 0.0 , 0.00080998, 0.000464987, },
				{0.00026204, 0.000373919, 0.0 , 0.0011114, 0.000602772, },
				{0.000179762, 0.000188531, 0.0 , 0.00035961, 0.000186171, },
				{0.000221073, 0.000200906, 0.0 , 0.00038286, 0.00021588, },
				{0.000382053, 0.000624342, 0.0 , 0.000768646, 0.000337005, },
				{0.000172699, 0.000178854, 0.0 , 0.000389863, 0.000207309, },
				{0.00018305, 0.000235443, 0.0 , 0.000375293, 0.000261447, },
				{0.000172714, 0.000251359, 0.0 , 0.000970002, 0.000315525, },
				{0.000279577, 0.000167228, 0.0 , 0.000549967, 0.000292164, },
				{0.000150386, 7.27626e-05, 0.0 , 0.000296482, 0.000213175, }		};


				static double ff_c_rhad_3[15][5]    = {{0, -0.00267811, 0.0 , 0, 0},
				{0, 0.000597254, 0.0 , 0, 0},
				{0, -0.000320961, 0.0 , 0, 0},
				{0, -0.000704423, 0.0 , 0, 0},
				{0, -2.06614e-06, 0.0 , 0, 0},
				{0, -0.000114451, 0.0 , 0, 0},
				{0, -0.000322536, 0.0 , 0, 0},
				{0, -0.000401715, 0.0 , 0, 0},
				{0, -0.00046355, 0.0 , 0, 0},
				{0, 0.000255787, 0.0 , 0, 0},
				{0, -0.000272332, 0.0 , 0, 0},
				{0, -9.40651e-05, 0.0 , 0, 0},
				{0, -0.000103012, 0.0 , 0, 0},
				{0, -0.000152187, 0.0 , 0, 0},
				{0, -5.4725e-06, 0.0 , 0, 0}		};


				static double fferr_c_rhad_3[15][5]    = {{0, 0.00051383, 0.0 , 0, 0, },
				{0, 0.00121605, 0.0 , 0, 0, },
				{0, 0.000145232, 0.0 , 0, 0, },
				{0, 0.000251349, 0.0 , 0, 0, },
				{0, 0.000237684, 0.0 , 0, 0, },
				{0, 0.000266255, 0.0 , 0, 0, },
				{0, 0.000419039, 0.0 , 0, 0, },
				{0, 0.000183116, 0.0 , 0, 0, },
				{0, 0.000186425, 0.0 , 0, 0, },
				{0, 0.000173638, 0.0 , 0, 0, },
				{0, 0.000202543, 0.0 , 0, 0, },
				{0, 0.000118862, 0.0 , 0, 0, },
				{0, 0.000132471, 0.0 , 0, 0, },
				{0, 0.00010412, 0.0 , 0, 0, },
				{0, 5.54677e-05, 0.0 , 0, 0, }		};


				static double ff_c_e277_3[15][5]    = {{292.812, 552.564, 0.0 , 1858.92, 1315.15},
				{-1108.89, 606.417, 0.0 , -1355.12, -1395.03},
				{-60.1387, 226.484, 0.0 , 240.883, -201.125},
				{-84.8359, 506.275, 0.0 , -567.82, 322.086},
				{-76.1035, -191.895, 0.0 , 362.145, -376.359},
				{-30.9648, 610.906, 0.0 , 251.418, -495.133},
				{864.992, 1164.83, 0.0 , -386.867, -1029.27},
				{-124.102, 698.242, 0.0 , -2905.01, -1040.05},
				{54.9102, 1913.3, 0.0 , -2607.66, -1012},
				{-1746.42, 2017.86, 0.0 , -6622.99, -9247.95},
				{-1428.04, 2427.29, 0.0 , -4521.28, -4895.81},
				{755.391, -567.312, 0.0 , -7029.61, -12607.9},
				{-4864.26, 1559.73, 0.0 , 20675.2, 289920},
				{6016.52, -3632.5, 0.0 , 23753.9, 284149},
				{-34850.1, -10252.1, 0.0 , 11963.5, 0}		};


				static double fferr_c_e277_3[15][5]    = {{16.9937, 22.1776, 0.0 , 59.3221, 64.9376, },
				{348.757, 367.133, 0.0 , 1119.23, 1203.96, },
				{56.5056, 74.9581, 0.0 , 230.369, 210.4, },
				{102.748, 153.431, 0.0 , 456.265, 423.036, },
				{179.555, 288.185, 0.0 , 912.761, 798.338, },
				{288.933, 438.624, 0.0 , 1563.95, 1309.21, },
				{407.74, 710.795, 0.0 , 2008.11, 1938.43, },
				{264.667, 379.632, 0.0 , 1443.77, 1188.6, },
				{421.581, 648.883, 0.0 , 2096.11, 2012.13, },
				{993.122, 1286.71, 0.0 , 4284.7, 3497.84, },
				{1141.75, 1679.72, 0.0 , 6216.64, 3238.83, },
				{1259.68, 2732.66, 0.0 , 9649.86, 4944.16, },
				{2594.43, 4398, 0.0 , 11945.3, 2285.51, },
				{3220.26, 6675.58, 0.0 , 13576.2, 0, },
				{5177.39, 5509.35, 0.0 , 14232.4, 0, }		};


				static double ff_c_reta_3[15][5]    = {{-0.0059253, -0.00738239, 0.0 , -0.00852323, -0.00941408},
				{-0.0038566, -0.0101901, 0.0 , -0.0214379, -0.0127313},
				{-0.00216854, -0.00770164, 0.0 , -0.0107648, -0.0120016},
				{-0.00257552, -0.00718701, 0.0 , -0.0121624, -0.0109183},
				{-0.00331724, -0.00677764, 0.0 , -0.00998324, -0.0109763},
				{-0.00526696, -0.00689816, 0.0 , -0.0109993, -0.0127786},
				{-0.00377142, -0.0054276, 0.0 , -0.0117297, -0.00895828},
				{-0.00531274, -0.0078916, 0.0 , -0.0123313, -0.0107001},
				{-0.00601989, -0.00704157, 0.0 , -0.0115697, -0.0115898},
				{-0.00540519, -0.00596291, 0.0 , -0.0127543, -0.0100822},
				{-0.00567311, -0.00757915, 0.0 , -0.0101377, -0.00979304},
				{-0.00502104, -0.00720555, 0.0 , -0.0125296, -0.00843465},
				{-0.00542802, -0.00866467, 0.0 , -0.00962681, -0.0108092},
				{-0.00526196, -0.00694442, 0.0 , -0.0131562, -0.011188},
				{-0.00471318, -0.00807583, 0.0 , -0.0103003, -0.0108964}		};


				static double fferr_c_reta_3[15][5]    = {{0.00228579, 0.00105506, 0.0 , 0.00100925, 0.000705251, },
				{0.00435857, 0.00257463, 0.0 , 0.00252165, 0.0018241, },
				{0.00087728, 0.000455293, 0.0 , 0.000709818, 0.000454162, },
				{0.00108313, 0.000679561, 0.0 , 0.000808243, 0.000666288, },
				{0.00136122, 0.000951614, 0.0 , 0.00150308, 0.000906953, },
				{0.00151112, 0.00108122, 0.0 , 0.00139025, 0.000972848, },
				{0.0019053, 0.00139003, 0.0 , 0.00179291, 0.00165383, },
				{0.000560751, 0.000474174, 0.0 , 0.000693575, 0.000641207, },
				{0.000640923, 0.000606988, 0.0 , 0.000971534, 0.00069036, },
				{0.000981398, 0.00096407, 0.0 , 0.00112514, 0.00123903, },
				{0.000780771, 0.000682797, 0.0 , 0.00107294, 0.00108724, },
				{0.00095655, 0.00132078, 0.0 , 0.0012792, 0.00140181, },
				{0.00120022, 0.000772366, 0.0 , 0.00167379, 0.00197536, },
				{0.000888741, 0.000917726, 0.0 , 0.00182499, 0.00188752, },
				{0.000659408, 0.000793174, 0.0 , 0.00119454, 0.00159891, }		};


				static double ff_c_rphi_3[15][5]    = {{-0.0121642, -0.0133714, 0.0 , -0.00845546, -0.00878078},
				{0.017596, -0.0106719, 0.0 , 0.00174105, -0.0188595},
				{-0.00716972, -0.00748992, 0.0 , -0.0168079, -0.0122809},
				{-0.0162054, -0.0134668, 0.0 , -0.0168058, -0.00932646},
				{-0.0171915, -0.0192338, 0.0 , -0.0128222, -0.011477},
				{0.00106925, -0.0128191, 0.0 , -0.00775439, -0.00240481},
				{0.00514418, -0.00667948, 0.0 , 0.00543797, -0.00909787},
				{-0.00416613, -0.00252634, 0.0 , 0.000512362, -0.00419998},
				{-0.00517762, -0.0029577, 0.0 , -0.00333422, -0.00731343},
				{-0.00432593, -0.00832689, 0.0 , -0.00759047, -0.00619233},
				{-0.0014897, -0.00353181, 0.0 , -0.000588059, -0.0065943},
				{-0.0050059, -0.00498551, 0.0 , -0.00305718, -0.00489056},
				{-0.0118812, -0.0031141, 0.0 , -0.00793666, -0.00806504},
				{-0.00520885, -0.00514621, 0.0 , -0.00869012, -0.00566143},
				{-0.0043537, -0.0062288, 0.0 , -0.00662583, -0.00726759}		};


				static double fferr_c_rphi_3[15][5]    = {{0.00449247, 0.00173156, 0.0 , 0.00192578, 0.00162946, },
				{0.0280056, 0.0106167, 0.0 , 0.0157169, 0.00943754, },
				{0.00357805, 0.00187352, 0.0 , 0.00232485, 0.0016503, },
				{0.00504079, 0.00253807, 0.0 , 0.0032725, 0.00210281, },
				{0.00714157, 0.00326615, 0.0 , 0.00429448, 0.0020223, },
				{0.00852127, 0.00432726, 0.0 , 0.00580422, 0.00349559, },
				{0.0100544, 0.00500652, 0.0 , 0.00680075, 0.00260472, },
				{0.00398804, 0.00229927, 0.0 , 0.00321634, 0.00131172, },
				{0.00303024, 0.00237425, 0.0 , 0.00267085, 0.00116956, },
				{0.00418036, 0.00204212, 0.0 , 0.00208454, 0.00226893, },
				{0.00384561, 0.00222793, 0.0 , 0.0046508, 0.00131023, },
				{0.00217538, 0.00212415, 0.0 , 0.00230496, 0.0013869, },
				{0.00207891, 0.00282755, 0.0 , 0.00188341, 0.00225156, },
				{0.00172663, 0.00312126, 0.0 , 0.00321313, 0.00178642, },
				{0.00120243, 0.00126862, 0.0 , 0.00178394, 0.00154664, }		};


				static double ff_c_weta2_3[15][5]    = {{0.000211241, 0.000339366, 0.0 , 0.000460719, 0.000349591},
				{0.000169044, 0.000189788, 0.0 , 0.000846793, 0.000453485},
				{0.000220171, 0.000303038, 0.0 , 0.000550722, 0.00056724},
				{0.000236568, 0.000288167, 0.0 , 0.000624296, 0.000454541},
				{0.000178188, 0.00027138, 0.0 , 0.000432994, 0.000449343},
				{0.00028824, 0.000283033, 0.0 , 0.00058951, 0.000526349},
				{3.24491e-05, 0.000130267, 0.0 , 0.000569238, 0.00037837},
				{0.000213787, 0.000304975, 0.0 , 0.000621668, 0.000438626},
				{0.000186756, 0.000226817, 0.0 , 0.000488237, 0.000551163},
				{0.000383223, 0.000244992, 0.0 , 0.000671322, 0.000437401},
				{0.000313184, 0.000336805, 0.0 , 0.000446489, 0.000408185},
				{0.00017615, 0.000352987, 0.0 , 0.000600662, 0.000329556},
				{0.000235946, 0.000338168, 0.0 , 0.000585613, 0.000539739},
				{0.000188489, 0.000287789, 0.0 , 0.000735307, 0.000523293},
				{0.0002956, 0.000299254, 0.0 , 0.000784857, 0.000530182}		};


				static double fferr_c_weta2_3[15][5]    = {{8.99092e-05, 3.55001e-05, 0.0 , 3.70657e-05, 2.55207e-05, },
				{0.0004169, 0.000117443, 0.0 , 0.000152202, 9.9114e-05, },
				{3.38066e-05, 2.01076e-05, 0.0 , 3.15924e-05, 2.46778e-05, },
				{3.86367e-05, 2.85651e-05, 0.0 , 4.26128e-05, 3.69486e-05, },
				{5.68096e-05, 3.57347e-05, 0.0 , 7.45816e-05, 4.89798e-05, },
				{6.88477e-05, 4.36018e-05, 0.0 , 6.65722e-05, 6.35478e-05, },
				{0.000109946, 5.71549e-05, 0.0 , 9.87698e-05, 7.98965e-05, },
				{3.31428e-05, 2.57886e-05, 0.0 , 4.21149e-05, 3.67617e-05, },
				{3.56803e-05, 3.59315e-05, 0.0 , 0.000104936, 5.01063e-05, },
				{5.11216e-05, 4.50978e-05, 0.0 , 9.19495e-05, 8.48716e-05, },
				{4.676e-05, 4.76132e-05, 0.0 , 8.67793e-05, 7.75288e-05, },
				{5.04131e-05, 5.58799e-05, 0.0 , 0.000108665, 8.51236e-05, },
				{9.55141e-05, 7.01742e-05, 0.0 , 0.000122697, 0.0001465, },
				{6.20522e-05, 6.4247e-05, 0.0 , 0.000143766, 0.000152077, },
				{4.46393e-05, 5.03599e-05, 0.0 , 0.00011446, 0.000127187, }		};


				static double ff_c_f1_3[15][5]    = {{-0.00283736, -0.00224003, 0.0 , 0.00277412, -0.0261427},
				{0.0237384, -0.0185746, 0.0 , 0.0187699, 0.00487632},
				{-0.00264636, 0.00041911, 0.0 , 0.0107647, 0.0179561},
				{-0.00842622, -0.000411332, 0.0 , 0.0122931, 0.0104484},
				{-0.00500602, 0.00365391, 0.0 , 0.00845629, 0.0104308},
				{0.00369114, -0.00115415, 0.0 , 0.0084424, 0.0176243},
				{-0.018076, -0.0182971, 0.0 , 0.0109882, 0.00579551},
				{0.00177297, -0.00206545, 0.0 , 0.00920016, 0.00998327},
				{0.00047645, -0.00727859, 0.0 , 0.00264901, 0.0164422},
				{0.0138007, -0.0147898, 0.0 , 0.0120805, 0.0170017},
				{0.0109786, -0.00591448, 0.0 , 0.0199934, 0.00841133},
				{-0.0105802, -0.00639421, 0.0 , 0.0266825, 0.00982489},
				{0.0347409, -0.00691761, 0.0 , -0.00753433, 0.00640576},
				{-0.0169988, -0.00481574, 0.0 , 0.0451113, 0.0142405},
				{0.00960271, 0.0193844, 0.0 , 0.0109586, 0.0081856}		};


				static double fferr_c_f1_3[15][5]    = {{0.00641471, 0.00254115, 0.0 , 0.00282888, 0.00293986, },
				{0.0365052, 0.0108067, 0.0 , 0.0143442, 0.0113363, },
				{0.00382968, 0.0020265, 0.0 , 0.00263497, 0.00251219, },
				{0.00480578, 0.00306091, 0.0 , 0.00399328, 0.00397472, },
				{0.00686401, 0.00462951, 0.0 , 0.00565747, 0.0053819, },
				{0.00775656, 0.00617447, 0.0 , 0.00791032, 0.00620097, },
				{0.0108446, 0.00756743, 0.0 , 0.00944674, 0.00811273, },
				{0.00518142, 0.00336358, 0.0 , 0.00531947, 0.00390064, },
				{0.00634059, 0.004482, 0.0 , 0.00584753, 0.00490345, },
				{0.00976269, 0.00649539, 0.0 , 0.0097156, 0.00919306, },
				{0.0093383, 0.00640718, 0.0 , 0.0107045, 0.00748049, },
				{0.00951918, 0.0080179, 0.0 , 0.0137986, 0.00896775, },
				{0.018353, 0.0104704, 0.0 , 0.0202819, 0.012952, },
				{0.0126276, 0.0115627, 0.0 , 0.0261085, 0.0130777, },
				{0.00860782, 0.00752338, 0.0 , 0.0142694, 0.013026, }		};


				static double ff_c_fside_3[15][5]    = {{0.0100942, 0.038134, 0.0 , 0.0435732, 0.0407881},
				{0.0248266, 0.0178492, 0.0 , 0.0616979, 0.0440257},
				{-0.00218776, 0.0172654, 0.0 , 0.0395399, 0.0164438},
				{-0.00473008, 0.0106091, 0.0 , 0.0367717, 0.0198814},
				{-0.0100792, 0.0115964, 0.0 , 0.0269876, 0.0253235},
				{0.0152557, 0.00492284, 0.0 , 0.0355296, 0.0179019},
				{0.0125739, -0.00827947, 0.0 , 0.0236133, 0.0288319},
				{-0.00512412, 0.00169811, 0.0 , 0.0327313, 0.0187401},
				{-0.00334696, 0.00614297, 0.0 , 0.0369884, 0.0288805},
				{0.0126417, 0.00909626, 0.0 , 0.0515058, 0.0217753},
				{0.00787869, 0.0207673, 0.0 , 0.0284234, 0.0237688},
				{0.00552179, 0.0218053, 0.0 , 0.030822, 0.0166866},
				{0.0277857, 0.0172872, 0.0 , 0.037407, 0.0367241},
				{0.0102402, 0.0260985, 0.0 , 0.0572183, 0.0372211},
				{0.011024, 0.0256156, 0.0 , 0.0371346, 0.0184029}		};


				static double fferr_c_fside_3[15][5]    = {{0.00575221, 0.00363145, 0.0 , 0.00359787, 0.00182361, },
				{0.0194383, 0.0115786, 0.0 , 0.0196105, 0.00671778, },
				{0.00353506, 0.00264631, 0.0 , 0.00419285, 0.00230736, },
				{0.00475646, 0.00382582, 0.0 , 0.00631507, 0.00311025, },
				{0.0069734, 0.00568843, 0.0 , 0.0116306, 0.00451186, },
				{0.00660687, 0.00812618, 0.0 , 0.0112069, 0.00618306, },
				{0.00658233, 0.0106188, 0.0 , 0.0162255, 0.00516196, },
				{0.00540204, 0.00449685, 0.0 , 0.0072409, 0.00344526, },
				{0.00667128, 0.00526863, 0.0 , 0.00877635, 0.00260407, },
				{0.00546595, 0.00807038, 0.0 , 0.0110868, 0.00527176, },
				{0.00533143, 0.00637922, 0.0 , 0.0122301, 0.0053513, },
				{0.00777679, 0.00697114, 0.0 , 0.0145882, 0.00807357, },
				{0.00836022, 0.00842232, 0.0 , 0.0122898, 0.00488619, },
				{0.00509522, 0.0083857, 0.0 , 0.0193428, 0.0077608, },
				{0.00734488, 0.00913755, 0.0 , 0.00981957, 0.00696896, }		};


				static double ff_c_wtot_3[15][5]    = {{0.0907226, 0.0673699, 0.0 , 0.109682, 0.158716},
				{0.0834816, 0.0867665, 0.0 , 0.117862, 0.135443},
				{0.0721939, 0.0505605, 0.0 , 0.0861378, 0.0792341},
				{0.0791664, 0.032335, 0.0 , 0.109523, 0.0924184},
				{0.0445981, 0.0475566, 0.0 , 0.0964983, 0.116686},
				{0.102197, 0.0470397, 0.0 , 0.119019, 0.0954285},
				{0.0761328, 0.0591919, 0.0 , 0.113239, 0.119088},
				{0.0731053, 0.062439, 0.0 , 0.111431, 0.0923889},
				{0.0781574, 0.0458379, 0.0 , 0.100981, 0.0986322},
				{0.104401, 0.0698087, 0.0 , 0.14434, 0.124926},
				{0.0859715, 0.129133, 0.0 , 0.0925939, 0.117717},
				{0.0791217, 0.100848, 0.0 , 0.139713, 0.148766},
				{0.135654, 0.0927033, 0.0 , 0.13094, 0.15189},
				{0.114707, 0.0886292, 0.0 , 0.132405, 0.146971},
				{0.096306, 0.0763898, 0.0 , 0.143042, 0.0564618}		};


				static double fferr_c_wtot_3[15][5]    = {{0.0274764, 0.0144002, 0.0 , 0.0130154, 0.00792317, },
				{0.0575314, 0.034921, 0.0 , 0.0634439, 0.0261203, },
				{0.00920921, 0.00679203, 0.0 , 0.00953852, 0.00589201, },
				{0.0124211, 0.00960771, 0.0 , 0.0152779, 0.00888904, },
				{0.0174866, 0.0127265, 0.0 , 0.0205852, 0.0133546, },
				{0.0217254, 0.0194052, 0.0 , 0.025492, 0.0169343, },
				{0.0245976, 0.0252494, 0.0 , 0.0339113, 0.0201772, },
				{0.0115863, 0.0109337, 0.0 , 0.0168047, 0.0118324, },
				{0.0138631, 0.0139013, 0.0 , 0.0184922, 0.0141206, },
				{0.0247858, 0.0227119, 0.0 , 0.0379699, 0.0217341, },
				{0.0196416, 0.0231531, 0.0 , 0.0351792, 0.0228244, },
				{0.0198277, 0.0263616, 0.0 , 0.0350468, 0.0285802, },
				{0.0326889, 0.0357352, 0.0 , 0.041447, 0.0382203, },
				{0.0220476, 0.0381965, 0.0 , 0.0808082, 0.0496194, },
				{0.0182266, 0.0264236, 0.0 , 0.0377322, 0.0455929, }		};


				static double ff_c_w1_3[15][5]    = {{0.0132845, 0.0199765, 0.0 , 0.0140423, 0.0374244},
				{-0.00741005, 0.00697881, 0.0 , 0.00587881, 0.0414731},
				{-0.00449193, 0.00671536, 0.0 , 0.00761831, 0.0391556},
				{-0.00490612, 0.00588256, 0.0 , 0.00815588, 0.0340305},
				{-0.00132614, 0.0121307, 0.0 , 0.00842655, 0.0295202},
				{0.00500166, 0.00124741, 0.0 , 0.00226444, 0.0287578},
				{-0.00378811, 0.00112408, 0.0 , 0.00414926, 0.0318141},
				{-0.00477022, 0.00595874, 0.0 , 0.00819439, 0.0317536},
				{-0.00598013, 0.0039078, 0.0 , 0.00444847, 0.0282691},
				{0.011159, 0.00569898, 0.0 , 0.010331, 0.0287033},
				{-0.000721157, 0.00915611, 0.0 , 0.00472254, 0.0265028},
				{0.002379, 0.00834143, 0.0 , 0.00785702, 0.0233479},
				{0.0124575, 0.00786495, 0.0 , 0.0114369, 0.032468},
				{-0.0010578, 0.0112813, 0.0 , 0.0287367, 0.0422415},
				{-7.05719e-05, 0.0136604, 0.0 , 0.0198708, 0.0286197}		};


				static double fferr_c_w1_3[15][5]    = {{0.0035923, 0.00197523, 0.0 , 0.00181241, 0.0016589, },
				{0.0145195, 0.00651686, 0.0 , 0.0110457, 0.00691982, },
				{0.00190608, 0.00121149, 0.0 , 0.00176906, 0.001685, },
				{0.00251668, 0.00172693, 0.0 , 0.00269724, 0.00254432, },
				{0.00363126, 0.00290804, 0.0 , 0.00362769, 0.00356979, },
				{0.00460908, 0.0034062, 0.0 , 0.004228, 0.00461405, },
				{0.00530438, 0.00477414, 0.0 , 0.00658661, 0.00540375, },
				{0.00273576, 0.00204148, 0.0 , 0.00308666, 0.00272806, },
				{0.00372437, 0.00251383, 0.0 , 0.00353206, 0.00345705, },
				{0.00455272, 0.00369024, 0.0 , 0.00482217, 0.00599716, },
				{0.00490766, 0.00401985, 0.0 , 0.00714006, 0.00516752, },
				{0.00443902, 0.00444829, 0.0 , 0.00677177, 0.0080542, },
				{0.00854035, 0.00635942, 0.0 , 0.00828201, 0.00928939, },
				{0.005339, 0.00656646, 0.0 , 0.0118262, 0.00815735, },
				{0.00437077, 0.00489497, 0.0 , 0.00628305, 0.00659122, }		};


				static double ff_c_deltae_3[15][5]    = {{-0.785927, -1.83691, 0.0 , -4.42087, -3.76677},
				{7.2575, 3.01823, 0.0 , -5.06828, 0.667828},
				{-5.00449, -5.24278, 0.0 , -3.43986, -5.53903},
				{-1.49082, -4.85159, 0.0 , -0.227646, -3.03908},
				{-4.3637, -1.96877, 0.0 , -3.89747, -4.61589},
				{-3.48011, -1.05347, 0.0 , -2.38289, -2.33827},
				{-0.634632, 0.848835, 0.0 , 3.34709, 1.07077},
				{-2.26796, -3.49242, 0.0 , -3.51233, -2.34111},
				{-2.09748, -1.7921, 0.0 , 1.34101, -1.31805},
				{-5.83494, -1.96873, 0.0 , -0.490498, -0.683294},
				{1.52159, -3.5943, 0.0 , -2.21316, -0.982386},
				{-2.81707, -2.79604, 0.0 , -10.4005, 1.87479},
				{-1.94047, -8.10898, 0.0 , -18.3064, -0.226549},
				{1.75637, -0.742567, 0.0 , 12.3404, -5.4667},
				{0.892994, -1.90577, 0.0 , 7.94258, -10.2811}		};


				static double fferr_c_deltae_3[15][5]    = {{0.837157, 0.510563, 0.0 , 1.07322, 0.728852, },
				{5.76036, 2.18731, 0.0 , 9.51449, 3.54328, },
				{1.48125, 0.856539, 0.0 , 1.50915, 1.00333, },
				{2.03926, 1.27715, 0.0 , 2.12577, 1.36305, },
				{2.90889, 1.74617, 0.0 , 3.63201, 2.07766, },
				{2.869, 1.97114, 0.0 , 3.64834, 2.40489, },
				{2.65593, 2.52993, 0.0 , 3.68292, 3.16906, },
				{1.74168, 1.09452, 0.0 , 2.95521, 1.71759, },
				{1.59229, 1.47928, 0.0 , 2.51194, 2.05927, },
				{3.08154, 2.28467, 0.0 , 6.01542, 4.27003, },
				{2.49313, 2.42615, 0.0 , 5.0011, 3.52923, },
				{3.27643, 3.26821, 0.0 , 5.441, 4.19523, },
				{5.66362, 5.55844, 0.0 , 17.1415, 4.40889, },
				{5.24577, 3.87957, 0.0 , 6.28921, 7.06551, },
				{3.07324, 2.82507, 0.0 , 5.25998, 6.00715, }		};


				static double ff_c_eratio_3[15][5]    = {{-0.00117612, -0.00123531, 0.0 , 0.00219792, 0.00035578},
				{0.00236231, -0.00977504, 0.0 , -0.0371457, -0.00154102},
				{0.0153341, 0.0140193, 0.0 , 0.0179525, 0.0072543},
				{0.00740397, 0.0148686, 0.0 , 0.0184461, 0.00218523},
				{0.01512, 0.00838852, 0.0 , 0.0110382, 0.00531119},
				{0.00626302, 0.00535411, 0.0 , -0.0113583, -0.00159723},
				{0.00704026, -0.00514823, 0.0 , -0.0201758, 0.00182605},
				{0.00651199, 0.0066697, 0.0 , 0.00544542, 0.00278956},
				{0.00224334, -0.00103116, 0.0 , 0.0223693, -0.000771761},
				{0.00240391, -0.00201011, 0.0 , -0.0129716, -0.00140113},
				{0.00877154, 0.00617278, 0.0 , 0.00660652, 0.000625789},
				{0.0100549, 0.00834262, 0.0 , 0.00892228, -0.00248522},
				{-0.00225317, 0.00598633, 0.0 , -0.00285798, 0.00179577},
				{-0.00500751, -0.0110963, 0.0 , -0.0132991, 0.00216985},
				{0.000647306, -0.000371575, 0.0 , -0.01222, 0.00443339}		};


				static double fferr_c_eratio_3[15][5]    = {{0.00297891, 0.00235796, 0.0 , 0.00288805, 0.0014082, },
				{0.0240611, 0.0128823, 0.0 , 0.00721628, 0.00300035, },
				{0.00407643, 0.00255738, 0.0 , 0.00431767, 0.00123836, },
				{0.00479196, 0.00384318, 0.0 , 0.00659697, 0.00140324, },
				{0.00795432, 0.00487726, 0.0 , 0.00901341, 0.00276063, },
				{0.00655879, 0.00498926, 0.0 , 0.00829441, 0.00117752, },
				{0.00764924, 0.00305677, 0.0 , 0.00216825, 0.00316338, },
				{0.00366329, 0.0029596, 0.0 , 0.00607432, 0.00171176, },
				{0.00377641, 0.00305377, 0.0 , 0.0106184, 0.00144365, },
				{0.00688332, 0.00408607, 0.0 , 0.00195062, 0.000971034, },
				{0.00815512, 0.00605768, 0.0 , 0.0117919, 0.00257737, },
				{0.00971676, 0.00545077, 0.0 , 0.00958213, 0.0010086, },
				{0.00277271, 0.00783541, 0.0 , 0.00206498, 0.00644466, },
				{0.00249448, 0.00456599, 0.0 , 0.00726222, 0.0053854, },
				{0.00200336, 0.00171476, 0.0 , 0.0169419, 0.00187683, }		};

				//unconverted photons

				static double ff_u_rhad1_3[15][5]    = {{-0.0034745, -0.00235099, 0.0 , -0.000155916, -0.000737883},
				{0.000744989, -0.00110784, 0.0 , -0.000556218, 0.00254812},
				{-0.000564042, -0.000485175, 0.0 , -0.000188405, -0.000521},
				{-0.000569941, 0.00011893, 0.0 , -0.00149687, -0.000730097},
				{-0.000341317, -0.000545282, 0.0 , -0.00139224, -0.000389023},
				{-0.000318478, -0.000568973, 0.0 , 0.000856707, -0.000390534},
				{-0.000411879, 0.000213057, 0.0 , 9.53348e-06, -0.000898713},
				{-0.000493028, -6.81262e-05, 0.0 , -0.000145628, -0.000108469},
				{-0.00032398, -0.000576657, 0.0 , 0.000773878, -0.000120155},
				{-0.000295906, -0.000212512, 0.0 , -0.00149979, -0.000379009},
				{-0.00029821, -0.000264573, 0.0 , -0.000388181, 0.000173547},
				{6.31384e-05, 0.000119036, 0.0 , -8.76216e-05, -0.000455548},
				{-0.000633782, -1.18738e-05, 0.0 , 0.000197221, 3.18905e-05},
				{-0.00047793, -0.000129307, 0.0 , -0.00109484, -0.000167764},
				{-0.0004274, 0.000127285, 0.0 , -0.000293432, -0.00025635}		};


				static double fferr_u_rhad1_3[15][5]    = {{0.000335328, 0.000525684, 0.0 , 0.000789336, 0.000428592, },
				{0.000651199, 0.000997868, 0.0 , 0.00168433, 0.000837623, },
				{0.000129737, 0.00018943, 0.0 , 0.000375763, 0.000195553, },
				{0.000181414, 0.000183928, 0.0 , 0.000560332, 0.000310391, },
				{0.000199884, 0.000328912, 0.0 , 0.000681844, 0.000360211, },
				{0.000221769, 0.000496635, 0.0 , 0.000831241, 0.000337506, },
				{0.000311155, 0.000270231, 0.0 , 0.000810708, 0.00066463, },
				{0.000154656, 0.000179112, 0.0 , 0.000454989, 0.000198378, },
				{0.000188669, 0.00037057, 0.0 , 0.000288565, 0.000222832, },
				{0.000191815, 0.000199762, 0.0 , 0.000754792, 0.00033256, },
				{0.000189556, 0.000170938, 0.0 , 0.000534543, 0.000262468, },
				{0.000143883, 0.000126214, 0.0 , 0.000749023, 0.000361342, },
				{0.000382597, 0.000163364, 0.0 , 0.000669588, 0.00031807, },
				{0.000277102, 0.000415423, 0.0 , 0.0011741, 0.000350805, },
				{0.00016787, 0.000136082, 0.0 , 0.000476111, 0.000290159, }		};


				static double ff_u_rhad_3[15][5]    = {{0, -0.00276564, 0.0 , 0, 0},
				{0, -0.00163934, 0.0 , 0, 0},
				{0, -0.000121347, 0.0 , 0, 0},
				{0, -0.00046459, 0.0 , 0, 0},
				{0, -0.000452925, 0.0 , 0, 0},
				{0, -0.000471185, 0.0 , 0, 0},
				{0, -0.000465611, 0.0 , 0, 0},
				{0, -5.76166e-05, 0.0 , 0, 0},
				{0, -0.0001852, 0.0 , 0, 0},
				{0, 0.000279547, 0.0 , 0, 0},
				{0, 1.44355e-05, 0.0 , 0, 0},
				{0, 0.000102757, 0.0 , 0, 0},
				{0, -8.34829e-05, 0.0 , 0, 0},
				{0, -2.23047e-05, 0.0 , 0, 0},
				{0, 5.36031e-06, 0.0 , 0, 0}		};


				static double fferr_u_rhad_3[15][5]    = {{0, 0.000402161, 0.0 , 0, 0, },
				{0, 0.000935684, 0.0 , 0, 0, },
				{0, 0.000135772, 0.0 , 0, 0, },
				{0, 0.000227771, 0.0 , 0, 0, },
				{0, 0.000196654, 0.0 , 0, 0, },
				{0, 0.00031721, 0.0 , 0, 0, },
				{0, 0.00046739, 0.0 , 0, 0, },
				{0, 0.000128714, 0.0 , 0, 0, },
				{0, 0.000188175, 0.0 , 0, 0, },
				{0, 0.000138272, 0.0 , 0, 0, },
				{0, 9.41798e-05, 0.0 , 0, 0, },
				{0, 8.06156e-05, 0.0 , 0, 0, },
				{0, 0.000113067, 0.0 , 0, 0, },
				{0, 0.00011779, 0.0 , 0, 0, },
				{0, 8.46184e-05, 0.0 , 0, 0, }		};


				static double ff_u_e277_3[15][5]    = {{227.592, 414.993, 0.0 , 911.721, 857.795},
				{-632.365, 861.805, 0.0 , 2141.69, 103.984},
				{308.437, 944.891, 0.0 , 2188.09, 1029.62},
				{322.912, 809.514, 0.0 , -682.496, 1541.77},
				{479.773, 1013.96, 0.0 , 2071.81, 1587.62},
				{543.051, 1527.09, 0.0 , 2921.76, 410.578},
				{125.781, 974.574, 0.0 , -5613.84, -1156.45},
				{42.2227, 1071.64, 0.0 , -1689.87, -1669.39},
				{-52.6211, 1080.58, 0.0 , -2308.45, 619.828},
				{631.414, 2156.22, 0.0 , -5545.95, 1189.97},
				{304.5, 4088.16, 0.0 , -2192.92, -3734},
				{-2381.29, 265.109, 0.0 , 4299.34, -11172.7},
				{494.016, -345.453, 0.0 , 8891.27, 0},
				{-3791.23, -9182.95, 0.0 , -42122, 0},
				{-29389.6, -10264, 0.0 , 0, 0}		};


				static double fferr_u_e277_3[15][5]    = {{6.65367, 11.3252, 0.0 , 40.2963, 45.3708, },
				{199.029, 375.756, 0.0 , 1112.47, 1094.89, },
				{38.0255, 76.4908, 0.0 , 311.854, 256.863, },
				{75.8654, 167.285, 0.0 , 662.5, 508.601, },
				{133.115, 298.647, 0.0 , 1199.68, 902.004, },
				{230.475, 484.216, 0.0 , 1968.63, 1533.76, },
				{348.094, 732.704, 0.0 , 2792.47, 2359.26, },
				{193.797, 410.961, 0.0 , 1603.23, 1495.13, },
				{348.052, 785.225, 0.0 , 2778.72, 2267.85, },
				{612.147, 1446.24, 0.0 , 5542.53, 3325.92, },
				{752.668, 1735.71, 0.0 , 5674.5, 3773.23, },
				{1205.1, 2258.88, 0.0 , 9825.63, 2622.9, },
				{1923.33, 4821.88, 0.0 , 18405.5, 0, },
				{2686.26, 5773.32, 0.0 , 10145.9, 0, },
				{3436.41, 4085.72, 0.0 , 0, 0, }		};


				static double ff_u_reta_3[15][5]    = {{-0.00236911, -0.00579596, 0.0 , -0.0101016, -0.00922811},
				{-0.00572747, -0.00678664, 0.0 , -0.0150055, -0.00907516},
				{-0.00258183, -0.00563121, 0.0 , -0.00976294, -0.00861955},
				{-0.00367403, -0.00606507, 0.0 , -0.0107141, -0.00997674},
				{-0.00287819, -0.00590307, 0.0 , -0.0105825, -0.00980276},
				{-0.0033381, -0.00670868, 0.0 , -0.0103733, -0.00858361},
				{-0.00419873, -0.00660658, 0.0 , -0.0132095, -0.0105674},
				{-0.00479686, -0.00671846, 0.0 , -0.0101184, -0.01029},
				{-0.00370848, -0.00653166, 0.0 , -0.0110464, -0.0106593},
				{-0.00495791, -0.00671661, 0.0 , -0.0124241, -0.0113125},
				{-0.00470561, -0.00674087, 0.0 , -0.00989091, -0.0111228},
				{-0.00394118, -0.0075233, 0.0 , -0.0128326, -0.0124456},
				{-0.00520647, -0.00739664, 0.0 , -0.00799888, -0.0104359},
				{-0.00420606, -0.00785506, 0.0 , -0.01013, -0.0108653},
				{-0.00416273, -0.00849926, 0.0 , -0.0097068, -0.0122566}		};


				static double fferr_u_reta_3[15][5]    = {{0.000686859, 0.000558634, 0.0 , 0.000857774, 0.000437326, },
				{0.00142475, 0.00266441, 0.0 , 0.00271385, 0.00133426, },
				{0.000404908, 0.00035658, 0.0 , 0.000525476, 0.000423011, },
				{0.000508956, 0.000546087, 0.0 , 0.000682957, 0.00052893, },
				{0.000784242, 0.000715566, 0.0 , 0.000841737, 0.000697191, },
				{0.00111332, 0.000829194, 0.0 , 0.00104976, 0.000881738, },
				{0.000793602, 0.000877836, 0.0 , 0.000947597, 0.00124283, },
				{0.000362807, 0.00036524, 0.0 , 0.000653547, 0.000482795, },
				{0.000547188, 0.000404641, 0.0 , 0.000730884, 0.000585514, },
				{0.00047671, 0.000516977, 0.0 , 0.00112042, 0.000957572, },
				{0.000482248, 0.000577237, 0.0 , 0.000990255, 0.00103845, },
				{0.000423521, 0.000478669, 0.0 , 0.00101116, 0.00107842, },
				{0.000635204, 0.000588331, 0.0 , 0.00226659, 0.00147646, },
				{0.000581201, 0.000825438, 0.0 , 0.00128011, 0.00165785, },
				{0.000407231, 0.000563672, 0.0 , 0.00120967, 0.00141991, }		};


				static double ff_u_rphi_3[15][5]    = {{-0.000305474, -0.00322962, 0.0 , -0.00395846, -0.00456816},
				{-0.00685787, -0.00854397, 0.0 , -0.00804943, -0.00976884},
				{-0.00229454, -0.00269032, 0.0 , -0.00319237, -0.00546974},
				{-0.00089407, -0.00342721, 0.0 , -0.00329864, -0.00725633},
				{-0.00258601, -0.00437403, 0.0 , -0.00493705, -0.00751364},
				{-0.00112391, -0.0052582, 0.0 , -0.00499374, -0.00637454},
				{-0.00315624, -0.00441563, 0.0 , -0.00615793, -0.00464082},
				{-0.00384063, -0.00394541, 0.0 , -0.00155443, -0.00625104},
				{-0.00346184, -0.004641, 0.0 , -0.004807, -0.00647062},
				{-0.000913143, -0.0056088, 0.0 , -0.00701874, -0.00650942},
				{-0.0029633, -0.00547069, 0.0 , -0.0043481, -0.00602138},
				{-0.00236446, -0.00430584, 0.0 , -0.00214875, -0.00762415},
				{-0.00290304, -0.00553507, 0.0 , -0.00148606, -0.00460804},
				{-0.00322992, -0.00409353, 0.0 , -0.00644088, -0.0065648},
				{-0.00394791, -0.0054211, 0.0 , -0.00453937, -0.00842947}		};


				static double fferr_u_rphi_3[15][5]    = {{0.000834532, 0.000843749, 0.0 , 0.00107971, 0.000584992, },
				{0.0013894, 0.00177633, 0.0 , 0.0023022, 0.00103266, },
				{0.000469983, 0.00054774, 0.0 , 0.00070035, 0.000603326, },
				{0.000842643, 0.000780156, 0.0 , 0.00118124, 0.000696193, },
				{0.000853624, 0.000811456, 0.0 , 0.00123909, 0.000609137, },
				{0.00174146, 0.000905765, 0.0 , 0.00131603, 0.000842795, },
				{0.000889942, 0.00137637, 0.0 , 0.00161497, 0.00174034, },
				{0.000363788, 0.000529357, 0.0 , 0.00114749, 0.000562812, },
				{0.000343189, 0.000614778, 0.0 , 0.00089997, 0.000656452, },
				{0.0014213, 0.000623836, 0.0 , 0.00130635, 0.000777536, },
				{0.000444916, 0.000737514, 0.0 , 0.000991732, 0.00137676, },
				{0.00047179, 0.000703012, 0.0 , 0.00292497, 0.00120422, },
				{0.000610828, 0.000889913, 0.0 , 0.00272047, 0.00152956, },
				{0.000510977, 0.000950054, 0.0 , 0.00124813, 0.00130969, },
				{0.00047476, 0.000593746, 0.0 , 0.00116724, 0.00135768, }		};


				static double ff_u_weta2_3[15][5]    = {{0.00028348, 0.000390826, 0.0 , 0.000474095, 0.000359847},
				{0.000222847, 0.000229702, 0.0 , 0.000631219, 0.000313238},
				{0.000207937, 0.000292157, 0.0 , 0.00054237, 0.00039867},
				{0.000204817, 0.000291603, 0.0 , 0.000578911, 0.000452554},
				{0.000196154, 0.000262211, 0.0 , 0.000593005, 0.000418627},
				{0.000255547, 0.000325335, 0.0 , 0.000615834, 0.000349823},
				{0.000186024, 0.00032093, 0.0 , 0.000695606, 0.000496361},
				{0.000238135, 0.000262124, 0.0 , 0.000574632, 0.000397653},
				{0.00024559, 0.000306756, 0.0 , 0.000582202, 0.000468925},
				{0.000201428, 0.000307975, 0.0 , 0.000745793, 0.000462009},
				{0.000246387, 0.000334051, 0.0 , 0.000598042, 0.0004251},
				{0.000200034, 0.000267032, 0.0 , 0.000733565, 0.000654751},
				{0.000188941, 0.000321447, 0.0 , 0.000422063, 0.000454101},
				{0.000239731, 0.00031817, 0.0 , 0.000660912, 0.000474905},
				{0.000308221, 0.000370061, 0.0 , 0.000584001, 0.0005279}		};


				static double fferr_u_weta2_3[15][5]    = {{2.40298e-05, 1.86712e-05, 0.0 , 2.68156e-05, 1.47883e-05, },
				{8.47199e-05, 0.000112922, 0.0 , 0.000102206, 7.52174e-05, },
				{1.42256e-05, 1.45757e-05, 0.0 , 2.49102e-05, 2.1905e-05, },
				{1.76693e-05, 1.99397e-05, 0.0 , 3.60858e-05, 3.52482e-05, },
				{2.77906e-05, 3.11183e-05, 0.0 , 5.74739e-05, 4.46137e-05, },
				{3.59478e-05, 3.95614e-05, 0.0 , 7.03551e-05, 5.90019e-05, },
				{4.33753e-05, 3.71385e-05, 0.0 , 7.30774e-05, 8.05869e-05, },
				{1.62762e-05, 1.7876e-05, 0.0 , 4.0868e-05, 3.21704e-05, },
				{2.24482e-05, 2.12253e-05, 0.0 , 5.22801e-05, 4.60522e-05, },
				{3.07083e-05, 2.92893e-05, 0.0 , 6.94813e-05, 7.58377e-05, },
				{2.69835e-05, 2.84693e-05, 0.0 , 7.33232e-05, 8.84992e-05, },
				{2.58146e-05, 2.9511e-05, 0.0 , 8.1389e-05, 8.98676e-05, },
				{4.54599e-05, 4.72221e-05, 0.0 , 9.4331e-05, 0.000109912, },
				{3.66397e-05, 4.74214e-05, 0.0 , 0.000106989, 0.000139112, },
				{2.67053e-05, 3.57317e-05, 0.0 , 6.56102e-05, 0.000112815, }		};


				static double ff_u_f1_3[15][5]    = {{-0.0132794, -0.00923616, 0.0 , -0.0161323, -0.0407871},
				{-0.023862, -0.0347714, 0.0 , 0.01296, -0.0327254},
				{-0.0115819, -0.01094, 0.0 , -0.00555271, -0.0118555},
				{-0.0112605, -0.0116678, 0.0 , 0.000586748, -0.00613321},
				{-0.0106728, -0.00728919, 0.0 , -0.00679323, -0.00867437},
				{-0.013451, -0.00666972, 0.0 , -0.00128117, -0.0114961},
				{-0.00808619, 0.00240634, 0.0 , 0.0407445, 0.00745131},
				{-0.00402491, -0.0100703, 0.0 , -0.000815719, 0.00360702},
				{0.00113527, -0.0146121, 0.0 , 0.00190702, -0.00321773},
				{-0.00370705, -0.0111584, 0.0 , 0.0317247, 0.000742704},
				{-0.00523086, -0.00801635, 0.0 , 0.00309137, 0.00695418},
				{-0.00138007, 0.00133362, 0.0 , 0.0310667, 0.0266611},
				{-0.00310862, -0.00374267, 0.0 , -0.0336001, -0.0171966},
				{-0.00286631, -0.00186023, 0.0 , 0.0257831, 0.012504},
				{0.0143795, 0.0125523, 0.0 , -0.00529353, 0.019855}		};


				static double fferr_u_f1_3[15][5]    = {{0.00265731, 0.00202228, 0.0 , 0.00289171, 0.00225613, },
				{0.0125232, 0.0115334, 0.0 , 0.0166793, 0.0109863, },
				{0.002545, 0.00208591, 0.0 , 0.00369829, 0.00289457, },
				{0.0035516, 0.00326872, 0.0 , 0.00604933, 0.00419218, },
				{0.00501711, 0.00465475, 0.0 , 0.00913379, 0.0058303, },
				{0.00742029, 0.00600509, 0.0 , 0.0118232, 0.00803833, },
				{0.00894437, 0.00749609, 0.0 , 0.0154068, 0.0106351, },
				{0.00368999, 0.00333992, 0.0 , 0.00769132, 0.00469618, },
				{0.00470218, 0.00427995, 0.0 , 0.00858667, 0.00603176, },
				{0.00730249, 0.00649601, 0.0 , 0.0158299, 0.00810659, },
				{0.00682323, 0.00643103, 0.0 , 0.013176, 0.00952799, },
				{0.00688767, 0.00608388, 0.0 , 0.0156892, 0.00799089, },
				{0.00924441, 0.0110844, 0.0 , 0.0238869, 0.0104322, },
				{0.0086091, 0.00988593, 0.0 , 0.0224885, 0.0134254, },
				{0.00599958, 0.00613788, 0.0 , 0.0135026, 0.0116243, }		};


				static double ff_u_fside_3[15][5]    = {{0.0184333, 0.0202613, 0.0 , 0.0271789, 0.0244302},
				{0.00895266, 0.0134084, 0.0 , 0.0460649, 0.0368582},
				{-0.00393361, 0.000102073, 0.0 , 0.0172051, 0.0105997},
				{-0.00533277, -0.00154111, 0.0 , 0.0276285, 0.00565819},
				{-0.00661579, -0.00577721, 0.0 , 0.0133656, 0.0086558},
				{-0.00944418, 0.00197107, 0.0 , 0.02821, 0.0146661},
				{-0.00333877, 0.00880924, 0.0 , 0.0294927, 0.0120539},
				{-0.0123948, -0.000899911, 0.0 , 0.0227809, 0.0151537},
				{8.31485e-05, -0.00359896, 0.0 , 0.0254115, 0.0195059},
				{0.00233634, 0.0164399, 0.0 , 0.0428541, 0.0244654},
				{-0.00469433, 0.0175474, 0.0 , 0.0164292, 0.0168118},
				{-2.55406e-05, 0.00582142, 0.0 , 0.0444934, 0.0259199},
				{0.0013677, 0.0200502, 0.0 , 0.0204745, 0.011108},
				{0.00322233, 0.00483049, 0.0 , 0.0420924, 0.0286032},
				{0.000893548, 0.0135594, 0.0 , 0.0228496, 0.0172752}		};


				static double fferr_u_fside_3[15][5]    = {{0.00156141, 0.00177377, 0.0 , 0.00237441, 0.000926962, },
				{0.00587734, 0.00989781, 0.0 , 0.013228, 0.00301708, },
				{0.00177997, 0.00201861, 0.0 , 0.00360079, 0.00183907, },
				{0.00248673, 0.00319985, 0.0 , 0.00528436, 0.00295234, },
				{0.00368216, 0.00473033, 0.0 , 0.00803369, 0.00416545, },
				{0.00506039, 0.00571386, 0.0 , 0.00971042, 0.00519429, },
				{0.00721287, 0.00812814, 0.0 , 0.013879, 0.00673841, },
				{0.00320252, 0.00327159, 0.0 , 0.00692349, 0.00302329, },
				{0.00298691, 0.00423178, 0.0 , 0.00700297, 0.00245909, },
				{0.00450819, 0.00366514, 0.0 , 0.0102126, 0.00313905, },
				{0.00498119, 0.00408612, 0.0 , 0.0142603, 0.0033696, },
				{0.00482072, 0.00407828, 0.0 , 0.00914537, 0.00423425, },
				{0.00599288, 0.00567073, 0.0 , 0.0153157, 0.00806374, },
				{0.00558145, 0.00639028, 0.0 , 0.0106543, 0.00645184, },
				{0.00362476, 0.00449441, 0.0 , 0.00685385, 0.00466538, }		};


				static double ff_u_wtot_3[15][5]    = {{0.0579259, 0.0781852, 0.0 , 0.0842078, 0.098034},
				{0.116355, 0.0626781, 0.0 , 0.247905, 0.156139},
				{0.0582972, 0.0489225, 0.0 , 0.0990003, 0.0704358},
				{0.0342921, 0.0510335, 0.0 , 0.112826, 0.0621874},
				{0.040807, 0.0450211, 0.0 , 0.0785652, 0.0660264},
				{0.0420097, 0.070593, 0.0 , 0.104328, 0.0988842},
				{0.067987, 0.0859059, 0.0 , 0.129202, 0.0907009},
				{0.0429789, 0.0569315, 0.0 , 0.108232, 0.105549},
				{0.0730537, 0.0697446, 0.0 , 0.126803, 0.0977081},
				{0.0606347, 0.0896521, 0.0 , 0.115646, 0.107983},
				{0.0707045, 0.102623, 0.0 , 0.163666, 0.0928379},
				{0.0246903, 0.0762097, 0.0 , 0.1487, 0.0927866},
				{0.0687231, 0.0447435, 0.0 , 0.137709, 0.032428},
				{0.0877959, 0.0818456, 0.0 , 0.182331, 0.131588},
				{0.0245583, 0.0649754, 0.0 , 0.146889, 0.0941547}		};


				static double fferr_u_wtot_3[15][5]    = {{0.00920584, 0.00808177, 0.0 , 0.0118026, 0.00541334, },
				{0.0374353, 0.0340457, 0.0 , 0.0582204, 0.0211959, },
				{0.00695482, 0.00631329, 0.0 , 0.0108882, 0.00589883, },
				{0.00925283, 0.00933245, 0.0 , 0.0163109, 0.00896434, },
				{0.0124086, 0.0124772, 0.0 , 0.0243592, 0.0132545, },
				{0.0173588, 0.0185761, 0.0 , 0.0347038, 0.0160083, },
				{0.0221842, 0.0210237, 0.0 , 0.0413636, 0.019274, },
				{0.0085341, 0.00939048, 0.0 , 0.0198877, 0.0105362, },
				{0.0117017, 0.0128033, 0.0 , 0.0219944, 0.0135414, },
				{0.0164345, 0.0171045, 0.0 , 0.0346116, 0.0253041, },
				{0.0160882, 0.0177995, 0.0 , 0.0288662, 0.0245427, },
				{0.0184801, 0.0190324, 0.0 , 0.0404626, 0.0268028, },
				{0.0224593, 0.0283378, 0.0 , 0.0445854, 0.0443027, },
				{0.0208414, 0.0288179, 0.0 , 0.0351135, 0.0568415, },
				{0.0158483, 0.020791, 0.0 , 0.0325392, 0.0417285, }		};


				static double ff_u_w1_3[15][5]    = {{0.0136379, 0.0155638, 0.0 , 0.012716, 0.0320737},
				{0.00204968, 0.00303531, 0.0 , 0.0244361, 0.0366581},
				{-0.00676537, 0.000777006, 0.0 , 0.000747323, 0.0253378},
				{-0.00772774, 7.48038e-05, 0.0 , 0.00827038, 0.0270193},
				{-0.00787061, 0.000850856, 0.0 , 0.00294745, 0.0216159},
				{-0.0120199, 0.0043875, 0.0 , 0.00579137, 0.0238544},
				{-0.00188982, 0.00736141, 0.0 , 0.016321, 0.0300658},
				{-0.00829059, -8.11219e-05, 0.0 , 0.00291854, 0.0219118},
				{-0.00468576, 0.00120503, 0.0 , 0.00921905, 0.0216106},
				{-0.00460905, 0.00834817, 0.0 , 0.0116231, 0.0216767},
				{-0.0100316, 0.0101452, 0.0 , 0.00946182, 0.0268877},
				{-0.00275654, 0.00606692, 0.0 , 0.0235783, 0.0217086},
				{-0.004094, 0.0078333, 0.0 , -0.00228995, 0.0184399},
				{-0.00590003, 0.00446522, 0.0 , 0.0168156, 0.0408084},
				{0.000677764, 0.0129141, 0.0 , 0.0106578, 0.0347177}		};


				static double fferr_u_w1_3[15][5]    = {{0.0012536, 0.00115623, 0.0 , 0.00165515, 0.00114271, },
				{0.00414918, 0.00502814, 0.0 , 0.00792667, 0.00532276, },
				{0.00113201, 0.00113136, 0.0 , 0.0019295, 0.00158859, },
				{0.00162235, 0.00172404, 0.0 , 0.00310012, 0.00240164, },
				{0.00244294, 0.00255574, 0.0 , 0.00467862, 0.00325524, },
				{0.00352846, 0.00308434, 0.0 , 0.0064771, 0.00448539, },
				{0.00438316, 0.00428891, 0.0 , 0.00715269, 0.00566297, },
				{0.00185722, 0.0017724, 0.0 , 0.00361625, 0.00286477, },
				{0.00233346, 0.00213631, 0.0 , 0.00442403, 0.00291483, },
				{0.00300877, 0.00287435, 0.0 , 0.00817747, 0.00448715, },
				{0.00371521, 0.003057, 0.0 , 0.0058576, 0.00521479, },
				{0.00371055, 0.00321547, 0.0 , 0.0077272, 0.00639899, },
				{0.00412045, 0.00522906, 0.0 , 0.00903935, 0.00910081, },
				{0.0037305, 0.00508423, 0.0 , 0.00819605, 0.00697392, },
				{0.00292271, 0.00340725, 0.0 , 0.00727795, 0.0083103, }		};


				static double ff_u_deltae_3[15][5]    = {{-1.5186, -1.20436, 0.0 , -3.0015, -4.41809},
				{2.07448, -2.7122, 0.0 , 9.17619, -5.46803},
				{-4.85172, -5.64693, 0.0 , -3.47467, -3.67115},
				{-2.49579, -3.03971, 0.0 , -3.74915, -4.49282},
				{-2.24946, -2.64749, 0.0 , -4.47965, -3.72329},
				{-2.99612, -4.17949, 0.0 , -6.14468, -2.346},
				{-3.64632, -1.73242, 0.0 , 5.79295, -4.28052},
				{-1.26716, -2.20135, 0.0 , -1.11192, -5.22998},
				{0.776773, -3.54403, 0.0 , 1.06126, 1.61638},
				{0.021574, -0.189642, 0.0 , 6.90584, 1.06955},
				{-2.54803, -3.2086, 0.0 , -2.88672, 1.61649},
				{0.775471, 1.09282, 0.0 , 3.34643, 1.49463},
				{-3.13636, -2.35677, 0.0 , -2.85831, 3.66275},
				{-3.2891, -5.81736, 0.0 , 6.11597, -2.25544},
				{-3.58897, -1.70168, 0.0 , 1.75497, -0.544491}		};


				static double fferr_u_deltae_3[15][5]    = {{0.365858, 0.286954, 0.0 , 0.789188, 0.551626, },
				{2.71588, 3.71624, 0.0 , 3.94495, 4.30196, },
				{0.812176, 0.754009, 0.0 , 1.33529, 0.90444, },
				{1.06709, 0.975946, 0.0 , 2.09391, 1.47636, },
				{1.31101, 1.32596, 0.0 , 2.91547, 1.87518, },
				{2.03092, 1.92627, 0.0 , 4.39648, 2.40015, },
				{2.25163, 2.09606, 0.0 , 3.44381, 3.62691, },
				{0.894944, 0.883078, 0.0 , 2.43151, 1.86287, },
				{1.02311, 1.15507, 0.0 , 2.53951, 2.11372, },
				{1.35938, 1.40539, 0.0 , 3.00261, 3.70812, },
				{1.69042, 1.68115, 0.0 , 3.98145, 2.49984, },
				{1.60826, 1.3727, 0.0 , 4.09619, 3.05058, },
				{2.18018, 2.6046, 0.0 , 8.17725, 3.66559, },
				{2.19813, 2.99219, 0.0 , 5.64141, 5.278, },
				{1.81521, 1.62424, 0.0 , 4.00211, 4.97346, }		};


				static double ff_u_eratio_3[15][5]    = {{-0.000681877, -0.00593126, 0.0 , -0.00516695, -0.00541687},
				{-0.0136097, -0.0156276, 0.0 , -0.0362453, -0.00955629},
				{0.00991696, 0.016761, 0.0 , 0.0177947, 0.00307733},
				{0.00762695, 0.00696719, 0.0 , 0.00781614, 0.002967},
				{-0.000151396, 0.00916338, 0.0 , 0.0109638, 0.00184762},
				{0.00841123, -0.000249326, 0.0 , 0.0165241, -0.00139987},
				{0.0016523, -0.00239831, 0.0 , 0.00929463, 0.000241578},
				{-0.000191212, 0.00150323, 0.0 , 0.00898391, 0.000947475},
				{0.000544012, 0.0049262, 0.0 , 0.00918645, -0.00170803},
				{-0.00292867, -0.00351191, 0.0 , 0.000484049, -0.00160158},
				{0.0021984, -0.00276649, 0.0 , -0.00929433, 0.000717819},
				{-0.000160813, -0.00325441, 0.0 , 0.00315827, -0.00113708},
				{-0.00349599, -0.00254589, 0.0 , -0.00809735, -0.00385445},
				{-0.00406927, -0.00886375, 0.0 , -0.000684321, -0.00191712},
				{-0.000897169, -0.00366962, 0.0 , -0.00477821, 0.00217783}		};


				static double fferr_u_eratio_3[15][5]    = {{0.00156458, 0.00141648, 0.0 , 0.00209833, 0.00115884, },
				{0.00847969, 0.00928462, 0.0 , 0.00386186, 0.00300213, },
				{0.00203181, 0.00218119, 0.0 , 0.00409203, 0.00129453, },
				{0.00272415, 0.00289405, 0.0 , 0.00516845, 0.0018377, },
				{0.00307078, 0.00406006, 0.0 , 0.00874067, 0.00208042, },
				{0.00455497, 0.00329128, 0.0 , 0.0122594, 0.00249178, },
				{0.00440804, 0.0046917, 0.0 , 0.008392, 0.002122, },
				{0.00188844, 0.00225946, 0.0 , 0.00532416, 0.00169064, },
				{0.00223728, 0.00351195, 0.0 , 0.00733047, 0.00169748, },
				{0.00214043, 0.00278513, 0.0 , 0.00463069, 0.00150185, },
				{0.00439783, 0.0029353, 0.0 , 0.0023804, 0.00188424, },
				{0.00263711, 0.00248167, 0.0 , 0.0127589, 0.00159181, },
				{0.0031686, 0.0036045, 0.0 , 0.00217773, 0.00143457, },
				{0.00236656, 0.00338153, 0.0 , 0.00336982, 0.00268969, },
				{0.00291543, 0.00279024, 0.0 , 0.00242859, 0.00233859, }		};


				if(isConv){
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad1_3[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rhad_3[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_e277_3[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_reta_3[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_rphi_3[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_weta2_3[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_f1_3[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_fside_3[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_wtot_3[0][0] ,  0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_w1_3[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_deltae_3[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_c_eratio_3[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad1_3[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rhad_3[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_e277_3[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_reta_3[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_rphi_3[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_weta2_3[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_f1_3[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_fside_3[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_wtot_3[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_w1_3[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_deltae_3[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_c_eratio_3[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad1_3[0][0],  0. );
				m_rhad_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rhad_3[0][0],   0. );
				m_e277_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_e277_3[0][0],   0. );
				m_reta_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_reta_3[0][0],   0. );
				m_rphi_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_rphi_3[0][0],   0. );
				m_weta2_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_weta2_3[0][0],  0. );
				m_f1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_f1_3[0][0],     0. );
				m_fside_ff.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_fside_3[0][0],  0. );
				m_wtot_ff.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_wtot_3[0][0],   0. );
				m_w1_ff.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_w1_3[0][0],     0. );
				m_deltae_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_deltae_3[0][0], 0. );
				m_eratio_ff.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &ff_u_eratio_3[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad1_3[0][0],  0. );
				m_rhad_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rhad_3[0][0],   0. );
				m_e277_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_e277_3[0][0],   0. );
				m_reta_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_reta_3[0][0],   0. );
				m_rphi_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_rphi_3[0][0],   0. );
				m_weta2_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_weta2_3[0][0],  0. );
				m_f1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_f1_3[0][0],     0. );
				m_fside_fferr.Initialize  ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_fside_3[0][0],  0. );
				m_wtot_fferr.Initialize   ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_wtot_3[0][0],   0. );
				m_w1_fferr.Initialize     ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_w1_3[0][0],     0. );
				m_deltae_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_deltae_3[0][0], 0. );
				m_eratio_fferr.Initialize ( 15, 5, pt_DVs_0, eta_DVs_0, &fferr_u_eratio_3[0][0], 0. );
				}

				break;

		case 4:  //GEO-16-11-00 for material budget systematics.
				// FFs obtained according to last id re-optimization (i.e. tight tune 5 in PhotonIDTool)
				// rel16 Data periods A-I
				// JF17,35,70,140,240 MC10 GEO-16-11-00
				// 'Iso' = EtCone40_corrected < 3GeV

				static double pt_DVs_4[13] = {25000,30000,35000,40000,45000,55000,70000,
				85000,100000,125000,150000,200000,400000};// in MeV

				//converted photons

				static double ff_c_rhad1_4[13][5]    = {{-0.000194094, 0.000684356, 0.0 , -0.000459693, -0.000231044},
				{-0.000112597, -0.0014968, 0.0 , -0.000158426, -0.000570195},
				{0.000363243, -0.00313486, 0.0 , 0.00161138, 0.000333004},
				{-0.000647188, -0.000105756, 0.0 , 6.63561e-05, -1.45886e-05},
				{0.00142345, 0.000278758, 0.0 , 0.000278109, -0.00217562},
				{-0.000342056, -0.00086599, 0.0 , -4.3685e-05, 0.000737476},
				{-0.000388636, -0.000132209, 0.0 , -0.000138969, -9.42192e-05},
				{-2.46973e-05, -0.00045022, 0.0 , 0.00147313, 0.000104827},
				{0.000178284, 0.000109069, 0.0 , 0.000472219, 0.000405198},
				{0.000329025, 0.000504275, 0.0 , -0.000685636, 4.53237e-05},
				{8.06258e-06, 0.000529497, 0.0 , 0.000507846, -2.45136e-05},
				{-0.000145413, 0.000623381, 0.0 , 0.000471868, -0.000225951},
				{-0.00030722, -6.27701e-05, 0.0 , -0.00034744, 0.000100327}};


				static double fferr_c_rhad1_4[13][5]    = {{0.000427759, 0.000586882, 0.0 , 0.00105208, 0.000489775, },
				{0.000710335, 0.00199081, 0.0 , 0.00137279, 0.000959132, },
				{0.000482202, 0.00258255, 0.0 , 0.00181771, 0.00100541, },
				{0.000932327, 0.000829135, 0.0 , 0.00218579, 0.000492689, },
				{0.000900667, 0.000604382, 0.0 , 0.00148613, 0.00119987, },
				{0.000300667, 0.000753557, 0.0 , 0.000608223, 0.000232192, },
				{0.000469392, 0.000473747, 0.0 , 0.000652827, 0.00038681, },
				{0.000518649, 0.000499158, 0.0 , 0.000547527, 0.000244986, },
				{0.000247628, 0.000152836, 0.0 , 0.000410821, 0.000186907, },
				{0.00015302, 0.000229937, 0.0 , 0.000505862, 0.00039329, },
				{0.000347703, 0.000237145, 0.0 , 0.000504417, 0.000509358, },
				{0.000412339, 0.000224137, 0.0 , 0.000535148, 0.000255228, },
				{0.000162205, 7.74864e-05, 0.0 , 0.000311208, 0.000206426, }};


				static double ff_c_rhad_4[13][5]    = {{0, -0.00059608, 0.0 , 0, 0},
				{0, -0.000100026, 0.0 , 0, 0},
				{0, 3.51536e-05, 0.0 , 0, 0},
				{0, -0.00132271, 0.0 , 0, 0},
				{0, -7.56562e-05, 0.0 , 0, 0},
				{0, 0.000183906, 0.0 , 0, 0},
				{0, 0.000407492, 0.0 , 0, 0},
				{0, -0.000600657, 0.0 , 0, 0},
				{0, -0.000138586, 0.0 , 0, 0},
				{0, -8.5446e-05, 0.0 , 0, 0},
				{0, 6.75385e-06, 0.0 , 0, 0},
				{0, 6.69361e-05, 0.0 , 0, 0},
				{0, -2.73874e-05, 0.0 , 0, 0}};


				static double fferr_c_rhad_4[13][5]    = {{0, 0.000475825, 0.0 , 0, 0, },
				{0, 0.000479918, 0.0 , 0, 0, },
				{0, 0.000404311, 0.0 , 0, 0, },
				{0, 0.00106246, 0.0 , 0, 0, },
				{0, 0.000453439, 0.0 , 0, 0, },
				{0, 0.000176507, 0.0 , 0, 0, },
				{0, 0.000173557, 0.0 , 0, 0, },
				{0, 0.000599056, 0.0 , 0, 0, },
				{0, 0.000237951, 0.0 , 0, 0, },
				{0, 0.000147324, 0.0 , 0, 0, },
				{0, 0.000116308, 0.0 , 0, 0, },
				{0, 0.000131164, 0.0 , 0, 0, },
				{0, 5.52633e-05, 0.0 , 0, 0, }};


				static double ff_c_e277_4[13][5]    = {{841.254, 1403.14, 0.0 , 1525.78, 1245.2},
				{-287.318, -89.5703, 0.0 , 3236.37, 1379.8},
				{199.455, 254.805, 0.0 , 5586.75, 1456.09},
				{717.869, 1590.08, 0.0 , 3731.79, 7001.89},
				{490.336, 4182.7, 0.0 , -1538.72, 7871.82},
				{-203.688, 1334.73, 0.0 , 7028.35, 5266.61},
				{236.336, 2303.05, 0.0 , 10493.1, 7792.31},
				{1473.38, 192.453, 0.0 , 12980.7, 1746.95},
				{3800.27, 5032.41, 0.0 , 6422.11, 744.109},
				{-539.57, 2692.12, 0.0 , 25641.6, 4726.88},
				{3612.8, 9495.11, 0.0 , -4379.31, 290754},
				{-483.656, 4508.02, 0.0 , -22260.5, 0},
				{-35801.8, -11348.6, 0.0 , 32218.1, 0}};


				static double fferr_c_e277_4[13][5]    = {{302.607, 338.517, 0.0 , 1035.47, 1121.95, },
				{410.034, 624.872, 0.0 , 2206.04, 2121.57, },
				{604.507, 1028.62, 0.0 , 3774.45, 2800.05, },
				{1210.55, 1044.5, 0.0 , 3993.94, 3117.48, },
				{1515.45, 1166.8, 0.0 , 4132.45, 4948.01, },
				{657.299, 1013.55, 0.0 , 3531.76, 2830.01, },
				{886.249, 1472.53, 0.0 , 4187.89, 4297.97, },
				{1618.72, 2519.75, 0.0 , 10160.8, 8655.49, },
				{1404.05, 2150.96, 0.0 , 5730.66, 4063.03, },
				{2145.16, 2938.47, 0.0 , 11944.9, 5160.87, },
				{2832.9, 4963.93, 0.0 , 16856.9, 2382.44, },
				{3102.07, 7687.51, 0.0 , 11518.6, 0, },
				{5263.99, 5390.59, 0.0 , 14742.4, 0, }};


				static double ff_c_reta_4[13][5]    = {{-0.00261521, -0.00445491, 0.0 , -0.0111419, -0.0100486},
				{-0.00378722, -0.00781864, 0.0 , -0.00715601, -0.0124694},
				{-0.00386488, -0.00496709, 0.0 , -0.00164938, -0.0094223},
				{-0.00795817, -0.00849169, 0.0 , -0.0088405, -0.00980741},
				{-0.00204003, -0.00599879, 0.0 , -0.00977558, -0.0164798},
				{-0.00395268, -0.00709569, 0.0 , -0.00868297, -0.00716835},
				{-0.00493807, -0.00595254, 0.0 , -0.00784272, -0.0112062},
				{-0.00477028, -0.00824082, 0.0 , -0.00904989, -0.00789499},
				{-0.00316405, -0.00716591, 0.0 , -0.00825012, -0.00883788},
				{-0.00474358, -0.00747859, 0.0 , -0.00647545, -0.0103214},
				{-0.00335735, -0.00712895, 0.0 , -0.0089522, -0.01186},
				{-0.00751376, -0.00519204, 0.0 , -0.00699109, -0.0096339},
				{-0.00417322, -0.00757527, 0.0 , -0.00918227, -0.00972348}};


				static double fferr_c_reta_4[13][5]    = {{0.00134256, 0.000921035, 0.0 , 0.00141731, 0.00108508, },
				{0.00154108, 0.00106187, 0.0 , 0.00193165, 0.00135184, },
				{0.0020563, 0.00169645, 0.0 , 0.00312106, 0.00145002, },
				{0.00197044, 0.00137094, 0.0 , 0.00229843, 0.00173667, },
				{0.00311933, 0.00140419, 0.0 , 0.00195996, 0.00169359, },
				{0.00107938, 0.00093628, 0.0 , 0.00135578, 0.00103353, },
				{0.00121282, 0.000889055, 0.0 , 0.00184345, 0.00108702, },
				{0.00106584, 0.000956685, 0.0 , 0.00117415, 0.00155939, },
				{0.000862084, 0.00068454, 0.0 , 0.00122684, 0.00105689, },
				{0.000932732, 0.000702899, 0.0 , 0.0013843, 0.00144847, },
				{0.00172885, 0.00107846, 0.0 , 0.00186771, 0.00219246, },
				{0.0010487, 0.00114631, 0.0 , 0.00153352, 0.00232394, },
				{0.000658908, 0.000771316, 0.0 , 0.00121825, 0.00143174, }};


				static double ff_c_rphi_4[13][5]    = {{-0.0187027, -0.00356758, 0.0 , -0.0191002, -0.020074},
				{-0.020129, -0.0170658, 0.0 , -0.00671154, -0.0109594},
				{-0.00142074, -0.00254929, 0.0 , 0.00504637, -0.00714099},
				{-0.0408013, -0.0152787, 0.0 , -0.0270395, -0.00601304},
				{-0.0166934, -0.00994956, 0.0 , -0.0131699, -0.0144727},
				{-0.011593, -0.0139781, 0.0 , 0.0030992, -0.00357491},
				{-0.00489074, -0.00138974, 0.0 , 0.000178158, -0.00555909},
				{0.00765365, -0.00688016, 0.0 , -0.000447214, -0.00567853},
				{-0.00642723, -0.00428754, 0.0 , -0.00506884, -0.00622225},
				{-0.00489944, -0.00424963, 0.0 , 0.00239658, -0.00515252},
				{0.00271022, -0.00476754, 0.0 , -0.00299114, -0.00722587},
				{-0.00455463, 0.00174183, 0.0 , -0.00224602, -0.001647},
				{-0.00514227, -0.00513154, 0.0 , -0.00563633, -0.00572997}};


				static double fferr_c_rphi_4[13][5]    = {{0.00907444, 0.00586076, 0.0 , 0.00650124, 0.00447715, },
				{0.0109297, 0.0068193, 0.0 , 0.00839561, 0.00477244, },
				{0.0141683, 0.0110471, 0.0 , 0.0134446, 0.00548914, },
				{0.0124286, 0.00906134, 0.0 , 0.00495815, 0.00492158, },
				{0.0163425, 0.00653592, 0.0 , 0.0050826, 0.00551434, },
				{0.00612389, 0.00381693, 0.0 , 0.00540485, 0.00205007, },
				{0.00587272, 0.00421416, 0.0 , 0.00408191, 0.00248337, },
				{0.00874186, 0.00499088, 0.0 , 0.00392743, 0.00204721, },
				{0.00331608, 0.00230675, 0.0 , 0.00193224, 0.00130104, },
				{0.00219106, 0.00220875, 0.0 , 0.00316366, 0.00155991, },
				{0.00512732, 0.00276986, 0.0 , 0.00310793, 0.00246861, },
				{0.00240205, 0.00335715, 0.0 , 0.00221975, 0.00304678, },
				{0.00125866, 0.00124812, 0.0 , 0.0018955, 0.00156603, }};

				static double ff_c_weta2_4[13][5]    = {{0.000125042, 0.000199883, 0.0 , 0.00043709, 0.000445378},
				{0.000398602, 0.000330696, 0.0 , 0.000326232, 0.000517326},
				{0.000241386, 0.000201643, 0.0 , 0.000387256, 0.000442943},
				{0.000184621, 0.000258983, 0.0 , 0.000352508, 0.000311547},
				{0.000419253, 0.000361644, 0.0 , 0.000422589, 0.000875204},
				{0.000181842, 0.000234582, 0.0 , 0.000452068, 0.000225348},
				{0.000203473, 7.66348e-05, 0.0 , 0.000359816, 0.000473021},
				{9.31071e-05, 0.000342017, 0.0 , 0.000363346, 0.000433451},
				{0.00018088, 0.000231246, 0.0 , 0.000503893, 0.000274845},
				{0.000146575, 0.000200986, 0.0 , 0.000233235, 0.00038322},
				{0.000181889, 0.000420211, 0.0 , 0.000488195, 0.000463616},
				{0.000296561, 0.000129871, 0.0 , 0.000304434, 0.000412227},
				{0.000241711, 0.000269824, 0.0 , 0.000745862, 0.000453279}};


				static double fferr_c_weta2_4[13][5]    = {{7.64172e-05, 5.72187e-05, 0.0 , 8.13756e-05, 6.87406e-05, },
				{7.34432e-05, 7.54055e-05, 0.0 , 0.00010711, 8.56307e-05, },
				{9.13724e-05, 8.07518e-05, 0.0 , 0.000135088, 9.77934e-05, },
				{0.000109249, 8.61528e-05, 0.0 , 0.000198378, 0.00012995, },
				{0.000121962, 9.8549e-05, 0.0 , 0.000143169, 0.000152008, },
				{6.41523e-05, 6.25267e-05, 0.0 , 9.96624e-05, 7.61217e-05, },
				{6.2437e-05, 5.66804e-05, 0.0 , 8.36152e-05, 8.16153e-05, },
				{8.63268e-05, 7.93585e-05, 0.0 , 0.000151172, 0.000130128, },
				{6.28114e-05, 5.61741e-05, 0.0 , 7.09174e-05, 8.54813e-05, },
				{5.78909e-05, 5.97848e-05, 0.0 , 0.000124849, 0.000108867, },
				{7.13118e-05, 6.73268e-05, 0.0 , 0.000168174, 0.000158575, },
				{7.29852e-05, 0.000104449, 0.0 , 0.000149175, 0.000167014, },
				{4.86226e-05, 4.99177e-05, 0.0 , 0.000115918, 0.000123685, }};


				static double ff_c_f1_4[13][5]    = {{-0.0244121, -0.0101453, 0.0 , -0.00116253, -0.00526518},
				{0.00432557, 0.0124178, 0.0 , 0.00804847, 0.00321364},
				{-0.0151716, 0.00894402, 0.0 , -0.0183972, -0.00197858},
				{-0.00966477, -0.00699684, 0.0 , 0.0034264, -0.0261639},
				{-0.00202298, -0.0304673, 0.0 , 0.0105085, 0.0159358},
				{-0.00407875, -0.00372678, 0.0 , -0.0115964, -0.0374064},
				{-0.00650945, -0.0231505, 0.0 , -0.0113047, -0.00868899},
				{-0.0186714, 0.00175594, 0.0 , -0.0323077, -0.00661971},
				{-0.0288494, -0.0238225, 0.0 , -0.0114211, -0.0116067},
				{-0.0157694, -0.0138002, 0.0 , -0.0227483, -0.0180271},
				{-0.0278233, -0.0115962, 0.0 , -0.0123112, 0.0250941},
				{0.00418088, -0.0234537, 0.0 , -0.0109585, -0.0297834},
				{-0.00699799, 0.00476234, 0.0 , 0.00366259, -0.0177025}};


				static double fferr_c_f1_4[13][5]    = {{0.00929283, 0.00573202, 0.0 , 0.0075101, 0.00760084, },
				{0.0122019, 0.00872579, 0.0 , 0.0100355, 0.00949168, },
				{0.0133303, 0.00879288, 0.0 , 0.0193458, 0.0141034, },
				{0.0279235, 0.0110851, 0.0 , 0.0149218, 0.0144725, },
				{0.0258188, 0.0115109, 0.0 , 0.0133885, 0.0141759, },
				{0.0101538, 0.00765368, 0.0 , 0.00931346, 0.00714228, },
				{0.0097246, 0.0083907, 0.0 , 0.00697912, 0.00806087, },
				{0.0125907, 0.0112104, 0.0 , 0.0142736, 0.0118119, },
				{0.0120031, 0.00859515, 0.0 , 0.00989841, 0.00937303, },
				{0.010477, 0.00810587, 0.0 , 0.0107684, 0.0102046, },
				{0.0177301, 0.0122562, 0.0 , 0.0130697, 0.0141083, },
				{0.0108938, 0.00917643, 0.0 , 0.0133992, 0.0178929, },
				{0.00900643, 0.00748167, 0.0 , 0.0145674, 0.0117059, }};

				static double ff_c_fside_4[13][5]    = {{-0.000137717, 0.00536677, 0.0 , 0.0149308, 0.00696801},
				{0.0128373, 0.0105803, 0.0 , -0.00273272, 0.00396571},
				{0.00877139, 0.00802234, 0.0 , -0.0223366, 0.0189363},
				{-0.000789732, 0.0146551, 0.0 , -0.00432152, 0.0159271},
				{0.0361533, 0.0250032, 0.0 , 0.0207596, 0.00576197},
				{0.0075521, 0.00324383, 0.0 , 0.00546885, 0.0111865},
				{0.000595093, -0.00687337, 0.0 , 0.00524685, 0.0111976},
				{-0.00669767, 0.0189899, 0.0 , 0.00477329, 0.013886},
				{-0.0102871, 0.00709611, 0.0 , 0.0167431, 0.0160069},
				{0.00522776, 0.0101769, 0.0 , -0.00407916, 0.0196654},
				{-5.82337e-05, 0.0152604, 0.0 , 0.0163526, 0.0224993},
				{0.00690146, -0.00672308, 0.0 , 0.0195028, 0.00643137},
				{0.00325039, 0.00901967, 0.0 , 0.0290225, 0.0166997}};


				static double fferr_c_fside_4[13][5]    = {{0.00434725, 0.00576028, 0.0 , 0.00976982, 0.00269655, },
				{0.00568334, 0.00852853, 0.0 , 0.0124295, 0.00341904, },
				{0.00751245, 0.0101654, 0.0 , 0.0200511, 0.00483863, },
				{0.00779939, 0.0105016, 0.0 , 0.0230182, 0.00559111, },
				{0.0146336, 0.0136166, 0.0 , 0.0190391, 0.00757048, },
				{0.00472562, 0.00657277, 0.0 , 0.0105896, 0.00322255, },
				{0.00565419, 0.00646615, 0.0 , 0.0113901, 0.00394388, },
				{0.00752172, 0.00877073, 0.0 , 0.0146933, 0.00503727, },
				{0.00638171, 0.0067415, 0.0 , 0.00884279, 0.00339667, },
				{0.00473539, 0.00628931, 0.0 , 0.0166792, 0.00412033, },
				{0.00814282, 0.0106624, 0.0 , 0.0179556, 0.00717146, },
				{0.00822836, 0.0105891, 0.0 , 0.0184707, 0.00758919, },
				{0.00392892, 0.00607169, 0.0 , 0.00988573, 0.00654025, }};


				static double ff_c_wtot_4[13][5]    = {{0.0354593, 0.0213006, 0.0 , 0.0830808, 0.0370271},
				{0.0982283, 0.0580826, 0.0 , 0.00624204, -0.0012697},
				{0.11879, 0.0615978, 0.0 , 0.00385737, 0.0684385},
				{0.0648412, 0.0425518, 0.0 , 0.0379741, 0.0538828},
				{-0.00260103, 0.0824921, 0.0 , 0.0896199, -0.0343066},
				{0.0848137, 0.0891182, 0.0 , 0.0694406, 0.0735081},
				{0.0551693, 0.0165219, 0.0 , 0.0314407, 0.0312277},
				{0.0799366, 0.070893, 0.0 , 0.0210791, 0.0721128},
				{0.0523902, 0.0588164, 0.0 , 0.0683913, 0.112697},
				{0.067426, 0.0764353, 0.0 , 0.0170217, 0.0919634},
				{0.0337985, 0.106378, 0.0 , 0.0412314, 0.0891304},
				{0.109653, 0.0260267, 0.0 , 0.0782938, 0.0382005},
				{0.0957135, 0.0904338, 0.0 , 0.134622, 0.060735}};


				static double fferr_c_wtot_4[13][5]    = {{0.0290539, 0.0217231, 0.0 , 0.0370932, 0.0193953, },
				{0.0258625, 0.0319313, 0.0 , 0.0499441, 0.0256351, },
				{0.030843, 0.0417402, 0.0 , 0.0580566, 0.0307586, },
				{0.0561991, 0.0382854, 0.0 , 0.072284, 0.0359564, },
				{0.0666363, 0.0521848, 0.0 , 0.0711471, 0.0556985, },
				{0.0196214, 0.0264445, 0.0 , 0.0338751, 0.0214983, },
				{0.0237012, 0.0244218, 0.0 , 0.0378006, 0.026067, },
				{0.0341809, 0.0362639, 0.0 , 0.0550446, 0.0348966, },
				{0.0245743, 0.0282792, 0.0 , 0.0378237, 0.0248413, },
				{0.0218673, 0.0271791, 0.0 , 0.058563, 0.0307638, },
				{0.0263612, 0.0465326, 0.0 , 0.0838052, 0.0564597, },
				{0.0256852, 0.0520271, 0.0 , 0.0835738, 0.0547245, },
				{0.0165549, 0.0295458, 0.0 , 0.0409059, 0.0518013, }};


				static double ff_c_w1_4[13][5]    = {{-0.00539434, 0.00474584, 0.0 , 0.00432897, 0.0220444},
				{-0.000967503, 0.0118975, 0.0 , -0.00304103, 0.0311043},
				{0.00702035, 0.00738293, 0.0 , -0.023491, 0.0290164},
				{-0.00269717, 0.00723219, 0.0 , -0.00551897, 0.0224043},
				{-0.000648379, 0.0101829, 0.0 , 0.0115772, 0.009251},
				{-0.00619692, 0.00361556, 0.0 , 0.00250083, 0.0228696},
				{-0.000879347, -0.00383121, 0.0 , -0.00517166, 0.028874},
				{-0.00258404, 0.0117447, 0.0 , -0.00494283, 0.0245856},
				{-0.00628269, 0.00642532, 0.0 , -0.000480473, 0.0236345},
				{-0.00224978, 0.00586504, 0.0 , -0.0102174, 0.0151552},
				{-0.0167121, 3.016e-05, 0.0 , -0.00383395, 0.0337637},
				{-0.00674993, -0.00898415, 0.0 , -0.0019722, 0.0190552},
				{-0.00423819, 0.00761122, 0.0 , 0.016261, 0.0253166}};


				static double fferr_c_w1_4[13][5]    = {{0.00391297, 0.00355641, 0.0 , 0.0046636, 0.00425321, },
				{0.00470493, 0.00526811, 0.0 , 0.00559842, 0.00557153, },
				{0.00591513, 0.00675055, 0.0 , 0.00935049, 0.00759687, },
				{0.0080222, 0.00563048, 0.0 , 0.00747793, 0.00671653, },
				{0.0079297, 0.00760093, 0.0 , 0.00994786, 0.0116161, },
				{0.00372642, 0.00394737, 0.0 , 0.00618555, 0.00450854, },
				{0.00469527, 0.0036858, 0.0 , 0.00562756, 0.00598036, },
				{0.00661956, 0.00570913, 0.0 , 0.00435926, 0.00595883, },
				{0.00604962, 0.00397023, 0.0 , 0.00600517, 0.00579196, },
				{0.00494187, 0.00448361, 0.0 , 0.00856937, 0.00560829, },
				{0.0101719, 0.00598443, 0.0 , 0.0100895, 0.0103716, },
				{0.00524328, 0.00767734, 0.0 , 0.0100093, 0.0111269, },
				{0.00409337, 0.00441003, 0.0 , 0.00588102, 0.0070839, }};


				static double ff_c_deltae_4[13][5]    = {{-1.55036, -2.058, 0.0 , -1.24297, -1.89743},
				{0.749146, -0.39473, 0.0 , 7.33358, -7.84436},
				{-1.46451, -4.27367, 0.0 , 11.3825, -1.94902},
				{-4.22851, -5.68394, 0.0 , -5.16912, -1.72497},
				{-4.25731, -6.51362, 0.0 , -4.94249, 0.0614452},
				{-2.68079, -2.57609, 0.0 , -10.3696, -0.687742},
				{-2.39935, -4.2919, 0.0 , -2.64204, -0.102131},
				{6.68656, -1.80087, 0.0 , 1.74173, -3.07211},
				{-3.89986, -0.873552, 0.0 , 1.33525, 4.05128},
				{1.46565, -0.463692, 0.0 , -3.53312, -2.82073},
				{-8.32388, -0.891716, 0.0 , 7.1904, -0.934849},
				{0.242676, 0.538734, 0.0 , -12.0969, 4.86268},
				{2.49491, -2.03771, 0.0 , 1.94604, -1.12576}};


				static double fferr_c_deltae_4[13][5]    = {{2.26913, 1.55091, 0.0 , 2.8922, 2.08298, },
				{2.4467, 1.82902, 0.0 , 3.39349, 2.77709, },
				{3.30259, 3.10602, 0.0 , 4.805, 3.35509, },
				{5.60808, 2.70539, 0.0 , 9.31752, 3.52367, },
				{11.2171, 3.60747, 0.0 , 6.11204, 4.01223, },
				{3.05935, 2.07149, 0.0 , 4.52868, 2.78357, },
				{2.94879, 2.2184, 0.0 , 4.4251, 2.78257, },
				{4.55094, 2.99469, 0.0 , 5.80569, 4.73148, },
				{2.64935, 2.15955, 0.0 , 3.91662, 2.99598, },
				{3.63674, 2.6133, 0.0 , 6.29413, 3.72503, },
				{5.61843, 3.74976, 0.0 , 8.64431, 5.41615, },
				{3.57446, 5.06906, 0.0 , 9.43193, 4.36192, },
				{3.25157, 2.7664, 0.0 , 5.66109, 4.73307, }};

				static double ff_c_eratio_4[13][5]    = {{-0.0036782, -0.00337279, 0.0 , -0.00549829, 0.000766158},
				{-0.00483054, -0.00122523, 0.0 , 0.00341827, 0.00295752},
				{-0.0043602, 0.00092572, 0.0 , 0.00938416, -0.000142634},
				{-0.00393575, 0.00231916, 0.0 , 0.00357878, -0.000254095},
				{0.00146437, -0.00496912, 0.0 , 0.0024991, 0.00153267},
				{-0.000133693, -0.00110394, 0.0 , -0.00268006, -0.00122219},
				{-0.00189912, -0.00239462, 0.0 , -0.000317633, -0.00109309},
				{-0.00434613, -0.000315249, 0.0 , -0.00768089, 0.00136864},
				{-0.000906765, -0.00303608, 0.0 , -0.00426322, -0.000803828},
				{-0.004933, -0.00259393, 0.0 , 4.8399e-05, -0.00194561},
				{6.04391e-05, 0.00135088, 0.0 , -0.00239778, -0.00130326},
				{-0.000712335, -0.00638628, 0.0 , -0.00328207, 0.000605047},
				{-0.00367612, -0.00098747, 0.0 , 0.000862241, -0.000408888}};


				static double fferr_c_eratio_4[13][5]    = {{0.00213762, 0.00271328, 0.0 , 0.00478182, 0.00107845, },
				{0.00246336, 0.00347222, 0.0 , 0.00576292, 0.00129379, },
				{0.00278826, 0.00532074, 0.0 , 0.00880849, 0.0013095, },
				{0.00389914, 0.00550397, 0.0 , 0.00584426, 0.00148784, },
				{0.0039486, 0.00607171, 0.0 , 0.00606565, 0.00218097, },
				{0.00229315, 0.00300137, 0.0 , 0.00288981, 0.000989427, },
				{0.00200491, 0.00201221, 0.0 , 0.00284844, 0.000686741, },
				{0.0040926, 0.00328536, 0.0 , 0.00235932, 0.00128101, },
				{0.00192454, 0.00186525, 0.0 , 0.00217366, 0.00106778, },
				{0.00126516, 0.00186538, 0.0 , 0.00253319, 0.00070903, },
				{0.00173412, 0.00532949, 0.0 , 0.00174091, 0.00131098, },
				{0.00196359, 0.00277772, 0.0 , 0.00152404, 0.00214857, },
				{0.00134403, 0.001605, 0.0 , 0.00321913, 0.000828842, }};


				//unconverted photons

				static double ff_u_rhad1_4[13][5]    = {{-0.000196863, -0.000645365, 0.0 , -0.00139224, 1.23766e-05},
				{-0.000240093, -0.000534716, 0.0 , 0.00144121, 7.68151e-05},
				{-0.000235269, -0.00020504, 0.0 , 6.93928e-06, -0.000186602},
				{0.000429989, -4.27233e-06, 0.0 , 0.00147129, 0.00160004},
				{0.000353882, -0.000607485, 0.0 , 0.00148967, 0.000236082},
				{-0.00045808, -5.17454e-05, 0.0 , 0.000531111, 0.000341889},
				{0.00016042, -0.000497975, 0.0 , -0.000193605, -0.000427571},
				{-0.000297625, -4.9163e-05, 0.0 , 0.000811304, 0.000476437},
				{-2.5965e-05, 0.000266607, 0.0 , 0.00037968, 0.000119548},
				{-0.00029503, 8.85547e-05, 0.0 , 0.000150884, -6.22243e-05},
				{2.31081e-05, -0.000297797, 0.0 , 0.00137491, -0.000222082},
				{-0.000343063, -5.90885e-05, 0.0 , 0.0015663, -6.46205e-05},
				{-3.76254e-06, 0.00020741, 0.0 , -0.000487367, -3.5211e-05}};


				static double fferr_u_rhad1_4[13][5]    = {{0.000339019, 0.000527318, 0.0 , 0.00129133, 0.000467977, },
				{0.000533691, 0.000889274, 0.0 , 0.00140531, 0.000525672, },
				{0.000570197, 0.000347376, 0.0 , 0.00114221, 0.000527011, },
				{0.000264329, 0.000741223, 0.0 , 0.00132277, 0.000707178, },
				{0.000284986, 0.000685049, 0.0 , 0.00085958, 0.000707047, },
				{0.000271281, 0.000198529, 0.0 , 0.000610893, 0.000342398, },
				{0.000137716, 0.000255412, 0.0 , 0.000618372, 0.000642206, },
				{0.000328174, 0.000277123, 0.0 , 0.000451243, 0.000440328, },
				{0.000182502, 0.000128767, 0.0 , 0.00106177, 0.000225556, },
				{0.000209595, 0.000115752, 0.0 , 0.000666678, 0.000249491, },
				{0.000269647, 0.000320383, 0.0 , 0.000668761, 0.000375933, },
				{0.000452191, 0.000172287, 0.0 , 0.000640993, 0.000427327, },
				{0.000163344, 0.000136818, 0.0 , 0.000548313, 0.000286087, }};


				static double ff_u_rhad_4[13][5]    = {{0, -0.000487611, 0.0 , 0, 0},
				{0, -0.0016464, 0.0 , 0, 0},
				{0, -3.643e-05, 0.0 , 0, 0},
				{0, -0.000267235, 0.0 , 0, 0},
				{0, -4.48361e-05, 0.0 , 0, 0},
				{0, 0.000118168, 0.0 , 0, 0},
				{0, -0.000151611, 0.0 , 0, 0},
				{0, 0.000239104, 0.0 , 0, 0},
				{0, -3.49234e-06, 0.0 , 0, 0},
				{0, -1.12192e-05, 0.0 , 0, 0},
				{0, 3.73432e-05, 0.0 , 0, 0},
				{0, -0.00032158, 0.0 , 0, 0},
				{0, 1.34876e-06, 0.0 , 0, 0}};


				static double fferr_u_rhad_4[13][5]    = {{0, 0.000491464, 0.0 , 0, 0, },
				{0, 0.000752946, 0.0 , 0, 0, },
				{0, 0.000483802, 0.0 , 0, 0, },
				{0, 0.000467186, 0.0 , 0, 0, },
				{0, 0.00049991, 0.0 , 0, 0, },
				{0, 0.000190907, 0.0 , 0, 0, },
				{0, 0.000315023, 0.0 , 0, 0, },
				{0, 0.000229634, 0.0 , 0, 0, },
				{0, 0.00012246, 0.0 , 0, 0, },
				{0, 0.000121043, 0.0 , 0, 0, },
				{0, 0.000135017, 0.0 , 0, 0, },
				{0, 0.000159187, 0.0 , 0, 0, },
				{0, 8.79395e-05, 0.0 , 0, 0, }};


				static double ff_u_e277_4[13][5]    = {{589.502, 606.781, 0.0 , 2967.95, 2439.54},
				{988.656, 521.682, 0.0 , -3919.27, 3465.45},
				{592.328, 2619.89, 0.0 , 343.234, 4413.81},
				{624.252, 693.109, 0.0 , -3415.8, -5015.88},
				{454.301, -347.391, 0.0 , -14233.8, -7630.22},
				{88.2305, 2644.57, 0.0 , 6037.21, 4453.17},
				{-663.016, 4839.47, 0.0 , 8416.8, 5128.36},
				{189.152, 3628.25, 0.0 , 15938.2, 5765.28},
				{1919.59, 5343.04, 0.0 , 21200.5, -934.406},
				{-1192.36, 7733.29, 0.0 , -4768.31, -9954.16},
				{2994.35, 4878.94, 0.0 , 19895.5, 0},
				{210.688, -7104.06, 0.0 , -13642.2, 0},
				{-24772.6, -12622.2, 0.0 , 0, 0}};


				static double fferr_u_e277_4[13][5]    = {{239.634, 387.546, 0.0 , 1171.22, 1196.73, },
				{386.399, 647.451, 0.0 , 2117.97, 2239.09, },
				{463.185, 967.139, 0.0 , 2697.32, 3249.51, },
				{576.242, 1097.38, 0.0 , 4803.48, 4811.6, },
				{837.503, 1475.35, 0.0 , 6587.61, 6520.17, },
				{526.345, 1064.38, 0.0 , 4880.85, 3241.85, },
				{658.609, 1671.18, 0.0 , 4166.08, 4685.17, },
				{1063.77, 2282.58, 0.0 , 10667.8, 6606.13, },
				{988.89, 1993.65, 0.0 , 7861.41, 3973.18, },
				{1345.25, 2725.29, 0.0 , 6618.63, 4543.91, },
				{1907.54, 3946.09, 0.0 , 15017.4, 0, },
				{2692.94, 5350.92, 0.0 , 11946.9, 0, },
				{3575.2, 4130.11, 0.0 , 0, 0, }};


				static double ff_u_reta_4[13][5]    = {{-0.00481641, -0.00598717, 0.0 , -0.0079065, -0.00753367},
				{-0.00356507, -0.0043363, 0.0 , -0.0124245, -0.00868338},
				{-0.00327086, -0.00705928, 0.0 , -0.00879043, -0.0099383},
				{-0.0029062, -0.00614107, 0.0 , -0.0117491, -0.0110842},
				{-0.00277829, -0.00748593, 0.0 , -0.009745, -0.0108368},
				{-0.00393116, -0.00621104, 0.0 , -0.00767291, -0.0100605},
				{-0.00431734, -0.00530636, 0.0 , -0.00895655, -0.011164},
				{-0.00448442, -0.00516862, 0.0 , -0.00676489, -0.0115171},
				{-0.00456166, -0.00642598, 0.0 , -0.00562996, -0.011945},
				{-0.00445664, -0.00712442, 0.0 , -0.0120856, -0.0112169},
				{-0.00286454, -0.00742298, 0.0 , -0.00838035, -0.0121503},
				{-0.00362468, -0.00653458, 0.0 , -0.0096063, -0.00887841},
				{-0.0037182, -0.00787276, 0.0 , -0.00901455, -0.0114813}};


				static double fferr_u_reta_4[13][5]    = {{0.00072995, 0.000654576, 0.0 , 0.00122188, 0.000779439, },
				{0.000940135, 0.00078766, 0.0 , 0.00124055, 0.00120547, },
				{0.000917404, 0.00105507, 0.0 , 0.00139167, 0.00157386, },
				{0.00109898, 0.000989583, 0.0 , 0.001255, 0.00203096, },
				{0.00115318, 0.00107124, 0.0 , 0.00383923, 0.00176077, },
				{0.000585619, 0.000614623, 0.0 , 0.00126766, 0.00110664, },
				{0.000568958, 0.000686237, 0.0 , 0.00142196, 0.00115892, },
				{0.000768102, 0.000711413, 0.0 , 0.00129044, 0.00153951, },
				{0.00050893, 0.000520139, 0.0 , 0.00140362, 0.00105694, },
				{0.000462773, 0.000551752, 0.0 , 0.000943385, 0.0010933, },
				{0.000807667, 0.000690777, 0.0 , 0.00250244, 0.00179711, },
				{0.000643455, 0.000698414, 0.0 , 0.00130842, 0.00149385, },
				{0.000392045, 0.000578863, 0.0 , 0.00133033, 0.00143321, }};


				static double ff_u_rphi_4[13][5]    = {{-0.0037511, -0.000850677, 0.0 , -0.00339299, -0.00511408},
				{-0.000994861, -0.00277174, 0.0 , -0.00509453, -0.00507969},
				{-0.00125074, -0.00579137, 0.0 , -0.00381833, -0.00665081},
				{-0.00160849, -0.00495547, 0.0 , -0.00575691, -0.00806886},
				{-0.00277507, -0.00569814, 0.0 , -0.00633675, -0.00668967},
				{-0.00260377, -0.00368893, 0.0 , -0.000880957, -0.00613326},
				{-0.00260466, -0.00441164, 0.0 , -0.00344592, -0.00543302},
				{-0.00366974, -0.00306821, 0.0 , -0.000573635, -0.00521648},
				{-0.00244409, -0.00440717, 0.0 , 0.000145078, -0.00581902},
				{-0.00280845, -0.00386184, 0.0 , -0.00358313, -0.00747496},
				{-0.00213605, -0.00307989, 0.0 , -0.0030697, -0.00788683},
				{-0.00266361, -0.00375402, 0.0 , -0.00502259, -0.00526428},
				{-0.00345111, -0.00446284, 0.0 , -0.00362176, -0.007613}};


				static double fferr_u_rphi_4[13][5]    = {{0.000661742, 0.00159228, 0.0 , 0.00124758, 0.000674247, },
				{0.000774544, 0.00128829, 0.0 , 0.0019186, 0.0010051, },
				{0.000860758, 0.00162202, 0.0 , 0.00198946, 0.00173292, },
				{0.00110839, 0.00161558, 0.0 , 0.00178886, 0.00164004, },
				{0.00104142, 0.00186177, 0.0 , 0.00304845, 0.00135396, },
				{0.000518529, 0.00111374, 0.0 , 0.00159237, 0.00093952, },
				{0.000558092, 0.000753687, 0.0 , 0.00151124, 0.00120522, },
				{0.000716845, 0.00114252, 0.0 , 0.00221546, 0.00133734, },
				{0.000536827, 0.000761943, 0.0 , 0.00173087, 0.00112226, },
				{0.000495949, 0.00078564, 0.0 , 0.00140909, 0.00103695, },
				{0.000674096, 0.00148336, 0.0 , 0.00267822, 0.00134191, },
				{0.000663057, 0.000996832, 0.0 , 0.00104374, 0.00161201, },
				{0.000470739, 0.000582785, 0.0 , 0.00126097, 0.00136509, }};


				static double ff_u_weta2_4[13][5]    = {{0.000170187, 0.000231603, 0.0 , 0.000409941, 0.00030921},
				{0.000163853, 0.000147202, 0.0 , 0.00058547, 0.000383228},
				{0.000132921, 0.000221834, 0.0 , 0.000500473, 0.000518528},
				{0.000111309, 0.000193644, 0.0 , 0.000463606, 0.000485987},
				{0.000250951, 0.000313086, 0.0 , 0.000713912, 0.000341126},
				{0.000238636, 0.000212438, 0.0 , 0.000352022, 0.000468963},
				{0.000252157, 0.000205319, 0.0 , 0.000501816, 0.000507},
				{0.000223385, 0.000201981, 0.0 , 0.000279909, 0.000465994},
				{0.000193349, 0.000271599, 0.0 , 0.000292126, 0.000542834},
				{0.000247567, 0.000270439, 0.0 , 0.000654981, 0.000542881},
				{0.000168292, 0.000318729, 0.0 , 0.000491521, 0.000674111},
				{0.000222259, 0.00021479, 0.0 , 0.000620727, 0.000378883},
				{0.000241484, 0.000317153, 0.0 , 0.000502231, 0.000438775}};


				static double fferr_u_weta2_4[13][5]    = {{4.06616e-05, 3.69919e-05, 0.0 , 5.79437e-05, 5.80132e-05, },
				{5.50349e-05, 4.61573e-05, 0.0 , 8.94283e-05, 7.89913e-05, },
				{5.65106e-05, 6.52052e-05, 0.0 , 9.7888e-05, 0.000105176, },
				{7.41485e-05, 6.24238e-05, 0.0 , 0.000157813, 0.00014269, },
				{7.13384e-05, 7.57522e-05, 0.0 , 0.000151465, 0.000124522, },
				{3.15354e-05, 3.74968e-05, 0.0 , 9.47662e-05, 7.14081e-05, },
				{3.60478e-05, 4.44941e-05, 0.0 , 8.89802e-05, 9.81448e-05, },
				{4.84424e-05, 5.03833e-05, 0.0 , 0.0001329, 0.000131685, },
				{3.16959e-05, 3.54632e-05, 0.0 , 8.6849e-05, 8.44333e-05, },
				{3.26374e-05, 4.05222e-05, 0.0 , 8.77068e-05, 9.18744e-05, },
				{4.77016e-05, 5.38862e-05, 0.0 , 0.000119919, 0.000139408, },
				{3.68083e-05, 4.41849e-05, 0.0 , 0.000110571, 0.000152836, },
				{2.65119e-05, 3.57742e-05, 0.0 , 7.5225e-05, 0.000114377, }};


				static double ff_u_f1_4[13][5]    = {{-0.0236424, -0.0142867, 0.0 , -0.0277252, -0.0254742},
				{-0.0350879, -0.0110443, 0.0 , 0.0237224, -0.0218039},
				{-0.01975, -0.0217725, 0.0 , 0.0067565, -0.0196797},
				{-0.0254806, -0.004622, 0.0 , 0.0174031, 0.0149366},
				{-0.0118318, 0.00294384, 0.0 , 0.0418237, 0.00257912},
				{-0.00850002, -0.0238469, 0.0 , -0.0341772, -0.0141584},
				{-0.00373335, -0.0364464, 0.0 , -0.0282395, -0.0138122},
				{-0.00619362, -0.0285643, 0.0 , -0.0454941, 0.00418198},
				{-0.0207521, -0.0170007, 0.0 , -0.0519859, -0.000500828},
				{0.00164823, -0.0224202, 0.0 , 0.0224995, 0.0132658},
				{-0.0244186, -0.014508, 0.0 , -0.0335737, 0.00385889},
				{-0.0138049, -0.0127615, 0.0 , -0.00828442, -0.00625581},
				{-0.000975952, 0.00371987, 0.0 , -0.0148515, 0.0137953}};


				static double fferr_u_f1_4[13][5]    = {{0.00828473, 0.00609227, 0.0 , 0.00883192, 0.00842498, },
				{0.0105063, 0.00847659, 0.0 , 0.014093, 0.0121715, },
				{0.0113928, 0.010501, 0.0 , 0.0162755, 0.0163576, },
				{0.0138761, 0.00911547, 0.0 , 0.0256993, 0.0155754, },
				{0.0165418, 0.0139415, 0.0 , 0.0361101, 0.0157865, },
				{0.00756275, 0.00756143, 0.0 , 0.0152423, 0.00986108, },
				{0.00862362, 0.00967018, 0.0 , 0.0134715, 0.013081, },
				{0.0117629, 0.0102648, 0.0 , 0.0214207, 0.0142111, },
				{0.00770123, 0.00702712, 0.0 , 0.0165804, 0.0102071, },
				{0.00834086, 0.00761762, 0.0 , 0.0170455, 0.00995869, },
				{0.0103546, 0.00870123, 0.0 , 0.0356957, 0.0141334, },
				{0.0089893, 0.00882301, 0.0 , 0.0194003, 0.0137911, },
				{0.00594611, 0.00631092, 0.0 , 0.014745, 0.0119257, }};

				static double ff_u_fside_4[13][5]    = {{0.00326048, -0.00120857, 0.0 , 0.00312334, 0.0136763},
				{-0.00263111, -0.00108889, 0.0 , 0.0416222, 0.00518788},
				{0.00241981, 0.013209, 0.0 , 0.0291235, 0.0135394},
				{0.0045599, -0.00948662, 0.0 , 0.0471239, 0.0151455},
				{0.00370504, 0.0150214, 0.0 , 0.0531728, 0.0131426},
				{-2.84314e-05, 0.00100112, 0.0 , 7.7337e-05, 0.0140137},
				{0.0078045, -0.00198177, 0.0 , 0.00212672, 0.0141424},
				{0.00447072, -0.00186402, 0.0 , -0.00843914, 0.0248621},
				{0.00140421, 0.00716659, 0.0 , 0.00135401, 0.010761},
				{0.00243045, -0.00166437, 0.0 , 0.0354158, 0.0200532},
				{-0.00400305, 0.00259732, 0.0 , 0.0161477, 0.0143307},
				{-0.0011975, 0.0050505, 0.0 , 0.0370044, 0.0197562},
				{0.00307097, 0.0122624, 0.0 , 0.0171075, 0.0169177}};


				static double fferr_u_fside_4[13][5]    = {{0.00327297, 0.00500302, 0.0 , 0.0107096, 0.00225187, },
				{0.00478254, 0.00596813, 0.0 , 0.0121708, 0.00281173, },
				{0.00439504, 0.00693976, 0.0 , 0.0121725, 0.00387154, },
				{0.00617346, 0.0106842, 0.0 , 0.0138827, 0.00594729, },
				{0.00481451, 0.008838, 0.0 , 0.021214, 0.00621515, },
				{0.00339736, 0.00504333, 0.0 , 0.0122605, 0.0030274, },
				{0.00310121, 0.00537974, 0.0 , 0.0116274, 0.00360147, },
				{0.00467776, 0.00633984, 0.0 , 0.0195425, 0.00531806, },
				{0.00295835, 0.00496106, 0.0 , 0.0117906, 0.00324172, },
				{0.00310985, 0.00508902, 0.0 , 0.00907349, 0.00389556, },
				{0.00404489, 0.00585829, 0.0 , 0.0175158, 0.0058849, },
				{0.00325314, 0.00613052, 0.0 , 0.00985374, 0.00385259, },
				{0.00262118, 0.00434342, 0.0 , 0.00739434, 0.00465515, }};


				static double ff_u_wtot_4[13][5]    = {{0.114963, 0.0468152, 0.0 , 0.0679207, 0.0960442},
				{0.0700234, 0.0528648, 0.0 , 0.19833, 0.0226682},
				{0.0706443, 0.121992, 0.0 , 0.111952, 0.0715532},
				{0.0876946, 0.0541179, 0.0 , 0.219706, 0.0773308},
				{0.0833684, 0.0935613, 0.0 , 0.198379, 0.0841671},
				{0.0706991, 0.07739, 0.0 , 0.0670935, 0.0738786},
				{0.0924555, 0.0695546, 0.0 , 0.0727909, 0.0888377},
				{0.0499886, 0.0944818, 0.0 , -0.0186384, 0.128662},
				{0.110014, 0.113489, 0.0 , 0.0251486, 0.0589154},
				{0.0456425, 0.0898235, 0.0 , 0.141598, 0.0716487},
				{0.0847936, 0.101476, 0.0 , 0.0671356, 0.0308561},
				{0.0557687, 0.0815744, 0.0 , 0.156333, 0.0933777},
				{0.0474583, 0.0604367, 0.0 , 0.11897, 0.0966601}};


				static double fferr_u_wtot_4[13][5]    = {{0.0242408, 0.0210445, 0.0 , 0.0399948, 0.01603, },
				{0.0223302, 0.0275493, 0.0 , 0.0360006, 0.0272489, },
				{0.0261043, 0.0370608, 0.0 , 0.0653711, 0.0341191, },
				{0.0388362, 0.0374984, 0.0 , 0.059284, 0.040778, },
				{0.0428735, 0.0437809, 0.0 , 0.11752, 0.0447572, },
				{0.0159374, 0.020682, 0.0 , 0.0399788, 0.0214037, },
				{0.0206123, 0.0251294, 0.0 , 0.0451711, 0.0250524, },
				{0.0299018, 0.0282099, 0.0 , 0.0586155, 0.0438641, },
				{0.0191032, 0.0221947, 0.0 , 0.0445855, 0.024642, },
				{0.0275842, 0.023867, 0.0 , 0.0405288, 0.0358297, },
				{0.0256344, 0.0306744, 0.0 , 0.076934, 0.0572383, },
				{0.0255682, 0.0334777, 0.0 , 0.0364205, 0.0472243, },
				{0.0191644, 0.0232655, 0.0 , 0.0362978, 0.0436369, }};

				static double ff_u_w1_4[13][5]    = {{-0.00292379, 0.00242525, 0.0 , -0.00606489, 0.021097},
				{-0.00979924, 0.00267231, 0.0 , 0.0118201, 0.0205213},
				{-0.00226408, 0.00620705, 0.0 , 0.00760818, 0.0241432},
				{-0.0134845, -0.00631976, 0.0 , 0.0285369, 0.0220503},
				{-0.00434816, 0.00503242, 0.0 , 0.0320955, 0.0376301},
				{-0.00329131, -0.00199014, 0.0 , -0.0083757, 0.0260112},
				{-0.00405264, -0.000221491, 0.0 , -0.00320548, 0.0185074},
				{-0.00385594, -0.00817251, 0.0 , -0.0166655, 0.0209813},
				{-0.00855899, 0.00199831, 0.0 , -0.00977767, 0.0245067},
				{-0.000350893, 0.00358754, 0.0 , 0.0126839, 0.0219801},
				{-0.00936651, 0.00432712, 0.0 , 0.00787914, 0.027944},
				{-0.00553018, -0.00168335, 0.0 , 0.0174488, 0.0408262},
				{-0.00306201, 0.00984311, 0.0 , 0.00572264, 0.0231812}};


				static double fferr_u_w1_4[13][5]    = {{0.00304605, 0.00322308, 0.0 , 0.00498608, 0.00347504, },
				{0.00361016, 0.00461858, 0.0 , 0.00683642, 0.00439757, },
				{0.00413862, 0.00529282, 0.0 , 0.00790834, 0.00625945, },
				{0.00573361, 0.00635091, 0.0 , 0.0151339, 0.00703366, },
				{0.00541832, 0.00714892, 0.0 , 0.0155953, 0.00654329, },
				{0.00274601, 0.00340573, 0.0 , 0.00751402, 0.00437238, },
				{0.00312989, 0.00394887, 0.0 , 0.00642632, 0.00470225, },
				{0.00459661, 0.0042108, 0.0 , 0.00918308, 0.00622857, },
				{0.00326709, 0.00340291, 0.0 , 0.00764452, 0.00590573, },
				{0.00333352, 0.00377393, 0.0 , 0.00661722, 0.00519317, },
				{0.00399246, 0.0046957, 0.0 , 0.0130127, 0.00827043, },
				{0.00402488, 0.0047467, 0.0 , 0.009785, 0.00807276, },
				{0.00283741, 0.00338094, 0.0 , 0.00714533, 0.00634237, }};


				static double ff_u_deltae_4[13][5]    = {{-4.05967, 0.392422, 0.0 , 1.34081, 0.397102},
				{0.535326, -0.158962, 0.0 , 4.98167, -6.22572},
				{1.58592, -1.41023, 0.0 , 0.911207, 1.53356},
				{-1.35057, -2.62955, 0.0 , -3.15588, -13.061},
				{-0.783401, -0.0719185, 0.0 , -3.58985, 3.62266},
				{0.430502, -2.21494, 0.0 , 4.82187, -0.40802},
				{-0.876425, -2.89664, 0.0 , -7.98174, 1.43319},
				{-4.00181, -0.178923, 0.0 , 4.3835, -14.0904},
				{0.0339565, -0.15373, 0.0 , -1.70802, 2.60789},
				{-1.63025, -0.814833, 0.0 , -0.134678, -1.29774},
				{-1.33414, -2.38244, 0.0 , 9.34853, -2.93222},
				{-2.12364, -1.0186, 0.0 , 0.0835819, 5.49628},
				{-2.78078, -2.34866, 0.0 , 3.6652, 2.69508}};


				static double fferr_u_deltae_4[13][5]    = {{1.46913, 1.11588, 0.0 , 2.57487, 1.80126, },
				{1.87791, 1.26342, 0.0 , 3.79699, 3.06444, },
				{2.05176, 2.19341, 0.0 , 3.38175, 3.73596, },
				{2.51761, 2.08637, 0.0 , 6.27474, 6.17732, },
				{3.10997, 2.25217, 0.0 , 7.67291, 4.15686, },
				{1.23693, 1.23384, 0.0 , 2.67961, 2.74368, },
				{1.58501, 1.80654, 0.0 , 3.60451, 3.53796, },
				{2.7707, 1.57531, 0.0 , 5.68433, 6.64089, },
				{1.6419, 1.37596, 0.0 , 3.70168, 2.5423, },
				{1.97539, 1.77608, 0.0 , 4.21641, 3.37481, },
				{2.43413, 2.41398, 0.0 , 3.99363, 5.5366, },
				{2.40898, 2.05395, 0.0 , 6.70572, 3.70073, },
				{1.4963, 1.66604, 0.0 , 4.19198, 4.8896, }};


				static double ff_u_eratio_4[13][5]    = {{0.000334978, -0.00431949, 0.0 , -0.00185287, -0.00407791},
				{-0.007357, -0.0029732, 0.0 , -0.00518638, 0.00457054},
				{-0.0017311, -0.0127707, 0.0 , -0.00320131, -0.000606477},
				{-0.00551468, -0.000977933, 0.0 , -0.00458169, 0.00279117},
				{-0.00116408, -0.00568169, 0.0 , -0.00510961, -0.00205564},
				{-0.0041846, -0.00397384, 0.0 , 0.00060159, -0.0034036},
				{-0.00695688, -0.00265592, 0.0 , -0.00393498, -0.0030598},
				{-0.00115132, -0.00725603, 0.0 , -0.00227863, -0.00097996},
				{-0.00285619, -0.00561768, 0.0 , -0.00261384, -0.000332475},
				{0.00127411, -0.00317192, 0.0 , -0.00235635, 0.000146091},
				{-0.00386214, -0.00447279, 0.0 , -0.00196898, -0.000313282},
				{-0.00280595, -0.00354964, 0.0 , -0.00315291, 0.00464457},
				{-0.00308561, -0.00294238, 0.0 , -0.00434059, -0.00061065}};


				static double fferr_u_eratio_4[13][5]    = {{0.00326073, 0.00198379, 0.0 , 0.00257652, 0.00129334, },
				{0.00410754, 0.00285587, 0.0 , 0.00219342, 0.00445372, },
				{0.0040633, 0.00252987, 0.0 , 0.00230202, 0.00220416, },
				{0.00391978, 0.00315222, 0.0 , 0.00462454, 0.0031269, },
				{0.00622961, 0.00328928, 0.0 , 0.0036912, 0.00257573, },
				{0.00206856, 0.00186242, 0.0 , 0.00243632, 0.00126863, },
				{0.00215179, 0.00288641, 0.0 , 0.00120635, 0.00148789, },
				{0.00357791, 0.00209974, 0.0 , 0.00312795, 0.00223256, },
				{0.00331117, 0.00158136, 0.0 , 0.00196343, 0.00204534, },
				{0.00324159, 0.00199547, 0.0 , 0.00251418, 0.00199565, },
				{0.00334621, 0.00184591, 0.0 , 0.0023478, 0.00228572, },
				{0.00291829, 0.00288886, 0.0 , 0.0011928, 0.00495506, },
				{0.00290787, 0.0021346, 0.0 , 0.00263078, 0.00152634, }};


				if(isConv) {

				m_rhad1_ff.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_rhad1_4[0][0],  0. );
				m_rhad_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_rhad_4[0][0],   0. );
				m_e277_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_e277_4[0][0],   0. );
				m_reta_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_reta_4[0][0],   0. );
				m_rphi_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_rphi_4[0][0],   0. );
				m_weta2_ff.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_weta2_4[0][0],  0. );
				m_f1_ff.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_f1_4[0][0],     0. );
				m_fside_ff.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_fside_4[0][0],  0. );
				m_wtot_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_wtot_4[0][0] ,  0. );
				m_w1_ff.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_w1_4[0][0],     0. );
				m_deltae_ff.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_deltae_4[0][0], 0. );
				m_eratio_ff.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_c_eratio_4[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_rhad1_4[0][0],  0. );
				m_rhad_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_rhad_4[0][0],   0. );
				m_e277_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_e277_4[0][0],   0. );
				m_reta_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_reta_4[0][0],   0. );
				m_rphi_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_rphi_4[0][0],   0. );
				m_weta2_fferr.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_weta2_4[0][0],  0. );
				m_f1_fferr.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_f1_4[0][0],     0. );
				m_fside_fferr.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_fside_4[0][0],  0. );
				m_wtot_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_wtot_4[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_w1_4[0][0],     0. );
				m_deltae_fferr.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_deltae_4[0][0], 0. );
				m_eratio_fferr.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_c_eratio_4[0][0], 0. );

				} else{

				m_rhad1_ff.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_rhad1_4[0][0],  0. );
				m_rhad_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_rhad_4[0][0],   0. );
				m_e277_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_e277_4[0][0],   0. );
				m_reta_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_reta_4[0][0],   0. );
				m_rphi_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_rphi_4[0][0],   0. );
				m_weta2_ff.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_weta2_4[0][0],  0. );
				m_f1_ff.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_f1_4[0][0],     0. );
				m_fside_ff.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_fside_4[0][0],  0. );
				m_wtot_ff.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_wtot_4[0][0],   0. );
				m_w1_ff.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_w1_4[0][0],     0. );
				m_deltae_ff.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_deltae_4[0][0], 0. );
				m_eratio_ff.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &ff_u_eratio_4[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_rhad1_4[0][0],  0. );
				m_rhad_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_rhad_4[0][0],   0. );
				m_e277_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_e277_4[0][0],   0. );
				m_reta_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_reta_4[0][0],   0. );
				m_rphi_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_rphi_4[0][0],   0. );
				m_weta2_fferr.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_weta2_4[0][0],  0. );
				m_f1_fferr.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_f1_4[0][0],     0. );
				m_fside_fferr.Initialize  ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_fside_4[0][0],  0. );
				m_wtot_fferr.Initialize   ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_wtot_4[0][0],   0. );
				m_w1_fferr.Initialize     ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_w1_4[0][0],     0. );
				m_deltae_fferr.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_deltae_4[0][0], 0. );
				m_eratio_fferr.Initialize ( 13, 5, pt_DVs_4, eta_DVs_0, &fferr_u_eratio_4[0][0], 0. );

				}

				break;

		case 5:  // FF from 2011 data

				static double pt_DVs_5[7] = {30000,35000,40000,45000,55000,70000,10000000};// in MeV
				static double eta_DVs_5[5] = { 0.6, 1.37, 1.52, 1.81, 2.37}; // eta binning according to SMDP inclusive analysis

				//converted photons

				static double ff_c_rhad1_5[7][5]    = {{0.000116397, 0.000108798, 0.0 , 0.000108448, 9.70257e-05},
				{-6.87343e-05, 0.000103274, 0.0 , 0.000190065, 3.41305e-05},
				{-0.000171728, 3.90341e-05, 0.0 , -0.000246648, 4.17183e-06},
				{3.19643e-05, 1.11274e-05, 0.0 , 9.96249e-05, 3.16097e-05},
				{-0.000125658, 2.85111e-05, 0.0 , -2.15987e-05, 3.28543e-05},
				{-0.000116651, 3.71913e-05, 0.0 , 2.83322e-05, -3.86153e-06},
				{-0.000128282, -2.79636e-05, 0.0 , 8.13077e-05, -5.72117e-06},   };



				static double fferr_c_rhad1_5[7][5]    = {	{7.3886e-05, 2.83888e-05, 0.0 , 8.27269e-05, 3.74658e-05},
				{9.40067e-05, 3.68663e-05, 0.0 , 0.000108875, 4.57367e-05},
				{0.000112051, 4.30594e-05, 0.0 , 0.00013245, 6.00929e-05},
				{8.24336e-05, 3.27012e-05, 0.0 , 9.13849e-05, 3.86217e-05},
				{0.000114381, 3.4577e-05, 0.0 , 0.000129806, 4.27518e-05},
				{0.000143783, 4.49064e-05, 0.0 , 0.000110322, 4.3149e-05},
				{0.000155431, 5.88475e-05, 0.0 , 0.000152859, 7.07626e-05}, };



				static double ff_c_rhad_5[7][5]    = { {0.000168263, 0.000161188, 0.0 , 0.000340806, 0.000317253},
				{-0.000213356, 0.000147575, 0.0 , 0.000240747, -3.56208e-05},
				{-0.000218434, 5.95286e-05, 0.0 , -0.000165261, 9.76233e-05},
				{0.000126991, 8.29584e-05, 0.0 , -1.61838e-06, 0.000119041},
				{-6.95002e-05, -1.09615e-05, 0.0 , -5.38304e-07, 4.59014e-05},
				{-0.000202652, 8.97021e-06, 0.0 , 0.000347175, 9.05459e-05},
				{-0.00029765, -8.15643e-05, 0.0 , 0.00036268, -0.000109692}, };



				static double fferr_c_rhad_5[7][5]    = { {0.000111227, 3.81984e-05, 0.0 , 0.000209659, 0.000115482},
				{0.000143975, 4.94772e-05, 0.0 , 0.000261665, 0.000135496},
				{0.000162969, 5.84071e-05, 0.0 , 0.000326075, 0.000182456},
				{0.00021158, 8.06748e-05, 0.0 , 0.00036851, 0.000217063},
				{0.000127207, 4.53848e-05, 0.0 , 0.000221791, 0.00011021},
				{0.000159145, 5.43499e-05, 0.0 , 0.000274354, 0.000125216},
				{0.000200969, 7.20592e-05, 0.0 , 0.00023336, 0.000100149}, };



				static double ff_c_e277_5[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};



				static double fferr_c_e277_5[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};



				static double ff_c_reta_5[7][5]    = { {-0.00461024, -0.00701648, 0.0 , -0.0100893, -0.00931102},
				{-0.00482398, -0.00735283, 0.0 , -0.00902379, -0.00921506},
				{-0.0053907, -0.00784278, 0.0 , -0.009893, -0.00906509},
				{-0.00394291, -0.00766432, 0.0 , -0.00923645, -0.0111474},
				{-0.00462312, -0.00695074, 0.0 , -0.0100248, -0.00947428},
				{-0.00478244, -0.00713646, 0.0 , -0.00911599, -0.0101972},
				{-0.00361997, -0.00736839, 0.0 , -0.00885564, -0.0102811}, };



				static double fferr_c_reta_5[7][5]    = {	{0.000362046, 0.000224212, 0.0 , 0.000362246, 0.000295271},
				{0.000468912, 0.000309371, 0.0 , 0.000527868, 0.000398952},
				{0.000596246, 0.00039021, 0.0 , 0.000639008, 0.000539605},
				{0.000741813, 0.000504149, 0.0 , 0.000865195, 0.000689862},
				{0.000419602, 0.000315759, 0.0 , 0.000521964, 0.000399779},
				{0.000518659, 0.00038948, 0.0 , 0.00072196, 0.000509252},
				{0.000646192, 0.000479437, 0.0 , 0.00082058, 0.00062726}, };



				static double ff_c_rphi_5[7][5]    = {{-0.00315613, -0.00369465, 0.0 ,-0.00340688, -0.00420791},
				{-0.001342, -0.00465286, 0.0 , -0.00437301, -0.00336027},
				{-0.00207573, -0.00673151, 0.0 , -0.00412005, -0.00359213},
				{-0.0019238, -0.00771767, 0.0 , -0.000696719, -0.00408614},
				{-0.00216031, -0.00280428, 0.0 , -0.00394583, -0.00406152},
				{-0.00243521, -0.00297576, 0.0 , -0.000729322, -0.00605351},
				{-0.00146425, -0.00336498, 0.0 , -0.00212371, -0.004345}, };



				static double fferr_c_rphi_5[7][5]    = {{0.00115486, 0.00064464, 0.0 , 0.000937351, 0.000726693},
				{0.00155734, 0.00089935, 0.0 , 0.00131313, 0.000976179},
				{0.00210785, 0.00119748, 0.0 , 0.00182936, 0.00125639},
				{0.00281306, 0.00159206, 0.0 , 0.00224286, 0.00157355},
				{0.00170374, 0.000995394, 0.0 , 0.0013626, 0.000851338},
				{0.00213242, 0.00119394, 0.0 , 0.00181578, 0.000985626},
				{0.00210045, 0.00132115, 0.0 , 0.00189076, 0.00114567}, };



				static double ff_c_weta2_5[7][5]    = {{0.000264973, 0.000221425, 0.0 , 0.000423228, 0.000303966},
				{0.00023779, 0.000221236, 0.0 , 0.000458021, 0.000304252},
				{0.000202141, 0.000230053, 0.0 , 0.000443482, 0.000347204},
				{0.000245636, 0.000231406, 0.0 , 0.000378706, 0.00041175},
				{0.000203775, 0.000225478, 0.0 , 0.000472404, 0.000285585},
				{0.000215879, 0.0002088, 0.0 , 0.000307559, 0.000374448},
				{0.000231412, 0.000202896, 0.0 , 0.000413664, 0.0003502},};



				static double fferr_c_weta2_5[7][5]    = {{1.78351e-05, 1.22302e-05, 0.0 , 2.11787e-05, 1.8318e-05},
				{2.35347e-05, 1.71175e-05, 0.0 , 2.90749e-05, 2.53957e-05},
				{3.04565e-05, 2.22684e-05, 0.0 , 3.72903e-05, 3.43194e-05},
				{3.99309e-05, 3.03041e-05, 0.0 , 4.90499e-05, 4.62477e-05},
				{2.26081e-05, 1.73327e-05, 0.0 , 2.91301e-05, 2.7007e-05},
				{2.90334e-05, 2.21735e-05, 0.0 , 4.3048e-05, 3.50365e-05},
				{3.11492e-05, 2.81181e-05, 0.0 , 4.43891e-05, 4.25833e-05},};



				static double ff_c_f1_5[7][5]    = {{0.00224742, -0.00431556, 0.0 , 0.0083667, 0.000423312},
				{0.00165001, -0.000476465, 0.0 , 0.0100259, 0.00268516},
				{-0.00663191, 0.000648215, 0.0 , 0.00919148, 0.00795123},
				{-0.00679174, 0.00196356, 0.0 , -0.00424016, 0.0137846},
				{-0.000352532, -0.00359093, 0.0 , 0.0142126, 0.00378859},
				{-0.00476629, -0.00871706, 0.0 , -0.00310344, 0.00828569},
				{-0.00355783, -0.00319682, 0.0 , 0.00450242, 0.0156806}, };



				static double fferr_c_f1_5[7][5]    = {{0.00260203, 0.00145388, 0.0 , 0.0022349, 0.00213747},
				{0.00358951, 0.00214161, 0.0 , 0.00309684, 0.00294101},
				{0.00467317, 0.00286794, 0.0 , 0.00418057, 0.00393667},
				{0.00635693, 0.00376344, 0.0 , 0.00544058, 0.00514332},
				{0.00367042, 0.00237529, 0.0 , 0.00375722, 0.00301892},
				{0.00492094, 0.00324551, 0.0 , 0.00475193, 0.00369424},
				{0.00583012, 0.00401053, 0.0 , 0.0068817, 0.00505884}, };



				static double ff_c_fside_5[7][5]    = {{0.0058786, 0.0104624, 0.0 , 0.0267844, 0.0182255},
				{0.00823183, 0.00996086, 0.0 , 0.0277735, 0.0177641},
				{0.0067662, 0.00708124, 0.0 , 0.0304948, 0.0176176},
				{0.00252779, 0.0119567, 0.0 , 0.0301109, 0.0171506},
				{0.00363667, 0.00581521, 0.0 , 0.0323325, 0.0190491},
				{0.00476116, 0.00844821, 0.0 , 0.0131588, 0.016935},
				{0.000319332, 0.0135838, 0.0 , 0.0225642, 0.0201226},};



				static double fferr_c_fside_5[7][5]    = {{0.00133919, 0.00149906, 0.0 , 0.00251102, 0.000850532},
				{0.00188684, 0.00215059, 0.0 , 0.00363532, 0.00114117},
				{0.0022597, 0.00286868, 0.0 , 0.00496756, 0.00155027},
				{0.00302012, 0.00372842, 0.0 , 0.00606956, 0.00216575},
				{0.00195032, 0.00226679, 0.0 , 0.00405421, 0.0012843},
				{0.00249352, 0.00290145, 0.0 , 0.0059016, 0.0016422},
				{0.00315727, 0.00360499, 0.0 , 0.0061265, 0.00205655},};



				static double ff_c_wtot_5[7][5]    = {{0.0792546, 0.0809703, 0.0 , 0.153447, 0.0939299},
				{0.0788096, 0.0854204, 0.0 , 0.152501, 0.0928177},
				{0.0699871, 0.0820711, 0.0 , 0.174108, 0.0861884},
				{0.0686636, 0.0815148, 0.0 , 0.144743, 0.0881563},
				{0.0784361, 0.074156, 0.0 , 0.162832, 0.10842},
				{0.0875851, 0.0772352, 0.0 , 0.111624, 0.0823516},
				{0.0802225, 0.0886805, 0.0 , 0.113382, 0.0923615},};



				static double fferr_c_wtot_5[7][5]    = {{0.00792349, 0.00586304, 0.0 , 0.00976904, 0.00559688},
				{0.0106373, 0.00820192, 0.0 , 0.0137463, 0.00744895},
				{0.0127435, 0.0110513, 0.0 , 0.0171694, 0.0104084},
				{0.0178314, 0.01431, 0.0 , 0.0230283, 0.0139431},
				{0.00973741, 0.00873103, 0.0 , 0.0151095, 0.00787652},
				{0.0113819, 0.0114883, 0.0 , 0.0202478, 0.0108636},
				{0.0147919, 0.0140664, 0.0 , 0.0222215, 0.0135474},};



				static double ff_c_w1_5[7][5]    = {{0.000451386, 0.0068686, 0.0 , 0.0123558, 0.0102509},
				{0.000899732, 0.0065192, 0.0 , 0.0128348, 0.010664},
				{-0.0039891, 0.00552583, 0.0 , 0.0142406, 0.0115945},
				{-0.00409561, 0.00618833, 0.0 , 0.00786376, 0.00994587},
				{0.000565708, 0.003613, 0.0 , 0.0147399, 0.0110731},
				{0.000793993, 0.00514197, 0.0 , 0.00650609, 0.0129067},
				{0.00187838, 0.00499141, 0.0 , 0.00902116, 0.0136743},};



				static double fferr_c_w1_5[7][5]    = {{0.00118959, 0.000846485, 0.0 , 0.00131727, 0.000936336},
				{0.00165309, 0.00124671, 0.0 , 0.00184044, 0.00128627},
				{0.00199722, 0.00167024, 0.0 , 0.00249429, 0.00178811},
				{0.00273472, 0.00224071, 0.0 , 0.0031042, 0.00229974},
				{0.00164444, 0.00138439, 0.0 , 0.00215583, 0.00149433},
				{0.00221333, 0.00172247, 0.0 , 0.00274401, 0.00180727},
				{0.00276099, 0.00228625, 0.0 , 0.00333005, 0.00243945},};



				static double ff_c_deltae_5[7][5]    = {{-2.10691, -2.59767, 0.0 , -2.30769, -3.66614},
				{-1.07361, -1.92798, 0.0 , 0.936928, -1.22025},
				{-2.73422, -3.01066, 0.0 , 0.433041, -5.11025},
				{1.57528, -3.34181, 0.0 , -4.02825, -3.34785},
				{-2.22137, -2.61592, 0.0 , 0.131554, -2.977},
				{-0.0702209, -3.41992, 0.0 , -4.75027, -5.86122},
				{-2.14583, -2.69562, 0.0 , -0.0115204, -0.625078},};



				static double fferr_c_deltae_5[7][5]    = {{0.697343, 0.395864, 0.0 , 0.852135, 0.643031},
				{1.02734, 0.557794, 0.0 , 1.23181, 0.914654},
				{1.30466, 0.746897, 0.0 , 1.6375, 1.2926},
				{1.61346, 1.02971, 0.0 , 2.27533, 1.67274},
				{1.20515, 0.651193, 0.0 , 1.46608, 1.0621},
				{1.37906, 0.920341, 0.0 , 2.21273, 1.54817},
				{1.84717, 1.22104, 0.0 , 2.74527, 1.79522},};



				static double ff_c_eratio_5[7][5]    = {{-0.000826597, -0.000477254, 0.0 , -0.00209057, -0.000208139},
				{-0.00171798, -0.00235331, 0.0 , -0.00131124, -0.0006935},
				{-0.000640333, -0.000827789, 0.0 , -0.00671726, -0.000356138},
				{0.000441253, 0.00162745, 0.0 , -0.00419092, -0.000263512},
				{-0.000827074, -0.000248671, 0.0 , -0.00302601, -0.000577211},
				{-0.000617921, 0.000911236, 0.0 , -0.000562131, 2.00868e-05},
				{-7.21812e-05, -0.000565529, 0.0 , -0.00111282, -0.000376403}, };



				static double fferr_c_eratio_5[7][5]    = {{0.000608024, 0.000717008, 0.0 , 0.00126709, 0.00028281},
				{0.00082483, 0.000979514, 0.0 , 0.00151577, 0.000368918},
				{0.00105119, 0.00125025, 0.0 , 0.0017979, 0.000477968},
				{0.00139192, 0.00159644, 0.0 , 0.00227327, 0.000670817},
				{0.000839731, 0.000928718, 0.0 , 0.00139812, 0.000342006},
				{0.00100003, 0.00115168, 0.0 , 0.00162066, 0.000426198},
				{0.00102148, 0.00133239, 0.0 , 0.00153504, 0.000509223},};

				//unconverted photons

				static double ff_u_rhad1_5[7][5]    = {{-4.24986e-05, 5.29536e-05, 0.0 , 0.000120799, 0.000127757},
				{-1.80978e-05, 3.51718e-05, 0.0 , -3.28941e-05, 7.90695e-05},
				{-4.51529e-05, -2.22937e-05, 0.0 , -0.000102632, 4.22486e-05},
				{-0.000112009, 7.09991e-05, 0.0 , 0.000298452, 9.29155e-05},
				{-0.000136397, 2.52535e-05, 0.0 , 0.000135529, 7.5823e-05},
				{-0.000227041, -5.54503e-05, 0.0 , -0.000261933, 5.85162e-07},
				{-0.000340822, 3.3461e-06, 0.0 , 3.39856e-05, 2.25612e-05},};



				static double fferr_u_rhad1_5[7][5]    = {{5.03392e-05, 3.09579e-05, 0.0 , 0.000100328, 3.72074e-05},
				{6.67078e-05, 3.8272e-05, 0.0 , 0.000130365, 4.57497e-05},
				{8.37415e-05, 5.00115e-05, 0.0 , 0.000152689, 5.79503e-05},
				{0.000105155, 6.31578e-05, 0.0 , 0.000187103, 7.02489e-05},
				{6.26257e-05, 3.32963e-05, 0.0 , 0.000116943, 4.32217e-05},
				{8.48579e-05, 4.2245e-05, 0.0 , 0.000144658, 4.58481e-05},
				{9.2507e-05, 4.21213e-05, 0.0 , 0.00013603, 5.66865e-05},};



				static double ff_u_rhad_5[7][5]    = {{-7.29199e-05, 0.000108416, 0.0 , -3.84846e-06, 0.000270799},
				{-4.06677e-05, -4.30188e-06, 0.0 , 0.000475225, -5.33409e-06},
				{-8.51371e-05, -1.77013e-05, 0.0 , 0.000652623, -5.88191e-05},
				{-4.63862e-05, 4.28718e-05, 0.0 , 0.000894009, -0.000123038},
				{-0.000273347, 7.22793e-05, 0.0 , -0.000245742, 0.000186934},
				{-0.000497308, -2.90061e-05, 0.0 , -5.32818e-05, -5.89812e-06},
				{-0.000549551, -1.11955e-05, 0.0 , 2.28311e-05, 6.18699e-05},};



				static double fferr_u_rhad_5[7][5]    = {{7.33481e-05, 4.37386e-05, 0.0 , 0.000245001, 0.000111334},
				{9.72703e-05, 5.68395e-05, 0.0 , 0.000325394, 0.000137556},
				{0.000115074, 7.46697e-05, 0.0 , 0.000383692, 0.000164799},
				{0.000150349, 9.25985e-05, 0.0 , 0.000482129, 0.000199083},
				{8.51677e-05, 4.79055e-05, 0.0 , 0.00027936, 0.000104848},
				{0.000113951, 5.45048e-05, 0.0 , 0.000298534, 0.000117515},
				{0.00010999, 5.90697e-05, 0.0 , 0.000264377, 0.00010944},};


				/*
				static double ff_u_e277_0[7][5]    = {{425.203, 1024.02, 0.0 , -23.375, 2288.88},
				{310.352, 631.141, 0.0 , 1871.47, 2008.77},
				{440.285, 1113.95, 0.0 , 1894.77, -1535.41},
				{198.871, -463.273, 0.0 , -7692.29, -1407.67},
				{79.7227, 533.355, 0.0 , 60.6719, -3502.28},
				{-153.34, 1583.26, 0.0 , 1582.92, 1331.8},
				{612.473, 2587.91, 0.0 , -6556.98, 2220.34},};



				static double fferr_u_e277_0[7][5]    = {{123.581, 241.947, 0.0 , 832.081, 812.67, },
				{196.094, 420.303, 0.0 , 1525.54, 1289.17, },
				{334.752, 641.21, 0.0 , 2398.77, 2083.63, },
				{497.225, 911.407, 0.0 , 3022.47, 3190.71, },
				{239.435, 478.502, 0.0 , 1898.26, 1813.12, },
				{394.463, 827.943, 0.0 , 2910.44, 2496.09, },
				{667.093, 1549.79, 0.0 , 5711.67, 3711.84, },};
				*/

				static double ff_u_e277_5[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};


				static double fferr_u_e277_5[7][5]= {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};


				static double ff_u_reta_5[7][5]    = {{-0.00368851, -0.00567037, 0.0 , -0.00932503, -0.00821835},
				{-0.0037387, -0.00590271, 0.0 , -0.00888985, -0.00891662},
				{-0.00427485, -0.0060637, 0.0 , -0.00855762, -0.00844246},
				{-0.00398368, -0.0061152, 0.0 , -0.00756788, -0.0082618},
				{-0.00403661, -0.00634736, 0.0 , -0.00879061, -0.00922877},
				{-0.00429136, -0.0064016, 0.0 , -0.00882453, -0.00911856},
				{-0.00447601, -0.00619215, 0.0 , -0.0101928, -0.00954342},};


				static double fferr_u_reta_5[7][5]    = {{0.000186957, 0.000184932, 0.0 , 0.000354711, 0.000265761},
				{0.000251943, 0.000247237, 0.0 , 0.000505452, 0.000376679},
				{0.000326129, 0.000321101, 0.0 , 0.000641786, 0.000503364},
				{0.000413199, 0.000431498, 0.0 , 0.000883533, 0.000649125},
				{0.000234666, 0.000258274, 0.0 , 0.000504316, 0.000414011},
				{0.00028641, 0.000307852, 0.0 , 0.00066238, 0.000544805},
				{0.000305971, 0.000348522, 0.0 , 0.00071352, 0.000648142},};


				static double ff_u_rphi_5[7][5]    = {{-0.0022558, -0.00315285, 0.0 , -0.00417382, -0.00560009},
				{-0.00230211, -0.00321436, 0.0 , -0.00476676, -0.00607479},
				{-0.00245893, -0.00352317, 0.0 , -0.00417972, -0.00653899},
				{-0.00212753, -0.00392789, 0.0 , -0.00335318, -0.0048697},
				{-0.00247091, -0.00353646, 0.0 , -0.00374907, -0.00608885},
				{-0.00194204, -0.00423354, 0.0 , -0.00269097, -0.00643432},
				{-0.00250626, -0.00369322, 0.0 , -0.00433987, -0.00624186},};


				static double fferr_u_rphi_5[7][5]    = {{0.00016213, 0.000295576, 0.0 , 0.000390229, 0.000230886},
				{0.000214311, 0.000396636, 0.0 , 0.000549653, 0.000317354},
				{0.000281362, 0.000524153, 0.0 , 0.000690859, 0.000422695},
				{0.000353416, 0.000672169, 0.0 , 0.00101756, 0.000548214},
				{0.000208392, 0.00038319, 0.0 , 0.00058036, 0.000360986},
				{0.000268024, 0.000461675, 0.0 , 0.000820904, 0.000455973},
				{0.00026778, 0.000536134, 0.0 , 0.000910121, 0.000605193},};


				static double ff_u_weta2_5[7][5]    = {{0.000214335, 0.000197734, 0.0 , 0.00039636, 0.00028124},
				{0.000202725, 0.000193878, 0.0 , 0.000448614, 0.000318673},
				{0.000228542, 0.000192644, 0.0 , 0.000387958, 0.000332086},
				{0.000226726, 0.000219889, 0.0 , 0.000389025, 0.000283701},
				{0.000229657, 0.000220167, 0.0 , 0.000400325, 0.000368405},
				{0.000213433, 0.000241164, 0.0 , 0.000436228, 0.000328287},
				{0.000248126, 0.000204313, 0.0 , 0.000446885, 0.000379608},};


				static double fferr_u_weta2_5[7][5]    = {{9.70798e-06, 1.05533e-05, 0.0 , 1.91519e-05, 1.75384e-05},
				{1.35328e-05, 1.52519e-05, 0.0 , 2.84018e-05, 2.48147e-05},
				{1.76053e-05, 1.96045e-05, 0.0 , 3.56192e-05, 3.32028e-05},
				{2.24491e-05, 2.68753e-05, 0.0 , 4.7078e-05, 4.1331e-05},
				{1.26823e-05, 1.5823e-05, 0.0 , 3.173e-05, 2.95284e-05},
				{1.61412e-05, 1.90216e-05, 0.0 , 4.48669e-05, 3.82776e-05},
				{1.69855e-05, 2.37262e-05, 0.0 , 4.75075e-05, 4.99526e-05},};


				static double ff_u_f1_5[7][5]    = {{-0.00934906, -0.00907968, 0.0 , 0.00307256, -0.0129397},
				{-0.010009, -0.0102086, 0.0 , 0.00696811, -0.00494161},
				{-0.00463903, -0.0129596, 0.0 , 0.00440642, -0.00572394},
				{0.00223833, -0.00691137, 0.0 , -0.00519565, -0.00787537},
				{-0.00615747, -0.00716169, 0.0 , 0.00555852, 0.000750393},
				{-0.00528519, -0.00793554, 0.0 , -0.00501229, -0.00126643},
				{-0.000704423, -0.00306869, 0.0 , 0.0111687, 0.00918797},};


				static double fferr_u_f1_5[7][5]    = {{0.00198852, 0.00175427, 0.0 , 0.00346095, 0.00249815},
				{0.00273046, 0.00249879, 0.0 , 0.0050148, 0.00343433},
				{0.00371509, 0.00339942, 0.0 , 0.00650958, 0.00462735},
				{0.00496391, 0.00466609, 0.0 , 0.00914956, 0.00593557},
				{0.00293248, 0.0027906, 0.0 , 0.00580696, 0.00375319},
				{0.00374337, 0.00367377, 0.0 , 0.00801691, 0.00490523},
				{0.00417577, 0.00406392, 0.0 , 0.00998132, 0.00640794},};



				static double ff_u_fside_5[7][5]    = {{0.00167942, 0.00710528, 0.0 , 0.0220233, 0.0130039},
				{0.00299369, 0.00414282, 0.0 , 0.0287309, 0.0127265},
				{0.00339824, 0.0052162, 0.0 , 0.0166872, 0.0129008},
				{0.00322044, 0.00694585, 0.0 , 0.0155192, 0.0153712},
				{0.00341216, 0.00683185, 0.0 , 0.0180028, 0.013894},
				{0.00316931, 0.00793521, 0.0 , 0.0135038, 0.0158898},
				{0.00205463, 0.00880991, 0.0 , 0.0344102, 0.0176818}, };


				static double fferr_u_fside_5[7][5]    = {{0.000861434, 0.00134707, 0.0 , 0.00299354, 0.000753975},
				{0.00121851, 0.00194392, 0.0 , 0.00410327, 0.00100901},
				{0.00161435, 0.0025251, 0.0 , 0.00589832, 0.00136282},
				{0.00205025, 0.00335515, 0.0 , 0.00850067, 0.00192148},
				{0.00122398, 0.00207136, 0.0 , 0.00534093, 0.00116151},
				{0.00160311, 0.00257855, 0.0 , 0.00662744, 0.00144191},
				{0.00172174, 0.00301984, 0.0 , 0.00562933, 0.00186567},};


				static double ff_u_wtot_5[7][5]    = {{0.0670551, 0.0728507, 0.0 , 0.133553, 0.0703841},
				{0.078301, 0.0812013, 0.0 , 0.132698, 0.0731535},
				{0.0645132, 0.0829816, 0.0 , 0.1116, 0.0732169},
				{0.0708816, 0.0785105, 0.0 , 0.0947998, 0.112958},
				{0.0830435, 0.0877941, 0.0 , 0.13781, 0.0818454},
				{0.074924, 0.0956852, 0.0 , 0.0970737, 0.0848978},
				{0.0722991, 0.0874205, 0.0 , 0.18463, 0.0975598},};


				static double fferr_u_wtot_5[7][5]    = {{0.00647132, 0.00644309, 0.0 , 0.0113383, 0.00576456},
				{0.00851371, 0.00869663, 0.0 , 0.015697, 0.00784388},
				{0.0108853, 0.0114778, 0.0 , 0.0199362, 0.0106306},
				{0.0147864, 0.0160336, 0.0 , 0.03068, 0.0139324},
				{0.00800064, 0.0093602, 0.0 , 0.0173944, 0.0095542},
				{0.0114717, 0.0113481, 0.0 , 0.0219232, 0.0114856},
				{0.0112267, 0.0144337, 0.0 , 0.0216948, 0.015755},};


				static double ff_u_w1_5[7][5]    = {{-0.00325984, 0.00406343, 0.0 , 0.0117654, 0.0076282},
				{-0.00493675, 0.00433415, 0.0 , 0.0154556, 0.00822371},
				{-0.00270933, 0.00352746, 0.0 , 0.0124838, 0.00817251},
				{0.000955462, 0.000544727, 0.0 , 0.0129741, 0.010637},
				{-0.00358403, 0.00488359, 0.0 , 0.00972492, 0.0096826},
				{-0.00281841, 0.00703198, 0.0 , 0.00852799, 0.00702566},
				{-0.00213522, 0.00618279, 0.0 , 0.0215852, 0.0161574},};


				static double fferr_u_w1_5[7][5]    = {{0.000765754, 0.000911167, 0.0 , 0.00168118, 0.000917104},
				{0.00109234, 0.00130202, 0.0 , 0.00241581, 0.00127965},
				{0.00147389, 0.00174178, 0.0 , 0.0032042, 0.00174888},
				{0.00202385, 0.00245653, 0.0 , 0.00417598, 0.00220012},
				{0.00113104, 0.00146164, 0.0 , 0.00272477, 0.00138221},
				{0.00149443, 0.00190246, 0.0 , 0.00396168, 0.00152816},
				{0.00170819, 0.00231584, 0.0 , 0.00458728, 0.00235017},};


				static double ff_u_deltae_5[7][5]    = {{-1.12465, -1.92239, 0.0 , -0.667253, -2.66955},
				{-0.466482, -0.889374, 0.0 , -2.86502, -5.95599},
				{-1.19754, -1.55874, 0.0 , -1.40029, -1.39386},
				{-1.61364, -1.67617, 0.0 , -0.144899, -1.57702},
				{-1.17805, -1.16276, 0.0 , -1.38413, -5.53087},
				{-1.8582, -0.634624, 0.0 , -0.367847, -5.00622},
				{-0.202206, -0.328228, 0.0 , -0.383261, -7.1192},};


				static double fferr_u_deltae_5[7][5]    = {{0.375285, 0.344888, 0.0 , 0.842185, 0.796911},
				{0.535859, 0.471738, 0.0 , 1.21426, 1.15791},
				{0.728473, 0.679278, 0.0 , 1.53113, 1.4915},
				{0.963789, 0.929437, 0.0 , 2.15683, 2.13457},
				{0.593449, 0.565967, 0.0 , 1.42928, 1.39327},
				{0.75069, 0.705195, 0.0 , 1.87867, 1.78152},
				{0.91341, 0.8586, 0.0 , 2.215, 2.82585},};




				static double ff_u_eratio_5[7][5]    = {{-0.000815868, -0.00132048, 0.0 , -0.00218815, -0.00139934},
				{-0.00180578, -0.00335813, 0.0 , -0.000485599, -0.000240028},
				{0.00170088, -0.00264025, 0.0 , -0.00114489, -0.00261128},
				{0.00157726, -0.0032776, 0.0 , -0.000922978, -0.00237429},
				{-0.00150275, -0.00219119, 0.0 , -0.00231737, -0.000367761},
				{-0.000868678, -0.00175655, 0.0 , -0.00050813, -0.00163895},
				{0.000912905, -0.00142366, 0.0 , -0.00375807, 0.00132108},};


				static double fferr_u_eratio_5[7][5]    = {{0.000778491, 0.000593743, 0.0 , 0.000754528, 0.000630561},
				{0.00103676, 0.000787635, 0.0 , 0.00102727, 0.00085307},
				{0.00139967, 0.00106053, 0.0 , 0.00119407, 0.00110374},
				{0.00183279, 0.0013883, 0.0 , 0.00190773, 0.00128293},
				{0.000975204, 0.000775386, 0.0 , 0.000942382, 0.000894372},
				{0.00120437, 0.00110973, 0.0 , 0.00134942, 0.000738314},
				{0.00166977, 0.00112704, 0.0 , 0.00109139, 0.00128812},};


				if(isConv){
				m_rhad1_ff.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_rhad1_5[0][0],  0. );
				m_rhad_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_rhad_5[0][0],   0. );
				m_e277_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_e277_5[0][0],   0. );
				m_reta_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_reta_5[0][0],   0. );
				m_rphi_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_rphi_5[0][0],   0. );
				m_weta2_ff.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_weta2_5[0][0],  0. );
				m_f1_ff.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_f1_5[0][0],     0. );
				m_fside_ff.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_fside_5[0][0],  0. );
				m_wtot_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_wtot_5[0][0] ,  0. );
				m_w1_ff.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_w1_5[0][0],     0. );
				m_deltae_ff.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_deltae_5[0][0], 0. );
				m_eratio_ff.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_c_eratio_5[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_rhad1_5[0][0],  0. );
				m_rhad_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_rhad_5[0][0],   0. );
				m_e277_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_e277_5[0][0],   0. );
				m_reta_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_reta_5[0][0],   0. );
				m_rphi_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_rphi_5[0][0],   0. );
				m_weta2_fferr.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_weta2_5[0][0],  0. );
				m_f1_fferr.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_f1_5[0][0],     0. );
				m_fside_fferr.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_fside_5[0][0],  0. );
				m_wtot_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_wtot_5[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_w1_5[0][0],     0. );
				m_deltae_fferr.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_deltae_5[0][0], 0. );
				m_eratio_fferr.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_c_eratio_5[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_rhad1_5[0][0],  0. );
				m_rhad_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_rhad_5[0][0],   0. );
				m_e277_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_e277_5[0][0],   0. );
				m_reta_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_reta_5[0][0],   0. );
				m_rphi_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_rphi_5[0][0],   0. );
				m_weta2_ff.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_weta2_5[0][0],  0. );
				m_f1_ff.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_f1_5[0][0],     0. );
				m_fside_ff.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_fside_5[0][0],  0. );
				m_wtot_ff.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_wtot_5[0][0],   0. );
				m_w1_ff.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_w1_5[0][0],     0. );
				m_deltae_ff.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_deltae_5[0][0], 0. );
				m_eratio_ff.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &ff_u_eratio_5[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_rhad1_5[0][0],  0. );
				m_rhad_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_rhad_5[0][0],   0. );
				m_e277_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_e277_5[0][0],   0. );
				m_reta_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_reta_5[0][0],   0. );
				m_rphi_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_rphi_5[0][0],   0. );
				m_weta2_fferr.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_weta2_5[0][0],  0. );
				m_f1_fferr.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_f1_5[0][0],     0. );
				m_fside_fferr.Initialize  ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_fside_5[0][0],  0. );
				m_wtot_fferr.Initialize   ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_wtot_5[0][0],   0. );
				m_w1_fferr.Initialize     ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_w1_5[0][0],     0. );
				m_deltae_fferr.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_deltae_5[0][0], 0. );
				m_eratio_fferr.Initialize ( 7, 5, pt_DVs_5, eta_DVs_5, &fferr_u_eratio_5[0][0], 0. );
				}

				break;

		case 6:  // FF from 2011 data + No Isolation cut

				static double pt_DVs_6[7] = {30000,35000,40000,45000,55000,70000,10000000};// in MeV
				static double eta_DVs_6[5] = { 0.6, 1.37, 1.52, 1.81, 2.37}; // eta binning according to SMDP inclusive analysis

				//converted photons

				static double ff_c_rhad1_6[7][5]    = { {-2.08015e-05, 0.000115876, 0.0 , 8.25606e-05, 9.97429e-05},
				{-6.77846e-05, 8.3778e-05, 0.0 , 0.00015331, 8.02901e-05},
				{-0.000238194, 5.82742e-06, 0.0 , -0.000102274, -2.21112e-05},
				{-0.000100149, -6.7997e-05, 0.0 , 0.000201927, 4.11982e-06},
				{-0.000213152, 1.4475e-05, 0.0 , -1.14038e-05, 2.54966e-05},
				{-1.72324e-05, 1.70885e-06, 0.0 , 7.56459e-05, 9.69929e-06},
				{5.94744e-05, -6.35613e-05, 0.0 , -2.7549e-05, 3.79123e-05}, };



				static double fferr_c_rhad1_6[7][5]    = { {6.39542e-05, 2.42339e-05, 0.0 , 6.5539e-05, 3.33501e-05},
				{8.50707e-05, 3.17504e-05, 0.0 , 8.73837e-05, 4.13246e-05},
				{0.000103108, 3.86567e-05, 0.0 , 0.000107454, 5.4006e-05},
				{0.00014194, 4.98948e-05, 0.0 , 0.000134076, 6.28025e-05},
				{8.09968e-05, 2.83748e-05, 0.0 , 7.9268e-05, 3.7687e-05},
				{0.000105541, 3.5305e-05, 0.0 , 0.000100694, 4.3786e-05},
				{0.000131106, 4.75586e-05, 0.0 , 0.000109169, 4.59445e-05},};



				static double ff_c_rhad_6[7][5]    = { {-7.81642e-05, 0.000147935, 0.0 , 0.000157244, 0.000376897},
				{-0.000178601, 0.000102203, 0.0 , 0.000304712, 8.23195e-05},
				{-0.000168641, 2.24971e-05, 0.0 , 3.91838e-05, 0.000193136},
				{5.23656e-05, 2.84683e-05, 0.0 , 0.000164237, 3.62889e-05},
				{-0.000205251, 7.09624e-06, 0.0 , -5.58903e-05, 9.48866e-05},
				{-7.32741e-06, 1.13762e-05, 0.0 , 0.000173751, 0.000291679},
				{-6.14349e-05, -0.000118203, 0.0 , 1.71048e-05, 8.09203e-07}, };


				static double fferr_c_rhad_6[7][5]    = { {9.89374e-05, 3.38717e-05, 0.0 , 0.000167105, 0.000104956},
				{0.000129807, 4.38737e-05, 0.0 , 0.00021204, 0.000129778},
				{0.000156309, 4.96981e-05, 0.0 , 0.000267652, 0.00017055},
				{0.000203785, 7.3756e-05, 0.0 , 0.000320298, 0.000207198},
				{0.000128206, 3.87863e-05, 0.0 , 0.000190867, 0.000109405},
				{0.000153108, 4.94569e-05, 0.0 , 0.000222206, 0.000119152},
				{0.000202914, 6.79778e-05, 0.0 , 0.00023448, 0.000149382}, };


				static double ff_c_e277_6[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};

				static double fferr_c_e277_6[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};

				static double ff_c_reta_6[7][5]    = { {-0.00459892, -0.00690609, 0.0 , -0.0086655, -0.00890076},
				{-0.00456166, -0.00696522, 0.0 , -0.00846344, -0.00865948},
				{-0.0049085, -0.00708199, 0.0 , -0.009233, -0.0088467},
				{-0.00421214, -0.00687677, 0.0 , -0.00928617, -0.0106791},
				{-0.0041644, -0.00665325, 0.0 , -0.00867379, -0.00932914},
				{-0.00506562, -0.00671268, 0.0 , -0.00887138, -0.00949949},
				{-0.00374323, -0.00742489, 0.0 , -0.00824052, -0.0095771},};

				static double fferr_c_reta_6[7][5]    = { {0.000308383, 0.000189507, 0.0 , 0.000304294, 0.000263738},
				{0.000402151, 0.000263752, 0.0 , 0.000437712, 0.000359428},
				{0.000531512, 0.000342389, 0.0 , 0.000565064, 0.000477953},
				{0.000649534, 0.000440078, 0.0 , 0.000750987, 0.000640215},
				{0.000385372, 0.000282023, 0.0 , 0.000484614, 0.00037237},
				{0.000498259, 0.000353711, 0.0 , 0.000635233, 0.000502344},
				{0.000583318, 0.000449349, 0.0 , 0.000794854, 0.000592722}, };



				static double ff_c_rphi_6[7][5]    = {{-0.00337923, -0.00372118, 0.0 , -0.00249183, -0.00356382},
				{-0.00130802, -0.00427699, 0.0 , -0.00193882, -0.0039764},
				{-0.00259739, -0.00573665, 0.0 , -0.00158471, -0.005683},
				{-0.00280696, -0.00873059, 0.0 , -0.00403219, -0.00628811},
				{-5.82933e-05, -0.00303441, 0.0 , -0.00272292, -0.00397462},
				{-0.00147325, -0.00437742, 0.0 , 0.00184572, -0.00617564},
				{-0.000419438, -0.00412232, 0.0 , -0.000641823, -0.00392717}, };


				static double fferr_c_rphi_6[7][5]    = {{0.000961644, 0.000532886, 0.0 , 0.000744182, 0.000650604},
				{0.00132924, 0.00074315, 0.0 , 0.00107689, 0.000858999},
				{0.00178754, 0.000988606, 0.0 , 0.00151464, 0.00119864},
				{0.00264932, 0.00150105, 0.0 , 0.0021025, 0.00153589},
				{0.00148221, 0.000820274, 0.0 , 0.00116345, 0.000776682},
				{0.00191005, 0.00100073, 0.0 , 0.00162651, 0.000907865},
				{0.00190292, 0.00117491, 0.0 , 0.00185222, 0.00108594},};



				static double ff_c_weta2_6[7][5]    = {{0.000252505, 0.000218329, 0.0 , 0.000382929, 0.000291604},
				{0.00023106, 0.000213747, 0.0 , 0.000397244, 0.000291266},
				{0.000201643, 0.000227043, 0.0 , 0.000398993, 0.000354036},
				{0.000233767, 0.000224724, 0.0 , 0.000349472, 0.00038388},
				{0.000182814, 0.000225122, 0.0 , 0.000404628, 0.000318847},
				{0.000210772, 0.000216773, 0.0 , 0.000303734, 0.000349742},
				{0.000230601, 0.000204442, 0.0 , 0.000374398, 0.000316679},};



				static double fferr_c_weta2_6[7][5]    = {{1.52623e-05, 1.04493e-05, 0.0 , 1.69284e-05, 1.60801e-05},
				{2.01192e-05, 1.44923e-05, 0.0 , 2.4143e-05, 2.21028e-05},
				{2.63298e-05, 1.88004e-05, 0.0 , 3.10065e-05, 2.8914e-05},
				{3.42492e-05, 2.48315e-05, 0.0 , 4.11273e-05, 3.92291e-05},
				{2.00507e-05, 1.47454e-05, 0.0 , 2.58665e-05, 2.33456e-05},
				{2.61439e-05, 1.89452e-05, 0.0 , 3.64184e-05, 3.04342e-05},
				{2.70231e-05, 2.45432e-05, 0.0 , 4.09086e-05, 3.59097e-05},};


				static double ff_c_f1_6[7][5]    = { {0.000368446, -0.00478266, 0.0 , 0.00370052, -0.00231692},
				{-0.00246477, -0.00247312, 0.0 , 0.00462916, 5.70416e-05},
				{-0.00604445, 7.63088e-05, 0.0 , 0.00596452, 0.0042918},
				{-0.00774294, -0.000235543, 0.0 , -0.00514525, 0.0116399},
				{-0.00276661, -0.00597289, 0.0 , 0.00947553, 0.00526825},
				{-0.00364286, -0.0055363, 0.0 , -0.00103086, 0.00672188},
				{-0.00783983, -0.00834531, 0.0 , 0.00130123, 0.0120773},};



				static double fferr_c_f1_6[7][5]    = {{0.00220852, 0.00120696, 0.0 , 0.00172637, 0.00186263},
				{0.00306562, 0.00172392, 0.0 , 0.00237844, 0.00251004},
				{0.00401698, 0.00235807, 0.0 , 0.00331211, 0.00329986},
				{0.00539072, 0.00303192, 0.0 , 0.00446515, 0.00441393},
				{0.00302157, 0.00189038, 0.0 , 0.00279971, 0.0025788},
				{0.00420042, 0.0024854, 0.0 , 0.00366203, 0.00316547},
				{0.00477103, 0.0031934, 0.0 , 0.00476926, 0.00421699},};


				static double ff_c_fside_6[7][5]    = {{0.00598904, 0.0118008, 0.0 , 0.0235514, 0.0173134},
				{0.00749569, 0.0118497, 0.0 , 0.023007, 0.0174804},
				{0.00824425, 0.00892109, 0.0 , 0.0267912, 0.0161329},
				{0.00536759, 0.0110762, 0.0 , 0.0305421, 0.0155456},
				{0.00354651, 0.00915062, 0.0 , 0.0282815, 0.0177007},
				{0.00537814, 0.0110005, 0.0 , 0.0149662, 0.017701},
				{0.000223756, 0.0169593, 0.0 , 0.0230644, 0.020501},};

				static double fferr_c_fside_6[7][5]    = { {0.00113931, 0.00125922, 0.0 , 0.00199368, 0.000739609},
				{0.00161945, 0.00176663, 0.0 , 0.00290682, 0.000993329},
				{0.00199637, 0.00237118, 0.0 , 0.00400193, 0.00131231},
				{0.00258099, 0.00308937, 0.0 , 0.00494151, 0.00181225},
				{0.00168912, 0.00191045, 0.0 , 0.00339227, 0.00108503},
				{0.0021679, 0.00244576, 0.0 , 0.00461884, 0.00140464},
				{0.00261257, 0.00302321, 0.0 , 0.00509222, 0.00168276}, };


				static double ff_c_wtot_6[7][5]    = {{0.083527, 0.0843644, 0.0 , 0.134344, 0.0900393},
				{0.0857077, 0.0896637, 0.0 , 0.137736, 0.0919983},
				{0.0799811, 0.0846255, 0.0 , 0.142004, 0.080995},
				{0.0741138, 0.0656106, 0.0 , 0.148318, 0.0788814},
				{0.0676371, 0.0850337, 0.0 , 0.142771, 0.0927554},
				{0.0811809, 0.0828946, 0.0 , 0.100024, 0.087114},
				{0.0862132, 0.107896, 0.0 , 0.10002, 0.098455}, };


				static double fferr_c_wtot_6[7][5]    = {{0.00687183, 0.00495443, 0.0 , 0.00789185, 0.00489083},
				{0.00909332, 0.00671394, 0.0 , 0.0110889, 0.00653666},
				{0.0113009, 0.00908195, 0.0 , 0.0146493, 0.00880596},
				{0.0150812, 0.0117046, 0.0 , 0.0188373, 0.0117475},
				{0.00883374, 0.00718309, 0.0 , 0.0120317, 0.00680296},
				{0.0113704, 0.0093854, 0.0 , 0.0160523, 0.00919131},
				{0.0127742, 0.011558, 0.0 , 0.0189235, 0.0110085},};


				static double ff_c_w1_6[7][5]    = {{-6.13332e-05, 0.0068903, 0.0 , 0.00985634, 0.00937939},
				{0.00154406, 0.0061298, 0.0 , 0.0109946, 0.0105989},
				{-0.00270736, 0.00608712, 0.0 , 0.0118372, 0.0125299},
				{-0.00190854, 0.00715029, 0.0 , 0.0113262, 0.0112678},
				{0.000574708, 0.00529069, 0.0 , 0.0140837, 0.0120218},
				{0.00195676, 0.00678122, 0.0 , 0.00665319, 0.0124847},
				{0.00320393, 0.00635231, 0.0 , 0.00573289, 0.0143996},};


				static double fferr_c_w1_6[7][5]    = {{0.00101982, 0.00071199, 0.0 , 0.00105337, 0.000822516},
				{0.00141063, 0.00103211, 0.0 , 0.00144677, 0.00113232},
				{0.00175848, 0.0013877, 0.0 , 0.00204322, 0.00169609},
				{0.00260217, 0.00213006, 0.0 , 0.00294236, 0.00219654},
				{0.00144133, 0.00112941, 0.0 , 0.00173323, 0.0012828},
				{0.00194171, 0.00141904, 0.0 , 0.00231145, 0.00164513},
				{0.00227625, 0.00185611, 0.0 , 0.00282896, 0.00206334},};



				static double ff_c_deltae_6[7][5]    = {{-1.79094, -2.57133, 0.0 , -2.24863, -3.73792},
				{-1.5115, -2.03847, 0.0 , -0.00982285, -1.47972},
				{-1.9987, -2.94546, 0.0 , 0.0675926, -5.30027},
				{1.42617, -3.93191, 0.0 , -4.93993, -2.46171},
				{-0.750044, -2.40271, 0.0 , 0.453339, -2.30194},
				{-2.39465, -3.38275, 0.0 , -5.3693, -3.8988},
				{-1.45441, -2.7042, 0.0 , 0.595955, -2.62178},};



				static double fferr_c_deltae_6[7][5]    = {{0.60069, 0.335533, 0.0 , 0.699852, 0.56991},
				{0.910209, 0.485188, 0.0 , 1.04036, 0.816148},
				{1.17031, 0.641129, 0.0 , 1.4057, 1.12184},
				{1.39756, 0.913881, 0.0 , 1.92317, 1.47553},
				{0.974163, 0.55739, 0.0 , 1.17134, 0.900787},
				{1.41451, 0.772769, 0.0 , 1.72302, 1.27034},
				{1.67852, 1.0343, 0.0 , 2.0163, 1.66823},};


				static double ff_c_eratio_6[7][5]    = {{-0.00102967, -0.000872731, 0.0 , -0.00109392, -0.000472844},
				{-0.00233763, -0.00301975, 0.0 , -0.00191003, -0.000523865},
				{-0.00107294, 0.000255704, 0.0 , -0.00556648, -0.000279546},
				{-0.00159854, 0.00178403, 0.0 , -0.00451154, -0.000174999},
				{-0.000805557, -0.00144196, 0.0 , -0.00258666, -0.000482917},
				{0.000160098, 0.000755727, 0.0 , -0.00215554, -0.00022471},
				{-0.000759661, -0.00187027, 0.0 , 0.000405788, -7.83205e-05},};





				static double fferr_c_eratio_6[7][5]    = {{0.000518016, 0.000605417, 0.0 , 0.00108008, 0.000251361},
				{0.000720706, 0.000830384, 0.0 , 0.00135821, 0.000336296},
				{0.000944942, 0.00109428, 0.0 , 0.00164752, 0.00041971},
				{0.00121357, 0.00135954, 0.0 , 0.0021875, 0.000601217},
				{0.000760227, 0.00079646, 0.0 , 0.00124202, 0.000314946},
				{0.000962495, 0.00101926, 0.0 , 0.00152588, 0.000388126},
				{0.000969686, 0.00120705, 0.0 , 0.00210691, 0.000484072}, };

				//unconverted photons

				static double ff_u_rhad1_6[7][5]    = {{-8.8301e-05, 4.13667e-05, 0.0 , 0.000196879, 0.00012337},
				{-4.21288e-05, 4.43923e-05, 0.0 , -7.21961e-06, 7.59102e-05},
				{-1.77301e-05, -5.7772e-05, 0.0 , 4.95088e-05, 6.33604e-05},
				{-9.51737e-05, 6.29071e-05, 0.0 , 0.000251281, 0.000133837},
				{-0.000132471, 3.72914e-05, 0.0 , 0.000110206, 5.68598e-05},
				{-0.000226228, -3.53026e-05, 0.0 , -0.000217972, 9.59373e-06},
				{-0.000362469, 2.6646e-06, 0.0 , -2.42342e-06, -6.74057e-05}, };



				static double fferr_u_rhad1_6[7][5]    = {{4.7419e-05, 2.91177e-05, 0.0 , 9.02063e-05, 3.48156e-05},
				{6.32526e-05, 3.57331e-05, 0.0 , 0.000116989, 4.31612e-05},
				{7.91295e-05, 4.68511e-05, 0.0 , 0.000139123, 5.43192e-05},
				{9.96643e-05, 6.07285e-05, 0.0 , 0.000167087, 6.67061e-05},
				{6.07523e-05, 3.34465e-05, 0.0 , 0.000104967, 4.17527e-05},
				{8.26069e-05, 3.94335e-05, 0.0 , 0.000134042, 4.41779e-05},
				{9.46213e-05, 4.55594e-05, 0.0 , 0.00013532, 5.66025e-05},};


				static double ff_u_rhad_6[7][5]    = { {-0.000172743, 7.75312e-05, 0.0 , 0.000177127, 0.000333271},
				{-8.34609e-05, -1.8605e-05, 0.0 , 0.000417425, 1.8726e-05},
				{-0.000115941, -9.47539e-06, 0.0 , 0.000648175, 7.46891e-05},
				{-3.23398e-05, 4.41519e-05, 0.0 , 0.000720441, 4.29303e-05},
				{-0.000234273, 7.93306e-05, 0.0 , -0.00030062, 0.00012587},
				{-0.000463596, -3.47694e-05, 0.0 , 2.15098e-05, 7.67846e-06},
				{-0.000626607, -2.76716e-05, 0.0 , 0.000223422, -0.000101222},};


				static double fferr_u_rhad_6[7][5]    = {{6.96523e-05, 4.17394e-05, 0.0 , 0.000224351, 0.000105698},
				{9.287e-05, 5.25613e-05, 0.0 , 0.000285414, 0.00013248},
				{0.000114129, 7.12359e-05, 0.0 , 0.000354506, 0.000160634},
				{0.000142448, 9.23203e-05, 0.0 , 0.000438622, 0.000198961},
				{8.44495e-05, 4.63366e-05, 0.0 , 0.00024735, 0.000117175},
				{0.000113389, 5.7469e-05, 0.0 , 0.000284487, 0.000127639},
				{0.000120495, 6.1058e-05, 0.0 , 0.000254935, 0.000122383},};

				static double ff_u_e277_6[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};

				static double fferr_u_e277_6[7][5]= {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0.,0.},};

				static double ff_u_reta_6[7][5]    = {{-0.00366378, -0.00549769, 0.0 , -0.00894433, -0.00811458},
				{-0.00367469, -0.00569004, 0.0 , -0.00806034, -0.00895441},
				{-0.00437105, -0.00593686, 0.0 , -0.00821769, -0.00824064},
				{-0.00401056, -0.00627649, 0.0 , -0.00697124, -0.00794643},
				{-0.00387502, -0.00613934, 0.0 , -0.00795674, -0.00869316},
				{-0.00411463, -0.00604266, 0.0 , -0.00840306, -0.00893772},
				{-0.0042116, -0.00585538, 0.0 , -0.00974739, -0.00817728},};


				static double fferr_u_reta_6[7][5]    = {{0.000173318, 0.000171127, 0.0 , 0.0003309, 0.00024761},
				{0.000230635, 0.000230576, 0.0 , 0.000465602, 0.000345894},
				{0.000299025, 0.000299526, 0.0 , 0.000599759, 0.000471864},
				{0.000380974, 0.000396946, 0.0 , 0.000824447, 0.000617394},
				{0.000223037, 0.000248744, 0.0 , 0.000490614, 0.000396539},
				{0.00027224, 0.000299249, 0.0 , 0.000671666, 0.000502256},
				{0.000303967, 0.000348037, 0.0 , 0.000764864, 0.000608079},};


				static double ff_u_rphi_6[7][5]    = {{-0.00228322, -0.00267363, 0.0 , -0.00396192, -0.00557011},
				{-0.00234568, -0.00311702, 0.0 , -0.00400412, -0.00582641},
				{-0.00254977, -0.0031876, 0.0 , -0.00442159, -0.00603098},
				{-0.00233001, -0.00452781, 0.0 , -0.00377589, -0.00450641},
				{-0.00237674, -0.00355357, 0.0 , -0.00367111, -0.00597888},
				{-0.00184011, -0.00422019, 0.0 , -0.00265265, -0.00606906},
				{-0.0025022, -0.00374079, 0.0 , -0.00471514, -0.00572056},};


				static double fferr_u_rphi_6[7][5]    = {{0.000148759, 0.000286765, 0.0 , 0.000362106, 0.000214178},
				{0.000198533, 0.000390484, 0.0 , 0.000513084, 0.000293315},
				{0.000260164, 0.000529772, 0.0 , 0.000633101, 0.000399787},
				{0.000327586, 0.000660265, 0.0 , 0.000916564, 0.000528653},
				{0.000198257, 0.000390169, 0.0 , 0.000524952, 0.000323485},
				{0.000251305, 0.000463755, 0.0 , 0.000760845, 0.000431257},
				{0.000263636, 0.000600063, 0.0 , 0.000878168, 0.000519912},};


				static double ff_u_weta2_6[7][5]    = { {0.00021572, 0.000192606, 0.0 , 0.00037313, 0.000279943},
				{0.000210023, 0.00019404, 0.0 , 0.000417031, 0.000331435},
				{0.00022574, 0.000179339, 0.0 , 0.000371424, 0.000302454},
				{0.000223174, 0.000235461, 0.0 , 0.000392343, 0.000256074},
				{0.000226799, 0.00021381, 0.0 , 0.000381879, 0.000350356},
				{0.000209319, 0.000230552, 0.0 , 0.000399353, 0.000301356},
				{0.00023662, 0.00022297, 0.0 , 0.000428687, 0.000334849},};

				static double fferr_u_weta2_6[7][5]    = {{8.92788e-06, 9.65046e-06, 0.0 , 1.72105e-05, 1.60788e-05},
				{1.24007e-05, 1.38537e-05, 0.0 , 2.53546e-05, 2.2667e-05},
				{1.61591e-05, 1.79365e-05, 0.0 , 3.2098e-05, 3.03892e-05},
				{2.12528e-05, 2.41462e-05, 0.0 , 4.11572e-05, 3.78873e-05},
				{1.17023e-05, 1.48125e-05, 0.0 , 2.73867e-05, 2.63405e-05},
				{1.48958e-05, 1.71646e-05, 0.0 , 4.16903e-05, 3.4162e-05},
				{1.61405e-05, 2.16772e-05, 0.0 , 4.51883e-05, 4.21587e-05},};

				static double ff_u_f1_6[7][5]    = { {-0.00963479, -0.00958504, 0.0 , 0.000500143, -0.0128902},
				{-0.0105253, -0.0105872, 0.0 , 0.00213557, -0.00567541},
				{-0.00605989, -0.01298, 0.0 , 0.0040608, -0.00626367},
				{0.000836447, -0.00628737, 0.0 , -0.00314987, -0.01053},
				{-0.00660242, -0.0073587, 0.0 , 0.00522235, -5.23031e-05},
				{-0.00555645, -0.00763248, 0.0 , -0.00629608, -0.00260742},
				{-0.00189854, -0.00343284, 0.0 , 0.00809859, 0.00374486}, };


				static double fferr_u_f1_6[7][5]    = {{0.00181989, 0.00159146, 0.0 , 0.0030726, 0.00227248},
				{0.00248449, 0.00226844, 0.0 , 0.00435752, 0.00308065},
				{0.0033692, 0.00309337, 0.0 , 0.00567089, 0.00420573},
				{0.0045225, 0.00420014, 0.0 , 0.00790907, 0.00545678},
				{0.00265776, 0.00251332, 0.0 , 0.00477587, 0.00330051},
				{0.00336825, 0.00325396, 0.0 , 0.0070597, 0.00431618},
				{0.00385207, 0.00363916, 0.0 , 0.00875376, 0.00522819},};


				static double ff_u_fside_6[7][5]    = { {0.00234464, 0.00697544, 0.0 , 0.0215667, 0.0127861},
				{0.00273053, 0.00533849, 0.0 , 0.0279927, 0.0115447},
				{0.00437392, 0.00471902, 0.0 , 0.0164502, 0.0126532},
				{0.00473291, 0.00959222, 0.0 , 0.0237407, 0.0143335},
				{0.00435549, 0.00738782, 0.0 , 0.0129167, 0.0133368},
				{0.00265414, 0.00741628, 0.0 , 0.0150797, 0.0156918},
				{0.00138649, 0.00969791, 0.0 , 0.0330444, 0.01613},};


				static double fferr_u_fside_6[7][5]    = {{0.000794938, 0.00122649, 0.0 , 0.00273435, 0.000687522},
				{0.00111667, 0.00178986, 0.0 , 0.00373609, 0.000921139},
				{0.00147238, 0.00237509, 0.0 , 0.00537533, 0.00125777},
				{0.00190342, 0.00308183, 0.0 , 0.00733767, 0.00177199},
				{0.00111746, 0.00193551, 0.0 , 0.00500778, 0.00105224},
				{0.00148881, 0.00244399, 0.0 , 0.00595083, 0.00133161},
				{0.00161426, 0.00275363, 0.0 , 0.00567668, 0.00157653},};



				static double ff_u_wtot_6[7][5]    = {{0.0826484, 0.0707967, 0.0 , 0.131546, 0.0707275},
				{0.0823569, 0.0830004, 0.0 , 0.132095, 0.0629014},
				{0.068885, 0.0828984, 0.0 , 0.123251, 0.0750329},
				{0.0809344, 0.0831051, 0.0 , 0.117672, 0.107787},
				{0.0889562, 0.0867121, 0.0 , 0.132672, 0.0772079},
				{0.0714976, 0.0952959, 0.0 , 0.0989267, 0.0873506},
				{0.0755293, 0.0870845, 0.0 , 0.175571, 0.0941253},};



				static double fferr_u_wtot_6[7][5]    = {{0.00631776, 0.00592309, 0.0 , 0.0101564, 0.00533505},
				{0.00798361, 0.0080853, 0.0 , 0.0141936, 0.00718742},
				{0.0100697, 0.0104684, 0.0 , 0.0182264, 0.00980944},
				{0.0136211, 0.0147247, 0.0 , 0.0258371, 0.0127273},
				{0.00752363, 0.00864217, 0.0 , 0.0151804, 0.00847471},
				{0.0103686, 0.0103819, 0.0 , 0.0205936, 0.0101227},
				{0.0107177, 0.0128497, 0.0 , 0.0220739, 0.0131192},};


				static double ff_u_w1_6[7][5]    = {{-0.00291431, 0.00399804, 0.0 , 0.0109493, 0.00788486},
				{-0.00465214, 0.00461507, 0.0 , 0.0143943, 0.00749362},
				{-0.00236249, 0.0024665, 0.0 , 0.0121182, 0.00782889},
				{0.000341415, 0.00252134, 0.0 , 0.0134298, 0.0100906},
				{-0.00295681, 0.0048486, 0.0 , 0.010397, 0.00861442},
				{-0.00372916, 0.00641656, 0.0 , 0.00792646, 0.0077427},
				{-0.00209254, 0.0053491, 0.0 , 0.018879, 0.0130733},};


				static double fferr_u_w1_6[7][5]    = {{0.000705578, 0.000829402, 0.0 , 0.0015079, 0.000842752},
				{0.000989863, 0.00118621, 0.0 , 0.00211934, 0.00117302},
				{0.00134433, 0.00162133, 0.0 , 0.00274172, 0.00163715},
				{0.00186507, 0.00222154, 0.0 , 0.00371212, 0.00204384},
				{0.00102805, 0.00134575, 0.0 , 0.00234392, 0.00129001},
				{0.00138023, 0.00174649, 0.0 , 0.00356797, 0.00136693},
				{0.0015646, 0.00205148, 0.0 , 0.0043349, 0.00213642},};


				static double ff_u_deltae_6[7][5]    = {{-0.990072, -2.0389, 0.0 , -0.388374, -2.79085},
				{-0.556639, -0.998913, 0.0 , -1.8284, -5.46291},
				{-1.13878, -1.19105, 0.0 , -1.53109, -2.05046},
				{-1.51247, -2.45515, 0.0 , -0.278374, -1.89966},
				{-1.57008, -1.32901, 0.0 , -0.379349, -4.70352},
				{-1.86283, -0.682304, 0.0 , 0.163847, -4.69242},
				{-0.345964, -0.429018, 0.0 , 1.06159, -5.18904},};


				static double fferr_u_deltae_6[7][5]    = {{0.350171, 0.32171, 0.0 , 0.765029, 0.747132},
				{0.499551, 0.442272, 0.0 , 1.07983, 1.07147},
				{0.675982, 0.630218, 0.0 , 1.36266, 1.4108},
				{0.906836, 0.893753, 0.0 , 1.91551, 1.95996},
				{0.556208, 0.533132, 0.0 , 1.2427, 1.25555},
				{0.717394, 0.657901, 0.0 , 1.62081, 1.6645},
				{0.852775, 0.812839, 0.0 , 2.01281, 2.30642},};


				static double ff_u_eratio_6[7][5]    = { {-0.00121582, -0.00134927, 0.0 , -0.0027138, -0.00117087},
				{-0.00180954, -0.00348663, 0.0 , -0.000341058, -0.000133336},
				{0.000765979, -0.00257868, 0.0 , -0.0013817, -0.00185591},
				{1.64509e-05, -0.00387526, 0.0 , -0.00219774, -0.00209725},
				{-0.00206548, -0.0021345, 0.0 , -0.00200725, -0.000609636},
				{-0.000984132, -0.00183743, 0.0 , -0.000486851, -0.0013693},
				{0.00102633, -0.00139433, 0.0 , -0.00468588, 0.000887215},};


				static double fferr_u_eratio_6[7][5]    = { {0.000747718, 0.000544724, 0.0 , 0.000683756, 0.000586377},
				{0.00100832, 0.000734527, 0.0 , 0.000940206, 0.000779807},
				{0.00126769, 0.00099448, 0.0 , 0.00112185, 0.0010734},
				{0.00176877, 0.0012664, 0.0 , 0.00164692, 0.00122649},
				{0.000918173, 0.000718001, 0.0 , 0.000937114, 0.000782378},
				{0.00118065, 0.000993922, 0.0 , 0.00119048, 0.000811676},
				{0.00153656, 0.00099427, 0.0 , 0.000917349, 0.0011207},};

				if(isConv){
				m_rhad1_ff.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_rhad1_6[0][0],  0. );
				m_rhad_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_rhad_6[0][0],   0. );
				m_e277_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_e277_6[0][0],   0. );
				m_reta_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_reta_6[0][0],   0. );
				m_rphi_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_rphi_6[0][0],   0. );
				m_weta2_ff.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_weta2_6[0][0],  0. );
				m_f1_ff.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_f1_6[0][0],     0. );
				m_fside_ff.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_fside_6[0][0],  0. );
				m_wtot_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_wtot_6[0][0] ,  0. );
				m_w1_ff.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_w1_6[0][0],     0. );
				m_deltae_ff.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_deltae_6[0][0], 0. );
				m_eratio_ff.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_c_eratio_6[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_rhad1_6[0][0],  0. );
				m_rhad_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_rhad_6[0][0],   0. );
				m_e277_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_e277_6[0][0],   0. );
				m_reta_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_reta_6[0][0],   0. );
				m_rphi_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_rphi_6[0][0],   0. );
				m_weta2_fferr.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_weta2_6[0][0],  0. );
				m_f1_fferr.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_f1_6[0][0],     0. );
				m_fside_fferr.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_fside_6[0][0],  0. );
				m_wtot_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_wtot_6[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_w1_6[0][0],     0. );
				m_deltae_fferr.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_deltae_6[0][0], 0. );
				m_eratio_fferr.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_c_eratio_6[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_rhad1_6[0][0],  0. );
				m_rhad_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_rhad_6[0][0],   0. );
				m_e277_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_e277_6[0][0],   0. );
				m_reta_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_reta_6[0][0],   0. );
				m_rphi_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_rphi_6[0][0],   0. );
				m_weta2_ff.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_weta2_6[0][0],  0. );
				m_f1_ff.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_f1_6[0][0],     0. );
				m_fside_ff.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_fside_6[0][0],  0. );
				m_wtot_ff.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_wtot_6[0][0],   0. );
				m_w1_ff.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_w1_6[0][0],     0. );
				m_deltae_ff.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_deltae_6[0][0], 0. );
				m_eratio_ff.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &ff_u_eratio_6[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_rhad1_6[0][0],  0. );
				m_rhad_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_rhad_6[0][0],   0. );
				m_e277_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_e277_6[0][0],   0. );
				m_reta_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_reta_6[0][0],   0. );
				m_rphi_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_rphi_6[0][0],   0. );
				m_weta2_fferr.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_weta2_6[0][0],  0. );
				m_f1_fferr.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_f1_6[0][0],     0. );
				m_fside_fferr.Initialize  ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_fside_6[0][0],  0. );
				m_wtot_fferr.Initialize   ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_wtot_6[0][0],   0. );
				m_w1_fferr.Initialize     ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_w1_6[0][0],     0. );
				m_deltae_fferr.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_deltae_6[0][0], 0. );
				m_eratio_fferr.Initialize ( 7, 5, pt_DVs_6, eta_DVs_6, &fferr_u_eratio_6[0][0], 0. );
				}

				break;

		case 7:  // FF from 2011 data and mc11a (only JF17)

				static double pt_DVs_7[4] = {32000,40000,55000,10000000};// in MeV
				static double eta_DVs_7[5] = { 0.6, 1.37, 1.52, 1.81, 2.37}; // eta binning according to SMDP inclusive analysis

				//converted photons

				static double ff_c_rhad1_7[4][5]    = { {-0.000115341, 7.9179e-05, 0.0 , 0.000183079, -2.04553e-05},
				{0.000208956, -2.04483e-05, 0.0 , 0.000426582, 1.1641e-05},
				{-0.000221827, 1.05538e-05, 0.0 , -0.000227389, -1.17118e-05},
				{0.000263472, -0.000284689, 0.0 , 0.000149428, 7.03151e-06}, };

				static double fferr_c_rhad1_7[4][5]    = { {0.000182122, 6.5948e-05, 0.0 , 0.000161694, 7.80489e-05},
				{0.000260966, 7.85073e-05, 0.0 , 0.000233492, 0.000105232},
				{0.000213617, 9.5323e-05, 0.0 , 0.000263454, 0.000112682},
				{0.000229768, 0.000116156, 0.0 , 0.000291231, 0.000140561}, };



				static double ff_c_rhad_7[4][5]    = { {0.000166558, 0.000134726, 0.0 , 0.000728627, 0.000431463},
				{0.000465789, 9.65891e-05, 0.0 , 0.00102688, -0.000368376},
				{-0.000774624, -0.00014933, 0.0 , -0.000169836, -0.000323386},
				{0.000306686, -0.000300355, 0.0 , 0.000595559, 1.98195e-05}, };




				static double fferr_c_rhad_7[4][5]    = {  {0.000274374, 8.57021e-05, 0.0 , 0.000456693, 0.000264883},
				{0.000339691, 0.000104433, 0.0 , 0.000525924, 0.000263176},
				{0.000382824, 0.000141217, 0.0 , 0.000452184, 0.000330064},
				{0.000375983, 0.000139167, 0.0 , 0.000678427, 0.000438496},};

				/*
				static double ff_c_e277_0[7][5]    = {{81.1914, 388.422, 0.0 , 181.961, -109.445},
				{-95.0898, -168.545, 0.0 , 1185.17, -264.789},
				{-287.219, 629.426, 0.0 , -875.207, -568.484},
				{917.502, 2498.71, 0.0 , -2801.07, -1516.09},
				{-346.707, 1055.13, 0.0 , -2523.62, -1829.48},
				{-160.855, 1785.84, 0.0 , -506.812, -1501.34},
				{-2401.08, 1768.79, 0.0 , -5566.27, -10223.9},};



				static double fferr_c_e277_0[7][5]    = {{187.903, 224.875, 0.0 , 711.477, 763.533, },
				{312.838, 412.172, 0.0 , 1357.94, 1103.68, },
				{379.886, 572.68, 0.0 , 2025.91, 1660.41, },
				{537.662, 899.029, 0.0 , 2762.42, 2810.67, },
				{376.662, 462.842, 0.0 , 1767.99, 1586.05, },
				{552.718, 764.483, 0.0 , 2444.53, 2487.72, },
				{1145.48, 1469.2, 0.0 , 4994.09, 4438.99, },};
				*/

				static double ff_c_e277_7[4][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},};



				static double fferr_c_e277_7[4][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},};


				static double ff_c_reta_7[4][5]    = {  {-0.00117791, -0.00463533, 0.0 , -0.00513804, -0.004448},
				{1.89543e-05, -0.00342321, 0.0 , -0.00718516, -0.00412279},
				{-0.00240582, -0.00254136, 0.0 , -0.00544208, -0.00450766},
				{0.000914454, -0.00361145, 0.0 , -0.00451398, -0.00385118},  };

				static double fferr_c_reta_7[4][5]    = { {0.000792023, 0.00048938, 0.0 , 0.000775675, 0.000628652},
				{0.00107794, 0.000685288, 0.0 , 0.000991568, 0.000981775},
				{0.00117321, 0.000917605, 0.0 , 0.00157825, 0.00110868},
				{0.00136213, 0.00109965, 0.0 , 0.00163117, 0.00197757},  };


				static double ff_c_rphi_7[4][5]    = { {-0.00436318, -0.00315499, 0.0 , -0.00259811, -0.0018571},
				{-0.00800961, -0.00499499, 0.0 , 0.00365806, 0.00138605},
				{0.00525224, -0.00340825, 0.0 , -0.00532013, -0.00198072},
				{-0.00141841, -0.00793815, 0.0 , -0.00527567, 0.00238407},};


				static double fferr_c_rphi_7[4][5]    = {{0.0023341, 0.00143596, 0.0 , 0.00179497, 0.00158279},
				{0.00376281, 0.00203389, 0.0 , 0.00318694, 0.00241342},
				{0.0044276, 0.0027127, 0.0 , 0.00371084, 0.00250849},
				{0.00582281, 0.00279169, 0.0 , 0.00449986, 0.00364672},  };


				static double ff_c_weta2_7[4][5]    = { {4.06289e-05, 0.000105708, 0.0 , 0.000196953, 0.000169733},
				{0.000146789, 2.26274e-05, 0.0 , 0.000254613, 0.000182374},
				{1.04215e-06, 0.000107157, 0.0 , 0.000373387, 0.000291882},
				{8.85297e-05, -1.46143e-05, 0.0 , 0.000283334, 0.000247911},};




				static double fferr_c_weta2_7[4][5]    = { {3.65436e-05, 2.8449e-05, 0.0 , 4.75092e-05, 3.97883e-05},
				{5.06933e-05, 3.97229e-05, 0.0 , 6.93419e-05, 5.98066e-05},
				{5.21814e-05, 4.82838e-05, 0.0 , 7.74854e-05, 7.47565e-05},
				{7.93715e-05, 6.9353e-05, 0.0 , 0.000111837, 0.000124727}, };


				static double ff_c_f1_7[4][5]    = {{-0.00505617, 0.000256687, 0.0 , 0.00612605, 0.00275639},
				{0.00983894, -0.00907424, 0.0 , 0.00410357, 0.00658485},
				{-0.00189453, -0.00184377, 0.0 , 0.0220071, 0.0112966},
				{-0.00817838, 0.00889303, 0.0 , -0.00309607, 0.00806846}, };




				static double fferr_c_f1_7[4][5]    = {  {0.00531603, 0.00317229, 0.0 , 0.00406838, 0.00441459},
				{0.00752569, 0.00521096, 0.0 , 0.0062567, 0.00664967},
				{0.00934356, 0.00624534, 0.0 , 0.0105152, 0.00767373},
				{0.0108164, 0.00892328, 0.0 , 0.0110562, 0.0131196}, };


				static double ff_c_fside_7[4][5]    = {  {0.00731383, 0.0180469, 0.0 , 0.0271695, 0.0176866},
				{0.0158292, 0.0136524, 0.0 , 0.026042, 0.0189009},
				{0.00789462, 0.0242877, 0.0 , 0.0331409, 0.01835},
				{0.00353742, 0.00753248, 0.0 , 0.0274068, 0.0218314},};

				static double fferr_c_fside_7[4][5]    = { {0.00269666, 0.00348315, 0.0 , 0.00525496, 0.00168581},
				{0.00402909, 0.00531844, 0.0 , 0.0083305, 0.00240267},
				{0.00479765, 0.00618024, 0.0 , 0.0102801, 0.00360342},
				{0.00618005, 0.00857332, 0.0 , 0.0194901, 0.00532003}, };


				static double ff_c_wtot_7[4][5]    = { {0.0528491, 0.0670764, 0.0 , 0.0985484, 0.0781269},
				{0.0768001, 0.0821593, 0.0 , 0.111961, 0.0666037},
				{0.0485626, 0.0996304, 0.0 , 0.123444, 0.061242},
				{0.046804, 0.071511, 0.0 , 0.15518, 0.0588564}, };


				static double fferr_c_wtot_7[4][5]    = {{0.0162756, 0.0132118, 0.0 , 0.0209954, 0.0117385},
				{0.0217413, 0.0195445, 0.0 , 0.0290633, 0.0178655},
				{0.0233568, 0.0229781, 0.0 , 0.0370852, 0.0239335},
				{0.0293624, 0.0338072, 0.0 , 0.0511994, 0.0426186},  };



				static double ff_c_w1_7[4][5]    = { {0.00148994, 0.0125996, 0.0 , 0.0101088, 0.0108731},
				{0.0106617, 0.00921446, 0.0 , 0.00747389, 0.0147852},
				{-0.00107104, 0.0142428, 0.0 , 0.0105024, 0.020638},
				{0.00102377, 0.00825381, 0.0 , 0.0183247, 0.0111766}, };


				static double fferr_c_w1_7[4][5]    = { {0.00232131, 0.00201352, 0.0 , 0.00260117, 0.00201535},
				{0.00367887, 0.00294962, 0.0 , 0.0041104, 0.00309298},
				{0.00420004, 0.00396078, 0.0 , 0.00598579, 0.00367124},
				{0.00570045, 0.00639889, 0.0 , 0.00668316, 0.00771474},};


				static double ff_c_deltae_7[4][5]    = {  {-0.846436, -1.48269, 0.0 , -2.15296, -3.10045},
				{-2.98165, -3.16942, 0.0 , -1.91703, -1.0025},
				{-1.24554, -0.635353, 0.0 , 0.624607, -3.55769},
				{2.41858, -3.40684, 0.0 , -3.29491, 1.66166},};



				static double fferr_c_deltae_7[4][5]    = {  {1.4765, 0.907891, 0.0 , 1.82608, 1.49118},
				{2.35954, 1.49362, 0.0 , 3.09432, 2.00115},
				{2.85332, 1.81813, 0.0 , 3.86544, 2.97416},
				{3.95268, 2.71669, 0.0 , 6.60607, 4.19232},  };


				static double ff_c_eratio_7[4][5]    = { {-0.000646353, 0.00106817, 0.0 , 0.000671327, 0.000188112},
				{0.00288039, -0.000504434, 0.0 , -0.00366604, 0.000579059},
				{0.000649035, -0.00370282, 0.0 , -0.00190353, 0.00165927},
				{4.19617e-05, 0.00118208, 0.0 , -0.00818491, 0.00334257}, };




				static double fferr_c_eratio_7[4][5]    = {  {0.00128908, 0.00162777, 0.0 , 0.00278315, 0.000606924},
				{0.00209552, 0.00239272, 0.0 , 0.00374331, 0.000898643},
				{0.00229441, 0.00255406, 0.0 , 0.00397307, 0.00109313},
				{0.00295036, 0.00349426, 0.0 , 0.00206807, 0.00255514}, };

				//unconverted photons

				static double ff_u_rhad1_7[4][5]    = {  {2.09542e-05, 9.11428e-05, 0.0 , -0.00012929, 4.85194e-05},
				{-6.52354e-05, -6.87666e-05, 0.0 , -5.34788e-05, -2.54971e-05},
				{-0.000321734, 8.40735e-05, 0.0 , 0.000369839, -2.32142e-05},
				{2.59529e-05, 4.22495e-05, 0.0 , 7.88021e-05, 1.58408e-05}, };



				static double fferr_u_rhad1_7[4][5]    = { {9.57154e-05, 5.74897e-05, 0.0 , 0.000198915, 6.69868e-05},
				{0.000160732, 7.23167e-05, 0.0 , 0.000279616, 9.31733e-05},
				{0.000157892, 8.42149e-05, 0.0 , 0.000313124, 9.1331e-05},
				{0.000210617, 9.35428e-05, 0.0 , 0.000387508, 0.00013095},};


				static double ff_u_rhad_7[4][5]    = { {-2.39432e-05, -1.54816e-05, 0.0 , -8.9502e-05, 0.000106628},
				{0.000111435, -4.85075e-05, 0.0 , 6.2875e-05, -5.25723e-05},
				{-0.000682296, 6.28384e-06, 0.0 , 0.000688478, -4.0179e-05},
				{-0.000140851, -2.10012e-05, 0.0 , 0.000346026, 0.000259814}, };


				static double fferr_u_rhad_7[4][5]    = {  {0.00014265, 8.3872e-05, 0.0 , 0.00047349, 0.000212119},
				{0.000215925, 0.000105642, 0.0 , 0.000569981, 0.000321728},
				{0.000238689, 0.000128185, 0.0 , 0.00085122, 0.000280668},
				{0.000260633, 0.000151161, 0.0 , 0.000624484, 0.000281327},  };


				/*
				static double ff_u_e277_0[7][5]    = {{425.203, 1024.02, 0.0 , -23.375, 2288.88},
				{310.352, 631.141, 0.0 , 1871.47, 2008.77},
				{440.285, 1113.95, 0.0 , 1894.77, -1535.41},
				{198.871, -463.273, 0.0 , -7692.29, -1407.67},
				{79.7227, 533.355, 0.0 , 60.6719, -3502.28},
				{-153.34, 1583.26, 0.0 , 1582.92, 1331.8},
				{612.473, 2587.91, 0.0 , -6556.98, 2220.34},};


				static double fferr_u_e277_0[7][5]    = {{123.581, 241.947, 0.0 , 832.081, 812.67, },
				{196.094, 420.303, 0.0 , 1525.54, 1289.17, },
				{334.752, 641.21, 0.0 , 2398.77, 2083.63, },
				{497.225, 911.407, 0.0 , 3022.47, 3190.71, },
				{239.435, 478.502, 0.0 , 1898.26, 1813.12, },
				{394.463, 827.943, 0.0 , 2910.44, 2496.09, },
				{667.093, 1549.79, 0.0 , 5711.67, 3711.84, },};
				*/

				static double ff_u_e277_7[4][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},};

				static double fferr_u_e277_7[4][5]= {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},};

				static double ff_u_reta_7[4][5]    = { {-0.000986934, -0.00303918, 0.0 , -0.00431269, -0.00399482},
				{-0.00111175, -0.00318986, 0.0 , -0.00589311, -0.00516045},
				{-0.000721991, -0.00171798, 0.0 , -0.00344133, -0.00483418},
				{-0.00271487, -0.00284016, 0.0 , -0.00278693, -0.00514698}, };



				static double fferr_u_reta_7[4][5]    = { {0.000366072, 0.000349249, 0.0 , 0.000655075, 0.000495957},
				{0.000469511, 0.000445211, 0.0 , 0.000794088, 0.000726685},
				{0.000638983, 0.000568224, 0.0 , 0.00127496, 0.000873835},
				{0.00067265, 0.00081898, 0.0 , 0.0017964, 0.00133877},};


				static double ff_u_rphi_7[4][5]    = { {0.000258982, -0.000718772, 0.0 , -0.000903487, -0.00324732},
				{-1.99676e-05, -0.000681102, 0.0 , -0.00195682, -0.00272709},
				{-5.38826e-05, 0.000640333, 0.0 , -0.000277877, -0.00201637},
				{-3.85642e-05, -0.00239801, 0.0 , 0.000263095, -0.00382298}, };


				static double fferr_u_rphi_7[4][5]    = { {0.000316622, 0.000589134, 0.0 , 0.000704048, 0.00041794},
				{0.000434776, 0.000804766, 0.0 , 0.00102642, 0.000650625},
				{0.000555446, 0.00105101, 0.0 , 0.00147893, 0.000776805},
				{0.000689693, 0.00101544, 0.0 , 0.00198564, 0.00104908}, };


				static double ff_u_weta2_7[4][5]    = {  {9.07686e-05, 3.84971e-05, 0.0 , 0.000225541, 0.000200179},
				{7.82106e-05, 6.08461e-05, 0.0 , 0.000324818, 0.000230847},
				{3.03565e-05, -3.47011e-05, 0.0 , 0.000150586, 0.000144219},
				{0.000124439, 3.83221e-05, 0.0 , 0.000194746, 0.00024464}, };

				static double fferr_u_weta2_7[4][5]    = {  {1.90704e-05, 2.10436e-05, 0.0 , 3.56051e-05, 3.30201e-05},
				{2.83041e-05, 2.79889e-05, 0.0 , 4.91089e-05, 5.09606e-05},
				{3.4859e-05, 4.08683e-05, 0.0 , 7.21288e-05, 5.95159e-05},
				{4.06526e-05, 5.29969e-05, 0.0 , 0.000102853, 8.45934e-05}, };


				static double ff_u_f1_7[4][5]    = { {-0.00822738, -0.00866558, 0.0 , 0.00485265, -0.0136346},
				{-0.00557503, -0.00960875, 0.0 , 0.0128653, -0.000973687},
				{-0.013408, -0.0195063, 0.0 , -0.0134024, -0.0012418},
				{0.0102998, 0.00640304, 0.0 , -0.0274208, 0.0101596}, };


				static double fferr_u_f1_7[4][5]    = { {0.00379482, 0.00335976, 0.0 , 0.0063366, 0.00439043},
				{0.00582831, 0.00498009, 0.0 , 0.00886354, 0.0065094},
				{0.00751738, 0.00620629, 0.0 , 0.0133547, 0.00843059},
				{0.00957007, 0.00916397, 0.0 , 0.0211889, 0.0106011}, };



				static double ff_u_fside_7[4][5]    = {  {0.00507322, 0.00587371, 0.0 , 0.022591, 0.0100096},
				{0.00525908, 0.00681987, 0.0 , 0.0282299, 0.0122015},
				{0.00135967, 0.00343853, 0.0 , 0.023907, 0.0140523},
				{0.0107659, 0.011184, 0.0 , 0.0147829, 0.0202766},  };


				static double fferr_u_fside_7[4][5]    = {  {0.00168112, 0.00269693, 0.0 , 0.00594561, 0.00133165},
				{0.00243673, 0.00380798, 0.0 , 0.00788988, 0.00213459},
				{0.00321361, 0.00461013, 0.0 , 0.00917073, 0.00254068},
				{0.00385318, 0.00613277, 0.0 , 0.0118916, 0.00404182},};


				static double ff_u_wtot_7[4][5]    = { {0.0445555, 0.0423195, 0.0 , 0.0613804, 0.0150074},
				{0.0402241, 0.0487499, 0.0 , 0.121982, 0.0410672},
				{0.0408473, 0.0418763, 0.0 , 0.0767634, 0.0712088},
				{0.0652323, 0.0422745, 0.0 , 0.0818932, 0.106668},  };



				static double fferr_u_wtot_7[4][5]    = { {0.0120257, 0.0118838, 0.0 , 0.0203143, 0.0114061},
				{0.0182637, 0.0162639, 0.0 , 0.031742, 0.0170533},
				{0.0201618, 0.023449, 0.0 , 0.0330825, 0.0210657},
				{0.0273191, 0.0358146, 0.0 , 0.0409581, 0.0312536},};


				static double ff_u_w1_7[4][5]    = { {-0.00339842, 0.00639588, 0.0 , 0.0153413, 0.00805432},
				{-0.00260937, 0.00415772, 0.0 , 0.0159622, 0.00888067},
				{-0.000139236, 0.00224656, 0.0 , 0.00357151, 0.0131841},
				{0.0115504, 0.0145245, 0.0 , 0.00377256, 0.0185137},  };


				static double fferr_u_w1_7[4][5]    = { {0.00148821, 0.00175768, 0.0 , 0.00319742, 0.00159771},
				{0.00213972, 0.00263263, 0.0 , 0.0042075, 0.00264062},
				{0.00299434, 0.00341531, 0.0 , 0.00632907, 0.00336719},
				{0.00407838, 0.00506476, 0.0 , 0.00693018, 0.00411042}, };


				static double ff_u_deltae_7[4][5]    = {  {-1.50904, -0.800547, 0.0 , 0.0483303, -3.36057},
				{0.493063, 0.0984821, 0.0 , -1.07018, -4.12056},
				{1.17537, -3.59494, 0.0 , -1.81272, -8.2636},
				{0.0296745, -1.62892, 0.0 , 9.48856, -1.85645}, };


				static double fferr_u_deltae_7[4][5]    = { {0.74429, 0.684939, 0.0 , 1.53355, 1.44787},
				{1.13105, 0.952896, 0.0 , 2.2038, 2.38556},
				{1.47291, 1.45749, 0.0 , 3.42465, 3.03937},
				{1.968, 2.04576, 0.0 , 4.11792, 4.44149},  };




				static double ff_u_eratio_7[4][5]    = { {7.59363e-05, -0.00223786, 0.0 , -1.7643e-05, 0.000183582},
				{-0.000650704, 0.000395477, 0.0 , 0.00143629, 0.00151801},
				{-0.00268543, -0.00306231, 0.0 , -0.00322545, 0.00138468},
				{0.00468481, 0.00430018, 0.0 , -0.00167191, -0.00252634}, };


				static double fferr_u_eratio_7[4][5]    = {  {0.00156513, 0.00110672, 0.0 , 0.00135112, 0.00120458},
				{0.00226342, 0.00166716, 0.0 , 0.00196191, 0.00179111},
				{0.00249364, 0.0018328, 0.0 , 0.00194905, 0.00196996},
				{0.00366904, 0.00316529, 0.0 , 0.00242403, 0.00151911}, };


				if(isConv){
				m_rhad1_ff.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_rhad1_7[0][0],  0. );
				m_rhad_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_rhad_7[0][0],   0. );
				m_e277_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_e277_7[0][0],   0. );
				m_reta_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_reta_7[0][0],   0. );
				m_rphi_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_rphi_7[0][0],   0. );
				m_weta2_ff.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_weta2_7[0][0],  0. );
				m_f1_ff.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_f1_7[0][0],     0. );
				m_fside_ff.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_fside_7[0][0],  0. );
				m_wtot_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_wtot_7[0][0] ,  0. );
				m_w1_ff.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_w1_7[0][0],     0. );
				m_deltae_ff.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_deltae_7[0][0], 0. );
				m_eratio_ff.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_c_eratio_7[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_rhad1_7[0][0],  0. );
				m_rhad_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_rhad_7[0][0],   0. );
				m_e277_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_e277_7[0][0],   0. );
				m_reta_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_reta_7[0][0],   0. );
				m_rphi_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_rphi_7[0][0],   0. );
				m_weta2_fferr.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_weta2_7[0][0],  0. );
				m_f1_fferr.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_f1_7[0][0],     0. );
				m_fside_fferr.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_fside_7[0][0],  0. );
				m_wtot_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_wtot_7[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_w1_7[0][0],     0. );
				m_deltae_fferr.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_deltae_7[0][0], 0. );
				m_eratio_fferr.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_c_eratio_7[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_rhad1_7[0][0],  0. );
				m_rhad_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_rhad_7[0][0],   0. );
				m_e277_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_e277_7[0][0],   0. );
				m_reta_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_reta_7[0][0],   0. );
				m_rphi_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_rphi_7[0][0],   0. );
				m_weta2_ff.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_weta2_7[0][0],  0. );
				m_f1_ff.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_f1_7[0][0],     0. );
				m_fside_ff.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_fside_7[0][0],  0. );
				m_wtot_ff.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_wtot_7[0][0],   0. );
				m_w1_ff.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_w1_7[0][0],     0. );
				m_deltae_ff.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_deltae_7[0][0], 0. );
				m_eratio_ff.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &ff_u_eratio_7[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_rhad1_7[0][0],  0. );
				m_rhad_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_rhad_7[0][0],   0. );
				m_e277_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_e277_7[0][0],   0. );
				m_reta_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_reta_7[0][0],   0. );
				m_rphi_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_rphi_7[0][0],   0. );
				m_weta2_fferr.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_weta2_7[0][0],  0. );
				m_f1_fferr.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_f1_7[0][0],     0. );
				m_fside_fferr.Initialize  ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_fside_7[0][0],  0. );
				m_wtot_fferr.Initialize   ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_wtot_7[0][0],   0. );
				m_w1_fferr.Initialize     ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_w1_7[0][0],     0. );
				m_deltae_fferr.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_deltae_7[0][0], 0. );
				m_eratio_fferr.Initialize ( 4, 5, pt_DVs_7, eta_DVs_7, &fferr_u_eratio_7[0][0], 0. );
				}

				break;

		case 8:  // FF from mc11a isolation+Tight (JF17+JF35+JF70)

				static double pt_DVs_8[7] = {30000,35000,40000,45000,50000,60000,85000}; //,10000000};// in MeV
				static double eta_DVs_8[5] = { 0.6, 1.37, 1.52, 1.81, 2.37}; // eta binning according to SMDP inclusive analysis

				//converted photons

				static double ff_c_rhad1_8[7][5]    = {  {-9.32093e-05, 0.000161838, 0.0 , 0.00014143, -3.96739e-05},
				{-0.000362903, -7.04346e-05, 0.0 , 0.000516465, 6.32174e-05},
				{0.000877957, -0.000146391, 0.0 , 0.000173696, -3.64526e-05},
				{-0.000245208, 8.38538e-05, 0.0 , -0.000208736, -4.30464e-05},
				{-6.0642e-05, 6.06211e-05, 0.0 , 0.000222942, -6.10225e-05},
				{0.000202635, 3.10088e-05, 0.0 , -0.000338991, -3.42522e-05},
				{0.000119796, -0.000172435, 0.0 , 0.000232037, 6.12431e-05},
				//{-0.000517156, -5.43425e-05, 0.0 , -0.000307121, 6.96293e-05},
				};

				static double fferr_c_rhad1_8[7][5]    = {  {0.000198343, 7.61781e-05, 0.0 , 0.000182423, 9.25511e-05},
				{0.000285342, 8.10949e-05, 0.0 , 0.00023859, 0.000107425},
				{0.000356876, 0.000118747, 0.0 , 0.000334112, 0.000132879},
				{0.000321222, 0.000149276, 0.0 , 0.000375606, 0.00014815},
				{0.000226665, 0.000106242, 0.0 , 0.00027511, 0.000127415},
				{0.000228043, 9.70429e-05, 0.0 , 0.000258258, 0.000121417},
				{0.000267134, 9.03958e-05, 0.0 , 0.000281005, 0.000128367},
				//{0.000266411, 6.35342e-05, 0.0 , 0.000190357, 8.6964e-05},
				};

				static double ff_c_rhad_8[7][5]    = {  {0.000192565, 0.0002659, 0.0 , 0.000732079, 0.000618808},
				{-5.77847e-05, -6.45874e-05, 0.0 , 0.00100776, -0.000222733},
				{0.00108725, -5.68203e-05, 0.0 , 0.000750745, -0.000474715},
				{-0.00111605, -0.000200144, 0.0 , -0.000319373, -0.000531527},
				{-5.86816e-05, 2.81236e-05, 0.0 , 0.000940919, -0.000595988},
				{0.000447036, 0.000167725, 0.0 , -0.000247133, -5.01534e-05},
				{1.74264e-05, -6.04504e-05, 0.0 , -0.000956918, -2.73404e-06},
				//{-0.00061884, -0.00012036, 0.0 , -0.000416955, -0.000140207},
				};

				static double fferr_c_rhad_8[7][5]    = {   {0.00030456, 9.90838e-05, 0.0 , 0.000519082, 0.000302963},
				{0.000383683, 0.000108852, 0.0 , 0.00062099, 0.000343299},
				{0.000485662, 0.000148569, 0.0 , 0.000716754, 0.000333157},
				{0.000650727, 0.000221593, 0.0 , 0.000663531, 0.000496073},
				{0.000343321, 0.000141903, 0.0 , 0.000578937, 0.000397938},
				{0.000340478, 0.000122005, 0.0 , 0.000577814, 0.000341678},
				{0.000390227, 0.000103547, 0.0 , 0.000626141, 0.000276062},
				//{0.000297457, 7.51552e-05, 0.0 , 0.00038818, 0.00021586},
				};

				/*
				static double ff_c_e277_0[7][5]    = {{81.1914, 388.422, 0.0 , 181.961, -109.445},
				{-95.0898, -168.545, 0.0 , 1185.17, -264.789},
				{-287.219, 629.426, 0.0 , -875.207, -568.484},
				{917.502, 2498.71, 0.0 , -2801.07, -1516.09},
				{-346.707, 1055.13, 0.0 , -2523.62, -1829.48},
				{-160.855, 1785.84, 0.0 , -506.812, -1501.34},
				{-2401.08, 1768.79, 0.0 , -5566.27, -10223.9},};

				static double fferr_c_e277_0[7][5]    = {{187.903, 224.875, 0.0 , 711.477, 763.533, },
				{312.838, 412.172, 0.0 , 1357.94, 1103.68, },
				{379.886, 572.68, 0.0 , 2025.91, 1660.41, },
				{537.662, 899.029, 0.0 , 2762.42, 2810.67, },
				{376.662, 462.842, 0.0 , 1767.99, 1586.05, },
				{552.718, 764.483, 0.0 , 2444.53, 2487.72, },
				{1145.48, 1469.2, 0.0 , 4994.09, 4438.99, },};
				*/

				static double ff_c_e277_8[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				//{0., 0., 0.0 , 0., 0.},
				};

				static double fferr_c_e277_8[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				//{0., 0., 0.0 , 0., 0.},
				};

				static double ff_c_reta_8[7][5]    = {  {-0.00174594, -0.00506252, 0.0 , -0.00534898, -0.00419009},
				{0.000176489, -0.00281143, 0.0 , -0.00593024, -0.00535446},
				{0.00120676, -0.00423986, 0.0 , -0.00667292, -0.00300413},
				{-0.0013954, -0.00247401, 0.0 , -0.00749594, -0.00451839},
				{-0.00341892, -0.00262809, 0.0 , -0.00396675, -0.00414813},
				{0.000730097, -0.00209773, 0.0 , -0.00752801, -0.00507015},
				{-0.00128049, -0.00450206, 0.0 , -0.00386089, -0.00420123},
				//{-0.000283778, -0.00251353, 0.0 , -0.00635737, -0.00326157},
				};

				static double fferr_c_reta_8[7][5]    = {    {0.000863182, 0.000561465, 0.0 , 0.000916419, 0.000735323},
				{0.00123433, 0.000700546, 0.0 , 0.000998972, 0.000924505},
				{0.00148373, 0.000919614, 0.0 , 0.0013007, 0.00134398},
				{0.00173033, 0.00118184, 0.0 , 0.00201518, 0.00164709},
				{0.00136954, 0.00112554, 0.0 , 0.00163484, 0.00132663},
				{0.00134024, 0.000904759, 0.0 , 0.00166905, 0.00133982},
				{0.0011263, 0.000901331, 0.0 , 0.00160399, 0.00171737},
				//{0.00105707, 0.000910439, 0.0 , 0.00157743, 0.00143465},
				};

				static double ff_c_rphi_8[7][5]    = { {-0.00296617, -0.00389105, 0.0 , -0.00275588, -0.00179309},
				{-0.00855672, -0.00256711, 0.0 , -0.00284266, -0.00049901},
				{-0.00963902, -0.00502872, 0.0 , 0.0106566, 0.00199229},
				{-0.00197297, -0.00464708, 0.0 , -0.00884503, -0.00377798},
				{0.00980502, -0.00282854, 0.0 , -0.00687504, -0.00325894},
				{0.00251478, 0.000964582, 0.0 , 0.000799775, -0.000584066},
				{0.00267839, -0.00455236, 0.0 , -0.00210744, -3.27826e-06},
				//{-0.00358182, -0.00393105, 0.0 , -0.00348896, 0.00152779},
				};

				static double fferr_c_rphi_8[7][5]    = {  {0.00257839, 0.00164991, 0.0 , 0.00206267, 0.00181253},
				{0.00379351, 0.00199971, 0.0 , 0.00266978, 0.002425},
				{0.00534387, 0.00298498, 0.0 , 0.00477835, 0.00321753},
				{0.0054185, 0.00388265, 0.0 , 0.00531942, 0.00326113},
				{0.0053778, 0.00316227, 0.0 , 0.00409003, 0.00307501},
				{0.00547011, 0.00300884, 0.0 , 0.00438835, 0.0028271},
				{0.00483971, 0.00288713, 0.0 , 0.00358126, 0.00260885},
				//{0.00479427, 0.00240622, 0.0 , 0.00297981, 0.00194584},
				};

				static double ff_c_weta2_8[7][5]    = { {6.28559e-05, 0.000118151, 0.0 , 0.000183978, 0.000168943},
				{4.89866e-05, 3.15979e-05, 0.0 , 0.000278621, 0.0002246},
				{0.000148795, 3.9977e-05, 0.0 , 0.000201352, 8.97329e-05},
				{-7.22455e-05, 0.000137947, 0.0 , 0.000350506, 0.000324475},
				{5.15133e-05, 0.00015664, 0.0 , 0.00036822, 0.000296444},
				{0.000121178, 2.23797e-05, 0.0 , 0.000302672, 0.000303183},
				{7.82823e-05, 3.15495e-05, 0.0 , 0.000189184, 0.000131294},
				//{3.46517e-05, 9.92231e-05, 0.0 , 0.000300951, 6.71493e-05},
				};

				static double fferr_c_weta2_8[7][5]    = {   {4.01615e-05, 3.25953e-05, 0.0 , 5.46835e-05, 4.51286e-05},
				{6.0318e-05, 3.98846e-05, 0.0 , 6.50572e-05, 6.11677e-05},
				{6.13209e-05, 5.68323e-05, 0.0 , 0.000103221, 8.07614e-05},
				{8.11471e-05, 6.64604e-05, 0.0 , 0.00010518, 0.000105631},
				{5.56683e-05, 6.30322e-05, 0.0 , 0.000102916, 8.97657e-05},
				{7.16153e-05, 5.61599e-05, 0.0 , 9.75226e-05, 9.47777e-05},
				{6.7659e-05, 5.44052e-05, 0.0 , 0.000100554, 0.000103531},
				//{7.92939e-05, 5.79932e-05, 0.0 , 9.16125e-05, 0.000103177},
				};

				static double ff_c_f1_8[7][5]    = {  {-0.00281984, 0.00286523, 0.0 , 0.00685847, 0.00413895},
				{-0.00152037, -0.00596578, 0.0 , 0.00845057, 0.00519112},
				{0.00958538, -0.0165123, 0.0 , -0.00541791, 0.000382036},
				{-0.00378114, -0.00461003, 0.0 , 0.0299157, 0.0129263},
				{0.00362158, -0.00661924, 0.0 , 0.00800681, 0.00414592},
				{0.00299954, 0.00500764, 0.0 , 0.0205384, 0.00664032},
				{0.0159827, -0.00656423, 0.0 , 0.00333232, 0.00932834},
				//{-0.0162314, 0.00207333, 0.0 , 0.0146126, -0.00491762},
				};

				static double fferr_c_f1_8[7][5]    = {  {0.00572099, 0.00352905, 0.0 , 0.00452652, 0.0051344},
				{0.00908073, 0.00510916, 0.0 , 0.00647882, 0.00630593},
				{0.00971547, 0.00747755, 0.0 , 0.00863206, 0.0095857},
				{0.0148138, 0.00881973, 0.0 , 0.0150738, 0.0115821},
				{0.00930347, 0.00758562, 0.0 , 0.00915407, 0.00889175},
				{0.0109779, 0.00690029, 0.0 , 0.0109809, 0.00895944},
				{0.00947963, 0.00784095, 0.0 , 0.0101384, 0.010611},
				//{0.0124122, 0.00834899, 0.0 , 0.00999947, 0.0108266},
				};

				static double ff_c_fside_8[7][5]    = {  {0.0074659, 0.0184686, 0.0 , 0.0265355, 0.0177566},
				{0.0112792, 0.0153741, 0.0 , 0.0306138, 0.0161721},
				{0.0166512, 0.0129861, 0.0 , 0.0209303, 0.0221021},
				{0.0158535, 0.0263374, 0.0 , 0.0438161, 0.0209411},
				{0.00677212, 0.0181378, 0.0 , 0.0277154, 0.0177166},
				{0.00856978, 0.00876269, 0.0 , 0.0096103, 0.0150135},
				{0.00602373, 0.0215867, 0.0 , 0.019137, 0.0217695},
				//{-0.00326434, 0.00952759, 0.0 , 0.0599301, 0.0205278},
				};

				static double fferr_c_fside_8[7][5]    = {  {0.0030279, 0.00395927, 0.0 , 0.006027, 0.0018924},
				{0.00393924, 0.00525438, 0.0 , 0.00812501, 0.00256312},
				{0.00604105, 0.00745743, 0.0 , 0.0110084, 0.0032325},
				{0.00725313, 0.00880483, 0.0 , 0.0147841, 0.00490112},
				{0.00526766, 0.00804897, 0.0 , 0.0112374, 0.00379227},
				{0.00497523, 0.00712736, 0.0 , 0.0142428, 0.00396805},
				{0.00550501, 0.00732657, 0.0 , 0.0137519, 0.00435997},
				//{0.00550132, 0.00721677, 0.0 , 0.012163, 0.00505383},
				};

				static double ff_c_wtot_8[7][5]    = {  {0.0533619, 0.0710099, 0.0 , 0.0931377, 0.0799216},
				{0.066295, 0.0673177, 0.0 , 0.123061, 0.0512601},
				{0.081218, 0.0815318, 0.0 , 0.0993152, 0.0974027},
				{0.0681121, 0.0912476, 0.0 , 0.149769, 0.0556867},
				{0.0163039, 0.0892625, 0.0 , 0.122854, 0.0593249},
				{0.0500399, 0.064575, 0.0 , 0.128951, 0.0373591},
				{0.0294838, 0.134617, 0.0 , 0.0626438, 0.0801469},
				//{0.0369489, 0.0532825, 0.0 , 0.189753, 0.111197},
				};

				static double fferr_c_wtot_8[7][5]    = {   {0.0187095, 0.0150002, 0.0 , 0.0243234, 0.0134135},
				{0.0194341, 0.0197863, 0.0 , 0.029087, 0.0183158},
				{0.0333201, 0.0270403, 0.0 , 0.0392256, 0.0229264},
				{0.0379187, 0.0325737, 0.0 , 0.0540851, 0.0355741},
				{0.0249056, 0.0291971, 0.0 , 0.0409743, 0.0272383},
				{0.0264118, 0.0254288, 0.0 , 0.0473582, 0.0284936},
				{0.0246796, 0.0293479, 0.0 , 0.0471229, 0.0327255},
				//{0.0249029, 0.0290349, 0.0 , 0.0403047, 0.0389202},
				};



				static double ff_c_w1_8[7][5]    = {  {0.00175035, 0.0139135, 0.0 , 0.00941819, 0.0106451},
				{0.0068993, 0.0100656, 0.0 , 0.00835848, 0.0151675},
				{0.00900656, 0.00561821, 0.0 , 0.0114837, 0.0106086},
				{-0.00278687, 0.0190987, 0.0 , 0.0117818, 0.0220888},
				{0.0015009, 0.0047822, 0.0 , 0.00991082, 0.0210287},
				{-0.00407195, 0.00384045, 0.0 , 0.0111932, 0.0104846},
				{0.00723112, 0.013244, 0.0 , 0.0107698, 0.0126218},
				//{-0.00409424, 0.00887883, 0.0 , 0.0180703, 0.0165147},
				};

				static double fferr_c_w1_8[7][5]    = {  {0.00256895, 0.00230087, 0.0 , 0.00299592, 0.0024022},
				{0.00373874, 0.00293472, 0.0 , 0.00402328, 0.00284811},
				{0.00537184, 0.00411365, 0.0 , 0.00521366, 0.00422246},
				{0.00579751, 0.00549484, 0.0 , 0.00949088, 0.0057081},
				{0.00477279, 0.0042163, 0.0 , 0.00555796, 0.00382123},
				{0.00493734, 0.00418789, 0.0 , 0.0075357, 0.00426345},
				{0.00563387, 0.00442346, 0.0 , 0.00651567, 0.00490873},
				//{0.00552077, 0.00471369, 0.0 , 0.00690241, 0.00586936},
				};

				static double ff_c_deltae_8[7][5]    = { {-0.94072, -1.87455, 0.0 , -2.48241, -3.69692},
				{0.489439, -0.568687, 0.0 , -3.03688, -1.72298},
				{-6.67478, -5.25285, 0.0 , 1.57771, 0.201414},
				{-2.03451, -1.23213, 0.0 , 9.37234, -1.843},
				{-3.77082, 0.198242, 0.0 , -5.3433, -7.62895},
				{0.131258, -3.11609, 0.0 , 3.93566, -2.47332},
				{3.84725, -6.72112, 0.0 , -3.4598, 4.23233},
				//{-5.20284, -3.75235, 0.0 , -2.53419, 5.53246},
				};

				static double fferr_c_deltae_8[7][5]    = {  {1.69032, 1.01471, 0.0 , 2.0871, 1.70349},
				{1.85957, 1.46329, 0.0 , 2.94362, 2.09308},
				{3.80143, 2.10833, 0.0 , 4.03315, 2.84186},
				{4.62744, 2.71627, 0.0 , 3.98944, 4.27825},
				{3.44153, 2.2815, 0.0 , 4.44961, 2.9779},
				{2.76188, 2.14432, 0.0 , 4.01877, 3.46544},
				{3.61021, 2.44994, 0.0 , 5.76637, 2.98736},
				//{4.7034, 2.5751, 0.0 , 5.13136, 4.52225},
				};

				static double ff_c_eratio_8[7][5]    = {  {-0.000363708, 0.00239515, 0.0 , 0.000841379, -7.92742e-06},
				{-0.000578761, -0.00250077, 0.0 , -0.00388128, 0.00139499},
				{0.00434172, -8.33869e-05, 0.0 , 0.000609219, -0.00042522},
				{0.00632149, -0.00524026, 0.0 , -0.00734067, 0.00321615},
				{-0.00178701, -0.00242108, 0.0 , -0.000697494, 0.000302553},
				{0.00117099, -0.00131738, 0.0 , -0.00431502, 0.000549555},
				{0.00160497, 0.00623637, 0.0 , 0.00449914, 0.000659168},
				//{0.00355464, 0.000390291, 0.0 , -0.00199521, -0.0017159},
				};

				static double fferr_c_eratio_8[7][5]    = { {0.00139521, 0.0018745, 0.0 , 0.00317679, 0.000657831},
				{0.00221779, 0.00234827, 0.0 , 0.00373208, 0.00102215},
				{0.0028903, 0.00326538, 0.0 , 0.00573176, 0.00109649},
				{0.00372812, 0.00357654, 0.0 , 0.00330503, 0.00175204},
				{0.00244461, 0.00267529, 0.0 , 0.00484545, 0.000965202},
				{0.00243815, 0.002531, 0.0 , 0.00352441, 0.00112414},
				{0.00249444, 0.0034042, 0.0 , 0.00424748, 0.00165717},
				//{0.00213285, 0.00308806, 0.0 , 0.00165996, 0.00116209},
				};

				//unconverted photons

				static double ff_u_rhad1_8[7][5]    = {  {3.81701e-05, 8.66415e-05, 0.0 , -0.000143263, 8.67172e-05},
				{-5.57928e-05, -9.53266e-06, 0.0 , -5.15974e-05, -8.52853e-05},
				{-5.44243e-05, -3.08394e-05, 0.0 , -6.41329e-05, 5.08339e-05},
				{-0.000437726, 5.69775e-05, 0.0 , 0.000853735, -9.23531e-05},
				{-0.000187615, 9.56659e-06, 0.0 , -0.000310649, 9.59365e-05},
				{-0.000275155, 1.64461e-05, 0.0 , -0.000199768, 6.70978e-05},
				{0.000122008, 4.03439e-05, 0.0 , 0.000107408, -9.29548e-05},
				//{-0.000290435, -7.06186e-05, 0.0 , -0.00020702, -4.82358e-06},
				};

				static double fferr_u_rhad1_8[7][5]    = {   {0.000106953, 6.48687e-05, 0.0 , 0.00023748, 7.788e-05},
				{0.000160289, 8.43093e-05, 0.0 , 0.000267346, 9.09514e-05},
				{0.000215776, 8.97654e-05, 0.0 , 0.000379026, 0.000132218},
				{0.000236153, 0.000121914, 0.0 , 0.00042216, 0.000135099},
				{0.000170475, 9.91875e-05, 0.0 , 0.00043401, 0.000106579},
				{0.000162878, 8.1312e-05, 0.0 , 0.000281329, 8.92589e-05},
				{0.000173233, 8.15747e-05, 0.0 , 0.000340683, 0.00010964},
				//{0.000169908, 6.85337e-05, 0.0 , 0.000220002, 7.30068e-05},
				};

				static double ff_u_rhad_8[7][5]    = {  {-8.78357e-05, -1.08135e-07, 0.0 , -0.000209302, 7.9665e-05},
				{0.000276983, -0.000104368, 0.0 , 6.6128e-05, -0.000141756},
				{-8.44644e-05, 1.15251e-05, 0.0 , 0.00038734, 0.000524504},
				{-0.000919775, -9.85168e-05, 0.0 , -6.02833e-05, -0.000534274},
				{-0.000441084, 0.000113341, 0.0 , -0.000533168, 0.000241085},
				{-0.000426805, -1.3177e-05, 0.0 , 0.000190553, -1.39594e-06},
				{-2.30328e-05, 1.44395e-05, 0.0 , 0.000128807, 0.000181602},
				//{-0.000475542, -8.85756e-05, 0.0 , -0.000628297, 0.00018152},
				};

				static double fferr_u_rhad_8[7][5]    = {   {0.000155061, 9.65088e-05, 0.0 , 0.000552618, 0.000254541},
				{0.000233945, 0.000115027, 0.0 , 0.000605822, 0.000297992},
				{0.000297943, 0.000133205, 0.0 , 0.000801234, 0.000378317},
				{0.000371403, 0.000189811, 0.0 , 0.00118362, 0.000438923},
				{0.000236816, 0.000122718, 0.0 , 0.00104097, 0.000295686},
				{0.000216798, 0.000105045, 0.0 , 0.000601843, 0.000202538},
				{0.000198721, 0.000112232, 0.0 , 0.000771239, 0.000252901},
				//{0.000204292, 8.2455e-05, 0.0 , 0.000380024, 0.000150902},
				};


				/*
				static double ff_u_e277_0[7][5]    = {{425.203, 1024.02, 0.0 , -23.375, 2288.88},
				{310.352, 631.141, 0.0 , 1871.47, 2008.77},
				{440.285, 1113.95, 0.0 , 1894.77, -1535.41},
				{198.871, -463.273, 0.0 , -7692.29, -1407.67},
				{79.7227, 533.355, 0.0 , 60.6719, -3502.28},
				{-153.34, 1583.26, 0.0 , 1582.92, 1331.8},
				{612.473, 2587.91, 0.0 , -6556.98, 2220.34},};


				static double fferr_u_e277_0[7][5]    = {{123.581, 241.947, 0.0 , 832.081, 812.67, },
				{196.094, 420.303, 0.0 , 1525.54, 1289.17, },
				{334.752, 641.21, 0.0 , 2398.77, 2083.63, },
				{497.225, 911.407, 0.0 , 3022.47, 3190.71, },
				{239.435, 478.502, 0.0 , 1898.26, 1813.12, },
				{394.463, 827.943, 0.0 , 2910.44, 2496.09, },
				{667.093, 1549.79, 0.0 , 5711.67, 3711.84, },};
				*/

				static double ff_u_e277_8[7][5]    = {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				//{0., 0., 0.0 , 0., 0.},
				};

				static double fferr_u_e277_8[7][5]= {{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				{0., 0., 0.0 , 0., 0.},
				//{0., 0., 0.0 , 0., 0.},
				};

				static double ff_u_reta_8[7][5]    = {    {-0.000860691, -0.00313699, 0.0 , -0.0048638, -0.00341755},
				{-0.00106621, -0.00312424, 0.0 , -0.00437862, -0.00559825},
				{-0.00158209, -0.00285131, 0.0 , -0.00527865, -0.00512981},
				{-0.000916839, -0.00167996, 0.0 , -0.0028432, -0.00506783},
				{-0.0015406, -0.0025804, 0.0 , -0.00359702, -0.00545323},
				{-0.000897229, -0.00321615, 0.0 , -0.0036431, -0.00367308},
				{-0.00127017, -0.00329328, 0.0 , -0.00339949, -0.00588655},
				//{-0.0162314, 0.00207333, 0.0 , 0.0146126, -0.00491762},
				};


				static double fferr_u_reta_8[7][5]    = {   {0.000418595, 0.000389587, 0.0 , 0.000719768, 0.000560286},
				{0.000517707, 0.000508649, 0.0 , 0.000933701, 0.00071952},
				{0.0006096, 0.00060167, 0.0 , 0.00117557, 0.00106691},
				{0.000962641, 0.000715573, 0.0 , 0.00214507, 0.00122224},
				{0.000643791, 0.000758125, 0.0 , 0.00144291, 0.00109577},
				{0.000635081, 0.000597642, 0.0 , 0.00132843, 0.000952826},
				{0.000592649, 0.000721529, 0.0 , 0.0014434, 0.0011493},
				//{0.0124122, 0.00834899, 0.0 , 0.00999947, 0.0108266},
				};


				static double ff_u_rphi_8[7][5]    = {  {0.000437021, -0.00061959, 0.0 , -0.00141448, -0.00267649},
				{-0.000255585, -0.000922382, 0.0 , -0.000519335, -0.00431287},
				{5.68032e-05, -0.000697672, 0.0 , -0.0019511, -0.00188756},
				{-7.15256e-07, 0.00179774, 0.0 , -0.0032987, -0.00185001},
				{-0.000291705, -0.000943899, 0.0 , 0.0015161, -0.00289327},
				{0.000374556, -0.000899673, 0.0 , -0.000913858, -0.00224257},
				{-1.90139e-05, -0.00206339, 0.0 , 0.000685573, -0.00368053},
				//{-0.00105101, -0.000321507, 0.0 , -0.000588119, -0.00377351},
				};


				static double fferr_u_rphi_8[7][5]    = {  {0.000362141, 0.00066758, 0.0 , 0.000829053, 0.000472956},
				{0.00044913, 0.000889888, 0.0 , 0.000981107, 0.000622041},
				{0.000602099, 0.00100235, 0.0 , 0.00145263, 0.000940275},
				{0.000849811, 0.00153268, 0.0 , 0.00220527, 0.00101313},
				{0.000639701, 0.000980144, 0.0 , 0.00127501, 0.000947896},
				{0.000548276, 0.00125704, 0.0 , 0.00140332, 0.000832839},
				{0.000574914, 0.00106935, 0.0 , 0.00180374, 0.00089042},
				//{0.000542923, 0.000809549, 0.0 , 0.00125682, 0.000944697},
				};


				static double ff_u_weta2_8[7][5]    = {  {9.82322e-05, 3.93214e-05, 0.0 , 0.000233524, 0.000174391},
				{5.85439e-05, 6.10137e-05, 0.0 , 0.000280721, 0.000276722},
				{9.63425e-05, 4.20762e-05, 0.0 , 0.000287114, 0.000186822},
				{4.91869e-05, -1.65207e-05, 0.0 , 0.000207094, 0.00015862},
				{5.67306e-05, 1.6151e-05, 0.0 , 0.000107696, 0.000242954},
				{8.01245e-05, -3.24063e-05, 0.0 , 0.000215735, 0.000131746},
				{8.43005e-05, 0.000100314, 0.0 , 0.000157487, 0.000280596},
				//{0.000112484, -2.37878e-05, 0.0 , 0.000327242, 0.00022702},
				};

				static double fferr_u_weta2_8[7][5]    = {  {2.16526e-05, 2.31179e-05, 0.0 , 4.08861e-05, 3.75834e-05},
				{2.805e-05, 3.27537e-05, 0.0 , 5.1542e-05, 4.83951e-05},
				{4.04339e-05, 3.72108e-05, 0.0 , 6.68584e-05, 7.57765e-05},
				{4.93567e-05, 5.07349e-05, 0.0 , 0.000112221, 7.6029e-05},
				{3.95058e-05, 4.9221e-05, 0.0 , 9.56122e-05, 7.18991e-05},
				{3.57691e-05, 4.24884e-05, 0.0 , 6.58083e-05, 6.38805e-05},
				{3.62276e-05, 4.62192e-05, 0.0 , 0.000100607, 7.84656e-05},
				//{3.60874e-05, 4.06738e-05, 0.0 , 7.61372e-05, 7.69796e-05},
				};

				static double ff_u_f1_8[7][5]    = {  {-0.00746891, -0.0076763, 0.0 , 0.00900793, -0.0159909},
				{-0.0111177, -0.0116703, 0.0 , 0.00156862, -0.00654939},
				{-0.000974283, -0.0089943, 0.0 , 0.013971, 0.00726958},
				{-0.0121146, -0.0235568, 0.0 , -0.0205632, -0.00971678},
				{-0.00369543, -0.00587529, 0.0 , 0.0047015, 0.0111472},
				{-0.0089827, -0.00932607, 0.0 , -0.00298898, -0.000630885},
				{-0.00257094, 0.00837806, 0.0 , -0.0196036, 0.00639251},
				//{0.0214031, -0.0201964, 0.0 , 0.021988, -0.000765711},
				};


				static double fferr_u_f1_8[7][5]    = {   {0.00431452, 0.00378043, 0.0 , 0.00748942, 0.00496779},
				{0.00577634, 0.00523374, 0.0 , 0.00854074, 0.00645748},
				{0.00799918, 0.00661335, 0.0 , 0.0125705, 0.00939016},
				{0.011056, 0.00842505, 0.0 , 0.019972, 0.0115467},
				{0.00879631, 0.00748588, 0.0 , 0.0157067, 0.00886371},
				{0.00764274, 0.00708468, 0.0 , 0.0138143, 0.00870589},
				{0.00878837, 0.00801403, 0.0 , 0.0165964, 0.00970377},
				//{0.0082802, 0.00761924, 0.0 , 0.0159325, 0.00875637},
				};


				static double ff_u_fside_8[7][5]    = {  {0.00578383, 0.00575131, 0.0 , 0.0203306, 0.00906374},
				{0.00261052, 0.00696355, 0.0 , 0.0282071, 0.00991687},
				{0.00752956, 0.00628056, 0.0 , 0.0314565, 0.0191406},
				{0.00251532, 0.00435369, 0.0 , 0.0297637, 0.0133161},
				{0.0066029, 0.00441201, 0.0 , 0.010425, 0.0138536},
				{0.00517796, 0.00586832, 0.0 , 0.014172, 0.0185399},
				{0.00565886, 0.0129573, 0.0 , 0.016993, 0.0141177},
				//{0.0110214, 0.00412135, 0.0 , 0.036686, 0.0198156},
				};


				static double fferr_u_fside_8[7][5]    = {  {0.00192845, 0.00292395, 0.0 , 0.00711809, 0.00154956},
				{0.00243709, 0.00448122, 0.0 , 0.00766903, 0.00189403},
				{0.00335004, 0.00495766, 0.0 , 0.0105892, 0.00313653},
				{0.00500925, 0.00592342, 0.0 , 0.0128256, 0.00322528},
				{0.00357229, 0.00650222, 0.0 , 0.0141386, 0.00312022},
				{0.0032078, 0.00562062, 0.0 , 0.0100155, 0.00263713},
				{0.0033401, 0.00621391, 0.0 , 0.0121213, 0.00355075},
				//{0.00311799, 0.00477671, 0.0 , 0.00903049, 0.00317969},
				};


				static double ff_u_wtot_8[7][5]    = {   {0.0450209, 0.0313513, 0.0 , 0.0591123, 0.0122899},
				{0.0356288, 0.0785668, 0.0 , 0.0964282, 0.0148982},
				{0.0504434, 0.0334263, 0.0 , 0.12334, 0.0857573},
				{0.0108324, 0.0508251, 0.0 , 0.14601, 0.0800072},
				{0.0819473, 0.0293558, 0.0 , 0.0378907, 0.0553242},
				{0.0394402, 0.0532277, 0.0 , 0.0583813, 0.0973401},
				{0.0471946, 0.0763783, 0.0 , 0.106753, 0.0596751},
				//{0.0222324, 0.0458956, 0.0 , 0.179767, 0.087387},
				};



				static double fferr_u_wtot_8[7][5]    = {  {0.0137685, 0.013011, 0.0 , 0.0224485, 0.0129796},
				{0.0177014, 0.0181966, 0.0 , 0.0341786, 0.0162485},
				{0.0259447, 0.0227926, 0.0 , 0.0373283, 0.0259318},
				{0.0282137, 0.0318483, 0.0 , 0.0504256, 0.02685},
				{0.0222572, 0.0273816, 0.0 , 0.0397378, 0.0254847},
				{0.0206469, 0.0244747, 0.0 , 0.0340887, 0.0213684},
				{0.0250544, 0.0265116, 0.0 , 0.0415764, 0.0282217},
				//{0.0239444, 0.0242636, 0.0 , 0.0357496, 0.0278648},
				};

				static double ff_u_w1_8[7][5]    = {   {-0.00269473, 0.00679129, 0.0 , 0.015981, 0.00655031},
				{-0.00457364, 0.00328255, 0.0 , 0.0128436, 0.00900769},
				{-0.00227183, 0.00594014, 0.0 , 0.0196443, 0.0139514},
				{0.00203526, 0.001239, 0.0 , 0.00714284, 0.0117894},
				{-0.000840664, 0.00610775, 0.0 , 0.00624537, 0.00976682},
				{0.0003196, 0.00674367, 0.0 , 0.00332129, 0.0145364},
				{0.00763339, 0.0143858, 0.0 , -0.00642014, 0.00848389},
				//{0.00802726, 0.00354832, 0.0 , 0.0294297, 0.0146068},
				};


				static double fferr_u_w1_8[7][5]    = { {0.00168459, 0.00193403, 0.0 , 0.00372375, 0.00177301},
				{0.00217042, 0.00282633, 0.0 , 0.00406168, 0.00256652},
				{0.00307575, 0.00362856, 0.0 , 0.00662878, 0.00373392},
				{0.00455568, 0.0046292, 0.0 , 0.00919321, 0.00439158},
				{0.00346028, 0.00405679, 0.0 , 0.00619714, 0.00371577},
				{0.00317357, 0.00378081, 0.0 , 0.00673261, 0.00343621},
				{0.00370805, 0.00435837, 0.0 , 0.00811027, 0.00417923},
				//{0.00347949, 0.00365395, 0.0 , 0.0069031, 0.00391963},

				};


				static double ff_u_deltae_8[7][5]    = {   {-2.29626, -0.978502, 0.0 , -0.14753, -4.17375},
				{1.01907, 0.431034, 0.0 , 2.55742, -1.21954},
				{0.396402, -0.505629, 0.0 , -7.07673, -6.91971},
				{3.06596, -3.56278, 0.0 , -3.10348, -3.47549},
				{-1.48664, -2.90592, 0.0 , 6.73623, -8.14938},
				{0.125238, 1.25914, 0.0 , 2.29453, -8.77831},
				{0.100706, -1.86522, 0.0 , 8.10991, 0.505833},
				//{-1.6152, -0.503487, 0.0 , 1.48792, -5.27595},
				};


				static double fferr_u_deltae_8[7][5]    = {  {0.846431, 0.764118, 0.0 , 1.75921, 1.63887},
				{1.12202, 1.00606, 0.0 , 2.10756, 2.03864},
				{1.5576, 1.35695, 0.0 , 3.27717, 4.18826},
				{2.10795, 1.86834, 0.0 , 5.35258, 3.64623},
				{1.60606, 1.63752, 0.0 , 3.89664, 4.05137},
				{1.56601, 1.4071, 0.0 , 3.51031, 3.25202},
				{1.57061, 1.674, 0.0 , 3.05829, 2.92341},
				//{1.89928, 1.64435, 0.0 , 3.96413, 4.07509},
				};

				static double ff_u_eratio_8[7][5]    = {  {0.000935853, -0.00187796, 0.0 , 7.75456e-05, 0.000616848},
				{-0.00277656, -0.00216776, 0.0 , 0.00158036, 0.000884235},
				{0.00130057, 0.0011518, 0.0 , -0.000285029, -0.000391603},
				{-0.00474834, -0.00183982, 0.0 , -0.00638622, 0.000184834},
				{-0.000693202, -0.00191492, 0.0 , -0.000137269, 0.00131798},
				{0.00054884, 7.47442e-05, 0.0 , 0.00235564, 0.00237507},
				{0.00136673, 0.00598556, 0.0 , -0.00307345, -0.00120068},
				//{0.00457287, 0.000847101, 0.0 , -0.00304741, -0.00222778},
				};

				static double fferr_u_eratio_8[7][5]    = {   {0.00179583, 0.00124708, 0.0 , 0.00151862, 0.00140989},
				{0.0021719, 0.00170516, 0.0 , 0.0021203, 0.00183741},
				{0.00333563, 0.00224893, 0.0 , 0.00229389, 0.00156948},
				{0.00356126, 0.00264565, 0.0 , 0.00285627, 0.00245973},
				{0.00879631, 0.00748588, 0.0 , 0.0157067, 0.00886371},
				{0.00764274, 0.00708468, 0.0 , 0.0138143, 0.00870589},
				{0.00274657, 0.00249495, 0.0 , 0.00173521, 0.00145935},
				//{0.00295831, 0.00229055, 0.0 , 0.00249757, 0.0020535},
				};


				if(isConv){
				m_rhad1_ff.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_rhad1_8[0][0],  0. );
				m_rhad_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_rhad_8[0][0],   0. );
				m_e277_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_e277_8[0][0],   0. );
				m_reta_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_reta_8[0][0],   0. );
				m_rphi_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_rphi_8[0][0],   0. );
				m_weta2_ff.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_weta2_8[0][0],  0. );
				m_f1_ff.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_f1_8[0][0],     0. );
				m_fside_ff.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_fside_8[0][0],  0. );
				m_wtot_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_wtot_8[0][0] ,  0. );
				m_w1_ff.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_w1_8[0][0],     0. );
				m_deltae_ff.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_deltae_8[0][0], 0. );
				m_eratio_ff.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_c_eratio_8[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_rhad1_8[0][0],  0. );
				m_rhad_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_rhad_8[0][0],   0. );
				m_e277_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_e277_8[0][0],   0. );
				m_reta_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_reta_8[0][0],   0. );
				m_rphi_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_rphi_8[0][0],   0. );
				m_weta2_fferr.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_weta2_8[0][0],  0. );
				m_f1_fferr.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_f1_8[0][0],     0. );
				m_fside_fferr.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_fside_8[0][0],  0. );
				m_wtot_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_wtot_8[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_w1_8[0][0],     0. );
				m_deltae_fferr.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_deltae_8[0][0], 0. );
				m_eratio_fferr.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_c_eratio_8[0][0], 0. );
				}
				else{
				m_rhad1_ff.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_rhad1_8[0][0],  0. );
				m_rhad_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_rhad_8[0][0],   0. );
				m_e277_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_e277_8[0][0],   0. );
				m_reta_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_reta_8[0][0],   0. );
				m_rphi_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_rphi_8[0][0],   0. );
				m_weta2_ff.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_weta2_8[0][0],  0. );
				m_f1_ff.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_f1_8[0][0],     0. );
				m_fside_ff.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_fside_8[0][0],  0. );
				m_wtot_ff.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_wtot_8[0][0],   0. );
				m_w1_ff.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_w1_8[0][0],     0. );
				m_deltae_ff.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_deltae_8[0][0], 0. );
				m_eratio_ff.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &ff_u_eratio_8[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_rhad1_8[0][0],  0. );
				m_rhad_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_rhad_8[0][0],   0. );
				m_e277_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_e277_8[0][0],   0. );
				m_reta_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_reta_8[0][0],   0. );
				m_rphi_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_rphi_8[0][0],   0. );
				m_weta2_fferr.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_weta2_8[0][0],  0. );
				m_f1_fferr.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_f1_8[0][0],     0. );
				m_fside_fferr.Initialize  ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_fside_8[0][0],  0. );
				m_wtot_fferr.Initialize   ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_wtot_8[0][0],   0. );
				m_w1_fferr.Initialize     ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_w1_8[0][0],     0. );
				m_deltae_fferr.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_deltae_8[0][0], 0. );
				m_eratio_fferr.Initialize ( 7, 5, pt_DVs_8, eta_DVs_8, &fferr_u_eratio_8[0][0], 0. );
				}

				break;

		case 10: // RobustTight preselection ('old' FFs set, rel 15. Full 2010 data + MC09.)

				static double pt_15_DVs_0[9] = {25000,30000,35000,40000,45000,50000,60000,100000,2000000};// in MeV

				//converted photons

				static double ff_c_rhad1_10[9][5]    = {{0.00044983, 0.000374889, 0.0, 0.000988124, 3.59691e-05},
				{0.000171636, -0.00013933, 0.0, 0.000703906, -0.000110949},
				{0.000197652, 0.000359985, 0.0, 0.000147445, -0.000690257},
				{-0.000123192, -0.00056915, 0.0, -0.000510919, 9.08909e-06},
				{7.85173e-05, -0.000100393, 0.0, -2.28543e-05, -0.00035755},
				{-6.14488e-05, 0.000370915, 0.0, 0.000210908, -0.000296418},
				{-9.91821e-05, -6.29224e-05, 0.0, -0.000212423, -0.000171527},
				{0.000152859, -0.000117083, 0.0, 0.000839204, -0.000150324}};

				static double fferr_c_rhad1_10[9][5]    = {{0.000151874, 0.000227564, 0.0, 0.000271911, 0.000197046},
				{0.000157231, 0.000237559, 0.0, 0.000323408, 0.000205343},
				{0.000209818, 0.000304573, 0.0, 0.000437415, 0.000285948},
				{0.000253507, 0.000383548, 0.0, 0.000554177, 0.000260309},
				{0.00010019, 0.000155148, 0.0, 0.00021971, 0.000139043},
				{0.000154558, 0.000178064, 0.0, 0.00029254, 0.000192827},
				{0.000114755, 0.000173794, 0.0, 0.000231881, 0.000178464},
				{0.000195448, 0.000247857, 0.0, 0.000340455, 0.000316474}};

				static double ff_c_rhad_10[9][5]    = {{0, 0.000368831, 0.0, 0, 0},
				{0, 0.000124766, 0.0, 0, 0},
				{0, 5.43364e-05, 0.0, 0, 0},
				{0, 0.0001207, 0.0, 0, 0},
				{0, 9.28671e-05, 0.0, 0, 0},
				{0, -1.70754e-05, 0.0, 0, 0},
				{0, 0.000130327, 0.0, 0, 0},
				{0, -1.34641e-05, 0.0, 0, 0}};

				static double fferr_c_rhad_10[9][5]    = {{0, 0.000108495, 0.0, 0, 0},
				{0, 0.000112565, 0.0, 0, 0},
				{0, 0.000147023, 0.0, 0, 0},
				{0, 0.000152876, 0.0, 0, 0},
				{0, 7.38271e-05, 0.0, 0, 0},
				{0, 9.96613e-05, 0.0, 0, 0},
				{0, 7.34096e-05, 0.0, 0, 0},
				{0, 0.000110511, 0.0, 0, 0}};

				static double ff_c_e277_10[9][5]    = {{581.595, 1538.58, 0.0, 3112.89, 139.1},
				{287.353, 757.272, 0.0, 2078.05, -446.942},
				{655.764, 932.234, 0.0, 2455.73, -1650.15},
				{-135.858, 317.576, 0.0, 2604.73, -3286.37},
				{2282.66, 3376.39, 0.0, 4860.11, 4742.34},
				{254.334, 1551.81, 0.0, 3624.66, -3097.88},
				{-1542.88, -3298.46, 0.0, -5187.3, -9912.89},
				{-559.995, 737.022, 0.0, 6435.83, 2198.44}};

				static double fferr_c_e277_10[9][5]    = {{117.892, 122.518, 0.0, 327.886, 456.557},
				{148.912, 177.604, 0.0, 518.689, 673.169},
				{248.982, 300.812, 0.0, 959.787, 1154.17},
				{391.177, 403.06, 0.0, 1354.97, 1492.12},
				{216.432, 257.522, 0.0, 810.842, 978.012},
				{346.899, 450.458, 0.0, 1526.72, 1709.22},
				{610.019, 779.716, 0.0, 2397.23, 2226.78},
				{4111.98, 3525.63, 0.0, 6848.38, 5653.43}};

				static double ff_c_reta_10[9][5]    = {{-0.00375056, -0.00641442, 0.0, -0.00899176, -0.00950652},
				{-0.00395265, -0.00741328, 0.0, -0.00955819, -0.00931247},
				{-0.0028144, -0.00609688, 0.0, -0.0102989, -0.00999462},
				{-0.0041638, -0.00694531, 0.0, -0.00850718, -0.010266},
				{-0.00285408, -0.00625652, 0.0, -0.00973527, -0.010551},
				{-0.00476999, -0.00581884, 0.0, -0.00991751, -0.00964236},
				{-0.00355127, -0.00651561, 0.0, -0.0113364, -0.0117152},
				{-0.00307999, -0.00682749, 0.0, -0.0128453, -0.0107069}};

				static double fferr_c_reta_10[9][5]    = {{0.000494384, 0.000296909, 0.0, 0.000433703, 0.000387418},
				{0.000501667, 0.000343956, 0.0, 0.000550029, 0.000485662},
				{0.00072799, 0.000482778, 0.0, 0.000840717, 0.000743364},
				{0.000900742, 0.000601899, 0.0, 0.0010548, 0.000817703},
				{0.000382843, 0.000296151, 0.0, 0.000504708, 0.000404799},
				{0.000578079, 0.000427826, 0.0, 0.000784026, 0.000649849},
				{0.00046615, 0.000406923, 0.0, 0.000677689, 0.000527679},
				{0.000897916, 0.000743208, 0.0, 0.00111997, 0.00098389}};

				static double ff_c_rphi_10[9][5]    = {{-0.0118992, -0.00573189, 0.0, -0.00136633, -0.00226423},
				{-0.00331666, -0.00197804, 0.0, 0.00139549, 0.00176447},
				{0.00462502, -0.000562083, 0.0, 0.00643708, -0.00356676},
				{-0.000814097, 0.00316924, 0.0, -0.00185454, -0.00502851},
				{0.0131573, 0.00146244, 0.0, 0.0044529, -0.00457888},
				{-0.00485493, 0.000510623, 0.0, 0.0015883, -0.00525752},
				{0.00514003, -0.000362342, 0.0, -0.00542892, -0.0101218},
				{-0.00371827, -0.00348125, 0.0, -0.00601331, -0.00412026}};

				static double fferr_c_rphi_10[9][5]    = {{0.00353725, 0.00179423, 0.0, 0.00218345, 0.00189737},
				{0.00356757, 0.00217685, 0.0, 0.00284805, 0.00228888},
				{0.005663, 0.00308453, 0.0, 0.0040631, 0.00268121},
				{0.0066031, 0.00372222, 0.0, 0.00433589, 0.00277536},
				{0.00306564, 0.00162864, 0.0, 0.00204851, 0.0013355},
				{0.00373833, 0.0023454, 0.0, 0.00310219, 0.00188584},
				{0.00294087, 0.00185711, 0.0, 0.00184052, 0.00123757},
				{0.00253586, 0.0030803, 0.0, 0.00275173, 0.00303871}};

				static double ff_c_weta2_10[9][5]    = {{0.000290316, 0.000325051, 0.0, 0.000477654, 0.000429538},
				{0.000308231, 0.000287413, 0.0, 0.000500576, 0.000430692},
				{0.000151538, 0.000274226, 0.0, 0.000501961, 0.000403972},
				{0.000296662, 0.000278126, 0.0, 0.00045604, 0.000433468},
				{0.000229736, 0.000297157, 0.0, 0.000457582, 0.000439182},
				{0.000314972, 0.000279029, 0.0, 0.00047095, 0.00040923},
				{0.000221483, 0.000289874, 0.0, 0.000512138, 0.000494435},
				{0.000229561, 0.000346588, 0.0, 0.000531164, 0.000374726}};

				static double fferr_c_weta2_10[9][5]    = {{2.78919e-05, 1.75167e-05, 0.0, 2.66561e-05, 2.57718e-05},
				{2.8683e-05, 2.01323e-05, 0.0, 3.09429e-05, 3.02741e-05},
				{4.34789e-05, 2.88917e-05, 0.0, 4.39367e-05, 4.76671e-05},
				{4.87846e-05, 3.70411e-05, 0.0, 6.05678e-05, 5.08829e-05},
				{2.17241e-05, 1.57883e-05, 0.0, 3.03392e-05, 2.77332e-05},
				{3.09994e-05, 2.38595e-05, 0.0, 4.4867e-05, 4.17747e-05},
				{2.37412e-05, 2.32597e-05, 0.0, 3.79871e-05, 3.54364e-05},
				{4.4533e-05, 3.06029e-05, 0.0, 7.56211e-05, 7.12778e-05}};

				static double ff_c_f1_10[9][5]    = {{-0.00737538, -0.0104887, 0.0, -0.000452774, -0.00034028},
				{-0.00363897, -0.00323162, 0.0, 0.00413935, 0.000741355},
				{-0.0132444, -0.0049626, 0.0, -0.00124982, 0.0091716},
				{0.00114555, 0.000500745, 0.0, 0.00327278, 0.00797778},
				{-0.00782549, -0.00446097, 0.0, 0.00408247, 0.00724195},
				{0.00203902, -0.00834213, 0.0, 0.00235492, 0.00783562},
				{-0.00993465, -0.00337771, 0.0, 0.00935782, 0.0139757},
				{-0.00587049, -0.00283598, 0.0, -0.0120989, 0.00549825}};

				static double fferr_c_f1_10[9][5]    = {{0.00357393, 0.00192238, 0.0, 0.00237167, 0.00278898},
				{0.00392862, 0.00233389, 0.0, 0.00291366, 0.00318782},
				{0.00544404, 0.00327346, 0.0, 0.00430792, 0.00524581},
				{0.00732549, 0.00380183, 0.0, 0.00548551, 0.00575804},
				{0.00326223, 0.00197196, 0.0, 0.00252945, 0.00276865},
				{0.00475769, 0.00297779, 0.0, 0.00373696, 0.00412824},
				{0.00366689, 0.00269102, 0.0, 0.00380477, 0.00346629},
				{0.00747688, 0.00493448, 0.0, 0.00635102, 0.00699699}};

				static double ff_c_fside_10[9][5]    = {{0.00636809, 0.00826564, 0.0, 0.0129869, 0.0153214},
				{0.00853854, 0.0075573, 0.0, 0.0184865, 0.0131001},
				{0.000110364, 0.00657767, 0.0, 0.0116713, 0.015354},
				{0.00747624, 0.00319303, 0.0, 0.00193714, 0.0146105},
				{0.00227157, 0.00152351, 0.0, 0.0178366, 0.0149605},
				{0.00286281, -0.00558568, 0.0, 0.0128524, 0.0173702},
				{0.00504228, 0.00228557, 0.0, 0.0219075, 0.01995},
				{-0.00090078, 0.00372166, 0.0, 0.020024, 0.0193477}};

				static double fferr_c_fside_10[9][5]    = {{0.0017492, 0.00198969, 0.0, 0.00285557, 0.00110525},
				{0.00191538, 0.00239384, 0.0, 0.00373682, 0.00132266},
				{0.00285763, 0.00346923, 0.0, 0.00537908, 0.00210025},
				{0.00397771, 0.00400459, 0.0, 0.00696233, 0.0021519},
				{0.00159498, 0.00198267, 0.0, 0.0035149, 0.00115308},
				{0.00265678, 0.00301488, 0.0, 0.00531054, 0.00184565},
				{0.0020278, 0.00286791, 0.0, 0.00483863, 0.00156976},
				{0.00468203, 0.00527164, 0.0, 0.00987086, 0.00326075}};

				static double ff_c_wtot_10[9][5]    = {{0.0744491, 0.0630831, 0.0, 0.10279, 0.08221},
				{0.0773812, 0.0631218, 0.0, 0.124271, 0.0741739},
				{0.0504097, 0.0633693, 0.0, 0.0969518, 0.0803977},
				{0.110323, 0.0607061, 0.0, 0.0629108, 0.073877},
				{0.059586, 0.0572783, 0.0, 0.114401, 0.0669001},
				{0.0557027, 0.060703, 0.0, 0.097136, 0.0913699},
				{0.0532947, 0.0649216, 0.0, 0.127623, 0.0801581},
				{0.0319794, 0.0565642, 0.0, 0.139165, 0.103004}};

				static double fferr_c_wtot_10[9][5]    = {{0.0108386, 0.00778435, 0.0, 0.0115653, 0.00756723},
				{0.0111394, 0.00886841, 0.0, 0.0141893, 0.00885278},
				{0.0149928, 0.0130029, 0.0, 0.0207803, 0.013483},
				{0.0207817, 0.014693, 0.0, 0.0257189, 0.0144635},
				{0.00839537, 0.00752788, 0.0, 0.0128812, 0.0076248},
				{0.0145673, 0.0111052, 0.0, 0.0187857, 0.0119295},
				{0.0117349, 0.010598, 0.0, 0.017404, 0.010917},
				{0.0249193, 0.0211133, 0.0, 0.0305457, 0.0218415}};

				static double ff_c_w1_10[9][5]    = {{0.00142823, 0.00515642, 0.0, 0.00324209, 0.0102336},
				{-0.000582763, 0.00633092, 0.0, 0.00696048, 0.00982976},
				{-0.000287507, 0.0031081, 0.0, 0.0073846, 0.0123661},
				{-0.00190387, 0.0035604, 0.0, 0.00423726, 0.0115105},
				{-0.00140313, 0.00282855, 0.0, 0.0101443, 0.00954253},
				{-0.00301612, 0.0027678, 0.0, 0.00589651, 0.0122536},
				{-0.00432546, 0.00304709, 0.0, 0.0102094, 0.0163138},
				{-0.00330196, 0.005018, 0.0, 0.0126259, 0.00886614}};

				static double fferr_c_w1_10[9][5]    = {{0.00160043, 0.00115982, 0.0, 0.00147933, 0.00115064},
				{0.00169919, 0.0013998, 0.0, 0.00186381, 0.00144288},
				{0.00243897, 0.00194937, 0.0, 0.00261557, 0.0022478},
				{0.00327777, 0.00247109, 0.0, 0.00316046, 0.00253103},
				{0.00146335, 0.0011725, 0.0, 0.0017437, 0.00125028},
				{0.00226044, 0.0017552, 0.0, 0.00268771, 0.00185385},
				{0.00183364, 0.00169867, 0.0, 0.00244224, 0.00168971},
				{0.00535777, 0.00301146, 0.0, 0.00428025, 0.00343811}};

				static double ff_c_deltae_10[9][5]    = {{-0.885269, -1.33638, 0.0, -2.55426, -1.01619},
				{-1.70973, -2.02061, 0.0, -2.31166, -2.34915},
				{-0.210393, -2.34151, 0.0, 3.88318, -0.476221},
				{-0.339511, -1.71749, 0.0, -3.81719, -3.75759},
				{-3.01171, -3.4159, 0.0, -0.299299, -1.68544},
				{-2.21275, -2.86392, 0.0, 1.37476, 0.379818},
				{-5.25414, -1.47319, 0.0, 0.500747, -2.50241},
				{-2.85606, 0.360783, 0.0, 3.05495, -0.991872}};

				static double fferr_c_deltae_10[9][5]    = {{0.846531, 0.512684, 0.0, 0.984649, 0.768638},
				{1.09535, 0.633723, 0.0, 1.22739, 1.02602},
				{1.61789, 0.985989, 0.0, 1.8335, 1.34156},
				{2.05921, 1.1859, 0.0, 2.37851, 1.79541},
				{1.04338, 0.644611, 0.0, 1.1475, 0.862931},
				{1.90901, 0.93778, 0.0, 1.73467, 1.32293},
				{1.76794, 0.880769, 0.0, 1.59779, 1.22455},
				{3.55373, 1.93223, 0.0, 3.44097, 2.6282}};

				static double ff_c_eratio_10[9][5]    = {{-0.00188757, -0.000861378, 0.0, -0.00513146, -0.00137925},
				{-0.000435326, -0.00174258, 0.0, -0.00257427, -0.000750724},
				{-0.000221033, -0.000261609, 0.0, -0.00157995, 0.0005194},
				{-0.00199197, -0.00219344, 0.0, -0.00327278, -0.000517099},
				{0.00102818, 0.00241189, 0.0, -0.0022985, -7.28231e-05},
				{0.00026313, -0.00219584, 0.0, 0.000448392, -0.000726936},
				{-0.000762964, -0.00120781, 0.0, -0.000962286, -7.53121e-05},
				{-0.00161399, -0.00255113, 0.0, -0.00518663, -0.000443012}};

				static double fferr_c_eratio_10[9][5]    = {{0.000813541, 0.00100975, 0.0, 0.00175449, 0.000401895},
				{0.000899148, 0.00113949, 0.0, 0.00192325, 0.000435611},
				{0.00128994, 0.00164548, 0.0, 0.00302877, 0.000691738},
				{0.00172738, 0.00182682, 0.0, 0.00370246, 0.000686414},
				{0.000762738, 0.00093061, 0.0, 0.00134198, 0.000369927},
				{0.00121675, 0.00110654, 0.0, 0.00216557, 0.000457003},
				{0.000961685, 0.00108109, 0.0, 0.0017915, 0.000427756},
				{0.00198697, 0.00152314, 0.0, 0.00132251, 0.000718675}};

				// unconverted photons

				static double ff_u_rhad1_10[9][5]    = {{0.000221664, 0.000409302, 0.0, -0.000484562, 0.000182384},
				{0.000204754, 3.58935e-05, 0.0, -8.18803e-05, -0.000393932},
				{0.000220571, 9.16727e-05, 0.0, -8.63209e-05, 0.000634481},
				{-6.86049e-05, -3.83663e-06, 0.0, -0.000278459, -7.45178e-05},
				{9.71934e-05, 8.31003e-05, 0.0, -7.51894e-05, 0.000123853},
				{0.000278383, 0.000229985, 0.0, 0.000301706, -0.000260913},
				{-6.73331e-05, -3.10417e-05, 0.0, -0.000145261, -0.000241234},
				{-3.51463e-05, 0.000125999, 0.0, -0.00058169, -0.000821368}};

				static double fferr_u_rhad1_10[9][5]    = {{0.00010073, 0.000150745, 0.0, 0.000343399, 0.000201391},
				{0.00011331, 0.00016675, 0.0, 0.000372095, 0.000223141},
				{0.000147196, 0.000235345, 0.0, 0.000503113, 0.000252393},
				{0.000193034, 0.000294076, 0.0, 0.000582017, 0.0003455},
				{7.51792e-05, 0.000108982, 0.0, 0.000288361, 0.000152133},
				{0.000106083, 0.000135482, 0.0, 0.000456163, 0.000227932},
				{7.93897e-05, 9.19103e-05, 0.0, 0.000304317, 0.000182394},
				{0.000128863, 0.000156095, 0.0, 0.000569263, 0.000370991}};

				static double ff_u_rhad_10[9][5]    = {{0, 0.00028942, 0.0, 0, 0},
				{0, 4.23114e-05, 0.0, 0, 0},
				{0, 6.95607e-05, 0.0, 0, 0},
				{0, 0.000392164, 0.0, 0, 0},
				{0, 0.000164258, 0.0, 0, 0},
				{0, 7.32408e-05, 0.0, 0, 0},
				{0, 1.57734e-05, 0.0, 0, 0},
				{0, -0.000139068, 0.0, 0, 0}};

				static double fferr_u_rhad_10[9][5]    = {{0, 0.000117211, 0.0, 0, 0},
				{0, 0.00012865, 0.0, 0, 0},
				{0, 0.000177227, 0.0, 0, 0},
				{0, 0.000190912, 0.0, 0, 0},
				{0, 7.75193e-05, 0.0, 0, 0},
				{0, 9.85637e-05, 0.0, 0, 0},
				{0, 6.87995e-05, 0.0, 0, 0},
				{0, 0.00013475, 0.0, 0, 0}};

				static double ff_u_e277_10[9][5]    = {{762.755, 1596.93, 0.0, 3799.84, 4636.27},
				{669.136, 1198.98, 0.0, 4867.2, 2801.29},
				{662.473, 1638.09, 0.0, 4343.16, 735.58},
				{634.027, 1203.27, 0.0, 2808.77, -2237.28},
				{2663.87, 4035.73, 0.0, 8687.13, 6914.79},
				{301.262, 836.416, 0.0, 2453.56, -1495.05},
				{-2166.46, -4658.52, 0.0, -11255.5, -6399.24},
				{-399.438, 6840.56, 0.0, 10003.1, 10341.7}};

				static double fferr_u_e277_10[9][5]    = {{78.9194, 142.135, 0.0, 429.84, 500.269},
				{117.931, 204.058, 0.0, 640.408, 739.361},
				{190.647, 348.309, 0.0, 1236.51, 1223.69},
				{295.347, 528.398, 0.0, 1673, 1958.56},
				{160.3, 313.441, 0.0, 1023.53, 1128.03},
				{276.528, 570.323, 0.0, 2225.05, 1984.26},
				{458.169, 773.709, 0.0, 2828.9, 2171.12},
				{2703.25, 2643.84, 0.0, 6535.07, 7341.68}};

				static double ff_u_reta_10[9][5]    = {{-0.00289522, -0.00524842, 0.0, -0.00811065, -0.00894986},
				{-0.0031434, -0.00573233, 0.0, -0.00857526, -0.009144},
				{-0.00291997, -0.00562338, 0.0, -0.0102302, -0.0086403},
				{-0.00221895, -0.00600915, 0.0, -0.00831691, -0.00944159},
				{-0.00348187, -0.00567926, 0.0, -0.00974061, -0.00980926},
				{-0.00410023, -0.00617647, 0.0, -0.00969117, -0.0103159},
				{-0.00397593, -0.00637453, 0.0, -0.0109551, -0.0106696},
				{-0.00393387, -0.0058405, 0.0, -0.00821717, -0.0105393}};

				static double fferr_u_reta_10[9][5]    = {{0.000247488, 0.000230494, 0.0, 0.000406049, 0.000332656},
				{0.000299349, 0.00027013, 0.0, 0.000510519, 0.000455009},
				{0.000411873, 0.00039908, 0.0, 0.000824593, 0.000666838},
				{0.000531624, 0.000486872, 0.0, 0.000988111, 0.000734787},
				{0.000222555, 0.000226807, 0.0, 0.00043893, 0.000367525},
				{0.000323919, 0.00032237, 0.0, 0.000799794, 0.000532883},
				{0.000256498, 0.000271014, 0.0, 0.000585825, 0.000455485},
				{0.000400775, 0.000486257, 0.0, 0.00122943, 0.000930588}};

				static double ff_u_rphi_10[9][5]    = {{-0.00392947, -0.0059638, 0.0, -0.00686177, -0.0102112},
				{-0.00352975, -0.00532554, 0.0, -0.00396079, -0.00886429},
				{-0.00438835, -0.00401789, 0.0, -0.00448223, -0.00897804},
				{-0.00412999, -0.00362948, 0.0, -0.00389611, -0.00864977},
				{-0.00456346, -0.00482438, 0.0, -0.00386004, -0.00895679},
				{-0.0049583, -0.00476731, 0.0, -0.00605337, -0.00973707},
				{-0.00552442, -0.005746, 0.0, -0.00582836, -0.00971776},
				{-0.00621187, -0.00430373, 0.0, -0.00367043, -0.00910051}};

				static double fferr_u_rphi_10[9][5]    = {{0.000219942, 0.000562503, 0.0, 0.000477677, 0.000277233},
				{0.000252487, 0.000624235, 0.0, 0.000607265, 0.000387988},
				{0.000356329, 0.00111109, 0.0, 0.000949712, 0.000531859},
				{0.000481998, 0.00146799, 0.0, 0.00106529, 0.000669651},
				{0.000202847, 0.000560945, 0.0, 0.000586739, 0.000312811},
				{0.000305688, 0.000885259, 0.0, 0.000864325, 0.000476289},
				{0.000227875, 0.00061758, 0.0, 0.000754359, 0.000369743},
				{0.000401911, 0.0012084, 0.0, 0.00139925, 0.000867313}};

				static double ff_u_weta2_10[9][5]    = {{0.000200219, 0.000247971, 0.0, 0.000467457, 0.000418936},
				{0.000202596, 0.000247226, 0.0, 0.000386479, 0.00042388},
				{0.000207626, 0.000278482, 0.0, 0.00053024, 0.000351177},
				{0.000215695, 0.000229162, 0.0, 0.000444695, 0.000426624},
				{0.000232449, 0.000249892, 0.0, 0.000477568, 0.000384206},
				{0.000251691, 0.000278607, 0.0, 0.000503742, 0.00042933},
				{0.000250283, 0.000297842, 0.0, 0.0005846, 0.000450079},
				{0.000236128, 0.000292942, 0.0, 0.000455576, 0.000477735}};

				static double fferr_u_weta2_10[9][5]    = {{1.35693e-05, 1.32985e-05, 0.0, 2.24645e-05, 2.26556e-05},
				{1.68652e-05, 1.61109e-05, 0.0, 2.97055e-05, 3.03346e-05},
				{2.33158e-05, 2.38967e-05, 0.0, 4.39999e-05, 4.54598e-05},
				{2.89172e-05, 2.85942e-05, 0.0, 4.46498e-05, 5.25531e-05},
				{1.24309e-05, 1.34575e-05, 0.0, 2.68485e-05, 2.44985e-05},
				{1.69425e-05, 1.88719e-05, 0.0, 3.84196e-05, 3.55592e-05},
				{1.3575e-05, 1.53132e-05, 0.0, 3.35151e-05, 3.15758e-05},
				{2.18718e-05, 2.83019e-05, 0.0, 6.12979e-05, 6.92066e-05}};

				static double ff_u_f1_10[9][5]    = {{-0.0195248, -0.0191321, 0.0, -0.0131535, -0.0234798},
				{-0.0177012, -0.0152438, 0.0, -0.0220335, -0.0165834},
				{-0.0168014, -0.0138564, 0.0, -0.00757452, -0.0101612},
				{-0.0131052, -0.0122411, 0.0, -0.00962806, -0.00660317},
				{-0.010584, -0.0134327, 0.0, -0.0124587, -0.00665372},
				{-0.00420372, -0.00945586, 0.0, -0.000459673, 0.00458705},
				{-0.00126831, -0.0044734, 0.0, 0.0150097, 0.00431334},
				{-0.000432202, -0.0126246, 0.0, -0.0113936, -0.00131817}};

				static double fferr_u_f1_10[9][5]    = {{0.00265522, 0.00220703, 0.0, 0.00369747, 0.00303161},
				{0.00329658, 0.00263489, 0.0, 0.00447303, 0.00406099},
				{0.00454412, 0.00388725, 0.0, 0.00763282, 0.00537303},
				{0.00616887, 0.00485664, 0.0, 0.00825721, 0.00665017},
				{0.00270107, 0.00236509, 0.0, 0.00428501, 0.00317277},
				{0.00389986, 0.00330143, 0.0, 0.00719371, 0.00452199},
				{0.00324536, 0.00264335, 0.0, 0.00613841, 0.00378159},
				{0.00497142, 0.00478696, 0.0, 0.0113267, 0.00721678}};

				static double ff_u_fside_10[9][5]    = {{-6.70365e-06, -0.000132742, 0.0, 0.0121276, 0.00805895},
				{-0.00189604, 0.00113108, 0.0, 0.00151392, 0.00817627},
				{0.00168001, 0.00380908, 0.0, 0.0105824, 0.00882582},
				{-0.00153318, -0.00749194, 0.0, -0.0020346, 0.00711193},
				{-4.82009e-05, -0.00307443, 0.0, 0.00181889, 0.0135787},
				{0.00200874, 0.000485449, 0.0, 0.00471748, 0.0143195},
				{0.00284358, 0.00363055, 0.0, 0.0245483, 0.0146208},
				{0.00575543, 0.00309613, 0.0, 0.00597897, 0.0159581}};

				static double fferr_u_fside_10[9][5]    = {{0.00117008, 0.00172476, 0.0, 0.00376176, 0.000963288},
				{0.00146614, 0.00210812, 0.0, 0.00509867, 0.00118486},
				{0.00197928, 0.00334898, 0.0, 0.00684265, 0.00170744},
				{0.00279637, 0.00438956, 0.0, 0.00951018, 0.00200447},
				{0.00119255, 0.00192244, 0.0, 0.00494056, 0.00102614},
				{0.00177551, 0.00282681, 0.0, 0.00843619, 0.00152696},
				{0.00136772, 0.00214229, 0.0, 0.00535676, 0.00132506},
				{0.00223576, 0.00379893, 0.0, 0.00985947, 0.00226537}};

				static double ff_u_wtot_10[9][5]    = {{0.0628359, 0.0474913, 0.0, 0.0967981, 0.0433951},
				{0.0411479, 0.0638471, 0.0, 0.0674956, 0.033709},
				{0.0588533, 0.0660414, 0.0, 0.0618302, 0.0606156},
				{0.0199321, 0.0452107, 0.0, 0.0379818, 0.0396601},
				{0.0597159, 0.0463507, 0.0, 0.0784795, 0.0800883},
				{0.0693552, 0.0591206, 0.0, 0.0800468, 0.0742335},
				{0.0673295, 0.0565086, 0.0, 0.117261, 0.0857018},
				{0.075121, 0.063799, 0.0, 0.0770014, 0.0797875}};

				static double fferr_u_wtot_10[9][5]    = {{0.00900708, 0.00826338, 0.0, 0.0128428, 0.00756071},
				{0.00996832, 0.00966953, 0.0, 0.0160185, 0.00962955},
				{0.0132543, 0.014446, 0.0, 0.0260806, 0.0125837},
				{0.0196005, 0.0167655, 0.0, 0.0279377, 0.0150808},
				{0.00799864, 0.00793612, 0.0, 0.0143762, 0.00737375},
				{0.0103642, 0.01258, 0.0, 0.0234822, 0.012543},
				{0.00893766, 0.010148, 0.0, 0.0170229, 0.00994171},
				{0.0141851, 0.0175657, 0.0, 0.0297982, 0.0205724}};

				static double ff_u_w1_10[9][5]    = {{-0.00551963, 0.00110477, 0.0, 0.00370408, 0.00372623},
				{-0.00546477, 0.0014494, 0.0, 0.00104595, 0.00339408},
				{-0.00330767, 0.00295147, 0.0, 0.0063591, 0.00848907},
				{-0.00685436, -0.00461202, 0.0, 0.00575826, 0.00361444},
				{-0.00533735, -0.000241271, 0.0, 0.00316841, 0.00917795},
				{-0.00380381, 0.0011734, 0.0, 0.0100844, 0.0112447},
				{-0.00552205, 0.0043915, 0.0, 0.013399, 0.0108132},
				{-0.00161538, 0.00328613, 0.0, 0.0104515, 0.0140004}};

				static double fferr_u_w1_10[9][5]    = {{0.00105122, 0.00114085, 0.0, 0.0018487, 0.00112307},
				{0.00124428, 0.0014062, 0.0, 0.00238928, 0.00147758},
				{0.00180636, 0.00217186, 0.0, 0.00384286, 0.00212348},
				{0.00244245, 0.00274801, 0.0, 0.00354938, 0.00236835},
				{0.00106245, 0.00127086, 0.0, 0.00217341, 0.00123062},
				{0.00155923, 0.00190699, 0.0, 0.00345641, 0.00202679},
				{0.0013471, 0.00149032, 0.0, 0.00301485, 0.00171598},
				{0.00242142, 0.00285165, 0.0, 0.00515597, 0.00343024}};

				static double ff_u_deltae_10[9][5]    = {{-1.06472, -0.972981, 0.0, -2.34686, -0.148548},
				{-2.09624, -1.27408, 0.0, -0.755538, -3.12387},
				{-1.5424, -1.47863, 0.0, -2.97718, -1.0522},
				{-1.85503, -0.247138, 0.0, -4.87862, -3.19376},
				{-0.998796, -1.84519, 0.0, -1.67703, -1.52459},
				{1.16457, -2.93221, 0.0, -1.89015, -2.88485},
				{-0.587346, -2.76468, 0.0, -2.69293, -0.370243},
				{-0.0803814, -0.405104, 0.0, 3.2802, 0.46913}};

				static double fferr_u_deltae_10[9][5]    = {{0.491564, 0.41733, 0.0, 0.9192, 0.879133},
				{0.623009, 0.530909, 0.0, 1.14918, 1.14722},
				{0.880446, 0.835356, 0.0, 1.85289, 1.62219},
				{1.31935, 0.983898, 0.0, 2.2443, 1.97161},
				{0.522964, 0.51437, 0.0, 1.1224, 0.958588},
				{0.761511, 0.797717, 0.0, 1.92036, 1.58441},
				{0.687109, 0.646293, 0.0, 1.4405, 1.36215},
				{1.2681, 1.19312, 0.0, 2.58851, 2.30494}};

				static double ff_u_eratio_10[9][5]    = {{-0.00274959, -0.00228591, 0.0, -0.0025117, -0.00210926},
				{-0.00114929, -0.00363009, 0.0, -0.00206533, -0.00125866},
				{-0.00146114, -0.00256013, 0.0, 0.00148607, -0.00015131},
				{0.000903884, -0.00204116, 0.0, -0.00162407, 0.000176799},
				{-0.00103343, -0.0010394, 0.0, -0.00166915, -0.00176775},
				{-0.00214433, -0.000404158, 0.0, -0.0010484, -0.00105583},
				{0.0016853, -0.00223916, 0.0, -0.00172646, -0.00067458},
				{-0.00180764, -0.00237291, 0.0, -1.07877e-05, 0.00211235}};

				static double fferr_u_eratio_10[9][5]    = {{0.00125597, 0.000792481, 0.0, 0.000892919, 0.000901844},
				{0.00156781, 0.000893928, 0.0, 0.00105779, 0.00106749},
				{0.0020094, 0.00138623, 0.0, 0.00181597, 0.00156079},
				{0.00283041, 0.00166986, 0.0, 0.00194284, 0.00139107},
				{0.001082, 0.000745474, 0.0, 0.000838506, 0.000638176},
				{0.00138995, 0.00109716, 0.0, 0.00154509, 0.000885651},
				{0.00140632, 0.000794703, 0.0, 0.00103893, 0.000800506},
				{0.0016341, 0.00136828, 0.0, 0.00193592, 0.00200486}};


				if (isConv) {

				m_rhad1_ff.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_rhad1_10[0][0],  0. );
				m_rhad_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_rhad_10[0][0],   0. );
				m_e277_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_e277_10[0][0],   0. );
				m_reta_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_reta_10[0][0],   0. );
				m_rphi_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_rphi_10[0][0],   0. );
				m_weta2_ff.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_weta2_10[0][0],  0. );
				m_f1_ff.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_f1_10[0][0],     0. );
				m_fside_ff.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_fside_10[0][0],  0. );
				m_wtot_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_wtot_10[0][0] ,  0. );
				m_w1_ff.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_w1_10[0][0],     0. );
				m_deltae_ff.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_deltae_10[0][0], 0. );
				m_eratio_ff.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_c_eratio_10[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_rhad1_10[0][0],  0. );
				m_rhad_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_rhad_10[0][0],   0. );
				m_e277_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_e277_10[0][0],   0. );
				m_reta_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_reta_10[0][0],   0. );
				m_rphi_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_rphi_10[0][0],   0. );
				m_weta2_fferr.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_weta2_10[0][0],  0. );
				m_f1_fferr.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_f1_10[0][0],     0. );
				m_fside_fferr.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_fside_10[0][0],  0. );
				m_wtot_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_wtot_10[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_w1_10[0][0],     0. );
				m_deltae_fferr.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_deltae_10[0][0], 0. );
				m_eratio_fferr.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_c_eratio_10[0][0], 0. );
				} else {

				m_rhad1_ff.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_rhad1_10[0][0],  0. );
				m_rhad_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_rhad_10[0][0],   0. );
				m_e277_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_e277_10[0][0],   0. );
				m_reta_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_reta_10[0][0],   0. );
				m_rphi_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_rphi_10[0][0],   0. );
				m_weta2_ff.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_weta2_10[0][0],  0. );
				m_f1_ff.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_f1_10[0][0],     0. );
				m_fside_ff.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_fside_10[0][0],  0. );
				m_wtot_ff.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_wtot_10[0][0],   0. );
				m_w1_ff.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_w1_10[0][0],     0. );
				m_deltae_ff.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_deltae_10[0][0], 0. );
				m_eratio_ff.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &ff_u_eratio_10[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_rhad1_10[0][0],  0. );
				m_rhad_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_rhad_10[0][0],   0. );
				m_e277_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_e277_10[0][0],   0. );
				m_reta_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_reta_10[0][0],   0. );
				m_rphi_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_rphi_10[0][0],   0. );
				m_weta2_fferr.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_weta2_10[0][0],  0. );
				m_f1_fferr.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_f1_10[0][0],     0. );
				m_fside_fferr.Initialize  ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_fside_10[0][0],  0. );
				m_wtot_fferr.Initialize   ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_wtot_10[0][0],   0. );
				m_w1_fferr.Initialize     ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_w1_10[0][0],     0. );
				m_deltae_fferr.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_deltae_10[0][0], 0. );
				m_eratio_fferr.Initialize ( 9, 5, pt_15_DVs_0, eta_DVs_0, &fferr_u_eratio_10[0][0], 0. );

				}

				break;

		case 11: // tune 11

				static double pt_DVs_chi[6] = {25000, 40000, 50000, 60000, 80000, 10000000};
				static double eta_DVs_chi[8] = {0.6, 0.8, 1.15, 1.37, 1.53, 1.81, 2.01, 2.37};

				//converted photons

				static double ff_u_weta2_chi[6][8] = {   {5.48663e-05, 8.53507e-05, 2.46983e-05, 6.17117e-05, -0, 0.000171347, 0.0001563, 0.000160625},
				{5.27269e-05, 0.00010107, 2.60881e-05, 5.88366e-05, -0, 0.000186502, 0.000164132, 0.000158551},
				{6.90063e-05, 0.000122284, 4.05504e-05, 7.47492e-05, -0, 0.0002283, 0.000205829, 0.000189111},
				{6.168e-05, 0.000112458, 3.65157e-05, 6.25761e-05, -0, 0.000223408, 0.000220366, 0.000197615},
				{6.27556e-05, 0.000117682, 3.00755e-05, 5.92573e-05, -0, 0.000225137, 0.000232032, 0.000207816},
				{6.29562e-05, 0.000125592, 3.06116e-05, 5.81325e-05, -0, 0.000243866, 0.000240021, 0.000219253}};

				static double ff_c_weta2_chi[6][8] = {   {2.86656e-05, 5.7796e-05, 2.04563e-05, 3.49299e-05, -0, 0.000137885, 0.000208762, 0.0001726},
				{3.70966e-05, 7.78874e-05, 3.42049e-05, 4.55514e-05, -0, 0.000186035, 0.000237961, 0.000183264},
				{4.19792e-05, 8.04914e-05, 2.0814e-05, 5.18906e-05, -0, 0.000191726, 0.000230104, 0.000189029},
				{4.87868e-05, 8.67757e-05, 2.28631e-05, 5.38531e-05, -0, 0.000214146, 0.000237121, 0.000186177},
				{4.86984e-05, 9.0693e-05, 2.62325e-05, 4.45104e-05, -0, 0.000225108, 0.00024141, 0.000196519},
				{5.1202e-05, 9.43039e-05, 1.9036e-05, 4.35676e-05, -0, 0.000234098, 0.000241181, 0.00018522}};

				static double ff_u_f1_chi[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_c_f1_chi[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_u_e277_chi[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_c_e277_chi[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_u_reta_chi[6][8] = {   {0.000126459, -0.000169748, -0.00108312, -0.000654228, -0, -0.00314863, -0.00309009, -0.0023192},
				{-0.000401312, -0.000800843, -0.002423, -0.00224569, -0, -0.00409757, -0.00362157, -0.00248169},
				{-0.00102759, -0.00144912, -0.00381694, -0.00435218, -0, -0.00518948, -0.0048656, -0.00416047},
				{-0.00102531, -0.00151421, -0.00396296, -0.00457174, -0, -0.00516941, -0.00518319, -0.00452373},
				{-0.00103906, -0.00160892, -0.00408625, -0.00467228, -0, -0.00514618, -0.00551058, -0.00477283},
				{-0.00103504, -0.00153663, -0.00416773, -0.00473262, -0, -0.00515568, -0.00549709, -0.0049502}};

				static double ff_c_reta_chi[6][8] = {   {0.00205944, 0.00182239, 0.00155361, 0.00160503, -0, -0.00165725, -0.00279465, -0.00244386},
				{0.000700246, 0.000537135, -0.000474739, -0.000365091, -0, -0.00398589, -0.00412122, -0.00298295},
				{-9.90698e-05, -0.000313928, -0.0014638, -0.00224338, -0, -0.00464659, -0.00471303, -0.0035693},
				{-0.000298361, -0.00060051, -0.0022005, -0.00315461, -0, -0.00520119, -0.005099, -0.00387365},
				{-0.000596896, -0.000904027, -0.00292221, -0.00353672, -0, -0.00550631, -0.00535331, -0.00421675},
				{-0.000774671, -0.00105821, -0.00331821, -0.0039456, -0, -0.00533827, -0.00545667, -0.00392389}};

				static double ff_u_rphi_chi[6][8] = {   {-0.000221325, -0.00109402, -0.00120021, -0.00294739, -0, -0.00151785, -0.00167013, -0.00384614},
				{-0.000223956, -0.00142035, -0.00135705, -0.0031806, -0, -0.00190772, -0.00208142, -0.00397045},
				{-0.000390856, -0.00173526, -0.00149682, -0.00332793, -0, -0.00192795, -0.00258541, -0.00344065},
				{-0.000259392, -0.00164682, -0.00148709, -0.00342904, -0, -0.00166316, -0.00271525, -0.00343203},
				{-0.000255198, -0.00157865, -0.00142077, -0.00326332, -0, -0.00144952, -0.00285243, -0.00341683},
				{-0.000118643, -0.00153837, -0.00129299, -0.00327251, -0, -0.00143973, -0.00276862, -0.00338347}};

				static double ff_c_rphi_chi[6][8] = {   {0.00158941, 0.00193255, -0.000795093, -0.0043675, -0, -0.00711415, -0.00850427, -0.0109922},
				{0.00302133, 0.00316147, -0.00361073, -0.00790891, -0, -0.0102159, -0.0102029, -0.00864414},
				{0.0027161, 0.00255007, -0.00131474, -0.00486458, -0, -0.00466757, -0.00568835, -0.0056848},
				{0.0012485, -0.000572599, -0.00255688, -0.00632253, -0, -0.00492612, -0.00530409, -0.00519417},
				{-0.000122343, -0.00206035, -0.00242878, -0.00495963, -0, -0.00354733, -0.00403603, -0.00406112},
				{-7.77528e-06, -0.00107144, -0.000915768, -0.00304986, -0, -0.00201465, -0.00294019, -0.00312341}};

				static double ff_u_wtot_chi[6][8] = {   {0.0307193, 0.0455631, 0.0438113, 0.0596414, -0, 0.0811642, 0.0470252, 0.0378371},
				{0.0315176, 0.0443923, 0.0525923, 0.0760194, -0, 0.103084, 0.0587421, 0.0375561},
				{0.0351361, 0.0535668, 0.0607647, 0.0814972, -0, 0.109332, 0.0708345, 0.0454048},
				{0.0333989, 0.0557644, 0.0629816, 0.0809934, -0, 0.11085, 0.0767279, 0.0475155},
				{0.0366381, 0.0570763, 0.065347, 0.0813153, -0, 0.108506, 0.0791303, 0.0517272},
				{0.0333015, 0.0533175, 0.069107, 0.0754424, -0, 0.102799, 0.0852986, 0.0546665}};

				static double ff_c_wtot_chi[6][8] = {   {0.0145484, 0.0220815, 0.0117987, 0.025306, -0, 0.0653703, 0.0584752, 0.0404114},
				{0.0277713, 0.0346886, 0.0389511, 0.0437943, -0, 0.106757, 0.0724879, 0.0419853},
				{0.0304704, 0.0430045, 0.0384747, 0.055031, -0, 0.107625, 0.0802776, 0.0439579},
				{0.0325808, 0.0465111, 0.0452333, 0.0662736, -0, 0.122227, 0.0862311, 0.0458097},
				{0.0325048, 0.045373, 0.0496256, 0.0704272, -0, 0.126291, 0.0927184, 0.0487912},
				{0.0349607, 0.0495998, 0.0546795, 0.0687358, -0, 0.121035, 0.0928401, 0.0476726}};

				static double ff_u_eratio_chi[6][8] = {   {0.000125016, -0.000264668, -0.000825357, -0.00153686, -0, -0.000959334, 0.000149637, 6.21465e-05},
				{0.000499233, 4.20693e-05, -0.000276016, -0.000689782, -0, -0.000549693, 0.000371326, 0.000299223},
				{0.000595679, 6.64973e-05, -0.000528219, -0.00140672, -0, -0.000802191, 0.000248834, 0.000226186},
				{0.000712645, 0.000123044, -0.000382762, -0.00097849, -0, -0.000728821, 0.000267855, 0.000185005},
				{0.000479713, 0.000148205, -0.000438582, -0.00106244, -0, -0.000614097, 0.00029911, 0.000177902},
				{0.000551647, 0.000155343, -0.00018806, -0.000872943, -0, -0.000604461, 0.000161784, 9.54257e-05}};

				static double ff_c_eratio_chi[6][8] = {   {0.000606264, 0.00065135, 0.000229841, -3.41098e-06, -0, -0.000618314, 0.000410372, 0.000173684},
				{0.000510385, 0.000213577, -0.000444951, -0.00131582, -0, -0.00162176, 0.000256488, -0.000116744},
				{0.000392928, -0.000149892, -0.00020146, -0.000736868, -0, -0.00122613, 0.000214077, 3.38401e-05},
				{0.000402624, -0.000159389, -4.96859e-05, -0.00124929, -0, -0.00115061, 0.000170385, -5.62268e-05},
				{0.000569653, 9.33785e-06, -8.60316e-05, -0.00124442, -0, -0.000858654, 0.000492989, -4.18813e-05},
				{0.00042721, 0.000355159, -0.000395247, -0.00113782, -0, -0.000886359, 0.000476104, 5.84334e-05}};

				static double ff_u_fside_chi[6][8] = {   {0.00741065, 0.0114383, 0.0142606, 0.0171416, -0, 0.0246262, 0.0196454, 0.0143781},
				{0.00715948, 0.0123034, 0.016601, 0.0202827, -0, 0.0303315, 0.0208866, 0.0137423},
				{0.00770861, 0.0136635, 0.0182054, 0.0210498, -0, 0.0313991, 0.0232502, 0.0138241},
				{0.0069037, 0.0136695, 0.0171882, 0.0207555, -0, 0.0318065, 0.0241857, 0.0141795},
				{0.0073056, 0.0130364, 0.0182618, 0.0203146, -0, 0.0315328, 0.0247129, 0.0144861},
				{0.00695225, 0.0125433, 0.0175069, 0.0189919, -0, 0.0297738, 0.0252149, 0.0149218}};

				static double ff_c_fside_chi[6][8] = {   {0.00495073, 0.00804074, 0.00933185, 0.0122864, -0, 0.0247356, 0.0260521, 0.0162084},
				{0.00643008, 0.0108981, 0.0153243, 0.0166994, -0, 0.0355573, 0.0287714, 0.0159069},
				{0.00602564, 0.0114728, 0.0138558, 0.0182469, -0, 0.0342641, 0.0275838, 0.0151701},
				{0.00757887, 0.0138774, 0.0164298, 0.022092, -0, 0.0388579, 0.0293159, 0.0151207},
				{0.00757007, 0.0138078, 0.0177314, 0.0224257, -0, 0.0386786, 0.0297391, 0.0151447},
				{0.0075206, 0.0132107, 0.0173204, 0.0202616, -0, 0.0359142, 0.0279948, 0.014209}};

				static double ff_u_rhad_chi[6][8] = {   {-2.56081e-05, 2.76666e-05, 0.000135726, 0.00014262, -0, -0.000347511, -0.000486476, -0.000533672},
				{-1.19545e-05, 3.23226e-05, 0.000131799, 8.088e-05, -0, -0.000379336, -0.000379567, -0.000393518},
				{-2.63301e-05, 2.38239e-05, 9.85535e-05, 9.29055e-05, -0, -0.000147045, -0.000143696, -9.7162e-05},
				{-3.34829e-05, 1.71931e-05, 9.38408e-05, 9.20743e-05, -0, -4.52082e-05, -0.000107544, -4.5452e-05},
				{-3.67732e-05, 1.89451e-05, 7.42323e-05, 5.90888e-05, -0, -9.75342e-06, -3.06476e-05, -2.79433e-05},
				{-8.33695e-05, -1.65788e-08, 4.61242e-05, 4.04295e-05, -0, -0.000130916, -8.16748e-05, -3.69845e-05}};

				static double ff_c_rhad_chi[6][8] = {   {-5.85604e-05, -5.05551e-05, 4.54699e-06, 2.57931e-06, -0, -0.000886303, -0.00080947, -0.000687465},
				{-5.46194e-06, -1.46137e-05, 6.21372e-05, 4.75443e-05, -0, -0.000703603, -0.000601513, -0.000537668},
				{-3.62756e-06, -1.08268e-05, 4.93076e-05, 5.22842e-05, -0, -0.000426552, -0.000351644, -0.000340047},
				{-3.80872e-05, 3.29898e-06, 6.56757e-05, 5.71955e-05, -0, -0.000322353, -0.000357948, -0.000272956},
				{-3.65012e-05, 1.54341e-05, 5.48753e-05, 5.70963e-05, -0, -0.000198871, -0.000179069, -0.000202995},
				{-4.43582e-05, -9.09688e-06, 4.73234e-05, 3.74693e-05, -0, -0.000141475, -0.000152368, -0.000125105}};

				static double ff_u_deltae_chi[6][8] = {   {-0.220222, -0.312447, -0.327626, -0.222567, -0, 0.0454026, -1.11844, -0.89478},
				{-0.171201, -0.222741, -0.374427, -0.180934, -0, 0.203755, -0.731102, -1.12594},
				{-0.221362, -0.511863, -0.397485, -0.543167, -0, 0.350422, -1.11194, -1.16173},
				{-0.227709, -0.514123, -0.482616, -0.433729, -0, 0.558593, -0.693731, -1.13963},
				{-0.208436, -0.526315, -0.584649, -0.731035, -0, 0.394409, -1.00163, -1.12206},
				{-0.314841, -0.530003, -0.670339, -0.773871, -0, 0.710933, -0.642552, -1.02013}};

				static double ff_c_deltae_chi[6][8] = {   {-0.370366, -0.25583, -0.263795, -0.499318, -0, -0.327578, -0.670259, -1.24869},
				{-0.390001, -0.285907, -0.437444, -0.554812, -0, -0.0150989, -0.585952, -1.07615},
				{-0.0872478, -0.313626, -0.594611, -0.677005, -0, 0.113509, -0.85138, -1.26073},
				{-0.218003, -0.40307, -0.451697, -0.814524, -0, 0.126448, -0.836522, -1.49393},
				{-0.27215, -0.272991, -0.698296, -0.744939, -0, 0.428301, -0.830167, -1.2331},
				{-0.250156, -0.570488, -0.655198, -0.852327, -0, 0.786784, -0.633486, -0.776476}};

				static double ff_u_w1_chi[6][8] = {   {0.000847918, 0.00492173, 0.0075167, 0.00779173, -0, 0.0114013, 0.0106582, 0.0122555},
				{0.000119695, 0.00565484, 0.00822038, 0.00851442, -0, 0.0140551, 0.011482, 0.0123003},
				{0.000376936, 0.00680535, 0.0107189, 0.0106276, -0, 0.0164936, 0.0139746, 0.0132631},
				{-2.56036e-05, 0.00669552, 0.0103548, 0.0117863, -0, 0.018069, 0.0144129, 0.013461},
				{-0.000226373, 0.00630309, 0.0116644, 0.0120787, -0, 0.018899, 0.0149006, 0.0134695},
				{-0.00024528, 0.00582503, 0.0124684, 0.0130444, -0, 0.0198472, 0.0153197, 0.0143236}};

				static double ff_c_w1_chi[6][8] = {   {0.00055722, 0.00324412, 0.00458122, 0.00488017, -0, 0.00822512, 0.0170127, 0.0151109},
				{0.000308256, 0.00395314, 0.00699612, 0.00690289, -0, 0.0116539, 0.0196131, 0.01546},
				{-0.000123962, 0.00405021, 0.00690036, 0.00792913, -0, 0.0122041, 0.0176647, 0.0154014},
				{0.000144894, 0.00620003, 0.00792612, 0.00954393, -0, 0.0146414, 0.0190081, 0.0155067},
				{0.000964061, 0.005571, 0.00902431, 0.00954985, -0, 0.0152926, 0.0182227, 0.0153517},
				{0.000919786, 0.00570915, 0.00943983, 0.00982737, -0, 0.0165903, 0.0167962, 0.0142445}};

				static double ff_u_rphi_chi_err[6][8] = {   {0.000145555, 0.000251143, 0.000587456, 0.00161861, 0, 0.000402677, 0.00438267},
				{0.00011059, 0.000411911, 0.000910795, 0.000412362, 0, 0.000750862, 0.00388078},
				{0.0013051, 0.00123342, 0.00070003, 0.000984029, 0, 0.00205485, 3.04605e-05},
				{0.000410067, 0.00125977, 0.000144962, 0.000565278, 0, 0.000379633, 0.00106428},
				{0.000468293, 0.00103781, 0.000517069, 0.000705955, 0, 0.000844178, 0.000274881},
				{0.000222143, 0.000250957, 0.000239207, 0.000538464, 0, 0.000513075, 0.00113828}};

				static double ff_u_rhad_chi_err[6][8] = {   {9.79841e-06, 2.91238e-05, 7.1895e-05, 7.33817e-06, 0, 0.00132255, 0.000876012},
				{1.94399e-05, 6.01646e-05, 7.23324e-05, 2.93711e-05, 0, 0.000636932, 0.000609662},
				{6.30643e-05, 5.4879e-05, 1.39081e-05, 9.66756e-07, 0, 0.000184905, 2.20035e-05},
				{1.89954e-05, 2.78326e-05, 3.11081e-05, 0.000150436, 0, 6.57256e-06, 1.48189e-05},
				{3.40774e-05, 6.76365e-06, 0.000159264, 0.00018586, 0, 0.000462591, 0.000180052},
				{0.000104197, 1.33654e-05, 6.53708e-05, 3.13592e-05, 0, 0.000321819, 0.000313973}};

				static double ff_u_reta_chi_err[6][8] = {   {0.00102706, 0.00048921, 0.00400043, 0.00360995, 0, 0.00241076, 0.00438581},
				{0.000527834, 0.000101319, 0.00233391, 0.00214396, 0, 0.00168218, 0.00374237},
				{0.00179502, 0.00165134, 0.000125017, 0.000587568, 0, 0.00209605, 0.000338974},
				{0.000871979, 0.00116226, 0.000164735, 0.000709153, 0, 6.57765e-05, 0.000979861},
				{0.000390077, 0.00081702, 0.00161319, 0.00137038, 0, 3.93721e-05, 0.00210269},
				{0.000297773, 0.000177982, 0.000589489, 0.0002611, 0, 0.000790529, 0.000688091}};

				static double ff_u_weta2_chi_err[6][8] = {   {7.1202e-05, 1.17299e-05, 1.95298e-06, 3.11678e-05, 0, 6.70669e-06, 0.000114791},
				{2.56372e-05, 1.83711e-05, 1.21221e-05, 5.07703e-05, 0, 1.72879e-05, 0.000127364},
				{9.63307e-05, 9.10368e-05, 4.81169e-05, 8.18606e-05, 0, 7.27804e-05, 6.21565e-06},
				{4.42357e-05, 0.000175239, 2.37284e-05, 2.84373e-05, 0, 2.4257e-05, 6.46085e-05},
				{6.64722e-06, 1.2284e-05, 3.40386e-05, 3.24961e-05, 0, 2.13922e-05, 5.35852e-05},
				{3.1675e-05, 5.03352e-05, 2.95174e-05, 4.07492e-05, 0, 1.78717e-05, 1.36532e-06}};

				static double ff_u_f1_chi_err[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_u_e277_chi_err[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_u_w1_chi_err[6][8] = {   {0.00371234, 0.00281797, 0.00168173, 0.000723196, 0, 0.00116206, 0.0134515},
				{0.00436411, 0.00146437, 0.000762466, 0.000493657, 0, 0.00122651, 0.0146219},
				{0.0095118, 0.00937812, 0.00366341, 0.00465473, 0, 0.0071245, 0.00135653},
				{0.00393187, 0.00982325, 0.00729881, 0.00590665, 0, 0.00757221, 0.00543194},
				{0.00440107, 0.00456161, 0.00512659, 0.00140228, 0, 0.0103917, 0.00504072},
				{0.00451904, 0.00123071, 0.00200703, 0.0027558, 0, 0.00292165, 0.00137391}};

				static double ff_u_fside_chi_err[6][8] = {   {0.00913793, 0.00883927, 0.00309012, 0.00643274, 0, 0.00365241, 0.0160639},
				{0.00616794, 0.00589542, 0.00532621, 0.0120994, 0, 0.00170149, 0.0196709},
				{0.00991892, 0.0148803, 0.0111056, 0.0163032, 0, 0.0116593, 0.000177419},
				{0.00495505, 0.0151499, 0.00307554, 0.00861681, 0, 0.0133697, 0.00311235},
				{0.00695625, 0.0110521, 0.00028196, 0.00587369, 0, 0.00910193, 0.00434791},
				{0.00234258, 0.00791064, 0.00886039, 0.000407279, 0, 0.00449833, 0.00226319}};

				static double ff_u_wtot_chi_err[6][8] = {   {0.10254, 0.0910314, 0.0423181, 0.0378824, 0, 0.0617723, 0.0461728},
				{0.0682785, 0.0410418, 0.0330434, 0.0652662, 0, 0.0343895, 0.0769995},
				{0.0718138, 0.0750812, 0.0495871, 0.0621898, 0, 0.0824177, 0.000865601},
				{0.0308609, 0.0423346, 0.0195408, 0.0278354, 0, 0.043065, 0.0222323},
				{0.0285184, 0.0203851, 0.00867095, 0.0278773, 0, 0.0228059, 0.0378373},
				{0.00368021, 0.0339077, 0.0450266, 0.0112787, 0, 0.00379147, 0.00606858}};

				static double ff_u_eratio_chi_err[6][8] = {   {0.00151867, 0.000753347, 7.56711e-05, 0.000421283, 0, 0.000340539, 6.92986e-05},
				{0.000415058, 0.000141914, 0.000527462, 0.00170303, 0, 0.000148271, 0.000420104},
				{0.00103535, 0.00101403, 7.53986e-05, 0.00186724, 0, 0.000283818, 8.52096e-05},
				{0.000486398, 0.00107923, 0.000952643, 0.000902153, 0, 0.000383539, 0.00189044},
				{0.000332998, 0.0015391, 0.000392367, 0.00291821, 0, 0.000733946, 0.000730926},
				{0.000143555, 0.000720778, 0.00159361, 0.000883296, 0, 0.000944475, 0.0014718}};

				static double ff_u_deltae_chi_err[6][8] = {   {3.58298, 2.16402, 1.94622, 2.16014, 0, 2.11486, 3.35809},
				{1.30357, 1.50909, 1.38991, 1.55066, 0, 1.16007, 2.52589},
				{1.78238, 1.13623, 2.00843, 1.18295, 0, 1.51954, 1.43984},
				{0.0867145, 1.21703, 1.18172, 0.592831, 0, 1.70157, 0.139377},
				{0.572386, 1.33345, 1.11264, 1.88971, 0, 3.22158, 0.486902},
				{0.210619, 0.452828, 1.23398, 0.452339, 0, 0.252405, 4.05356}};

				static double ff_c_rphi_chi_err[6][8] = {   {0.00284161, 0.00559012, 0.00851575, 0.0135177, 0, 0.00492686, 0.00902919},
				{0.00434741, 0.000866638, 0.00953911, 0.0124148, 0, 0.00280018, 0.00663933},
				{0.00583995, 0.00257671, 0.000900738, 0.000329827, 0, 0.00171913, 0.00216619},
				{0.00224165, 0.00406704, 0.00142001, 0.00413897, 0, 0.00317383, 4.69205e-05},
				{2.27993e-05, 0.00017402, 0.0019989, 0.00647134, 0, 0.00233093, 0.00352613},
				{0.000411942, 0.00321625, 0.00185059, 0.00300055, 0, 0.00158866, 0.0007092}};

				static double ff_c_rhad_chi_err[6][8] = {   {4.72883e-05, 4.74497e-05, 0.000102284, 0.000189297, 0, 0.000793124, 0.000123634},
				{8.76455e-05, 5.46348e-05, 1.17347e-05, 7.44386e-05, 0, 0.000420785, 0.000390956},
				{0.000114949, 4.29922e-05, 5.91757e-05, 4.63872e-05, 0, 5.09626e-05, 2.36447e-05},
				{8.52865e-05, 4.56089e-05, 9.41594e-06, 1.72688e-05, 0, 5.21394e-05, 2.46408e-05},
				{2.66939e-06, 0.000102443, 7.37301e-05, 4.55626e-05, 0, 4.858e-06, 6.29201e-05},
				{9.29134e-05, 1.75474e-05, 5.85676e-05, 9.84636e-05, 0, 9.06679e-05, 0.000271143}};

				static double ff_c_reta_chi_err[6][8] = {   {0.00041387, 0.0047073, 0.00393548, 0.00100631, 0, 0.000261004, 0.00212901},
				{0.00191094, 0.0038881, 0.00189043, 0.000712576, 0, 0.00205796, 0.00197173},
				{0.00320744, 0.0040134, 0.00031871, 0.00435119, 0, 0.00369361, 0.000422752},
				{0.00266242, 0.004407, 0.000235972, 0.00269963, 0, 0.00267598, 0.000436573},
				{0.000273128, 0.00010594, 0.000233222, 0.000963236, 0, 0.00205796, 0.00271694},
				{0.000330268, 0.00397075, 0.000436083, 0.000278275, 0, 0.00105876, 0.000518691}};

				static double ff_c_weta2_chi_err[6][8] = {   {0.000117486, 6.08528e-06, 5.10786e-05, 1.55982e-05, 0, 3.73754e-06, 5.81969e-05},
				{0.000144163, 9.10323e-05, 2.67777e-05, 3.91924e-05, 0, 2.59247e-05, 5.62413e-05},
				{0.000134922, 8.39788e-05, 7.86098e-05, 9.97029e-05, 0, 1.89756e-05, 2.73479e-05},
				{0.000115017, 0.000194895, 4.14098e-05, 0.00018006, 0, 6.86619e-05, 5.26409e-05},
				{7.7853e-06, 1.14488e-05, 7.07745e-05, 9.80987e-05, 0, 5.21851e-05, 9.24238e-05},
				{2.59732e-05, 8.80437e-05, 3.03397e-05, 3.22893e-05, 0, 2.93567e-06, 5.10657e-06}};

				static double ff_c_f1_chi_err[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};

				static double ff_c_e277_chi_err[6][8] = {{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0},
				{0., 0., 0., 0., 0., 0., 0., 0}};


				static double ff_c_w1_chi_err[6][8] = {   {0.00252391, 0.00318935, 0.00994866, 0.00857821, 0, 0.00953056, 0.0107075},
				{0.00564381, 0.00195682, 0.00484184, 0.00824637, 0, 0.00549494, 0.00437733},
				{0.00569869, 0.00475191, 0.000829015, 0.00124776, 0, 0.000525171, 0.00372868},
				{0.00525603, 0.00941328, 0.00281947, 0.00367208, 0, 0.0041289, 7.73519e-05},
				{0.00184974, 0.00249751, 0.00390663, 0.00343357, 0, 0.00285892, 0.0075188},
				{0.00294614, 0.00317692, 0.00165969, 0.000799104, 0, 0.00117082, 0.00943402}};

				static double ff_c_fside_chi_err[6][8] = {   {0.0126081, 0.0136497, 0.00726204, 0.00382259, 0, 0.00197543, 0.0115755},
				{0.0135454, 0.0180141, 0.00971758, 0.00384175, 0, 0.00847778, 0.0109834},
				{0.0102617, 0.0179196, 0.012826, 0.0104152, 0, 0.0186508, 0.00270787},
				{0.0151651, 0.0251708, 0.00845501, 0.0146246, 0, 0.0151818, 0.00114162},
				{0.00100931, 0.000176865, 0.00379681, 0.00179274, 0, 0.00631325, 0.00570146},
				{0.0016115, 0.0165259, 0.00475264, 0.00135864, 0, 0.00362643, 0.00539564}};

				static double ff_c_wtot_chi_err[6][8] = {   {0.241327, 0.226523, 0.119719, 0.0988437, 0, 0.17543, 0.00619493},
				{0.174013, 0.140375, 0.0698311, 0.058249, 0, 0.12878, 0.0111864},
				{0.0881063, 0.0920265, 0.0550266, 0.104017, 0, 0.105823, 0.0175544},
				{0.0845509, 0.11929, 0.0272041, 0.0356868, 0, 0.0941826, 0.032752},
				{0.00856708, 0.014385, 0.0336213, 0.0121556, 0, 0.0606694, 0.0330332},
				{0.0191425, 0.126561, 0.023859, 0.0294365, 0, 0.00511335, 0.011789}};

				static double ff_c_eratio_chi_err[6][8] = {   {0.00164282, 0.0012311, 0.000933729, 0.000537639, 0, 1.18189e-05, 0.000859877},
				{0.00142802, 0.00281423, 0.000305883, 0.00186237, 0, 6.40319e-06, 0.000578453},
				{0.00178011, 0.000485303, 0.000106829, 0.00239744, 0, 0.000151747, 9.11586e-06},
				{0.00244121, 0.000717719, 0.0023382, 0.000650615, 0, 0.000613819, 0.00187887},
				{0.000142659, 0.00151917, 0.000110291, 0.000484642, 0, 0.000568498, 1.00675e-05},
				{7.71838e-05, 0.00368299, 0.000473918, 0.000675649, 0, 0.00172014, 0.0013422}};

				static double ff_c_deltae_chi_err[6][8] = {   {7.36227, 7.56667, 8.43381, 5.81584, 0, 7.22459, 7.30561},
				{8.63784, 6.5389, 5.43356, 4.17721, 0, 6.4034, 5.35981},
				{2.44383, 2.09541, 3.96146, 5.59494, 0, 4.01474, 1.95888},
				{0.689197, 0.77232, 1.19159, 3.7579, 0, 2.56834, 12.3579},
				{1.13776, 1.51187, 3.81596, 4.34696, 0, 1.65685, 1.37495},
				{1.50038, 4.36603, 4.4211, 1.58884, 0, 2.32911, 0.681247}};


				if(isConv) {

				m_rhad1_ff.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_rhad_chi[0][0],   0. );
				m_rhad_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_rhad_chi[0][0],   0. );
				m_e277_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_e277_chi[0][0],   0. );
				m_reta_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_reta_chi[0][0],   0. );
				m_rphi_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_rphi_chi[0][0],   0. );
				m_weta2_ff.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_weta2_chi[0][0],  0. );
				m_f1_ff.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_f1_chi[0][0],     0. );
				m_fside_ff.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_fside_chi[0][0],  0. );
				m_wtot_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_wtot_chi[0][0] ,  0. );
				m_w1_ff.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_w1_chi[0][0],     0. );
				m_deltae_ff.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_deltae_chi[0][0], 0. );
				m_eratio_ff.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_eratio_chi[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_rhad_chi_err[0][0],   0. );
				m_rhad_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_rhad_chi_err[0][0],   0. );
				m_e277_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_e277_chi_err[0][0],   0. );
				m_reta_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_reta_chi_err[0][0],   0. );
				m_rphi_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_rphi_chi_err[0][0],   0. );
				m_weta2_fferr.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_weta2_chi_err[0][0],  0. );
				m_f1_fferr.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_f1_chi_err[0][0],     0. );
				m_fside_fferr.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_fside_chi_err[0][0],  0. );
				m_wtot_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_wtot_chi_err[0][0] ,  0. );
				m_w1_fferr.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_w1_chi_err[0][0],     0. );
				m_deltae_fferr.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_deltae_chi_err[0][0], 0. );
				m_eratio_fferr.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_c_eratio_chi_err[0][0], 0. );

				} else {

				m_rhad1_ff.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_rhad_chi[0][0],   0. );
				m_rhad_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_rhad_chi[0][0],   0. );
				m_e277_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_e277_chi[0][0],   0. );
				m_reta_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_reta_chi[0][0],   0. );
				m_rphi_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_rphi_chi[0][0],   0. );
				m_weta2_ff.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_weta2_chi[0][0],  0. );
				m_f1_ff.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_f1_chi[0][0],     0. );
				m_fside_ff.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_fside_chi[0][0],  0. );
				m_wtot_ff.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_wtot_chi[0][0],   0. );
				m_w1_ff.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_w1_chi[0][0],     0. );
				m_deltae_ff.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_deltae_chi[0][0], 0. );
				m_eratio_ff.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_eratio_chi[0][0], 0. );

				m_rhad1_fferr.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_rhad_chi_err[0][0],   0. );
				m_rhad_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_rhad_chi_err[0][0],   0. );
				m_e277_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_e277_chi_err[0][0],   0. );
				m_reta_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_reta_chi_err[0][0],   0. );
				m_rphi_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_rphi_chi_err[0][0],   0. );
				m_weta2_fferr.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_weta2_chi_err[0][0],  0. );
				m_f1_fferr.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_f1_chi_err[0][0],     0. );
				m_fside_fferr.Initialize  ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_fside_chi_err[0][0],  0. );
				m_wtot_fferr.Initialize   ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_wtot_chi_err[0][0],   0. );
				m_w1_fferr.Initialize     ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_w1_chi_err[0][0],     0. );
				m_deltae_fferr.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_deltae_chi_err[0][0], 0. );
				m_eratio_fferr.Initialize ( 6, 8, pt_DVs_chi, eta_DVs_chi, &ff_u_eratio_chi_err[0][0], 0. );

				}

				break;

		case 12: // tune 12

				static double pt_DVs_new_chisq_pur[8] = {25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_new_chisq_pur[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				// unconverted photons

				static double ff_u_rhad_new_chisq_pur[8][8] = {{1.7858e-04, 1.4402e-04, 6.8406e-04, 5.5012e-04, 9.9900e+02, 2.8417e-04, 3.6281e-04, -7.3599e-05},
				{8.1959e-05, 1.2329e-04, 4.3850e-04, 2.1686e-04, 9.9900e+02, 5.5351e-04, -3.5299e-04, 6.0236e-04},
				{-7.2296e-07, 9.8700e-05, 2.6697e-04, 2.8270e-04, 9.9900e+02, 1.4392e-04, -1.5271e-04, -3.7067e-05},
				{-2.0095e-05, 4.2643e-05, 2.6346e-04, 2.2738e-04, 9.9900e+02, 8.2978e-05, 2.6454e-04, -2.0966e-04},
				{-5.2462e-05, 2.2363e-05, 2.2802e-04, 1.9947e-04, 9.9900e+02, -9.4281e-05, 5.6085e-05, -7.3620e-05},
				{-1.1262e-04, 7.4728e-07, 1.3647e-04, 1.1595e-04, 9.9900e+02, -3.9199e-04, -1.5580e-04, 1.3486e-04},
				{-1.0669e-04, -1.8352e-05, 1.1616e-04, 7.7848e-05, 9.9900e+02, -2.5687e-04, -1.0858e-04, 9.8353e-06},
				{-6.5587e-04, -1.6765e-04, -9.1685e-05, -4.1898e-05, 9.9900e+02, -6.6073e-04, -5.7820e-04, -1.6899e-04}};

				static double ff_u_reta_new_chisq_pur[8][8] = {{-2.3068e-03, -2.8993e-03, -4.5743e-03, -4.1682e-03, 9.9900e+02, -6.6325e-03, -6.3081e-03, -6.4569e-03},
				{-2.0555e-03, -3.4335e-03, -4.9261e-03, -5.3187e-03, 9.9900e+02, -5.9449e-03, -7.0378e-03, -7.8881e-03},
				{-1.9363e-03, -2.6957e-03, -4.3542e-03, -4.8997e-03, 9.9900e+02, -6.2851e-03, -6.7338e-03, -8.2952e-03},
				{-1.7918e-03, -2.4901e-03, -4.0442e-03, -4.9332e-03, 9.9900e+02, -5.9399e-03, -6.2472e-03, -8.0472e-03},
				{-1.6300e-03, -1.9873e-03, -4.2981e-03, -5.0598e-03, 9.9900e+02, -5.2391e-03, -6.6208e-03, -8.1708e-03},
				{-1.4905e-03, -2.2135e-03, -4.1849e-03, -4.3723e-03, 9.9900e+02, -5.7103e-03, -6.3306e-03, -7.5901e-03},
				{-1.2511e-03, -1.8502e-03, -3.8846e-03, -4.1984e-03, 9.9900e+02, -4.9506e-03, -5.7797e-03, -6.9167e-03},
				{-2.6530e-03, -3.2598e-03, -5.1407e-03, -5.6445e-03, 9.9900e+02, -5.9842e-03, -6.4964e-03, -7.7465e-03}};

				static double ff_u_rphi_new_chisq_pur[8][8] = {{8.5213e-04, -3.1529e-04, 9.6052e-04, 1.5231e-05, 9.9900e+02, 6.9458e-04, 2.5717e-03, 1.9984e-03},
				{9.1010e-04, -7.4114e-05, 1.0489e-03, 8.2568e-04, 9.9900e+02, 7.1986e-04, 1.9710e-03, -2.3474e-04},
				{8.6877e-04, -2.9354e-04, 1.5536e-03, -3.5740e-04, 9.9900e+02, -3.2989e-07, 1.7353e-03, 1.4969e-04},
				{1.2671e-03, 2.0691e-04, 1.5653e-03, -3.1363e-04, 9.9900e+02, 5.3966e-04, 1.7818e-03, 8.9386e-04},
				{1.3449e-03, 2.2986e-04, 1.7716e-03, -3.7985e-04, 9.9900e+02, 7.1632e-04, 1.5798e-03, 7.9430e-04},
				{1.3550e-03, -5.4377e-05, 1.4697e-03, 7.1364e-05, 9.9900e+02, 9.2869e-04, 2.1355e-03, 1.4809e-03},
				{1.4209e-03, 3.1335e-04, 1.7704e-03, 2.3396e-04, 9.9900e+02, 1.7012e-03, 2.0209e-03, 2.0087e-03},
				{4.0058e-04, -9.9172e-04, 4.3677e-04, -1.4456e-03, 9.9900e+02, 6.3334e-04, 2.0021e-03, 1.8262e-03}};

				static double ff_u_weta2_new_chisq_pur[8][8] = {{9.3458e-05, 1.6103e-04, 6.0892e-05, 1.1057e-04, 9.9900e+02, 3.2310e-04, 3.3996e-04, 3.2018e-04},
				{1.1584e-04, 1.5626e-04, 5.3682e-05, 4.4900e-05, 9.9900e+02, 3.2606e-04, 3.7811e-04, 4.1968e-04},
				{1.1910e-04, 1.8329e-04, 6.3581e-05, 8.9093e-05, 9.9900e+02, 3.4057e-04, 3.7862e-04, 4.0401e-04},
				{9.7005e-05, 1.4150e-04, 5.2289e-05, 9.2457e-05, 9.9900e+02, 3.4885e-04, 3.9474e-04, 3.9161e-04},
				{9.9524e-05, 1.4623e-04, 4.0408e-05, 9.0906e-05, 9.9900e+02, 3.3932e-04, 4.1356e-04, 3.8757e-04},
				{9.0747e-05, 1.7420e-04, 4.9860e-05, 1.0720e-04, 9.9900e+02, 3.4222e-04, 3.8398e-04, 3.8552e-04},
				{8.3714e-05, 1.4496e-04, 4.3860e-05, 9.3575e-05, 9.9900e+02, 3.3467e-04, 3.8377e-04, 3.5055e-04},
				{2.2780e-04, 2.9397e-04, 1.5893e-04, 2.1975e-04, 9.9900e+02, 4.2985e-04, 4.2641e-04, 4.1532e-04}};

				static double ff_u_f1_new_chisq_pur[8][8] = {{-6.2080e-03, -8.9730e-03, -9.1638e-03, -1.0843e-02, 9.9900e+02, 1.6775e-04, 1.1778e-02, 1.4347e-02},
				{-2.6898e-03, -1.5356e-03, -6.0194e-03, -6.7691e-03, 9.9900e+02, 4.0404e-03, 1.7155e-02, 3.3840e-02},
				{2.5486e-03, -2.0180e-03, -4.0611e-03, -5.1328e-03, 9.9900e+02, 1.4509e-02, 2.2575e-02, 1.8293e-02},
				{-2.7140e-03, -7.4032e-03, -8.1568e-03, -7.4500e-03, 9.9900e+02, 9.3926e-03, 1.8753e-02, 1.9443e-02},
				{-2.5462e-03, -8.6732e-03, -5.7485e-03, -5.5116e-03, 9.9900e+02, 7.8666e-03, 2.1613e-02, 1.7603e-02},
				{1.8172e-04, 8.2576e-04, -3.2113e-03, -4.7934e-03, 9.9900e+02, 1.3119e-02, 1.7380e-02, 1.3740e-02},
				{-3.7097e-03, -8.7664e-04, -4.2560e-03, -5.9967e-03, 9.9900e+02, 6.0602e-03, 1.1446e-02, 4.4280e-03},
				{3.7104e-02, 4.3456e-02, 2.1530e-02, 2.1196e-02, 9.9900e+02, 2.6023e-02, 2.0590e-02, 1.4852e-02}};

				static double ff_u_deltae_new_chisq_pur[8][8] = {{-1.3739e+00, -1.0774e+00, -1.5007e+00, -1.7015e+00, 9.9900e+02, -1.1257e+00, -6.0373e+00, -2.0727e+00},
				{-1.5444e+00, -1.2974e+00, -1.8495e+00, -1.7712e+00, 9.9900e+02, -2.0673e+00, -3.0024e+00, -3.3184e+00},
				{-1.0543e+00, -1.2688e+00, -1.1217e+00, -7.3906e-01, 9.9900e+02, -1.9869e+00, -4.3620e+00, -1.7989e+00},
				{-1.5148e+00, -1.7912e+00, -9.0143e-01, -1.3898e+00, 9.9900e+02, 1.4168e-02, -3.3580e+00, -1.5658e+00},
				{-1.5414e+00, -1.0457e+00, -6.6648e-01, -1.3506e+00, 9.9900e+02, -4.9641e-01, -3.6883e+00, -1.0992e+00},
				{-1.3154e+00, -1.5041e+00, -1.0116e+00, -1.6259e+00, 9.9900e+02, -5.7845e-01, -2.3155e+00, -1.1231e+00},
				{-1.0659e+00, -1.4799e+00, -1.3949e+00, -1.4202e+00, 9.9900e+02, -1.2014e+00, -5.0457e+00, -2.2387e+00},
				{3.9626e-02, 5.8205e-01, 7.3358e-03, 8.2364e-01, 9.9900e+02, 5.4254e-01, -3.0481e+00, -2.0074e-01}};

				static double ff_u_wtot_new_chisq_pur[8][8] = {{7.1983e-02, 7.6138e-02, 8.0021e-02, 8.8388e-02, 9.9900e+02, 1.3514e-01, 1.3883e-01, 1.0947e-01},
				{5.3154e-02, 7.3466e-02, 7.6292e-02, 6.6369e-02, 9.9900e+02, 1.2153e-01, 1.2626e-01, 1.1401e-01},
				{5.2844e-02, 8.1229e-02, 8.0744e-02, 9.4198e-02, 9.9900e+02, 1.4696e-01, 1.4354e-01, 1.0942e-01},
				{5.0140e-02, 7.0032e-02, 7.0291e-02, 9.6855e-02, 9.9900e+02, 1.4028e-01, 1.4875e-01, 1.0796e-01},
				{4.8693e-02, 6.7975e-02, 7.9260e-02, 1.0417e-01, 9.9900e+02, 1.3008e-01, 1.4260e-01, 1.1635e-01},
				{4.7690e-02, 7.3190e-02, 8.2654e-02, 9.7499e-02, 9.9900e+02, 1.3395e-01, 1.4097e-01, 1.1425e-01},
				{4.0691e-02, 5.8929e-02, 6.2158e-02, 8.8617e-02, 9.9900e+02, 1.1400e-01, 1.3630e-01, 1.0766e-01},
				{8.7258e-03, 5.7075e-02, 5.6976e-02, 8.9145e-02, 9.9900e+02, 1.1622e-01, 1.1934e-01, 1.1192e-01}};

				static double ff_u_fside_new_chisq_pur[8][8] = {{1.3071e-02, 1.9288e-02, 2.3892e-02, 2.1756e-02, 9.9900e+02, 3.6826e-02, 3.5327e-02, 2.9326e-02},
				{1.1524e-02, 2.0590e-02, 2.3627e-02, 1.7394e-02, 9.9900e+02, 3.7056e-02, 3.9473e-02, 3.1563e-02},
				{1.1800e-02, 1.8942e-02, 2.1492e-02, 2.5045e-02, 9.9900e+02, 4.4580e-02, 3.8411e-02, 2.9255e-02},
				{1.0570e-02, 1.6049e-02, 2.0849e-02, 2.4929e-02, 9.9900e+02, 4.1895e-02, 3.6648e-02, 2.8643e-02},
				{1.0154e-02, 1.6499e-02, 2.0227e-02, 2.7313e-02, 9.9900e+02, 3.8876e-02, 3.5369e-02, 3.0295e-02},
				{1.0126e-02, 1.6628e-02, 2.0399e-02, 2.4503e-02, 9.9900e+02, 3.7212e-02, 3.4932e-02, 2.7573e-02},
				{7.5043e-03, 1.3647e-02, 1.9131e-02, 2.2268e-02, 9.9900e+02, 3.3822e-02, 3.2968e-02, 2.5976e-02},
				{9.5007e-03, 1.8118e-02, 2.0987e-02, 2.7409e-02, 9.9900e+02, 3.4169e-02, 3.0804e-02, 2.6554e-02}};

				static double ff_u_w1_new_chisq_pur[8][8] = {{4.2807e-03, 1.0174e-02, 1.2879e-02, 1.1466e-02, 9.9900e+02, 2.0131e-02, 3.0904e-02, 3.4806e-02},
				{5.1809e-03, 9.6268e-03, 1.3396e-02, 9.7379e-03, 9.9900e+02, 2.2226e-02, 3.1359e-02, 3.9073e-02},
				{5.2681e-03, 1.0066e-02, 1.2743e-02, 1.2452e-02, 9.9900e+02, 2.2713e-02, 3.4326e-02, 3.6497e-02},
				{3.9436e-03, 9.1359e-03, 1.2202e-02, 1.3720e-02, 9.9900e+02, 2.2620e-02, 3.4307e-02, 3.2157e-02},
				{3.8525e-03, 8.2132e-03, 1.2609e-02, 1.5741e-02, 9.9900e+02, 2.5358e-02, 3.6848e-02, 3.4035e-02},
				{4.3762e-03, 1.2047e-02, 1.2765e-02, 1.5841e-02, 9.9900e+02, 2.4337e-02, 3.2473e-02, 3.2831e-02},
				{3.6857e-03, 9.4018e-03, 1.3561e-02, 1.6502e-02, 9.9900e+02, 2.3177e-02, 2.9490e-02, 3.1795e-02},
				{9.7592e-03, 1.5898e-02, 2.2422e-02, 2.3961e-02, 9.9900e+02, 2.7471e-02, 3.1598e-02, 3.0956e-02}};

				static double ff_u_eratio_new_chisq_pur[8][8] = {{-9.6958e-05, -3.4242e-04, -2.1964e-03, -1.1030e-03, 9.9900e+02, -2.0277e-03, -3.9162e-04, -5.1865e-04},
				{2.8123e-04, -5.8860e-04, -3.5323e-04, -1.7086e-05, 9.9900e+02, -1.0479e-03, -5.3820e-04, 6.0886e-05},
				{-8.7900e-05, -1.0026e-03, -9.2076e-04, -1.6421e-03, 9.9900e+02, -2.0873e-03, -1.0344e-04, -8.8286e-04},
				{4.9649e-04, -4.0489e-04, -7.6554e-04, -1.2599e-03, 9.9900e+02, -1.0190e-03, -3.0616e-05, -3.9208e-04},
				{1.0674e-03, -8.9351e-05, -6.6943e-04, 1.7938e-04, 9.9900e+02, -9.5436e-04, 4.2978e-04, -2.3619e-04},
				{1.6689e-04, -5.7027e-04, -1.0379e-04, -1.1863e-03, 9.9900e+02, -1.0991e-03, -3.1090e-04, -5.3819e-04},
				{7.4386e-04, -2.2432e-05, 5.7510e-04, -5.8267e-04, 9.9900e+02, -8.8226e-04, 3.5879e-04, -5.0035e-04},
				{-2.1309e-03, -2.3962e-03, -3.5599e-03, -4.7105e-03, 9.9900e+02, -2.9062e-03, -9.4111e-04, -8.3679e-04}};

				static double ff_u_e277_new_chisq_pur[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				// converted photons

				static double ff_c_rhad_new_chisq_pur[8][8] = {{9.1556e-05, 1.1465e-04, 6.7787e-04, 6.0443e-05, 9.9900e+02, -5.4007e-04, -5.9934e-05, 1.1164e-04},
				{1.7671e-04, 2.8144e-04, 4.9312e-04, 2.2977e-04, 9.9900e+02, -2.8662e-05, -7.6771e-04, 4.9390e-05},
				{-3.0305e-05, -5.3635e-05, 2.8955e-04, 3.0601e-04, 9.9900e+02, -3.0721e-04, -2.4976e-04, 7.5877e-05},
				{6.7461e-05, 8.7333e-05, 3.0555e-04, 2.1864e-04, 9.9900e+02, -1.9126e-04, -1.8922e-04, -1.6343e-04},
				{-2.3336e-05, 8.6910e-05, 2.5501e-04, 1.7994e-04, 9.9900e+02, 5.3524e-05, -1.2737e-04, 3.0757e-04},
				{-1.1624e-04, -8.4055e-05, 2.4136e-04, 1.4031e-04, 9.9900e+02, -4.4791e-05, -1.5613e-04, -9.6423e-05},
				{-1.0005e-04, -5.8050e-05, 1.9792e-04, -1.9760e-05, 9.9900e+02, 4.5942e-06, 5.9591e-05, 4.7973e-06},
				{-5.1084e-04, -1.1372e-04, -4.2823e-05, 2.1796e-06, 9.9900e+02, -5.6291e-04, -5.5119e-04, -2.0291e-04}};

				static double ff_c_reta_new_chisq_pur[8][8] = {{-1.3400e-03, -2.8020e-03, -4.3988e-03, -4.2591e-03, 9.9900e+02, -7.3343e-03, -6.9802e-03, -6.5924e-03},
				{-9.2905e-04, -3.5081e-03, -2.6275e-03, -6.1334e-03, 9.9900e+02, -7.9083e-03, -7.6790e-03, -6.4277e-03},
				{-1.6684e-03, -2.6424e-03, -4.0385e-03, -4.3736e-03, 9.9900e+02, -7.4233e-03, -7.6555e-03, -8.0149e-03},
				{-1.4535e-03, -1.6870e-03, -3.5008e-03, -4.3984e-03, 9.9900e+02, -7.5707e-03, -7.3832e-03, -7.3381e-03},
				{-1.4937e-03, -1.9258e-03, -3.8749e-03, -5.1780e-03, 9.9900e+02, -7.0738e-03, -7.8150e-03, -7.8186e-03},
				{-1.5121e-03, -2.1659e-03, -4.0871e-03, -5.0273e-03, 9.9900e+02, -6.7139e-03, -7.3191e-03, -7.9004e-03},
				{-2.1242e-03, -2.0371e-03, -3.0796e-03, -2.4018e-03, 9.9900e+02, -6.4435e-03, -6.9200e-03, -7.7046e-03},
				{-3.6530e-03, -4.2136e-03, -5.4649e-03, -6.5313e-03, 9.9900e+02, -7.8872e-03, -7.6620e-03, -8.0931e-03}};

				static double ff_c_rphi_new_chisq_pur[8][8] = {{-1.5507e-03, 2.7907e-03, -5.5222e-04, -3.5433e-03, 9.9900e+02, -7.4987e-03, -7.4748e-03, -6.9086e-03},
				{1.2575e-03, -5.0808e-03, -6.6955e-03, -8.0742e-03, 9.9900e+02, -1.4485e-02, -9.5522e-03, -5.7143e-03},
				{9.7946e-04, -7.6525e-03, -7.8474e-03, -9.8454e-03, 9.9900e+02, -8.5040e-03, -6.1802e-03, -3.7612e-03},
				{4.2857e-04, -4.7885e-03, -5.7303e-03, -1.0967e-02, 9.9900e+02, -7.5368e-03, -5.0055e-03, -2.3275e-03},
				{-1.4584e-03, -7.7815e-03, -6.9749e-03, -1.0871e-02, 9.9900e+02, -6.3528e-03, -4.2559e-03, -1.4662e-03},
				{-1.8970e-03, -5.2674e-03, -2.7961e-03, -6.4688e-03, 9.9900e+02, -2.2266e-03, -1.1755e-03, -3.8684e-04},
				{2.4100e-04, -2.2869e-03, -6.5567e-04, -1.2688e-03, 9.9900e+02, -9.3894e-04, 4.1677e-04, 5.4576e-04},
				{-3.0259e-03, -5.3230e-03, -3.4547e-03, -5.9889e-03, 9.9900e+02, -2.3579e-03, -2.3451e-04, 5.1004e-04}};

				static double ff_c_weta2_new_chisq_pur[8][8] = {{1.4231e-04, 7.0072e-05, 1.7698e-04, 1.0226e-04, 9.9900e+02, 3.5367e-04, 4.0060e-04, 2.7044e-04},
				{1.3743e-04, 1.9884e-04, 1.0251e-04, 7.7251e-05, 9.9900e+02, 4.1447e-04, 4.0007e-04, 3.2213e-04},
				{1.2062e-04, 2.1253e-04, 5.9672e-05, 8.3694e-05, 9.9900e+02, 3.7736e-04, 4.3016e-04, 3.6181e-04},
				{9.7787e-05, 1.2596e-04, 5.7947e-05, 1.3193e-04, 9.9900e+02, 3.5594e-04, 3.9943e-04, 3.6164e-04},
				{1.1782e-04, 1.7165e-04, 9.2857e-05, 1.0981e-04, 9.9900e+02, 3.5966e-04, 4.6248e-04, 3.5979e-04},
				{8.6255e-05, 1.5570e-04, 1.0211e-04, 8.7293e-05, 9.9900e+02, 3.5504e-04, 4.3057e-04, 4.2271e-04},
				{1.2060e-04, 1.8682e-04, 1.8351e-05, 1.1292e-04, 9.9900e+02, 3.3554e-04, 4.1568e-04, 3.8851e-04},
				{2.7062e-04, 3.4155e-04, 2.0818e-04, 2.5578e-04, 9.9900e+02, 4.7709e-04, 4.9270e-04, 4.0196e-04}};

				static double ff_c_f1_new_chisq_pur[8][8] = {{1.4316e-03, 1.3183e-02, -1.1530e-03, -3.7634e-03, 9.9900e+02, 8.2663e-03, 1.3700e-02, 6.6818e-03},
				{5.4594e-03, -9.9421e-04, -1.4964e-03, -7.7321e-03, 9.9900e+02, 1.7070e-02, 2.7893e-02, 1.4969e-02},
				{6.8274e-03, 7.2186e-03, 8.2965e-04, -4.4247e-03, 9.9900e+02, 1.4478e-02, 3.0595e-02, 2.2068e-02},
				{4.9701e-03, -3.5049e-04, -3.7318e-03, -1.1717e-03, 9.9900e+02, 1.2807e-02, 2.6448e-02, 2.2610e-02},
				{1.6509e-03, 2.4531e-03, -1.1773e-03, -3.4196e-05, 9.9900e+02, 1.2486e-02, 3.0129e-02, 1.5154e-02},
				{5.8387e-03, 8.9925e-03, 6.3563e-04, -1.3853e-03, 9.9900e+02, 1.3248e-02, 2.3634e-02, 2.6370e-02},
				{4.4989e-03, -8.2267e-03, -5.7955e-03, -4.3952e-03, 9.9900e+02, 1.7738e-02, 2.1915e-02, 1.3526e-02},
				{5.6418e-02, 5.0666e-02, 2.9572e-02, 2.1880e-02, 9.9900e+02, 3.1195e-02, 3.9005e-02, 3.1119e-02}};

				static double ff_c_deltae_new_chisq_pur[8][8] = {{-3.8469e+00, -2.6271e+00, -1.5152e+00, -2.0253e+00, 9.9900e+02, -1.6871e+00, -3.8785e+00, -2.4780e+00},
				{-2.7594e+00, -2.1875e+00, -3.0526e+00, -3.6707e+00, 9.9900e+02, -3.1251e+00, -4.7592e+00, -2.8182e+00},
				{-2.0513e+00, -2.3604e+00, -1.6819e+00, -2.5743e+00, 9.9900e+02, 8.0075e-01, -1.8809e+00, -1.2902e+00},
				{-8.9868e-01, -1.5440e+00, -1.0658e+00, -2.0459e+00, 9.9900e+02, -7.0275e-01, -1.7909e+00, -2.0947e+00},
				{-1.1440e+00, -3.2742e+00, -1.4752e+00, -1.1677e+00, 9.9900e+02, 9.5697e-02, -1.2039e+00, -6.2429e-01},
				{-1.6913e+00, -1.0265e-01, -1.0659e+00, -2.7912e+00, 9.9900e+02, -2.4652e-01, -2.6966e+00, -2.2062e+00},
				{-1.8684e+00, -2.7017e-01, -1.0326e+00, -1.5705e+00, 9.9900e+02, 5.8783e-01, -2.6436e+00, -2.0501e+00},
				{6.4429e-01, 1.5022e+00, 1.0632e+00, 1.5027e+00, 9.9900e+02, -9.8959e-01, -1.1690e+00, 7.8730e-02}};

				static double ff_c_wtot_new_chisq_pur[8][8] = {{5.9341e-02, 1.0844e-01, 9.5311e-02, 9.6817e-02, 9.9900e+02, 1.2537e-01, 1.4986e-01, 8.6922e-02},
				{8.9545e-02, 8.0570e-02, 9.4543e-02, 5.7506e-02, 9.9900e+02, 1.5673e-01, 1.4143e-01, 9.4502e-02},
				{6.8700e-02, 1.0133e-01, 1.0579e-01, 9.3619e-02, 9.9900e+02, 1.6030e-01, 1.5754e-01, 1.0207e-01},
				{4.8087e-02, 8.8253e-02, 9.6745e-02, 1.2636e-01, 9.9900e+02, 1.6764e-01, 1.5702e-01, 9.8503e-02},
				{6.4664e-02, 8.4878e-02, 9.3290e-02, 1.2321e-01, 9.9900e+02, 1.7759e-01, 1.6253e-01, 1.0670e-01},
				{5.9479e-02, 1.1493e-01, 1.0851e-01, 1.2628e-01, 9.9900e+02, 1.6259e-01, 1.4122e-01, 1.0415e-01},
				{4.4681e-02, 8.5403e-02, 1.0448e-01, 9.9571e-02, 9.9900e+02, 1.6157e-01, 1.5111e-01, 1.1063e-01},
				{6.3554e-02, 1.1259e-01, 1.1858e-01, 1.4112e-01, 9.9900e+02, 1.8309e-01, 1.5897e-01, 1.0903e-01}};

				static double ff_c_fside_new_chisq_pur[8][8] = {{1.4535e-02, 1.8603e-02, 2.5969e-02, 2.6387e-02, 9.9900e+02, 4.4836e-02, 5.0149e-02, 2.7033e-02},
				{1.8021e-02, 2.1123e-02, 2.9635e-02, 2.5044e-02, 9.9900e+02, 5.5666e-02, 4.8645e-02, 3.1295e-02},
				{1.5483e-02, 2.5377e-02, 2.9263e-02, 2.9295e-02, 9.9900e+02, 4.9796e-02, 4.7920e-02, 3.1911e-02},
				{1.3621e-02, 2.1656e-02, 2.6276e-02, 3.4779e-02, 9.9900e+02, 5.1352e-02, 4.7396e-02, 3.0460e-02},
				{1.4410e-02, 2.0648e-02, 2.5166e-02, 3.8778e-02, 9.9900e+02, 5.1798e-02, 4.6649e-02, 3.1850e-02},
				{1.3455e-02, 2.2938e-02, 2.7751e-02, 3.4682e-02, 9.9900e+02, 4.7848e-02, 4.0869e-02, 3.0503e-02},
				{1.4427e-02, 2.3012e-02, 2.5899e-02, 2.8335e-02, 9.9900e+02, 4.6122e-02, 4.0759e-02, 2.9985e-02},
				{2.1801e-02, 3.4274e-02, 4.0137e-02, 4.4919e-02, 9.9900e+02, 5.2918e-02, 4.2431e-02, 2.9305e-02}};

				static double ff_c_w1_new_chisq_pur[8][8] = {{4.9204e-03, 1.6860e-02, 1.6880e-02, 1.3660e-02, 9.9900e+02, 1.7197e-02, 3.5400e-02, 3.2078e-02},
				{1.0168e-02, 5.4465e-03, 1.7874e-02, 1.1531e-02, 9.9900e+02, 2.5331e-02, 3.6229e-02, 3.3872e-02},
				{6.4330e-03, 1.5734e-02, 1.4790e-02, 1.2431e-02, 9.9900e+02, 2.1067e-02, 3.4554e-02, 3.3145e-02},
				{7.1684e-03, 1.4795e-02, 1.1995e-02, 1.3330e-02, 9.9900e+02, 2.1464e-02, 3.5037e-02, 2.9851e-02},
				{4.6992e-03, 1.1430e-02, 1.0953e-02, 1.6666e-02, 9.9900e+02, 2.1998e-02, 3.7485e-02, 2.8917e-02},
				{6.2584e-03, 1.6173e-02, 1.3929e-02, 1.3840e-02, 9.9900e+02, 2.0471e-02, 3.1060e-02, 3.2362e-02},
				{6.7858e-03, 1.1404e-02, 1.7850e-02, 1.5983e-02, 9.9900e+02, 2.2163e-02, 2.8610e-02, 3.3304e-02},
				{1.5486e-02, 2.2565e-02, 2.7256e-02, 2.4829e-02, 9.9900e+02, 2.7990e-02, 3.5025e-02, 3.2252e-02}};

				static double ff_c_eratio_new_chisq_pur[8][8] = {{6.8613e-04, 4.5502e-03, 1.2325e-03, 1.5049e-03, 9.9900e+02, -1.2207e-03, -4.8314e-04, -2.0097e-04},
				{-4.9654e-04, 2.6310e-04, 2.7778e-04, 2.2358e-03, 9.9900e+02, -1.2122e-04, 3.6858e-04, -9.6819e-04},
				{9.9208e-05, -2.4659e-05, -1.2047e-03, 2.9588e-05, 9.9900e+02, -3.5813e-03, -3.9042e-04, -6.4716e-04},
				{1.4347e-03, -5.5318e-04, 6.2610e-05, -3.9798e-03, 9.9900e+02, -2.5477e-03, -6.4128e-05, -4.8700e-04},
				{1.5036e-03, 2.3490e-04, -1.8195e-04, -1.3746e-03, 9.9900e+02, -2.2475e-03, 7.9780e-05, -8.4258e-04},
				{-2.0437e-05, -1.9949e-03, -5.8130e-04, -2.6318e-03, 9.9900e+02, -1.3357e-03, 3.2314e-05, -9.3000e-05},
				{-3.1858e-04, 4.9383e-04, -9.4082e-04, -2.6795e-03, 9.9900e+02, -1.0881e-03, -2.2854e-04, -7.4506e-04},
				{-2.8176e-03, -3.2910e-03, -5.2302e-03, -5.8513e-03, 9.9900e+02, -3.8179e-03, -1.0006e-03, -6.4177e-04}};

				static double ff_c_e277_new_chisq_pur[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};


				static double ff_u_rhad_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_reta_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_rphi_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_weta2_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_f1_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_deltae_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_wtot_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_fside_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_w1_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_eratio_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_u_e277_err_new_chisq_pur[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				static double ff_c_rhad_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_reta_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_rphi_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_weta2_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_f1_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_deltae_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_wtot_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_fside_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_w1_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_eratio_err_new_chisq_pur[8][8] = {{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00},
				{0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00, 0.0000e+00}};

				static double ff_c_e277_err_new_chisq_pur[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};


				if (isConv) {

				m_rhad_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_rhad_new_chisq_pur[0][0], 0.);
				m_rhad1_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_rhad_new_chisq_pur[0][0], 0.);
				m_reta_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_reta_new_chisq_pur[0][0], 0.);
				m_rphi_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_rphi_new_chisq_pur[0][0], 0.);
				m_weta2_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_weta2_new_chisq_pur[0][0], 0.);
				m_f1_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_f1_new_chisq_pur[0][0], 0.);
				m_deltae_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_deltae_new_chisq_pur[0][0], 0.);
				m_wtot_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_wtot_new_chisq_pur[0][0], 0.);
				m_fside_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_fside_new_chisq_pur[0][0], 0.);
				m_w1_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_w1_new_chisq_pur[0][0], 0.);
				m_eratio_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_eratio_new_chisq_pur[0][0], 0.);
				m_e277_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_e277_new_chisq_pur[0][0], 0.);

				m_rhad_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_rhad_err_new_chisq_pur[0][0], 0.);
				m_rhad1_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_rhad_err_new_chisq_pur[0][0], 0.);
				m_reta_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_reta_err_new_chisq_pur[0][0], 0.);
				m_rphi_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_rphi_err_new_chisq_pur[0][0], 0.);
				m_weta2_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_weta2_err_new_chisq_pur[0][0], 0.);
				m_f1_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_f1_err_new_chisq_pur[0][0], 0.);
				m_deltae_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_deltae_err_new_chisq_pur[0][0], 0.);
				m_wtot_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_wtot_err_new_chisq_pur[0][0], 0.);
				m_fside_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_fside_err_new_chisq_pur[0][0], 0.);
				m_w1_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_w1_err_new_chisq_pur[0][0], 0.);
				m_eratio_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_eratio_err_new_chisq_pur[0][0], 0.);
				m_e277_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_c_e277_err_new_chisq_pur[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_rhad_new_chisq_pur[0][0], 0.);
				m_rhad1_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_rhad_new_chisq_pur[0][0], 0.);
				m_reta_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_reta_new_chisq_pur[0][0], 0.);
				m_rphi_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_rphi_new_chisq_pur[0][0], 0.);
				m_weta2_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_weta2_new_chisq_pur[0][0], 0.);
				m_f1_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_f1_new_chisq_pur[0][0], 0.);
				m_deltae_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_deltae_new_chisq_pur[0][0], 0.);
				m_wtot_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_wtot_new_chisq_pur[0][0], 0.);
				m_fside_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_fside_new_chisq_pur[0][0], 0.);
				m_w1_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_w1_new_chisq_pur[0][0], 0.);
				m_eratio_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_eratio_new_chisq_pur[0][0], 0.);
				m_e277_ff.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_e277_new_chisq_pur[0][0], 0.);

				m_rhad_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_rhad_err_new_chisq_pur[0][0], 0.);
				m_rhad1_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_rhad_err_new_chisq_pur[0][0], 0.);
				m_reta_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_reta_err_new_chisq_pur[0][0], 0.);
				m_rphi_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_rphi_err_new_chisq_pur[0][0], 0.);
				m_weta2_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_weta2_err_new_chisq_pur[0][0], 0.);
				m_f1_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_f1_err_new_chisq_pur[0][0], 0.);
				m_deltae_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_deltae_err_new_chisq_pur[0][0], 0.);
				m_wtot_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_wtot_err_new_chisq_pur[0][0], 0.);
				m_fside_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_fside_err_new_chisq_pur[0][0], 0.);
				m_w1_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_w1_err_new_chisq_pur[0][0], 0.);
				m_eratio_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_eratio_err_new_chisq_pur[0][0], 0.);
				m_e277_fferr.Initialize(8, 8, pt_DVs_new_chisq_pur, eta_DVs_new_chisq_pur, &ff_u_e277_err_new_chisq_pur[0][0], 0.);

				}

				break;

		case 13: // tune 13 has to be used for 2012 analysis. It has been obtained comparing data12 in periodA
				// with MC12 JF samples after pileup reweighting. The method here is the chi2

				static double pt_DVs_13_chisq[8] = {25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_13_chisq[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				//unconverted photons

				static double ff_u_rhad_13_chisq[8][8] = {{1.2833e-04, 2.1679e-04, 6.6720e-04, 4.7214e-04, 9.9900e+02, -6.3263e-04, 1.6676e-03, -4.9325e-05},
				{2.3753e-04, 1.9994e-04, -1.1956e-05, 3.7625e-04, 9.9900e+02, 1.0683e-03, -1.1247e-04, -3.4640e-04},
				{3.9203e-05, -3.6523e-05, 3.9540e-04, -1.2430e-05, 9.9900e+02, 1.1811e-03, -5.0413e-04, -7.1306e-04},
				{-1.1253e-04, 4.6473e-06, 3.8214e-04, 1.3473e-04, 9.9900e+02, 2.9433e-04, 1.6857e-03, -2.9948e-05},
				{-4.0916e-05, -2.5095e-04, 2.7341e-04, 9.7869e-05, 9.9900e+02, 9.6945e-04, 2.4436e-04, 1.6389e-04},
				{-1.0745e-04, -6.3189e-06, 2.5459e-04, 2.3106e-04, 9.9900e+02, 6.7424e-04, 5.4849e-06, -5.0039e-06},
				{-1.5511e-04, -3.6698e-04, 1.8064e-04, -1.8091e-04, 9.9900e+02, -3.3914e-06, -9.8439e-04, -1.7662e-04},
				{-3.6311e-04, -1.3206e-05, 2.3372e-05, 7.1267e-06, 9.9900e+02, -2.9548e-04, 3.2052e-04, -6.9867e-05}};

				static double ff_u_reta_13_chisq[8][8] = {{-2.7890e-03, -3.1398e-03, -4.3743e-03, -3.3237e-03, 9.9900e+02, -6.6365e-03, -6.8258e-03, -8.0813e-03},
				{-1.8224e-03, -4.5915e-03, -3.4159e-03, -4.3950e-03, 9.9900e+02, -7.2058e-03, -7.0472e-03, -8.7689e-03},
				{-9.8536e-04, -3.9560e-03, -5.4173e-03, -5.8489e-03, 9.9900e+02, -5.8044e-03, -7.9397e-03, -9.6702e-03},
				{-1.6123e-03, -1.8852e-03, -3.4351e-03, -5.3439e-03, 9.9900e+02, -6.6647e-03, -6.3820e-03, -9.2232e-03},
				{-7.8247e-04, -7.4932e-04, -3.8756e-03, -4.1773e-03, 9.9900e+02, -5.1671e-03, -6.5291e-03, -9.7084e-03},
				{-1.6848e-03, -2.2316e-03, -6.0613e-03, -4.9577e-03, 9.9900e+02, -4.9543e-03, -7.8720e-03, -7.8257e-03},
				{-1.0586e-03, -2.3140e-03, -4.4761e-03, -4.3487e-03, 9.9900e+02, -5.0574e-03, -4.6911e-03, -8.5026e-03},
				{-1.4657e-03, -1.3201e-03, -4.6554e-03, -4.6902e-03, 9.9900e+02, -4.0183e-03, -5.9250e-03, -7.4048e-03}};

				static double ff_u_rphi_13_chisq[8][8] = {{1.1311e-03, -7.7083e-05, 4.2625e-04, 8.3683e-04, 9.9900e+02, 1.0141e-03, 2.8328e-03, 2.5349e-04},
				{1.7681e-03, 1.2484e-04, 3.1294e-03, -7.3923e-04, 9.9900e+02, 3.2944e-04, 7.4662e-06, -1.7404e-03},
				{2.4004e-04, -1.6976e-03, 1.4587e-03, 1.3308e-03, 9.9900e+02, 3.9073e-05, 1.9957e-03, -8.7080e-04},
				{1.7262e-03, 1.9124e-03, 1.9428e-03, -7.3749e-04, 9.9900e+02, 7.1067e-04, 1.8341e-03, 5.7764e-05},
				{1.8217e-03, -9.8559e-04, 3.2472e-03, 3.3672e-03, 9.9900e+02, 2.2718e-03, 6.4608e-04, 2.8238e-04},
				{1.1034e-03, 2.3811e-03, 5.8050e-04, -2.6052e-04, 9.9900e+02, 1.9090e-03, 2.2655e-03, 1.1272e-03},
				{2.0433e-03, -8.4666e-04, 2.9112e-04, -2.0601e-05, 9.9900e+02, 2.0325e-03, 3.7041e-03, 1.3319e-03},
				{1.4815e-03, 1.0776e-03, 1.1663e-03, 2.8013e-04, 9.9900e+02, 2.2258e-03, 2.2263e-03, 2.4113e-03}};

				static double ff_u_weta2_13_chisq[8][8] = {{8.4698e-05, 1.9027e-04, 3.9604e-05, 7.2759e-05, 9.9900e+02, 2.9237e-04, 3.6949e-04, 3.6653e-04},
				{8.7896e-05, 1.6868e-04, -3.2078e-05, 2.1962e-05, 9.9900e+02, 3.2670e-04, 4.4581e-04, 4.3723e-04},
				{1.3778e-04, 2.1452e-04, 2.0736e-05, 9.7064e-05, 9.9900e+02, 2.6125e-04, 4.9490e-04, 4.5820e-04},
				{7.7438e-05, 8.3151e-05, 6.6244e-05, 7.1464e-05, 9.9900e+02, 3.1639e-04, 3.6964e-04, 4.0318e-04},
				{9.0688e-05, 1.2086e-04, 8.4649e-06, -2.7764e-05, 9.9900e+02, 3.3712e-04, 3.9991e-04, 4.7367e-04},
				{4.3001e-05, 1.0025e-04, 1.1218e-04, 6.7416e-05, 9.9900e+02, 2.4257e-04, 3.4528e-04, 3.4078e-04},
				{5.9202e-05, 2.0980e-04, 4.7670e-05, 1.0347e-04, 9.9900e+02, 2.9283e-04, 3.2162e-04, 3.7067e-04},
				{1.1134e-04, 1.4639e-04, 6.5511e-05, 8.7868e-05, 9.9900e+02, 3.6151e-04, 3.5511e-04, 4.0436e-04}};

				static double ff_u_f1_13_chisq[8][8] = {{-8.9122e-03, -5.3596e-03, -5.2743e-03, -1.6517e-02, 9.9900e+02, -2.9597e-03, 4.5608e-03, 1.9649e-02},
				{-9.0009e-03, -8.0512e-03, -1.1202e-02, -1.0602e-02, 9.9900e+02, 1.0193e-02, 2.9269e-02, 3.6467e-02},
				{3.4521e-03, 1.6599e-04, -8.4051e-03, -6.7223e-03, 9.9900e+02, 3.2301e-04, 4.0024e-02, 3.3059e-02},
				{-2.9139e-03, -1.0400e-02, -1.1384e-02, -4.8835e-03, 9.9900e+02, 1.1299e-02, 1.9191e-02, 2.4319e-02},
				{-2.9656e-03, -2.5251e-02, -1.0326e-02, -1.4664e-02, 9.9900e+02, 1.1736e-02, 1.2092e-02, 3.1040e-02},
				{-6.9407e-03, -2.2302e-02, 8.1371e-03, -1.6652e-03, 9.9900e+02, -9.3642e-03, 2.7390e-02, 1.0582e-02},
				{-6.4145e-04, 9.4871e-03, -2.7561e-03, -5.0438e-03, 9.9900e+02, -1.2399e-02, 5.0008e-03, -1.4927e-03},
				{7.8450e-03, 1.1615e-03, 2.0691e-03, -1.8959e-03, 9.9900e+02, 2.3815e-03, -2.7429e-03, 1.2173e-02}};

				static double ff_u_deltae_13_chisq[8][8] = {{-1.1407e+00, -7.9947e-01, -2.0688e+00, -1.9583e+00, 9.9900e+02, -3.7720e-01, -5.9737e+00, -1.5206e+00},
				{-1.9061e+00, -1.6403e+00, -3.7322e-01, -1.1389e+00, 9.9900e+02, 6.3953e-01, -6.5181e+00, -2.8492e+00},
				{-1.9201e+00, -3.0448e-01, -6.0594e-01, -1.2284e+00, 9.9900e+02, -3.4296e+00, -3.9941e+00, -3.4511e+00},
				{-2.1976e+00, -3.4249e+00, -2.7550e+00, -1.3832e+00, 9.9900e+02, 3.7409e+00, 1.7118e+00, -4.0127e+00},
				{-3.7257e+00, -3.7260e+00, -1.6399e+00, -3.8309e+00, 9.9900e+02, -2.4224e-01, -6.5572e+00, -4.3527e+00},
				{6.4874e-01, -4.0732e+00, -2.5799e+00, -1.1052e+00, 9.9900e+02, -2.8175e+00, -2.8497e+00, -3.5770e+00},
				{-5.2646e+00, -6.3620e+00, -2.2377e+00, 3.2036e+00, 9.9900e+02, -3.4689e+00, -8.9889e+00, -1.4763e+01},
				{-1.7333e+00, -2.8028e+00, -7.1915e-01, -2.0062e+00, 9.9900e+02, -6.5435e-01, -6.1186e+00, 1.2950e+00}};

				static double ff_u_wtot_13_chisq[8][8] = {{4.5488e-02, 3.7439e-02, 7.2249e-02, 6.6039e-02, 9.9900e+02, 1.0337e-01, 1.3827e-01, 1.1028e-01},
				{4.7712e-02, 9.7039e-02, 4.8492e-02, 1.2801e-01, 9.9900e+02, 1.4599e-01, 1.4766e-01, 1.2151e-01},
				{4.0132e-02, 8.8598e-02, 8.4873e-02, 1.0170e-01, 9.9900e+02, 1.4446e-01, 1.4275e-01, 1.1759e-01},
				{5.5395e-02, 3.6445e-02, 4.8685e-02, 1.0149e-01, 9.9900e+02, 1.2189e-01, 1.5325e-01, 1.1303e-01},
				{2.1717e-02, 9.1621e-02, 1.2292e-01, 6.5753e-02, 9.9900e+02, 1.4929e-01, 1.3323e-01, 1.0233e-01},
				{7.4139e-02, 9.5078e-02, 1.0437e-01, 1.1504e-01, 9.9900e+02, 1.3156e-01, 1.5061e-01, 9.5280e-02},
				{3.1941e-02, 3.1586e-02, 8.4813e-02, 5.3160e-02, 9.9900e+02, 7.2167e-02, 1.2435e-01, 1.0711e-01},
				{3.5546e-02, 4.9840e-02, 5.9855e-02, 7.1597e-02, 9.9900e+02, 9.8614e-02, 1.1882e-01, 1.2181e-01}};

				static double ff_u_fside_13_chisq[8][8] = {{1.0797e-02, 1.9870e-02, 2.1714e-02, 1.7835e-02, 9.9900e+02, 2.8017e-02, 3.4756e-02, 3.0471e-02},
				{8.6332e-03, 2.1034e-02, 1.0897e-02, 2.4483e-02, 9.9900e+02, 4.8820e-02, 4.4554e-02, 3.4935e-02},
				{1.1227e-02, 2.1409e-02, 1.8198e-02, 1.9249e-02, 9.9900e+02, 4.2267e-02, 4.5474e-02, 3.4143e-02},
				{8.1422e-03, 7.4497e-03, 7.9769e-03, 2.6913e-02, 9.9900e+02, 3.8267e-02, 3.6625e-02, 3.0924e-02},
				{8.1165e-03, 9.9958e-03, 1.7861e-02, 3.9550e-03, 9.9900e+02, 4.1228e-02, 3.5091e-02, 2.6584e-02},
				{9.5253e-03, 9.5097e-03, 2.0465e-02, 2.6698e-02, 9.9900e+02, 3.1090e-02, 3.2447e-02, 2.6906e-02},
				{1.0959e-02, 6.8787e-03, 2.0805e-02, 2.6140e-02, 9.9900e+02, 1.4781e-02, 3.5428e-02, 2.7665e-02},
				{8.6788e-03, 1.4990e-02, 1.6429e-02, 2.4872e-02, 9.9900e+02, 3.0058e-02, 3.0646e-02, 2.7270e-02}};

				static double ff_u_w1_13_chisq[8][8] = {{2.5990e-03, 1.2614e-02, 1.1512e-02, 1.3749e-02, 9.9900e+02, 1.7830e-02, 3.2197e-02, 3.4266e-02},
				{4.7485e-03, 3.0410e-03, 5.7242e-03, 1.3257e-02, 9.9900e+02, 3.1180e-02, 3.5392e-02, 3.7916e-02},
				{3.3648e-03, 1.2427e-02, 1.1604e-02, 8.4266e-03, 9.9900e+02, 1.9841e-02, 3.7984e-02, 3.8930e-02},
				{6.4143e-03, 7.5807e-03, 5.3705e-03, 1.5995e-02, 9.9900e+02, 1.8120e-02, 3.3885e-02, 3.3722e-02},
				{-2.4313e-03, 7.3405e-03, 8.9653e-03, 1.8904e-03, 9.9900e+02, 2.1091e-02, 3.5977e-02, 3.6148e-02},
				{6.3347e-04, 2.5699e-03, 2.2135e-02, 1.1281e-02, 9.9900e+02, 1.1653e-02, 2.8840e-02, 3.3954e-02},
				{6.7298e-03, 7.3885e-03, 1.6524e-02, 1.0726e-02, 9.9900e+02, 1.4829e-02, 2.1073e-02, 2.4428e-02},
				{3.3309e-03, 6.5044e-03, 1.6797e-02, 1.8074e-02, 9.9900e+02, 1.9716e-02, 2.6949e-02, 3.2290e-02}};

				static double ff_u_eratio_13_chisq[8][8] = {{4.9523e-04, -1.7214e-04, -4.2728e-03, -6.9553e-04, 9.9900e+02, -6.8761e-04, -1.1396e-03, -5.8250e-04},
				{9.2714e-04, -2.5098e-03, -1.0157e-03, -4.6216e-03, 9.9900e+02, 1.4718e-03, -4.2560e-05, -2.9823e-04},
				{9.9069e-04, 2.1753e-03, 8.5787e-05, -2.1942e-03, 9.9900e+02, -4.1481e-03, 6.8523e-04, -2.3799e-04},
				{4.2806e-04, 2.8251e-03, 1.9713e-03, 3.0341e-04, 9.9900e+02, 7.1464e-04, 1.1171e-03, 3.5347e-05},
				{2.1528e-03, 2.8003e-04, -1.5491e-04, 6.3937e-04, 9.9900e+02, -2.0574e-03, 2.7635e-04, 6.8749e-04},
				{5.9973e-04, -7.9177e-04, -1.6580e-03, -8.8482e-04, 9.9900e+02, -4.0480e-03, 1.8146e-03, -8.1080e-04},
				{5.1685e-04, -1.2974e-03, -1.0418e-04, -4.5492e-03, 9.9900e+02, -1.4591e-03, 1.1789e-03, -1.3376e-03},
				{7.8051e-04, -1.2225e-03, -5.8242e-04, -2.5386e-03, 9.9900e+02, -1.2696e-03, -8.9343e-04, -8.9228e-04}};

				static double ff_u_e277_13_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				// converted photons

				static double ff_c_rhad_13_chisq[8][8] = {{3.3341e-05, -3.0553e-04, 5.5422e-04, 3.1986e-04, 9.9900e+02, -9.5792e-04, 6.4558e-04, 8.2337e-04},
				{1.6543e-04, 5.1937e-04, 2.4147e-04, 1.2240e-04, 9.9900e+02, -3.2445e-04, -1.2240e-03, 3.4254e-04},
				{2.3066e-04, -4.2080e-04, 3.4095e-04, 5.1126e-04, 9.9900e+02, -2.4500e-04, 5.5890e-04, -3.5460e-04},
				{3.5255e-04, -2.5022e-04, 5.0386e-04, 1.7339e-04, 9.9900e+02, 1.6672e-04, 7.1660e-05, -1.5879e-04},
				{1.4805e-04, 1.9795e-04, 2.0461e-04, 1.5007e-04, 9.9900e+02, -6.2022e-04, -6.3778e-04, 7.7669e-04},
				{-4.7835e-04, -1.2001e-04, 8.6923e-05, -3.2849e-05, 9.9900e+02, -9.2565e-04, -5.6323e-04, 8.7802e-05},
				{1.1058e-04, -8.7761e-05, -5.9420e-05, -1.4878e-04, 9.9900e+02, -4.8233e-04, 4.3380e-04, 4.4944e-04},
				{-1.1901e-04, -1.2049e-04, 6.6849e-05, 8.6441e-05, 9.9900e+02, 6.4852e-05, -3.7747e-04, 2.3076e-04}};

				static double ff_c_reta_13_chisq[8][8] = {{-8.1020e-04, 2.5986e-03, -4.2101e-03, -4.1850e-03, 9.9900e+02, -8.2713e-03, -7.9236e-03, -7.3393e-03},
				{-9.7443e-04, -3.5272e-03, -4.1332e-03, -6.1710e-03, 9.9900e+02, -8.7399e-03, -9.0359e-03, -6.1074e-03},
				{-2.9649e-03, -4.7505e-03, -6.6156e-03, -2.4154e-03, 9.9900e+02, -7.5507e-03, -6.8442e-03, -9.6895e-03},
				{-1.2261e-03, -3.1469e-03, -3.6152e-03, -3.5292e-03, 9.9900e+02, -7.0445e-03, -8.9073e-03, -7.5369e-03},
				{-1.1055e-03, -1.5667e-03, -3.0150e-03, -4.4986e-03, 9.9900e+02, -8.3751e-03, -7.4900e-03, -6.4451e-03},
				{-3.1110e-03, -1.0169e-03, -4.3586e-03, -6.0053e-03, 9.9900e+02, -5.2645e-03, -1.0633e-02, -7.1315e-03},
				{-1.4317e-03, -3.6611e-03, -4.0612e-03, -2.8121e-03, 9.9900e+02, -8.0926e-03, -7.2150e-03, -9.2070e-03},
				{-1.8015e-03, -2.7229e-03, -3.5256e-03, -5.7367e-03, 9.9900e+02, -6.2865e-03, -7.4624e-03, -8.2600e-03}};

				static double ff_c_rphi_13_chisq[8][8] = {{1.3512e-03, 7.2337e-03, 4.4988e-03, -1.0158e-03, 9.9900e+02, -9.0173e-03, -9.5853e-03, -1.0174e-02},
				{2.5578e-03, -2.9725e-03, -7.1330e-03, -4.6912e-03, 9.9900e+02, -1.5870e-02, -1.5423e-02, -6.2501e-03},
				{5.1222e-03, -6.7899e-03, -9.6083e-03, -6.8789e-03, 9.9900e+02, -7.6246e-03, -8.5225e-03, -3.3430e-03},
				{5.6426e-04, 3.1234e-03, -2.9768e-03, -9.0213e-03, 9.9900e+02, -6.5518e-03, -7.8568e-03, -2.1857e-03},
				{3.4804e-03, 1.0359e-02, -7.2100e-03, -1.0522e-02, 9.9900e+02, -6.5602e-03, -3.9935e-03, -1.2716e-03},
				{-3.0872e-03, 1.3300e-02, 3.3127e-03, -4.5525e-03, 9.9900e+02, 1.1873e-03, -6.4233e-03, 1.1339e-04},
				{-8.1467e-05, -3.7194e-03, -3.1321e-03, 4.8459e-03, 9.9900e+02, -1.4677e-03, -7.9735e-04, -9.9449e-04},
				{9.6072e-04, -5.3306e-03, 3.6266e-04, -3.6750e-03, 9.9900e+02, -2.4171e-03, 4.6987e-04, 4.0519e-04}};

				static double ff_c_weta2_13_chisq[8][8] = {{9.7283e-05, 4.3212e-05, 1.3725e-04, 7.6809e-05, 9.9900e+02, 3.8064e-04, 4.2782e-04, 3.2162e-04},
				{6.3029e-05, 3.3678e-04, 6.3242e-05, 6.5245e-05, 9.9900e+02, 4.8068e-04, 4.0485e-04, 3.7033e-04},
				{4.3161e-06, 4.3269e-04, 1.4110e-04, 5.0204e-05, 9.9900e+02, 4.6206e-04, 4.1935e-04, 3.4480e-04},
				{8.8572e-05, 4.2241e-05, 9.5031e-05, 1.0318e-04, 9.9900e+02, 2.9425e-04, 4.9240e-04, 3.6238e-04},
				{9.3821e-05, 3.6468e-06, 7.5781e-05, 2.0228e-04, 9.9900e+02, 3.4245e-04, 4.1920e-04, 3.8226e-04},
				{1.0899e-04, 2.0031e-05, 9.4580e-05, 1.1328e-04, 9.9900e+02, 2.1848e-04, 6.2857e-04, 4.5616e-04},
				{2.8023e-05, 1.0829e-04, 1.1130e-05, 1.3881e-04, 9.9900e+02, 4.2157e-04, 5.0993e-04, 4.7367e-04},
				{1.5017e-04, 2.6929e-04, 7.8321e-05, 1.7255e-04, 9.9900e+02, 3.9260e-04, 3.5273e-04, 3.6014e-04}};

				static double ff_c_f1_13_chisq[8][8] = {{-1.1950e-02, -7.5126e-03, -7.2828e-03, -8.7472e-03, 9.9900e+02, 8.0073e-03, 2.1520e-02, 1.1624e-02},
				{-3.0100e-03, -1.9929e-02, -1.3281e-02, -1.3956e-02, 9.9900e+02, 1.8322e-02, 3.1915e-02, 1.3138e-02},
				{-4.7821e-04, 1.4255e-02, 2.7751e-03, -8.0370e-03, 9.9900e+02, 1.0152e-02, 2.8377e-02, 2.1539e-02},
				{1.0828e-02, -1.1588e-02, -1.0387e-03, 9.7218e-04, 9.9900e+02, 1.3367e-02, 4.1650e-02, 2.6894e-02},
				{-1.0227e-02, -9.9625e-03, -3.6428e-03, -3.0334e-03, 9.9900e+02, 1.7048e-02, 2.8168e-02, 1.8620e-02},
				{1.6698e-02, -1.6475e-02, -4.8202e-03, 6.3737e-03, 9.9900e+02, -5.9545e-03, 5.4018e-02, 3.5874e-02},
				{-9.7933e-03, 2.1128e-02, -9.6423e-03, 1.1791e-02, 9.9900e+02, 3.8859e-02, 4.3371e-02, 4.7323e-02},
				{9.6247e-03, 6.2223e-03, -1.4043e-03, 1.4040e-02, 9.9900e+02, 1.9501e-02, 2.4870e-02, 3.3989e-02}};

				static double ff_c_deltae_13_chisq[8][8] = {{-9.7800e-01, 1.3608e+00, -1.3995e+00, -2.4092e+00, 9.9900e+02, -1.6462e+00, -6.1124e-01, -3.6097e+00},
				{-2.5687e+00, 1.6964e+00, -3.5353e+00, -6.9182e+00, 9.9900e+02, -1.8011e-01, -4.6060e+00, -5.7783e+00},
				{5.3258e-01, -3.6059e+00, -3.1918e+00, -9.4238e-01, 9.9900e+02, -1.0304e+00, -8.3943e+00, -8.6958e-01},
				{-2.3616e+00, -1.1260e+00, -2.2456e+00, -2.0374e+00, 9.9900e+02, -1.2239e+00, -1.8936e+00, -3.4635e+00},
				{7.4372e-01, 6.5458e-01, 4.3160e-01, 2.7517e+00, 9.9900e+02, 4.2628e+00, -1.0362e+01, -2.3125e+00},
				{-4.8257e-01, -1.5749e+00, 2.1895e+00, -7.5641e+00, 9.9900e+02, -6.7464e+00, -1.5769e+00, -1.3896e+01},
				{-8.1779e+00, -7.0659e+00, -8.2505e+00, -7.4784e+00, 9.9900e+02, 8.0424e+00, 7.2637e+00, 3.6342e+00},
				{-5.4710e-01, -2.5801e+00, -1.4580e+00, 5.4757e-01, 9.9900e+02, -3.3096e+00, -5.2500e+00, -2.0391e+00}};

				static double ff_c_wtot_13_chisq[8][8] = {{8.7191e-02, 4.6689e-02, 7.0103e-02, 1.0030e-01, 9.9900e+02, 1.1940e-01, 1.6783e-01, 8.4663e-02},
				{1.0601e-01, 1.3739e-01, 1.4417e-01, 4.5345e-02, 9.9900e+02, 1.4740e-01, 1.6531e-01, 9.3568e-02},
				{5.1282e-02, 6.7545e-02, 1.3761e-01, 1.5132e-01, 9.9900e+02, 1.3188e-01, 1.7604e-01, 9.3445e-02},
				{5.4028e-02, 6.2370e-02, 8.9623e-02, 1.2493e-01, 9.9900e+02, 1.7724e-01, 1.7837e-01, 8.9176e-02},
				{4.6311e-02, 4.0295e-02, 6.7332e-02, 1.3411e-01, 9.9900e+02, 1.9768e-01, 1.5267e-01, 1.1214e-01},
				{2.9765e-02, 7.2698e-02, 8.7793e-02, 6.9847e-02, 9.9900e+02, 1.3161e-01, 1.9665e-01, 7.7253e-02},
				{3.3110e-02, 1.6058e-01, 1.6451e-01, 8.2435e-02, 9.9900e+02, 1.1051e-01, 1.6401e-01, 1.2103e-01},
				{3.1786e-02, 9.0760e-02, 1.1638e-01, 1.0301e-01, 9.9900e+02, 1.8959e-01, 1.5821e-01, 1.0695e-01}};

				static double ff_c_fside_13_chisq[8][8] = {{1.3490e-02, 1.4307e-02, 1.7987e-02, 2.5829e-02, 9.9900e+02, 3.9577e-02, 5.2060e-02, 2.5750e-02},
				{1.6031e-02, 2.9409e-02, 3.0900e-02, 2.4165e-02, 9.9900e+02, 4.8805e-02, 5.3370e-02, 3.2916e-02},
				{9.0735e-03, 2.8661e-02, 3.7211e-02, 3.8205e-02, 9.9900e+02, 4.5295e-02, 4.8963e-02, 3.3793e-02},
				{9.7227e-03, 1.7900e-02, 2.3741e-02, 3.0901e-02, 9.9900e+02, 4.8664e-02, 5.4381e-02, 2.6978e-02},
				{1.1132e-02, 1.9746e-02, 2.3904e-02, 3.4420e-02, 9.9900e+02, 4.5604e-02, 4.8879e-02, 3.2645e-02},
				{4.3665e-03, 2.3589e-02, 1.7952e-02, 2.8836e-02, 9.9900e+02, 3.7649e-02, 4.8658e-02, 2.8847e-02},
				{1.0782e-02, 3.2242e-02, 3.3434e-02, 3.2596e-02, 9.9900e+02, 4.2919e-02, 5.1070e-02, 3.2066e-02},
				{8.3953e-03, 1.8227e-02, 3.0009e-02, 2.8971e-02, 9.9900e+02, 4.9930e-02, 4.2944e-02, 3.3271e-02}};

				static double ff_c_w1_13_chisq[8][8] = {{-7.7765e-04, 1.1609e-02, 1.0302e-02, 1.3079e-02, 9.9900e+02, 1.6429e-02, 4.2885e-02, 3.0358e-02},
				{4.7997e-03, 1.0271e-02, 1.2260e-02, 1.2462e-02, 9.9900e+02, 3.1165e-02, 3.8165e-02, 2.9278e-02},
				{4.2765e-03, 1.7117e-02, 2.2293e-02, 8.0554e-03, 9.9900e+02, 2.4185e-02, 2.9909e-02, 3.0944e-02},
				{8.3317e-03, 2.3096e-02, 1.1776e-02, 1.3193e-02, 9.9900e+02, 1.8223e-02, 3.3180e-02, 3.3415e-02},
				{1.8556e-03, 7.0416e-03, 8.8598e-03, 1.3774e-02, 9.9900e+02, 1.5419e-02, 4.1595e-02, 3.2818e-02},
				{2.4921e-03, 2.7341e-03, 1.4525e-02, 1.1142e-02, 9.9900e+02, 1.5338e-02, 3.5535e-02, 3.1247e-02},
				{1.1042e-02, 8.2100e-03, 1.9268e-02, 1.7398e-02, 9.9900e+02, 2.6521e-02, 3.6148e-02, 3.1840e-02},
				{4.3207e-03, 1.1626e-02, 1.5962e-02, 1.7926e-02, 9.9900e+02, 2.4510e-02, 3.0345e-02, 2.7455e-02}};

				static double ff_c_eratio_13_chisq[8][8] = {{-9.0230e-04, 5.3155e-03, 1.5851e-03, -7.1173e-04, 9.9900e+02, -7.6192e-05, -6.9680e-04, -6.4629e-06},
				{-1.5238e-03, -4.9327e-03, -2.2339e-05, 6.3697e-03, 9.9900e+02, 3.9133e-04, 1.7173e-03, -1.5094e-03},
				{-1.0519e-03, 5.4288e-03, -2.0924e-03, -9.1477e-04, 9.9900e+02, -2.6472e-03, 1.1307e-03, 4.7738e-04},
				{8.2988e-04, 8.5147e-05, 1.5833e-03, -1.7448e-03, 9.9900e+02, -2.2600e-03, 1.2898e-04, -4.5742e-04},
				{2.0247e-03, -1.2871e-03, 7.5440e-04, -8.1989e-03, 9.9900e+02, -2.0739e-03, -2.3722e-03, -1.0536e-03},
				{2.7559e-03, -2.1581e-04, -1.7598e-03, 3.2194e-03, 9.9900e+02, -1.0557e-03, 2.2018e-03, 2.3360e-03},
				{-2.5946e-04, 5.0758e-03, 9.6830e-04, -5.0597e-03, 9.9900e+02, -5.2537e-04, -7.9262e-04, -3.0963e-03},
				{4.0394e-04, -1.1032e-03, -3.2967e-03, -4.6166e-03, 9.9900e+02, -2.6534e-03, -8.5956e-04, 1.3425e-03}};

				static double ff_c_e277_13_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};


				static double ff_u_rhad_err_13_chisq[8][8] = {{3.7385e-03, 3.6895e-03, 4.5539e-03, 4.1765e-03, 0.0000e+00, 1.7177e-02, 1.5006e-02, 1.1865e-02},
				{3.0516e-03, 2.7917e-03, 3.5678e-03, 3.4327e-03, 0.0000e+00, 1.4156e-02, 1.3134e-02, 9.9894e-03},
				{2.5464e-03, 2.4324e-03, 2.9550e-03, 2.7407e-03, 0.0000e+00, 1.0601e-02, 9.4540e-03, 7.3559e-03},
				{2.2407e-03, 1.9066e-03, 2.5312e-03, 2.2187e-03, 0.0000e+00, 8.7079e-03, 7.7139e-03, 5.7994e-03},
				{1.9299e-03, 1.6513e-03, 1.9176e-03, 1.9181e-03, 0.0000e+00, 7.8220e-03, 7.4110e-03, 4.5956e-03},
				{1.8736e-03, 1.3729e-03, 1.7916e-03, 1.4845e-03, 0.0000e+00, 6.1016e-03, 5.1710e-03, 3.9541e-03},
				{1.7045e-03, 1.4359e-03, 1.3944e-03, 1.3371e-03, 0.0000e+00, 4.0467e-03, 3.6807e-03, 3.1101e-03},
				{1.7355e-03, 9.4198e-04, 1.0167e-03, 9.5081e-04, 0.0000e+00, 3.5937e-03, 2.9587e-03, 1.9408e-03}};

				static double ff_u_reta_err_13_chisq[8][8] = {{2.0622e-02, 2.2264e-02, 1.8815e-02, 1.8225e-02, 0.0000e+00, 1.7750e-02, 1.4113e-02, 1.5725e-02},
				{1.8325e-02, 1.8246e-02, 1.5507e-02, 1.5889e-02, 0.0000e+00, 1.5018e-02, 1.2842e-02, 1.5186e-02},
				{1.5845e-02, 1.6513e-02, 1.4160e-02, 1.3694e-02, 0.0000e+00, 1.4490e-02, 1.1130e-02, 1.3808e-02},
				{1.2356e-02, 1.3625e-02, 1.2414e-02, 1.1743e-02, 0.0000e+00, 1.2195e-02, 1.0178e-02, 1.2241e-02},
				{1.1452e-02, 1.1173e-02, 9.7379e-03, 9.3330e-03, 0.0000e+00, 1.1185e-02, 9.6767e-03, 1.1111e-02},
				{9.4058e-03, 1.1962e-02, 8.2832e-03, 8.2779e-03, 0.0000e+00, 1.0924e-02, 1.0393e-02, 1.1026e-02},
				{8.5158e-03, 8.1998e-03, 7.8864e-03, 7.7560e-03, 0.0000e+00, 1.0743e-02, 7.7244e-03, 1.0404e-02},
				{6.7801e-03, 7.2749e-03, 7.1841e-03, 7.2980e-03, 0.0000e+00, 9.4005e-03, 7.2978e-03, 9.8129e-03}};

				static double ff_u_rphi_err_13_chisq[8][8] = {{1.6514e-02, 1.7880e-02, 1.6472e-02, 2.0531e-02, 0.0000e+00, 1.6662e-02, 1.2895e-02, 1.5284e-02},
				{1.3546e-02, 1.6470e-02, 1.5735e-02, 1.9885e-02, 0.0000e+00, 1.5296e-02, 1.0843e-02, 1.4401e-02},
				{1.1813e-02, 1.2618e-02, 1.3937e-02, 1.9204e-02, 0.0000e+00, 1.4253e-02, 1.0921e-02, 1.3094e-02},
				{1.0347e-02, 1.3905e-02, 1.1735e-02, 1.5076e-02, 0.0000e+00, 1.3265e-02, 9.2738e-03, 1.1376e-02},
				{9.3896e-03, 1.1078e-02, 1.1010e-02, 1.4628e-02, 0.0000e+00, 1.1771e-02, 8.5983e-03, 1.0289e-02},
				{8.4741e-03, 1.0126e-02, 9.6591e-03, 1.1519e-02, 0.0000e+00, 1.0697e-02, 9.0441e-03, 1.0702e-02},
				{7.6042e-03, 8.6560e-03, 8.3952e-03, 9.1155e-03, 0.0000e+00, 1.1334e-02, 9.0045e-03, 8.5533e-03},
				{6.5466e-03, 7.1951e-03, 7.7346e-03, 9.3907e-03, 0.0000e+00, 9.2690e-03, 7.4500e-03, 8.7488e-03}};

				static double ff_u_weta2_err_13_chisq[8][8] = {{7.9606e-04, 7.9213e-04, 6.6332e-04, 7.0982e-04, 0.0000e+00, 7.9704e-04, 6.8539e-04, 6.2371e-04},
				{6.8116e-04, 7.5666e-04, 6.4845e-04, 6.8412e-04, 0.0000e+00, 7.4815e-04, 6.3390e-04, 6.2290e-04},
				{6.4441e-04, 7.1050e-04, 6.4059e-04, 6.4037e-04, 0.0000e+00, 7.1590e-04, 6.3394e-04, 5.9863e-04},
				{5.6153e-04, 6.8913e-04, 5.6880e-04, 5.4643e-04, 0.0000e+00, 6.3660e-04, 5.5350e-04, 5.6619e-04},
				{5.2500e-04, 6.4888e-04, 5.5282e-04, 5.2387e-04, 0.0000e+00, 5.9220e-04, 5.8200e-04, 5.2436e-04},
				{4.9969e-04, 6.0180e-04, 4.8235e-04, 5.0853e-04, 0.0000e+00, 6.0123e-04, 5.1180e-04, 5.4835e-04},
				{4.3598e-04, 5.0819e-04, 4.6462e-04, 4.5111e-04, 0.0000e+00, 6.0023e-04, 5.9329e-04, 5.7706e-04},
				{3.9086e-04, 4.5808e-04, 3.8627e-04, 4.3898e-04, 0.0000e+00, 5.3515e-04, 5.0649e-04, 4.9645e-04}};

				static double ff_u_f1_err_13_chisq[8][8] = {{1.4054e-01, 1.3335e-01, 1.0249e-01, 8.3076e-02, 0.0000e+00, 1.1541e-01, 1.2300e-01, 1.4132e-01},
				{1.3395e-01, 1.3477e-01, 9.6733e-02, 7.6302e-02, 0.0000e+00, 1.2471e-01, 1.2345e-01, 1.3661e-01},
				{1.4393e-01, 1.4203e-01, 9.8287e-02, 7.9200e-02, 0.0000e+00, 1.1041e-01, 1.2551e-01, 1.3014e-01},
				{1.3025e-01, 1.2602e-01, 9.8051e-02, 9.0720e-02, 0.0000e+00, 1.1443e-01, 1.1350e-01, 1.2946e-01},
				{1.3307e-01, 1.3269e-01, 9.5253e-02, 8.0991e-02, 0.0000e+00, 1.1639e-01, 1.0838e-01, 1.3364e-01},
				{1.2760e-01, 1.2704e-01, 1.0383e-01, 8.0481e-02, 0.0000e+00, 1.1935e-01, 1.0784e-01, 1.2107e-01},
				{1.1739e-01, 1.2917e-01, 9.4081e-02, 8.8761e-02, 0.0000e+00, 1.3120e-01, 9.3219e-02, 1.1851e-01},
				{1.0466e-01, 1.2401e-01, 8.5757e-02, 7.9420e-02, 0.0000e+00, 1.3336e-01, 8.9654e-02, 9.7107e-02}};

				static double ff_u_deltae_err_13_chisq[8][8] = {{2.5517e+01, 2.4427e+01, 2.1778e+01, 2.1668e+01, 0.0000e+00, 3.3504e+01, 3.8002e+01, 3.4770e+01},
				{2.4859e+01, 2.3889e+01, 2.0454e+01, 2.1869e+01, 0.0000e+00, 3.2943e+01, 4.0100e+01, 3.2861e+01},
				{2.4478e+01, 2.2358e+01, 2.0561e+01, 2.0436e+01, 0.0000e+00, 3.1709e+01, 3.2913e+01, 3.4170e+01},
				{2.3522e+01, 2.3454e+01, 2.1618e+01, 2.0013e+01, 0.0000e+00, 2.9164e+01, 3.4334e+01, 3.3380e+01},
				{2.4293e+01, 2.3649e+01, 2.0447e+01, 2.1575e+01, 0.0000e+00, 3.1380e+01, 4.0950e+01, 3.3952e+01},
				{2.2600e+01, 2.3935e+01, 2.0289e+01, 2.1368e+01, 0.0000e+00, 3.0378e+01, 2.8925e+01, 3.0098e+01},
				{2.3905e+01, 2.3958e+01, 2.0391e+01, 1.8886e+01, 0.0000e+00, 2.9851e+01, 4.3228e+01, 3.5139e+01},
				{2.2170e+01, 2.3497e+01, 1.9163e+01, 1.9713e+01, 0.0000e+00, 2.8569e+01, 3.0287e+01, 2.6108e+01}};

				static double ff_u_wtot_err_13_chisq[8][8] = {{3.8970e-01, 3.7427e-01, 4.4295e-01, 4.0219e-01, 0.0000e+00, 4.3063e-01, 2.1316e-01, 1.6938e-01},
				{3.4330e-01, 3.4503e-01, 4.1085e-01, 3.7275e-01, 0.0000e+00, 4.2403e-01, 2.2100e-01, 1.5163e-01},
				{3.3140e-01, 3.1827e-01, 3.5866e-01, 3.7802e-01, 0.0000e+00, 3.9664e-01, 1.7359e-01, 1.6121e-01},
				{2.7663e-01, 3.0324e-01, 3.4851e-01, 3.3800e-01, 0.0000e+00, 3.5448e-01, 2.0760e-01, 1.4083e-01},
				{2.8012e-01, 2.7883e-01, 3.5362e-01, 2.9769e-01, 0.0000e+00, 3.2570e-01, 1.5307e-01, 1.4519e-01},
				{2.6340e-01, 2.6110e-01, 3.1353e-01, 3.1543e-01, 0.0000e+00, 3.4536e-01, 1.3966e-01, 1.3064e-01},
				{2.3303e-01, 2.2359e-01, 3.1045e-01, 3.2389e-01, 0.0000e+00, 3.6668e-01, 1.3464e-01, 1.3050e-01},
				{1.9499e-01, 2.3594e-01, 2.8246e-01, 3.1432e-01, 0.0000e+00, 2.6039e-01, 1.3441e-01, 1.0989e-01}};

				static double ff_u_fside_err_13_chisq[8][8] = {{6.6069e-02, 7.8993e-02, 9.2308e-02, 1.0421e-01, 0.0000e+00, 9.9862e-02, 4.7904e-02, 4.5405e-02},
				{6.1633e-02, 8.0671e-02, 9.3685e-02, 1.0681e-01, 0.0000e+00, 9.6545e-02, 4.9111e-02, 4.3026e-02},
				{6.2653e-02, 7.3312e-02, 8.7882e-02, 9.4589e-02, 0.0000e+00, 9.7154e-02, 4.2435e-02, 4.1741e-02},
				{5.6007e-02, 7.3420e-02, 8.4660e-02, 9.1803e-02, 0.0000e+00, 8.8656e-02, 4.2067e-02, 3.7926e-02},
				{5.3480e-02, 6.8379e-02, 8.0238e-02, 8.6986e-02, 0.0000e+00, 7.7852e-02, 3.6379e-02, 3.8608e-02},
				{4.6142e-02, 5.7174e-02, 7.3206e-02, 7.7858e-02, 0.0000e+00, 8.1813e-02, 3.5920e-02, 3.6685e-02},
				{4.6893e-02, 5.5697e-02, 6.1346e-02, 7.0716e-02, 0.0000e+00, 8.5577e-02, 3.8728e-02, 3.0567e-02},
				{3.9342e-02, 4.8810e-02, 5.8998e-02, 6.0222e-02, 0.0000e+00, 6.1483e-02, 2.8390e-02, 2.7244e-02}};

				static double ff_u_w1_err_13_chisq[8][8] = {{5.4786e-02, 6.2017e-02, 6.6298e-02, 5.2627e-02, 0.0000e+00, 5.5947e-02, 5.5516e-02, 4.5513e-02},
				{5.3943e-02, 7.2743e-02, 6.0054e-02, 4.5693e-02, 0.0000e+00, 5.7114e-02, 5.2329e-02, 4.8049e-02},
				{5.4218e-02, 5.6790e-02, 6.5707e-02, 4.5211e-02, 0.0000e+00, 5.4381e-02, 4.9286e-02, 4.4986e-02},
				{4.9363e-02, 5.7883e-02, 6.0641e-02, 4.5444e-02, 0.0000e+00, 5.5601e-02, 4.9374e-02, 4.6168e-02},
				{5.1868e-02, 5.2004e-02, 6.4188e-02, 4.1557e-02, 0.0000e+00, 5.2352e-02, 4.3313e-02, 4.3489e-02},
				{5.0955e-02, 4.8710e-02, 5.8132e-02, 4.3508e-02, 0.0000e+00, 5.2819e-02, 5.3278e-02, 4.1974e-02},
				{4.3721e-02, 5.4519e-02, 5.6503e-02, 4.4092e-02, 0.0000e+00, 5.8709e-02, 5.2124e-02, 3.5065e-02},
				{3.8453e-02, 4.5602e-02, 5.3932e-02, 4.6197e-02, 0.0000e+00, 5.3990e-02, 4.2495e-02, 3.2878e-02}};

				static double ff_u_eratio_err_13_chisq[8][8] = {{2.4821e-02, 2.4733e-02, 3.4829e-02, 3.4257e-02, 0.0000e+00, 2.1224e-02, 1.8316e-02, 1.6251e-02},
				{2.2873e-02, 2.2391e-02, 2.9825e-02, 2.9774e-02, 0.0000e+00, 1.9839e-02, 1.7186e-02, 1.5773e-02},
				{2.0477e-02, 2.3017e-02, 2.7467e-02, 2.9854e-02, 0.0000e+00, 1.6218e-02, 1.4355e-02, 1.4084e-02},
				{1.8426e-02, 1.9220e-02, 2.8342e-02, 2.8016e-02, 0.0000e+00, 1.5580e-02, 1.5941e-02, 1.3922e-02},
				{1.7699e-02, 1.7949e-02, 2.1176e-02, 2.1773e-02, 0.0000e+00, 1.3733e-02, 1.4272e-02, 1.3009e-02},
				{1.5149e-02, 1.5753e-02, 2.1490e-02, 2.5450e-02, 0.0000e+00, 1.0180e-02, 1.2324e-02, 1.1579e-02},
				{1.5048e-02, 1.4604e-02, 2.0995e-02, 2.0201e-02, 0.0000e+00, 1.1187e-02, 1.0916e-02, 1.0446e-02},
				{1.3000e-02, 1.3570e-02, 1.9183e-02, 1.9898e-02, 0.0000e+00, 9.5449e-03, 9.5374e-03, 1.0527e-02}};


				static double ff_u_e277_err_13_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				static double ff_c_rhad_err_13_chisq[8][8] = {{3.4646e-03, 3.7180e-03, 4.1151e-03, 3.9523e-03, 0.0000e+00, 1.7056e-02, 1.5122e-02, 1.1940e-02},
				{3.0422e-03, 3.0225e-03, 3.4036e-03, 3.4772e-03, 0.0000e+00, 1.4053e-02, 1.2928e-02, 9.3284e-03},
				{2.2721e-03, 2.2585e-03, 2.7729e-03, 3.0396e-03, 0.0000e+00, 1.0860e-02, 1.0268e-02, 7.0976e-03},
				{2.1256e-03, 1.8995e-03, 2.2471e-03, 2.4520e-03, 0.0000e+00, 8.5246e-03, 7.6253e-03, 5.7101e-03},
				{1.7118e-03, 1.7823e-03, 1.8842e-03, 1.7931e-03, 0.0000e+00, 7.8249e-03, 6.4213e-03, 5.0771e-03},
				{1.7829e-03, 1.3759e-03, 1.7694e-03, 1.8404e-03, 0.0000e+00, 5.2113e-03, 4.8444e-03, 3.5760e-03},
				{1.3897e-03, 9.6755e-04, 1.5397e-03, 1.2965e-03, 0.0000e+00, 4.6093e-03, 3.4053e-03, 2.8186e-03},
				{1.3663e-03, 9.9554e-04, 1.0795e-03, 8.9763e-04, 0.0000e+00, 3.1287e-03, 2.5559e-03, 2.0744e-03}};

				static double ff_c_reta_err_13_chisq[8][8] = {{2.8368e-02, 2.5572e-02, 2.2617e-02, 2.3021e-02, 0.0000e+00, 2.0129e-02, 1.4976e-02, 1.6283e-02},
				{2.1787e-02, 2.2465e-02, 1.8792e-02, 1.8539e-02, 0.0000e+00, 1.7782e-02, 1.3393e-02, 1.5628e-02},
				{1.9672e-02, 1.9613e-02, 1.5059e-02, 1.8169e-02, 0.0000e+00, 1.5902e-02, 1.1166e-02, 1.3649e-02},
				{1.4144e-02, 1.6553e-02, 1.4607e-02, 1.5253e-02, 0.0000e+00, 1.4132e-02, 1.0747e-02, 1.3509e-02},
				{1.2515e-02, 1.3141e-02, 1.2598e-02, 1.2967e-02, 0.0000e+00, 1.1878e-02, 9.6071e-03, 1.1676e-02},
				{1.3610e-02, 1.2808e-02, 1.0905e-02, 1.3966e-02, 0.0000e+00, 1.2256e-02, 8.8629e-03, 1.1136e-02},
				{1.1141e-02, 1.1229e-02, 8.0749e-03, 1.0643e-02, 0.0000e+00, 1.1094e-02, 6.9468e-03, 1.1154e-02},
				{7.1230e-03, 7.9321e-03, 7.6281e-03, 7.8839e-03, 0.0000e+00, 9.6739e-03, 7.1441e-03, 8.9377e-03}};

				static double ff_c_rphi_err_13_chisq[8][8] = {{3.7235e-02, 4.2480e-02, 5.9948e-02, 6.4314e-02, 0.0000e+00, 4.2723e-02, 2.8764e-02, 4.5081e-02},
				{4.3167e-02, 4.5745e-02, 5.4299e-02, 6.3764e-02, 0.0000e+00, 4.3161e-02, 3.5069e-02, 3.4242e-02},
				{3.0158e-02, 3.8722e-02, 5.2222e-02, 6.2643e-02, 0.0000e+00, 4.2273e-02, 2.5630e-02, 2.6043e-02},
				{2.6274e-02, 5.3003e-02, 4.2549e-02, 4.8524e-02, 0.0000e+00, 3.2688e-02, 2.0819e-02, 2.3019e-02},
				{2.9414e-02, 5.6905e-02, 3.3065e-02, 3.6241e-02, 0.0000e+00, 2.4947e-02, 1.6581e-02, 1.9896e-02},
				{1.9764e-02, 3.3813e-02, 2.6755e-02, 3.1497e-02, 0.0000e+00, 2.4056e-02, 1.7639e-02, 1.6059e-02},
				{1.8256e-02, 1.7443e-02, 1.9065e-02, 2.2974e-02, 0.0000e+00, 1.8764e-02, 1.3156e-02, 1.2711e-02},
				{1.2922e-02, 1.3102e-02, 1.4475e-02, 1.5613e-02, 0.0000e+00, 1.4596e-02, 1.1716e-02, 1.1844e-02}};

				static double ff_c_weta2_err_13_chisq[8][8] = {{9.7441e-04, 1.0438e-03, 8.3699e-04, 9.4057e-04, 0.0000e+00, 9.0165e-04, 7.7529e-04, 6.5806e-04},
				{8.5471e-04, 8.4317e-04, 9.2099e-04, 8.2403e-04, 0.0000e+00, 8.5776e-04, 7.5688e-04, 6.3345e-04},
				{7.0932e-04, 8.7288e-04, 7.2887e-04, 9.2354e-04, 0.0000e+00, 8.2748e-04, 6.8065e-04, 5.8130e-04},
				{6.9885e-04, 8.0598e-04, 6.4466e-04, 7.3249e-04, 0.0000e+00, 7.5187e-04, 6.8148e-04, 5.8250e-04},
				{5.4902e-04, 7.3832e-04, 6.0730e-04, 6.3909e-04, 0.0000e+00, 7.1611e-04, 6.1820e-04, 5.5126e-04},
				{5.3305e-04, 6.7448e-04, 5.2392e-04, 6.6432e-04, 0.0000e+00, 6.6177e-04, 4.9521e-04, 5.3309e-04},
				{4.1621e-04, 6.7447e-04, 5.2428e-04, 4.7736e-04, 0.0000e+00, 6.2987e-04, 6.1011e-04, 5.6771e-04},
				{3.9610e-04, 4.6680e-04, 4.4797e-04, 4.6866e-04, 0.0000e+00, 6.0689e-04, 5.0323e-04, 4.5086e-04}};

				static double ff_c_f1_err_13_chisq[8][8] = {{1.1018e-01, 9.6743e-02, 6.8734e-02, 6.0650e-02, 0.0000e+00, 6.9548e-02, 8.8559e-02, 1.0268e-01},
				{9.6341e-02, 8.2330e-02, 6.7830e-02, 5.6451e-02, 0.0000e+00, 7.1369e-02, 8.9893e-02, 1.0961e-01},
				{9.7578e-02, 8.7117e-02, 6.4206e-02, 5.7817e-02, 0.0000e+00, 6.8828e-02, 8.6079e-02, 1.0261e-01},
				{8.8588e-02, 9.1055e-02, 6.8099e-02, 6.4022e-02, 0.0000e+00, 6.6975e-02, 8.2751e-02, 9.8255e-02},
				{9.1068e-02, 9.0890e-02, 7.1424e-02, 5.1407e-02, 0.0000e+00, 7.2328e-02, 7.7433e-02, 9.7608e-02},
				{9.3670e-02, 9.9274e-02, 7.2304e-02, 6.6594e-02, 0.0000e+00, 7.0628e-02, 8.1478e-02, 9.8609e-02},
				{8.2905e-02, 8.9851e-02, 6.7170e-02, 5.0818e-02, 0.0000e+00, 8.3192e-02, 7.0093e-02, 9.8468e-02},
				{9.0793e-02, 9.2333e-02, 6.6027e-02, 5.5905e-02, 0.0000e+00, 8.8838e-02, 7.9710e-02, 8.6989e-02}};

				static double ff_c_deltae_err_13_chisq[8][8] = {{2.6421e+01, 2.5431e+01, 2.4204e+01, 2.4680e+01, 0.0000e+00, 3.3990e+01, 3.3624e+01, 2.9873e+01},
				{2.7223e+01, 2.5721e+01, 2.5232e+01, 2.6870e+01, 0.0000e+00, 3.3621e+01, 3.5331e+01, 3.2525e+01},
				{2.2912e+01, 2.9963e+01, 2.3602e+01, 2.3542e+01, 0.0000e+00, 3.2391e+01, 3.4418e+01, 2.8702e+01},
				{2.4631e+01, 2.3368e+01, 2.5126e+01, 2.4721e+01, 0.0000e+00, 3.0964e+01, 3.4571e+01, 3.0109e+01},
				{2.3244e+01, 2.2568e+01, 2.1292e+01, 1.9709e+01, 0.0000e+00, 2.7061e+01, 3.3524e+01, 2.7855e+01},
				{2.4700e+01, 3.5244e+01, 2.0807e+01, 2.5846e+01, 0.0000e+00, 3.8080e+01, 2.9634e+01, 3.3932e+01},
				{2.5750e+01, 2.4876e+01, 2.6876e+01, 2.4902e+01, 0.0000e+00, 2.6630e+01, 2.8088e+01, 2.5530e+01},
				{2.3652e+01, 2.4314e+01, 2.0432e+01, 1.9042e+01, 0.0000e+00, 3.1180e+01, 3.2340e+01, 3.2919e+01}};

				static double ff_c_wtot_err_13_chisq[8][8] = {{3.3191e-01, 3.3714e-01, 3.7301e-01, 3.8571e-01, 0.0000e+00, 4.2551e-01, 1.9348e-01, 1.2958e-01},
				{2.8598e-01, 2.8391e-01, 3.5600e-01, 3.5965e-01, 0.0000e+00, 4.0189e-01, 1.8340e-01, 1.3444e-01},
				{2.8433e-01, 2.8843e-01, 3.8463e-01, 3.2357e-01, 0.0000e+00, 4.1939e-01, 1.8877e-01, 1.2479e-01},
				{2.4749e-01, 2.6791e-01, 3.4372e-01, 3.2088e-01, 0.0000e+00, 3.8325e-01, 1.5459e-01, 1.2609e-01},
				{2.1486e-01, 2.6993e-01, 3.1332e-01, 2.9936e-01, 0.0000e+00, 3.3824e-01, 1.6000e-01, 1.1770e-01},
				{2.0776e-01, 2.2512e-01, 3.1129e-01, 3.5064e-01, 0.0000e+00, 3.4171e-01, 1.7921e-01, 1.2079e-01},
				{2.0091e-01, 2.0451e-01, 2.7906e-01, 2.7652e-01, 0.0000e+00, 4.1620e-01, 1.2446e-01, 1.1770e-01},
				{1.7477e-01, 2.0996e-01, 2.7567e-01, 2.8382e-01, 0.0000e+00, 3.0228e-01, 1.3401e-01, 1.1352e-01}};

				static double ff_c_fside_err_13_chisq[8][8] = {{6.4763e-02, 8.2351e-02, 1.0717e-01, 1.1820e-01, 0.0000e+00, 1.1453e-01, 5.0421e-02, 4.2369e-02},
				{6.0560e-02, 7.5204e-02, 9.6724e-02, 1.1320e-01, 0.0000e+00, 1.1431e-01, 4.5505e-02, 3.7735e-02},
				{6.0866e-02, 6.9979e-02, 9.0914e-02, 9.8700e-02, 0.0000e+00, 1.1110e-01, 4.6238e-02, 3.5023e-02},
				{5.4883e-02, 5.9818e-02, 8.5427e-02, 9.5348e-02, 0.0000e+00, 1.0123e-01, 4.0040e-02, 3.8668e-02},
				{4.5443e-02, 6.1797e-02, 7.6749e-02, 9.7241e-02, 0.0000e+00, 9.0514e-02, 3.6474e-02, 3.4194e-02},
				{4.3439e-02, 5.5970e-02, 7.7482e-02, 8.1077e-02, 0.0000e+00, 1.0740e-01, 3.7137e-02, 2.9083e-02},
				{3.3313e-02, 5.0797e-02, 6.1483e-02, 7.7648e-02, 0.0000e+00, 9.1559e-02, 3.0665e-02, 2.9585e-02},
				{3.5577e-02, 4.6539e-02, 5.4436e-02, 5.9694e-02, 0.0000e+00, 6.9579e-02, 3.0895e-02, 2.5362e-02}};

				static double ff_c_w1_err_13_chisq[8][8] = {{5.6075e-02, 6.1721e-02, 5.5260e-02, 5.0712e-02, 0.0000e+00, 4.9731e-02, 5.6274e-02, 4.6515e-02},
				{5.3382e-02, 6.0798e-02, 5.7144e-02, 4.6822e-02, 0.0000e+00, 5.3447e-02, 5.8226e-02, 4.2018e-02},
				{4.6370e-02, 5.0979e-02, 4.9183e-02, 4.9745e-02, 0.0000e+00, 4.8465e-02, 5.1233e-02, 4.0367e-02},
				{4.3423e-02, 5.5023e-02, 5.4202e-02, 4.5107e-02, 0.0000e+00, 4.8636e-02, 4.8935e-02, 4.3376e-02},
				{4.1436e-02, 5.1872e-02, 5.0567e-02, 4.2673e-02, 0.0000e+00, 4.6741e-02, 4.9202e-02, 4.5524e-02},
				{4.5034e-02, 4.9453e-02, 4.9374e-02, 3.4135e-02, 0.0000e+00, 4.5982e-02, 5.1069e-02, 4.7409e-02},
				{3.2203e-02, 4.4254e-02, 4.6082e-02, 4.1405e-02, 0.0000e+00, 4.3133e-02, 4.1857e-02, 4.0333e-02},
				{3.4437e-02, 4.4686e-02, 4.0358e-02, 3.2948e-02, 0.0000e+00, 4.2497e-02, 4.8145e-02, 3.7829e-02}};

				static double ff_c_eratio_err_13_chisq[8][8] = {{2.4404e-02, 2.9394e-02, 4.2767e-02, 4.7484e-02, 0.0000e+00, 2.7323e-02, 1.6112e-02, 1.4082e-02},
				{2.3122e-02, 2.3828e-02, 3.7968e-02, 4.4343e-02, 0.0000e+00, 2.4970e-02, 1.6111e-02, 1.2576e-02},
				{1.9998e-02, 1.9879e-02, 3.6263e-02, 3.6156e-02, 0.0000e+00, 2.0857e-02, 1.5115e-02, 1.3162e-02},
				{1.8939e-02, 1.9835e-02, 3.0107e-02, 3.3672e-02, 0.0000e+00, 1.7242e-02, 1.1503e-02, 1.1875e-02},
				{1.6723e-02, 1.9719e-02, 2.8190e-02, 3.0246e-02, 0.0000e+00, 1.6343e-02, 1.0368e-02, 1.2701e-02},
				{1.6912e-02, 1.5368e-02, 2.6029e-02, 2.7170e-02, 0.0000e+00, 1.3971e-02, 1.0633e-02, 1.1676e-02},
				{1.3087e-02, 1.5398e-02, 1.9229e-02, 2.2306e-02, 0.0000e+00, 1.3092e-02, 8.1285e-03, 8.4297e-03},
				{1.2416e-02, 1.4300e-02, 1.8394e-02, 1.9106e-02, 0.0000e+00, 9.4379e-03, 7.6434e-03, 1.1270e-02}};

				static double ff_c_e277_err_13_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};



				if (isConv) {

				m_rhad_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_rhad_13_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_rhad_13_chisq[0][0], 0.);
				m_reta_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_reta_13_chisq[0][0], 0.);
				m_rphi_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_rphi_13_chisq[0][0], 0.);
				m_weta2_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_weta2_13_chisq[0][0], 0.);
				m_f1_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_f1_13_chisq[0][0], 0.);
				m_deltae_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_deltae_13_chisq[0][0], 0.);
				m_wtot_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_wtot_13_chisq[0][0], 0.);
				m_fside_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_fside_13_chisq[0][0], 0.);
				m_w1_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_w1_13_chisq[0][0], 0.);
				m_eratio_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_eratio_13_chisq[0][0], 0.);
				m_e277_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_e277_13_chisq[0][0], 0.);
				m_rhad_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_rhad_err_13_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_rhad_err_13_chisq[0][0], 0.);
				m_reta_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_reta_err_13_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_rphi_err_13_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_weta2_err_13_chisq[0][0], 0.);
				m_f1_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_f1_err_13_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_deltae_err_13_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_wtot_err_13_chisq[0][0], 0.);
				m_fside_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_fside_err_13_chisq[0][0], 0.);
				m_w1_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_w1_err_13_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_eratio_err_13_chisq[0][0], 0.);
				m_e277_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_c_e277_err_13_chisq[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_rhad_13_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_rhad_13_chisq[0][0], 0.);
				m_reta_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_reta_13_chisq[0][0], 0.);
				m_rphi_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_rphi_13_chisq[0][0], 0.);
				m_weta2_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_weta2_13_chisq[0][0], 0.);
				m_f1_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_f1_13_chisq[0][0], 0.);
				m_deltae_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_deltae_13_chisq[0][0], 0.);
				m_wtot_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_wtot_13_chisq[0][0], 0.);
				m_fside_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_fside_13_chisq[0][0], 0.);
				m_w1_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_w1_13_chisq[0][0], 0.);
				m_eratio_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_eratio_13_chisq[0][0], 0.);
				m_e277_ff.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_e277_13_chisq[0][0], 0.);
				m_rhad_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_rhad_err_13_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_rhad_err_13_chisq[0][0], 0.);
				m_reta_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_reta_err_13_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_rphi_err_13_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_weta2_err_13_chisq[0][0], 0.);
				m_f1_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_f1_err_13_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_deltae_err_13_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_wtot_err_13_chisq[0][0], 0.);
				m_fside_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_fside_err_13_chisq[0][0], 0.);
				m_w1_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_w1_err_13_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_eratio_err_13_chisq[0][0], 0.);
				m_e277_fferr.Initialize(8, 8, pt_DVs_13_chisq, eta_DVs_13_chisq, &ff_u_e277_err_13_chisq[0][0], 0.);

				}

				break;

		case 14: // tune 14 -- optimized for low PT photon. It has been obtained comparing data12-MC using radiative Z decay
				// For PT > 30 GeV the results has been taken from tune 13

				//static double pt_DVs_14_chisq[10] = {10000, 15000, 20000, 25000, 30000, 40000, 50000, 60000, 80000, 100000};
				static double pt_DVs_14_chisq[10] = {15000, 20000, 25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_14_chisq[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				// unconverted photons

				static double ff_u_rhad_14_chisq[10][8]={{3.01990e-04,1.06600e-04,1.49942e-03,1.04182e-03,9.99000e+02,6.86890e-04,1.57791e-03,7.00595e-04},
				{1.87434e-04,3.50741e-04,8.03042e-04,5.81278e-04,9.99000e+02,-4.63109e-04,-2.32530e-04,2.35541e-04},
				{7.98147e-05,4.89718e-04,8.62799e-04,4.20126e-04,9.99000e+02,-7.48079e-04,8.95378e-04,-2.53557e-04},
				{1.01316e-04,-5.19284e-05,5.08670e-04,2.48532e-04,9.99000e+02,-9.18033e-04,-1.10455e-03,8.57802e-04},
				{3.92030e-05,-3.65230e-05,3.95400e-04,-1.24300e-05,9.99000e+02,1.18110e-03,-5.04130e-04,-7.13060e-04},
				{-1.12530e-04,4.64730e-06,3.82140e-04,1.34730e-04,9.99000e+02,2.94330e-04,1.68570e-03,-2.99480e-05},
				{-4.09160e-05,-2.50950e-04,2.73410e-04,9.78690e-05,9.99000e+02,9.69450e-04,2.44360e-04,1.63890e-04},
				{-1.07450e-04,-6.31890e-06,2.54590e-04,2.31060e-04,9.99000e+02,6.74240e-04,5.48490e-06,-5.00390e-06},
				{-1.55110e-04,-3.66980e-04,1.80640e-04,-1.80910e-04,9.99000e+02,-3.39140e-06,-9.84390e-04,-1.76620e-04},
				{-3.63110e-04,-1.32060e-05,2.33720e-05,7.12670e-06,9.99000e+02,-2.95480e-04,3.20520e-04,-6.98670e-05}
				};

				static double ff_u_reta_14_chisq[10][8]={{-3.14493e-03,-5.40224e-03,-6.55478e-03,-6.39558e-03,9.99000e+02,-7.75078e-03,-8.20945e-03,-8.56934e-03},
				{-2.58385e-03,-3.92957e-03,-5.70366e-03,-5.08516e-03,9.99000e+02,-7.15009e-03,-6.91429e-03,-7.55832e-03},
				{-2.67726e-03,-3.58830e-03,-4.64208e-03,-5.55243e-03,9.99000e+02,-6.17117e-03,-5.28978e-03,-7.73905e-03},
				{-2.34446e-03,-2.02349e-03,-4.29439e-03,-6.08580e-03,9.99000e+02,-5.59200e-03,-6.03506e-03,-8.13631e-03},
				{-9.85360e-04,-3.95600e-03,-5.41730e-03,-5.84890e-03,9.99000e+02,-5.80440e-03,-7.93970e-03,-9.67020e-03},
				{-1.61230e-03,-1.88520e-03,-3.43510e-03,-5.34390e-03,9.99000e+02,-6.66470e-03,-6.38200e-03,-9.22320e-03},
				{-7.82470e-04,-7.49320e-04,-3.87560e-03,-4.17730e-03,9.99000e+02,-5.16710e-03,-6.52910e-03,-9.70840e-03},
				{-1.68480e-03,-2.23160e-03,-6.06130e-03,-4.95770e-03,9.99000e+02,-4.95430e-03,-7.87200e-03,-7.82570e-03},
				{-1.05860e-03,-2.31400e-03,-4.47610e-03,-4.34870e-03,9.99000e+02,-5.05740e-03,-4.69110e-03,-8.50260e-03},
				{-1.46570e-03,-1.32010e-03,-4.65540e-03,-4.69020e-03,9.99000e+02,-4.01830e-03,-5.92500e-03,-7.40480e-03}
				};

				static double ff_u_rphi_14_chisq[10][8]={{5.00622e-04,-2.28691e-03,2.30311e-04,-1.56821e-03,9.99000e+02,1.44753e-04,1.30386e-03,1.55092e-03},
				{7.40026e-04,-1.04691e-03,8.09942e-04,-1.71369e-03,9.99000e+02,4.77230e-04,1.61961e-03,2.24752e-03},
				{6.59889e-04,3.89566e-04,2.30257e-03,-8.36413e-04,9.99000e+02,1.63108e-03,3.96969e-03,1.88046e-03},
				{9.80534e-04,9.57152e-04,2.52149e-03,-1.51262e-03,9.99000e+02,9.33773e-04,2.53117e-03,8.08853e-04},
				{2.40040e-04,-1.69760e-03,1.45870e-03,1.33080e-03,9.99000e+02,3.90730e-05,1.99570e-03,-8.70800e-04},
				{1.72620e-03,1.91240e-03,1.94280e-03,-7.37490e-04,9.99000e+02,7.10670e-04,1.83410e-03,5.77640e-05},
				{1.82170e-03,-9.85590e-04,3.24720e-03,3.36720e-03,9.99000e+02,2.27180e-03,6.46080e-04,2.82380e-04},
				{1.10340e-03,2.38110e-03,5.80500e-04,-2.60520e-04,9.99000e+02,1.90900e-03,2.26550e-03,1.12720e-03},
				{2.04330e-03,-8.46660e-04,2.91120e-04,-2.06010e-05,9.99000e+02,2.03250e-03,3.70410e-03,1.33190e-03},
				{1.48150e-03,1.07760e-03,1.16630e-03,2.80130e-04,9.99000e+02,2.22580e-03,2.22630e-03,2.41130e-03}
				};

				static double ff_u_weta2_14_chisq[10][8]={{1.37797e-04,2.05421e-04,9.64048e-05,1.28772e-04,9.99000e+02,3.75914e-04,4.24049e-04,3.83919e-04},
				{1.10681e-04,1.71013e-04,7.14709e-05,1.40253e-04,9.99000e+02,3.89292e-04,3.94320e-04,3.62787e-04},
				{1.27207e-04,1.44093e-04,3.13636e-05,1.13621e-04,9.99000e+02,3.53258e-04,3.44819e-04,4.03749e-04},
				{9.84282e-05,1.54104e-04,5.40799e-05,8.48850e-05,9.99000e+02,3.83474e-04,4.02271e-04,4.08465e-04},
				{1.37780e-04,2.14520e-04,2.07360e-05,9.70640e-05,9.99000e+02,2.61250e-04,4.94900e-04,4.58200e-04},
				{7.74380e-05,8.31510e-05,6.62440e-05,7.14640e-05,9.99000e+02,3.16390e-04,3.69640e-04,4.03180e-04},
				{9.06880e-05,1.20860e-04,8.46490e-06,-2.77640e-05,9.99000e+02,3.37120e-04,3.99910e-04,4.73670e-04},
				{4.30010e-05,1.00250e-04,1.12180e-04,6.74160e-05,9.99000e+02,2.42570e-04,3.45280e-04,3.40780e-04},
				{5.92020e-05,2.09800e-04,4.76700e-05,1.03470e-04,9.99000e+02,2.92830e-04,3.21620e-04,3.70670e-04},
				{1.11340e-04,1.46390e-04,6.55110e-05,8.78680e-05,9.99000e+02,3.61510e-04,3.55110e-04,4.04360e-04}
				};

				static double ff_u_f1_14_chisq[10][8]={{-5.04201e-03,-2.91776e-03,-2.35237e-03,-7.16889e-03,9.99000e+02,1.41390e-02,2.63520e-02,1.37710e-02},
				{-6.96270e-03,-4.15445e-03,-3.58566e-03,-2.61210e-03,9.99000e+02,1.60857e-02,1.61161e-02,8.93669e-03},
				{-1.05810e-03,-3.52562e-03,-3.50389e-03,-4.43855e-03,9.99000e+02,1.27026e-02,7.13778e-03,1.44411e-02},
				{-1.88537e-03,1.39717e-03,-5.05811e-03,-6.26670e-03,9.99000e+02,2.13310e-02,1.99264e-02,2.26892e-02},
				{3.45210e-03,1.65990e-04,-8.40510e-03,-6.72230e-03,9.99000e+02,3.23010e-04,4.00240e-02,3.30590e-02},
				{-2.91390e-03,-1.04000e-02,-1.13840e-02,-4.88350e-03,9.99000e+02,1.12990e-02,1.91910e-02,2.43190e-02},
				{-2.96560e-03,-2.52510e-02,-1.03260e-02,-1.46640e-02,9.99000e+02,1.17360e-02,1.20920e-02,3.10400e-02},
				{-6.94070e-03,-2.23020e-02,8.13710e-03,-1.66520e-03,9.99000e+02,-9.36420e-03,2.73900e-02,1.05820e-02},
				{-6.41450e-04,9.48710e-03,-2.75610e-03,-5.04380e-03,9.99000e+02,-1.23990e-02,5.00080e-03,-1.49270e-03},
				{7.84500e-03,1.16150e-03,2.06910e-03,-1.89590e-03,9.99000e+02,2.38150e-03,-2.74290e-03,1.21730e-02}
				};

				static double ff_u_wtot_14_chisq[10][8]={{1.00598e-01,1.15364e-01,1.04984e-01,1.26512e-01,9.99000e+02,1.73276e-01,1.92537e-01,1.30458e-01},
				{7.10968e-02,1.14285e-01,1.08651e-01,1.31264e-01,9.99000e+02,1.46392e-01,1.52760e-01,1.09555e-01},
				{6.16461e-02,9.23411e-02,1.13174e-01,1.11019e-01,9.99000e+02,1.22340e-01,1.40691e-01,1.28330e-01},
				{8.35439e-02,6.53211e-02,9.88464e-02,1.14273e-01,9.99000e+02,1.48746e-01,1.71288e-01,1.28682e-01},
				{4.01320e-02,8.85980e-02,8.48730e-02,1.01700e-01,9.99000e+02,1.44460e-01,1.42750e-01,1.17590e-01},
				{5.53950e-02,3.64450e-02,4.86850e-02,1.01490e-01,9.99000e+02,1.21890e-01,1.53250e-01,1.13030e-01},
				{2.17170e-02,9.16210e-02,1.22920e-01,6.57530e-02,9.99000e+02,1.49290e-01,1.33230e-01,1.02330e-01},
				{7.41390e-02,9.50780e-02,1.04370e-01,1.15040e-01,9.99000e+02,1.31560e-01,1.50610e-01,9.52800e-02},
				{3.19410e-02,3.15860e-02,8.48130e-02,5.31600e-02,9.99000e+02,7.21670e-02,1.24350e-01,1.07110e-01},
				{3.55460e-02,4.98400e-02,5.98550e-02,7.15970e-02,9.99000e+02,9.86140e-02,1.18820e-01,1.21810e-01}
				};

				static double ff_u_fside_14_chisq[10][8]={{1.51307e-02,2.16050e-02,2.27618e-02,2.96307e-02,9.99000e+02,4.19227e-02,4.27675e-02,3.17941e-02},
				{1.21047e-02,2.17804e-02,2.19347e-02,3.46137e-02,9.99000e+02,3.66173e-02,3.88230e-02,2.87762e-02},
				{1.39090e-02,1.63442e-02,2.06442e-02,2.74197e-02,9.99000e+02,3.54478e-02,3.23018e-02,3.09492e-02},
				{1.10829e-02,1.71837e-02,2.06966e-02,3.49803e-02,9.99000e+02,3.33628e-02,4.28644e-02,3.05171e-02},
				{1.12270e-02,2.14090e-02,1.81980e-02,1.92490e-02,9.99000e+02,4.22670e-02,4.54740e-02,3.41430e-02},
				{8.14220e-03,7.44970e-03,7.97690e-03,2.69130e-02,9.99000e+02,3.82670e-02,3.66250e-02,3.09240e-02},
				{8.11650e-03,9.99580e-03,1.78610e-02,3.95500e-03,9.99000e+02,4.12280e-02,3.50910e-02,2.65840e-02},
				{9.52530e-03,9.50970e-03,2.04650e-02,2.66980e-02,9.99000e+02,3.10900e-02,3.24470e-02,2.69060e-02},
				{1.09590e-02,6.87870e-03,2.08050e-02,2.61400e-02,9.99000e+02,1.47810e-02,3.54280e-02,2.76650e-02},
				{8.67880e-03,1.49900e-02,1.64290e-02,2.48720e-02,9.99000e+02,3.00580e-02,3.06460e-02,2.72700e-02}
				};

				static double ff_u_w1_14_chisq[10][8]={{5.79406e-03,1.28041e-02,1.36519e-02,1.35418e-02,9.99000e+02,2.48104e-02,3.89537e-02,3.59167e-02},
				{4.33169e-03,1.07594e-02,1.46054e-02,1.56331e-02,9.99000e+02,2.26855e-02,3.78717e-02,3.33670e-02},
				{5.63043e-03,1.12390e-02,1.19500e-02,1.28254e-02,9.99000e+02,2.18336e-02,3.05487e-02,3.13232e-02},
				{3.07132e-03,1.20302e-02,1.01998e-02,1.87655e-02,9.99000e+02,2.21612e-02,2.96443e-02,3.81390e-02},
				{3.36480e-03,1.24270e-02,1.16040e-02,8.42660e-03,9.99000e+02,1.98410e-02,3.79840e-02,3.89300e-02},
				{6.41430e-03,7.58070e-03,5.37050e-03,1.59950e-02,9.99000e+02,1.81200e-02,3.38850e-02,3.37220e-02},
				{-2.43130e-03,7.34050e-03,8.96530e-03,1.89040e-03,9.99000e+02,2.10910e-02,3.59770e-02,3.61480e-02},
				{6.33470e-04,2.56990e-03,2.21350e-02,1.12810e-02,9.99000e+02,1.16530e-02,2.88400e-02,3.39540e-02},
				{6.72980e-03,7.38850e-03,1.65240e-02,1.07260e-02,9.99000e+02,1.48290e-02,2.10730e-02,2.44280e-02},
				{3.33090e-03,6.50440e-03,1.67970e-02,1.80740e-02,9.99000e+02,1.97160e-02,2.69490e-02,3.22900e-02}
				};

				static double ff_u_eratio_14_chisq[10][8]={{-1.50168e-03,-2.26765e-03,-1.31868e-03,-4.35092e-03,9.99000e+02,-4.17823e-03,-2.41672e-03,-1.66433e-03},
				{-8.97242e-04,-1.99220e-03,-1.97832e-03,-1.76409e-03,9.99000e+02,-2.18798e-03,-5.86191e-04,-6.98177e-04},
				{1.85910e-04,-1.39620e-03,-1.41511e-03,-1.90186e-03,9.99000e+02,-5.10097e-04,-1.47576e-03,4.80491e-04},
				{-7.97437e-04,9.15061e-04,-1.26778e-03,-3.08350e-03,9.99000e+02,-2.02790e-03,-2.16606e-03,-1.01736e-03},
				{9.90690e-04,2.17530e-03,8.57870e-05,-2.19420e-03,9.99000e+02,-4.14810e-03,6.85230e-04,-2.37990e-04},
				{4.28060e-04,2.82510e-03,1.97130e-03,3.03410e-04,9.99000e+02,7.14640e-04,1.11710e-03,3.53470e-05},
				{2.15280e-03,2.80030e-04,-1.54910e-04,6.39370e-04,9.99000e+02,-2.05740e-03,2.76350e-04,6.87490e-04},
				{5.99730e-04,-7.91770e-04,-1.65800e-03,-8.84820e-04,9.99000e+02,-4.04800e-03,1.81460e-03,-8.10800e-04},
				{5.16850e-04,-1.29740e-03,-1.04180e-04,-4.54920e-03,9.99000e+02,-1.45910e-03,1.17890e-03,-1.33760e-03},
				{7.80510e-04,-1.22250e-03,-5.82420e-04,-2.53860e-03,9.99000e+02,-1.26960e-03,-8.93430e-04,-8.92280e-04}
				};

				static double ff_u_deltae_14_chisq[10][8]={{-1.94363e+00,-1.01686e+00,-3.84862e+00,-1.39978e+00,9.99000e+02,-1.54897e+00,-4.90577e+00,2.71038e-01},
				{-1.67536e+00,-9.61427e-01,-2.14541e+00,-7.55474e-01,9.99000e+02,-1.49931e+00,-3.06661e+00,8.87349e-01},
				{-1.42556e+00,-1.50335e+00,-1.37614e+00,-8.08339e-01,9.99000e+02,-2.09399e+00,-6.48161e+00,2.46465e+00},
				{-1.10371e+00,-1.85367e+00,-2.70968e+00,3.30350e+00,9.99000e+02,-1.29260e+00,-3.81168e+00,2.73703e+00},
				{-1.92010e+00,-3.04480e-01,-6.05940e-01,-1.22840e+00,9.99000e+02,-3.42960e+00,-3.99410e+00,-3.45110e+00},
				{-2.19760e+00,-3.42490e+00,-2.75500e+00,-1.38320e+00,9.99000e+02,3.74090e+00,1.71180e+00,-4.01270e+00},
				{-3.72570e+00,-3.72600e+00,-1.63990e+00,-3.83090e+00,9.99000e+02,-2.42240e-01,-6.55720e+00,-4.35270e+00},
				{6.48740e-01,-4.07320e+00,-2.57990e+00,-1.10520e+00,9.99000e+02,-2.81750e+00,-2.84970e+00,-3.57700e+00},
				{-5.26460e+00,-6.36200e+00,-2.23770e+00,3.20360e+00,9.99000e+02,-3.46890e+00,-8.98890e+00,-1.47630e+01},
				{-1.73330e+00,-2.80280e+00,-7.19150e-01,-2.00620e+00,9.99000e+02,-6.54350e-01,-6.11860e+00,1.29500e+00}
				};

				static double ff_u_e277_14_chisq[10][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				// converted photons

				static double ff_c_rhad_14_chisq[10][8]={{4.64757e-04,-1.27008e-04,1.08799e-03,6.64241e-04,9.99000e+02,1.48317e-03,-1.07479e-03,2.73357e-04},
				{4.55091e-05,5.18423e-04,9.78948e-04,6.91613e-04,9.99000e+02,1.20708e-03,-2.16136e-04,-1.80492e-04},
				{1.85544e-04,-3.77453e-04,5.06926e-04,2.57080e-04,9.99000e+02,-1.57149e-04,-1.44076e-05,7.43256e-04},
				{2.31183e-04,5.26746e-04,6.06356e-04,-5.33411e-05,9.99000e+02,-6.27614e-04,-5.99580e-04,9.85897e-06},
				{2.30660e-04,-4.20800e-04,3.40950e-04,5.11260e-04,9.99000e+02,-2.45000e-04,5.58900e-04,-3.54600e-04},
				{3.52550e-04,-2.50220e-04,5.03860e-04,1.73390e-04,9.99000e+02,1.66720e-04,7.16600e-05,-1.58790e-04},
				{1.48050e-04,1.97950e-04,2.04610e-04,1.50070e-04,9.99000e+02,-6.20220e-04,-6.37780e-04,7.76690e-04},
				{-4.78350e-04,-1.20010e-04,8.69230e-05,-3.28490e-05,9.99000e+02,-9.25650e-04,-5.63230e-04,8.78020e-05},
				{1.10580e-04,-8.77610e-05,-5.94200e-05,-1.48780e-04,9.99000e+02,-4.82330e-04,4.33800e-04,4.49440e-04},
				{-1.19010e-04,-1.20490e-04,6.68490e-05,8.64410e-05,9.99000e+02,6.48520e-05,-3.77470e-04,2.30760e-04}
				};

				static double ff_c_reta_14_chisq[10][8]={{-3.73610e-03,-5.03152e-03,-4.18345e-03,-8.60384e-03,9.99000e+02,-9.49046e-03,-1.02004e-02,-8.93526e-03},
				{-4.59087e-03,-6.18706e-03,-6.35614e-03,-5.75742e-03,9.99000e+02,-7.49433e-03,-9.27863e-03,-9.38862e-03},
				{-2.64774e-03,-5.84488e-03,-6.66151e-03,-5.60286e-03,9.99000e+02,-5.99606e-03,-9.91913e-03,-7.09727e-03},
				{-8.01449e-04,-2.38203e-04,-5.74187e-03,-4.08679e-03,9.99000e+02,-6.76346e-03,-7.97438e-03,-8.49087e-03},
				{-2.96490e-03,-4.75050e-03,-6.61560e-03,-2.41540e-03,9.99000e+02,-7.55070e-03,-6.84420e-03,-9.68950e-03},
				{-1.22610e-03,-3.14690e-03,-3.61520e-03,-3.52920e-03,9.99000e+02,-7.04450e-03,-8.90730e-03,-7.53690e-03},
				{-1.10550e-03,-1.56670e-03,-3.01500e-03,-4.49860e-03,9.99000e+02,-8.37510e-03,-7.49000e-03,-6.44510e-03},
				{-3.11100e-03,-1.01690e-03,-4.35860e-03,-6.00530e-03,9.99000e+02,-5.26450e-03,-1.06330e-02,-7.13150e-03},
				{-1.43170e-03,-3.66110e-03,-4.06120e-03,-2.81210e-03,9.99000e+02,-8.09260e-03,-7.21500e-03,-9.20700e-03},
				{-1.80150e-03,-2.72290e-03,-3.52560e-03,-5.73670e-03,9.99000e+02,-6.28650e-03,-7.46240e-03,-8.26000e-03}
				};

				static double ff_c_rphi_14_chisq[10][8]={{-4.22799e-03,-3.86668e-03,-1.86766e-03,2.26187e-03,9.99000e+02,1.22707e-03,-1.58721e-03,1.50059e-03},
				{-1.02764e-03,-3.24934e-03,-2.62678e-03,1.52289e-03,9.99000e+02,2.22713e-03,-3.42478e-04,-5.65309e-04},
				{-1.91692e-03,2.64860e-04,4.67518e-04,-3.82912e-04,9.99000e+02,2.21256e-03,7.07012e-04,2.01482e-03},
				{4.71611e-04,-6.44922e-03,-3.10325e-03,2.26974e-05,9.99000e+02,9.35995e-04,3.94415e-03,1.79858e-03},
				{5.12220e-03,-6.78990e-03,-9.60830e-03,-6.87890e-03,9.99000e+02,-7.62460e-03,-8.52250e-03,-3.34300e-03},
				{5.64260e-04,3.12340e-03,-2.97680e-03,-9.02130e-03,9.99000e+02,-6.55180e-03,-7.85680e-03,-2.18570e-03},
				{3.48040e-03,1.03590e-02,-7.21000e-03,-1.05220e-02,9.99000e+02,-6.56020e-03,-3.99350e-03,-1.27160e-03},
				{-3.08720e-03,1.33000e-02,3.31270e-03,-4.55250e-03,9.99000e+02,1.18730e-03,-6.42330e-03,1.13390e-04},
				{-8.14670e-05,-3.71940e-03,-3.13210e-03,4.84590e-03,9.99000e+02,-1.46770e-03,-7.97350e-04,-9.94490e-04},
				{9.60720e-04,-5.33060e-03,3.62660e-04,-3.67500e-03,9.99000e+02,-2.41710e-03,4.69870e-04,4.05190e-04}
				};

				static double ff_c_weta2_14_chisq[10][8]={{1.98409e-04,3.22970e-04,7.92208e-05,2.19314e-04,9.99000e+02,3.75507e-04,4.03931e-04,3.76047e-04},
				{1.34755e-04,2.69886e-04,6.54611e-05,1.00722e-04,9.99000e+02,3.46028e-04,4.41806e-04,3.97585e-04},
				{1.48300e-04,1.68573e-04,6.50760e-05,1.63989e-04,9.99000e+02,3.74303e-04,3.35534e-04,3.35675e-04},
				{1.26161e-04,1.08118e-04,1.36962e-04,1.02021e-04,9.99000e+02,3.04198e-04,3.13415e-04,3.47264e-04},
				{4.31610e-06,4.32690e-04,1.41100e-04,5.02040e-05,9.99000e+02,4.62060e-04,4.19350e-04,3.44800e-04},
				{8.85720e-05,4.22410e-05,9.50310e-05,1.03180e-04,9.99000e+02,2.94250e-04,4.92400e-04,3.62380e-04},
				{9.38210e-05,3.64680e-06,7.57810e-05,2.02280e-04,9.99000e+02,3.42450e-04,4.19200e-04,3.82260e-04},
				{1.08990e-04,2.00310e-05,9.45800e-05,1.13280e-04,9.99000e+02,2.18480e-04,6.28570e-04,4.56160e-04},
				{2.80230e-05,1.08290e-04,1.11300e-05,1.38810e-04,9.99000e+02,4.21570e-04,5.09930e-04,4.73670e-04},
				{1.50170e-04,2.69290e-04,7.83210e-05,1.72550e-04,9.99000e+02,3.92600e-04,3.52730e-04,3.60140e-04}
				};

				static double ff_c_f1_14_chisq[10][8]={{3.26631e-03,5.46598e-03,-1.07193e-04,2.49858e-04,9.99000e+02,1.13769e-02,2.10017e-02,1.60434e-02},
				{-5.75569e-03,-8.05427e-03,-3.53615e-03,-2.58938e-03,9.99000e+02,7.85458e-03,3.17606e-02,1.89800e-02},
				{1.91498e-03,-3.95861e-03,-3.36613e-03,-6.24512e-03,9.99000e+02,8.96793e-03,2.50378e-02,9.69809e-03},
				{-1.26381e-03,-1.09244e-02,-7.22179e-03,-3.55385e-03,9.99000e+02,1.21745e-02,2.07059e-02,1.51878e-02},
				{-4.78210e-04,1.42550e-02,2.77510e-03,-8.03700e-03,9.99000e+02,1.01520e-02,2.83770e-02,2.15390e-02},
				{1.08280e-02,-1.15880e-02,-1.03870e-03,9.72180e-04,9.99000e+02,1.33670e-02,4.16500e-02,2.68940e-02},
				{-1.02270e-02,-9.96250e-03,-3.64280e-03,-3.03340e-03,9.99000e+02,1.70480e-02,2.81680e-02,1.86200e-02},
				{1.66980e-02,-1.64750e-02,-4.82020e-03,6.37370e-03,9.99000e+02,-5.95450e-03,5.40180e-02,3.58740e-02},
				{-9.79330e-03,2.11280e-02,-9.64230e-03,1.17910e-02,9.99000e+02,3.88590e-02,4.33710e-02,4.73230e-02},
				{9.62470e-03,6.22230e-03,-1.40430e-03,1.40400e-02,9.99000e+02,1.95010e-02,2.48700e-02,3.39890e-02}
				};

				static double ff_c_wtot_14_chisq[10][8]={{1.22288e-01,1.22941e-01,9.77039e-02,1.53301e-01,9.99000e+02,1.70148e-01,2.08533e-01,1.21245e-01},
				{9.14638e-02,8.19421e-02,1.08757e-01,1.14880e-01,9.99000e+02,1.66018e-01,1.84575e-01,1.33550e-01},
				{8.58485e-02,1.26044e-01,1.23028e-01,1.26858e-01,9.99000e+02,1.45307e-01,1.93311e-01,1.16541e-01},
				{6.10707e-02,1.48916e-01,1.23566e-01,1.09969e-01,9.99000e+02,1.68660e-01,1.89491e-01,1.16027e-01},
				{5.12820e-02,6.75450e-02,1.37610e-01,1.51320e-01,9.99000e+02,1.31880e-01,1.76040e-01,9.34450e-02},
				{5.40280e-02,6.23700e-02,8.96230e-02,1.24930e-01,9.99000e+02,1.77240e-01,1.78370e-01,8.91760e-02},
				{4.63110e-02,4.02950e-02,6.73320e-02,1.34110e-01,9.99000e+02,1.97680e-01,1.52670e-01,1.12140e-01},
				{2.97650e-02,7.26980e-02,8.77930e-02,6.98470e-02,9.99000e+02,1.31610e-01,1.96650e-01,7.72530e-02},
				{3.31100e-02,1.60580e-01,1.64510e-01,8.24350e-02,9.99000e+02,1.10510e-01,1.64010e-01,1.21030e-01},
				{3.17860e-02,9.07600e-02,1.16380e-01,1.03010e-01,9.99000e+02,1.89590e-01,1.58210e-01,1.06950e-01}
				};

				static double ff_c_fside_14_chisq[10][8]={{2.33590e-02,2.99866e-02,2.90045e-02,2.85036e-02,9.99000e+02,4.75685e-02,4.88135e-02,3.75037e-02},
				{1.65011e-02,1.72889e-02,3.14656e-02,2.69882e-02,9.99000e+02,4.47634e-02,4.67335e-02,3.56157e-02},
				{1.59316e-02,2.87870e-02,2.48002e-02,3.33964e-02,9.99000e+02,3.91680e-02,4.66179e-02,3.15828e-02},
				{1.74402e-02,2.49362e-02,3.11522e-02,3.16376e-02,9.99000e+02,4.07989e-02,4.22678e-02,3.49066e-02},
				{9.07350e-03,2.86610e-02,3.72110e-02,3.82050e-02,9.99000e+02,4.52950e-02,4.89630e-02,3.37930e-02},
				{9.72270e-03,1.79000e-02,2.37410e-02,3.09010e-02,9.99000e+02,4.86640e-02,5.43810e-02,2.69780e-02},
				{1.11320e-02,1.97460e-02,2.39040e-02,3.44200e-02,9.99000e+02,4.56040e-02,4.88790e-02,3.26450e-02},
				{4.36650e-03,2.35890e-02,1.79520e-02,2.88360e-02,9.99000e+02,3.76490e-02,4.86580e-02,2.88470e-02},
				{1.07820e-02,3.22420e-02,3.34340e-02,3.25960e-02,9.99000e+02,4.29190e-02,5.10700e-02,3.20660e-02},
				{8.39530e-03,1.82270e-02,3.00090e-02,2.89710e-02,9.99000e+02,4.99300e-02,4.29440e-02,3.32710e-02}
				};

				static double ff_c_w1_14_chisq[10][8]={{1.10175e-02,1.61725e-02,1.25803e-02,1.34212e-02,9.99000e+02,1.88527e-02,3.39228e-02,3.22951e-02},
				{7.07155e-03,8.59663e-03,1.27786e-02,1.17164e-02,9.99000e+02,1.90543e-02,3.43627e-02,2.69988e-02},
				{7.68057e-03,9.60620e-03,1.18132e-02,1.22961e-02,9.99000e+02,1.86296e-02,3.02869e-02,2.91606e-02},
				{5.11466e-03,1.27792e-02,1.35095e-02,1.24201e-02,9.99000e+02,1.77076e-02,3.48903e-02,3.09586e-02},
				{4.27650e-03,1.71170e-02,2.22930e-02,8.05540e-03,9.99000e+02,2.41850e-02,2.99090e-02,3.09440e-02},
				{8.33170e-03,2.30960e-02,1.17760e-02,1.31930e-02,9.99000e+02,1.82230e-02,3.31800e-02,3.34150e-02},
				{1.85560e-03,7.04160e-03,8.85980e-03,1.37740e-02,9.99000e+02,1.54190e-02,4.15950e-02,3.28180e-02},
				{2.49210e-03,2.73410e-03,1.45250e-02,1.11420e-02,9.99000e+02,1.53380e-02,3.55350e-02,3.12470e-02},
				{1.10420e-02,8.21000e-03,1.92680e-02,1.73980e-02,9.99000e+02,2.65210e-02,3.61480e-02,3.18400e-02},
				{4.32070e-03,1.16260e-02,1.59620e-02,1.79260e-02,9.99000e+02,2.45100e-02,3.03450e-02,2.74550e-02}
				};

				static double ff_c_eratio_14_chisq[10][8]={{-2.90838e-03,-3.41207e-03,-3.41016e-03,5.04755e-05,9.99000e+02,-4.82753e-03,-3.34969e-03,7.71518e-05},
				{-1.11345e-04,1.48832e-03,7.03136e-04,-3.51272e-03,9.99000e+02,-4.13307e-03,-6.62911e-04,2.99959e-04},
				{7.11975e-04,-5.03064e-03,-2.11895e-04,-3.07017e-03,9.99000e+02,-3.27835e-03,-1.56819e-03,2.73070e-04},
				{-1.70465e-03,2.62046e-03,-5.33622e-03,4.75597e-03,9.99000e+02,-2.12991e-03,-2.27542e-03,3.61022e-03},
				{-1.05190e-03,5.42880e-03,-2.09240e-03,-9.14770e-04,9.99000e+02,-2.64720e-03,1.13070e-03,4.77380e-04},
				{8.29880e-04,8.51470e-05,1.58330e-03,-1.74480e-03,9.99000e+02,-2.26000e-03,1.28980e-04,-4.57420e-04},
				{2.02470e-03,-1.28710e-03,7.54400e-04,-8.19890e-03,9.99000e+02,-2.07390e-03,-2.37220e-03,-1.05360e-03},
				{2.75590e-03,-2.15810e-04,-1.75980e-03,3.21940e-03,9.99000e+02,-1.05570e-03,2.20180e-03,2.33600e-03},
				{-2.59460e-04,5.07580e-03,9.68300e-04,-5.05970e-03,9.99000e+02,-5.25370e-04,-7.92620e-04,-3.09630e-03},
				{4.03940e-04,-1.10320e-03,-3.29670e-03,-4.61660e-03,9.99000e+02,-2.65340e-03,-8.59560e-04,1.34250e-03}
				};

				static double ff_c_deltae_14_chisq[10][8]={{-2.54352e+00,-1.68880e+00,-3.37722e+00,-1.23740e+00,9.99000e+02,-1.79906e+00,-3.87213e+00,3.29392e+00},
				{-1.76179e+00,-1.00820e+00,-1.79012e+00,-9.70514e-01,9.99000e+02,-1.22895e+00,-3.09603e+00,5.13328e+00},
				{-1.98794e+00,-2.06932e+00,-2.76086e+00,-4.11643e-01,9.99000e+02,-1.76124e+00,-9.12913e+00,8.09530e+00},
				{-1.34664e+00,2.51862e+00,4.60612e-01,-7.04701e-02,9.99000e+02,-2.28255e+00,-2.94764e+00,2.02596e+00},
				{5.32580e-01,-3.60590e+00,-3.19180e+00,-9.42380e-01,9.99000e+02,-1.03040e+00,-8.39430e+00,-8.69580e-01},
				{-2.36160e+00,-1.12600e+00,-2.24560e+00,-2.03740e+00,9.99000e+02,-1.22390e+00,-1.89360e+00,-3.46350e+00},
				{7.43720e-01,6.54580e-01,4.31600e-01,2.75170e+00,9.99000e+02,4.26280e+00,-1.03620e+01,-2.31250e+00},
				{-4.82570e-01,-1.57490e+00,2.18950e+00,-7.56410e+00,9.99000e+02,-6.74640e+00,-1.57690e+00,-1.38960e+01},
				{-8.17790e+00,-7.06590e+00,-8.25050e+00,-7.47840e+00,9.99000e+02,8.04240e+00,7.26370e+00,3.63420e+00},
				{-5.47100e-01,-2.58010e+00,-1.45800e+00,5.47570e-01,9.99000e+02,-3.30960e+00,-5.25000e+00,-2.03910e+00}
				};

				static double ff_c_e277_14_chisq[10][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};


				static double ff_u_rhad_err_14_chisq[10][8]={{7.81682e-05,1.37127e-04,1.44824e-04,1.76844e-04,9.99000e+02,7.14896e-04,7.91206e-04,5.24806e-04},
				{7.67125e-05,1.33542e-04,1.33074e-04,1.82250e-04,9.99000e+02,5.88914e-04,7.58093e-04,4.89296e-04},
				{8.16625e-05,1.38659e-04,1.48882e-04,1.75410e-04,9.99000e+02,6.83059e-04,7.50949e-04,5.24841e-04},
				{9.17070e-05,1.53274e-04,1.65415e-04,2.13570e-04,9.99000e+02,7.54538e-04,8.24692e-04,5.82327e-04},
				{2.54640e-03,2.43240e-03,2.95500e-03,2.74070e-03,0.00000e+00,1.06010e-02,9.45400e-03,7.35590e-03},
				{2.24070e-03,1.90660e-03,2.53120e-03,2.21870e-03,0.00000e+00,8.70790e-03,7.71390e-03,5.79940e-03},
				{1.92990e-03,1.65130e-03,1.91760e-03,1.91810e-03,0.00000e+00,7.82200e-03,7.41100e-03,4.59560e-03},
				{1.87360e-03,1.37290e-03,1.79160e-03,1.48450e-03,0.00000e+00,6.10160e-03,5.17100e-03,3.95410e-03},
				{1.70450e-03,1.43590e-03,1.39440e-03,1.33710e-03,0.00000e+00,4.04670e-03,3.68070e-03,3.11010e-03},
				{1.73550e-03,9.41980e-04,1.01670e-03,9.50810e-04,0.00000e+00,3.59370e-03,2.95870e-03,1.94080e-03}
				};

				static double ff_u_reta_err_14_chisq[10][8]={{4.06126e-04,7.47275e-04,5.29367e-04,7.20229e-04,9.99000e+02,6.75316e-04,5.91345e-04,5.94846e-04},
				{4.06760e-04,6.78330e-04,5.20722e-04,6.79215e-04,9.99000e+02,6.81930e-04,6.63604e-04,5.58138e-04},
				{3.76238e-04,6.91862e-04,5.31843e-04,6.91077e-04,9.99000e+02,7.48441e-04,7.59581e-04,7.04890e-04},
				{4.56818e-04,8.64216e-04,6.54762e-04,8.71435e-04,9.99000e+02,8.06301e-04,8.61628e-04,8.43782e-04},
				{1.58450e-02,1.65130e-02,1.41600e-02,1.36940e-02,0.00000e+00,1.44900e-02,1.11300e-02,1.38080e-02},
				{1.23560e-02,1.36250e-02,1.24140e-02,1.17430e-02,0.00000e+00,1.21950e-02,1.01780e-02,1.22410e-02},
				{1.14520e-02,1.11730e-02,9.73790e-03,9.33300e-03,0.00000e+00,1.11850e-02,9.67670e-03,1.11110e-02},
				{9.40580e-03,1.19620e-02,8.28320e-03,8.27790e-03,0.00000e+00,1.09240e-02,1.03930e-02,1.10260e-02},
				{8.51580e-03,8.19980e-03,7.88640e-03,7.75600e-03,0.00000e+00,1.07430e-02,7.72440e-03,1.04040e-02},
				{6.78010e-03,7.27490e-03,7.18410e-03,7.29800e-03,0.00000e+00,9.40050e-03,7.29780e-03,9.81290e-03}
				};

				static double ff_u_rphi_err_14_chisq[10][8]={{3.01395e-04,5.79668e-04,4.44935e-04,7.08964e-04,9.99000e+02,5.39415e-04,4.89298e-04,4.29163e-04},
				{2.79185e-04,5.14833e-04,4.57124e-04,7.18277e-04,9.99000e+02,5.80728e-04,5.74051e-04,4.96275e-04},
				{3.09326e-04,5.96417e-04,5.08551e-04,7.83424e-04,9.99000e+02,6.66940e-04,6.27729e-04,5.27348e-04},
				{3.38059e-04,8.23422e-04,6.43434e-04,9.62286e-04,9.99000e+02,7.45850e-04,7.53338e-04,7.67802e-04},
				{1.18130e-02,1.26180e-02,1.39370e-02,1.92040e-02,0.00000e+00,1.42530e-02,1.09210e-02,1.30940e-02},
				{1.03470e-02,1.39050e-02,1.17350e-02,1.50760e-02,0.00000e+00,1.32650e-02,9.27380e-03,1.13760e-02},
				{9.38960e-03,1.10780e-02,1.10100e-02,1.46280e-02,0.00000e+00,1.17710e-02,8.59830e-03,1.02890e-02},
				{8.47410e-03,1.01260e-02,9.65910e-03,1.15190e-02,0.00000e+00,1.06970e-02,9.04410e-03,1.07020e-02},
				{7.60420e-03,8.65600e-03,8.39520e-03,9.11550e-03,0.00000e+00,1.13340e-02,9.00450e-03,8.55330e-03},
				{6.54660e-03,7.19510e-03,7.73460e-03,9.39070e-03,0.00000e+00,9.26900e-03,7.45000e-03,8.74880e-03}
				};

				static double ff_u_weta2_err_14_chisq[10][8]={{1.18107e-05,2.36764e-05,1.71399e-05,2.31195e-05,9.99000e+02,2.53415e-05,2.51939e-05,1.92090e-05},
				{1.27372e-05,2.38736e-05,1.87613e-05,2.62205e-05,9.99000e+02,2.69991e-05,2.82666e-05,2.33375e-05},
				{1.57045e-05,2.73006e-05,2.24009e-05,2.72106e-05,9.99000e+02,3.22657e-05,3.54631e-05,2.79971e-05},
				{1.82704e-05,3.80229e-05,2.76367e-05,3.66041e-05,9.99000e+02,3.78094e-05,4.53502e-05,3.87461e-05},
				{6.44410e-04,7.10500e-04,6.40590e-04,6.40370e-04,0.00000e+00,7.15900e-04,6.33940e-04,5.98630e-04},
				{5.61530e-04,6.89130e-04,5.68800e-04,5.46430e-04,0.00000e+00,6.36600e-04,5.53500e-04,5.66190e-04},
				{5.25000e-04,6.48880e-04,5.52820e-04,5.23870e-04,0.00000e+00,5.92200e-04,5.82000e-04,5.24360e-04},
				{4.99690e-04,6.01800e-04,4.82350e-04,5.08530e-04,0.00000e+00,6.01230e-04,5.11800e-04,5.48350e-04},
				{4.35980e-04,5.08190e-04,4.64620e-04,4.51110e-04,0.00000e+00,6.00230e-04,5.93290e-04,5.77060e-04},
				{3.90860e-04,4.58080e-04,3.86270e-04,4.38980e-04,0.00000e+00,5.35150e-04,5.06490e-04,4.96450e-04}
				};

				static double ff_u_f1_err_14_chisq[10][8]={{1.54177e-03,2.54115e-03,1.65187e-03,2.11743e-03,9.99000e+02,2.28854e-03,4.44473e-03,3.96072e-03},
				{2.06320e-03,3.30690e-03,2.23548e-03,2.61962e-03,9.99000e+02,2.95031e-03,5.58818e-03,5.01513e-03},
				{2.74413e-03,4.82390e-03,2.66397e-03,3.58918e-03,9.99000e+02,4.23121e-03,6.41132e-03,5.68357e-03},
				{3.70911e-03,6.85198e-03,3.88264e-03,4.63567e-03,9.99000e+02,5.99125e-03,8.44280e-03,7.72024e-03},
				{1.43930e-01,1.42030e-01,9.82870e-02,7.92000e-02,0.00000e+00,1.10410e-01,1.25510e-01,1.30140e-01},
				{1.30250e-01,1.26020e-01,9.80510e-02,9.07200e-02,0.00000e+00,1.14430e-01,1.13500e-01,1.29460e-01},
				{1.33070e-01,1.32690e-01,9.52530e-02,8.09910e-02,0.00000e+00,1.16390e-01,1.08380e-01,1.33640e-01},
				{1.27600e-01,1.27040e-01,1.03830e-01,8.04810e-02,0.00000e+00,1.19350e-01,1.07840e-01,1.21070e-01},
				{1.17390e-01,1.29170e-01,9.40810e-02,8.87610e-02,0.00000e+00,1.31200e-01,9.32190e-02,1.18510e-01},
				{1.04660e-01,1.24010e-01,8.57570e-02,7.94200e-02,0.00000e+00,1.33360e-01,8.96540e-02,9.71070e-02}
				};

				static double ff_u_wtot_err_14_chisq[10][8]={{5.87383e-03,1.03279e-02,1.02616e-02,1.20236e-02,9.99000e+02,1.39124e-02,1.04932e-02,6.19321e-03},
				{7.21580e-03,1.05523e-02,1.23921e-02,1.43890e-02,9.99000e+02,1.73247e-02,1.07634e-02,7.03541e-03},
				{8.40099e-03,1.34630e-02,1.43230e-02,1.58214e-02,9.99000e+02,1.70038e-02,1.46481e-02,8.08033e-03},
				{1.06704e-02,1.60295e-02,1.77421e-02,2.45029e-02,9.99000e+02,2.56398e-02,1.88235e-02,1.07994e-02},
				{3.31400e-01,3.18270e-01,3.58660e-01,3.78020e-01,0.00000e+00,3.96640e-01,1.73590e-01,1.61210e-01},
				{2.76630e-01,3.03240e-01,3.48510e-01,3.38000e-01,0.00000e+00,3.54480e-01,2.07600e-01,1.40830e-01},
				{2.80120e-01,2.78830e-01,3.53620e-01,2.97690e-01,0.00000e+00,3.25700e-01,1.53070e-01,1.45190e-01},
				{2.63400e-01,2.61100e-01,3.13530e-01,3.15430e-01,0.00000e+00,3.45360e-01,1.39660e-01,1.30640e-01},
				{2.33030e-01,2.23590e-01,3.10450e-01,3.23890e-01,0.00000e+00,3.66680e-01,1.34640e-01,1.30500e-01},
				{1.94990e-01,2.35940e-01,2.82460e-01,3.14320e-01,0.00000e+00,2.60390e-01,1.34410e-01,1.09890e-01}
				};

				static double ff_u_fside_err_14_chisq[10][8]={{9.24280e-04,1.90524e-03,2.06435e-03,2.71338e-03,9.99000e+02,2.45348e-03,1.88520e-03,1.29501e-03},
				{1.07520e-03,2.27129e-03,2.34087e-03,3.37039e-03,9.99000e+02,3.12427e-03,2.32770e-03,1.51976e-03},
				{1.19934e-03,2.45341e-03,2.85554e-03,3.83480e-03,9.99000e+02,3.71588e-03,2.44059e-03,1.88676e-03},
				{1.62295e-03,3.42996e-03,3.49476e-03,4.83134e-03,9.99000e+02,4.75933e-03,3.01419e-03,2.22640e-03},
				{6.26530e-02,7.33120e-02,8.78820e-02,9.45890e-02,0.00000e+00,9.71540e-02,4.24350e-02,4.17410e-02},
				{5.60070e-02,7.34200e-02,8.46600e-02,9.18030e-02,0.00000e+00,8.86560e-02,4.20670e-02,3.79260e-02},
				{5.34800e-02,6.83790e-02,8.02380e-02,8.69860e-02,0.00000e+00,7.78520e-02,3.63790e-02,3.86080e-02},
				{4.61420e-02,5.71740e-02,7.32060e-02,7.78580e-02,0.00000e+00,8.18130e-02,3.59200e-02,3.66850e-02},
				{4.68930e-02,5.56970e-02,6.13460e-02,7.07160e-02,0.00000e+00,8.55770e-02,3.87280e-02,3.05670e-02},
				{3.93420e-02,4.88100e-02,5.89980e-02,6.02220e-02,0.00000e+00,6.14830e-02,2.83900e-02,2.72440e-02}
				};

				static double ff_u_w1_err_14_chisq[10][8]={{6.88922e-04,1.47073e-03,1.28329e-03,1.48735e-03,9.99000e+02,1.72131e-03,1.80756e-03,1.23524e-03},
				{8.74821e-04,1.64858e-03,1.56002e-03,1.63469e-03,9.99000e+02,2.11296e-03,2.18053e-03,1.55758e-03},
				{1.07132e-03,1.97162e-03,1.91261e-03,2.00823e-03,9.99000e+02,2.85046e-03,2.82090e-03,1.95358e-03},
				{1.44610e-03,2.82191e-03,2.68067e-03,3.04458e-03,9.99000e+02,3.37166e-03,3.69418e-03,2.59270e-03},
				{5.42180e-02,5.67900e-02,6.57070e-02,4.52110e-02,0.00000e+00,5.43810e-02,4.92860e-02,4.49860e-02},
				{4.93630e-02,5.78830e-02,6.06410e-02,4.54440e-02,0.00000e+00,5.56010e-02,4.93740e-02,4.61680e-02},
				{5.18680e-02,5.20040e-02,6.41880e-02,4.15570e-02,0.00000e+00,5.23520e-02,4.33130e-02,4.34890e-02},
				{5.09550e-02,4.87100e-02,5.81320e-02,4.35080e-02,0.00000e+00,5.28190e-02,5.32780e-02,4.19740e-02},
				{4.37210e-02,5.45190e-02,5.65030e-02,4.40920e-02,0.00000e+00,5.87090e-02,5.21240e-02,3.50650e-02},
				{3.84530e-02,4.56020e-02,5.39320e-02,4.61970e-02,0.00000e+00,5.39900e-02,4.24950e-02,3.28780e-02}
				};

				static double ff_u_eratio_err_14_chisq[10][8]={{3.61406e-04,6.24895e-04,8.14909e-04,1.28632e-03,9.99000e+02,7.05352e-04,5.64131e-04,3.94399e-04},
				{4.38711e-04,6.68699e-04,9.37219e-04,1.13039e-03,9.99000e+02,6.64560e-04,6.58657e-04,5.23348e-04},
				{5.53484e-04,8.78193e-04,9.17988e-04,1.09410e-03,9.99000e+02,1.10203e-03,6.75489e-04,5.88744e-04},
				{4.30970e-04,1.07591e-03,1.32444e-03,2.20731e-03,9.99000e+02,3.96995e-04,1.19782e-03,8.89078e-04},
				{2.04770e-02,2.30170e-02,2.74670e-02,2.98540e-02,0.00000e+00,1.62180e-02,1.43550e-02,1.40840e-02},
				{1.84260e-02,1.92200e-02,2.83420e-02,2.80160e-02,0.00000e+00,1.55800e-02,1.59410e-02,1.39220e-02},
				{1.76990e-02,1.79490e-02,2.11760e-02,2.17730e-02,0.00000e+00,1.37330e-02,1.42720e-02,1.30090e-02},
				{1.51490e-02,1.57530e-02,2.14900e-02,2.54500e-02,0.00000e+00,1.01800e-02,1.23240e-02,1.15790e-02},
				{1.50480e-02,1.46040e-02,2.09950e-02,2.02010e-02,0.00000e+00,1.11870e-02,1.09160e-02,1.04460e-02},
				{1.30000e-02,1.35700e-02,1.91830e-02,1.98980e-02,0.00000e+00,9.54490e-03,9.53740e-03,1.05270e-02}
				};

				static double ff_u_deltae_err_14_chisq[10][8]={{1.81841e-01,4.90328e-01,1.69901e+00,3.60693e-01,9.99000e+02,6.62563e-01,1.30421e+00,1.29803e+00},
				{2.93472e-01,4.19619e-01,5.88405e-01,6.05822e-01,9.99000e+02,7.40681e-01,1.25207e+00,1.50661e+00},
				{4.19626e-01,5.52975e-01,8.11968e-01,6.67996e-01,9.99000e+02,6.70085e-01,2.61049e+00,1.08486e+00},
				{5.55691e-01,8.84569e-01,5.86948e-01,3.94061e-01,9.99000e+02,8.77002e-01,2.32447e+00,2.04172e+00},
				{2.44780e+01,2.23580e+01,2.05610e+01,2.04360e+01,0.00000e+00,3.17090e+01,3.29130e+01,3.41700e+01},
				{2.35220e+01,2.34540e+01,2.16180e+01,2.00130e+01,0.00000e+00,2.91640e+01,3.43340e+01,3.33800e+01},
				{2.42930e+01,2.36490e+01,2.04470e+01,2.15750e+01,0.00000e+00,3.13800e+01,4.09500e+01,3.39520e+01},
				{2.26000e+01,2.39350e+01,2.02890e+01,2.13680e+01,0.00000e+00,3.03780e+01,2.89250e+01,3.00980e+01},
				{2.39050e+01,2.39580e+01,2.03910e+01,1.88860e+01,0.00000e+00,2.98510e+01,4.32280e+01,3.51390e+01},
				{2.21700e+01,2.34970e+01,1.91630e+01,1.97130e+01,0.00000e+00,2.85690e+01,3.02870e+01,2.61080e+01}
				};

				static double ff_u_e277_err_14_chisq[10][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				static double ff_c_rhad_err_14_chisq[10][8]={{1.76421e-04,2.80514e-04,2.12331e-04,2.56615e-04,9.99000e+02,9.21229e-04,9.31287e-04,7.02209e-04},
				{1.59698e-04,2.67863e-04,2.06796e-04,2.36158e-04,9.99000e+02,7.52943e-04,9.35699e-04,6.83226e-04},
				{1.63044e-04,2.56150e-04,2.04339e-04,2.16314e-04,9.99000e+02,6.92569e-04,1.35700e-03,8.46291e-04},
				{1.86770e-04,2.72673e-04,2.35334e-04,2.78855e-04,9.99000e+02,1.03221e-03,1.09241e-03,8.06657e-04},
				{2.27210e-03,2.25850e-03,2.77290e-03,3.03960e-03,0.00000e+00,1.08600e-02,1.02680e-02,7.09760e-03},
				{2.12560e-03,1.89950e-03,2.24710e-03,2.45200e-03,0.00000e+00,8.52460e-03,7.62530e-03,5.71010e-03},
				{1.71180e-03,1.78230e-03,1.88420e-03,1.79310e-03,0.00000e+00,7.82490e-03,6.42130e-03,5.07710e-03},
				{1.78290e-03,1.37590e-03,1.76940e-03,1.84040e-03,0.00000e+00,5.21130e-03,4.84440e-03,3.57600e-03},
				{1.38970e-03,9.67550e-04,1.53970e-03,1.29650e-03,0.00000e+00,4.60930e-03,3.40530e-03,2.81860e-03},
				{1.36630e-03,9.95540e-04,1.07950e-03,8.97630e-04,0.00000e+00,3.12870e-03,2.55590e-03,2.07440e-03}
				};

				static double ff_c_reta_err_14_chisq[10][8]={{1.35385e-03,2.10119e-03,1.15304e-03,1.30491e-03,9.99000e+02,1.21300e-03,1.06641e-03,9.85328e-04},
				{9.58709e-04,1.88929e-03,9.98407e-04,1.29718e-03,9.99000e+02,9.30217e-04,8.43762e-04,1.01019e-03},
				{9.75820e-04,1.82329e-03,9.55385e-04,1.19332e-03,9.99000e+02,1.00222e-03,9.84385e-04,1.26842e-03},
				{1.19846e-03,1.79974e-03,1.16020e-03,1.32280e-03,9.99000e+02,1.22114e-03,1.10299e-03,1.40653e-03},
				{1.96720e-02,1.96130e-02,1.50590e-02,1.81690e-02,0.00000e+00,1.59020e-02,1.11660e-02,1.36490e-02},
				{1.41440e-02,1.65530e-02,1.46070e-02,1.52530e-02,0.00000e+00,1.41320e-02,1.07470e-02,1.35090e-02},
				{1.25150e-02,1.31410e-02,1.25980e-02,1.29670e-02,0.00000e+00,1.18780e-02,9.60710e-03,1.16760e-02},
				{1.36100e-02,1.28080e-02,1.09050e-02,1.39660e-02,0.00000e+00,1.22560e-02,8.86290e-03,1.11360e-02},
				{1.11410e-02,1.12290e-02,8.07490e-03,1.06430e-02,0.00000e+00,1.10940e-02,6.94680e-03,1.11540e-02},
				{7.12300e-03,7.93210e-03,7.62810e-03,7.88390e-03,0.00000e+00,9.67390e-03,7.14410e-03,8.93770e-03}
				};

				static double ff_c_rphi_err_14_chisq[10][8]={{1.41922e-03,2.59247e-03,1.79638e-03,2.68316e-03,9.99000e+02,1.54485e-03,1.37115e-03,1.78148e-03},
				{1.39699e-03,2.60786e-03,1.77954e-03,3.16643e-03,9.99000e+02,1.70615e-03,1.38826e-03,2.36046e-03},
				{1.64717e-03,3.10428e-03,1.96733e-03,3.10473e-03,9.99000e+02,1.81682e-03,2.02059e-03,3.22409e-03},
				{1.89256e-03,3.65909e-03,2.49594e-03,2.71904e-03,9.99000e+02,1.95392e-03,2.05013e-03,2.28053e-03},
				{3.01580e-02,3.87220e-02,5.22220e-02,6.26430e-02,0.00000e+00,4.22730e-02,2.56300e-02,2.60430e-02},
				{2.62740e-02,5.30030e-02,4.25490e-02,4.85240e-02,0.00000e+00,3.26880e-02,2.08190e-02,2.30190e-02},
				{2.94140e-02,5.69050e-02,3.30650e-02,3.62410e-02,0.00000e+00,2.49470e-02,1.65810e-02,1.98960e-02},
				{1.97640e-02,3.38130e-02,2.67550e-02,3.14970e-02,0.00000e+00,2.40560e-02,1.76390e-02,1.60590e-02},
				{1.82560e-02,1.74430e-02,1.90650e-02,2.29740e-02,0.00000e+00,1.87640e-02,1.31560e-02,1.27110e-02},
				{1.29220e-02,1.31020e-02,1.44750e-02,1.56130e-02,0.00000e+00,1.45960e-02,1.17160e-02,1.18440e-02}
				};

				static double ff_c_weta2_err_14_chisq[10][8]={{3.28814e-05,6.56600e-05,3.41293e-05,4.67811e-05,9.99000e+02,3.95258e-05,3.83096e-05,3.00721e-05},
				{3.51414e-05,5.96253e-05,3.24915e-05,4.37241e-05,9.99000e+02,3.70647e-05,3.70796e-05,3.53802e-05},
				{3.67431e-05,5.97346e-05,3.51692e-05,4.30810e-05,9.99000e+02,4.31774e-05,5.22231e-05,4.44427e-05},
				{4.33674e-05,8.60527e-05,4.17249e-05,5.66739e-05,9.99000e+02,5.25018e-05,5.65824e-05,5.31480e-05},
				{7.09320e-04,8.72880e-04,7.28870e-04,9.23540e-04,0.00000e+00,8.27480e-04,6.80650e-04,5.81300e-04},
				{6.98850e-04,8.05980e-04,6.44660e-04,7.32490e-04,0.00000e+00,7.51870e-04,6.81480e-04,5.82500e-04},
				{5.49020e-04,7.38320e-04,6.07300e-04,6.39090e-04,0.00000e+00,7.16110e-04,6.18200e-04,5.51260e-04},
				{5.33050e-04,6.74480e-04,5.23920e-04,6.64320e-04,0.00000e+00,6.61770e-04,4.95210e-04,5.33090e-04},
				{4.16210e-04,6.74470e-04,5.24280e-04,4.77360e-04,0.00000e+00,6.29870e-04,6.10110e-04,5.67710e-04},
				{3.96100e-04,4.66800e-04,4.47970e-04,4.68660e-04,0.00000e+00,6.06890e-04,5.03230e-04,4.50860e-04}
				};

				static double ff_c_f1_err_14_chisq[10][8]={{2.85332e-03,3.75282e-03,1.80948e-03,2.02489e-03,9.99000e+02,2.08541e-03,4.09047e-03,3.46828e-03},
				{3.21165e-03,4.75844e-03,2.43398e-03,2.34283e-03,9.99000e+02,2.64173e-03,4.49359e-03,4.88398e-03},
				{3.89236e-03,5.51673e-03,2.96150e-03,3.14479e-03,9.99000e+02,3.39340e-03,6.79468e-03,7.11001e-03},
				{5.84666e-03,7.58187e-03,4.22836e-03,4.04625e-03,9.99000e+02,3.86174e-03,7.00793e-03,9.88744e-03},
				{9.75780e-02,8.71170e-02,6.42060e-02,5.78170e-02,0.00000e+00,6.88280e-02,8.60790e-02,1.02610e-01},
				{8.85880e-02,9.10550e-02,6.80990e-02,6.40220e-02,0.00000e+00,6.69750e-02,8.27510e-02,9.82550e-02},
				{9.10680e-02,9.08900e-02,7.14240e-02,5.14070e-02,0.00000e+00,7.23280e-02,7.74330e-02,9.76080e-02},
				{9.36700e-02,9.92740e-02,7.23040e-02,6.65940e-02,0.00000e+00,7.06280e-02,8.14780e-02,9.86090e-02},
				{8.29050e-02,8.98510e-02,6.71700e-02,5.08180e-02,0.00000e+00,8.31920e-02,7.00930e-02,9.84680e-02},
				{9.07930e-02,9.23330e-02,6.60270e-02,5.59050e-02,0.00000e+00,8.88380e-02,7.97100e-02,8.69890e-02}
				};

				static double ff_c_wtot_err_14_chisq[10][8]={{1.24822e-02,1.94125e-02,1.69088e-02,1.78606e-02,9.99000e+02,1.83862e-02,1.15555e-02,6.63930e-03},
				{1.41995e-02,2.29094e-02,1.55228e-02,1.89257e-02,9.99000e+02,1.72474e-02,1.14768e-02,7.39546e-03},
				{1.46809e-02,2.33994e-02,1.72721e-02,1.75510e-02,9.99000e+02,2.02140e-02,1.36184e-02,9.75437e-03},
				{1.74103e-02,2.88468e-02,2.12823e-02,2.50225e-02,9.99000e+02,2.44718e-02,1.65169e-02,9.59357e-03},
				{2.84330e-01,2.88430e-01,3.84630e-01,3.23570e-01,0.00000e+00,4.19390e-01,1.88770e-01,1.24790e-01},
				{2.47490e-01,2.67910e-01,3.43720e-01,3.20880e-01,0.00000e+00,3.83250e-01,1.54590e-01,1.26090e-01},
				{2.14860e-01,2.69930e-01,3.13320e-01,2.99360e-01,0.00000e+00,3.38240e-01,1.60000e-01,1.17700e-01},
				{2.07760e-01,2.25120e-01,3.11290e-01,3.50640e-01,0.00000e+00,3.41710e-01,1.79210e-01,1.20790e-01},
				{2.00910e-01,2.04510e-01,2.79060e-01,2.76520e-01,0.00000e+00,4.16200e-01,1.24460e-01,1.17700e-01},
				{1.74770e-01,2.09960e-01,2.75670e-01,2.83820e-01,0.00000e+00,3.02280e-01,1.34010e-01,1.13520e-01}
				};

				static double ff_c_fside_err_14_chisq[10][8]={{2.04764e-03,4.34530e-03,3.38844e-03,4.31728e-03,9.99000e+02,4.13034e-03,2.30028e-03,1.58191e-03},
				{1.93792e-03,4.84920e-03,3.77535e-03,4.88088e-03,9.99000e+02,4.30085e-03,2.46152e-03,1.88317e-03},
				{2.41536e-03,5.31379e-03,4.24825e-03,5.11772e-03,9.99000e+02,5.32948e-03,2.81640e-03,2.38259e-03},
				{2.86875e-03,6.61636e-03,4.88107e-03,6.97101e-03,9.99000e+02,5.83280e-03,3.45619e-03,2.85212e-03},
				{6.08660e-02,6.99790e-02,9.09140e-02,9.87000e-02,0.00000e+00,1.11100e-01,4.62380e-02,3.50230e-02},
				{5.48830e-02,5.98180e-02,8.54270e-02,9.53480e-02,0.00000e+00,1.01230e-01,4.00400e-02,3.86680e-02},
				{4.54430e-02,6.17970e-02,7.67490e-02,9.72410e-02,0.00000e+00,9.05140e-02,3.64740e-02,3.41940e-02},
				{4.34390e-02,5.59700e-02,7.74820e-02,8.10770e-02,0.00000e+00,1.07400e-01,3.71370e-02,2.90830e-02},
				{3.33130e-02,5.07970e-02,6.14830e-02,7.76480e-02,0.00000e+00,9.15590e-02,3.06650e-02,2.95850e-02},
				{3.55770e-02,4.65390e-02,5.44360e-02,5.96940e-02,0.00000e+00,6.95790e-02,3.08950e-02,2.53620e-02}
				};

				static double ff_c_w1_err_14_chisq[10][8]={{1.50682e-03,2.83160e-03,2.07425e-03,2.02105e-03,9.99000e+02,1.59010e-03,2.45531e-03,1.80093e-03},
				{1.69630e-03,3.52254e-03,1.83858e-03,2.41132e-03,9.99000e+02,1.93761e-03,2.38074e-03,2.06176e-03},
				{1.93277e-03,3.82025e-03,2.44012e-03,2.46011e-03,9.99000e+02,2.54553e-03,3.64506e-03,2.93950e-03},
				{2.76843e-03,4.66983e-03,3.15125e-03,2.99209e-03,9.99000e+02,3.00092e-03,3.93320e-03,3.45558e-03},
				{4.63700e-02,5.09790e-02,4.91830e-02,4.97450e-02,0.00000e+00,4.84650e-02,5.12330e-02,4.03670e-02},
				{4.34230e-02,5.50230e-02,5.42020e-02,4.51070e-02,0.00000e+00,4.86360e-02,4.89350e-02,4.33760e-02},
				{4.14360e-02,5.18720e-02,5.05670e-02,4.26730e-02,0.00000e+00,4.67410e-02,4.92020e-02,4.55240e-02},
				{4.50340e-02,4.94530e-02,4.93740e-02,3.41350e-02,0.00000e+00,4.59820e-02,5.10690e-02,4.74090e-02},
				{3.22030e-02,4.42540e-02,4.60820e-02,4.14050e-02,0.00000e+00,4.31330e-02,4.18570e-02,4.03330e-02},
				{3.44370e-02,4.46860e-02,4.03580e-02,3.29480e-02,0.00000e+00,4.24970e-02,4.81450e-02,3.78290e-02}
				};

				static double ff_c_eratio_err_14_chisq[10][8]={{8.32328e-04,2.09444e-03,1.66924e-03,2.82567e-03,9.99000e+02,1.28368e-03,6.85452e-04,8.37561e-04},
				{1.17493e-03,9.82801e-04,1.35294e-03,2.45471e-03,9.99000e+02,1.19491e-03,1.16043e-03,3.78885e-04},
				{9.39575e-04,4.65830e-04,1.66558e-03,2.29462e-03,9.99000e+02,1.28200e-03,7.00331e-04,3.86407e-04},
				{1.17494e-03,2.80559e-03,2.26498e-03,2.99820e-03,9.99000e+02,1.42468e-03,4.89434e-04,2.20318e-03},
				{1.99980e-02,1.98790e-02,3.62630e-02,3.61560e-02,0.00000e+00,2.08570e-02,1.51150e-02,1.31620e-02},
				{1.89390e-02,1.98350e-02,3.01070e-02,3.36720e-02,0.00000e+00,1.72420e-02,1.15030e-02,1.18750e-02},
				{1.67230e-02,1.97190e-02,2.81900e-02,3.02460e-02,0.00000e+00,1.63430e-02,1.03680e-02,1.27010e-02},
				{1.69120e-02,1.53680e-02,2.60290e-02,2.71700e-02,0.00000e+00,1.39710e-02,1.06330e-02,1.16760e-02},
				{1.30870e-02,1.53980e-02,1.92290e-02,2.23060e-02,0.00000e+00,1.30920e-02,8.12850e-03,8.42970e-03},
				{1.24160e-02,1.43000e-02,1.83940e-02,1.91060e-02,0.00000e+00,9.43790e-03,7.64340e-03,1.12700e-02}
				};

				static double ff_c_deltae_err_14_chisq[10][8]={{5.61268e-01,1.08452e+00,4.71853e-01,5.28814e-01,9.99000e+02,8.15150e-01,1.40084e+00,6.74287e-01},
				{5.45504e-01,8.53872e-01,1.01931e+00,8.27781e-01,9.99000e+02,7.24813e-01,1.59103e+00,1.10174e+00},
				{6.54801e-01,1.03735e+00,6.86091e-01,8.18190e-01,9.99000e+02,1.52759e+00,4.16840e+00,7.72404e-01},
				{8.38602e-01,1.32820e+00,1.76706e+00,1.60135e+00,9.99000e+02,1.15321e+00,1.87737e+00,3.72731e+00},
				{2.29120e+01,2.99630e+01,2.36020e+01,2.35420e+01,0.00000e+00,3.23910e+01,3.44180e+01,2.87020e+01},
				{2.46310e+01,2.33680e+01,2.51260e+01,2.47210e+01,0.00000e+00,3.09640e+01,3.45710e+01,3.01090e+01},
				{2.32440e+01,2.25680e+01,2.12920e+01,1.97090e+01,0.00000e+00,2.70610e+01,3.35240e+01,2.78550e+01},
				{2.47000e+01,3.52440e+01,2.08070e+01,2.58460e+01,0.00000e+00,3.80800e+01,2.96340e+01,3.39320e+01},
				{2.57500e+01,2.48760e+01,2.68760e+01,2.49020e+01,0.00000e+00,2.66300e+01,2.80880e+01,2.55300e+01},
				{2.36520e+01,2.43140e+01,2.04320e+01,1.90420e+01,0.00000e+00,3.11800e+01,3.23400e+01,3.29190e+01}
				};

				static double ff_c_e277_err_14_chisq[10][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				if (isConv) {

				m_rhad_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_rhad_14_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_rhad_14_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_reta_14_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_rphi_14_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_weta2_14_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_f1_14_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_deltae_14_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_wtot_14_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_fside_14_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_w1_14_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_eratio_14_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_e277_14_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_rhad_err_14_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_rhad_err_14_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_reta_err_14_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_rphi_err_14_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_weta2_err_14_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_f1_err_14_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_deltae_err_14_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_wtot_err_14_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_fside_err_14_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_w1_err_14_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_eratio_err_14_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_c_e277_err_14_chisq[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_rhad_14_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_rhad_14_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_reta_14_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_rphi_14_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_weta2_14_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_f1_14_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_deltae_14_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_wtot_14_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_fside_14_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_w1_14_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_eratio_14_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_e277_14_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_rhad_err_14_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_rhad_err_14_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_reta_err_14_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_rphi_err_14_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_weta2_err_14_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_f1_err_14_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_deltae_err_14_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_wtot_err_14_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_fside_err_14_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_w1_err_14_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_eratio_err_14_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8, pt_DVs_14_chisq, eta_DVs_14_chisq, &ff_u_e277_err_14_chisq[0][0], 0.);

				}

				break;

		case 15: // MC12 GEO21 to MC12 GEO20

				static double pt_DVs_15_chisq[8] = {25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_15_chisq[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				// unconverted photons

				static double ff_u_rhad_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.6095e-04, -4.3086e-05, 9.0374e-05},
				{0., 0., 0., 0., 0., 2.8610e-06, -1.4733e-04, -6.4167e-05},
				{0., 0., 0., 0., 0., -3.8957e-05, -1.7431e-04, 6.7216e-05},
				{0., 0., 0., 0., 0., 8.9737e-05, 8.2504e-05, -1.8004e-06},
				{0., 0., 0., 0., 0., 2.1214e-04, 7.7802e-05, -3.8187e-05},
				{0., 0., 0., 0., 0., 1.3989e-04, 1.2235e-04, -1.9965e-05},
				{0., 0., 0., 0., 0., 9.4021e-05, 9.9950e-05, 3.0172e-05},
				{0., 0., 0., 0., 0., 1.8077e-04, 1.2954e-04, 2.2997e-05}};

				static double ff_u_reta_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.3378e-03, 1.2054e-03, 9.4829e-04},
				{0., 0., 0., 0., 0., 9.6673e-04, 9.2039e-04, 9.5145e-04},
				{0., 0., 0., 0., 0., 1.1576e-03, 1.2118e-03, 9.5505e-04},
				{0., 0., 0., 0., 0., 1.1617e-03, 1.1850e-03, 8.8815e-04},
				{0., 0., 0., 0., 0., 1.2422e-03, 1.0978e-03, 9.0372e-04},
				{0., 0., 0., 0., 0., 1.2539e-03, 7.7209e-04, 9.8356e-04},
				{0., 0., 0., 0., 0., 1.1591e-03, 9.8109e-04, 9.2605e-04},
				{0., 0., 0., 0., 0., 1.1172e-03, 8.5272e-04, 8.3286e-04}};

				static double ff_u_rphi_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.1340e-03, 1.0917e-03, 8.0004e-04},
				{0., 0., 0., 0., 0., 1.0782e-03, 1.1434e-03, 8.5891e-04},
				{0., 0., 0., 0., 0., 1.0599e-03, 8.7998e-04, 8.8784e-04},
				{0., 0., 0., 0., 0., 1.1895e-03, 8.9106e-04, 8.3668e-04},
				{0., 0., 0., 0., 0., 1.2473e-03, 9.1985e-04, 6.8478e-04},
				{0., 0., 0., 0., 0., 9.6992e-04, 6.5426e-04, 7.4840e-04},
				{0., 0., 0., 0., 0., 1.1100e-03, 8.1293e-04, 7.5410e-04},
				{0., 0., 0., 0., 0., 9.8711e-04, 7.4277e-04, 6.7131e-04}};

				static double ff_u_weta2_15_chisq[8][8] = {{0., 0., 0., 0., 0., -6.9576e-05, -5.7226e-05, -4.1639e-05},
				{0., 0., 0., 0., 0., -6.0845e-05, -6.0290e-05, -4.0737e-05},
				{0., 0., 0., 0., 0., -6.3913e-05, -5.7091e-05, -4.2027e-05},
				{0., 0., 0., 0., 0., -6.8054e-05, -5.9225e-05, -4.5380e-05},
				{0., 0., 0., 0., 0., -7.7345e-05, -6.0091e-05, -4.1380e-05},
				{0., 0., 0., 0., 0., -6.6755e-05, -4.5524e-05, -4.3561e-05},
				{0., 0., 0., 0., 0., -7.0122e-05, -5.2844e-05, -4.5707e-05},
				{0., 0., 0., 0., 0., -6.4584e-05, -5.1283e-05, -3.8967e-05}};

				static double ff_u_f1_15_chisq[8][8] = {{0., 0., 0., 0., 0., -1.2068e-02, -2.0364e-02, -1.0154e-02},
				{0., 0., 0., 0., 0., -1.0724e-02, -1.7759e-02, -1.4017e-02},
				{0., 0., 0., 0., 0., -1.0309e-02, -1.7548e-02, -1.0073e-02},
				{0., 0., 0., 0., 0., -1.5050e-02, -1.8122e-02, -1.1622e-02},
				{0., 0., 0., 0., 0., -1.8466e-02, -1.6895e-02, -9.0859e-03},
				{0., 0., 0., 0., 0., -1.8700e-02, -1.3984e-02, -1.0571e-02},
				{0., 0., 0., 0., 0., -1.8811e-02, -1.5501e-02, -1.0235e-02},
				{0., 0., 0., 0., 0., -1.9701e-02, -1.1076e-02, -7.8567e-03}};

				static double ff_u_deltae_15_chisq[8][8] = {{0., 0., 0., 0., 0., 7.4365e-01, 2.0675e-01, 3.2048e-01},
				{0., 0., 0., 0., 0., 1.6046e+00, -1.1997e+00, 4.9467e-01},
				{0., 0., 0., 0., 0., 3.0224e-01, 4.9665e-01, 8.8761e-01},
				{0., 0., 0., 0., 0., 3.1873e-02, 1.5042e-01, 9.1075e-01},
				{0., 0., 0., 0., 0., 1.3882e-01, -1.0629e-01, 4.9289e-01},
				{0., 0., 0., 0., 0., 1.0224e+00, -3.0910e-01, 7.7707e-01},
				{0., 0., 0., 0., 0., 3.9610e-01, 6.2185e-01, 8.9390e-01},
				{0., 0., 0., 0., 0., -9.9004e-02, 6.3180e-01, 7.6856e-01}};

				static double ff_u_wtot_15_chisq[8][8] = {{0., 0., 0., 0., 0., -4.2951e-02, -2.6567e-02, -1.3371e-02},
				{0., 0., 0., 0., 0., -4.3679e-02, -2.5083e-02, -1.3806e-02},
				{0., 0., 0., 0., 0., -4.2550e-02, -2.3671e-02, -1.1979e-02},
				{0., 0., 0., 0., 0., -3.8559e-02, -2.1262e-02, -1.1662e-02},
				{0., 0., 0., 0., 0., -4.9717e-02, -2.1541e-02, -9.8741e-03},
				{0., 0., 0., 0., 0., -4.1805e-02, -1.6905e-02, -1.0093e-02},
				{0., 0., 0., 0., 0., -3.5046e-02, -1.5449e-02, -9.2221e-03},
				{0., 0., 0., 0., 0., -2.8529e-02, -8.6874e-03, -8.2265e-03}};

				static double ff_u_fside_15_chisq[8][8] = {{0., 0., 0., 0., 0., -9.9795e-03, -6.2318e-03, -3.3035e-03},
				{0., 0., 0., 0., 0., -1.0360e-02, -5.0081e-03, -3.7033e-03},
				{0., 0., 0., 0., 0., -9.0020e-03, -5.7263e-03, -3.2434e-03},
				{0., 0., 0., 0., 0., -9.7377e-03, -5.5358e-03, -3.0827e-03},
				{0., 0., 0., 0., 0., -1.1491e-02, -4.7370e-03, -2.5431e-03},
				{0., 0., 0., 0., 0., -9.9204e-03, -4.2201e-03, -3.0611e-03},
				{0., 0., 0., 0., 0., -9.1519e-03, -4.3369e-03, -2.4221e-03},
				{0., 0., 0., 0., 0., -6.9988e-03, -2.9173e-03, -2.2684e-03}};

				static double ff_u_w1_15_chisq[8][8] = {{0., 0., 0., 0., 0., -5.6945e-03, -5.7007e-03, -2.5872e-03},
				{0., 0., 0., 0., 0., -5.4071e-03, -5.0028e-03, -2.8652e-03},
				{0., 0., 0., 0., 0., -6.3394e-03, -4.6249e-03, -3.0048e-03},
				{0., 0., 0., 0., 0., -7.7157e-03, -6.4869e-03, -3.4347e-03},
				{0., 0., 0., 0., 0., -9.1849e-03, -5.4128e-03, -2.7639e-03},
				{0., 0., 0., 0., 0., -8.5903e-03, -4.5425e-03, -2.2902e-03},
				{0., 0., 0., 0., 0., -7.5592e-03, -4.6237e-03, -2.9338e-03},
				{0., 0., 0., 0., 0., -7.4916e-03, -3.6568e-03, -2.4052e-03}};

				static double ff_u_eratio_15_chisq[8][8] = {{0., 0., 0., 0., 0., 4.1642e-04, -1.0120e-04, 1.5409e-04},
				{0., 0., 0., 0., 0., 6.5441e-04, -8.9830e-05, 3.1700e-04},
				{0., 0., 0., 0., 0., 8.5016e-04, -6.7084e-05, 2.3042e-04},
				{0., 0., 0., 0., 0., 6.4005e-04, -3.1485e-04, 1.4947e-04},
				{0., 0., 0., 0., 0., 3.4124e-04, -5.5158e-04, 2.0760e-04},
				{0., 0., 0., 0., 0., 3.1919e-04, -2.6904e-04, 2.1261e-05},
				{0., 0., 0., 0., 0., 2.3294e-04, -6.2665e-04, 3.3130e-05},
				{0., 0., 0., 0., 0., -1.4708e-04, -5.5435e-04, 9.6417e-06}};

				static double ff_u_e277_15_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				// converted photons

				static double ff_c_rhad_15_chisq[8][8] = {{0., 0., 0., 0., 0., 3.3044e-04, -5.9312e-05, -1.1112e-04},
				{0., 0., 0., 0., 0., -9.9518e-05, 4.2418e-04, -2.2468e-04},
				{0., 0., 0., 0., 0., -4.0554e-06, 5.5606e-04, 1.4575e-04},
				{0., 0., 0., 0., 0., 1.3196e-04, 6.9633e-05, 3.7429e-06},
				{0., 0., 0., 0., 0., 1.7597e-04, -5.6649e-05, 4.7689e-05},
				{0., 0., 0., 0., 0., 3.2735e-05, -6.3348e-05, 1.2469e-05},
				{0., 0., 0., 0., 0., 8.4282e-05, 1.1322e-04, -7.1829e-06},
				{0., 0., 0., 0., 0., 1.3299e-04, 1.0888e-04, 8.9996e-06}};

				static double ff_c_reta_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.6407e-03, 1.8122e-03, 1.5044e-03},
				{0., 0., 0., 0., 0., 1.5912e-03, 1.7392e-03, 1.4152e-03},
				{0., 0., 0., 0., 0., 1.3346e-03, 1.6573e-03, 9.3582e-04},
				{0., 0., 0., 0., 0., 1.8956e-03, 1.2343e-03, 1.2842e-03},
				{0., 0., 0., 0., 0., 2.0811e-03, 1.6226e-03, 1.0140e-03},
				{0., 0., 0., 0., 0., 1.5864e-03, 1.5500e-03, 1.1929e-03},
				{0., 0., 0., 0., 0., 1.7068e-03, 1.5347e-03, 1.0897e-03},
				{0., 0., 0., 0., 0., 1.4710e-03, 1.2703e-03, 9.1439e-04}};

				static double ff_c_rphi_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.5723e-03, 2.2311e-03, 1.2038e-03},
				{0., 0., 0., 0., 0., 2.1634e-03, 2.1281e-03, 1.0551e-03},
				{0., 0., 0., 0., 0., 1.1107e-03, 1.4531e-03, 1.0349e-03},
				{0., 0., 0., 0., 0., 1.3939e-03, 1.7611e-03, 1.3600e-03},
				{0., 0., 0., 0., 0., 1.4577e-03, 2.2160e-03, 9.2056e-04},
				{0., 0., 0., 0., 0., 1.1330e-03, 1.2991e-03, 1.1203e-03},
				{0., 0., 0., 0., 0., 1.5696e-03, 1.6475e-03, 8.9314e-04},
				{0., 0., 0., 0., 0., 1.2519e-03, 1.2445e-03, 7.8023e-04}};

				static double ff_c_weta2_15_chisq[8][8] = {{0., 0., 0., 0., 0., -1.1078e-04, -9.1385e-05, -5.5786e-05},
				{0., 0., 0., 0., 0., -6.7299e-05, -8.6846e-05, -6.0930e-05},
				{0., 0., 0., 0., 0., -7.6403e-05, -7.4295e-05, -4.8043e-05},
				{0., 0., 0., 0., 0., -7.6364e-05, -7.6004e-05, -5.7415e-05},
				{0., 0., 0., 0., 0., -9.2090e-05, -1.0810e-04, -4.5237e-05},
				{0., 0., 0., 0., 0., -6.9388e-05, -7.7930e-05, -5.4358e-05},
				{0., 0., 0., 0., 0., -8.8796e-05, -8.9363e-05, -5.4244e-05},
				{0., 0., 0., 0., 0., -8.2483e-05, -7.7040e-05, -3.9031e-05}};

				static double ff_c_f1_15_chisq[8][8] = {{0., 0., 0., 0., 0., -6.5585e-03, -2.0565e-02, -1.4507e-02},
				{0., 0., 0., 0., 0., -6.0546e-03, -2.4661e-02, -1.4165e-02},
				{0., 0., 0., 0., 0., -4.1313e-03, -2.0542e-02, -1.3567e-02},
				{0., 0., 0., 0., 0., -5.8294e-03, -1.9693e-02, -1.4665e-02},
				{0., 0., 0., 0., 0., -8.7290e-03, -2.2868e-02, -1.2762e-02},
				{0., 0., 0., 0., 0., -7.5809e-03, -2.1915e-02, -1.5703e-02},
				{0., 0., 0., 0., 0., -1.1005e-02, -2.1700e-02, -1.4976e-02},
				{0., 0., 0., 0., 0., -1.5071e-02, -1.9393e-02, -1.2880e-02}};

				static double ff_c_deltae_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.4187e-01, 1.3592e+00, 3.9047e-01},
				{0., 0., 0., 0., 0., 3.8926e-02, 7.0792e-01, 8.7990e-01},
				{0., 0., 0., 0., 0., 5.6572e-02, 1.7285e+00, 6.3969e-01},
				{0., 0., 0., 0., 0., 2.8142e-01, -1.8052e-01, 1.1593e+00},
				{0., 0., 0., 0., 0., -4.7654e-01, 5.5427e-01, 8.8217e-01},
				{0., 0., 0., 0., 0., 2.9206e-01, -8.9187e-01, 8.7370e-01},
				{0., 0., 0., 0., 0., -2.7178e-02, 1.0259e-01, 1.4127e+00},
				{0., 0., 0., 0., 0., -3.0668e-03, 4.7856e-01, 1.0601e+00}};

				static double ff_c_wtot_15_chisq[8][8] = {{0., 0., 0., 0., 0., -6.4054e-02, -4.7946e-02, -1.8740e-02},
				{0., 0., 0., 0., 0., -5.8832e-02, -4.9635e-02, -1.5779e-02},
				{0., 0., 0., 0., 0., -4.8029e-02, -4.2004e-02, -1.6173e-02},
				{0., 0., 0., 0., 0., -6.4545e-02, -4.2641e-02, -1.4567e-02},
				{0., 0., 0., 0., 0., -6.4949e-02, -4.0627e-02, -1.4633e-02},
				{0., 0., 0., 0., 0., -6.1236e-02, -3.5529e-02, -1.5070e-02},
				{0., 0., 0., 0., 0., -6.2889e-02, -3.4732e-02, -1.4990e-02},
				{0., 0., 0., 0., 0., -4.9298e-02, -2.8234e-02, -1.1374e-02}};

				static double ff_c_fside_15_chisq[8][8] = {{0., 0., 0., 0., 0., -1.5137e-02, -1.1378e-02, -5.4662e-03},
				{0., 0., 0., 0., 0., -1.4155e-02, -1.1498e-02, -4.6347e-03},
				{0., 0., 0., 0., 0., -1.2728e-02, -1.0831e-02, -5.1127e-03},
				{0., 0., 0., 0., 0., -1.4850e-02, -1.0119e-02, -4.7591e-03},
				{0., 0., 0., 0., 0., -1.4972e-02, -9.7353e-03, -4.4166e-03},
				{0., 0., 0., 0., 0., -1.3947e-02, -8.8386e-03, -4.6176e-03},
				{0., 0., 0., 0., 0., -1.3689e-02, -8.6608e-03, -4.3942e-03},
				{0., 0., 0., 0., 0., -1.2059e-02, -7.2365e-03, -3.3847e-03}};

				static double ff_c_w1_15_chisq[8][8] = {{0., 0., 0., 0., 0., -5.2498e-03, -1.0089e-02, -4.8375e-03},
				{0., 0., 0., 0., 0., -5.4219e-03, -9.0471e-03, -5.3408e-03},
				{0., 0., 0., 0., 0., -6.0154e-03, -6.7939e-03, -4.1001e-03},
				{0., 0., 0., 0., 0., -6.3337e-03, -7.6718e-03, -4.3864e-03},
				{0., 0., 0., 0., 0., -6.9483e-03, -7.1788e-03, -4.7724e-03},
				{0., 0., 0., 0., 0., -6.6572e-03, -6.0012e-03, -4.1698e-03},
				{0., 0., 0., 0., 0., -7.2051e-03, -6.6926e-03, -4.0761e-03},
				{0., 0., 0., 0., 0., -7.3561e-03, -6.6510e-03, -3.4028e-03}};

				static double ff_c_eratio_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.0630e-03, 5.9929e-04, 1.9955e-04},
				{0., 0., 0., 0., 0., 1.5395e-03, 1.2417e-04, 3.4440e-04},
				{0., 0., 0., 0., 0., 7.4067e-04, 9.0489e-05, 2.4095e-04},
				{0., 0., 0., 0., 0., 1.1716e-03, 9.4002e-05, 1.8599e-04},
				{0., 0., 0., 0., 0., 1.2053e-03, 2.1601e-04, 1.5332e-04},
				{0., 0., 0., 0., 0., 1.0628e-03, -4.2071e-04, 2.0407e-04},
				{0., 0., 0., 0., 0., 8.3647e-04, -2.8719e-04, 6.4605e-05},
				{0., 0., 0., 0., 0., 3.0810e-04, -2.5803e-04, 3.3165e-05}};

				static double ff_c_e277_15_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};


				static double ff_u_rhad_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.8562e-18, 2.5611e-18, 1.5160e-05},
				{0., 0., 0., 0., 0., 1.1261e-05, 1.6105e-05, 1.5218e-05},
				{0., 0., 0., 0., 0., 8.4317e-06, 9.6593e-06, 1.0574e-05},
				{0., 0., 0., 0., 0., 1.1253e-18, 1.0710e-05, 1.3662e-05},
				{0., 0., 0., 0., 0., 2.0202e-18, 9.3287e-06, 9.4812e-06},
				{0., 0., 0., 0., 0., 1.9624e-18, 8.8236e-06, 3.7831e-19},
				{0., 0., 0., 0., 0., 7.8801e-06, 6.2978e-19, 7.2080e-06},
				{0., 0., 0., 0., 0., 1.6986e-05, 1.4770e-05, 1.8141e-05}};

				static double ff_u_reta_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 4.8416e-18, 1.8203e-05, 1.4267e-05},
				{0., 0., 0., 0., 0., 2.4008e-05, 1.5887e-05, 2.5199e-05},
				{0., 0., 0., 0., 0., 2.5289e-05, 2.0295e-05, 1.1177e-05},
				{0., 0., 0., 0., 0., 1.6547e-05, 9.3777e-06, 1.4732e-05},
				{0., 0., 0., 0., 0., 1.9929e-05, 6.0266e-06, 8.1090e-06},
				{0., 0., 0., 0., 0., 2.3099e-05, 8.9278e-06, 9.9515e-06},
				{0., 0., 0., 0., 0., 1.9492e-05, 2.9420e-06, 4.7270e-06},
				{0., 0., 0., 0., 0., 1.7901e-05, 6.9858e-06, 1.0118e-05}};

				static double ff_u_rphi_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 4.1119e-05, 2.2010e-05, 2.3613e-05},
				{0., 0., 0., 0., 0., 4.3431e-05, 1.7672e-05, 2.4264e-05},
				{0., 0., 0., 0., 0., 4.5966e-05, 1.9046e-05, 1.5649e-05},
				{0., 0., 0., 0., 0., 3.2251e-05, 2.7272e-05, 2.4088e-05},
				{0., 0., 0., 0., 0., 3.6768e-05, 3.6501e-05, 2.5460e-05},
				{0., 0., 0., 0., 0., 4.5078e-05, 2.5359e-05, 2.1780e-05},
				{0., 0., 0., 0., 0., 5.3970e-05, 2.6107e-05, 2.3219e-05},
				{0., 0., 0., 0., 0., 5.6659e-05, 2.4747e-05, 2.2977e-05}};

				static double ff_u_weta2_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.3325e-19, 4.5787e-07, 6.1072e-07},
				{0., 0., 0., 0., 0., 9.7783e-20, 2.5479e-19, 7.5279e-07},
				{0., 0., 0., 0., 0., 1.3591e-19, 2.2644e-19, 5.9120e-07},
				{0., 0., 0., 0., 0., 1.6843e-19, 2.9567e-19, 5.9938e-07},
				{0., 0., 0., 0., 0., 1.5983e-19, 7.1660e-07, 8.6783e-07},
				{0., 0., 0., 0., 0., 1.4631e-19, 3.8165e-07, 6.3381e-07},
				{0., 0., 0., 0., 0., 1.5195e-19, 6.6892e-07, 8.8558e-07},
				{0., 0., 0., 0., 0., 9.6644e-07, 2.2921e-07, 6.8461e-07}};

				static double ff_u_f1_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.3260e-17, 2.9312e-17, 1.9901e-17},
				{0., 0., 0., 0., 0., 1.8255e-17, 2.4341e-17, 5.9382e-05},
				{0., 0., 0., 0., 0., 3.5730e-17, 2.1430e-17, 5.8263e-05},
				{0., 0., 0., 0., 0., 2.7237e-17, 2.6641e-17, 1.5190e-17},
				{0., 0., 0., 0., 0., 3.4749e-17, 3.0824e-17, 1.4550e-17},
				{0., 0., 0., 0., 0., 8.4710e-18, 1.2286e-17, 2.0820e-17},
				{0., 0., 0., 0., 0., 2.2024e-17, 6.8396e-05, 2.3541e-17},
				{0., 0., 0., 0., 0., 2.0966e-17, 2.1859e-17, 6.4790e-05}};

				static double ff_u_deltae_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 3.9679e-14, 1.0554e-13},
				{0., 0., 0., 0., 0., 0., 4.2185e-15, 2.1830e-13},
				{0., 0., 0., 0., 0., 2.7903e-14, 4.3583e-14, 2.4395e-13},
				{0., 0., 0., 0., 0., 4.0025e-14, 1.4690e-13, 4.5263e-14},
				{0., 0., 0., 0., 0., 2.3866e-14, 2.7445e-14, 1.4621e-13},
				{0., 0., 0., 0., 0., 7.0106e-14, 0., 9.6149e-15},
				{0., 0., 0., 0., 0., 3.0331e-14, 1.0995e-13, 1.1603e-13},
				{0., 0., 0., 0., 0., 3.4871e-14, 3.1212e-14, 1.4306e-13}};

				static double ff_u_wtot_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.0093e-16, 6.8272e-04, 4.1288e-04},
				{0., 0., 0., 0., 0., 1.6957e-16, 5.7727e-04, 5.9253e-04},
				{0., 0., 0., 0., 0., 1.5352e-16, 3.9146e-04, 5.4180e-04},
				{0., 0., 0., 0., 0., 1.4389e-16, 4.6676e-04, 4.5321e-04},
				{0., 0., 0., 0., 0., 6.5845e-04, 3.9581e-04, 5.0297e-04},
				{0., 0., 0., 0., 0., 6.2111e-04, 5.1652e-04, 6.1458e-04},
				{0., 0., 0., 0., 0., 7.7914e-04, 4.3966e-04, 6.0263e-04},
				{0., 0., 0., 0., 0., 8.3881e-04, 7.1813e-04, 6.5825e-04}};

				static double ff_u_fside_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.2052e-17, 1.0746e-05, 4.9481e-05},
				{0., 0., 0., 0., 0., 2.5460e-17, 6.5022e-06, 9.2168e-05},
				{0., 0., 0., 0., 0., 2.6568e-17, 2.4960e-05, 6.4088e-05},
				{0., 0., 0., 0., 0., 2.4539e-17, 4.7243e-05, 6.1801e-05},
				{0., 0., 0., 0., 0., 1.8388e-04, 3.3465e-05, 9.3919e-05},
				{0., 0., 0., 0., 0., 1.7447e-04, 2.9090e-05, 1.0991e-04},
				{0., 0., 0., 0., 0., 2.7519e-17, 5.9040e-05, 1.3226e-04},
				{0., 0., 0., 0., 0., 2.5499e-17, 1.1283e-04, 1.5430e-04}};

				static double ff_u_w1_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 6.0578e-05, 3.8573e-05, 3.9049e-05},
				{0., 0., 0., 0., 0., 5.8385e-05, 4.7328e-05, 8.3009e-05},
				{0., 0., 0., 0., 0., 5.9376e-05, 4.2234e-05, 6.6916e-05},
				{0., 0., 0., 0., 0., 8.0285e-05, 7.0460e-05, 4.4825e-05},
				{0., 0., 0., 0., 0., 6.8345e-05, 2.6487e-05, 2.8913e-05},
				{0., 0., 0., 0., 0., 3.9006e-05, 4.8112e-05, 2.8175e-05},
				{0., 0., 0., 0., 0., 4.4652e-05, 4.1877e-05, 3.8544e-05},
				{0., 0., 0., 0., 0., 3.6038e-05, 4.0859e-05, 2.3051e-05}};

				static double ff_u_eratio_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.7064e-04, 3.1884e-04, 3.0012e-04},
				{0., 0., 0., 0., 0., 2.9709e-04, 2.8624e-04, 2.8480e-04},
				{0., 0., 0., 0., 0., 2.5901e-04, 2.9533e-04, 2.5914e-04},
				{0., 0., 0., 0., 0., 2.4435e-04, 2.5205e-04, 2.2156e-04},
				{0., 0., 0., 0., 0., 2.3085e-04, 2.2266e-04, 1.9861e-04},
				{0., 0., 0., 0., 0., 1.9099e-04, 2.0176e-04, 1.8163e-04},
				{0., 0., 0., 0., 0., 1.1789e-04, 1.4912e-04, 1.3774e-04},
				{0., 0., 0., 0., 0., 1.7868e-05, 6.2447e-05, 7.8750e-05}};

				static double ff_u_e277_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};

				static double ff_c_rhad_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.8155e-18, 1.2138e-05, 1.2842e-05},
				{0., 0., 0., 0., 0., 1.1096e-05, 1.1014e-05, 1.5858e-05},
				{0., 0., 0., 0., 0., 1.0375e-05, 1.2705e-05, 1.0326e-05},
				{0., 0., 0., 0., 0., 1.1368e-18, 9.0680e-06, 1.0307e-05},
				{0., 0., 0., 0., 0., 1.3542e-18, 8.2061e-06, 7.0788e-06},
				{0., 0., 0., 0., 0., 8.5784e-06, 9.8335e-06, 3.3306e-19},
				{0., 0., 0., 0., 0., 6.8388e-06, 4.1423e-19, 7.4539e-06},
				{0., 0., 0., 0., 0., 1.8900e-05, 1.5570e-05, 1.8082e-05}};

				static double ff_c_reta_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 4.0822e-18, 2.0060e-05, 5.1195e-18},
				{0., 0., 0., 0., 0., 4.4060e-18, 2.6062e-05, 1.5842e-05},
				{0., 0., 0., 0., 0., 4.0394e-18, 1.1060e-05, 2.0396e-05},
				{0., 0., 0., 0., 0., 4.1289e-05, 1.1802e-05, 1.4789e-05},
				{0., 0., 0., 0., 0., 3.3149e-05, 4.1730e-06, 1.6285e-05},
				{0., 0., 0., 0., 0., 2.8121e-05, 1.1953e-05, 2.4248e-05},
				{0., 0., 0., 0., 0., 2.4242e-05, 4.0969e-06, 9.0116e-06},
				{0., 0., 0., 0., 0., 1.9363e-05, 8.7477e-06, 2.2108e-06}};

				static double ff_c_rphi_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.0827e-17, 3.0570e-04, 2.9093e-04},
				{0., 0., 0., 0., 0., 2.1874e-17, 3.0802e-04, 1.9221e-04},
				{0., 0., 0., 0., 0., 2.3730e-04, 3.4274e-04, 2.2342e-04},
				{0., 0., 0., 0., 0., 2.5838e-04, 3.1240e-04, 2.0287e-04},
				{0., 0., 0., 0., 0., 2.7320e-04, 2.8567e-04, 1.9546e-04},
				{0., 0., 0., 0., 0., 2.6842e-04, 2.6945e-04, 1.7235e-04},
				{0., 0., 0., 0., 0., 2.1338e-04, 2.3171e-04, 1.3305e-04},
				{0., 0., 0., 0., 0., 1.7788e-04, 1.0569e-04, 7.7491e-05}};

				static double ff_c_weta2_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.6087e-19, 8.5301e-07, 2.2674e-19},
				{0., 0., 0., 0., 0., 1.4981e-19, 5.0724e-07, 2.3905e-19},
				{0., 0., 0., 0., 0., 2.1915e-19, 2.3321e-19, 4.0627e-07},
				{0., 0., 0., 0., 0., 1.6411e-19, 2.5801e-19, 2.2325e-19},
				{0., 0., 0., 0., 0., 1.3206e-19, 2.4950e-19, 1.8226e-19},
				{0., 0., 0., 0., 0., 1.1505e-19, 2.6361e-19, 2.0933e-19},
				{0., 0., 0., 0., 0., 1.1742e-19, 4.4715e-07, 3.9940e-07},
				{0., 0., 0., 0., 0., 1.5516e-19, 4.7623e-07, 4.3753e-07}};

				static double ff_c_f1_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.9665e-17, 4.6671e-05, 1.9127e-17},
				{0., 0., 0., 0., 0., 2.5011e-17, 6.5879e-05, 1.3363e-17},
				{0., 0., 0., 0., 0., 2.3150e-17, 5.7475e-05, 2.1597e-17},
				{0., 0., 0., 0., 0., 2.4939e-17, 5.2780e-05, 1.0953e-17},
				{0., 0., 0., 0., 0., 2.0757e-17, 8.6868e-05, 1.9236e-17},
				{0., 0., 0., 0., 0., 1.7633e-17, 1.8498e-17, 4.7422e-05},
				{0., 0., 0., 0., 0., 1.9583e-17, 1.4666e-17, 5.1611e-05},
				{0., 0., 0., 0., 0., 1.1511e-04, 2.0927e-17, 6.8542e-05}};

				static double ff_c_deltae_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.1190e-14, 4.8496e-14, 2.3740e-14},
				{0., 0., 0., 0., 0., 8.5583e-14, 7.8197e-15, 1.4172e-14},
				{0., 0., 0., 0., 0., 1.2355e-13, 1.8954e-14, 5.5770e-13},
				{0., 0., 0., 0., 0., 6.7015e-14, 1.4965e-14, 1.9913e-13},
				{0., 0., 0., 0., 0., 1.3819e-14, 6.2841e-14, 3.1975e-13},
				{0., 0., 0., 0., 0., 4.7535e-14, 7.5547e-15, 5.7386e-15},
				{0., 0., 0., 0., 0., 6.9466e-15, 2.6084e-14, 2.8519e-12},
				{0., 0., 0., 0., 0., 4.3548e-14, 9.6962e-14, 1.1523e-11}};

				static double ff_c_wtot_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.9468e-16, 2.0606e-04, 3.4360e-04},
				{0., 0., 0., 0., 0., 2.0353e-16, 4.4424e-04, 3.8077e-04},
				{0., 0., 0., 0., 0., 2.0274e-16, 2.6088e-04, 4.1328e-04},
				{0., 0., 0., 0., 0., 2.2366e-16, 2.2827e-04, 4.6736e-04},
				{0., 0., 0., 0., 0., 2.3004e-16, 3.2584e-04, 4.6793e-04},
				{0., 0., 0., 0., 0., 7.7737e-04, 2.7614e-04, 4.6943e-04},
				{0., 0., 0., 0., 0., 7.1140e-04, 2.2905e-04, 5.6490e-04},
				{0., 0., 0., 0., 0., 1.0833e-03, 2.9062e-04, 6.5909e-04}};

				static double ff_c_fside_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.1850e-04, 1.0437e-04, 1.0161e-04},
				{0., 0., 0., 0., 0., 2.7470e-04, 1.3440e-04, 9.6395e-05},
				{0., 0., 0., 0., 0., 2.2451e-17, 4.3024e-05, 1.1277e-04},
				{0., 0., 0., 0., 0., 2.4745e-17, 6.9993e-05, 1.2001e-04},
				{0., 0., 0., 0., 0., 2.7707e-17, 9.7595e-05, 1.2602e-04},
				{0., 0., 0., 0., 0., 2.5451e-17, 8.7640e-05, 1.2822e-04},
				{0., 0., 0., 0., 0., 2.5912e-04, 7.8857e-05, 1.5073e-04},
				{0., 0., 0., 0., 0., 2.7735e-17, 8.3187e-05, 1.5293e-04}};

				static double ff_c_w1_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 1.5817e-18, 3.8255e-05, 6.1374e-05},
				{0., 0., 0., 0., 0., 7.6946e-19, 2.1295e-05, 4.3547e-05},
				{0., 0., 0., 0., 0., 7.5969e-19, 5.4525e-05, 6.0411e-05},
				{0., 0., 0., 0., 0., 5.3500e-05, 4.5421e-05, 7.5249e-05},
				{0., 0., 0., 0., 0., 4.9941e-05, 5.9264e-05, 4.5467e-05},
				{0., 0., 0., 0., 0., 3.9762e-05, 8.2235e-05, 7.0607e-05},
				{0., 0., 0., 0., 0., 5.2796e-05, 8.3558e-05, 4.4608e-05},
				{0., 0., 0., 0., 0., 5.0875e-05, 2.7045e-05, 5.5528e-05}};

				static double ff_c_eratio_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 2.6819e-04, 2.3565e-04, 2.2583e-04},
				{0., 0., 0., 0., 0., 2.5772e-04, 2.4297e-04, 2.1022e-04},
				{0., 0., 0., 0., 0., 3.1203e-04, 1.8881e-04, 1.5903e-04},
				{0., 0., 0., 0., 0., 3.0521e-04, 1.3642e-04, 1.1581e-04},
				{0., 0., 0., 0., 0., 2.9369e-04, 9.5882e-05, 8.1972e-05},
				{0., 0., 0., 0., 0., 2.4406e-04, 3.7551e-05, 5.3807e-05},
				{0., 0., 0., 0., 0., 1.7517e-04, 1.6116e-05, 3.0064e-05},
				{0., 0., 0., 0., 0., 2.4687e-05, 5.2898e-07, 1.0227e-05}};

				static double ff_c_e277_err_15_chisq[8][8] = {{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.},
				{0., 0., 0., 0., 0., 0., 0., 0.}};


				if (isConv) {

				m_rhad_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_rhad_15_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_rhad_15_chisq[0][0], 0.);
				m_reta_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_reta_15_chisq[0][0], 0.);
				m_rphi_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_rphi_15_chisq[0][0], 0.);
				m_weta2_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_weta2_15_chisq[0][0], 0.);
				m_f1_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_f1_15_chisq[0][0], 0.);
				m_deltae_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_deltae_15_chisq[0][0], 0.);
				m_wtot_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_wtot_15_chisq[0][0], 0.);
				m_fside_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_fside_15_chisq[0][0], 0.);
				m_w1_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_w1_15_chisq[0][0], 0.);
				m_eratio_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_eratio_15_chisq[0][0], 0.);
				m_e277_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_e277_15_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_rhad_err_15_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_rhad_err_15_chisq[0][0], 0.);
				m_reta_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_reta_err_15_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_rphi_err_15_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_weta2_err_15_chisq[0][0], 0.);
				m_f1_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_f1_err_15_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_deltae_err_15_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_wtot_err_15_chisq[0][0], 0.);
				m_fside_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_fside_err_15_chisq[0][0], 0.);
				m_w1_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_w1_err_15_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_eratio_err_15_chisq[0][0], 0.);
				m_e277_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_c_e277_err_15_chisq[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_rhad_15_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_rhad_15_chisq[0][0], 0.);
				m_reta_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_reta_15_chisq[0][0], 0.);
				m_rphi_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_rphi_15_chisq[0][0], 0.);
				m_weta2_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_weta2_15_chisq[0][0], 0.);
				m_f1_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_f1_15_chisq[0][0], 0.);
				m_deltae_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_deltae_15_chisq[0][0], 0.);
				m_wtot_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_wtot_15_chisq[0][0], 0.);
				m_fside_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_fside_15_chisq[0][0], 0.);
				m_w1_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_w1_15_chisq[0][0], 0.);
				m_eratio_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_eratio_15_chisq[0][0], 0.);
				m_e277_ff.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_e277_15_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_rhad_err_15_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_rhad_err_15_chisq[0][0], 0.);
				m_reta_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_reta_err_15_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_rphi_err_15_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_weta2_err_15_chisq[0][0], 0.);
				m_f1_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_f1_err_15_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_deltae_err_15_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_wtot_err_15_chisq[0][0], 0.);
				m_fside_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_fside_err_15_chisq[0][0], 0.);
				m_w1_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_w1_err_15_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_eratio_err_15_chisq[0][0], 0.);
				m_e277_fferr.Initialize(8, 8, pt_DVs_15_chisq, eta_DVs_15_chisq, &ff_u_e277_err_15_chisq[0][0], 0.);

				}

				break;

		case 16: // tune 16 -- sum of tune 14 (GEO20->DATA12) and tune 15 (GEO21->GEO20)

				static double pt_DVs_16_chisq[10] = {15000, 20000, 25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_16_chisq[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				// unconverted photons

				static double ff_u_rhad_16_chisq[10][8]=
				{{ 3.019900e-04,  1.066000e-04,  1.499420e-03,  1.041820e-03,  9.990000e+02,  9.478400e-04,  1.534824e-03,  7.909690e-04},
				{ 1.874340e-04,  3.507410e-04,  8.030420e-04,  5.812780e-04,  9.990000e+02, -2.021590e-04, -2.756160e-04,  3.259150e-04},
				{ 7.981470e-05,  4.897180e-04,  8.627990e-04,  4.201260e-04,  9.990000e+02, -4.871290e-04,  8.522920e-04, -1.631830e-04},
				{ 1.013160e-04, -5.192840e-05,  5.086700e-04,  2.485320e-04,  9.990000e+02, -9.151720e-04, -1.251880e-03,  7.936350e-04},
				{ 3.920300e-05, -3.652300e-05,  3.954000e-04, -1.243000e-05,  9.990000e+02,  1.142143e-03, -6.784400e-04, -6.458440e-04},
				{-1.125300e-04,  4.647300e-06,  3.821400e-04,  1.347300e-04,  9.990000e+02,  3.840670e-04,  1.768204e-03, -3.174840e-05},
				{-4.091600e-05, -2.509500e-04,  2.734100e-04,  9.786900e-05,  9.990000e+02,  1.181590e-03,  3.221620e-04,  1.257030e-04},
				{-1.074500e-04, -6.318900e-06,  2.545900e-04,  2.310600e-04,  9.990000e+02,  8.141300e-04,  1.278349e-04, -2.496890e-05},
				{-1.551100e-04, -3.669800e-04,  1.806400e-04, -1.809100e-04,  9.990000e+02,  9.062960e-05, -8.844400e-04, -1.464480e-04},
				{-3.631100e-04, -1.320600e-05,  2.337200e-05,  7.126700e-06,  9.990000e+02, -1.147100e-04,  4.500600e-04, -4.687000e-05}};

				static double ff_u_reta_16_chisq[10][8]=
				{{-3.144930e-03, -5.402240e-03, -6.554780e-03, -6.395580e-03,  9.990000e+02, -6.412980e-03, -7.004050e-03, -7.621050e-03},
				{-2.583850e-03, -3.929570e-03, -5.703660e-03, -5.085160e-03,  9.990000e+02, -5.812290e-03, -5.708890e-03, -6.610030e-03},
				{-2.677260e-03, -3.588300e-03, -4.642080e-03, -5.552430e-03,  9.990000e+02, -4.833370e-03, -4.084380e-03, -6.790760e-03},
				{-2.344460e-03, -2.023490e-03, -4.294390e-03, -6.085800e-03,  9.990000e+02, -4.625270e-03, -5.114670e-03, -7.184860e-03},
				{-9.853600e-04, -3.956000e-03, -5.417300e-03, -5.848900e-03,  9.990000e+02, -4.646800e-03, -6.727900e-03, -8.715150e-03},
				{-1.612300e-03, -1.885200e-03, -3.435100e-03, -5.343900e-03,  9.990000e+02, -5.503000e-03, -5.197000e-03, -8.335050e-03},
				{-7.824700e-04, -7.493200e-04, -3.875600e-03, -4.177300e-03,  9.990000e+02, -3.924900e-03, -5.431300e-03, -8.804680e-03},
				{-1.684800e-03, -2.231600e-03, -6.061300e-03, -4.957700e-03,  9.990000e+02, -3.700400e-03, -7.099910e-03, -6.842140e-03},
				{-1.058600e-03, -2.314000e-03, -4.476100e-03, -4.348700e-03,  9.990000e+02, -3.898300e-03, -3.710010e-03, -7.576550e-03},
				{-1.465700e-03, -1.320100e-03, -4.655400e-03, -4.690200e-03,  9.990000e+02, -2.901100e-03, -5.072280e-03, -6.571940e-03}};

				static double ff_u_rphi_16_chisq[10][8]=
				{{ 5.006220e-04, -2.286910e-03,  2.303110e-04, -1.568210e-03,  9.990000e+02,  1.278753e-03,  2.395560e-03,  2.350960e-03},
				{ 7.400260e-04, -1.046910e-03,  8.099420e-04, -1.713690e-03,  9.990000e+02,  1.611230e-03,  2.711310e-03,  3.047560e-03},
				{ 6.598890e-04,  3.895660e-04,  2.302570e-03, -8.364130e-04,  9.990000e+02,  2.765080e-03,  5.061390e-03,  2.680500e-03},
				{ 9.805340e-04,  9.571520e-04,  2.521490e-03, -1.512620e-03,  9.990000e+02,  2.011973e-03,  3.674570e-03,  1.667763e-03},
				{ 2.400400e-04, -1.697600e-03,  1.458700e-03,  1.330800e-03,  9.990000e+02,  1.098973e-03,  2.875680e-03,  1.704000e-05},
				{ 1.726200e-03,  1.912400e-03,  1.942800e-03, -7.374900e-04,  9.990000e+02,  1.900170e-03,  2.725160e-03,  8.944440e-04},
				{ 1.821700e-03, -9.855900e-04,  3.247200e-03,  3.367200e-03,  9.990000e+02,  3.519100e-03,  1.565930e-03,  9.671600e-04},
				{ 1.103400e-03,  2.381100e-03,  5.805000e-04, -2.605200e-04,  9.990000e+02,  2.878920e-03,  2.919760e-03,  1.875600e-03},
				{ 2.043300e-03, -8.466600e-04,  2.911200e-04, -2.060100e-05,  9.990000e+02,  3.142500e-03,  4.517030e-03,  2.086000e-03},
				{ 1.481500e-03,  1.077600e-03,  1.166300e-03,  2.801300e-04,  9.990000e+02,  3.212910e-03,  2.969070e-03,  3.082610e-03}};

				static double ff_u_weta2_16_chisq[10][8]=
				{{ 1.377970e-04,  2.054210e-04,  9.640480e-05,  1.287720e-04,  9.990000e+02,  3.063380e-04,  3.668230e-04,  3.422800e-04},
				{ 1.106810e-04,  1.710130e-04,  7.147090e-05,  1.402530e-04,  9.990000e+02,  3.197160e-04,  3.370940e-04,  3.211480e-04},
				{ 1.272070e-04,  1.440930e-04,  3.136360e-05,  1.136210e-04,  9.990000e+02,  2.836820e-04,  2.875930e-04,  3.621100e-04},
				{ 9.842820e-05,  1.541040e-04,  5.407990e-05,  8.488500e-05,  9.990000e+02,  3.226290e-04,  3.419810e-04,  3.677280e-04},
				{ 1.377800e-04,  2.145200e-04,  2.073600e-05,  9.706400e-05,  9.990000e+02,  1.973370e-04,  4.378090e-04,  4.161730e-04},
				{ 7.743800e-05,  8.315100e-05,  6.624400e-05,  7.146400e-05,  9.990000e+02,  2.483360e-04,  3.104150e-04,  3.578000e-04},
				{ 9.068800e-05,  1.208600e-04,  8.464900e-06, -2.776400e-05,  9.990000e+02,  2.597750e-04,  3.398190e-04,  4.322900e-04},
				{ 4.300100e-05,  1.002500e-04,  1.121800e-04,  6.741600e-05,  9.990000e+02,  1.758150e-04,  2.997560e-04,  2.972190e-04},
				{ 5.920200e-05,  2.098000e-04,  4.767000e-05,  1.034700e-04,  9.990000e+02,  2.227080e-04,  2.687760e-04,  3.249630e-04},
				{ 1.113400e-04,  1.463900e-04,  6.551100e-05,  8.786800e-05,  9.990000e+02,  2.969260e-04,  3.038270e-04,  3.653930e-04}};

				static double ff_u_f1_16_chisq[10][8]=
				{{-5.042010e-03, -2.917760e-03, -2.352370e-03, -7.168890e-03,  9.990000e+02,  2.071000e-03,  5.988000e-03,  3.617000e-03},
				{-6.962700e-03, -4.154450e-03, -3.585660e-03, -2.612100e-03,  9.990000e+02,  4.017700e-03, -4.247900e-03, -1.217310e-03},
				{-1.058100e-03, -3.525620e-03, -3.503890e-03, -4.438550e-03,  9.990000e+02,  6.346000e-04, -1.322622e-02,  4.287100e-03},
				{-1.885370e-03,  1.397170e-03, -5.058110e-03, -6.266700e-03,  9.990000e+02,  1.060700e-02,  2.167400e-03,  8.672200e-03},
				{ 3.452100e-03,  1.659900e-04, -8.405100e-03, -6.722300e-03,  9.990000e+02, -9.985990e-03,  2.247600e-02,  2.298600e-02},
				{-2.913900e-03, -1.040000e-02, -1.138400e-02, -4.883500e-03,  9.990000e+02, -3.751000e-03,  1.069000e-03,  1.269700e-02},
				{-2.965600e-03, -2.525100e-02, -1.032600e-02, -1.466400e-02,  9.990000e+02, -6.730000e-03, -4.803000e-03,  2.195410e-02},
				{-6.940700e-03, -2.230200e-02,  8.137100e-03, -1.665200e-03,  9.990000e+02, -2.806420e-02,  1.340600e-02,  1.100000e-05},
				{-6.414500e-04,  9.487100e-03, -2.756100e-03, -5.043800e-03,  9.990000e+02, -3.121000e-02, -1.050020e-02, -1.172770e-02},
				{ 7.845000e-03,  1.161500e-03,  2.069100e-03, -1.895900e-03,  9.990000e+02, -1.731950e-02, -1.381890e-02,  4.316300e-03}};

				static double ff_u_wtot_16_chisq[10][8]=
				{{ 1.005980e-01,  1.153640e-01,  1.049840e-01,  1.265120e-01,  9.990000e+02,  1.303250e-01,  1.659700e-01,  1.170870e-01},
				{ 7.109680e-02,  1.142850e-01,  1.086510e-01,  1.312640e-01,  9.990000e+02,  1.034410e-01,  1.261930e-01,  9.618400e-02},
				{ 6.164610e-02,  9.234110e-02,  1.131740e-01,  1.110190e-01,  9.990000e+02,  7.938900e-02,  1.141240e-01,  1.149590e-01},
				{ 8.354390e-02,  6.532110e-02,  9.884640e-02,  1.142730e-01,  9.990000e+02,  1.050670e-01,  1.462050e-01,  1.148760e-01},
				{ 4.013200e-02,  8.859800e-02,  8.487300e-02,  1.017000e-01,  9.990000e+02,  1.019100e-01,  1.190790e-01,  1.056110e-01},
				{ 5.539500e-02,  3.644500e-02,  4.868500e-02,  1.014900e-01,  9.990000e+02,  8.333100e-02,  1.319880e-01,  1.013680e-01},
				{ 2.171700e-02,  9.162100e-02,  1.229200e-01,  6.575300e-02,  9.990000e+02,  9.957300e-02,  1.116890e-01,  9.245590e-02},
				{ 7.413900e-02,  9.507800e-02,  1.043700e-01,  1.150400e-01,  9.990000e+02,  8.975500e-02,  1.337050e-01,  8.518700e-02},
				{ 3.194100e-02,  3.158600e-02,  8.481300e-02,  5.316000e-02,  9.990000e+02,  3.712100e-02,  1.089010e-01,  9.788790e-02},
				{ 3.554600e-02,  4.984000e-02,  5.985500e-02,  7.159700e-02,  9.990000e+02,  7.008500e-02,  1.101326e-01,  1.135835e-01}};

				static double ff_u_fside_16_chisq[10][8]=
				{{ 1.513070e-02,  2.160500e-02,  2.276180e-02,  2.963070e-02,  9.990000e+02,  3.194320e-02,  3.653570e-02,  2.849060e-02},
				{ 1.210470e-02,  2.178040e-02,  2.193470e-02,  3.461370e-02,  9.990000e+02,  2.663780e-02,  3.259120e-02,  2.547270e-02},
				{ 1.390900e-02,  1.634420e-02,  2.064420e-02,  2.741970e-02,  9.990000e+02,  2.546830e-02,  2.607000e-02,  2.764570e-02},
				{ 1.108290e-02,  1.718370e-02,  2.069660e-02,  3.498030e-02,  9.990000e+02,  2.300280e-02,  3.785630e-02,  2.681380e-02},
				{ 1.122700e-02,  2.140900e-02,  1.819800e-02,  1.924900e-02,  9.990000e+02,  3.326500e-02,  3.974770e-02,  3.089960e-02},
				{ 8.142200e-03,  7.449700e-03,  7.976900e-03,  2.691300e-02,  9.990000e+02,  2.852930e-02,  3.108920e-02,  2.784130e-02},
				{ 8.116500e-03,  9.995800e-03,  1.786100e-02,  3.955000e-03,  9.990000e+02,  2.973700e-02,  3.035400e-02,  2.404090e-02},
				{ 9.525300e-03,  9.509700e-03,  2.046500e-02,  2.669800e-02,  9.990000e+02,  2.116960e-02,  2.822690e-02,  2.384490e-02},
				{ 1.095900e-02,  6.878700e-03,  2.080500e-02,  2.614000e-02,  9.990000e+02,  5.629100e-03,  3.109110e-02,  2.524290e-02},
				{ 8.678800e-03,  1.499000e-02,  1.642900e-02,  2.487200e-02,  9.990000e+02,  2.305920e-02,  2.772870e-02,  2.500160e-02}};

				static double ff_u_w1_16_chisq[10][8]=
				{{ 5.794060e-03,  1.280410e-02,  1.365190e-02,  1.354180e-02,  9.990000e+02,  1.911590e-02,  3.325300e-02,  3.332950e-02},
				{ 4.331690e-03,  1.075940e-02,  1.460540e-02,  1.563310e-02,  9.990000e+02,  1.699100e-02,  3.217100e-02,  3.077980e-02},
				{ 5.630430e-03,  1.123900e-02,  1.195000e-02,  1.282540e-02,  9.990000e+02,  1.613910e-02,  2.484800e-02,  2.873600e-02},
				{ 3.071320e-03,  1.203020e-02,  1.019980e-02,  1.876550e-02,  9.990000e+02,  1.675410e-02,  2.464150e-02,  3.527380e-02},
				{ 3.364800e-03,  1.242700e-02,  1.160400e-02,  8.426600e-03,  9.990000e+02,  1.350160e-02,  3.335910e-02,  3.592520e-02},
				{ 6.414300e-03,  7.580700e-03,  5.370500e-03,  1.599500e-02,  9.990000e+02,  1.040430e-02,  2.739810e-02,  3.028730e-02},
				{-2.431300e-03,  7.340500e-03,  8.965300e-03,  1.890400e-03,  9.990000e+02,  1.190610e-02,  3.056420e-02,  3.338410e-02},
				{ 6.334700e-04,  2.569900e-03,  2.213500e-02,  1.128100e-02,  9.990000e+02,  3.062700e-03,  2.429750e-02,  3.166380e-02},
				{ 6.729800e-03,  7.388500e-03,  1.652400e-02,  1.072600e-02,  9.990000e+02,  7.269800e-03,  1.644930e-02,  2.149420e-02},
				{ 3.330900e-03,  6.504400e-03,  1.679700e-02,  1.807400e-02,  9.990000e+02,  1.222440e-02,  2.329220e-02,  2.988480e-02}};

				static double ff_u_eratio_16_chisq[10][8]=
				{{-1.501680e-03, -2.267650e-03, -1.318680e-03, -4.350920e-03,  9.990000e+02, -3.761810e-03, -2.517920e-03, -1.510240e-03},
				{-8.972420e-04, -1.992200e-03, -1.978320e-03, -1.764090e-03,  9.990000e+02, -1.771560e-03, -6.873910e-04, -5.440870e-04},
				{ 1.859100e-04, -1.396200e-03, -1.415110e-03, -1.901860e-03,  9.990000e+02, -9.367700e-05, -1.576960e-03,  6.345810e-04},
				{-7.974370e-04,  9.150610e-04, -1.267780e-03, -3.083500e-03,  9.990000e+02, -1.373490e-03, -2.255890e-03, -7.003600e-04},
				{ 9.906900e-04,  2.175300e-03,  8.578700e-05, -2.194200e-03,  9.990000e+02, -3.297940e-03,  6.181460e-04, -7.570000e-06},
				{ 4.280600e-04,  2.825100e-03,  1.971300e-03,  3.034100e-04,  9.990000e+02,  1.354690e-03,  8.022500e-04,  1.848170e-04},
				{ 2.152800e-03,  2.800300e-04, -1.549100e-04,  6.393700e-04,  9.990000e+02, -1.716160e-03, -2.752300e-04,  8.950900e-04},
				{ 5.997300e-04, -7.917700e-04, -1.658000e-03, -8.848200e-04,  9.990000e+02, -3.728810e-03,  1.545560e-03, -7.895390e-04},
				{ 5.168500e-04, -1.297400e-03, -1.041800e-04, -4.549200e-03,  9.990000e+02, -1.226160e-03,  5.522500e-04, -1.304470e-03},
				{ 7.805100e-04, -1.222500e-03, -5.824200e-04, -2.538600e-03,  9.990000e+02, -1.416680e-03, -1.447780e-03, -8.826383e-04}};

				static double ff_u_deltae_16_chisq[10][8]=
				{{-1.943630e+00, -1.016860e+00, -3.848620e+00, -1.399780e+00,  9.990000e+02, -8.053200e-01, -4.699020e+00,  5.915180e-01},
				{-1.675360e+00, -9.614270e-01, -2.145410e+00, -7.554740e-01,  9.990000e+02, -7.556600e-01, -2.859860e+00,  1.207829e+00},
				{-1.425560e+00, -1.503350e+00, -1.376140e+00, -8.083390e-01,  9.990000e+02, -1.350340e+00, -6.274860e+00,  2.785130e+00},
				{-1.103710e+00, -1.853670e+00, -2.709680e+00,  3.303500e+00,  9.990000e+02,  3.120000e-01, -5.011380e+00,  3.231700e+00},
				{-1.920100e+00, -3.044800e-01, -6.059400e-01, -1.228400e+00,  9.990000e+02, -3.127360e+00, -3.497450e+00, -2.563490e+00},
				{-2.197600e+00, -3.424900e+00, -2.755000e+00, -1.383200e+00,  9.990000e+02,  3.772773e+00,  1.862220e+00, -3.101950e+00},
				{-3.725700e+00, -3.726000e+00, -1.639900e+00, -3.830900e+00,  9.990000e+02, -1.034200e-01, -6.663490e+00, -3.859810e+00},
				{ 6.487400e-01, -4.073200e+00, -2.579900e+00, -1.105200e+00,  9.990000e+02, -1.795100e+00, -3.158800e+00, -2.799930e+00},
				{-5.264600e+00, -6.362000e+00, -2.237700e+00,  3.203600e+00,  9.990000e+02, -3.072800e+00, -8.367050e+00, -1.386910e+01},
				{-1.733300e+00, -2.802800e+00, -7.191500e-01, -2.006200e+00,  9.990000e+02, -7.533540e-01, -5.486800e+00,  2.063560e+00}};

				static double ff_u_e277_16_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				// converted photons

				static double ff_c_rhad_16_chisq[10][8]=
				{{ 4.647570e-04, -1.270080e-04,  1.087990e-03,  6.642410e-04,  9.990000e+02,  1.813610e-03, -1.134102e-03,  1.622370e-04},
				{ 4.550910e-05,  5.184230e-04,  9.789480e-04,  6.916130e-04,  9.990000e+02,  1.537520e-03, -2.754480e-04, -2.916120e-04},
				{ 1.855440e-04, -3.774530e-04,  5.069260e-04,  2.570800e-04,  9.990000e+02,  1.732910e-04, -7.371960e-05,  6.321360e-04},
				{ 2.311830e-04,  5.267460e-04,  6.063560e-04, -5.334110e-05,  9.990000e+02, -7.271320e-04, -1.754000e-04, -2.148210e-04},
				{ 2.306600e-04, -4.208000e-04,  3.409500e-04,  5.112600e-04,  9.990000e+02, -2.490554e-04,  1.114960e-03, -2.088500e-04},
				{ 3.525500e-04, -2.502200e-04,  5.038600e-04,  1.733900e-04,  9.990000e+02,  2.986800e-04,  1.412930e-04, -1.550471e-04},
				{ 1.480500e-04,  1.979500e-04,  2.046100e-04,  1.500700e-04,  9.990000e+02, -4.442500e-04, -6.944290e-04,  8.243790e-04},
				{-4.783500e-04, -1.200100e-04,  8.692300e-05, -3.284900e-05,  9.990000e+02, -8.929150e-04, -6.265780e-04,  1.002710e-04},
				{ 1.105800e-04, -8.776100e-05, -5.942000e-05, -1.487800e-04,  9.990000e+02, -3.980480e-04,  5.470200e-04,  4.422571e-04},
				{-1.190100e-04, -1.204900e-04,  6.684900e-05,  8.644100e-05,  9.990000e+02,  1.978420e-04, -2.685900e-04,  2.397596e-04}};

				static double ff_c_reta_16_chisq[10][8]=
				{{-3.736100e-03, -5.031520e-03, -4.183450e-03, -8.603840e-03,  9.990000e+02, -7.849760e-03, -8.388200e-03, -7.430860e-03},
				{-4.590870e-03, -6.187060e-03, -6.356140e-03, -5.757420e-03,  9.990000e+02, -5.853630e-03, -7.466430e-03, -7.884220e-03},
				{-2.647740e-03, -5.844880e-03, -6.661510e-03, -5.602860e-03,  9.990000e+02, -4.355360e-03, -8.106930e-03, -5.592870e-03},
				{-8.014490e-04, -2.382030e-04, -5.741870e-03, -4.086790e-03,  9.990000e+02, -5.172260e-03, -6.235180e-03, -7.075670e-03},
				{-2.964900e-03, -4.750500e-03, -6.615600e-03, -2.415400e-03,  9.990000e+02, -6.216100e-03, -5.186900e-03, -8.753680e-03},
				{-1.226100e-03, -3.146900e-03, -3.615200e-03, -3.529200e-03,  9.990000e+02, -5.148900e-03, -7.673000e-03, -6.252700e-03},
				{-1.105500e-03, -1.566700e-03, -3.015000e-03, -4.498600e-03,  9.990000e+02, -6.294000e-03, -5.867400e-03, -5.431100e-03},
				{-3.111000e-03, -1.016900e-03, -4.358600e-03, -6.005300e-03,  9.990000e+02, -3.678100e-03, -9.083000e-03, -5.938600e-03},
				{-1.431700e-03, -3.661100e-03, -4.061200e-03, -2.812100e-03,  9.990000e+02, -6.385800e-03, -5.680300e-03, -8.117300e-03},
				{-1.801500e-03, -2.722900e-03, -3.525600e-03, -5.736700e-03,  9.990000e+02, -4.815500e-03, -6.192100e-03, -7.345610e-03}};

				static double ff_c_rphi_16_chisq[10][8]=
				{{-4.227990e-03, -3.866680e-03, -1.867660e-03,  2.261870e-03,  9.990000e+02,  2.799370e-03,  6.438900e-04,  2.704390e-03},
				{-1.027640e-03, -3.249340e-03, -2.626780e-03,  1.522890e-03,  9.990000e+02,  3.799430e-03,  1.888622e-03,  6.384910e-04},
				{-1.916920e-03,  2.648600e-04,  4.675180e-04, -3.829120e-04,  9.990000e+02,  3.784860e-03,  2.938112e-03,  3.218620e-03},
				{ 4.716110e-04, -6.449220e-03, -3.103250e-03,  2.269740e-05,  9.990000e+02,  3.099395e-03,  6.072250e-03,  2.853680e-03},
				{ 5.122200e-03, -6.789900e-03, -9.608300e-03, -6.878900e-03,  9.990000e+02, -6.513900e-03, -7.069400e-03, -2.308100e-03},
				{ 5.642600e-04,  3.123400e-03, -2.976800e-03, -9.021300e-03,  9.990000e+02, -5.157900e-03, -6.095700e-03, -8.257000e-04},
				{ 3.480400e-03,  1.035900e-02, -7.210000e-03, -1.052200e-02,  9.990000e+02, -5.102500e-03, -1.777500e-03, -3.510400e-04},
				{-3.087200e-03,  1.330000e-02,  3.312700e-03, -4.552500e-03,  9.990000e+02,  2.320300e-03, -5.124200e-03,  1.233690e-03},
				{-8.146700e-05, -3.719400e-03, -3.132100e-03,  4.845900e-03,  9.990000e+02,  1.019000e-04,  8.501500e-04, -1.013500e-04},
				{ 9.607200e-04, -5.330600e-03,  3.626600e-04, -3.675000e-03,  9.990000e+02, -1.165200e-03,  1.714370e-03,  1.185420e-03}};

				static double ff_c_weta2_16_chisq[10][8]=
				{{ 1.984090e-04,  3.229700e-04,  7.922080e-05,  2.193140e-04,  9.990000e+02,  2.647270e-04,  3.125460e-04,  3.202610e-04},
				{ 1.347550e-04,  2.698860e-04,  6.546110e-05,  1.007220e-04,  9.990000e+02,  2.352480e-04,  3.504210e-04,  3.417990e-04},
				{ 1.483000e-04,  1.685730e-04,  6.507600e-05,  1.639890e-04,  9.990000e+02,  2.635230e-04,  2.441490e-04,  2.798890e-04},
				{ 1.261610e-04,  1.081180e-04,  1.369620e-04,  1.020210e-04,  9.990000e+02,  2.368990e-04,  2.265690e-04,  2.863340e-04},
				{ 4.316100e-06,  4.326900e-04,  1.411000e-04,  5.020400e-05,  9.990000e+02,  3.856570e-04,  3.450550e-04,  2.967570e-04},
				{ 8.857200e-05,  4.224100e-05,  9.503100e-05,  1.031800e-04,  9.990000e+02,  2.178860e-04,  4.163960e-04,  3.049650e-04},
				{ 9.382100e-05,  3.646800e-06,  7.578100e-05,  2.022800e-04,  9.990000e+02,  2.503600e-04,  3.111000e-04,  3.370230e-04},
				{ 1.089900e-04,  2.003100e-05,  9.458000e-05,  1.132800e-04,  9.990000e+02,  1.490920e-04,  5.506400e-04,  4.018020e-04},
				{ 2.802300e-05,  1.082900e-04,  1.113000e-05,  1.388100e-04,  9.990000e+02,  3.327740e-04,  4.205670e-04,  4.194260e-04},
				{ 1.501700e-04,  2.692900e-04,  7.832100e-05,  1.725500e-04,  9.990000e+02,  3.101170e-04,  2.756900e-04,  3.211090e-04}};

				static double ff_c_f1_16_chisq[10][8]=
				{{ 3.266310e-03,  5.465980e-03, -1.071930e-04,  2.498580e-04,  9.990000e+02,  4.818400e-03,  4.367000e-04,  1.536400e-03},
				{-5.755690e-03, -8.054270e-03, -3.536150e-03, -2.589380e-03,  9.990000e+02,  1.296080e-03,  1.119560e-02,  4.473000e-03},
				{ 1.914980e-03, -3.958610e-03, -3.366130e-03, -6.245120e-03,  9.990000e+02,  2.409430e-03,  4.472800e-03, -4.808910e-03},
				{-1.263810e-03, -1.092440e-02, -7.221790e-03, -3.553850e-03,  9.990000e+02,  6.119900e-03, -3.955100e-03,  1.022800e-03},
				{-4.782100e-04,  1.425500e-02,  2.775100e-03, -8.037000e-03,  9.990000e+02,  6.020700e-03,  7.835000e-03,  7.972000e-03},
				{ 1.082800e-02, -1.158800e-02, -1.038700e-03,  9.721800e-04,  9.990000e+02,  7.537600e-03,  2.195700e-02,  1.222900e-02},
				{-1.022700e-02, -9.962500e-03, -3.642800e-03, -3.033400e-03,  9.990000e+02,  8.319000e-03,  5.300000e-03,  5.858000e-03},
				{ 1.669800e-02, -1.647500e-02, -4.820200e-03,  6.373700e-03,  9.990000e+02, -1.353540e-02,  3.210300e-02,  2.017100e-02},
				{-9.793300e-03,  2.112800e-02, -9.642300e-03,  1.179100e-02,  9.990000e+02,  2.785400e-02,  2.167100e-02,  3.234700e-02},
				{ 9.624700e-03,  6.222300e-03, -1.404300e-03,  1.404000e-02,  9.990000e+02,  4.430000e-03,  5.477000e-03,  2.110900e-02}};

				static double ff_c_wtot_16_chisq[10][8]=
				{{ 1.222880e-01,  1.229410e-01,  9.770390e-02,  1.533010e-01,  9.990000e+02,  1.060940e-01,  1.605870e-01,  1.025050e-01},
				{ 9.146380e-02,  8.194210e-02,  1.087570e-01,  1.148800e-01,  9.990000e+02,  1.019640e-01,  1.366290e-01,  1.148100e-01},
				{ 8.584850e-02,  1.260440e-01,  1.230280e-01,  1.268580e-01,  9.990000e+02,  8.125300e-02,  1.453650e-01,  9.780100e-02},
				{ 6.107070e-02,  1.489160e-01,  1.235660e-01,  1.099690e-01,  9.990000e+02,  1.098280e-01,  1.398560e-01,  1.002480e-01},
				{ 5.128200e-02,  6.754500e-02,  1.376100e-01,  1.513200e-01,  9.990000e+02,  8.385100e-02,  1.340360e-01,  7.727200e-02},
				{ 5.402800e-02,  6.237000e-02,  8.962300e-02,  1.249300e-01,  9.990000e+02,  1.126950e-01,  1.357290e-01,  7.460900e-02},
				{ 4.631100e-02,  4.029500e-02,  6.733200e-02,  1.341100e-01,  9.990000e+02,  1.327310e-01,  1.120430e-01,  9.750700e-02},
				{ 2.976500e-02,  7.269800e-02,  8.779300e-02,  6.984700e-02,  9.990000e+02,  7.037400e-02,  1.611210e-01,  6.218300e-02},
				{ 3.311000e-02,  1.605800e-01,  1.645100e-01,  8.243500e-02,  9.990000e+02,  4.762100e-02,  1.292780e-01,  1.060400e-01},
				{ 3.178600e-02,  9.076000e-02,  1.163800e-01,  1.030100e-01,  9.990000e+02,  1.402920e-01,  1.299760e-01,  9.557600e-02}};

				static double ff_c_fside_16_chisq[10][8]=
				{{ 2.335900e-02,  2.998660e-02,  2.900450e-02,  2.850360e-02,  9.990000e+02,  3.243150e-02,  3.743550e-02,  3.203750e-02},
				{ 1.650110e-02,  1.728890e-02,  3.146560e-02,  2.698820e-02,  9.990000e+02,  2.962640e-02,  3.535550e-02,  3.014950e-02},
				{ 1.593160e-02,  2.878700e-02,  2.480020e-02,  3.339640e-02,  9.990000e+02,  2.403100e-02,  3.523990e-02,  2.611660e-02},
				{ 1.744020e-02,  2.493620e-02,  3.115220e-02,  3.163760e-02,  9.990000e+02,  2.664390e-02,  3.076980e-02,  3.027190e-02},
				{ 9.073500e-03,  2.866100e-02,  3.721100e-02,  3.820500e-02,  9.990000e+02,  3.256700e-02,  3.813200e-02,  2.868030e-02},
				{ 9.722700e-03,  1.790000e-02,  2.374100e-02,  3.090100e-02,  9.990000e+02,  3.381400e-02,  4.426200e-02,  2.221890e-02},
				{ 1.113200e-02,  1.974600e-02,  2.390400e-02,  3.442000e-02,  9.990000e+02,  3.063200e-02,  3.914370e-02,  2.822840e-02},
				{ 4.366500e-03,  2.358900e-02,  1.795200e-02,  2.883600e-02,  9.990000e+02,  2.370200e-02,  3.981940e-02,  2.422940e-02},
				{ 1.078200e-02,  3.224200e-02,  3.343400e-02,  3.259600e-02,  9.990000e+02,  2.923000e-02,  4.240920e-02,  2.767180e-02},
				{ 8.395300e-03,  1.822700e-02,  3.000900e-02,  2.897100e-02,  9.990000e+02,  3.787100e-02,  3.570750e-02,  2.988630e-02}};

				static double ff_c_w1_16_chisq[10][8]=
				{{ 1.101750e-02,  1.617250e-02,  1.258030e-02,  1.342120e-02,  9.990000e+02,  1.360290e-02,  2.383380e-02,  2.745760e-02},
				{ 7.071550e-03,  8.596630e-03,  1.277860e-02,  1.171640e-02,  9.990000e+02,  1.380450e-02,  2.427370e-02,  2.216130e-02},
				{ 7.680570e-03,  9.606200e-03,  1.181320e-02,  1.229610e-02,  9.990000e+02,  1.337980e-02,  2.019790e-02,  2.432310e-02},
				{ 5.114660e-03,  1.277920e-02,  1.350950e-02,  1.242010e-02,  9.990000e+02,  1.228570e-02,  2.584320e-02,  2.561780e-02},
				{ 4.276500e-03,  1.711700e-02,  2.229300e-02,  8.055400e-03,  9.990000e+02,  1.816960e-02,  2.311510e-02,  2.684390e-02},
				{ 8.331700e-03,  2.309600e-02,  1.177600e-02,  1.319300e-02,  9.990000e+02,  1.188930e-02,  2.550820e-02,  2.902860e-02},
				{ 1.855600e-03,  7.041600e-03,  8.859800e-03,  1.377400e-02,  9.990000e+02,  8.470700e-03,  3.441620e-02,  2.804560e-02},
				{ 2.492100e-03,  2.734100e-03,  1.452500e-02,  1.114200e-02,  9.990000e+02,  8.680800e-03,  2.953380e-02,  2.707720e-02},
				{ 1.104200e-02,  8.210000e-03,  1.926800e-02,  1.739800e-02,  9.990000e+02,  1.931590e-02,  2.945540e-02,  2.776390e-02},
				{ 4.320700e-03,  1.162600e-02,  1.596200e-02,  1.792600e-02,  9.990000e+02,  1.715390e-02,  2.369400e-02,  2.405220e-02}};

				static double ff_c_eratio_16_chisq[10][8]=
				{{-2.908380e-03, -3.412070e-03, -3.410160e-03,  5.047550e-05,  9.990000e+02, -2.764530e-03, -2.750400e-03,  2.767018e-04},
				{-1.113450e-04,  1.488320e-03,  7.031360e-04, -3.512720e-03,  9.990000e+02, -2.070070e-03, -6.362100e-05,  4.995090e-04},
				{ 7.119750e-04, -5.030640e-03, -2.118950e-04, -3.070170e-03,  9.990000e+02, -1.215350e-03, -9.689000e-04,  4.726200e-04},
				{-1.704650e-03,  2.620460e-03, -5.336220e-03,  4.755970e-03,  9.990000e+02, -5.904100e-04, -2.151250e-03,  3.954620e-03},
				{-1.051900e-03,  5.428800e-03, -2.092400e-03, -9.147700e-04,  9.990000e+02, -1.906530e-03,  1.221189e-03,  7.183300e-04},
				{ 8.298800e-04,  8.514700e-05,  1.583300e-03, -1.744800e-03,  9.990000e+02, -1.088400e-03,  2.229820e-04, -2.714300e-04},
				{ 2.024700e-03, -1.287100e-03,  7.544000e-04, -8.198900e-03,  9.990000e+02, -8.686000e-04, -2.156190e-03, -9.002800e-04},
				{ 2.755900e-03, -2.158100e-04, -1.759800e-03,  3.219400e-03,  9.990000e+02,  7.100000e-06,  1.781090e-03,  2.540070e-03},
				{-2.594600e-04,  5.075800e-03,  9.683000e-04, -5.059700e-03,  9.990000e+02,  3.111000e-04, -1.079810e-03, -3.031695e-03},
				{ 4.039400e-04, -1.103200e-03, -3.296700e-03, -4.616600e-03,  9.990000e+02, -2.345300e-03, -1.117590e-03,  1.375665e-03}};

				static double ff_c_deltae_16_chisq[10][8]=
				{{-2.543520e+00, -1.688800e+00, -3.377220e+00, -1.237400e+00,  9.990000e+02, -1.657190e+00, -2.512930e+00,  3.684390e+00},
				{-1.761790e+00, -1.008200e+00, -1.790120e+00, -9.705140e-01,  9.990000e+02, -1.087080e+00, -1.736830e+00,  5.523750e+00},
				{-1.987940e+00, -2.069320e+00, -2.760860e+00, -4.116430e-01,  9.990000e+02, -1.619370e+00, -7.769930e+00,  8.485770e+00},
				{-1.346640e+00,  2.518620e+00,  4.606120e-01, -7.047010e-02,  9.990000e+02, -2.243624e+00, -2.239720e+00,  2.905860e+00},
				{ 5.325800e-01, -3.605900e+00, -3.191800e+00, -9.423800e-01,  9.990000e+02, -9.738280e-01, -6.665800e+00, -2.298900e-01},
				{-2.361600e+00, -1.126000e+00, -2.245600e+00, -2.037400e+00,  9.990000e+02, -9.424800e-01, -2.074120e+00, -2.304200e+00},
				{ 7.437200e-01,  6.545800e-01,  4.316000e-01,  2.751700e+00,  9.990000e+02,  3.786260e+00, -9.807730e+00, -1.430330e+00},
				{-4.825700e-01, -1.574900e+00,  2.189500e+00, -7.564100e+00,  9.990000e+02, -6.454340e+00, -2.468770e+00, -1.302230e+01},
				{-8.177900e+00, -7.065900e+00, -8.250500e+00, -7.478400e+00,  9.990000e+02,  8.015222e+00,  7.366290e+00,  5.046900e+00},
				{-5.471000e-01, -2.580100e+00, -1.458000e+00,  5.475700e-01,  9.990000e+02, -3.312667e+00, -4.771440e+00, -9.790000e-01}};

				static double ff_c_e277_16_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				// unconverted photons -- errors

				static double ff_u_rhad_err_16_chisq[10][8]=
				{{7.816820e-05, 1.371270e-04, 1.448240e-04, 1.768440e-04, 9.990000e+02, 7.148960e-04, 7.912060e-04, 5.250249e-04},
				{7.671250e-05, 1.335420e-04, 1.330740e-04, 1.822500e-04, 9.990000e+02, 5.889140e-04, 7.580930e-04, 4.895308e-04},
				{8.166250e-05, 1.386590e-04, 1.488820e-04, 1.754100e-04, 9.990000e+02, 6.830590e-04, 7.509490e-04, 5.250599e-04},
				{9.170700e-05, 1.532740e-04, 1.654150e-04, 2.135700e-04, 9.990000e+02, 7.546220e-04, 8.248492e-04, 5.825258e-04},
				{2.546400e-03, 2.432400e-03, 2.955000e-03, 2.740700e-03, 0.000000e+00, 1.060100e-02, 9.454005e-03, 7.355908e-03},
				{2.240700e-03, 1.906600e-03, 2.531200e-03, 2.218700e-03, 0.000000e+00, 8.707900e-03, 7.713907e-03, 5.799416e-03},
				{1.929900e-03, 1.651300e-03, 1.917600e-03, 1.918100e-03, 0.000000e+00, 7.822000e-03, 7.411006e-03, 4.595610e-03},
				{1.873600e-03, 1.372900e-03, 1.791600e-03, 1.484500e-03, 0.000000e+00, 6.101600e-03, 5.171008e-03, 3.954100e-03},
				{1.704500e-03, 1.435900e-03, 1.394400e-03, 1.337100e-03, 0.000000e+00, 4.046708e-03, 3.680700e-03, 3.110108e-03},
				{1.735500e-03, 9.419800e-04, 1.016700e-03, 9.508100e-04, 0.000000e+00, 3.593740e-03, 2.958737e-03, 1.940885e-03}};

				static double ff_u_reta_err_16_chisq[10][8]=
				{{4.061260e-04, 7.472750e-04, 5.293670e-04, 7.202290e-04, 9.990000e+02, 6.753160e-04, 5.916251e-04, 5.950171e-04},
				{4.067600e-04, 6.783300e-04, 5.207220e-04, 6.792150e-04, 9.990000e+02, 6.819300e-04, 6.638536e-04, 5.583203e-04},
				{3.762380e-04, 6.918620e-04, 5.318430e-04, 6.910770e-04, 9.990000e+02, 7.484410e-04, 7.597991e-04, 7.050344e-04},
				{4.568180e-04, 8.642160e-04, 6.547620e-04, 8.714350e-04, 9.990000e+02, 8.066583e-04, 8.617745e-04, 8.441582e-04},
				{1.584500e-02, 1.651300e-02, 1.416000e-02, 1.369400e-02, 0.000000e+00, 1.449002e-02, 1.113002e-02, 1.380800e-02},
				{1.235600e-02, 1.362500e-02, 1.241400e-02, 1.174300e-02, 0.000000e+00, 1.219501e-02, 1.017800e-02, 1.224101e-02},
				{1.145200e-02, 1.117300e-02, 9.737900e-03, 9.333000e-03, 0.000000e+00, 1.118502e-02, 9.676702e-03, 1.111100e-02},
				{9.405800e-03, 1.196200e-02, 8.283200e-03, 8.277900e-03, 0.000000e+00, 1.092402e-02, 1.039300e-02, 1.102600e-02},
				{8.515800e-03, 8.199800e-03, 7.886400e-03, 7.756000e-03, 0.000000e+00, 1.074302e-02, 7.724401e-03, 1.040400e-02},
				{6.780100e-03, 7.274900e-03, 7.184100e-03, 7.298000e-03, 0.000000e+00, 9.400517e-03, 7.297803e-03, 9.812905e-03}};

				static double ff_u_rphi_err_16_chisq[10][8]=
				{{3.013950e-04, 5.796680e-04, 4.449350e-04, 7.089640e-04, 9.990000e+02, 5.409800e-04, 4.897928e-04, 4.298121e-04},
				{2.791850e-04, 5.148330e-04, 4.571240e-04, 7.182770e-04, 9.990000e+02, 5.821819e-04, 5.744728e-04, 4.968364e-04},
				{3.093260e-04, 5.964170e-04, 5.085510e-04, 7.834240e-04, 9.990000e+02, 6.682064e-04, 6.281147e-04, 5.278764e-04},
				{3.380590e-04, 8.234220e-04, 6.434340e-04, 9.622860e-04, 9.990000e+02, 7.471134e-04, 7.535452e-04, 7.681853e-04},
				{1.181300e-02, 1.261800e-02, 1.393700e-02, 1.920400e-02, 0.000000e+00, 1.425307e-02, 1.092102e-02, 1.309401e-02},
				{1.034700e-02, 1.390500e-02, 1.173500e-02, 1.507600e-02, 0.000000e+00, 1.326504e-02, 9.273840e-03, 1.137603e-02},
				{9.389600e-03, 1.107800e-02, 1.101000e-02, 1.462800e-02, 0.000000e+00, 1.177106e-02, 8.598377e-03, 1.028903e-02},
				{8.474100e-03, 1.012600e-02, 9.659100e-03, 1.151900e-02, 0.000000e+00, 1.069709e-02, 9.044136e-03, 1.070202e-02},
				{7.604200e-03, 8.656000e-03, 8.395200e-03, 9.115500e-03, 0.000000e+00, 1.133413e-02, 9.004538e-03, 8.553332e-03},
				{6.546600e-03, 7.195100e-03, 7.734600e-03, 9.390700e-03, 0.000000e+00, 9.269173e-03, 7.450041e-03, 8.748830e-03}};

				static double ff_u_weta2_err_16_chisq[10][8]=
				{{1.181070e-05, 2.367640e-05, 1.713990e-05, 2.311950e-05, 9.990000e+02, 2.534150e-05, 2.519806e-05, 1.921871e-05},
				{1.273720e-05, 2.387360e-05, 1.876130e-05, 2.622050e-05, 9.990000e+02, 2.699910e-05, 2.827031e-05, 2.334549e-05},
				{1.570450e-05, 2.730060e-05, 2.240090e-05, 2.721060e-05, 9.990000e+02, 3.226570e-05, 3.546606e-05, 2.800376e-05},
				{1.827040e-05, 3.802290e-05, 2.763670e-05, 3.660410e-05, 9.990000e+02, 3.780940e-05, 4.535020e-05, 3.875341e-05},
				{6.444100e-04, 7.105000e-04, 6.405900e-04, 6.403700e-04, 0.000000e+00, 7.159000e-04, 6.339400e-04, 5.986303e-04},
				{5.615300e-04, 6.891300e-04, 5.688000e-04, 5.464300e-04, 0.000000e+00, 6.366000e-04, 5.535000e-04, 5.661903e-04},
				{5.250000e-04, 6.488800e-04, 5.528200e-04, 5.238700e-04, 0.000000e+00, 5.922000e-04, 5.820004e-04, 5.243607e-04},
				{4.996900e-04, 6.018000e-04, 4.823500e-04, 5.085300e-04, 0.000000e+00, 6.012300e-04, 5.118001e-04, 5.483504e-04},
				{4.359800e-04, 5.081900e-04, 4.646200e-04, 4.511100e-04, 0.000000e+00, 6.002300e-04, 5.932904e-04, 5.770607e-04},
				{3.908600e-04, 4.580800e-04, 3.862700e-04, 4.389800e-04, 0.000000e+00, 5.351509e-04, 5.064901e-04, 4.964505e-04}};

				static double ff_u_f1_err_16_chisq[10][8]=
				{{1.541770e-03, 2.541150e-03, 1.651870e-03, 2.117430e-03, 9.990000e+02, 2.288540e-03, 4.444730e-03, 3.960720e-03},
				{2.063200e-03, 3.306900e-03, 2.235480e-03, 2.619620e-03, 9.990000e+02, 2.950310e-03, 5.588180e-03, 5.015130e-03},
				{2.744130e-03, 4.823900e-03, 2.663970e-03, 3.589180e-03, 9.990000e+02, 4.231210e-03, 6.411320e-03, 5.683570e-03},
				{3.709110e-03, 6.851980e-03, 3.882640e-03, 4.635670e-03, 9.990000e+02, 5.991250e-03, 8.442800e-03, 7.720468e-03},
				{1.439300e-01, 1.420300e-01, 9.828700e-02, 7.920000e-02, 0.000000e+00, 1.104100e-01, 1.255100e-01, 1.301400e-01},
				{1.302500e-01, 1.260200e-01, 9.805100e-02, 9.072000e-02, 0.000000e+00, 1.144300e-01, 1.135000e-01, 1.294600e-01},
				{1.330700e-01, 1.326900e-01, 9.525300e-02, 8.099100e-02, 0.000000e+00, 1.163900e-01, 1.083800e-01, 1.336400e-01},
				{1.276000e-01, 1.270400e-01, 1.038300e-01, 8.048100e-02, 0.000000e+00, 1.193500e-01, 1.078400e-01, 1.210700e-01},
				{1.173900e-01, 1.291700e-01, 9.408100e-02, 8.876100e-02, 0.000000e+00, 1.312000e-01, 9.321903e-02, 1.185100e-01},
				{1.046600e-01, 1.240100e-01, 8.575700e-02, 7.942000e-02, 0.000000e+00, 1.333600e-01, 8.965400e-02, 9.710702e-02}};

				static double ff_u_wtot_err_16_chisq[10][8]=
				{{5.873830e-03, 1.032790e-02, 1.026160e-02, 1.202360e-02, 9.990000e+02, 1.391240e-02, 1.051539e-02, 6.206957e-03},
				{7.215800e-03, 1.055230e-02, 1.239210e-02, 1.438900e-02, 9.990000e+02, 1.732470e-02, 1.078503e-02, 7.047515e-03},
				{8.400990e-03, 1.346300e-02, 1.432300e-02, 1.582140e-02, 9.990000e+02, 1.700380e-02, 1.466400e-02, 8.090872e-03},
				{1.067040e-02, 1.602950e-02, 1.774210e-02, 2.450290e-02, 9.990000e+02, 2.563980e-02, 1.883235e-02, 1.081564e-02},
				{3.314000e-01, 3.182700e-01, 3.586600e-01, 3.780200e-01, 0.000000e+00, 3.966400e-01, 1.735904e-01, 1.612109e-01},
				{2.766300e-01, 3.032400e-01, 3.485100e-01, 3.380000e-01, 0.000000e+00, 3.544800e-01, 2.076005e-01, 1.408307e-01},
				{2.801200e-01, 2.788300e-01, 3.536200e-01, 2.976900e-01, 0.000000e+00, 3.257007e-01, 1.530705e-01, 1.451909e-01},
				{2.634000e-01, 2.611000e-01, 3.135300e-01, 3.154300e-01, 0.000000e+00, 3.453606e-01, 1.396610e-01, 1.306414e-01},
				{2.330300e-01, 2.235900e-01, 3.104500e-01, 3.238900e-01, 0.000000e+00, 3.666808e-01, 1.346407e-01, 1.305014e-01},
				{1.949900e-01, 2.359400e-01, 2.824600e-01, 3.143200e-01, 0.000000e+00, 2.603914e-01, 1.344119e-01, 1.098920e-01}};

				static double ff_u_fside_err_16_chisq[10][8]=
				{{9.242800e-04, 1.905240e-03, 2.064350e-03, 2.713380e-03, 9.990000e+02, 2.453480e-03, 1.885231e-03, 1.295955e-03},
				{1.075200e-03, 2.271290e-03, 2.340870e-03, 3.370390e-03, 9.990000e+02, 3.124270e-03, 2.327725e-03, 1.520565e-03},
				{1.199340e-03, 2.453410e-03, 2.855540e-03, 3.834800e-03, 9.990000e+02, 3.715880e-03, 2.440614e-03, 1.887409e-03},
				{1.622950e-03, 3.429960e-03, 3.494760e-03, 4.831340e-03, 9.990000e+02, 4.759330e-03, 3.014197e-03, 2.228307e-03},
				{6.265300e-02, 7.331200e-02, 8.788200e-02, 9.458900e-02, 0.000000e+00, 9.715400e-02, 4.243501e-02, 4.174105e-02},
				{5.600700e-02, 7.342000e-02, 8.466000e-02, 9.180300e-02, 0.000000e+00, 8.865600e-02, 4.206703e-02, 3.792605e-02},
				{5.348000e-02, 6.837900e-02, 8.023800e-02, 8.698600e-02, 0.000000e+00, 7.785222e-02, 3.637902e-02, 3.860811e-02},
				{4.614200e-02, 5.717400e-02, 7.320600e-02, 7.785800e-02, 0.000000e+00, 8.181319e-02, 3.592001e-02, 3.668516e-02},
				{4.689300e-02, 5.569700e-02, 6.134600e-02, 7.071600e-02, 0.000000e+00, 8.557700e-02, 3.872805e-02, 3.056729e-02},
				{3.934200e-02, 4.881000e-02, 5.899800e-02, 6.022200e-02, 0.000000e+00, 6.148300e-02, 2.839022e-02, 2.724444e-02}};

				static double ff_u_w1_err_16_chisq[10][8]=
				{{6.889220e-04, 1.470730e-03, 1.283290e-03, 1.487350e-03, 9.990000e+02, 1.722376e-03, 1.807972e-03, 1.235857e-03},
				{8.748210e-04, 1.648580e-03, 1.560020e-03, 1.634690e-03, 9.990000e+02, 2.113828e-03, 2.180871e-03, 1.558069e-03},
				{1.071320e-03, 1.971620e-03, 1.912610e-03, 2.008230e-03, 9.990000e+02, 2.851104e-03, 2.821164e-03, 1.953970e-03},
				{1.446100e-03, 2.821910e-03, 2.680670e-03, 3.044580e-03, 9.990000e+02, 3.372165e-03, 3.694483e-03, 2.594028e-03},
				{5.421800e-02, 5.679000e-02, 6.570700e-02, 4.521100e-02, 0.000000e+00, 5.438103e-02, 4.928602e-02, 4.498605e-02},
				{4.936300e-02, 5.788300e-02, 6.064100e-02, 4.544400e-02, 0.000000e+00, 5.560106e-02, 4.937405e-02, 4.616802e-02},
				{5.186800e-02, 5.200400e-02, 6.418800e-02, 4.155700e-02, 0.000000e+00, 5.235204e-02, 4.331301e-02, 4.348901e-02},
				{5.095500e-02, 4.871000e-02, 5.813200e-02, 4.350800e-02, 0.000000e+00, 5.281901e-02, 5.327802e-02, 4.197401e-02},
				{4.372100e-02, 5.451900e-02, 5.650300e-02, 4.409200e-02, 0.000000e+00, 5.870902e-02, 5.212402e-02, 3.506502e-02},
				{3.845300e-02, 4.560200e-02, 5.393200e-02, 4.619700e-02, 0.000000e+00, 5.399001e-02, 4.249502e-02, 3.287801e-02}};

				static double ff_u_eratio_err_16_chisq[10][8]=
				{{3.614060e-04, 6.248950e-04, 8.149090e-04, 1.286320e-03, 9.990000e+02, 7.554915e-04, 6.479990e-04, 4.956033e-04},
				{4.387110e-04, 6.686990e-04, 9.372190e-04, 1.130390e-03, 9.990000e+02, 7.175556e-04, 7.317704e-04, 6.032952e-04},
				{5.534840e-04, 8.781930e-04, 9.179880e-04, 1.094100e-03, 9.990000e+02, 1.134776e-03, 7.469567e-04, 6.608264e-04},
				{4.309700e-04, 1.075910e-03, 1.324440e-03, 2.207310e-03, 9.990000e+02, 4.958503e-04, 1.231546e-03, 9.335795e-04},
				{2.047700e-02, 2.301700e-02, 2.746700e-02, 2.985400e-02, 0.000000e+00, 1.622007e-02, 1.435804e-02, 1.408638e-02},
				{1.842600e-02, 1.922000e-02, 2.834200e-02, 2.801600e-02, 0.000000e+00, 1.558192e-02, 1.594299e-02, 1.392376e-02},
				{1.769900e-02, 1.794900e-02, 2.117600e-02, 2.177300e-02, 0.000000e+00, 1.373494e-02, 1.427374e-02, 1.301052e-02},
				{1.514900e-02, 1.575300e-02, 2.149000e-02, 2.545000e-02, 0.000000e+00, 1.018179e-02, 1.232565e-02, 1.158042e-02},
				{1.504800e-02, 1.460400e-02, 2.099500e-02, 2.020100e-02, 0.000000e+00, 1.118762e-02, 1.091702e-02, 1.044691e-02},
				{1.300000e-02, 1.357000e-02, 1.918300e-02, 1.989800e-02, 0.000000e+00, 9.544917e-03, 9.537604e-03, 1.052729e-02}};

				static double ff_u_deltae_err_16_chisq[10][8]=
				{{1.818410e-01, 4.903280e-01, 1.699010e+00, 3.606930e-01, 9.990000e+02, 6.625630e-01, 1.304210e+00, 1.298030e+00},
				{2.934720e-01, 4.196190e-01, 5.884050e-01, 6.058220e-01, 9.990000e+02, 7.406810e-01, 1.252070e+00, 1.506610e+00},
				{4.196260e-01, 5.529750e-01, 8.119680e-01, 6.679960e-01, 9.990000e+02, 6.700850e-01, 2.610490e+00, 1.084860e+00},
				{5.556910e-01, 8.845690e-01, 5.869480e-01, 3.940610e-01, 9.990000e+02, 8.770020e-01, 2.324470e+00, 2.041720e+00},
				{2.447800e+01, 2.235800e+01, 2.056100e+01, 2.043600e+01, 0.000000e+00, 3.170900e+01, 3.291300e+01, 3.417000e+01},
				{2.352200e+01, 2.345400e+01, 2.161800e+01, 2.001300e+01, 0.000000e+00, 2.916400e+01, 3.433400e+01, 3.338000e+01},
				{2.429300e+01, 2.364900e+01, 2.044700e+01, 2.157500e+01, 0.000000e+00, 3.138000e+01, 4.095000e+01, 3.395200e+01},
				{2.260000e+01, 2.393500e+01, 2.028900e+01, 2.136800e+01, 0.000000e+00, 3.037800e+01, 2.892500e+01, 3.009800e+01},
				{2.390500e+01, 2.395800e+01, 2.039100e+01, 1.888600e+01, 0.000000e+00, 2.985100e+01, 4.322800e+01, 3.513900e+01},
				{2.217000e+01, 2.349700e+01, 1.916300e+01, 1.971300e+01, 0.000000e+00, 2.856900e+01, 3.028700e+01, 2.610800e+01}};

				static double ff_u_e277_err_16_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				// converted photons -- errors

				static double ff_c_rhad_err_16_chisq[10][8]=
				{{1.764210e-04, 2.805140e-04, 2.123310e-04, 2.566150e-04, 9.990000e+02, 9.212290e-04, 9.313661e-04, 7.023264e-04},
				{1.596980e-04, 2.678630e-04, 2.067960e-04, 2.361580e-04, 9.990000e+02, 7.529430e-04, 9.357777e-04, 6.833467e-04},
				{1.630440e-04, 2.561500e-04, 2.043390e-04, 2.163140e-04, 9.990000e+02, 6.925690e-04, 1.357054e-03, 8.463884e-04},
				{1.867700e-04, 2.726730e-04, 2.353340e-04, 2.788550e-04, 9.990000e+02, 1.032270e-03, 1.092466e-03, 8.068129e-04},
				{2.272100e-03, 2.258500e-03, 2.772900e-03, 3.039600e-03, 0.000000e+00, 1.086000e-02, 1.026801e-02, 7.097608e-03},
				{2.125600e-03, 1.899500e-03, 2.247100e-03, 2.452000e-03, 0.000000e+00, 8.524600e-03, 7.625305e-03, 5.710109e-03},
				{1.711800e-03, 1.782300e-03, 1.884200e-03, 1.793100e-03, 0.000000e+00, 7.824900e-03, 6.421305e-03, 5.077105e-03},
				{1.782900e-03, 1.375900e-03, 1.769400e-03, 1.840400e-03, 0.000000e+00, 5.211307e-03, 4.844410e-03, 3.576000e-03},
				{1.389700e-03, 9.675500e-04, 1.539700e-03, 1.296500e-03, 0.000000e+00, 4.609305e-03, 3.405300e-03, 2.818610e-03},
				{1.366300e-03, 9.955400e-04, 1.079500e-03, 8.976300e-04, 0.000000e+00, 3.128757e-03, 2.555947e-03, 2.074479e-03}};

				static double ff_c_reta_err_16_chisq[10][8]=
				{{1.353850e-03, 2.101190e-03, 1.153040e-03, 1.304910e-03, 9.990000e+02, 1.213000e-03, 1.066599e-03, 9.853280e-04},
				{9.587090e-04, 1.889290e-03, 9.984070e-04, 1.297180e-03, 9.990000e+02, 9.302170e-04, 8.440004e-04, 1.010190e-03},
				{9.758200e-04, 1.823290e-03, 9.553850e-04, 1.193320e-03, 9.990000e+02, 1.002220e-03, 9.845894e-04, 1.268420e-03},
				{1.198460e-03, 1.799740e-03, 1.160200e-03, 1.322800e-03, 9.990000e+02, 1.221140e-03, 1.103298e-03, 1.406619e-03},
				{1.967200e-02, 1.961300e-02, 1.505900e-02, 1.816900e-02, 0.000000e+00, 1.590200e-02, 1.116601e-02, 1.364902e-02},
				{1.414400e-02, 1.655300e-02, 1.460700e-02, 1.525300e-02, 0.000000e+00, 1.413206e-02, 1.074701e-02, 1.350901e-02},
				{1.251500e-02, 1.314100e-02, 1.259800e-02, 1.296700e-02, 0.000000e+00, 1.187805e-02, 9.607101e-03, 1.167601e-02},
				{1.361000e-02, 1.280800e-02, 1.090500e-02, 1.396600e-02, 0.000000e+00, 1.225603e-02, 8.862908e-03, 1.113603e-02},
				{1.114100e-02, 1.122900e-02, 8.074900e-03, 1.064300e-02, 0.000000e+00, 1.109403e-02, 6.946801e-03, 1.115400e-02},
				{7.123000e-03, 7.932100e-03, 7.628100e-03, 7.883900e-03, 0.000000e+00, 9.673919e-03, 7.144105e-03, 8.937700e-03}};

				static double ff_c_rphi_err_16_chisq[10][8]=
				{{1.419220e-03, 2.592470e-03, 1.796380e-03, 2.683160e-03, 9.990000e+02, 1.544850e-03, 1.404815e-03, 1.805079e-03},
				{1.396990e-03, 2.607860e-03, 1.779540e-03, 3.166430e-03, 9.990000e+02, 1.706150e-03, 1.421520e-03, 2.378321e-03},
				{1.647170e-03, 3.104280e-03, 1.967330e-03, 3.104730e-03, 9.990000e+02, 1.816820e-03, 2.043584e-03, 3.237190e-03},
				{1.892560e-03, 3.659090e-03, 2.495940e-03, 2.719040e-03, 9.990000e+02, 1.953920e-03, 2.073140e-03, 2.288616e-03},
				{3.015800e-02, 3.872200e-02, 5.222200e-02, 6.264300e-02, 0.000000e+00, 4.227367e-02, 2.563229e-02, 2.604396e-02},
				{2.627400e-02, 5.300300e-02, 4.254900e-02, 4.852400e-02, 0.000000e+00, 3.268902e-02, 2.082134e-02, 2.301989e-02},
				{2.941400e-02, 5.690500e-02, 3.306500e-02, 3.624100e-02, 0.000000e+00, 2.494850e-02, 1.658346e-02, 1.989696e-02},
				{1.976400e-02, 3.381300e-02, 2.675500e-02, 3.149700e-02, 0.000000e+00, 2.405750e-02, 1.764106e-02, 1.605992e-02},
				{1.825600e-02, 1.744300e-02, 1.906500e-02, 2.297400e-02, 0.000000e+00, 1.876521e-02, 1.315804e-02, 1.271170e-02},
				{1.292200e-02, 1.310200e-02, 1.447500e-02, 1.561300e-02, 0.000000e+00, 1.459708e-02, 1.171648e-02, 1.184425e-02}};

				static double ff_c_weta2_err_16_chisq[10][8]=
				{{3.288140e-05, 6.566000e-05, 3.412930e-05, 4.678110e-05, 9.990000e+02, 3.952580e-05, 3.831910e-05, 3.007210e-05},
				{3.514140e-05, 5.962530e-05, 3.249150e-05, 4.372410e-05, 9.990000e+02, 3.706470e-05, 3.708941e-05, 3.538020e-05},
				{3.674310e-05, 5.973460e-05, 3.516920e-05, 4.308100e-05, 9.990000e+02, 4.317740e-05, 5.223007e-05, 4.444270e-05},
				{4.336740e-05, 8.605270e-05, 4.172490e-05, 5.667390e-05, 9.990000e+02, 5.250180e-05, 5.658467e-05, 5.314800e-05},
				{7.093200e-04, 8.728800e-04, 7.288700e-04, 9.235400e-04, 0.000000e+00, 8.274800e-04, 6.806500e-04, 5.813001e-04},
				{6.988500e-04, 8.059800e-04, 6.446600e-04, 7.324900e-04, 0.000000e+00, 7.518700e-04, 6.814800e-04, 5.825000e-04},
				{5.490200e-04, 7.383200e-04, 6.073000e-04, 6.390900e-04, 0.000000e+00, 7.161100e-04, 6.182000e-04, 5.512600e-04},
				{5.330500e-04, 6.744800e-04, 5.239200e-04, 6.643200e-04, 0.000000e+00, 6.617700e-04, 4.952100e-04, 5.330900e-04},
				{4.162100e-04, 6.744700e-04, 5.242800e-04, 4.773600e-04, 0.000000e+00, 6.298700e-04, 6.101102e-04, 5.677101e-04},
				{3.961000e-04, 4.668000e-04, 4.479700e-04, 4.686600e-04, 0.000000e+00, 6.068900e-04, 5.032302e-04, 4.508602e-04}};

				static double ff_c_f1_err_16_chisq[10][8]=
				{{2.853320e-03, 3.752820e-03, 1.809480e-03, 2.024890e-03, 9.990000e+02, 2.085410e-03, 4.090736e-03, 3.468280e-03},
				{3.211650e-03, 4.758440e-03, 2.433980e-03, 2.342830e-03, 9.990000e+02, 2.641730e-03, 4.493832e-03, 4.883980e-03},
				{3.892360e-03, 5.516730e-03, 2.961500e-03, 3.144790e-03, 9.990000e+02, 3.393400e-03, 6.794840e-03, 7.110010e-03},
				{5.846660e-03, 7.581870e-03, 4.228360e-03, 4.046250e-03, 9.990000e+02, 3.861740e-03, 7.008240e-03, 9.887440e-03},
				{9.757800e-02, 8.711700e-02, 6.420600e-02, 5.781700e-02, 0.000000e+00, 6.882800e-02, 8.607902e-02, 1.026100e-01},
				{8.858800e-02, 9.105500e-02, 6.809900e-02, 6.402200e-02, 0.000000e+00, 6.697500e-02, 8.275102e-02, 9.825500e-02},
				{9.106800e-02, 9.089000e-02, 7.142400e-02, 5.140700e-02, 0.000000e+00, 7.232800e-02, 7.743305e-02, 9.760800e-02},
				{9.367000e-02, 9.927400e-02, 7.230400e-02, 6.659400e-02, 0.000000e+00, 7.062800e-02, 8.147800e-02, 9.860901e-02},
				{8.290500e-02, 8.985100e-02, 6.717000e-02, 5.081800e-02, 0.000000e+00, 8.319200e-02, 7.009300e-02, 9.846801e-02},
				{9.079300e-02, 9.233300e-02, 6.602700e-02, 5.590500e-02, 0.000000e+00, 8.883807e-02, 7.971000e-02, 8.698903e-02}};

				static double ff_c_wtot_err_16_chisq[10][8]=
				{{1.248220e-02, 1.941250e-02, 1.690880e-02, 1.786060e-02, 9.990000e+02, 1.838620e-02, 1.155734e-02, 6.648185e-03},
				{1.419950e-02, 2.290940e-02, 1.552280e-02, 1.892570e-02, 9.990000e+02, 1.724740e-02, 1.147865e-02, 7.403438e-03},
				{1.468090e-02, 2.339940e-02, 1.727210e-02, 1.755100e-02, 9.990000e+02, 2.021400e-02, 1.361996e-02, 9.760420e-03},
				{1.741030e-02, 2.884680e-02, 2.128230e-02, 2.502250e-02, 9.990000e+02, 2.447180e-02, 1.652287e-02, 9.601123e-03},
				{2.843300e-01, 2.884300e-01, 3.846300e-01, 3.235700e-01, 0.000000e+00, 4.193900e-01, 1.887702e-01, 1.247907e-01},
				{2.474900e-01, 2.679100e-01, 3.437200e-01, 3.208800e-01, 0.000000e+00, 3.832500e-01, 1.545902e-01, 1.260909e-01},
				{2.148600e-01, 2.699300e-01, 3.133200e-01, 2.993600e-01, 0.000000e+00, 3.382400e-01, 1.600003e-01, 1.177009e-01},
				{2.077600e-01, 2.251200e-01, 3.112900e-01, 3.506400e-01, 0.000000e+00, 3.417109e-01, 1.792102e-01, 1.207909e-01},
				{2.009100e-01, 2.045100e-01, 2.790600e-01, 2.765200e-01, 0.000000e+00, 4.162006e-01, 1.244602e-01, 1.177014e-01},
				{1.747700e-01, 2.099600e-01, 2.756700e-01, 2.838200e-01, 0.000000e+00, 3.022819e-01, 1.340103e-01, 1.135219e-01}};

				static double ff_c_fside_err_16_chisq[10][8]=
				{{2.047640e-03, 4.345300e-03, 3.388440e-03, 4.317280e-03, 9.990000e+02, 4.136115e-03, 2.302647e-03, 1.585170e-03},
				{1.937920e-03, 4.849200e-03, 3.775350e-03, 4.880880e-03, 9.990000e+02, 4.306397e-03, 2.463732e-03, 1.885909e-03},
				{2.415360e-03, 5.313790e-03, 4.248250e-03, 5.117720e-03, 9.990000e+02, 5.333957e-03, 2.818333e-03, 2.384756e-03},
				{2.868750e-03, 6.616360e-03, 4.881070e-03, 6.971010e-03, 9.990000e+02, 5.839265e-03, 3.458802e-03, 2.853748e-03},
				{6.086600e-02, 6.997900e-02, 9.091400e-02, 9.870000e-02, 0.000000e+00, 1.111000e-01, 4.623802e-02, 3.502318e-02},
				{5.488300e-02, 5.981800e-02, 8.542700e-02, 9.534800e-02, 0.000000e+00, 1.012300e-01, 4.004006e-02, 3.866819e-02},
				{4.544300e-02, 6.179700e-02, 7.674900e-02, 9.724100e-02, 0.000000e+00, 9.051400e-02, 3.647413e-02, 3.419423e-02},
				{4.343900e-02, 5.597000e-02, 7.748200e-02, 8.107700e-02, 0.000000e+00, 1.074000e-01, 3.713710e-02, 2.908328e-02},
				{3.331300e-02, 5.079700e-02, 6.148300e-02, 7.764800e-02, 0.000000e+00, 9.155937e-02, 3.066510e-02, 2.958538e-02},
				{3.557700e-02, 4.653900e-02, 5.443600e-02, 5.969400e-02, 0.000000e+00, 6.957900e-02, 3.089511e-02, 2.536246e-02}};

				static double ff_c_w1_err_16_chisq[10][8]=
				{{1.506820e-03, 2.831600e-03, 2.074250e-03, 2.021050e-03, 9.990000e+02, 1.590100e-03, 2.455608e-03, 1.801975e-03},
				{1.696300e-03, 3.522540e-03, 1.838580e-03, 2.411320e-03, 9.990000e+02, 1.937610e-03, 2.381047e-03, 2.062673e-03},
				{1.932770e-03, 3.820250e-03, 2.440120e-03, 2.460110e-03, 9.990000e+02, 2.545530e-03, 3.645261e-03, 2.940141e-03},
				{2.768430e-03, 4.669830e-03, 3.151250e-03, 2.992090e-03, 9.990000e+02, 3.000920e-03, 3.933258e-03, 3.455854e-03},
				{4.637000e-02, 5.097900e-02, 4.918300e-02, 4.974500e-02, 0.000000e+00, 4.846500e-02, 5.123303e-02, 4.036705e-02},
				{4.342300e-02, 5.502300e-02, 5.420200e-02, 4.510700e-02, 0.000000e+00, 4.863603e-02, 4.893502e-02, 4.337607e-02},
				{4.143600e-02, 5.187200e-02, 5.056700e-02, 4.267300e-02, 0.000000e+00, 4.674103e-02, 4.920204e-02, 4.552402e-02},
				{4.503400e-02, 4.945300e-02, 4.937400e-02, 3.413500e-02, 0.000000e+00, 4.598202e-02, 5.106907e-02, 4.740905e-02},
				{3.220300e-02, 4.425400e-02, 4.608200e-02, 4.140500e-02, 0.000000e+00, 4.313303e-02, 4.185708e-02, 4.033302e-02},
				{3.443700e-02, 4.468600e-02, 4.035800e-02, 3.294800e-02, 0.000000e+00, 4.249703e-02, 4.814501e-02, 3.782904e-02}};

				static double ff_c_eratio_err_16_chisq[10][8]=
				{{8.323280e-04, 2.094440e-03, 1.669240e-03, 2.825670e-03, 9.990000e+02, 1.311396e-03, 7.248278e-04, 8.674720e-04},
				{1.174930e-03, 9.828010e-04, 1.352940e-03, 2.454710e-03, 9.990000e+02, 1.224637e-03, 1.184115e-03, 4.410817e-04},
				{9.395750e-04, 4.658300e-04, 1.665580e-03, 2.294620e-03, 9.990000e+02, 1.309752e-03, 7.389144e-04, 4.475596e-04},
				{1.174940e-03, 2.805590e-03, 2.264980e-03, 2.998200e-03, 9.990000e+02, 1.447803e-03, 5.464248e-04, 2.213187e-03},
				{1.999800e-02, 1.987900e-02, 3.626300e-02, 3.615600e-02, 0.000000e+00, 2.085933e-02, 1.511618e-02, 1.316296e-02},
				{1.893900e-02, 1.983500e-02, 3.010700e-02, 3.367200e-02, 0.000000e+00, 1.724470e-02, 1.150381e-02, 1.187556e-02},
				{1.672300e-02, 1.971900e-02, 2.819000e-02, 3.024600e-02, 0.000000e+00, 1.634564e-02, 1.036844e-02, 1.270126e-02},
				{1.691200e-02, 1.536800e-02, 2.602900e-02, 2.717000e-02, 0.000000e+00, 1.397313e-02, 1.063307e-02, 1.167612e-02},
				{1.308700e-02, 1.539800e-02, 1.922900e-02, 2.230600e-02, 0.000000e+00, 1.309317e-02, 8.128516e-03, 8.429754e-03},
				{1.241600e-02, 1.430000e-02, 1.839400e-02, 1.910600e-02, 0.000000e+00, 9.437932e-03, 7.643400e-03, 1.127000e-02}};

				static double ff_c_deltae_err_16_chisq[10][8]=
				{{5.612680e-01, 1.084520e+00, 4.718530e-01, 5.288140e-01, 9.990000e+02, 8.151500e-01, 1.400840e+00, 6.742870e-01},
				{5.455040e-01, 8.538720e-01, 1.019310e+00, 8.277810e-01, 9.990000e+02, 7.248130e-01, 1.591030e+00, 1.101740e+00},
				{6.548010e-01, 1.037350e+00, 6.860910e-01, 8.181900e-01, 9.990000e+02, 1.527590e+00, 4.168400e+00, 7.724040e-01},
				{8.386020e-01, 1.328200e+00, 1.767060e+00, 1.601350e+00, 9.990000e+02, 1.153210e+00, 1.877370e+00, 3.727310e+00},
				{2.291200e+01, 2.996300e+01, 2.360200e+01, 2.354200e+01, 0.000000e+00, 3.239100e+01, 3.441800e+01, 2.870200e+01},
				{2.463100e+01, 2.336800e+01, 2.512600e+01, 2.472100e+01, 0.000000e+00, 3.096400e+01, 3.457100e+01, 3.010900e+01},
				{2.324400e+01, 2.256800e+01, 2.129200e+01, 1.970900e+01, 0.000000e+00, 2.706100e+01, 3.352400e+01, 2.785500e+01},
				{2.470000e+01, 3.524400e+01, 2.080700e+01, 2.584600e+01, 0.000000e+00, 3.808000e+01, 2.963400e+01, 3.393200e+01},
				{2.575000e+01, 2.487600e+01, 2.687600e+01, 2.490200e+01, 0.000000e+00, 2.663000e+01, 2.808800e+01, 2.553000e+01},
				{2.365200e+01, 2.431400e+01, 2.043200e+01, 1.904200e+01, 0.000000e+00, 3.118000e+01, 3.234000e+01, 3.291900e+01}};

				static double ff_c_e277_err_16_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};


				if (isConv) {
				m_rhad_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_rhad_16_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_rhad_16_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_reta_16_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_rphi_16_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_weta2_16_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_f1_16_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_deltae_16_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_wtot_16_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_fside_16_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_w1_16_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_eratio_16_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_e277_16_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_rhad_err_16_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_rhad_err_16_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_reta_err_16_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_rphi_err_16_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_weta2_err_16_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_f1_err_16_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_deltae_err_16_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_wtot_err_16_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_fside_err_16_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_w1_err_16_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_eratio_err_16_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_c_e277_err_16_chisq[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_rhad_16_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_rhad_16_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_reta_16_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_rphi_16_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_weta2_16_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_f1_16_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_deltae_16_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_wtot_16_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_fside_16_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_w1_16_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_eratio_16_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_e277_16_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_rhad_err_16_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_rhad_err_16_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_reta_err_16_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_rphi_err_16_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_weta2_err_16_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_f1_err_16_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_deltae_err_16_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_wtot_err_16_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_fside_err_16_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_w1_err_16_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_eratio_err_16_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8, pt_DVs_16_chisq, eta_DVs_16_chisq, &ff_u_e277_err_16_chisq[0][0], 0.);

				}

				break;

		case 17: // tune 17 -- same as tune 16 except for FF on f1 set to zeros
                
                static double pt_DVs_17_chisq[10] = {15000, 20000, 25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_17_chisq[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				// unconverted photons -- values

				static double ff_u_rhad_17_chisq[10][8]=
				{{ 3.019900e-04,  1.066000e-04,  1.499420e-03,  1.041820e-03,  9.990000e+02,  9.478400e-04,  1.534824e-03,  7.909690e-04},
				{ 1.874340e-04,  3.507410e-04,  8.030420e-04,  5.812780e-04,  9.990000e+02, -2.021590e-04, -2.756160e-04,  3.259150e-04},
				{ 7.981470e-05,  4.897180e-04,  8.627990e-04,  4.201260e-04,  9.990000e+02, -4.871290e-04,  8.522920e-04, -1.631830e-04},
				{ 1.013160e-04, -5.192840e-05,  5.086700e-04,  2.485320e-04,  9.990000e+02, -9.151720e-04, -1.251880e-03,  7.936350e-04},
				{ 3.920300e-05, -3.652300e-05,  3.954000e-04, -1.243000e-05,  9.990000e+02,  1.142143e-03, -6.784400e-04, -6.458440e-04},
				{-1.125300e-04,  4.647300e-06,  3.821400e-04,  1.347300e-04,  9.990000e+02,  3.840670e-04,  1.768204e-03, -3.174840e-05},
				{-4.091600e-05, -2.509500e-04,  2.734100e-04,  9.786900e-05,  9.990000e+02,  1.181590e-03,  3.221620e-04,  1.257030e-04},
				{-1.074500e-04, -6.318900e-06,  2.545900e-04,  2.310600e-04,  9.990000e+02,  8.141300e-04,  1.278349e-04, -2.496890e-05},
				{-1.551100e-04, -3.669800e-04,  1.806400e-04, -1.809100e-04,  9.990000e+02,  9.062960e-05, -8.844400e-04, -1.464480e-04},
				{-3.631100e-04, -1.320600e-05,  2.337200e-05,  7.126700e-06,  9.990000e+02, -1.147100e-04,  4.500600e-04, -4.687000e-05}};

				static double ff_u_reta_17_chisq[10][8]=
				{{-3.144930e-03, -5.402240e-03, -6.554780e-03, -6.395580e-03,  9.990000e+02, -6.412980e-03, -7.004050e-03, -7.621050e-03},
				{-2.583850e-03, -3.929570e-03, -5.703660e-03, -5.085160e-03,  9.990000e+02, -5.812290e-03, -5.708890e-03, -6.610030e-03},
				{-2.677260e-03, -3.588300e-03, -4.642080e-03, -5.552430e-03,  9.990000e+02, -4.833370e-03, -4.084380e-03, -6.790760e-03},
				{-2.344460e-03, -2.023490e-03, -4.294390e-03, -6.085800e-03,  9.990000e+02, -4.625270e-03, -5.114670e-03, -7.184860e-03},
				{-9.853600e-04, -3.956000e-03, -5.417300e-03, -5.848900e-03,  9.990000e+02, -4.646800e-03, -6.727900e-03, -8.715150e-03},
				{-1.612300e-03, -1.885200e-03, -3.435100e-03, -5.343900e-03,  9.990000e+02, -5.503000e-03, -5.197000e-03, -8.335050e-03},
				{-7.824700e-04, -7.493200e-04, -3.875600e-03, -4.177300e-03,  9.990000e+02, -3.924900e-03, -5.431300e-03, -8.804680e-03},
				{-1.684800e-03, -2.231600e-03, -6.061300e-03, -4.957700e-03,  9.990000e+02, -3.700400e-03, -7.099910e-03, -6.842140e-03},
				{-1.058600e-03, -2.314000e-03, -4.476100e-03, -4.348700e-03,  9.990000e+02, -3.898300e-03, -3.710010e-03, -7.576550e-03},
				{-1.465700e-03, -1.320100e-03, -4.655400e-03, -4.690200e-03,  9.990000e+02, -2.901100e-03, -5.072280e-03, -6.571940e-03}};

				static double ff_u_rphi_17_chisq[10][8]=
				{{ 5.006220e-04, -2.286910e-03,  2.303110e-04, -1.568210e-03,  9.990000e+02,  1.278753e-03,  2.395560e-03,  2.350960e-03},
				{ 7.400260e-04, -1.046910e-03,  8.099420e-04, -1.713690e-03,  9.990000e+02,  1.611230e-03,  2.711310e-03,  3.047560e-03},
				{ 6.598890e-04,  3.895660e-04,  2.302570e-03, -8.364130e-04,  9.990000e+02,  2.765080e-03,  5.061390e-03,  2.680500e-03},
				{ 9.805340e-04,  9.571520e-04,  2.521490e-03, -1.512620e-03,  9.990000e+02,  2.011973e-03,  3.674570e-03,  1.667763e-03},
				{ 2.400400e-04, -1.697600e-03,  1.458700e-03,  1.330800e-03,  9.990000e+02,  1.098973e-03,  2.875680e-03,  1.704000e-05},
				{ 1.726200e-03,  1.912400e-03,  1.942800e-03, -7.374900e-04,  9.990000e+02,  1.900170e-03,  2.725160e-03,  8.944440e-04},
				{ 1.821700e-03, -9.855900e-04,  3.247200e-03,  3.367200e-03,  9.990000e+02,  3.519100e-03,  1.565930e-03,  9.671600e-04},
				{ 1.103400e-03,  2.381100e-03,  5.805000e-04, -2.605200e-04,  9.990000e+02,  2.878920e-03,  2.919760e-03,  1.875600e-03},
				{ 2.043300e-03, -8.466600e-04,  2.911200e-04, -2.060100e-05,  9.990000e+02,  3.142500e-03,  4.517030e-03,  2.086000e-03},
				{ 1.481500e-03,  1.077600e-03,  1.166300e-03,  2.801300e-04,  9.990000e+02,  3.212910e-03,  2.969070e-03,  3.082610e-03}};

				static double ff_u_weta2_17_chisq[10][8]=
				{{ 1.377970e-04,  2.054210e-04,  9.640480e-05,  1.287720e-04,  9.990000e+02,  3.063380e-04,  3.668230e-04,  3.422800e-04},
				{ 1.106810e-04,  1.710130e-04,  7.147090e-05,  1.402530e-04,  9.990000e+02,  3.197160e-04,  3.370940e-04,  3.211480e-04},
				{ 1.272070e-04,  1.440930e-04,  3.136360e-05,  1.136210e-04,  9.990000e+02,  2.836820e-04,  2.875930e-04,  3.621100e-04},
				{ 9.842820e-05,  1.541040e-04,  5.407990e-05,  8.488500e-05,  9.990000e+02,  3.226290e-04,  3.419810e-04,  3.677280e-04},
				{ 1.377800e-04,  2.145200e-04,  2.073600e-05,  9.706400e-05,  9.990000e+02,  1.973370e-04,  4.378090e-04,  4.161730e-04},
				{ 7.743800e-05,  8.315100e-05,  6.624400e-05,  7.146400e-05,  9.990000e+02,  2.483360e-04,  3.104150e-04,  3.578000e-04},
				{ 9.068800e-05,  1.208600e-04,  8.464900e-06, -2.776400e-05,  9.990000e+02,  2.597750e-04,  3.398190e-04,  4.322900e-04},
				{ 4.300100e-05,  1.002500e-04,  1.121800e-04,  6.741600e-05,  9.990000e+02,  1.758150e-04,  2.997560e-04,  2.972190e-04},
				{ 5.920200e-05,  2.098000e-04,  4.767000e-05,  1.034700e-04,  9.990000e+02,  2.227080e-04,  2.687760e-04,  3.249630e-04},
				{ 1.113400e-04,  1.463900e-04,  6.551100e-05,  8.786800e-05,  9.990000e+02,  2.969260e-04,  3.038270e-04,  3.653930e-04}};

				static double ff_u_f1_17_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_wtot_17_chisq[10][8]=
				{{ 1.005980e-01,  1.153640e-01,  1.049840e-01,  1.265120e-01,  9.990000e+02,  1.303250e-01,  1.659700e-01,  1.170870e-01},
				{ 7.109680e-02,  1.142850e-01,  1.086510e-01,  1.312640e-01,  9.990000e+02,  1.034410e-01,  1.261930e-01,  9.618400e-02},
				{ 6.164610e-02,  9.234110e-02,  1.131740e-01,  1.110190e-01,  9.990000e+02,  7.938900e-02,  1.141240e-01,  1.149590e-01},
				{ 8.354390e-02,  6.532110e-02,  9.884640e-02,  1.142730e-01,  9.990000e+02,  1.050670e-01,  1.462050e-01,  1.148760e-01},
				{ 4.013200e-02,  8.859800e-02,  8.487300e-02,  1.017000e-01,  9.990000e+02,  1.019100e-01,  1.190790e-01,  1.056110e-01},
				{ 5.539500e-02,  3.644500e-02,  4.868500e-02,  1.014900e-01,  9.990000e+02,  8.333100e-02,  1.319880e-01,  1.013680e-01},
				{ 2.171700e-02,  9.162100e-02,  1.229200e-01,  6.575300e-02,  9.990000e+02,  9.957300e-02,  1.116890e-01,  9.245590e-02},
				{ 7.413900e-02,  9.507800e-02,  1.043700e-01,  1.150400e-01,  9.990000e+02,  8.975500e-02,  1.337050e-01,  8.518700e-02},
				{ 3.194100e-02,  3.158600e-02,  8.481300e-02,  5.316000e-02,  9.990000e+02,  3.712100e-02,  1.089010e-01,  9.788790e-02},
				{ 3.554600e-02,  4.984000e-02,  5.985500e-02,  7.159700e-02,  9.990000e+02,  7.008500e-02,  1.101326e-01,  1.135835e-01}};

				static double ff_u_fside_17_chisq[10][8]=
				{{ 1.513070e-02,  2.160500e-02,  2.276180e-02,  2.963070e-02,  9.990000e+02,  3.194320e-02,  3.653570e-02,  2.849060e-02},
				{ 1.210470e-02,  2.178040e-02,  2.193470e-02,  3.461370e-02,  9.990000e+02,  2.663780e-02,  3.259120e-02,  2.547270e-02},
				{ 1.390900e-02,  1.634420e-02,  2.064420e-02,  2.741970e-02,  9.990000e+02,  2.546830e-02,  2.607000e-02,  2.764570e-02},
				{ 1.108290e-02,  1.718370e-02,  2.069660e-02,  3.498030e-02,  9.990000e+02,  2.300280e-02,  3.785630e-02,  2.681380e-02},
				{ 1.122700e-02,  2.140900e-02,  1.819800e-02,  1.924900e-02,  9.990000e+02,  3.326500e-02,  3.974770e-02,  3.089960e-02},
				{ 8.142200e-03,  7.449700e-03,  7.976900e-03,  2.691300e-02,  9.990000e+02,  2.852930e-02,  3.108920e-02,  2.784130e-02},
				{ 8.116500e-03,  9.995800e-03,  1.786100e-02,  3.955000e-03,  9.990000e+02,  2.973700e-02,  3.035400e-02,  2.404090e-02},
				{ 9.525300e-03,  9.509700e-03,  2.046500e-02,  2.669800e-02,  9.990000e+02,  2.116960e-02,  2.822690e-02,  2.384490e-02},
				{ 1.095900e-02,  6.878700e-03,  2.080500e-02,  2.614000e-02,  9.990000e+02,  5.629100e-03,  3.109110e-02,  2.524290e-02},
				{ 8.678800e-03,  1.499000e-02,  1.642900e-02,  2.487200e-02,  9.990000e+02,  2.305920e-02,  2.772870e-02,  2.500160e-02}};

				static double ff_u_w1_17_chisq[10][8]=
				{{ 5.794060e-03,  1.280410e-02,  1.365190e-02,  1.354180e-02,  9.990000e+02,  1.911590e-02,  3.325300e-02,  3.332950e-02},
				{ 4.331690e-03,  1.075940e-02,  1.460540e-02,  1.563310e-02,  9.990000e+02,  1.699100e-02,  3.217100e-02,  3.077980e-02},
				{ 5.630430e-03,  1.123900e-02,  1.195000e-02,  1.282540e-02,  9.990000e+02,  1.613910e-02,  2.484800e-02,  2.873600e-02},
				{ 3.071320e-03,  1.203020e-02,  1.019980e-02,  1.876550e-02,  9.990000e+02,  1.675410e-02,  2.464150e-02,  3.527380e-02},
				{ 3.364800e-03,  1.242700e-02,  1.160400e-02,  8.426600e-03,  9.990000e+02,  1.350160e-02,  3.335910e-02,  3.592520e-02},
				{ 6.414300e-03,  7.580700e-03,  5.370500e-03,  1.599500e-02,  9.990000e+02,  1.040430e-02,  2.739810e-02,  3.028730e-02},
				{-2.431300e-03,  7.340500e-03,  8.965300e-03,  1.890400e-03,  9.990000e+02,  1.190610e-02,  3.056420e-02,  3.338410e-02},
				{ 6.334700e-04,  2.569900e-03,  2.213500e-02,  1.128100e-02,  9.990000e+02,  3.062700e-03,  2.429750e-02,  3.166380e-02},
				{ 6.729800e-03,  7.388500e-03,  1.652400e-02,  1.072600e-02,  9.990000e+02,  7.269800e-03,  1.644930e-02,  2.149420e-02},
				{ 3.330900e-03,  6.504400e-03,  1.679700e-02,  1.807400e-02,  9.990000e+02,  1.222440e-02,  2.329220e-02,  2.988480e-02}};

				static double ff_u_eratio_17_chisq[10][8]=
				{{-1.501680e-03, -2.267650e-03, -1.318680e-03, -4.350920e-03,  9.990000e+02, -3.761810e-03, -2.517920e-03, -1.510240e-03},
				{-8.972420e-04, -1.992200e-03, -1.978320e-03, -1.764090e-03,  9.990000e+02, -1.771560e-03, -6.873910e-04, -5.440870e-04},
				{ 1.859100e-04, -1.396200e-03, -1.415110e-03, -1.901860e-03,  9.990000e+02, -9.367700e-05, -1.576960e-03,  6.345810e-04},
				{-7.974370e-04,  9.150610e-04, -1.267780e-03, -3.083500e-03,  9.990000e+02, -1.373490e-03, -2.255890e-03, -7.003600e-04},
				{ 9.906900e-04,  2.175300e-03,  8.578700e-05, -2.194200e-03,  9.990000e+02, -3.297940e-03,  6.181460e-04, -7.570000e-06},
				{ 4.280600e-04,  2.825100e-03,  1.971300e-03,  3.034100e-04,  9.990000e+02,  1.354690e-03,  8.022500e-04,  1.848170e-04},
				{ 2.152800e-03,  2.800300e-04, -1.549100e-04,  6.393700e-04,  9.990000e+02, -1.716160e-03, -2.752300e-04,  8.950900e-04},
				{ 5.997300e-04, -7.917700e-04, -1.658000e-03, -8.848200e-04,  9.990000e+02, -3.728810e-03,  1.545560e-03, -7.895390e-04},
				{ 5.168500e-04, -1.297400e-03, -1.041800e-04, -4.549200e-03,  9.990000e+02, -1.226160e-03,  5.522500e-04, -1.304470e-03},
				{ 7.805100e-04, -1.222500e-03, -5.824200e-04, -2.538600e-03,  9.990000e+02, -1.416680e-03, -1.447780e-03, -8.826383e-04}};

				static double ff_u_deltae_17_chisq[10][8]=
				{{-1.943630e+00, -1.016860e+00, -3.848620e+00, -1.399780e+00,  9.990000e+02, -8.053200e-01, -4.699020e+00,  5.915180e-01},
				{-1.675360e+00, -9.614270e-01, -2.145410e+00, -7.554740e-01,  9.990000e+02, -7.556600e-01, -2.859860e+00,  1.207829e+00},
				{-1.425560e+00, -1.503350e+00, -1.376140e+00, -8.083390e-01,  9.990000e+02, -1.350340e+00, -6.274860e+00,  2.785130e+00},
				{-1.103710e+00, -1.853670e+00, -2.709680e+00,  3.303500e+00,  9.990000e+02,  3.120000e-01, -5.011380e+00,  3.231700e+00},
				{-1.920100e+00, -3.044800e-01, -6.059400e-01, -1.228400e+00,  9.990000e+02, -3.127360e+00, -3.497450e+00, -2.563490e+00},
				{-2.197600e+00, -3.424900e+00, -2.755000e+00, -1.383200e+00,  9.990000e+02,  3.772773e+00,  1.862220e+00, -3.101950e+00},
				{-3.725700e+00, -3.726000e+00, -1.639900e+00, -3.830900e+00,  9.990000e+02, -1.034200e-01, -6.663490e+00, -3.859810e+00},
				{ 6.487400e-01, -4.073200e+00, -2.579900e+00, -1.105200e+00,  9.990000e+02, -1.795100e+00, -3.158800e+00, -2.799930e+00},
				{-5.264600e+00, -6.362000e+00, -2.237700e+00,  3.203600e+00,  9.990000e+02, -3.072800e+00, -8.367050e+00, -1.386910e+01},
				{-1.733300e+00, -2.802800e+00, -7.191500e-01, -2.006200e+00,  9.990000e+02, -7.533540e-01, -5.486800e+00,  2.063560e+00}};

				static double ff_u_e277_17_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				// converted photons -- values

				static double ff_c_rhad_17_chisq[10][8]=
				{{ 4.647570e-04, -1.270080e-04,  1.087990e-03,  6.642410e-04,  9.990000e+02,  1.813610e-03, -1.134102e-03,  1.622370e-04},
				{ 4.550910e-05,  5.184230e-04,  9.789480e-04,  6.916130e-04,  9.990000e+02,  1.537520e-03, -2.754480e-04, -2.916120e-04},
				{ 1.855440e-04, -3.774530e-04,  5.069260e-04,  2.570800e-04,  9.990000e+02,  1.732910e-04, -7.371960e-05,  6.321360e-04},
				{ 2.311830e-04,  5.267460e-04,  6.063560e-04, -5.334110e-05,  9.990000e+02, -7.271320e-04, -1.754000e-04, -2.148210e-04},
				{ 2.306600e-04, -4.208000e-04,  3.409500e-04,  5.112600e-04,  9.990000e+02, -2.490554e-04,  1.114960e-03, -2.088500e-04},
				{ 3.525500e-04, -2.502200e-04,  5.038600e-04,  1.733900e-04,  9.990000e+02,  2.986800e-04,  1.412930e-04, -1.550471e-04},
				{ 1.480500e-04,  1.979500e-04,  2.046100e-04,  1.500700e-04,  9.990000e+02, -4.442500e-04, -6.944290e-04,  8.243790e-04},
				{-4.783500e-04, -1.200100e-04,  8.692300e-05, -3.284900e-05,  9.990000e+02, -8.929150e-04, -6.265780e-04,  1.002710e-04},
				{ 1.105800e-04, -8.776100e-05, -5.942000e-05, -1.487800e-04,  9.990000e+02, -3.980480e-04,  5.470200e-04,  4.422571e-04},
				{-1.190100e-04, -1.204900e-04,  6.684900e-05,  8.644100e-05,  9.990000e+02,  1.978420e-04, -2.685900e-04,  2.397596e-04}};

				static double ff_c_reta_17_chisq[10][8]=
				{{-3.736100e-03, -5.031520e-03, -4.183450e-03, -8.603840e-03,  9.990000e+02, -7.849760e-03, -8.388200e-03, -7.430860e-03},
				{-4.590870e-03, -6.187060e-03, -6.356140e-03, -5.757420e-03,  9.990000e+02, -5.853630e-03, -7.466430e-03, -7.884220e-03},
				{-2.647740e-03, -5.844880e-03, -6.661510e-03, -5.602860e-03,  9.990000e+02, -4.355360e-03, -8.106930e-03, -5.592870e-03},
				{-8.014490e-04, -2.382030e-04, -5.741870e-03, -4.086790e-03,  9.990000e+02, -5.172260e-03, -6.235180e-03, -7.075670e-03},
				{-2.964900e-03, -4.750500e-03, -6.615600e-03, -2.415400e-03,  9.990000e+02, -6.216100e-03, -5.186900e-03, -8.753680e-03},
				{-1.226100e-03, -3.146900e-03, -3.615200e-03, -3.529200e-03,  9.990000e+02, -5.148900e-03, -7.673000e-03, -6.252700e-03},
				{-1.105500e-03, -1.566700e-03, -3.015000e-03, -4.498600e-03,  9.990000e+02, -6.294000e-03, -5.867400e-03, -5.431100e-03},
				{-3.111000e-03, -1.016900e-03, -4.358600e-03, -6.005300e-03,  9.990000e+02, -3.678100e-03, -9.083000e-03, -5.938600e-03},
				{-1.431700e-03, -3.661100e-03, -4.061200e-03, -2.812100e-03,  9.990000e+02, -6.385800e-03, -5.680300e-03, -8.117300e-03},
				{-1.801500e-03, -2.722900e-03, -3.525600e-03, -5.736700e-03,  9.990000e+02, -4.815500e-03, -6.192100e-03, -7.345610e-03}};

				static double ff_c_rphi_17_chisq[10][8]=
				{{-4.227990e-03, -3.866680e-03, -1.867660e-03,  2.261870e-03,  9.990000e+02,  2.799370e-03,  6.438900e-04,  2.704390e-03},
				{-1.027640e-03, -3.249340e-03, -2.626780e-03,  1.522890e-03,  9.990000e+02,  3.799430e-03,  1.888622e-03,  6.384910e-04},
				{-1.916920e-03,  2.648600e-04,  4.675180e-04, -3.829120e-04,  9.990000e+02,  3.784860e-03,  2.938112e-03,  3.218620e-03},
				{ 4.716110e-04, -6.449220e-03, -3.103250e-03,  2.269740e-05,  9.990000e+02,  3.099395e-03,  6.072250e-03,  2.853680e-03},
				{ 5.122200e-03, -6.789900e-03, -9.608300e-03, -6.878900e-03,  9.990000e+02, -6.513900e-03, -7.069400e-03, -2.308100e-03},
				{ 5.642600e-04,  3.123400e-03, -2.976800e-03, -9.021300e-03,  9.990000e+02, -5.157900e-03, -6.095700e-03, -8.257000e-04},
				{ 3.480400e-03,  1.035900e-02, -7.210000e-03, -1.052200e-02,  9.990000e+02, -5.102500e-03, -1.777500e-03, -3.510400e-04},
				{-3.087200e-03,  1.330000e-02,  3.312700e-03, -4.552500e-03,  9.990000e+02,  2.320300e-03, -5.124200e-03,  1.233690e-03},
				{-8.146700e-05, -3.719400e-03, -3.132100e-03,  4.845900e-03,  9.990000e+02,  1.019000e-04,  8.501500e-04, -1.013500e-04},
				{ 9.607200e-04, -5.330600e-03,  3.626600e-04, -3.675000e-03,  9.990000e+02, -1.165200e-03,  1.714370e-03,  1.185420e-03}};

				static double ff_c_weta2_17_chisq[10][8]=
				{{ 1.984090e-04,  3.229700e-04,  7.922080e-05,  2.193140e-04,  9.990000e+02,  2.647270e-04,  3.125460e-04,  3.202610e-04},
				{ 1.347550e-04,  2.698860e-04,  6.546110e-05,  1.007220e-04,  9.990000e+02,  2.352480e-04,  3.504210e-04,  3.417990e-04},
				{ 1.483000e-04,  1.685730e-04,  6.507600e-05,  1.639890e-04,  9.990000e+02,  2.635230e-04,  2.441490e-04,  2.798890e-04},
				{ 1.261610e-04,  1.081180e-04,  1.369620e-04,  1.020210e-04,  9.990000e+02,  2.368990e-04,  2.265690e-04,  2.863340e-04},
				{ 4.316100e-06,  4.326900e-04,  1.411000e-04,  5.020400e-05,  9.990000e+02,  3.856570e-04,  3.450550e-04,  2.967570e-04},
				{ 8.857200e-05,  4.224100e-05,  9.503100e-05,  1.031800e-04,  9.990000e+02,  2.178860e-04,  4.163960e-04,  3.049650e-04},
				{ 9.382100e-05,  3.646800e-06,  7.578100e-05,  2.022800e-04,  9.990000e+02,  2.503600e-04,  3.111000e-04,  3.370230e-04},
				{ 1.089900e-04,  2.003100e-05,  9.458000e-05,  1.132800e-04,  9.990000e+02,  1.490920e-04,  5.506400e-04,  4.018020e-04},
				{ 2.802300e-05,  1.082900e-04,  1.113000e-05,  1.388100e-04,  9.990000e+02,  3.327740e-04,  4.205670e-04,  4.194260e-04},
				{ 1.501700e-04,  2.692900e-04,  7.832100e-05,  1.725500e-04,  9.990000e+02,  3.101170e-04,  2.756900e-04,  3.211090e-04}};

				static double ff_c_f1_17_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_c_wtot_17_chisq[10][8]=
				{{ 1.222880e-01,  1.229410e-01,  9.770390e-02,  1.533010e-01,  9.990000e+02,  1.060940e-01,  1.605870e-01,  1.025050e-01},
				{ 9.146380e-02,  8.194210e-02,  1.087570e-01,  1.148800e-01,  9.990000e+02,  1.019640e-01,  1.366290e-01,  1.148100e-01},
				{ 8.584850e-02,  1.260440e-01,  1.230280e-01,  1.268580e-01,  9.990000e+02,  8.125300e-02,  1.453650e-01,  9.780100e-02},
				{ 6.107070e-02,  1.489160e-01,  1.235660e-01,  1.099690e-01,  9.990000e+02,  1.098280e-01,  1.398560e-01,  1.002480e-01},
				{ 5.128200e-02,  6.754500e-02,  1.376100e-01,  1.513200e-01,  9.990000e+02,  8.385100e-02,  1.340360e-01,  7.727200e-02},
				{ 5.402800e-02,  6.237000e-02,  8.962300e-02,  1.249300e-01,  9.990000e+02,  1.126950e-01,  1.357290e-01,  7.460900e-02},
				{ 4.631100e-02,  4.029500e-02,  6.733200e-02,  1.341100e-01,  9.990000e+02,  1.327310e-01,  1.120430e-01,  9.750700e-02},
				{ 2.976500e-02,  7.269800e-02,  8.779300e-02,  6.984700e-02,  9.990000e+02,  7.037400e-02,  1.611210e-01,  6.218300e-02},
				{ 3.311000e-02,  1.605800e-01,  1.645100e-01,  8.243500e-02,  9.990000e+02,  4.762100e-02,  1.292780e-01,  1.060400e-01},
				{ 3.178600e-02,  9.076000e-02,  1.163800e-01,  1.030100e-01,  9.990000e+02,  1.402920e-01,  1.299760e-01,  9.557600e-02}};

				static double ff_c_fside_17_chisq[10][8]=
				{{ 2.335900e-02,  2.998660e-02,  2.900450e-02,  2.850360e-02,  9.990000e+02,  3.243150e-02,  3.743550e-02,  3.203750e-02},
				{ 1.650110e-02,  1.728890e-02,  3.146560e-02,  2.698820e-02,  9.990000e+02,  2.962640e-02,  3.535550e-02,  3.014950e-02},
				{ 1.593160e-02,  2.878700e-02,  2.480020e-02,  3.339640e-02,  9.990000e+02,  2.403100e-02,  3.523990e-02,  2.611660e-02},
				{ 1.744020e-02,  2.493620e-02,  3.115220e-02,  3.163760e-02,  9.990000e+02,  2.664390e-02,  3.076980e-02,  3.027190e-02},
				{ 9.073500e-03,  2.866100e-02,  3.721100e-02,  3.820500e-02,  9.990000e+02,  3.256700e-02,  3.813200e-02,  2.868030e-02},
				{ 9.722700e-03,  1.790000e-02,  2.374100e-02,  3.090100e-02,  9.990000e+02,  3.381400e-02,  4.426200e-02,  2.221890e-02},
				{ 1.113200e-02,  1.974600e-02,  2.390400e-02,  3.442000e-02,  9.990000e+02,  3.063200e-02,  3.914370e-02,  2.822840e-02},
				{ 4.366500e-03,  2.358900e-02,  1.795200e-02,  2.883600e-02,  9.990000e+02,  2.370200e-02,  3.981940e-02,  2.422940e-02},
				{ 1.078200e-02,  3.224200e-02,  3.343400e-02,  3.259600e-02,  9.990000e+02,  2.923000e-02,  4.240920e-02,  2.767180e-02},
				{ 8.395300e-03,  1.822700e-02,  3.000900e-02,  2.897100e-02,  9.990000e+02,  3.787100e-02,  3.570750e-02,  2.988630e-02}};

				static double ff_c_w1_17_chisq[10][8]=
				{{ 1.101750e-02,  1.617250e-02,  1.258030e-02,  1.342120e-02,  9.990000e+02,  1.360290e-02,  2.383380e-02,  2.745760e-02},
				{ 7.071550e-03,  8.596630e-03,  1.277860e-02,  1.171640e-02,  9.990000e+02,  1.380450e-02,  2.427370e-02,  2.216130e-02},
				{ 7.680570e-03,  9.606200e-03,  1.181320e-02,  1.229610e-02,  9.990000e+02,  1.337980e-02,  2.019790e-02,  2.432310e-02},
				{ 5.114660e-03,  1.277920e-02,  1.350950e-02,  1.242010e-02,  9.990000e+02,  1.228570e-02,  2.584320e-02,  2.561780e-02},
				{ 4.276500e-03,  1.711700e-02,  2.229300e-02,  8.055400e-03,  9.990000e+02,  1.816960e-02,  2.311510e-02,  2.684390e-02},
				{ 8.331700e-03,  2.309600e-02,  1.177600e-02,  1.319300e-02,  9.990000e+02,  1.188930e-02,  2.550820e-02,  2.902860e-02},
				{ 1.855600e-03,  7.041600e-03,  8.859800e-03,  1.377400e-02,  9.990000e+02,  8.470700e-03,  3.441620e-02,  2.804560e-02},
				{ 2.492100e-03,  2.734100e-03,  1.452500e-02,  1.114200e-02,  9.990000e+02,  8.680800e-03,  2.953380e-02,  2.707720e-02},
				{ 1.104200e-02,  8.210000e-03,  1.926800e-02,  1.739800e-02,  9.990000e+02,  1.931590e-02,  2.945540e-02,  2.776390e-02},
				{ 4.320700e-03,  1.162600e-02,  1.596200e-02,  1.792600e-02,  9.990000e+02,  1.715390e-02,  2.369400e-02,  2.405220e-02}};

				static double ff_c_eratio_17_chisq[10][8]=
				{{-2.908380e-03, -3.412070e-03, -3.410160e-03,  5.047550e-05,  9.990000e+02, -2.764530e-03, -2.750400e-03,  2.767018e-04},
				{-1.113450e-04,  1.488320e-03,  7.031360e-04, -3.512720e-03,  9.990000e+02, -2.070070e-03, -6.362100e-05,  4.995090e-04},
				{ 7.119750e-04, -5.030640e-03, -2.118950e-04, -3.070170e-03,  9.990000e+02, -1.215350e-03, -9.689000e-04,  4.726200e-04},
				{-1.704650e-03,  2.620460e-03, -5.336220e-03,  4.755970e-03,  9.990000e+02, -5.904100e-04, -2.151250e-03,  3.954620e-03},
				{-1.051900e-03,  5.428800e-03, -2.092400e-03, -9.147700e-04,  9.990000e+02, -1.906530e-03,  1.221189e-03,  7.183300e-04},
				{ 8.298800e-04,  8.514700e-05,  1.583300e-03, -1.744800e-03,  9.990000e+02, -1.088400e-03,  2.229820e-04, -2.714300e-04},
				{ 2.024700e-03, -1.287100e-03,  7.544000e-04, -8.198900e-03,  9.990000e+02, -8.686000e-04, -2.156190e-03, -9.002800e-04},
				{ 2.755900e-03, -2.158100e-04, -1.759800e-03,  3.219400e-03,  9.990000e+02,  7.100000e-06,  1.781090e-03,  2.540070e-03},
				{-2.594600e-04,  5.075800e-03,  9.683000e-04, -5.059700e-03,  9.990000e+02,  3.111000e-04, -1.079810e-03, -3.031695e-03},
				{ 4.039400e-04, -1.103200e-03, -3.296700e-03, -4.616600e-03,  9.990000e+02, -2.345300e-03, -1.117590e-03,  1.375665e-03}};

				static double ff_c_deltae_17_chisq[10][8]=
				{{-2.543520e+00, -1.688800e+00, -3.377220e+00, -1.237400e+00,  9.990000e+02, -1.657190e+00, -2.512930e+00,  3.684390e+00},
				{-1.761790e+00, -1.008200e+00, -1.790120e+00, -9.705140e-01,  9.990000e+02, -1.087080e+00, -1.736830e+00,  5.523750e+00},
				{-1.987940e+00, -2.069320e+00, -2.760860e+00, -4.116430e-01,  9.990000e+02, -1.619370e+00, -7.769930e+00,  8.485770e+00},
				{-1.346640e+00,  2.518620e+00,  4.606120e-01, -7.047010e-02,  9.990000e+02, -2.243624e+00, -2.239720e+00,  2.905860e+00},
				{ 5.325800e-01, -3.605900e+00, -3.191800e+00, -9.423800e-01,  9.990000e+02, -9.738280e-01, -6.665800e+00, -2.298900e-01},
				{-2.361600e+00, -1.126000e+00, -2.245600e+00, -2.037400e+00,  9.990000e+02, -9.424800e-01, -2.074120e+00, -2.304200e+00},
				{ 7.437200e-01,  6.545800e-01,  4.316000e-01,  2.751700e+00,  9.990000e+02,  3.786260e+00, -9.807730e+00, -1.430330e+00},
				{-4.825700e-01, -1.574900e+00,  2.189500e+00, -7.564100e+00,  9.990000e+02, -6.454340e+00, -2.468770e+00, -1.302230e+01},
				{-8.177900e+00, -7.065900e+00, -8.250500e+00, -7.478400e+00,  9.990000e+02,  8.015222e+00,  7.366290e+00,  5.046900e+00},
				{-5.471000e-01, -2.580100e+00, -1.458000e+00,  5.475700e-01,  9.990000e+02, -3.312667e+00, -4.771440e+00, -9.790000e-01}};

				static double ff_c_e277_17_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				// unconverted photons -- errors

				static double ff_u_rhad_err_17_chisq[10][8]=
				{{7.816820e-05, 1.371270e-04, 1.448240e-04, 1.768440e-04, 9.990000e+02, 7.148960e-04, 7.912060e-04, 5.250249e-04},
				{7.671250e-05, 1.335420e-04, 1.330740e-04, 1.822500e-04, 9.990000e+02, 5.889140e-04, 7.580930e-04, 4.895308e-04},
				{8.166250e-05, 1.386590e-04, 1.488820e-04, 1.754100e-04, 9.990000e+02, 6.830590e-04, 7.509490e-04, 5.250599e-04},
				{9.170700e-05, 1.532740e-04, 1.654150e-04, 2.135700e-04, 9.990000e+02, 7.546220e-04, 8.248492e-04, 5.825258e-04},
				{2.546400e-03, 2.432400e-03, 2.955000e-03, 2.740700e-03, 0.000000e+00, 1.060100e-02, 9.454005e-03, 7.355908e-03},
				{2.240700e-03, 1.906600e-03, 2.531200e-03, 2.218700e-03, 0.000000e+00, 8.707900e-03, 7.713907e-03, 5.799416e-03},
				{1.929900e-03, 1.651300e-03, 1.917600e-03, 1.918100e-03, 0.000000e+00, 7.822000e-03, 7.411006e-03, 4.595610e-03},
				{1.873600e-03, 1.372900e-03, 1.791600e-03, 1.484500e-03, 0.000000e+00, 6.101600e-03, 5.171008e-03, 3.954100e-03},
				{1.704500e-03, 1.435900e-03, 1.394400e-03, 1.337100e-03, 0.000000e+00, 4.046708e-03, 3.680700e-03, 3.110108e-03},
				{1.735500e-03, 9.419800e-04, 1.016700e-03, 9.508100e-04, 0.000000e+00, 3.593740e-03, 2.958737e-03, 1.940885e-03}};

				static double ff_u_reta_err_17_chisq[10][8]=
				{{4.061260e-04, 7.472750e-04, 5.293670e-04, 7.202290e-04, 9.990000e+02, 6.753160e-04, 5.916251e-04, 5.950171e-04},
				{4.067600e-04, 6.783300e-04, 5.207220e-04, 6.792150e-04, 9.990000e+02, 6.819300e-04, 6.638536e-04, 5.583203e-04},
				{3.762380e-04, 6.918620e-04, 5.318430e-04, 6.910770e-04, 9.990000e+02, 7.484410e-04, 7.597991e-04, 7.050344e-04},
				{4.568180e-04, 8.642160e-04, 6.547620e-04, 8.714350e-04, 9.990000e+02, 8.066583e-04, 8.617745e-04, 8.441582e-04},
				{1.584500e-02, 1.651300e-02, 1.416000e-02, 1.369400e-02, 0.000000e+00, 1.449002e-02, 1.113002e-02, 1.380800e-02},
				{1.235600e-02, 1.362500e-02, 1.241400e-02, 1.174300e-02, 0.000000e+00, 1.219501e-02, 1.017800e-02, 1.224101e-02},
				{1.145200e-02, 1.117300e-02, 9.737900e-03, 9.333000e-03, 0.000000e+00, 1.118502e-02, 9.676702e-03, 1.111100e-02},
				{9.405800e-03, 1.196200e-02, 8.283200e-03, 8.277900e-03, 0.000000e+00, 1.092402e-02, 1.039300e-02, 1.102600e-02},
				{8.515800e-03, 8.199800e-03, 7.886400e-03, 7.756000e-03, 0.000000e+00, 1.074302e-02, 7.724401e-03, 1.040400e-02},
				{6.780100e-03, 7.274900e-03, 7.184100e-03, 7.298000e-03, 0.000000e+00, 9.400517e-03, 7.297803e-03, 9.812905e-03}};

				static double ff_u_rphi_err_17_chisq[10][8]=
				{{3.013950e-04, 5.796680e-04, 4.449350e-04, 7.089640e-04, 9.990000e+02, 5.409800e-04, 4.897928e-04, 4.298121e-04},
				{2.791850e-04, 5.148330e-04, 4.571240e-04, 7.182770e-04, 9.990000e+02, 5.821819e-04, 5.744728e-04, 4.968364e-04},
				{3.093260e-04, 5.964170e-04, 5.085510e-04, 7.834240e-04, 9.990000e+02, 6.682064e-04, 6.281147e-04, 5.278764e-04},
				{3.380590e-04, 8.234220e-04, 6.434340e-04, 9.622860e-04, 9.990000e+02, 7.471134e-04, 7.535452e-04, 7.681853e-04},
				{1.181300e-02, 1.261800e-02, 1.393700e-02, 1.920400e-02, 0.000000e+00, 1.425307e-02, 1.092102e-02, 1.309401e-02},
				{1.034700e-02, 1.390500e-02, 1.173500e-02, 1.507600e-02, 0.000000e+00, 1.326504e-02, 9.273840e-03, 1.137603e-02},
				{9.389600e-03, 1.107800e-02, 1.101000e-02, 1.462800e-02, 0.000000e+00, 1.177106e-02, 8.598377e-03, 1.028903e-02},
				{8.474100e-03, 1.012600e-02, 9.659100e-03, 1.151900e-02, 0.000000e+00, 1.069709e-02, 9.044136e-03, 1.070202e-02},
				{7.604200e-03, 8.656000e-03, 8.395200e-03, 9.115500e-03, 0.000000e+00, 1.133413e-02, 9.004538e-03, 8.553332e-03},
				{6.546600e-03, 7.195100e-03, 7.734600e-03, 9.390700e-03, 0.000000e+00, 9.269173e-03, 7.450041e-03, 8.748830e-03}};

				static double ff_u_weta2_err_17_chisq[10][8]=
				{{1.181070e-05, 2.367640e-05, 1.713990e-05, 2.311950e-05, 9.990000e+02, 2.534150e-05, 2.519806e-05, 1.921871e-05},
				{1.273720e-05, 2.387360e-05, 1.876130e-05, 2.622050e-05, 9.990000e+02, 2.699910e-05, 2.827031e-05, 2.334549e-05},
				{1.570450e-05, 2.730060e-05, 2.240090e-05, 2.721060e-05, 9.990000e+02, 3.226570e-05, 3.546606e-05, 2.800376e-05},
				{1.827040e-05, 3.802290e-05, 2.763670e-05, 3.660410e-05, 9.990000e+02, 3.780940e-05, 4.535020e-05, 3.875341e-05},
				{6.444100e-04, 7.105000e-04, 6.405900e-04, 6.403700e-04, 0.000000e+00, 7.159000e-04, 6.339400e-04, 5.986303e-04},
				{5.615300e-04, 6.891300e-04, 5.688000e-04, 5.464300e-04, 0.000000e+00, 6.366000e-04, 5.535000e-04, 5.661903e-04},
				{5.250000e-04, 6.488800e-04, 5.528200e-04, 5.238700e-04, 0.000000e+00, 5.922000e-04, 5.820004e-04, 5.243607e-04},
				{4.996900e-04, 6.018000e-04, 4.823500e-04, 5.085300e-04, 0.000000e+00, 6.012300e-04, 5.118001e-04, 5.483504e-04},
				{4.359800e-04, 5.081900e-04, 4.646200e-04, 4.511100e-04, 0.000000e+00, 6.002300e-04, 5.932904e-04, 5.770607e-04},
				{3.908600e-04, 4.580800e-04, 3.862700e-04, 4.389800e-04, 0.000000e+00, 5.351509e-04, 5.064901e-04, 4.964505e-04}};

				static double ff_u_f1_err_17_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_wtot_err_17_chisq[10][8]=
				{{5.873830e-03, 1.032790e-02, 1.026160e-02, 1.202360e-02, 9.990000e+02, 1.391240e-02, 1.051539e-02, 6.206957e-03},
				{7.215800e-03, 1.055230e-02, 1.239210e-02, 1.438900e-02, 9.990000e+02, 1.732470e-02, 1.078503e-02, 7.047515e-03},
				{8.400990e-03, 1.346300e-02, 1.432300e-02, 1.582140e-02, 9.990000e+02, 1.700380e-02, 1.466400e-02, 8.090872e-03},
				{1.067040e-02, 1.602950e-02, 1.774210e-02, 2.450290e-02, 9.990000e+02, 2.563980e-02, 1.883235e-02, 1.081564e-02},
				{3.314000e-01, 3.182700e-01, 3.586600e-01, 3.780200e-01, 0.000000e+00, 3.966400e-01, 1.735904e-01, 1.612109e-01},
				{2.766300e-01, 3.032400e-01, 3.485100e-01, 3.380000e-01, 0.000000e+00, 3.544800e-01, 2.076005e-01, 1.408307e-01},
				{2.801200e-01, 2.788300e-01, 3.536200e-01, 2.976900e-01, 0.000000e+00, 3.257007e-01, 1.530705e-01, 1.451909e-01},
				{2.634000e-01, 2.611000e-01, 3.135300e-01, 3.154300e-01, 0.000000e+00, 3.453606e-01, 1.396610e-01, 1.306414e-01},
				{2.330300e-01, 2.235900e-01, 3.104500e-01, 3.238900e-01, 0.000000e+00, 3.666808e-01, 1.346407e-01, 1.305014e-01},
				{1.949900e-01, 2.359400e-01, 2.824600e-01, 3.143200e-01, 0.000000e+00, 2.603914e-01, 1.344119e-01, 1.098920e-01}};

				static double ff_u_fside_err_17_chisq[10][8]=
				{{9.242800e-04, 1.905240e-03, 2.064350e-03, 2.713380e-03, 9.990000e+02, 2.453480e-03, 1.885231e-03, 1.295955e-03},
				{1.075200e-03, 2.271290e-03, 2.340870e-03, 3.370390e-03, 9.990000e+02, 3.124270e-03, 2.327725e-03, 1.520565e-03},
				{1.199340e-03, 2.453410e-03, 2.855540e-03, 3.834800e-03, 9.990000e+02, 3.715880e-03, 2.440614e-03, 1.887409e-03},
				{1.622950e-03, 3.429960e-03, 3.494760e-03, 4.831340e-03, 9.990000e+02, 4.759330e-03, 3.014197e-03, 2.228307e-03},
				{6.265300e-02, 7.331200e-02, 8.788200e-02, 9.458900e-02, 0.000000e+00, 9.715400e-02, 4.243501e-02, 4.174105e-02},
				{5.600700e-02, 7.342000e-02, 8.466000e-02, 9.180300e-02, 0.000000e+00, 8.865600e-02, 4.206703e-02, 3.792605e-02},
				{5.348000e-02, 6.837900e-02, 8.023800e-02, 8.698600e-02, 0.000000e+00, 7.785222e-02, 3.637902e-02, 3.860811e-02},
				{4.614200e-02, 5.717400e-02, 7.320600e-02, 7.785800e-02, 0.000000e+00, 8.181319e-02, 3.592001e-02, 3.668516e-02},
				{4.689300e-02, 5.569700e-02, 6.134600e-02, 7.071600e-02, 0.000000e+00, 8.557700e-02, 3.872805e-02, 3.056729e-02},
				{3.934200e-02, 4.881000e-02, 5.899800e-02, 6.022200e-02, 0.000000e+00, 6.148300e-02, 2.839022e-02, 2.724444e-02}};

				static double ff_u_w1_err_17_chisq[10][8]=
				{{6.889220e-04, 1.470730e-03, 1.283290e-03, 1.487350e-03, 9.990000e+02, 1.722376e-03, 1.807972e-03, 1.235857e-03},
				{8.748210e-04, 1.648580e-03, 1.560020e-03, 1.634690e-03, 9.990000e+02, 2.113828e-03, 2.180871e-03, 1.558069e-03},
				{1.071320e-03, 1.971620e-03, 1.912610e-03, 2.008230e-03, 9.990000e+02, 2.851104e-03, 2.821164e-03, 1.953970e-03},
				{1.446100e-03, 2.821910e-03, 2.680670e-03, 3.044580e-03, 9.990000e+02, 3.372165e-03, 3.694483e-03, 2.594028e-03},
				{5.421800e-02, 5.679000e-02, 6.570700e-02, 4.521100e-02, 0.000000e+00, 5.438103e-02, 4.928602e-02, 4.498605e-02},
				{4.936300e-02, 5.788300e-02, 6.064100e-02, 4.544400e-02, 0.000000e+00, 5.560106e-02, 4.937405e-02, 4.616802e-02},
				{5.186800e-02, 5.200400e-02, 6.418800e-02, 4.155700e-02, 0.000000e+00, 5.235204e-02, 4.331301e-02, 4.348901e-02},
				{5.095500e-02, 4.871000e-02, 5.813200e-02, 4.350800e-02, 0.000000e+00, 5.281901e-02, 5.327802e-02, 4.197401e-02},
				{4.372100e-02, 5.451900e-02, 5.650300e-02, 4.409200e-02, 0.000000e+00, 5.870902e-02, 5.212402e-02, 3.506502e-02},
				{3.845300e-02, 4.560200e-02, 5.393200e-02, 4.619700e-02, 0.000000e+00, 5.399001e-02, 4.249502e-02, 3.287801e-02}};

				static double ff_u_eratio_err_17_chisq[10][8]=
				{{3.614060e-04, 6.248950e-04, 8.149090e-04, 1.286320e-03, 9.990000e+02, 7.554915e-04, 6.479990e-04, 4.956033e-04},
				{4.387110e-04, 6.686990e-04, 9.372190e-04, 1.130390e-03, 9.990000e+02, 7.175556e-04, 7.317704e-04, 6.032952e-04},
				{5.534840e-04, 8.781930e-04, 9.179880e-04, 1.094100e-03, 9.990000e+02, 1.134776e-03, 7.469567e-04, 6.608264e-04},
				{4.309700e-04, 1.075910e-03, 1.324440e-03, 2.207310e-03, 9.990000e+02, 4.958503e-04, 1.231546e-03, 9.335795e-04},
				{2.047700e-02, 2.301700e-02, 2.746700e-02, 2.985400e-02, 0.000000e+00, 1.622007e-02, 1.435804e-02, 1.408638e-02},
				{1.842600e-02, 1.922000e-02, 2.834200e-02, 2.801600e-02, 0.000000e+00, 1.558192e-02, 1.594299e-02, 1.392376e-02},
				{1.769900e-02, 1.794900e-02, 2.117600e-02, 2.177300e-02, 0.000000e+00, 1.373494e-02, 1.427374e-02, 1.301052e-02},
				{1.514900e-02, 1.575300e-02, 2.149000e-02, 2.545000e-02, 0.000000e+00, 1.018179e-02, 1.232565e-02, 1.158042e-02},
				{1.504800e-02, 1.460400e-02, 2.099500e-02, 2.020100e-02, 0.000000e+00, 1.118762e-02, 1.091702e-02, 1.044691e-02},
				{1.300000e-02, 1.357000e-02, 1.918300e-02, 1.989800e-02, 0.000000e+00, 9.544917e-03, 9.537604e-03, 1.052729e-02}};

				static double ff_u_deltae_err_17_chisq[10][8]=
				{{1.818410e-01, 4.903280e-01, 1.699010e+00, 3.606930e-01, 9.990000e+02, 6.625630e-01, 1.304210e+00, 1.298030e+00},
				{2.934720e-01, 4.196190e-01, 5.884050e-01, 6.058220e-01, 9.990000e+02, 7.406810e-01, 1.252070e+00, 1.506610e+00},
				{4.196260e-01, 5.529750e-01, 8.119680e-01, 6.679960e-01, 9.990000e+02, 6.700850e-01, 2.610490e+00, 1.084860e+00},
				{5.556910e-01, 8.845690e-01, 5.869480e-01, 3.940610e-01, 9.990000e+02, 8.770020e-01, 2.324470e+00, 2.041720e+00},
				{2.447800e+01, 2.235800e+01, 2.056100e+01, 2.043600e+01, 0.000000e+00, 3.170900e+01, 3.291300e+01, 3.417000e+01},
				{2.352200e+01, 2.345400e+01, 2.161800e+01, 2.001300e+01, 0.000000e+00, 2.916400e+01, 3.433400e+01, 3.338000e+01},
				{2.429300e+01, 2.364900e+01, 2.044700e+01, 2.157500e+01, 0.000000e+00, 3.138000e+01, 4.095000e+01, 3.395200e+01},
				{2.260000e+01, 2.393500e+01, 2.028900e+01, 2.136800e+01, 0.000000e+00, 3.037800e+01, 2.892500e+01, 3.009800e+01},
				{2.390500e+01, 2.395800e+01, 2.039100e+01, 1.888600e+01, 0.000000e+00, 2.985100e+01, 4.322800e+01, 3.513900e+01},
				{2.217000e+01, 2.349700e+01, 1.916300e+01, 1.971300e+01, 0.000000e+00, 2.856900e+01, 3.028700e+01, 2.610800e+01}};

				static double ff_u_e277_err_17_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				// converted photons -- errors

				static double ff_c_rhad_err_17_chisq[10][8]=
				{{1.764210e-04, 2.805140e-04, 2.123310e-04, 2.566150e-04, 9.990000e+02, 9.212290e-04, 9.313661e-04, 7.023264e-04},
				{1.596980e-04, 2.678630e-04, 2.067960e-04, 2.361580e-04, 9.990000e+02, 7.529430e-04, 9.357777e-04, 6.833467e-04},
				{1.630440e-04, 2.561500e-04, 2.043390e-04, 2.163140e-04, 9.990000e+02, 6.925690e-04, 1.357054e-03, 8.463884e-04},
				{1.867700e-04, 2.726730e-04, 2.353340e-04, 2.788550e-04, 9.990000e+02, 1.032270e-03, 1.092466e-03, 8.068129e-04},
				{2.272100e-03, 2.258500e-03, 2.772900e-03, 3.039600e-03, 0.000000e+00, 1.086000e-02, 1.026801e-02, 7.097608e-03},
				{2.125600e-03, 1.899500e-03, 2.247100e-03, 2.452000e-03, 0.000000e+00, 8.524600e-03, 7.625305e-03, 5.710109e-03},
				{1.711800e-03, 1.782300e-03, 1.884200e-03, 1.793100e-03, 0.000000e+00, 7.824900e-03, 6.421305e-03, 5.077105e-03},
				{1.782900e-03, 1.375900e-03, 1.769400e-03, 1.840400e-03, 0.000000e+00, 5.211307e-03, 4.844410e-03, 3.576000e-03},
				{1.389700e-03, 9.675500e-04, 1.539700e-03, 1.296500e-03, 0.000000e+00, 4.609305e-03, 3.405300e-03, 2.818610e-03},
				{1.366300e-03, 9.955400e-04, 1.079500e-03, 8.976300e-04, 0.000000e+00, 3.128757e-03, 2.555947e-03, 2.074479e-03}};

				static double ff_c_reta_err_17_chisq[10][8]=
				{{1.353850e-03, 2.101190e-03, 1.153040e-03, 1.304910e-03, 9.990000e+02, 1.213000e-03, 1.066599e-03, 9.853280e-04},
				{9.587090e-04, 1.889290e-03, 9.984070e-04, 1.297180e-03, 9.990000e+02, 9.302170e-04, 8.440004e-04, 1.010190e-03},
				{9.758200e-04, 1.823290e-03, 9.553850e-04, 1.193320e-03, 9.990000e+02, 1.002220e-03, 9.845894e-04, 1.268420e-03},
				{1.198460e-03, 1.799740e-03, 1.160200e-03, 1.322800e-03, 9.990000e+02, 1.221140e-03, 1.103298e-03, 1.406619e-03},
				{1.967200e-02, 1.961300e-02, 1.505900e-02, 1.816900e-02, 0.000000e+00, 1.590200e-02, 1.116601e-02, 1.364902e-02},
				{1.414400e-02, 1.655300e-02, 1.460700e-02, 1.525300e-02, 0.000000e+00, 1.413206e-02, 1.074701e-02, 1.350901e-02},
				{1.251500e-02, 1.314100e-02, 1.259800e-02, 1.296700e-02, 0.000000e+00, 1.187805e-02, 9.607101e-03, 1.167601e-02},
				{1.361000e-02, 1.280800e-02, 1.090500e-02, 1.396600e-02, 0.000000e+00, 1.225603e-02, 8.862908e-03, 1.113603e-02},
				{1.114100e-02, 1.122900e-02, 8.074900e-03, 1.064300e-02, 0.000000e+00, 1.109403e-02, 6.946801e-03, 1.115400e-02},
				{7.123000e-03, 7.932100e-03, 7.628100e-03, 7.883900e-03, 0.000000e+00, 9.673919e-03, 7.144105e-03, 8.937700e-03}};

				static double ff_c_rphi_err_17_chisq[10][8]=
				{{1.419220e-03, 2.592470e-03, 1.796380e-03, 2.683160e-03, 9.990000e+02, 1.544850e-03, 1.404815e-03, 1.805079e-03},
				{1.396990e-03, 2.607860e-03, 1.779540e-03, 3.166430e-03, 9.990000e+02, 1.706150e-03, 1.421520e-03, 2.378321e-03},
				{1.647170e-03, 3.104280e-03, 1.967330e-03, 3.104730e-03, 9.990000e+02, 1.816820e-03, 2.043584e-03, 3.237190e-03},
				{1.892560e-03, 3.659090e-03, 2.495940e-03, 2.719040e-03, 9.990000e+02, 1.953920e-03, 2.073140e-03, 2.288616e-03},
				{3.015800e-02, 3.872200e-02, 5.222200e-02, 6.264300e-02, 0.000000e+00, 4.227367e-02, 2.563229e-02, 2.604396e-02},
				{2.627400e-02, 5.300300e-02, 4.254900e-02, 4.852400e-02, 0.000000e+00, 3.268902e-02, 2.082134e-02, 2.301989e-02},
				{2.941400e-02, 5.690500e-02, 3.306500e-02, 3.624100e-02, 0.000000e+00, 2.494850e-02, 1.658346e-02, 1.989696e-02},
				{1.976400e-02, 3.381300e-02, 2.675500e-02, 3.149700e-02, 0.000000e+00, 2.405750e-02, 1.764106e-02, 1.605992e-02},
				{1.825600e-02, 1.744300e-02, 1.906500e-02, 2.297400e-02, 0.000000e+00, 1.876521e-02, 1.315804e-02, 1.271170e-02},
				{1.292200e-02, 1.310200e-02, 1.447500e-02, 1.561300e-02, 0.000000e+00, 1.459708e-02, 1.171648e-02, 1.184425e-02}};

				static double ff_c_weta2_err_17_chisq[10][8]=
				{{3.288140e-05, 6.566000e-05, 3.412930e-05, 4.678110e-05, 9.990000e+02, 3.952580e-05, 3.831910e-05, 3.007210e-05},
				{3.514140e-05, 5.962530e-05, 3.249150e-05, 4.372410e-05, 9.990000e+02, 3.706470e-05, 3.708941e-05, 3.538020e-05},
				{3.674310e-05, 5.973460e-05, 3.516920e-05, 4.308100e-05, 9.990000e+02, 4.317740e-05, 5.223007e-05, 4.444270e-05},
				{4.336740e-05, 8.605270e-05, 4.172490e-05, 5.667390e-05, 9.990000e+02, 5.250180e-05, 5.658467e-05, 5.314800e-05},
				{7.093200e-04, 8.728800e-04, 7.288700e-04, 9.235400e-04, 0.000000e+00, 8.274800e-04, 6.806500e-04, 5.813001e-04},
				{6.988500e-04, 8.059800e-04, 6.446600e-04, 7.324900e-04, 0.000000e+00, 7.518700e-04, 6.814800e-04, 5.825000e-04},
				{5.490200e-04, 7.383200e-04, 6.073000e-04, 6.390900e-04, 0.000000e+00, 7.161100e-04, 6.182000e-04, 5.512600e-04},
				{5.330500e-04, 6.744800e-04, 5.239200e-04, 6.643200e-04, 0.000000e+00, 6.617700e-04, 4.952100e-04, 5.330900e-04},
				{4.162100e-04, 6.744700e-04, 5.242800e-04, 4.773600e-04, 0.000000e+00, 6.298700e-04, 6.101102e-04, 5.677101e-04},
				{3.961000e-04, 4.668000e-04, 4.479700e-04, 4.686600e-04, 0.000000e+00, 6.068900e-04, 5.032302e-04, 4.508602e-04}};

				static double ff_c_f1_err_17_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_c_wtot_err_17_chisq[10][8]=
				{{1.248220e-02, 1.941250e-02, 1.690880e-02, 1.786060e-02, 9.990000e+02, 1.838620e-02, 1.155734e-02, 6.648185e-03},
				{1.419950e-02, 2.290940e-02, 1.552280e-02, 1.892570e-02, 9.990000e+02, 1.724740e-02, 1.147865e-02, 7.403438e-03},
				{1.468090e-02, 2.339940e-02, 1.727210e-02, 1.755100e-02, 9.990000e+02, 2.021400e-02, 1.361996e-02, 9.760420e-03},
				{1.741030e-02, 2.884680e-02, 2.128230e-02, 2.502250e-02, 9.990000e+02, 2.447180e-02, 1.652287e-02, 9.601123e-03},
				{2.843300e-01, 2.884300e-01, 3.846300e-01, 3.235700e-01, 0.000000e+00, 4.193900e-01, 1.887702e-01, 1.247907e-01},
				{2.474900e-01, 2.679100e-01, 3.437200e-01, 3.208800e-01, 0.000000e+00, 3.832500e-01, 1.545902e-01, 1.260909e-01},
				{2.148600e-01, 2.699300e-01, 3.133200e-01, 2.993600e-01, 0.000000e+00, 3.382400e-01, 1.600003e-01, 1.177009e-01},
				{2.077600e-01, 2.251200e-01, 3.112900e-01, 3.506400e-01, 0.000000e+00, 3.417109e-01, 1.792102e-01, 1.207909e-01},
				{2.009100e-01, 2.045100e-01, 2.790600e-01, 2.765200e-01, 0.000000e+00, 4.162006e-01, 1.244602e-01, 1.177014e-01},
				{1.747700e-01, 2.099600e-01, 2.756700e-01, 2.838200e-01, 0.000000e+00, 3.022819e-01, 1.340103e-01, 1.135219e-01}};

				static double ff_c_fside_err_17_chisq[10][8]=
				{{2.047640e-03, 4.345300e-03, 3.388440e-03, 4.317280e-03, 9.990000e+02, 4.136115e-03, 2.302647e-03, 1.585170e-03},
				{1.937920e-03, 4.849200e-03, 3.775350e-03, 4.880880e-03, 9.990000e+02, 4.306397e-03, 2.463732e-03, 1.885909e-03},
				{2.415360e-03, 5.313790e-03, 4.248250e-03, 5.117720e-03, 9.990000e+02, 5.333957e-03, 2.818333e-03, 2.384756e-03},
				{2.868750e-03, 6.616360e-03, 4.881070e-03, 6.971010e-03, 9.990000e+02, 5.839265e-03, 3.458802e-03, 2.853748e-03},
				{6.086600e-02, 6.997900e-02, 9.091400e-02, 9.870000e-02, 0.000000e+00, 1.111000e-01, 4.623802e-02, 3.502318e-02},
				{5.488300e-02, 5.981800e-02, 8.542700e-02, 9.534800e-02, 0.000000e+00, 1.012300e-01, 4.004006e-02, 3.866819e-02},
				{4.544300e-02, 6.179700e-02, 7.674900e-02, 9.724100e-02, 0.000000e+00, 9.051400e-02, 3.647413e-02, 3.419423e-02},
				{4.343900e-02, 5.597000e-02, 7.748200e-02, 8.107700e-02, 0.000000e+00, 1.074000e-01, 3.713710e-02, 2.908328e-02},
				{3.331300e-02, 5.079700e-02, 6.148300e-02, 7.764800e-02, 0.000000e+00, 9.155937e-02, 3.066510e-02, 2.958538e-02},
				{3.557700e-02, 4.653900e-02, 5.443600e-02, 5.969400e-02, 0.000000e+00, 6.957900e-02, 3.089511e-02, 2.536246e-02}};

				static double ff_c_w1_err_17_chisq[10][8]=
				{{1.506820e-03, 2.831600e-03, 2.074250e-03, 2.021050e-03, 9.990000e+02, 1.590100e-03, 2.455608e-03, 1.801975e-03},
				{1.696300e-03, 3.522540e-03, 1.838580e-03, 2.411320e-03, 9.990000e+02, 1.937610e-03, 2.381047e-03, 2.062673e-03},
				{1.932770e-03, 3.820250e-03, 2.440120e-03, 2.460110e-03, 9.990000e+02, 2.545530e-03, 3.645261e-03, 2.940141e-03},
				{2.768430e-03, 4.669830e-03, 3.151250e-03, 2.992090e-03, 9.990000e+02, 3.000920e-03, 3.933258e-03, 3.455854e-03},
				{4.637000e-02, 5.097900e-02, 4.918300e-02, 4.974500e-02, 0.000000e+00, 4.846500e-02, 5.123303e-02, 4.036705e-02},
				{4.342300e-02, 5.502300e-02, 5.420200e-02, 4.510700e-02, 0.000000e+00, 4.863603e-02, 4.893502e-02, 4.337607e-02},
				{4.143600e-02, 5.187200e-02, 5.056700e-02, 4.267300e-02, 0.000000e+00, 4.674103e-02, 4.920204e-02, 4.552402e-02},
				{4.503400e-02, 4.945300e-02, 4.937400e-02, 3.413500e-02, 0.000000e+00, 4.598202e-02, 5.106907e-02, 4.740905e-02},
				{3.220300e-02, 4.425400e-02, 4.608200e-02, 4.140500e-02, 0.000000e+00, 4.313303e-02, 4.185708e-02, 4.033302e-02},
				{3.443700e-02, 4.468600e-02, 4.035800e-02, 3.294800e-02, 0.000000e+00, 4.249703e-02, 4.814501e-02, 3.782904e-02}};

				static double ff_c_eratio_err_17_chisq[10][8]=
				{{8.323280e-04, 2.094440e-03, 1.669240e-03, 2.825670e-03, 9.990000e+02, 1.311396e-03, 7.248278e-04, 8.674720e-04},
				{1.174930e-03, 9.828010e-04, 1.352940e-03, 2.454710e-03, 9.990000e+02, 1.224637e-03, 1.184115e-03, 4.410817e-04},
				{9.395750e-04, 4.658300e-04, 1.665580e-03, 2.294620e-03, 9.990000e+02, 1.309752e-03, 7.389144e-04, 4.475596e-04},
				{1.174940e-03, 2.805590e-03, 2.264980e-03, 2.998200e-03, 9.990000e+02, 1.447803e-03, 5.464248e-04, 2.213187e-03},
				{1.999800e-02, 1.987900e-02, 3.626300e-02, 3.615600e-02, 0.000000e+00, 2.085933e-02, 1.511618e-02, 1.316296e-02},
				{1.893900e-02, 1.983500e-02, 3.010700e-02, 3.367200e-02, 0.000000e+00, 1.724470e-02, 1.150381e-02, 1.187556e-02},
				{1.672300e-02, 1.971900e-02, 2.819000e-02, 3.024600e-02, 0.000000e+00, 1.634564e-02, 1.036844e-02, 1.270126e-02},
				{1.691200e-02, 1.536800e-02, 2.602900e-02, 2.717000e-02, 0.000000e+00, 1.397313e-02, 1.063307e-02, 1.167612e-02},
				{1.308700e-02, 1.539800e-02, 1.922900e-02, 2.230600e-02, 0.000000e+00, 1.309317e-02, 8.128516e-03, 8.429754e-03},
				{1.241600e-02, 1.430000e-02, 1.839400e-02, 1.910600e-02, 0.000000e+00, 9.437932e-03, 7.643400e-03, 1.127000e-02}};

				static double ff_c_deltae_err_17_chisq[10][8]=
				{{5.612680e-01, 1.084520e+00, 4.718530e-01, 5.288140e-01, 9.990000e+02, 8.151500e-01, 1.400840e+00, 6.742870e-01},
				{5.455040e-01, 8.538720e-01, 1.019310e+00, 8.277810e-01, 9.990000e+02, 7.248130e-01, 1.591030e+00, 1.101740e+00},
				{6.548010e-01, 1.037350e+00, 6.860910e-01, 8.181900e-01, 9.990000e+02, 1.527590e+00, 4.168400e+00, 7.724040e-01},
				{8.386020e-01, 1.328200e+00, 1.767060e+00, 1.601350e+00, 9.990000e+02, 1.153210e+00, 1.877370e+00, 3.727310e+00},
				{2.291200e+01, 2.996300e+01, 2.360200e+01, 2.354200e+01, 0.000000e+00, 3.239100e+01, 3.441800e+01, 2.870200e+01},
				{2.463100e+01, 2.336800e+01, 2.512600e+01, 2.472100e+01, 0.000000e+00, 3.096400e+01, 3.457100e+01, 3.010900e+01},
				{2.324400e+01, 2.256800e+01, 2.129200e+01, 1.970900e+01, 0.000000e+00, 2.706100e+01, 3.352400e+01, 2.785500e+01},
				{2.470000e+01, 3.524400e+01, 2.080700e+01, 2.584600e+01, 0.000000e+00, 3.808000e+01, 2.963400e+01, 3.393200e+01},
				{2.575000e+01, 2.487600e+01, 2.687600e+01, 2.490200e+01, 0.000000e+00, 2.663000e+01, 2.808800e+01, 2.553000e+01},
				{2.365200e+01, 2.431400e+01, 2.043200e+01, 1.904200e+01, 0.000000e+00, 3.118000e+01, 3.234000e+01, 3.291900e+01}};

				static double ff_c_e277_err_17_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				if (isConv) {

				m_rhad_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_rhad_17_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_rhad_17_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_reta_17_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_rphi_17_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_weta2_17_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_f1_17_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_deltae_17_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_wtot_17_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_fside_17_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_w1_17_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_eratio_17_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_e277_17_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_rhad_err_17_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_rhad_err_17_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_reta_err_17_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_rphi_err_17_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_weta2_err_17_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_f1_err_17_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_deltae_err_17_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_wtot_err_17_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_fside_err_17_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_w1_err_17_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_eratio_err_17_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_c_e277_err_17_chisq[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_rhad_17_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_rhad_17_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_reta_17_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_rphi_17_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_weta2_17_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_f1_17_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_deltae_17_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_wtot_17_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_fside_17_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_w1_17_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_eratio_17_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_e277_17_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_rhad_err_17_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_rhad_err_17_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_reta_err_17_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_rphi_err_17_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_weta2_err_17_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_f1_err_17_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_deltae_err_17_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_wtot_err_17_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_fside_err_17_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_w1_err_17_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_eratio_err_17_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8, pt_DVs_17_chisq, eta_DVs_17_chisq, &ff_u_e277_err_17_chisq[0][0], 0.);

				}

				break;
		case 18: // tune 18 -- done using gamjet MC15b for et>30 and Zllg for et < 30 with 2015 data
                
                static double pt_DVs_18_chisq[10] = {15000, 20000, 25000, 30000, 40000, 50000, 60000, 80000, 100000, 1000000};
				static double eta_DVs_18_chisq[8] = {0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

				// unconverted photons -- values

        static double ff_u_fside_18_chisq[10][8]=
        {{0.034,0.042,0.05,0.05,-0.06,0.058,0.05,0.044},
        {0.024,0.036,0.042,0.04,0.056,0.052,0.048,0.04},
        {0.024,0.036,0.042,0.04,0.056,0.052,0.048,0.04},
        {0.018,0.026,0.03,0.034,0.056,0.04,0.04,0.034},
        {0.016,0.022,0.034,0.04,-0.01,0.032,0.04,0.036},
        {0.016,0.024,0.028,0.032,0.03,0.036,0.038,0.036},
        {0.014,0.022,0.024,0.028,0.056,0.038,0.038,0.036},
        {0.014,0.02,0.024,0.026,0.032,0.032,0.038,0.036},
        {0.012,0.018,0.022,0.026,-0.034,0.03,0.038,0.034},
        {0.012,0.018,0.02,0.024,0.014,0.032,0.038,0.034}};
        static double ff_u_reta_18_chisq[10][8]=
        {{-0.0036,-0.0072,-0.0072,-0.0072,-0.0108,-0.0072,-0.0072,-0.0084},
        {-0.003,-0.0066,-0.006,-0.006,-0.0126,-0.0072,-0.0066,-0.0078},
        {-0.0036,-0.0054,-0.0054,-0.0066,-0.0054,-0.006,-0.0054,-0.0078},
        {-0.003,-0.0042,-0.0048,-0.0054,-0.0138,-0.0054,-0.006,-0.0084},
        {-0.0024,-0.003,-0.0048,-0.0054,-0.0042,-0.0054,-0.006,-0.009},
        {-0.0024,-0.003,-0.0042,-0.0048,-0.0012,-0.0048,-0.0054,-0.0078},
        {-0.0024,-0.0024,-0.0036,-0.0042,-0.0036,-0.0048,-0.0054,-0.0078},
        {-0.0024,-0.0024,-0.0036,-0.0042,-0.006,-0.0042,-0.0054,-0.0078},
        {-0.0018,-0.0024,-0.0036,-0.0042,-0.0078,-0.0042,-0.0054,-0.0072},
        {-0.0024,-0.0024,-0.0036,-0.0042,-0.0006,-0.0042,-0.0054,-0.0078}};
        static double ff_u_rhad_18_chisq[10][8]=
        {{-0.00192,-0.00208,-0.0016,-0.0024,-0.00816,0.00336,0.00176,0.00272},
        {-0.00128,-0.00144,-0.00112,-0.00144,0.00128,0.0016,0.00144,0.00128},
        {-0.00128,-0.00144,-0.00112,-0.00144,0.00128,0.0016,0.00144,0.00128},
        {-0.0008,-0.00112,-0.0008,-0.00096,-0.00192,-0.00016,0.00032,0.00064},
        {-0.00064,-0.00064,-0.00048,-0.0008,-0.00032,0.00064,0.00096,0.00048},
        {-0.00064,-0.00064,-0.00064,-0.00064,-0.00128,0.000001,0.00016,0.00016},
        {-0.00048,-0.00064,-0.00048,-0.00064,-0.0024,0.00032,0.00032,0.00016},
        {-0.00048,-0.00048,-0.00048,-0.00048,-0.00112,0.000001,0.00016,0.00016},
        {-0.00048,-0.00032,-0.00032,-0.00048,-0.00096,0.000001,0.000001,0.00016},
        {-0.00048,-0.00032,-0.00032,-0.00032,-0.0008,-0.00032,0.00001,0.0}};
        static double ff_u_rphi_18_chisq[10][8]=
        {{-0.0022,-0.0033,-0.0022,-0.0066,-0.0077,-0.0022,-0.0011,-0.0022},
        {0.0033,0.0022,0.0033,0.000001,-0.0165,0.000001,0.0011,0.0011},
        {0.0033,0.0022,0.0033,0.000001,-0.0165,0.000001,0.0011,0.0011},
        {0.0044,0.0044,0.0044,0.0022,-0.0033,0.0011,0.0011,0.0011},
        {0.0044,0.0055,0.0033,0.0022,0.0033,0.0022,0.0022,0.0011},
        {0.0055,0.0044,0.0044,0.0033,0.0077,0.0022,0.0033,0.0033},
        {0.0055,0.0055,0.0044,0.0033,0.0088,0.0022,0.0033,0.0033},
        {0.0055,0.0055,0.0055,0.0033,0.000001,0.0033,0.0033,0.0033},
        {0.0055,0.0055,0.0055,0.0044,0.0011,0.0033,0.0033,0.0033},
        {0.0055,0.0055,0.0055,0.0044,0.0055,0.0033,0.0033,0.0033}};
        static double ff_u_w1_18_chisq[10][8]=
        {{0.02,0.024,0.02,0.018,-0.022,0.027,0.042,0.045},
        {0.02,0.022,0.022,0.021,0.049,0.027,0.04,0.043},
        {0.02,0.022,0.022,0.021,0.049,0.027,0.04,0.043},
        {0.009,0.019,0.017,0.019,0.018,0.026,0.04,0.042},
        {0.009,0.016,0.019,0.02,-0.01,0.022,0.039,0.042},
        {0.013,0.019,0.019,0.02,-0.014,0.024,0.039,0.043},
        {0.013,0.018,0.019,0.018,-0.026,0.026,0.039,0.043},
        {0.012,0.017,0.02,0.019,0.027,0.025,0.038,0.043},
        {0.011,0.018,0.019,0.02,-0.013,0.024,0.037,0.043},
        {0.012,0.017,0.02,0.021,-0.014,0.027,0.038,0.044}};
        static double ff_u_weta2_18_chisq[10][8]=
        {{0.000176,0.000208,0.000112,0.000128,0.000464,0.0004,0.000416,0.000384},
        {0.000192,0.00024,9.6e-05,0.000144,0.000256,0.0004,0.000432,0.0004},
        {0.000176,0.000208,8e-05,0.00016,-0.000176,0.000368,0.000432,0.000432},
        {0.00016,0.000192,9.6e-05,0.000144,0.000336,0.0004,0.000448,0.000464},
        {0.000128,0.000144,8e-05,0.000128,-0.000368,0.000384,0.000432,0.000496},
        {0.000144,0.000176,9.6e-05,0.000144,0.000304,0.0004,0.000432,0.000464},
        {0.000144,0.00016,8e-05,0.000112,0.000416,0.000384,0.000432,0.000464},
        {0.000128,0.00016,6.4e-05,0.000112,0.000208,0.000368,0.000416,0.000464},
        {0.000128,0.000176,8e-05,0.000112,0.000336,0.000368,0.000432,0.000464},
        {0.00016,0.000192,8e-05,0.00016,0.000112,0.000416,0.000448,0.000496}};
        static double ff_u_wtot_18_chisq[10][8]=
        {{0.1575,0.1725,0.15,0.15,-0.0675,0.165,0.1875,0.15},
        {0.15,0.195,0.15,0.1575,0.3375,0.165,0.18,0.15},
        {0.135,0.1725,0.1575,0.15,0.0675,0.135,0.1725,0.15},
        {0.105,0.135,0.1275,0.1425,0.2325,0.1575,0.18,0.1575},
        {0.105,0.1425,0.1425,0.135,0.075,0.1275,0.1875,0.1575},
        {0.0975,0.12,0.12,0.12,-0.0225,0.135,0.1725,0.15},
        {0.0825,0.105,0.1125,0.1125,-0.0075,0.135,0.1725,0.15},
        {0.0675,0.0975,0.105,0.1125,0.225,0.1275,0.1725,0.1425},
        {0.06,0.0825,0.0975,0.105,-0.0975,0.135,0.1725,0.1425},
        {0.0525,0.075,0.09,0.0975,-0.1425,0.12,0.165,0.1425}};


				static double ff_u_f1_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};


				static double ff_u_eratio_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_deltae_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_e277_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				// converted photons -- values

        static double ff_c_fside_18_chisq[10][8]=
        {{0.032,0.026,0.036,0.04,0.094,0.044,0.056,0.05},
        {0.026,0.038,0.036,0.034,0.094,0.046,0.052,0.042},
        {0.02,0.024,0.034,0.024,-0.036,0.044,0.046,0.036},
        {0.02,0.016,0.03,0.036,0.064,0.054,0.044,0.036},
        {0.02,0.028,0.036,0.042,0.038,0.042,0.052,0.042},
        {0.018,0.024,0.032,0.036,0.05,0.042,0.046,0.04},
        {0.018,0.024,0.03,0.038,0.034,0.042,0.046,0.038},
        {0.018,0.022,0.028,0.034,0.024,0.036,0.042,0.038},
        {0.016,0.022,0.028,0.036,0.024,0.038,0.042,0.036},
        {0.016,0.022,0.03,0.036,0.028,0.042,0.042,0.036}};
        static double ff_c_reta_18_chisq[10][8]=
        {{-0.0048,-0.0066,-0.0054,-0.0096,-0.0132,-0.009,-0.0084,-0.009},
        {-0.0048,-0.0072,-0.0066,-0.006,-0.0156,-0.0078,-0.0078,-0.0096},
        {-0.0036,-0.006,-0.0066,-0.006,-0.0048,-0.0066,-0.0084,-0.0078},
        {-0.003,-0.0096,-0.0078,-0.0078,-0.0174,-0.0102,-0.009,-0.0102},
        {-0.003,-0.003,-0.0036,-0.0042,-0.0066,-0.006,-0.006,-0.0084},
        {-0.0024,-0.003,-0.0036,-0.0042,-0.003,-0.0054,-0.0066,-0.0078},
        {-0.0024,-0.003,-0.0036,-0.0048,-0.0036,-0.0048,-0.006,-0.0078},
        {-0.0024,-0.0024,-0.0036,-0.0048,-0.0054,-0.0042,-0.006,-0.0078},
        {-0.0024,-0.0024,-0.0042,-0.0048,-0.0042,-0.0048,-0.006,-0.0078},
        {-0.0024,-0.0024,-0.0036,-0.0048,-0.0048,-0.0048,-0.006,-0.0078}};
        static double ff_c_rhad_18_chisq[10][8]=
        {{-0.0024,-0.00304,-0.0016,-0.00192,-0.00672,0.00128,0.00512,0.00208},
        {-0.00112,-0.00176,-0.0016,-0.00032,0.00144,0.00128,0.00128,0.00272},
        {-0.00112,-0.00176,-0.00112,-0.00112,0.00336,0.00016,0.00304,0.00192},
        {-0.00112,-0.00096,-0.00064,0.00016,0.00144,0.0016,0.0016,0.0008},
        {-0.00064,-0.0008,-0.00064,-0.00096,-0.00064,0.000001,0.000001,0.00016},
        {-0.00064,-0.00064,-0.00048,-0.0008,-0.00016,0.00016,0.00032,0.00016},
        {-0.00048,-0.00048,-0.00048,-0.00064,0.000001,0.00016,0.00016,0.00016},
        {-0.00048,-0.00048,-0.00032,-0.00048,-0.00016,0.00016,0.000001,0.00032},
        {-0.00032,-0.00032,-0.00048,-0.00048,-0.00048,0.00016,0.00016,0.00016},
        {-0.00032,-0.00032,-0.00032,-0.00032,-0.00032,0.000001,0.000001,0.0}};
        static double ff_c_rphi_18_chisq[10][8]=
        {{-0.0165,-0.0165,-0.0033,-0.0132,0.0286,-0.0077,-0.0044,-0.0088},
        {-0.0132,-0.0143,-0.0077,-0.011,-0.0264,-0.0055,-0.0022,0.0},
        {-0.0077,-0.0198,-0.0022,0.0088,-0.066,0.0066,0.000001,0.0033},
        {0.0066,-0.0066,0.0022,-0.0011,-0.0583,-0.0011,0.0099,0.0},
        {0.0055,0.0044,0.0011,-0.0033,0.0176,-0.0022,-0.0033,-0.0022},
        {0.0044,0.0044,0.0044,0.0022,0.0055,0.0022,0.0011,0.0011},
        {0.0044,0.0044,0.0033,0.0022,-0.0022,0.0011,0.0011,0.0022},
        {0.0055,0.0044,0.0044,0.0022,0.0022,0.0033,0.0033,0.0022},
        {0.0055,0.0044,0.0033,0.0022,0.000001,0.0022,0.0022,0.0022},
        {0.0044,0.0044,0.0033,0.0011,0.000001,0.0011,0.0022,0.0022}};
        static double ff_c_w1_18_chisq[10][8]=
        {{0.016,0.016,0.014,0.012,0.014,0.014,0.036,0.041},
        {0.013,0.024,0.016,0.013,0.009,0.015,0.032,0.037},
        {0.015,0.02,0.012,0.009,0.023,0.02,0.032,0.038},
        {0.016,0.011,0.013,0.012,0.013,0.022,0.028,0.041},
        {0.014,0.014,0.017,0.016,0.004,0.019,0.038,0.041},
        {0.012,0.017,0.016,0.016,0.013,0.02,0.035,0.039},
        {0.012,0.015,0.017,0.016,0.011,0.022,0.035,0.04},
        {0.011,0.014,0.018,0.017,0.013,0.02,0.034,0.039},
        {0.012,0.015,0.019,0.019,0.014,0.023,0.034,0.04},
        {0.013,0.016,0.022,0.021,0.013,0.026,0.036,0.042}};
        static double ff_c_weta2_18_chisq[10][8]=
        {{0.000144,0.000176,0.000144,0.00016,0.000576,0.000368,0.000432,0.000336},
        {0.000144,0.00024,6.4e-05,0.00016,-0.000112,0.000368,0.00048,0.000352},
        {0.000144,0.00016,8e-05,0.000112,0.000112,0.000336,0.000464,0.00032},
        {0.000256,0.000256,9.6e-05,0.000176,0.00096,0.000368,0.000224,0.000464},
        {0.000128,0.00016,8e-05,0.000144,0.000128,0.000352,0.000496,0.000368},
        {0.000128,0.000144,8e-05,0.000128,0.000192,0.000336,0.000464,0.0004},
        {0.000128,0.000144,6.4e-05,0.000112,0.000192,0.000336,0.000448,0.000416},
        {0.000112,0.000128,6.4e-05,0.000112,0.000224,0.00032,0.000416,0.000448},
        {0.000128,0.000144,6.4e-05,0.000112,0.000112,0.000336,0.000432,0.000464},
        {0.00016,0.000176,9.6e-05,0.000144,0.000208,0.0004,0.00048,0.00048}};
        static double ff_c_wtot_18_chisq[10][8]=
        {{0.1425,0.135,0.135,0.1575,0.075,0.135,0.195,0.1275},
        {0.1125,0.15,0.1425,0.135,0.0675,0.1425,0.195,0.135},
        {0.1125,0.1275,0.1275,0.1425,0.2025,0.1125,0.2025,0.135},
        {0.0975,0.1275,0.1425,0.1575,0.4425,0.1575,0.165,0.1425},
        {0.09,0.105,0.12,0.15,0.1725,0.135,0.195,0.1425},
        {0.09,0.0975,0.1125,0.1275,0.0825,0.1275,0.1875,0.1425},
        {0.0825,0.1125,0.1125,0.135,0.1125,0.135,0.18,0.135},
        {0.0825,0.105,0.1125,0.1275,0.0825,0.12,0.1725,0.135},
        {0.075,0.09,0.105,0.135,0.1125,0.1275,0.1725,0.135},
        {0.075,0.0975,0.12,0.1425,0.0975,0.15,0.1725,0.1425}};



				static double ff_c_f1_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};


				static double ff_c_eratio_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_c_deltae_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_c_e277_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				// unconverted photons -- errors

        static double ff_u_reta_err_18_chisq[10][8]= {{0.00207953726794,0.00222356799674,0.00151219993895,0.00358399152911,0.0078196436498,0.0034992455887,0.0018004727798,0.00302041064041},
        {0.00213640300531,0.00202419208162,0.00155074817876,0.00334135965156,0.00653431954255,0.00205095945911,0.000980705953338,0.0017670058781},
        {0.00147075500086,0.00261772672437,0.00180098226229,0.00247442699405,0.00488533576712,0.00266154734547,0.00154961983333,0.00142871540816},
        {0.00173247204676,0.00176766176293,0.00173245290227,0.00224526934689,0.00941636350924,0.00204966771779,0.000775040063737,0.00221816506944},
        {0.00202003788429,0.00252225673441,0.00202007078303,0.00154942709596,0.00262076884369,0.00186560228113,0.00154930415855,0.00154924347145},
        {0.00154937861453,0.00142892509596,0.00146993493923,0.00109584546321,0.00245718432797,0.00142850431049,0.00158758762273,0.00158753198948},
        {0.00109579451442,0.00109658868065,0.00109589212335,0.00109601174484,0.000647459004485,0.000775204769555,0.00109578456915,0.00103946839607},
        {0.00109567080811,0.00109619709716,0.0010957408791,0.000775147314045,0.00203088575485,0.00109576495938,0.000693253469163,0.000774881622803},
        {0.0010957507906,0.00109651164617,0.0010959108065,0.000693690061931,0.00394689255772,0.00109599362667,0.00069354388608,0.00109581620564},
        {0.000848751348269,0.00109607157054,0.0010956732196,0.000775040104323,0.00163816982125,0.000775179719541,0.000775108192226,0.000600663010654}};
        static double ff_u_rphi_err_18_chisq[10][8]= {{0.00201730268779,0.0020439610213,0.0023084086315,0.004634108621,0.00764675626286,0.00229435986566,0.000655699718025,0.001560812645},
        {0.00091710791862,0.00147318240058,0.000941201927798,0.00158614243549,0.0112580160413,0.00111190987146,0.000657541872971,0.00190843034477},
        {0.000911204722118,0.00145890590591,0.000668726786381,0.00157208172868,0.00770716742124,0.0014255992404,0.000646382236061,0.00063892351539},
        {0.00142264757253,0.00229686819466,0.00237894806394,0.00277138382345,0.00227938837881,0.00142217323454,0.00142195153621,0.00200858991243},
        {0.00089991730636,0.00237906831785,0.00142218911386,0.00142306465485,0.0035228164073,0.0020089560426,0.00142079520738,0.00142028449213},
        {0.00142139940067,0.000907265188777,0.000640589652547,0.00142358980509,0.00607353019989,0.000637314974229,0.00155633964665,0.00155583869261},
        {0.000638330341255,0.000650822416915,0.000902616716679,0.000904627655829,0.00662173596785,0.000637691878618,0.000899568466106,0.00142044628282},
        {0.000636602841836,0.000642161499199,0.00142148418191,0.000901464569733,0.00369581287731,0.000899181439078,0.000636334643306,0.000635643601809},
        {0.000636699346303,0.000641953964126,0.00142179144035,0.00142242645609,0.00365654538608,0.000637124002466,0.000636817300718,0.000635954493593},
        {0.000898618972028,0.000900147962655,0.000636354917409,0.00127130161844,0.00215709311943,0.000636292218051,0.000636256683849,3.09281e-05}};
        static double ff_u_weta2_err_18_chisq[10][8]= {{0.000168605145737,0.000143669536912,0.000159795459491,0.000164553687624,0.000130964755095,0.000172606905903,6.79655542409e-05,5.91567603209e-05},
        {3.93361776345e-05,5.35339530825e-05,8.77233836984e-05,8.42734957798e-05,0.000613977353006,8.91461396942e-05,3.82368074569e-05,4.97559995136e-05},
        {4.71738741685e-05,6.5517097285e-05,8.57130029076e-05,5.92348310255e-05,0.000804560719212,9.95283207718e-05,3.92920660474e-05,1.84997567337e-05},
        {0.000107737707648,0.000107761933694,0.00011276977174,0.000105744227362,0.00030930040764,8.7163059322e-05,0.000111632055796,1.30766957149e-05},
        {0.000108127467845,0.000127008165884,9.99253583252e-05,9.28448503599e-05,0.000324791703404,7.44846215302e-05,0.000112006493906,3.45676661684e-05},
        {6.26603461308e-05,5.91802750574e-05,5.06093927619e-05,3.45927050994e-05,0.000246913783976,4.23544347141e-05,7.94783630739e-05,4.13184332724e-05},
        {5.06108592174e-05,5.91976854569e-05,3.92176721817e-05,3.92319975412e-05,0.000390970358002,3.92307867632e-05,5.06327225118e-05,3.92047301913e-05},
        {5.91583797228e-05,3.92434650932e-05,4.80157204342e-05,3.81171097727e-05,0.000136969316426,5.06207161101e-05,6.26781476338e-05,3.92040045677e-05},
        {6.7262864604e-05,4.8067193653e-05,5.061987661e-05,4.71393384868e-05,0.000254177134014,5.06406265345e-05,4.9802119406e-05,5.06158498725e-05},
        {5.46607037661e-05,5.46904051136e-05,5.06111405337e-05,3.45958311835e-05,7.24942407673e-05,4.03104130567e-05,3.81489811112e-05,3.45924813287e-05}};
        static double ff_u_w1_err_18_chisq[10][8]= {{0.00909393714642,0.00661375475679,0.00209526467197,0.00245836596208,0.0128919710948,0.0034668848311,0.00209273891907,0.00602904238105},
        {0.00793931613749,0.0041715366711,0.00252585655275,0.00238852090536,0.0123451244229,0.00183215223821,0.00440295099037,0.0037439035498},
        {0.0058328419422,0.00295224028746,0.00294894194233,0.00208735078989,0.0384069129937,0.00294694252261,0.00316816828995,0.00369820726703},
        {0.0144340218342,0.00993398604734,0.0123562535104,0.00739428009141,0.00391538590158,0.00519683717529,0.00700107715381,0.00912886512309},
        {0.0133042797388,0.0117477417431,0.0101162672675,0.00635122877955,0.023170544793,0.0102471402917,0.00725767202107,0.00754995649406},
        {0.00707142358102,0.00559893204002,0.00707163848019,0.00374260500185,0.0256498674578,0.00535470709215,0.00479699080226,0.00369733252261},
        {0.0055983645104,0.00465767289024,0.00496800484975,0.00341743388891,0.0337396467585,0.00479695897168,0.00311226876384,0.00300118525453},
        {0.00489966731884,0.00443736087587,0.00568729567773,0.00387419183092,0.0181019129535,0.00496747207157,0.0029468917549,0.00245085300057},
        {0.0059170290177,0.00497079222541,0.00602920402103,0.00391767869476,0.0237137144478,0.0078750513948,0.00365549674749,0.0024520064552},
        {0.00571620713167,0.00678467184122,0.007394647859,0.00451038954789,0.0236288144911,0.0057166580971,0.00311301246475,0.00182911365843}};
        static double ff_u_wtot_err_18_chisq[10][8]= {{0.0756427878846,0.0704141779076,0.0962602446538,0.0957649803943,0.412004042284,0.0867203643351,0.0396992763221,0.0252524343475},
        {0.043109674579,0.0411493064981,0.045442608951,0.035994923218,0.405012872591,0.0397096547298,0.026709366702,0.00969003632204},
        {0.0394641640981,0.0327428258452,0.038017065082,0.0351955360661,0.114776047927,0.0521543524741,0.0267030884194,0.00968644319293},
        {0.0988386448462,0.0950702757033,0.113001145183,0.102196900176,0.0410963852899,0.0891657217671,0.0580964799669,0.038730082593},
        {0.0718080970125,0.0649551982832,0.0761183018931,0.0758716585708,0.134548466896,0.093876316075,0.0428670955146,0.0329774592631},
        {0.0367447417307,0.0318285549696,0.0450037994412,0.042651450591,0.155155912416,0.0408551836808,0.0329791743027,0.0193655115076},
        {0.0309276249546,0.0237360587551,0.0273964934376,0.0318306938018,0.13668653685,0.0351871643402,0.022504512697,0.0136947720134},
        {0.0266964395566,0.0237320004337,0.0277346532474,0.0263495501018,0.1357812413,0.0277354939288,0.0183755313403,0.0136946338582},
        {0.0266986063501,0.0263627211969,0.0306297824042,0.0267098490558,0.195168995669,0.0267094758613,0.0143707667375,0.0136961154009},
        {0.0263430922989,0.0306338006677,0.0357146196528,0.0306295728797,0.226339915636,0.0220949357753,0.0137008398205,0.00968681800847}};
        static double ff_u_fside_err_18_chisq[10][8]= {{0.00840857588424,0.00865325418397,0.00910026868431,0.0124443127289,0.0893644552173,0.0139552305127,0.00739668865253,0.00931005252466},
        {0.00529395181601,0.00433602069006,0.00731080702357,0.00703480049996,0.059561435069,0.0062252480198,0.00980057180049,0.0077467443997},
        {0.00432275107885,0.00477139008911,0.00663801678499,0.00600833435749,0.0443928947561,0.00931308562737,0.0110166890757,0.00476163569174},
        {0.00930989338777,0.0118897217335,0.0144691824574,0.0157491625099,0.129059944685,0.012910971844,0.00879463368339,0.0124900618825},
        {0.00774624658846,0.0129621402203,0.00979864288875,0.0080841353949,0.0591693903091,0.0184033017701,0.00774632941601,0.00930955406506},
        {0.00489962794862,0.00529392210003,0.00683278515479,0.00589055157476,0.0819659574492,0.00702528945928,0.00663385340926,0.0051641694085},
        {0.00346547110989,0.00541993463992,0.0052947054054,0.00529636324705,0.107743937954,0.00490266048823,0.00529276549613,0.00258275387788},
        {0.00383067116091,0.00432441618154,0.00622029085429,0.00529507082409,0.0307529628487,0.00589013598471,0.00383102218341,0.00258266015912},
        {0.00365310182405,0.00490470513976,0.0060034372277,0.00529676862334,0.0293174176308,0.00739698520787,0.00258533790232,0.00231081799586},
        {0.00346525638093,0.00432498232606,0.00529371023139,0.00490260533149,0.023778665297,0.00432471262203,0.00163709428449,0.00115749389098}};
        static double ff_u_rhad_err_18_chisq[10][8]= {{0.00122924901664,0.000211983576461,0.000230954914743,0.000282442259517,0.00258902338578,0.0059818311666,0.00588188248032,0.000616302807293},
        {0.00064029658736,3.61312e-05,9.81990630789e-05,0.000210100914462,0.01349138714,0.00404485943696,0.0021828674304,0.000757497242905},
        {0.0010573792364,0.000480573093599,0.000207630848931,0.000208109911836,0.0135913771884,0.00446301722191,0.00345918702896,0.00253838541461},
        {0.00045260272248,0.000206889622149,0.000160343395364,9.32778442609e-05,0.000445575215533,0.00652221199441,0.00713456540989,0.00464185344044},
        {0.000277162048393,0.000130832144812,6.4447e-06,9.27114565761e-05,0.000970324427444,0.00388092207531,0.00581457932094,0.00389845228874},
        {4.37921e-06,0.000160146546863,0.000160124159539,8.01248e-06,0.00124716659548,0.00317997203231,0.00345396031333,0.00256999536217},
        {9.25151980215e-05,0.000160196444283,9.26494337578e-05,0.000160250416987,0.00128807049884,0.00189547110829,0.00201760716819,0.00157042821125},
        {4.11003e-06,9.25848104676e-05,0.000160088672976,6.63973e-06,0.000680527410088,0.00136397385296,0.00139797673197,0.000946609483846},
        {4.88209e-06,0.000160149596691,5.9562e-06,0.000160154830167,0.000455990280458,0.00067896131423,0.000679004033449,0.00064670111605},
        {0.000160044636519,9.25062722852e-05,0.000160039326521,0.000160059669417,0.00122954157018,0.000678887204785,0.000423463977605,0.000530706125855}};


				static double ff_u_f1_err_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_eratio_err_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_deltae_err_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};

				static double ff_u_e277_err_18_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				// converted photons -- errors


        static double ff_c_reta_err_18_chisq[10][8]= {{0.000352810499769,0.00159507198462,0.000778940726474,0.000604148392976,0.00516786482119,0.000603920267916,0.00104004914827,0.00109675906046},
        {0.000603504599046,0.00254899069894,0.00104154688513,0.00147128662097,0.00409601414598,8.02166e-05,0.000491715534545,0.000776086189293},
        {0.00272820825316,0.00317686845865,0.00213624369142,0.00300046478881,0.0137646832712,0.00294003984193,0.000849306557056,0.00244974033675},
        {0.00103993239819,0.000696152359841,0.000775495363352,0.00176670157076,0.00162108042313,0.000348048492852,0.000347400489923,0.000693005249096},
        {0.00077495358183,0.00109625682862,0.0015876668317,0.00173225988977,0.0051525167959,0.000693160783189,0.000774818285615,0.000692932416508},
        {0.00109570711925,0.00142892509596,0.00109576855474,0.00158772707958,0.00208752360984,0.00114918430423,0.000848784106726,0.00109556278579},
        {0.00109579451442,0.00109658868065,0.00103970166203,0.00109601174484,0.00144886271347,0.00147001443351,0.000848966325589,0.00103946839607},
        {0.00142845879176,0.00176681863128,0.00142851253902,0.00142858438969,0.00280080291153,0.00176655055015,0.00103951929876,0.00138579996009},
        {0.00180018604458,0.00210768541063,0.00142864288603,0.00176669349408,0.00146217675476,0.00180033386618,0.00138600256924,0.0013859340376},
        {0.00247393994494,0.00281449336253,0.00244959176275,0.00244963000539,0.00233315245178,0.00244967418192,0.00176657655075,0.00210731963697}};
        static double ff_c_rphi_err_18_chisq[10][8]= {{0.00449475733132,0.00344109139517,0.00431726191121,0.00349260591028,0.0717027164618,0.0018020600785,0.00775392000562,0.00426217504483},
        {0.00500410700669,0.00547724989254,0.00463204717905,0.00582888049505,0.00854982993119,0.00348228998825,0.00128154645437,0.00142435004388},
        {0.00512252809124,0.00972068617823,0.00740926416825,0.00648059469712,0.0146795922851,0.00298144258051,0.00156028095603,0.00381115335892},
        {0.00237779858601,0.0032432910503,0.00237894806394,0.00381277260842,0.0140277918689,0.00155753331983,0.00155733089119,0.00127060357165},
        {0.00200910871407,0.00201162440025,0.0020097981347,0.00291177717987,0.0217211165023,0.00416484145929,0.00311159000426,0.00269453917617},
        {0.00200923938915,0.00391703077889,0.00496088249236,0.00775284515085,0.00636535694906,0.00602518356924,0.00523725689291,0.00391500966845},
        {0.00142154808498,0.00391751236778,0.00364939039712,0.00546427956785,0.00207863430195,0.00364874191999,0.00311168069675,0.00311143069595},
        {0.00291065797456,0.00347980431314,0.0034790732405,0.00406726423662,0.00374998215428,0.00347877285745,0.00311152509952,0.00210650803982},
        {0.00261891568992,0.00323966637563,0.00323905710043,0.00323933589227,0.00277914690036,0.00261901896286,0.00200886442412,0.00200859107783},
        {0.00210654125449,0.00323886806688,0.00291060375997,0.00347891857791,0.00301491581849,0.00291059005245,0.00200868677692,0.0020085541767}};
        static double ff_c_weta2_err_18_chisq[10][8]= {{4.1453128175e-05,0.000109256285113,4.82070763204e-05,5.00191574367e-05,0.000125983201567,5.31771626919e-05,4.72226982493e-05,3.20133663491e-05},
        {3.93361776345e-05,4.85168438033e-05,4.14976952873e-05,4.63970770373e-05,0.000333014799476,3.82321970208e-05,4.34594076255e-05,9.29118691366e-06},
        {5.39262558568e-05,7.46491127654e-05,5.9217555399e-05,8.3710404809e-05,0.000220884474101,0.000108942890096,2.78543794452e-05,5.22644652947e-05},
        {4.62105361272e-05,3.92848701176e-05,3.33419868179e-05,4.13502312019e-05,8.31869911715e-05,2.92585983437e-05,3.33523994306e-05,2.26347808507e-05},
        {4.13225035844e-05,4.13490129002e-05,2.9230302252e-05,4.13299678e-05,0.000117650544412,3.81045777368e-05,4.97606405091e-05,2.92162662056e-05},
        {2.92287354709e-05,5.46836808917e-05,2.92342943999e-05,4.13358832746e-05,0.000126136500336,5.06151967294e-05,7.94783630739e-05,2.77226669247e-05},
        {2.92368330032e-05,4.13807438725e-05,4.13363336306e-05,4.23692061653e-05,9.96217220427e-05,3.81276972388e-05,6.13052954942e-05,3.81008862895e-05},
        {5.84327011021e-05,5.84583859327e-05,4.80157204342e-05,4.71265748535e-05,0.000149481081218,5.6212011464e-05,7.56343188692e-05,3.8100139643e-05},
        {6.78942286801e-05,7.62175949e-05,5.9169574738e-05,6.72764240501e-05,6.98045995788e-05,8.5192760192e-05,8.95930676113e-05,5.6207629301e-05},
        {9.64492571401e-05,0.000105345655241,0.000105736248339,8.81336383111e-05,0.000102277799535,0.000105746218533,9.46819839946e-05,8.56775336041e-05}};
        static double ff_c_w1_err_18_chisq[10][8]= {{0.00245622192734,0.0067878630891,0.00347229041301,0.0027160442811,0.00886206815829,0.00443688596865,0.00217091904886,0.00238383836824},
        {0.00208951527022,0.0050068338164,0.00239024782364,0.00117119544995,0.0166453826536,0.00294790012676,0.00131122490707,0.00191923955172},
        {0.00252098759535,0.00370347441126,0.00216708527271,0.00130014101288,0.0160548320566,0.00191949739035,0.00183592582823,0.00182849765013},
        {0.00294521979562,0.00416542260225,0.00208414670285,0.00163565829874,0.00152651457845,0.000583480899264,0.00216373472965,0.00310958385784},
        {0.00264648056228,0.00387420134519,0.00182725570927,0.00182704871587,0.00626158760531,0.00282911366975,0.00300118918076,0.00316256906046},
        {0.00310993538981,0.00311149053062,0.00294529072607,0.00182768310526,0.00335395795071,0.00300103671688,0.00519722240789,0.00443511756133},
        {0.00365262716291,0.00370052925302,0.00294636592894,0.00173556553273,0.00183405653795,0.00316398725914,0.0029472388533,0.00369780740422},
        {0.00424343490999,0.00535632070947,0.00420460051125,0.00258380127128,0.00200314400702,0.00465572537656,0.00408462617813,0.00506688731837},
        {0.0059170290177,0.00768388630067,0.00648212679558,0.003698856538,0.00251798588348,0.00588923604022,0.00532566019108,0.00479711743199},
        {0.00912916702875,0.0123030526832,0.0126628386874,0.00707179471849,0.00360382309775,0.00920218342565,0.00605729697189,0.00522930748527}};
        static double ff_c_wtot_err_18_chisq[10][8]= {{0.0761369250689,0.058003503777,0.0569849515294,0.0399491110029,0.0797485476873,0.0486227476642,0.0248954321212,0.0136998335929},
        {0.0417844952382,0.0501823218402,0.0448194233371,0.0454354982087,0.0814197578262,0.0428877217716,0.0188981551909,0.0106135198649},
        {0.0450130008772,0.061721897608,0.0579465032375,0.0293892116087,0.156879065454,0.0700986910149,0.0183862701801,0.00968644319293},
        {0.0445850615681,0.044591000463,0.0408565638861,0.0470420705926,0.0341161674855,0.0372562201283,0.0193701054345,0.0193654149879},
        {0.0445830998984,0.0387353557363,0.0387330076691,0.0329811851559,0.019125635784,0.0491834598194,0.0193671855947,0.0178539300841},
        {0.0277295157703,0.0454208862909,0.0329824493351,0.0372544794289,0.0622864925788,0.0530367422981,0.0183746003409,0.0136939050659},
        {0.0273910201588,0.0267095579377,0.030932472463,0.0273987785842,0.0170135051126,0.0399297700257,0.0178592578718,0.0136947720134},
        {0.0266964395566,0.0309347352434,0.0394551769829,0.0399286712848,0.0373201753628,0.0486107768245,0.0173251306499,0.0136946338582},
        {0.0351826033863,0.0484251284862,0.0521481885603,0.0486149774924,0.0282344978764,0.0566316704749,0.0173282698682,0.0178559115443},
        {0.0526802478342,0.0832972373092,0.0918587723462,0.0834081574835,0.0709207826404,0.0883214933463,0.0220842254061,0.0225018764357}};
        static double ff_c_fside_err_18_chisq[10][8]= {{0.00589102269568,0.00434497502552,0.00841515835422,0.00463978296501,0.0339117362617,0.0058947823253,0.00347049511682,0.00383106747683},
        {0.00346784166741,0.00433602069006,0.00348059851212,0.00684264213645,0.0187855764036,0.0051723991443,0.00283746499832,0.00283055626941},
        {0.00416567444195,0.00433199300351,0.00622333781057,0.00347851717711,0.0718706878463,0.0101357567011,0.00258858484935,0.00346504080699},
        {0.00432135567753,0.00416719124821,0.00283265028792,0.00529491451892,0.0296277345627,0.0120565277183,0.0168526629437,0.044121064272},
        {0.00383027451706,0.00365382161925,0.00346603555397,0.00516590538207,0.0310647187265,0.011944936837,0.00346491261382,0.00382985598821},
        {0.00258321905798,0.00632657973958,0.00476308229738,0.00739585004275,0.0421396667512,0.00993418468321,0.00711861487385,0.00365175469417},
        {0.00258382597713,0.00476539870676,0.00589071914087,0.00490422915908,0.027894255666,0.00824839862415,0.00346603038028,0.00365202467959},
        {0.00365249342363,0.00702618260351,0.00702557364054,0.00702645773954,0.0197756935987,0.0106470826325,0.00490000656153,0.00231015154572},
        {0.00476219343058,0.00702776392897,0.00739648059644,0.00783511909179,0.0196852984874,0.0104585877073,0.00476276936973,0.00346504638882},
        {0.007394232107,0.0118337992823,0.012910849495,0.0118898642705,0.0229221491861,0.0122761206927,0.00365331963602,0.00461950128344}};
        static double ff_c_rhad_err_18_chisq[10][8]= {{0.000293234510671,0.000532794866738,0.000586066127649,0.000833170828799,0.000792028256303,0.00134577267981,0.00139876428046,0.00103490538228},
        {0.000132084769945,0.00022914070702,0.000334728331621,0.000347767730328,0.00146403334202,0.000680652528653,0.000897465070086,0.000872659960318},
        {0.00102041699788,0.000562391173138,2.1069e-05,0.000381719445934,0.00169207974942,0.00553270181645,0.00572303895682,0.00386004324411},
        {0.00029220293929,0.000160426460056,9.29695529204e-05,0.000131278671395,0.000556300823324,0.00468399216838,0.00536742243429,0.00293432616272},
        {0.000130711390996,0.000130832144812,9.26005804054e-05,9.27114565761e-05,0.000508785640353,0.00300474449628,0.00421808795698,0.00151510073841},
        {9.24797859727e-05,0.000130818894426,6.30448e-06,0.000130884936117,0.000349033690202,0.00152916822474,0.00154858274326,0.00133549347743},
        {5.07233e-06,0.000130879973363,9.26494337578e-05,9.28091023428e-05,0.000474895367405,0.00064684675338,0.0014953501324,0.0009863627291},
        {0.000130704089504,9.25848104676e-05,5.32759e-06,9.26143582162e-05,0.000139227233498,0.000756234975542,0.00123599040306,0.000831426193301},
        {9.25049627647e-05,9.26349105552e-05,0.000130775161957,0.000160154830167,0.000384959481686,0.000856770175069,0.000697600514221,0.00064670111605},
        {0.000306399987508,0.000206617384906,0.000226301979738,0.000206605334977,0.000308608391763,0.00112763225543,0.000750547626956,0.000783867968488}};
        
				static double ff_c_f1_err_18_chisq[10][8]=
				{{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00},
				{ 0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00,  0.000000e+00}};


				static double ff_c_eratio_err_18_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				static double ff_c_deltae_err_18_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				static double ff_c_e277_err_18_chisq[10][8]=
				{{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00},
				{0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00, 0.000000e+00}};

				if (isConv) {

				m_rhad_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_rhad_18_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_rhad_18_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_reta_18_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_rphi_18_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_c_weta2_18_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,    &ff_c_f1_18_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq,&ff_c_deltae_18_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_wtot_18_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_c_fside_18_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,    &ff_c_w1_18_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq,&ff_c_eratio_18_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_e277_18_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,   &ff_c_rhad_err_18_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq,   &ff_c_rhad_err_18_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,   &ff_c_reta_err_18_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,   &ff_c_rphi_err_18_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_weta2_err_18_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,     &ff_c_f1_err_18_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_c_deltae_err_18_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,   &ff_c_wtot_err_18_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_c_fside_err_18_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,     &ff_c_w1_err_18_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_c_eratio_err_18_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,   &ff_c_e277_err_18_chisq[0][0], 0.);

				} else {

				m_rhad_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_rhad_18_chisq[0][0], 0.);
				m_rhad1_ff.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_rhad_18_chisq[0][0], 0.);
				m_reta_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_reta_18_chisq[0][0], 0.);
				m_rphi_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_rphi_18_chisq[0][0], 0.);
				m_weta2_ff.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_u_weta2_18_chisq[0][0], 0.);
				m_f1_ff.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,    &ff_u_f1_18_chisq[0][0], 0.);
				m_deltae_ff.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq,&ff_u_deltae_18_chisq[0][0], 0.);
				m_wtot_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_wtot_18_chisq[0][0], 0.);
				m_fside_ff.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_u_fside_18_chisq[0][0], 0.);
				m_w1_ff.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,    &ff_u_w1_18_chisq[0][0], 0.);
				m_eratio_ff.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq,&ff_u_eratio_18_chisq[0][0], 0.);
				m_e277_ff.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_e277_18_chisq[0][0], 0.);

				m_rhad_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_rhad_err_18_chisq[0][0], 0.);
				m_rhad1_fferr.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_rhad_err_18_chisq[0][0], 0.);
				m_reta_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_reta_err_18_chisq[0][0], 0.);
				m_rphi_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_rphi_err_18_chisq[0][0], 0.);
				m_weta2_fferr.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_u_weta2_err_18_chisq[0][0], 0.);
				m_f1_fferr.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,    &ff_u_f1_err_18_chisq[0][0], 0.);
				m_deltae_fferr.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq,&ff_u_deltae_err_18_chisq[0][0], 0.);
				m_wtot_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_wtot_err_18_chisq[0][0], 0.);
				m_fside_fferr.Initialize(10, 8, pt_DVs_18_chisq, eta_DVs_18_chisq, &ff_u_fside_err_18_chisq[0][0], 0.);
				m_w1_fferr.Initialize(10, 8,    pt_DVs_18_chisq, eta_DVs_18_chisq,    &ff_u_w1_err_18_chisq[0][0], 0.);
				m_eratio_fferr.Initialize(10, 8,pt_DVs_18_chisq, eta_DVs_18_chisq,&ff_u_eratio_err_18_chisq[0][0], 0.);
				m_e277_fferr.Initialize(10, 8,  pt_DVs_18_chisq, eta_DVs_18_chisq,  &ff_u_e277_err_18_chisq[0][0], 0.);

				}

				break;

		default:
				this->LoadFFsDummy();

	}

}

void FudgeMCTool::LoadFFsDummy()
{
	static double pt_DVs_0[13] = {25000,30000,35000,40000,45000,55000,70000,
		85000,100000,125000,150000,200000,400000};// in MeV
	static double eta_DVs_0[5] = { 0.6, 1.37, 1.52, 1.81, 2.37}; // eta binning according to inclusive analysis

	static double ff_dummy[13][5]  = { {  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} ,
		{  0.0 ,  0.0 ,   0.0 ,  0.0 , 0.0} };

	m_rhad1_ff.Initialize  ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_rhad_ff.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_e277_ff.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_reta_ff.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_rphi_ff.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_weta2_ff.Initialize  ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_f1_ff.Initialize     ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_fside_ff.Initialize  ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_wtot_ff.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_w1_ff.Initialize     ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_deltae_ff.Initialize ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_eratio_ff.Initialize ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );

	m_rhad1_fferr.Initialize  ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_rhad_fferr.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_e277_fferr.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_reta_fferr.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_rphi_fferr.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_weta2_fferr.Initialize  ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_f1_fferr.Initialize     ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_fside_fferr.Initialize  ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_wtot_fferr.Initialize   ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_w1_fferr.Initialize     ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_deltae_fferr.Initialize ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );
	m_eratio_fferr.Initialize ( 13, 5, pt_DVs_0, eta_DVs_0, &ff_dummy[0][0], 0. );


}

	//Get graph of FFs
TGraphErrors* FudgeMCTool::GetFFmap(int var, double eta, int isConv, int preselection ){
	if((preselection>=0 && preselection != m_preselection) || (isConv>=0 && isConv!=m_conv))
		this->LoadFFs(isConv,preselection);

	const int ptbins = m_rhad1_ff.GetPtBins()+1;
	double *ptarray = m_rhad1_ff.GetPtArray();

	// Strict C++ compiler in Athena does not allow variable widht arrays.
        // reserve enough space, use only first fraction to create TGraphErrors
	//double x[ptbins]; double xerr[ptbins];
	//double y[ptbins]; double yerr[ptbins];
	double x[100];	double xerr[100];
	double y[100];	double yerr[100];

	for (int i=0; i<ptbins-1; i++){

		if(i==0){
			x[0]=ptarray[0]*0.5;
			xerr[0]=ptarray[0]*0.5;
		}
		else{
			x[i] =	(ptarray[i]+ptarray[i-1])*0.5;
			xerr[i] =		(ptarray[i]-ptarray[i-1])*0.5;
		}

		switch (var) {
			case IDVAR::RHAD1:
				y[i]    = GetFF_Rhad1( x[i], eta );
				yerr[i] = GetFFerr_Rhad1( x[i], eta );
				break;
			case IDVAR::RHAD:
				y[i]    = GetFF_Rhad( x[i], eta );
				yerr[i] = GetFFerr_Rhad( x[i], eta );
				break;
			case IDVAR::E277:
				y[i]    = GetFF_E277( x[i], eta );
				yerr[i] = GetFFerr_E277( x[i], eta );
				break;
			case IDVAR::RETA:
				y[i]    = GetFF_Reta( x[i], eta );
				yerr[i] = GetFFerr_Reta( x[i], eta );
				break;
			case IDVAR::RPHI:
				y[i]    = GetFF_Rphi( x[i], eta );
				yerr[i] = GetFFerr_Rphi( x[i], eta );
				break;
			case IDVAR::WETA2:
				y[i]    = GetFF_Weta2( x[i], eta );
				yerr[i] = GetFFerr_Weta2( x[i], eta );
				break;
			case IDVAR::F1:
				y[i]    = GetFF_F1( x[i], eta );
				yerr[i] = GetFFerr_F1( x[i], eta );
				break;
			case IDVAR::FSIDE:
				y[i]    = GetFF_Fside( x[i], eta );
				yerr[i] = GetFFerr_Fside( x[i], eta );
				break;
			case IDVAR::WTOT:
				y[i]    = GetFF_Wtot( x[i], eta );
				yerr[i] = GetFFerr_Wtot( x[i], eta );
				break;
			case IDVAR::W1:
				y[i]    = GetFF_W1( x[i], eta );
				yerr[i] = GetFFerr_W1( x[i], eta );
				break;
			case IDVAR::DE:
				y[i]    = GetFF_DE( x[i], eta );
				yerr[i] = GetFFerr_DE( x[i], eta );
				break;
			case IDVAR::ERATIO:
				y[i]    = GetFF_Eratio( x[i], eta );
				yerr[i] = GetFFerr_Eratio( x[i], eta );
				break;
			default:
				break;
		}
	}

		//rescale to GeV
	for (int i=0; i<ptbins-1; i++){
		x[i] = x[i]/GeV;
		xerr[i] = xerr[i]/GeV;
	}

	TGraphErrors* graph = new TGraphErrors(ptbins-1,x,y,xerr,yerr);

	return graph;
}

TGraphErrors* FudgeMCTool::GetFFmap_Rhad1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RHAD1 , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Rhad(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RHAD , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_E277(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::E277 , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Reta(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RETA , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Rphi(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RPHI , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Weta2(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::WETA2 , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_F1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::F1 , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Fside(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::FSIDE , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Wtot(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::WTOT , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_W1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::W1 , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_DE(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::DE , eta, isConv, preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Eratio(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::ERATIO , eta, isConv, preselection);
}



TGraphErrors* FudgeMCTool::GetFFmap_Rhad1(){
	return  this->GetFFmap( IDVAR::RHAD1 , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Rhad(){
	return  this->GetFFmap( IDVAR::RHAD , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_E277(){
	return  this->GetFFmap( IDVAR::E277 , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Reta(){
	return  this->GetFFmap( IDVAR::RETA , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Rphi(){
	return  this->GetFFmap( IDVAR::RPHI , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Weta2(){
	return  this->GetFFmap( IDVAR::WETA2 , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_F1(){
	return  this->GetFFmap( IDVAR::F1 , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Fside(){
	return  this->GetFFmap( IDVAR::FSIDE , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Wtot(){
	return  this->GetFFmap( IDVAR::WTOT , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_W1(){
	return  this->GetFFmap( IDVAR::W1 , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_DE(){
	return  this->GetFFmap( IDVAR::DE , m_eta2, m_conv, m_preselection);
}
TGraphErrors* FudgeMCTool::GetFFmap_Eratio(){
	return  this->GetFFmap( IDVAR::ERATIO , m_eta2, m_conv, m_preselection);
}

