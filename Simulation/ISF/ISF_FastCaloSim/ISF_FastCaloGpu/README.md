#FastCaloGpu

This package contains the cuda kernels for the fast calorimeter simulation. Please be noted the cuda (>=11.3) is needed for compiling. 

This is the discription of kernels :

1: simulate_clean() 
This kernel reset allocated work space GPU memory to 0.
Which include the array of energy deposit (size of ncells) to 0.
Also set the memeory which hold count of number of cells got hit to 0. 

2. simulate_A()
This is main kernel nhits threads. Each thread does one hit simulation. 
Each thread going through chain ( as device function similar as host function
)
CenterPositionCalculation_d() 
   Set the hit center position from extrapolation info (all hits are same)
HistoLateralShapeParametrization_d()
   Use 2 random numbers and paramtrization table find eta/phi
   Reweight for pion
HitCellMappingWiggle_d()              
   This function use 1 random number and parametrization table futher wiggle phi
   This last fuction alao  calls 
HitCellMapping() 
   Which identify the cells got hit (from E, eta and phi and geormetry info ), 
   And write energy to that cell (atomicly).


3. simulate_ct()
This kernel count the hit cells(using atomic)  and move hit cells together as a array of (cell, energy) pairs.



Geometry and all the parameterization files are loaded to GPU before loop on events in FastCaloSimV2ParamSvc::Initialization

The function Simulate_hits()
generate random number
call 3 kernels
copy result array of (cell, energy) pairs back to host  

For more information, please refer to https://doi.org/10.3389/fdata.2021.665783
