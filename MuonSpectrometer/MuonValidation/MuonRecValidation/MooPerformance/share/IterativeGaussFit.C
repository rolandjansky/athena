
    
//-----------------------------------------------------------------------------
int IterativeGaussFit(TH1* hist, double &mu, double &mu_err, double &sigma, double &sigma_err, int mode)
{
    //  mode 1:  symmetric fit to 1.5 sigma, display to 3.0 sigma  (e.g. resolution summed over regions with varying widths)
    double fit_range_lower	= -1.5;
    double fit_range_upper	= +1.5;
    double display_range_lower	= -3.0;
    double display_range_upper	= +3.0;
    //  colour code fits according to mode
    Color_t lineColour		= kRed;	// 2;	// red
    
    //  mode 2:  symmetric fit to 2.5 sigma, display to 3.0 sigma
    if (mode == 2)
    {
	fit_range_lower		= -2.5;
	fit_range_upper		= +2.5;
	display_range_lower	= -3.0;
	display_range_upper	= +3.0;
	lineColour		= 4;	// blue
    }
       
    //  mode 3:  asymmetric fit between -1.0 and +2.5 sigma, display to 3.0 sigma (e.g. pt pull with Landau tail)
    if (mode == 3)
    {
	fit_range_lower		= -1.0;
	fit_range_upper		= +2.5;
	display_range_lower	= -3.0;
	display_range_upper	= +3.0;
	lineColour		= 6;	// magenta
    }
    
    //constants for fitting algorithm
    const int iteration_limit = 20;
    const float percent_limit = 0.05;		// was 0.01
    // const float fit_range_multiplier = 2.5;	// was 1.5
    const int fDebug = 1;
  
    double last_mu;
    double last_sigma;
    double current_mu;
    double current_sigma;
    double mu_percent_diff;
    double sigma_percent_diff;
  
    if (! hist)
    {
	if (fDebug) std::cout<< "Error in  Anp::IterativeGaussFit(): Histogram to be fit is missing" <<std::endl;
	return 5;
    }
    if (hist->GetEntries() < 100)
    {
	if (fDebug) std::cout<< "Error in  Anp::IterativeGaussFit(): histogram statistics too low" <<std::endl;
	return 6;
    }  
  
    TF1* fit_func = new TF1("fit_func","gaus");

    int bad_fit = hist->Fit(fit_func,"QN");

    if (fDebug && bad_fit) std::cout <<"BAD INITIAL FIT: "<< hist->GetTitle() << std::endl;

    last_mu	= fit_func->GetParameter(1);
    last_sigma	= fit_func->GetParameter(2);

    if (mode == 5)
    {
	std::cout <<"      mode 5:   iteration "<< 0 << "   mu " << last_mu << "   sigma " << last_sigma << std::endl;
	fit_func->SetRange(0.,2.5);
    }
    else
    {
	// fit_func->SetRange(last_mu-fit_range_multiplier*last_sigma,last_mu+fit_range_multiplier*last_sigma);
	fit_func->SetRange(last_mu+fit_range_lower*last_sigma, last_mu+fit_range_upper*last_sigma);
    }

    int iteration = 0;

    while ( iteration < iteration_limit )
    {
	++iteration;
	// if ( fDebug ) std::cout<< " iteration " << iteration << std::endl;
	
	bad_fit = hist->Fit(fit_func, "RQN");

	if (fDebug && bad_fit) std::cout<<"BAD FIT: bin "<< hist->GetTitle() <<" iteration "<<iteration<<std::endl;

	current_mu	= fit_func->GetParameter(1);
	current_sigma	= fit_func->GetParameter(2);

	if (mode == 5)
	{
	    if (current_mu >  1.)	current_mu =  1.0;
	    if (current_mu < -1.)	current_mu = -1.0;
	    std::cout <<"      mode 3:   iteration "<< iteration << "   mu " << current_mu << "   sigma " << current_sigma << std::endl;
	    fit_func->SetRange(current_mu, current_mu+2.5*current_sigma);
	}
	else
	{
	    // fit_func->SetRange(current_mu-fit_range_multiplier*current_sigma,current_mu+fit_range_multiplier*current_sigma);
	    fit_func->SetRange(current_mu+fit_range_lower*current_sigma, current_mu+fit_range_upper*current_sigma);
	}
	
	//std::cout<<"Iteration: "<<iteration<<" Current: "<<current_mu<<" "<<current_sigma<<" Last: "<<last_mu<<" "<<last_sigma<<std::endl;

	float average_mu	= 0.5*(last_mu+current_mu);
	float average_sigma	= 0.5*(last_sigma+current_sigma);

	if (average_mu == 0)
	{
	    if ( fDebug ) std::cout<<" Average mu = 0 in bin "<< hist->GetTitle() <<std::endl;
	    average_mu = current_mu;
	} 

	if (average_sigma == 0)
	{
	    if ( fDebug ) std::cout<<"Average sigma = 0; Fit Problem in "<< hist->GetTitle() <<". "<<last_sigma<<" "<<current_sigma<<std::endl;
	    average_sigma = current_sigma;
	}

	mu_percent_diff		= fabs(last_mu-current_mu)/average_sigma;
	sigma_percent_diff	= fabs(last_sigma-current_sigma)/average_sigma;
	if (iteration > 15)
	{
	    std::cout <<"   iteration "<<iteration << "   mu " << current_mu << "   sigma " << current_sigma
		      << " average_sigma " << average_sigma << "   mu_percent_diff " << mu_percent_diff
		      << "   sigma_percent_diff " << sigma_percent_diff << std::endl;
	}

	if ( mu_percent_diff < percent_limit && sigma_percent_diff < percent_limit ) break;

	if (iteration != iteration_limit)
	{
	    // dampen large changes
	    if (mu_percent_diff < 1.)
	    {
		last_mu		= current_mu;
		last_sigma	= current_sigma;
	    }
	    else
	    {
		last_mu		= average_mu;
		last_sigma	= average_sigma;
	    }	
	}    
    }

    if (iteration == iteration_limit)
    {
	if (fDebug ) std::cout<<"WARNING: Fit did not converge to < "<<percent_limit*100<<"% in "<< hist->GetTitle() <<" in "<<iteration_limit<<" iterations. Percent Diffs: "<<mu_percent_diff*100<<"% (Mu),"<<" "<<sigma_percent_diff*100<<"% (Sigma)"<<std::endl;
	// return value 1 indicates not converged
	delete fit_func;
	return 1;
    }

    mu		= current_mu;
    mu_err	= fit_func->GetParError(1);
    sigma	= current_sigma;
    sigma_err	= fit_func->GetParError(2);
    fit_func->SetRange(mu+display_range_lower*current_sigma, mu+display_range_upper*current_sigma);
    fit_func->SetLineColor(lineColour);
    fit_func->SetLineWidth(1);
    hist->GetListOfFunctions()->Add(fit_func);
    TF1* fit_func_core = new TF1("fit_func_tail","gaus",mu,sigma);
    Double_t par[3];
    fit_func->GetParameters(&par[0]);
    fit_func_core->SetParameters(par);
    fit_func_core->SetLineColor(lineColour);
    fit_func_core->SetLineWidth(3);
    fit_func_core->SetRange(mu+fit_range_lower*sigma,mu+fit_range_upper*sigma);
    hist->GetListOfFunctions()->Add(fit_func_core);
    return 0;
}
